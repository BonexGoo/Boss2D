#pragma once
#include <boss.hpp>

typedef Map<String> MapStrings;
typedef Map<class ZDProfile> ZDProfiles;
typedef Map<class ZDProgram> ZDPrograms;
typedef Map<class ZDToken> ZDTokens;

////////////////////////////////////////////////////////////////////////////////
// ZDSubscribe
class ZDSubscribe
{
public:
    ZDSubscribe();
    virtual ~ZDSubscribe();

public:
    void Bind(sint32 peerid);
    void Unbind(sint32 peerid);
    void Update(id_server server, sint32 peerid);
    void UpdateAll(id_server server, sint32 excluded_peerid = -1);

protected:
    virtual String BuildPacket() const = 0;

private:
    sint32s mPeerIDs;
};

////////////////////////////////////////////////////////////////////////////////
// ZDProfile
class ZDProfile : public ZDSubscribe
{
public:
    ZDProfile();
    ~ZDProfile() override;

private:
    String BuildPacket() const override;

public:
    String mAuthor;
    String mPassword;
    Context mUserData;
};

////////////////////////////////////////////////////////////////////////////////
// ZDProgram
class ZDProgram
{
public:
    static String CreateTokenCode(chars deviceid);

public:
    MapStrings mFastLogin; // [deviceid/ClInavrmjQ] → author
    ZDProfiles mProfiles; // [author/BonexGoo] → profile
};

////////////////////////////////////////////////////////////////////////////////
// ZDToken
class ZDToken
{
public:
    void UpdateExpiry();
    bool HasExpiry(uint64 now);

public:
    String mProgramID; // ZayPro
    String mAuthor; // BonexGoo
    String mDeviceID; // ClInavrmjQ

private:
    uint64 mExpiryMsec {0}; // 만료시각
};
