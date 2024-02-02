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

void ZDSubscribe::VersionUp(id_server server, sint32 peerid, chars dirpath)
{
    // 새 버전코드 확보
    const String AssetDir = Platform::File::RootForAssets() + dirpath;
    const String LatestPath = AssetDir + ".latest";
    sint32 NewVersionCode = 1;
    if(Platform::File::Exist(LatestPath))
    {
        const String OldVersion = String::FromFile(LatestPath);
        const sint32 OldVersionCode = Parser::GetInt(((chars) OldVersion) + 1); // "w1_"
        NewVersionCode = OldVersionCode + 1;
    }

    // 새 버전 생성/저장
    const String TimeCode = ZDProgram::CreateTimeCode();
    ip4address GetIP4;
    Platform::Server::GetPeerInfo(server, peerid, &GetIP4);
    mVersion = String::Format("w%d_t%s_a%03d%03d%03d%03d", NewVersionCode,
        (chars) TimeCode, GetIP4.ip[0], GetIP4.ip[1], GetIP4.ip[2], GetIP4.ip[3]);
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

void ZDProfile::SaveFile(chars dirpath) const
{
    Context Detail;
    Detail.At("password").Set(mPassword);
    Detail.At("written").Set(String::FromInteger(mWritten));
    Detail.At("like").Set(String::FromInteger(mLike));
    const String AssetDir = Platform::File::RootForAssets() + dirpath;
    Detail.SaveJson().ToFile(AssetDir + "detail.json", true);
    mData.SaveJson().ToFile(AssetDir + mVersion + ".json", true);
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

void ZDAsset::SaveFile(chars dirpath) const
{
    const String AssetDir = Platform::File::RootForAssets() + dirpath;
    mData.SaveJson().ToFile(AssetDir + mVersion + ".json", true);
}

String ZDAsset::BuildPacket() const
{
    Context Packet;
    Packet.At("type").Set("AssetUpdated");
    Packet.At("author").Set(mAuthor);
    Packet.At("status").Set((mLocked)? "lock" : "unlock");
    Packet.At("route").Set(mRoute);
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

String ZDProgram::CreateTimeCode()
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

ZDProfile* ZDProgram::ValidProfile(chars author, chars dirpath, bool entering)
{
    if(!mProfiles.Access(author))
    {
        const String AssetDir = Platform::File::RootForAssets() + dirpath;
        const String LatestPath = AssetDir + ".latest";
        if(Platform::File::Exist(LatestPath))
        {
            const String DetailJson = String::FromFile(AssetDir + "detail.json");
            const Context Detail(ST_Json, SO_OnlyReference, DetailJson);

            auto& NewProfile = mProfiles(author);
            NewProfile.mAuthor = author;
            NewProfile.mEntered = entering;
            NewProfile.mPassword = Detail("password").GetText();
            NewProfile.mWritten = Detail("written").GetInt();
            NewProfile.mLike = Detail("like").GetInt();
            const String CurVersion = String::FromFile(LatestPath);
            if(0 < CurVersion.Length())
            {
                NewProfile.mVersion = CurVersion;
                const String DataJson = String::FromFile(AssetDir + CurVersion + ".json");
                NewProfile.mData.LoadJson(SO_NeedCopy, DataJson);
            }
        }
    }
    return mProfiles.Access(author);
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
