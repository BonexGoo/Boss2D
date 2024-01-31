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

void ZDSubscribe::Update(id_server server, sint32 peerid)
{
    const String Message = BuildPacket();
    Platform::Server::SendToPeer(server, peerid, (chars) Message, Message.Length() + 1, true);
}

void ZDSubscribe::UpdateAll(id_server server, sint32 excluded_peerid)
{
    const String Message = BuildPacket();
    for(sint32 i = 0, iend = mPeerIDs.Count(); i < iend; ++i)
        if(mPeerIDs[i] != excluded_peerid)
            Platform::Server::SendToPeer(server, mPeerIDs[i], (chars) Message, Message.Length() + 1, true);
}

////////////////////////////////////////////////////////////////////////////////
// ZDProfile
ZDProfile::ZDProfile()
{
}

ZDProfile::~ZDProfile()
{
}

String ZDProfile::BuildPacket() const
{
    Context Json;
    Json.At("type").Set("ProfileUpdated");
    Json.At("author").Set(mAuthor);
    Json.At("history").Set("w1/d20240129T102834Z/a210.34.20.3"); ///////////////////// 임시로!
    Json.At("userdata") = mUserData;
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
