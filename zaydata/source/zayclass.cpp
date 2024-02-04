#include <boss.hpp>
#include "zayclass.hpp"

#include <platform/boss_platform.hpp>

////////////////////////////////////////////////////////////////////////////////
// ZDSubscribe
ZDSubscribe::ZDSubscribe()
{
}

ZDSubscribe::~ZDSubscribe()
{
}

void ZDSubscribe::Bind(sint32 peerid)
{
    for(sint32 i = 0, iend = mPeerIDs.Count(); i < iend; ++i)
        if(mPeerIDs[i] == peerid)
            return;
    mPeerIDs.AtAdding() = peerid;
}

void ZDSubscribe::Unbind(sint32 peerid)
{
    for(sint32 i = 0, iend = mPeerIDs.Count(); i < iend; ++i)
        if(mPeerIDs[i] == peerid)
        {
            mPeerIDs.SubtractionSection(i);
            break;
        }
}

void ZDSubscribe::VersionUp(chars programid, id_server server, sint32 peerid)
{
    const String LatestPath = DataDir(programid) + ".latest";

    // 새 버전코드 확보
    sint32 NewVersionCode = 1;
    if(Platform::File::Exist(LatestPath))
    {
        const String OldVersion = String::FromFile(LatestPath);
        const sint32 OldVersionCode = Parser::GetInt(((chars) OldVersion) + 1); // "w1_"
        NewVersionCode = OldVersionCode + 1;
    }

    // 새 버전 생성/저장
    const String TimeTag = ZDProgram::CreateTimeTag();
    ip4address GetIP4;
    Platform::Server::GetPeerInfo(server, peerid, &GetIP4);
    mVersion = String::Format("w%d_t%s_a%03d%03d%03d%03d", NewVersionCode,
        (chars) TimeTag, GetIP4.ip[0], GetIP4.ip[1], GetIP4.ip[2], GetIP4.ip[3]);
    mVersion.ToFile(LatestPath, true);
    Platform::File::SetAttributeHidden(WString::FromChars(LatestPath));
}

void ZDSubscribe::SendPacket(id_server server, sint32 peerid)
{
    const String NewPacket = BuildPacket();
    Platform::Server::SendToPeer(server, peerid,
        (chars) NewPacket, NewPacket.Length() + 1, true);
}

void ZDSubscribe::SendPacketAll(id_server server)
{
    const String NewPacket = BuildPacket();
    for(sint32 i = 0, iend = mPeerIDs.Count(); i < iend; ++i)
        Platform::Server::SendToPeer(server, mPeerIDs[i],
            (chars) NewPacket, NewPacket.Length() + 1, true);
}

////////////////////////////////////////////////////////////////////////////////
// ZDProfile
ZDProfile::ZDProfile()
{
}

ZDProfile::~ZDProfile()
{
}

void ZDProfile::ValidStatus(id_server server, bool entered)
{
    if(mEntered != entered)
    {
        mEntered = entered;
        Context Packet;
        Packet.At("type").Set("ProfileChanged");
        Packet.At("author").Set(mAuthor);
        Packet.At("status").Set((mEntered)? "enter" : "exit");
        const String NewPacket = Packet.SaveJson();
        for(sint32 i = 0, iend = mPeerIDs.Count(); i < iend; ++i)
            Platform::Server::SendToPeer(server, mPeerIDs[i],
                (chars) NewPacket, NewPacket.Length() + 1, true);
    }
}

void ZDProfile::SaveFile(chars programid) const
{
    Context Detail;
    Detail.At("author").Set(mAuthor);
    Detail.At("password").Set(mPassword);
    Detail.At("written").Set(String::FromInteger(mWritten));
    Detail.At("like").Set(String::FromInteger(mLike));
    Detail.SaveJson().ToFile(DataDir(programid) + "detail.json", true);
    mData.SaveJson().ToFile(DataDir(programid) + mVersion + ".json", true);
}

String ZDProfile::MakeDataDir(chars programid, chars author)
{
    return Platform::File::RootForDocuments() + "ZayData/" + programid + "/profile/" + author + "/";
}

String ZDProfile::DataDir(chars programid) const
{
    return MakeDataDir(programid, mAuthor);
}

String ZDProfile::BuildPacket() const
{
    Context Packet;
    Packet.At("type").Set("ProfileUpdated");
    Packet.At("author").Set(mAuthor);
    Packet.At("status").Set((mEntered)? "enter" : "exit");
    Packet.At("written").Set(String::FromInteger(mWritten));
    Packet.At("like").Set(String::FromInteger(mLike));
    Packet.At("version").Set(mVersion);
    if(0 < mData.LengthOfNamable())
        Packet.At("data") = mData;
    return Packet.SaveJson();
}

////////////////////////////////////////////////////////////////////////////////
// ZDAsset
ZDAsset::ZDAsset()
{
}

ZDAsset::~ZDAsset()
{
}

bool ZDAsset::Locking(id_server server, chars author)
{
    if(!mAuthor.Compare(author))
    {
        mLocked = true;
        Context Packet;
        Packet.At("type").Set("AssetChanged");
        Packet.At("route").Set(mRoute);
        Packet.At("status").Set("lock");
        const String NewPacket = Packet.SaveJson();
        for(sint32 i = 0, iend = mPeerIDs.Count(); i < iend; ++i)
            Platform::Server::SendToPeer(server, mPeerIDs[i],
                (chars) NewPacket, NewPacket.Length() + 1, true);
        return true;
    }
    return false;
}

void ZDAsset::SaveFile(chars programid) const
{
    Context Detail;
    Detail.At("author").Set(mAuthor);
    Detail.SaveJson().ToFile(DataDir(programid) + "detail.json", true);
    mData.SaveJson().ToFile(DataDir(programid) + mVersion + ".json", true);
}

String ZDAsset::MakeDataDir(chars programid, chars route)
{
    if(route == nullptr)
        return Platform::File::RootForDocuments() + "ZayData/" + programid + "/asset/";
    return Platform::File::RootForDocuments() + "ZayData/" + programid + "/asset/" + String(route).Replace(".", "/") + "/";
}

String ZDAsset::DataDir(chars programid) const
{
    return MakeDataDir(programid, mRoute);
}

String ZDAsset::BuildPacket() const
{
    Context Packet;
    Packet.At("type").Set("AssetUpdated");
    Packet.At("author").Set(mAuthor);
    Packet.At("route").Set(mRoute);
    Packet.At("status").Set((mLocked)? "lock" : "unlock");
    Packet.At("version").Set(mVersion);
    if(0 < mData.LengthOfNamable())
        Packet.At("data") = mData;
    return Packet.SaveJson();
}

////////////////////////////////////////////////////////////////////////////////
// ZDProgram
String ZDProgram::CreateTokenCode(chars deviceid)
{
    static uint32 LastSeed = Platform::Utility::Random();
    const uint32 SkipValue = 10 + (Platform::Utility::Random() % 90); // 10 ~ 99
    LastSeed = (LastSeed + SkipValue) % 1000000; // 0 ~ 999999
    return String::Format("%s%06u", deviceid, LastSeed);
}

String ZDProgram::CreateTimeTag()
{
    sint32 Year = 0, Month = 0, Day = 0, Hour = 0, Min = 0, Sec = 0;
    if(auto NewClock = Platform::Clock::CreateAsCurrent())
    {
        const sint64 UTCMsec = Platform::Clock::GetLocalMsecFromUTC();
        Platform::Clock::AddNsec(NewClock, -UTCMsec * 1000 * 1000);
        Platform::Clock::GetDetail(NewClock, nullptr, &Sec, &Min, &Hour, &Day, &Month, &Year);
        Platform::Clock::Release(NewClock);
    }
    return String::Format("%04d%02d%02dT%02d%02d%02dZ", Year, Month, Day, Hour, Min, Sec);
}

ZDProfile* ZDProgram::ValidProfile(chars programid, chars author, bool entering)
{
    if(!mProfiles.Access(author))
    {
        const String DataDir = ZDProfile::MakeDataDir(programid, author);
        const String LatestPath = DataDir + ".latest";
        if(Platform::File::Exist(LatestPath))
        {
            const String DetailJson = String::FromFile(DataDir + "detail.json");
            const Context Detail(ST_Json, SO_OnlyReference, DetailJson);

            auto& NewProfile = mProfiles(author);
            NewProfile.mAuthor = Detail("author").GetText();
            NewProfile.mEntered = entering;
            NewProfile.mPassword = Detail("password").GetText();
            NewProfile.mWritten = Detail("written").GetInt();
            NewProfile.mLike = Detail("like").GetInt();
            const String CurVersion = String::FromFile(LatestPath);
            if(0 < CurVersion.Length())
            {
                NewProfile.mVersion = CurVersion;
                const String DataJson = String::FromFile(DataDir + CurVersion + ".json");
                NewProfile.mData.LoadJson(SO_NeedCopy, DataJson);
            }
        }
    }
    return mProfiles.Access(author);
}

String ZDProgram::ValidAssetRoute(chars programid, chars route_requested)
{
    String Route = String(route_requested).Replace(".", "/"); // board.post.[next]
    while(true)
    {
        const sint32 NextPos = Route.Find(0, "[next]");
        if(NextPos == -1) break;

        struct Payload
        {
            String mDataDir;
            MapStrings mCollector;
        } NewPayload;
        NewPayload.mDataDir = ZDAsset::MakeDataDir(programid,
            (NextPos == 0)? nullptr : (chars) Route.Left(NextPos - 1));

        // 하위폴더를 MapStrings으로 소팅하여 수집
        Platform::File::Search(NewPayload.mDataDir + '*', [](chars itemname, payload data)->void
            {
                Payload& OnePayload = *((Payload*) data);
                if(Platform::File::ExistForDir(OnePayload.mDataDir + itemname))
                    OnePayload.mCollector(itemname) = itemname;
            }, &NewPayload, false);

        // NextName제작
        const sint32 Count = NewPayload.mCollector.Count();
        if(Count == 0) // 하위폴더가 없을 경우
            Route.Replace("[next]", "0");
        else if(auto LastFolder = NewPayload.mCollector.AccessByOrder(Count - 1))
        {
            chars FolderName = *LastFolder;
            String NextName;
            for(sint32 i = 0, iend = LastFolder->Length(); i < iend; ++i)
            {
                if('0' <= FolderName[i] && FolderName[i] <= '9') // 숫자가 등장하면 다음숫자로 변경
                {
                    sint32 Offset = i;
                    const sint32 Number = Parser::GetInt(FolderName, iend, &Offset); // "aa123bb"일 경우 5
                    NextName = LastFolder->Left(i) + String::FromInteger(Number + 1) + LastFolder->Offset(Offset);
                    break;
                }
            }
            if(NextName.Length() == 0) // FolderName에 숫자가 하나도 없을 경우
                Route.Replace("[next]", *LastFolder + '0');
            else Route.Replace("[next]", NextName);
        }
    }
    return Route.Replace("/", ".");
}

ZDAsset* ZDProgram::ValidAsset(chars programid, chars route)
{
    if(!mAssets.Access(route))
    {
        const String DataDir = ZDAsset::MakeDataDir(programid, route);
        const String LatestPath = DataDir + ".latest";
        if(Platform::File::Exist(LatestPath))
        {
            const String DetailJson = String::FromFile(DataDir + "detail.json");
            const Context Detail(ST_Json, SO_OnlyReference, DetailJson);

            auto& NewAsset = mAssets(route);
            NewAsset.mAuthor = Detail("author").GetText();
            NewAsset.mRoute = route;
            const String CurVersion = String::FromFile(LatestPath);
            if(0 < CurVersion.Length())
            {
                NewAsset.mVersion = CurVersion;
                const String DataJson = String::FromFile(DataDir + CurVersion + ".json");
                NewAsset.mData.LoadJson(SO_NeedCopy, DataJson);
            }
        }
    }
    return mAssets.Access(route);
}

////////////////////////////////////////////////////////////////////////////////
// ZDToken
void ZDToken::UpdateExpiry()
{
    mExpiryMsec = Platform::Utility::CurrentTimeMsec() + 60000 * 5; // 5분후
}

bool ZDToken::HasExpiry(uint64 now)
{
    return (mExpiryMsec < now);
}
