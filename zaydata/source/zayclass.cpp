#include <boss.hpp>
#include "zayclass.hpp"

#include <platform/boss_platform.hpp>

////////////////////////////////////////////////////////////////////////////////
// ZDFocusable
ZDFocusable::ZDFocusable()
{
}

ZDFocusable::~ZDFocusable()
{
}

void ZDFocusable::Bind(sint32 peerid)
{
    for(sint32 i = 0, iend = mPeerIDs.Count(); i < iend; ++i)
        if(mPeerIDs[i] == peerid)
            return;
    mPeerIDs.AtAdding() = peerid;
}

void ZDFocusable::Unbind(sint32 peerid)
{
    for(sint32 i = 0, iend = mPeerIDs.Count(); i < iend; ++i)
        if(mPeerIDs[i] == peerid)
        {
            mPeerIDs.SubtractionSection(i);
            break;
        }
}

void ZDFocusable::SendPacket(id_server server, sint32 peerid)
{
    const String NewPacket = BuildPacket();
    Platform::Server::SendToPeer(server, peerid,
        (chars) NewPacket, NewPacket.Length() + 1, true);
}

void ZDFocusable::SendPacketAll(id_server server)
{
    const String NewPacket = BuildPacket();
    for(sint32 i = 0, iend = mPeerIDs.Count(); i < iend; ++i)
        Platform::Server::SendToPeer(server, mPeerIDs[i],
            (chars) NewPacket, NewPacket.Length() + 1, true);
}

String ZDFocusable::MakeProfileDir(chars programid, chars author)
{
    return Platform::File::RootForDocuments() + "ZayData/" + programid + "/profile/" + author + "/";
}

String ZDFocusable::MakeAssetDir(chars programid, chars path)
{
    if(path == nullptr || *path == '\0')
        return Platform::File::RootForDocuments() + "ZayData/" + programid + "/asset/";
    return Platform::File::RootForDocuments() + "ZayData/" + programid + "/asset/" + path + "/";
}

////////////////////////////////////////////////////////////////////////////////
// ZDRange
ZDRange::ZDRange()
{
}

ZDRange::~ZDRange()
{
}

void ZDRange::SaveFile(chars programid) const
{
    String::Format("%d:%d", mFirst, mLast).ToFile(DataDir(programid) + ".range", true);
}

String ZDRange::BuildPacket() const
{
    const bool HasRange = (mFirst <= mLast);
    Context Packet;
    Packet.At("type").Set("RangeUpdated");
    Packet.At("route").Set(mRoute.mNormal);
    Packet.At("first").Set(String::FromInteger((HasRange)? mFirst : 0));
    Packet.At("last").Set(String::FromInteger((HasRange)? mLast : 0));
    return Packet.SaveJson();
}

String ZDRange::DataDir(chars programid) const
{
    return MakeAssetDir(programid, mRoute.mPath);
}

////////////////////////////////////////////////////////////////////////////////
// ZDRecordable
ZDRecordable::ZDRecordable()
{
}

ZDRecordable::~ZDRecordable()
{
}

void ZDRecordable::VersionUp(chars programid, id_server server, sint32 peerid)
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

String ZDProfile::DataDir(chars programid) const
{
    return MakeProfileDir(programid, mAuthor);
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
        Packet.At("route").Set(mRoute.mNormal);
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

String ZDAsset::BuildPacket() const
{
    Context Packet;
    Packet.At("type").Set("AssetUpdated");
    Packet.At("author").Set(mAuthor);
    Packet.At("route").Set(mRoute.mNormal);
    Packet.At("status").Set((mLocked)? "lock" : "unlock");
    Packet.At("version").Set(mVersion);
    if(0 < mData.LengthOfNamable())
        Packet.At("data") = mData;
    return Packet.SaveJson();
}

String ZDAsset::DataDir(chars programid) const
{
    return MakeAssetDir(programid, mRoute.mPath);
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

ZDProfile* ZDProgram::GetProfile(chars programid, chars author, bool entering)
{
    if(!mProfiles.Access(author))
    {
        const String DataDir = ZDFocusable::MakeProfileDir(programid, author);
        const String LatestPath = DataDir + ".latest";
        if(Platform::File::Exist(LatestPath))
        {
            const String DetailJson = String::FromFile(DataDir + "detail.json");
            const Context Detail(ST_Json, SO_OnlyReference, DetailJson);
            // 새 프로필
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

ZDAsset* ZDProgram::GetAsset(chars programid, const ZDRoute& route)
{
    if(!mAssets.Access(route.mPath))
    {
        const String DataDir = ZDFocusable::MakeAssetDir(programid, route.mPath);
        const String LatestPath = DataDir + ".latest";
        if(Platform::File::Exist(LatestPath))
        {
            const String DetailJson = String::FromFile(DataDir + "detail.json");
            const Context Detail(ST_Json, SO_OnlyReference, DetailJson);
            // 새 어셋
            auto& NewAsset = mAssets(route.mPath);
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
    return mAssets.Access(route.mPath);
}

ZDRange& ZDProgram::ValidRange(chars programid, const ZDRoute& route)
{
    if(!mRanges.Access(route.mPath))
    {
        const String DataDir = ZDAsset::MakeAssetDir(programid, route.mPath);
        const String Range = String::FromFile(DataDir + ".range");
        const sint32 ColonPos = Range.Find(0, ":");
        // 새 범위
        auto& NewRange = mRanges(route.mPath);
        NewRange.mRoute = route;
        NewRange.mFirst = (ColonPos == -1)? 1 : Parser::GetInt(Range.Left(ColonPos));
        NewRange.mLast = (ColonPos == -1)? -1 : Parser::GetInt(Range.Offset(ColonPos + 1));
    }
    return mRanges(route.mPath);
}

ZDRoute ZDProgram::ParseRoute(chars programid, chars route, id_server writable_server)
{
    if(*route == '\0')
        return {"", ""};

    // ".<first>.a.b<last>.c..d.<prev><next>e."의 경우
    chararray Path; // "0/a/b/1/c/d/-1/2/e"가 됨
    chararray Normal; // "<0>a.b<1>c.d<-1><2>e"가 됨
    do
    {
        if(*route == '<')
        {
            chars RouteNext = route;
            while(RouteNext[0] != '>' && RouteNext[1] != '\0')
                RouteNext++;

            // first, last, prev, next는 4글자이상
            if(4 <= RouteNext - route - 1)
            {
                // 마지막 기호제거
                if(0 < Path.Count() && Path[-1] == '/')
                    Path.SubtractionOne();
                if(0 < Normal.Count() && Normal[-1] == '.')
                    Normal.SubtractionOne();

                // 범위정보 열람
                auto& CurRange = ValidRange(programid, {String(&Path[0], Path.Count()), String(&Normal[0], Normal.Count())});
                const bool HasRange = (CurRange.mFirst <= CurRange.mLast);

                // 명령어파싱
                sint32 Number = 0;
                if(!String::Compare(route, strpair("<first>"))) Number = (HasRange)? CurRange.mFirst : 0;
                else if(!String::Compare(route, strpair("<last>"))) Number = (HasRange)? CurRange.mLast : 0;
                else if(!String::Compare(route, strpair("<prev>"))) Number = CurRange.mFirst - 1;
                else if(!String::Compare(route, strpair("<next>"))) Number = CurRange.mLast + 1;
                else Number = Parser::GetInt(route + 1);

                // 범위정보 갱신
                if(writable_server && (Number < CurRange.mFirst || CurRange.mLast < Number))
                {
                    CurRange.mFirst = Math::Min(CurRange.mFirst, Number);
                    CurRange.mLast = Math::Max(CurRange.mLast, Number);
                    // 포커싱된 피어들에게 업데이트
                    CurRange.SendPacketAll(writable_server);
                    // 파일세이브
                    CurRange.SaveFile(programid);
                }

                // 패스화 및 노말화
                if(0 < Path.Count())
                    Path = chararray((id_cloned_share) String::Format("%.*s/%d/", Path.Count(), &Path[0], Number));
                else Path = chararray((id_cloned_share) String::Format("%d/", Number));
                if(0 < Normal.Count())
                    Normal = chararray((id_cloned_share) String::Format("%.*s<%d>", Normal.Count(), &Normal[0], Number));
                else Normal = chararray((id_cloned_share) String::Format("<%d>", Number));
            }
            route = RouteNext;
        }
        else if(*route == '.')
        {
            if(0 < Path.Count() && Path[-1] != '/')
                Path.AtAdding() = '/';
            if(0 < Normal.Count() && Normal[-1] != '.' && Normal[-1] != '>')
                Normal.AtAdding() = '.';
        }
        else
        {
            Path.AtAdding() = *route;
            Normal.AtAdding() = *route;
        }
    }
    while(*(++route) != '\0');

    // 마지막 기호제거
    if(0 < Path.Count() && Path[-1] == '/')
        Path.SubtractionOne();
    if(0 < Normal.Count() && Normal[-1] == '.')
        Normal.SubtractionOne();
    return {String(&Path[0], Path.Count()), String(&Normal[0], Normal.Count())};
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
