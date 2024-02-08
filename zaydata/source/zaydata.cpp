#include <boss.hpp>
#include "zaydata.hpp"

#include <resource.hpp>
#include <service/boss_zaywidget.hpp>

extern chars gAppName;
extern sint32 gStartPort;
extern bool gStartSSL;

#define PACKET_TEXT(NAME) \
    json(NAME).GetText(); \
    do { \
        if(!json(NAME).HasValue()) \
        { \
            SendError(peerid, json, "Packet lacks content(" NAME ")"); \
            return; \
        } \
    } while(false)
#define PACKET_INT(NAME) \
    json(NAME).GetInt(); \
    do { \
        if(!json(NAME).HasValue()) \
        { \
            SendError(peerid, json, "Packet lacks content(" NAME ")"); \
            return; \
        } \
    } while(false)

ZAY_DECLARE_VIEW_CLASS("zaydataView", zaydataData)

ZAY_VIEW_API OnCommand(CommandType type, id_share in, id_cloned_share* out)
{
    if(type == CT_Tick)
    {
        const uint64 CurMsec = Platform::Utility::CurrentTimeMsec();

        // 토큰 유효성확인
        static sint32 iToken = 0;
        if(iToken <= 0) iToken = m->mTokens.Count();
        else iToken = Math::Min(iToken, m->mTokens.Count());
        while(0 < iToken--)
        {
            chararray GetTokenCode;
            if(auto CurToken = m->mTokens.AccessByOrder(iToken, &GetTokenCode))
            if(CurToken->HasExpiry(CurMsec))
            {
                m->mTokens.Remove(&GetTokenCode[0]);
                m->invalidate();
            }
            // 최대 2ms동안 실시
            if(CurMsec + 2 < Platform::Utility::CurrentTimeMsec())
                break;
        }
    }
}

ZAY_VIEW_API OnNotify(NotifyType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == NT_SocketReceive)
    {
        if(!String::Compare(topic, "error"))
            Platform::Popup::MessageDialog("SocketError", String(in));
        else if(m->OnPacketOnce())
            m->invalidate();
    }
}

ZAY_VIEW_API OnGesture(GestureType type, sint32 x, sint32 y)
{
}

ZAY_VIEW_API OnRender(ZayPanel& panel)
{
    ZAY_RGB(panel, 192, 192, 192)
        panel.fill();

    if(m->mServer == nullptr)
    {
        ZAY_XYWH(panel, (panel.w() - 200) / 2, (panel.h() - 120) / 2, 200, 120)
        {
            ZAY_LTRB(panel, 0, 0, panel.w(), 30)
            ZAY_RGB(panel, 0, 0, 0)
                panel.text("Port Number", UIFA_LeftBottom, UIFE_Right);

            ZAY_LTRB(panel, 0, 30, panel.w(), panel.h() - 40)
            {
                ZAY_RGB(panel, 255, 255, 255)
                    panel.fill();
                ZAY_RGB(panel, 0, 0, 128)
                ZAY_FONT(panel, 2.0)
                if(ZayControl::RenderEditBox(panel, "ui_port", "port", 10, true, false))
                    m->invalidate(2);
            }

            // WSS여부
            ZAY_LTRB_UI(panel, 0, panel.h() - 40 + 5, 80, panel.h() - 5, "wss",
                ZAY_GESTURE_T(t)
                {
                    if(t == GT_InReleased)
                        m->mUseWSS ^= true;
                })
            {
                const bool Focused = ((panel.state("wss") & PS_Focused) != 0);
                const bool Pressed = ((panel.state("wss") & (PS_Pressed | PS_Dragging)) != 0);
                ZAY_MOVE_IF(panel, 1, 1, Pressed)
                {
                    if(Focused)
                    ZAY_RGBA(panel, 0, 0, 0, 16)
                        panel.fill();
                    ZAY_RGB(panel, 0, 0, 0)
                        panel.text((m->mUseWSS)? " [v] wss" : " [ ] wss", UIFA_LeftMiddle, UIFE_Right);
                }
            }

            // 스타트버튼
            ZAY_LTRB_UI(panel, panel.w() - 80, panel.h() - 40 + 5, panel.w(), panel.h() - 5, "start",
                ZAY_GESTURE_T(t)
                {
                    if(t == GT_InReleased)
                        m->ServerStart();
                })
            {
                const bool Focused = ((panel.state("start") & PS_Focused) != 0);
                const bool Pressed = ((panel.state("start") & (PS_Pressed | PS_Dragging)) != 0);
                ZAY_MOVE_IF(panel, 1, 1, Pressed)
                {
                    ZAY_RGB(panel, 160, 160, 160)
                    ZAY_RGB_IF(panel, 140, 140, 140, Focused)
                        panel.fill();
                    ZAY_RGB(panel, 0, 0, 0)
                        panel.rect(1);
                    ZAY_RGB(panel, 0, 0, 0)
                        panel.text("Start", UIFA_CenterMiddle, UIFE_Right);
                }
            }
        }
    }
    else
    {
        Mutex::LocalLock(m->mPacketMutex);
        {
            const sint32 Gap = 10;
            sint32 XPos = Gap, YPos = Gap, XSize = 0;
            ZAY_RGB(panel, 64, 64, 255)
            {
                ZAY_XYWH(panel, XPos, YPos, panel.w(), 30)
                    XPos += m->RenderValue(panel, "PEER", m->mPeerTokens.Count()) + Gap;
                ZAY_XYWH(panel, XPos, YPos, panel.w(), 30)
                    XPos += m->RenderValue(panel, "TOKEN", m->mTokens.Count()) + Gap;
            }

            for(sint32 i = 0, iend = m->mPrograms.Count(); i < iend; ++i)
            {
                if(panel.w() < XPos)
                {
                    XPos = Gap;
                    YPos += 30 + Gap;
                }
                chararray GetProgramID;
                if(auto CurProgram = m->mPrograms.AccessByOrder(i, &GetProgramID))
                    ZAY_XYWH(panel, XPos, YPos, panel.w(), 30)
                        XPos += m->RenderValue(panel, &GetProgramID[0], CurProgram->mProfiles.Count()) + Gap;
            }
        }
        Mutex::LocalUnlock(m->mPacketMutex);
    }
}

zaydataData::zaydataData()
{
    mPort = (*gAppName)? gStartPort : 8000;
    mUseWSS = gStartSSL;
    mServer = nullptr;
    mPacketMutex = Mutex::Open();
    ZayWidgetDOM::SetComment("port", String::FromInteger(mPort));

    if(*gAppName)
        ServerStart();
}

zaydataData::~zaydataData()
{
    Platform::Server::Release(mServer);
    Mutex::Close(mPacketMutex);
}

sint32 zaydataData::RenderValue(ZayPanel& panel, chars name, sint32 value)
{
    const String ValueText = String::FromInteger(value);
    const sint32 NameWidth = Platform::Graphics::GetStringWidth(name) + 10;
    const sint32 ValueWidth = Platform::Graphics::GetStringWidth(ValueText) + 10;
    ZAY_XYWH(panel, 0, 0, NameWidth + ValueWidth, panel.h())
    {
        ZAY_XYWH(panel, 0, 0, NameWidth, panel.h())
        {
            ZAY_RGB(panel, 224, 224, 224)
                panel.fill();
            ZAY_RGB(panel, 0, 0, 0)
                panel.text(name);
        }
        ZAY_XYWH(panel, NameWidth, 0, ValueWidth, panel.h())
        {
            ZAY_RGB(panel, 255, 255, 255)
                panel.fill();
            ZAY_RGB(panel, 0, 0, 0)
                panel.text(ValueText);
        }
        ZAY_RGB(panel, 0, 0, 0)
            panel.rect(1);
    }
    return NameWidth + ValueWidth;
}

ZDToken* zaydataData::ValidToken(sint32 peerid, chars token)
{
    // 동일한 피어가 통신중 토큰을 바꾸면
    if(!!mPeerTokens[peerid].Compare(token))
    {
        // 이전 토큰이 있을 경우 제거
        if(0 < mPeerTokens[peerid].Length())
            mTokens.Remove(mPeerTokens[peerid]);
        mPeerTokens[peerid] = token;
    }
    // 유효기간갱신
    if(auto OneToken = mTokens.Access(token))
    {
        OneToken->UpdateExpiry();
        return OneToken;
    }
    return nullptr;
}

void zaydataData::ServerStart()
{
    mPort = Parser::GetInt(ZayWidgetDOM::GetComment("port"));
    mServer = Platform::Server::CreateWS("zaydata", mUseWSS);
    if(!Platform::Server::Listen(mServer, mPort))
    {
        Platform::Server::Release(mServer);
        mServer = nullptr;
    }
    else
    {
        if(*gAppName)
        {
            const WString AppNameW = WString::FromCp949(false, gAppName);
            Platform::SetWindowName(String::Format("[%s] %s://host:%d/",
                (chars) String::FromWChars(AppNameW), (mUseWSS)? "wss" : "ws", mPort));
        }
        else Platform::SetWindowName(String::Format("%s://host:%d/", (mUseWSS)? "wss" : "ws", mPort));
    }
}

bool zaydataData::OnPacketOnce()
{
    bool NeedUpdate = false;
    Mutex::LocalLock(mPacketMutex);
    {
        // Event서버
        while(Platform::Server::TryNextPacket(mServer))
        {
            const sint32 CurPeerID = Platform::Server::GetPacketPeerID(mServer);
            switch(Platform::Server::GetPacketType(mServer))
            {
            case packettype_entrance:
                mPeerTokens[CurPeerID];
                break;
            case packettype_leaved:
            case packettype_kicked:
                // 로그인된 상태였다면
                if(0 < mPeerTokens[CurPeerID].Length())
                {
                    // ValidStatus처리
                    if(auto CurToken = mTokens.Access(mPeerTokens[CurPeerID]))
                    {
                        auto& CurProfile = mPrograms(CurToken->mProgramID).mProfiles(CurToken->mAuthor);
                        CurProfile.ValidStatus(mServer, false);
                    }
                }
                mPeerTokens.Remove(CurPeerID);
                break;
            case packettype_message:
                {
                    sint32 BufferSize = 0;
                    bytes Buffer = Platform::Server::GetPacketBuffer(mServer, &BufferSize);
                    const Context RecvJson(ST_Json, SO_OnlyReference, (chars) Buffer, BufferSize);
                    const String Type = RecvJson("type").GetText();

                    branch;
                    jump(!Type.Compare("Login")) OnRecv_Login(CurPeerID, RecvJson);
                    jump(!Type.Compare("LoginUpdate")) OnRecv_LoginUpdate(CurPeerID, RecvJson);
                    jump(!Type.Compare("Logout")) OnRecv_Logout(CurPeerID, RecvJson);
                    jump(!Type.Compare("FocusProfile")) OnRecv_FocusProfile(CurPeerID, RecvJson);
                    jump(!Type.Compare("UnfocusProfile")) OnRecv_UnfocusProfile(CurPeerID, RecvJson);
                    jump(!Type.Compare("LockAsset")) OnRecv_LockAsset(CurPeerID, RecvJson);
                    jump(!Type.Compare("UnlockAsset")) OnRecv_UnlockAsset(CurPeerID, RecvJson);
                    jump(!Type.Compare("FocusAsset")) OnRecv_FocusAsset(CurPeerID, RecvJson);
                    jump(!Type.Compare("UnfocusAsset")) OnRecv_UnfocusAsset(CurPeerID, RecvJson);
                    jump(!Type.Compare("EnumAsset")) OnRecv_EnumAsset(CurPeerID, RecvJson);
                }
                break;
            }
            NeedUpdate = true;
        }
    }
    Mutex::LocalUnlock(mPacketMutex);
    return NeedUpdate;
}

void zaydataData::OnRecv_Login(sint32 peerid, const Context& json)
{
    const String ProgramID = PACKET_TEXT("programid");
    const String DeviceID = PACKET_TEXT("deviceid");
    if(auto CurAuthor = mPrograms(ProgramID).mFastLogin.Access(DeviceID))
    {
        // 새 토큰을 생성
        const String TokenCode = ZDProgram::CreateTokenCode(DeviceID);
        auto& NewToken = mTokens(TokenCode);
        NewToken.mProgramID = ProgramID;
        NewToken.mAuthor = *CurAuthor;
        NewToken.mDeviceID = DeviceID;
        // 피어에 토큰등록 및 유효기간갱신
        ValidToken(peerid, TokenCode);

        // 응답처리1
        Context Json;
        Json.At("type").Set("Logined");
        Json.At("author").Set(*CurAuthor);
        Json.At("token").Set(TokenCode);
        SendPacket(peerid, Json);
        // 응답처리2
        if(auto CurProfile = mPrograms(ProgramID).ValidProfile(ProgramID, *CurAuthor, true))
        {
            CurProfile->SendPacket(mServer, peerid);
            // 혹시 입장상태가 아니었다면 포커싱된 피어들에게 알림
            CurProfile->ValidStatus(mServer, true);
        }
    }
    else SendError(peerid, json, "Unregistered device");
}

void zaydataData::OnRecv_LoginUpdate(sint32 peerid, const Context& json)
{
    const String ProgramID = PACKET_TEXT("programid");
    const String DeviceID = PACKET_TEXT("deviceid");
    const String Author = PACKET_TEXT("author");
    const String Password = PACKET_TEXT("password");
    const bool HasData = (0 < json("data").LengthOfNamable());

    // 프로필이 있으면 비번체크를 실시
    bool ProfileCreated = false;
    if(auto CurProfile = mPrograms(ProgramID).ValidProfile(ProgramID, Author, true))
    {
        if(!CurProfile->mPassword.Compare(Password))
        {
            if(HasData)
            {
                CurProfile->mEntered = true; // 어차피 업데이트할 것이라서 ValidStatus가 불필요
                CurProfile->mData = json("data");
            }
        }
        else
        {
            SendError(peerid, json, "Wrong password");
            return;
        }
    }
    // 프로필이 없으면 프로필을 생성
    else
    {
        ProfileCreated = true;
        auto& NewProfile = mPrograms(ProgramID).mProfiles(Author);
        NewProfile.mAuthor = Author;
        NewProfile.mEntered = true;
        NewProfile.mPassword = Password;
        if(HasData)
            NewProfile.mData = json("data");
    }

    // 업데이트처리
    if(ProfileCreated || HasData)
    hook(mPrograms(ProgramID).mProfiles(Author))
    {
        // 버전상승
        fish.VersionUp(ProgramID, mServer, peerid);
        // 포커싱된 피어들에게 ProfileUpdated전파
        fish.SendPacketAll(mServer);
        // 파일세이브
        fish.SaveFile(ProgramID);
    }

    // DeviceID to Author를 갱신
    mPrograms(ProgramID).mFastLogin(DeviceID) = Author;

    // 로그인처리
    OnRecv_Login(peerid, json);
}

void zaydataData::OnRecv_Logout(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    if(auto CurToken = ValidToken(peerid, Token))
    {
        hook(mPrograms(CurToken->mProgramID))
        {
            // DeviceID to Author를 제거
            fish.mFastLogin.Remove(CurToken->mDeviceID);
            // ValidStatus처리
            fish.mProfiles(CurToken->mAuthor).ValidStatus(mServer, false);
        }
        mPeerTokens[peerid].Empty();
        mTokens.Remove(Token);
    }
    else SendError(peerid, json, "Expired token");
}

void zaydataData::OnRecv_FocusProfile(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    const String Author = PACKET_TEXT("author");
    if(auto CurToken = ValidToken(peerid, Token))
    {
        if(auto CurProfile = mPrograms(CurToken->mProgramID).ValidProfile(CurToken->mProgramID, Author, false))
        {
            CurProfile->Bind(peerid);
            CurProfile->SendPacket(mServer, peerid); // 포커싱하면 최신데이터전송
        }
        else SendError(peerid, json, "Unregistered author");
    }
    else SendError(peerid, json, "Expired token");
}

void zaydataData::OnRecv_UnfocusProfile(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    const String Author = PACKET_TEXT("author");
    if(auto CurToken = ValidToken(peerid, Token))
    {
        if(auto CurProfile = mPrograms(CurToken->mProgramID).ValidProfile(CurToken->mProgramID, Author, false))
            CurProfile->Unbind(peerid);
        else SendError(peerid, json, "Unregistered author");
    }
    else SendError(peerid, json, "Expired token");
}

void zaydataData::OnRecv_LockAsset(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    const String LockID = PACKET_TEXT("lockid");
    const String RouteRequested = PACKET_TEXT("route"); // board.post.[next]도 가능
    if(auto CurToken = ValidToken(peerid, Token))
    {
        bool Locked = false;
        const String Route = mPrograms(CurToken->mProgramID).ValidAssetRoute(CurToken->mProgramID, RouteRequested);
        if(auto CurAsset = mPrograms(CurToken->mProgramID).ValidAsset(CurToken->mProgramID, Route))
        {
            if(!CurAsset->mLocked)
            {
                if(!CurToken->mLockedRoutes.Access(LockID))
                {
                    if(CurAsset->Locking(mServer, CurToken->mAuthor)) // 포커싱된 피어들에게 AssetChanged전파
                        Locked = true;
                    else SendError(peerid, json, "No asset match author");
                }
                else SendError(peerid, json, "LockID already in use");
            }
            else SendError(peerid, json, "Asset already locked");
        }
        // 어셋이 없으면 어셋을 생성
        else
        {
            Locked = true;
            auto& NewAsset = mPrograms(CurToken->mProgramID).mAssets(Route);
            NewAsset.mAuthor = CurToken->mAuthor;
            NewAsset.mLocked = true;
            NewAsset.mRoute = Route;
        }

        // 잠금처리
        if(Locked)
        {
            CurToken->mLockedRoutes(LockID) = Route;
            // 응답처리
            Context Json;
            Json.At("type").Set("AssetLocked");
            Json.At("lockid").Set(LockID);
            Json.At("route").Set(Route);
            SendPacket(peerid, Json);
        }
    }
    else SendError(peerid, json, "Expired token");
}

void zaydataData::OnRecv_UnlockAsset(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    const String LockID = PACKET_TEXT("lockid");
    if(auto CurToken = ValidToken(peerid, Token))
    {
        if(auto CurRoute = CurToken->mLockedRoutes.Access(LockID))
        {
            const String OneRoute = *CurRoute;
            if(auto CurAsset = mPrograms(CurToken->mProgramID).ValidAsset(CurToken->mProgramID, OneRoute))
            {
                CurToken->mLockedRoutes.Remove(LockID);
                CurAsset->mLocked = false;
                CurAsset->mData = json("data");
                hook(mPrograms(CurToken->mProgramID).mAssets(OneRoute))
                {
                    // 버전상승
                    fish.VersionUp(CurToken->mProgramID, mServer, peerid);
                    // 포커싱된 피어들에게 업데이트
                    fish.SendPacketAll(mServer);
                    // 파일세이브
                    fish.SaveFile(CurToken->mProgramID);
                }
            }
            else SendError(peerid, json, "No asset matching the route");
        }
        else SendError(peerid, json, "Expired lockID");
    }
    else SendError(peerid, json, "Expired token");
}

void zaydataData::OnRecv_FocusAsset(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    const String Route = PACKET_TEXT("route");
    if(auto CurToken = ValidToken(peerid, Token))
    {
        if(auto CurAsset = mPrograms(CurToken->mProgramID).ValidAsset(CurToken->mProgramID, Route))
        {
            CurAsset->Bind(peerid);
            CurAsset->SendPacket(mServer, peerid); // 포커싱하면 최신데이터전송
        }
        else SendError(peerid, json, "No asset matching the route");
    }
    else SendError(peerid, json, "Expired token");
}

void zaydataData::OnRecv_UnfocusAsset(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    const String Route = PACKET_TEXT("route");
    if(auto CurToken = ValidToken(peerid, Token))
    {
        if(auto CurAsset = mPrograms(CurToken->mProgramID).ValidAsset(CurToken->mProgramID, Route))
            CurAsset->Unbind(peerid);
        else SendError(peerid, json, "No asset matching the route");
    }
    else SendError(peerid, json, "Expired token");
}

void zaydataData::OnRecv_EnumAsset(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    const String RouteRequested = PACKET_TEXT("route");
    const sint32 MaxCount = PACKET_INT("maxcount");
    if(auto CurToken = ValidToken(peerid, Token))
    {
        sint32 TotalCount = 0;
        const String Route = mPrograms(CurToken->mProgramID).ValidAssetRoute(CurToken->mProgramID, RouteRequested);
        const Strings Routes = mPrograms(CurToken->mProgramID).EnumAssetRoutes(CurToken->mProgramID, Route, MaxCount, TotalCount);
        // 응답처리
        Context Json;
        Json.At("type").Set("AssetEnumed");
        Json.At("totalcount").Set(String::FromInteger(TotalCount));
        for(sint32 i = 0, iend = Routes.Count(); i < iend; ++i)
            Json.At("routes").AtAdding().Set(Routes[i]);
        SendPacket(peerid, Json);
    }
    else SendError(peerid, json, "Expired token");
}

void zaydataData::SendPacket(sint32 peerid, const Context& json)
{
    const String Packet = json.SaveJson();
    Platform::Server::SendToPeer(mServer, peerid, (chars) Packet, Packet.Length() + 1, true);
}

void zaydataData::SendError(sint32 peerid, const Context& json, chars text)
{
    Context Json;
    Json.At("type").Set("Errored");
    Json.At("packet").Set(json("type").GetText("UnknownType"));
    Json.At("text").Set(text);
    SendPacket(peerid, Json);
}
