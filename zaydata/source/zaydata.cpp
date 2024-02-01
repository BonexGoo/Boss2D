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

void zaydataData::CheckToken(sint32 peerid, chars token)
{
    // 동일한 피어가 통신중 토큰을 바꾸면
    if(!!mPeerTokens[peerid].Compare(token))
    {
        // 이전 토큰을 제거
        mTokens.Remove(mPeerTokens[peerid]);
        mPeerTokens[peerid] = token;
    }
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
                    jump(!Type.Compare("LockData")) OnRecv_LockData(CurPeerID, RecvJson);
                    jump(!Type.Compare("UnlockData")) OnRecv_UnlockData(CurPeerID, RecvJson);
                    jump(!Type.Compare("FocusData")) OnRecv_FocusData(CurPeerID, RecvJson);
                    jump(!Type.Compare("UnfocusData")) OnRecv_UnfocusData(CurPeerID, RecvJson);
                    jump(!Type.Compare("EnumData")) OnRecv_EnumData(CurPeerID, RecvJson);
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
        NewToken.UpdateExpiry();
        // 토큰확인
        CheckToken(peerid, TokenCode);

        // 응답처리1
        Context Json;
        Json.At("type").Set("Logined");
        Json.At("author").Set(*CurAuthor);
        Json.At("token").Set(TokenCode);
        SendPacket(peerid, Json);
        // 응답처리2
        if(auto CurProfile = mPrograms(ProgramID).mProfiles.Access(*CurAuthor))
            CurProfile->SendPacket(mServer, peerid);
    }
    // 에러처리
    else SendError(peerid, json, "Unregistered device");
}

void zaydataData::OnRecv_LoginUpdate(sint32 peerid, const Context& json)
{
    const String ProgramID = PACKET_TEXT("programid");
    const String DeviceID = PACKET_TEXT("deviceid");
    const String Author = PACKET_TEXT("author");
    const String Password = PACKET_TEXT("password");
    const bool HasUserData = (0 < json("userdata").LengthOfNamable());

    // 프로필 보장
    const String DirPath = ProgramID + "/profile/" + Author + "/";
    mPrograms(ProgramID).ValidProfile(Author, DirPath);
    mPrograms(ProgramID).mFastLogin(DeviceID) = Author; // DeviceID to Author를 갱신

    // 프로필이 있으면 비번체크를 실시
    bool ProfileCreated = false;
    if(auto CurProfile = mPrograms(ProgramID).mProfiles.Access(Author))
    {
        if(!CurProfile->mPassword.Compare(Password))
        {
            if(HasUserData)
                CurProfile->mData = json("userdata");
        }
        else // 에러처리
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
        NewProfile.mPassword = Password;
        if(HasUserData)
            NewProfile.mData = json("userdata");
    }

    // 업데이트처리
    if(ProfileCreated || HasUserData)
    hook(mPrograms(ProgramID).mProfiles(Author))
    {
        // 버전상승
        fish.VersionUp(mServer, peerid, DirPath);
        // 포커싱된 피어들에게 업데이트
        fish.SendPacketAll(mServer, peerid);
        // 파일세이브
        fish.SaveFile(DirPath);
    }

    // 로그인처리
    OnRecv_Login(peerid, json);
}

void zaydataData::OnRecv_Logout(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    if(auto CurToken = mTokens.Access(Token))
    {
        // 토큰확인
        CheckToken(peerid, Token);
        // DeviceID to Author를 제거
        mPrograms(CurToken->mProgramID).mFastLogin.Remove(CurToken->mDeviceID);
        mPeerTokens[peerid].Empty();
        mTokens.Remove(Token);
    }
    // 에러처리
    else SendError(peerid, json, "Expired token");
}

void zaydataData::OnRecv_FocusProfile(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    const String Author = PACKET_TEXT("author");
    if(auto CurToken = mTokens.Access(Token))
    {
        // 토큰확인
        CheckToken(peerid, Token);
        hook(mPrograms(CurToken->mProgramID).mProfiles(Author))
        {
            fish.Bind(peerid);
            fish.SendPacket(mServer, peerid); // 포커싱하면 최신데이터전송
        }
        CurToken->UpdateExpiry();
    }
    // 에러처리
    else SendError(peerid, json, "Expired token");
}

void zaydataData::OnRecv_UnfocusProfile(sint32 peerid, const Context& json)
{
    const String Token = PACKET_TEXT("token");
    const String Author = PACKET_TEXT("author");
    if(auto CurToken = mTokens.Access(Token))
    {
        // 토큰확인
        CheckToken(peerid, Token);
        mPrograms(CurToken->mProgramID).mProfiles(Author).Unbind(peerid);
        CurToken->UpdateExpiry();
    }
    // 에러처리
    else SendError(peerid, json, "Expired token");
}

void zaydataData::OnRecv_LockData(sint32 peerid, const Context& json)
{
}

void zaydataData::OnRecv_UnlockData(sint32 peerid, const Context& json)
{
}

void zaydataData::OnRecv_FocusData(sint32 peerid, const Context& json)
{
}

void zaydataData::OnRecv_UnfocusData(sint32 peerid, const Context& json)
{
}

void zaydataData::OnRecv_EnumData(sint32 peerid, const Context& json)
{
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
