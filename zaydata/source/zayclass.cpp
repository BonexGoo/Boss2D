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

void ZDSubscribe::UpdateVersion(id_server server, sint32 peerid, chars dirpath)
{
    // 최신버전 갱신
    const String VersionPath = Platform::File::RootForAssets() + dirpath + "version.txt";
    const String Version = String::FromFile(VersionPath);
    const sint32 NewVersion = Parser::GetInt(Version) + 1;
    String::FromInteger(NewVersion).ToFile(VersionPath, true);

    // 버전코드 생성
    const String TimeCode = ZDProgram::CreateTimeCode();
    ip4address GetIP4;
    Platform::Server::GetPeerInfo(server, peerid, &GetIP4);
    mVersion = String::Format("w%d_t%s_a%03d%03d%03d%03d",
        NewVersion, (chars) TimeCode, GetIP4.ip[0], GetIP4.ip[1], GetIP4.ip[2], GetIP4.ip[3]);
}

void ZDSubscribe::SendPacket(id_server server, sint32 peerid)
{
    const String NewPacket = BuildPacket();
    Platform::Server::SendToPeer(server, peerid, (chars) NewPacket, NewPacket.Length() + 1, true);
}

void ZDSubscribe::SendPacketAll(id_server server, sint32 excluded_peerid)
{
    const String NewPacket = BuildPacket();
    for(sint32 i = 0, iend = mPeerIDs.Count(); i < iend; ++i)
        if(mPeerIDs[i] != excluded_peerid)
            Platform::Server::SendToPeer(server, mPeerIDs[i], (chars) NewPacket, NewPacket.Length() + 1, true);
}

////////////////////////////////////////////////////////////////////////////////
// ZDProfile
ZDProfile::ZDProfile()
{
}

ZDProfile::~ZDProfile()
{
}

void ZDProfile::SaveFile(chars dirpath) const
{
    mPassword.ToFile(Platform::File::RootForAssets() + dirpath + "password.txt", true);
    mData.SaveJson().ToFile(Platform::File::RootForAssets() + dirpath + version() + ".json", true);
}

String ZDProfile::BuildPacket() const
{
    Context Json;
    Json.At("type").Set("ProfileUpdated");
    Json.At("author").Set(mAuthor);
    Json.At("version").Set(version());
    if(0 < mData.LengthOfNamable())
        Json.At("userdata") = mData;
    return Json.SaveJson();
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
