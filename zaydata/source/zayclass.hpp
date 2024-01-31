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
    void UpdateVersion(id_server server, sint32 peerid, chars dirpath);
    void SendPacket(id_server server, sint32 peerid);
    void SendPacketAll(id_server server, sint32 excluded_peerid = -1);

protected:
    virtual String BuildPacket() const = 0;

public:
    inline chars version() const {return mVersion;}

private:
    String mVersion;
    sint32s mPeerIDs;
};

////////////////////////////////////////////////////////////////////////////////
// ZDProfile
class ZDProfile : public ZDSubscribe
{
public:
    ZDProfile();
    ~ZDProfile() override;

public:
    void SaveFile(chars dirpath) const;

private:
    String BuildPacket() const override;

public:
    String mAuthor;
    String mPassword;
    Context mData;
};

////////////////////////////////////////////////////////////////////////////////
// ZDProgram
class ZDProgram
{
public:
    static String CreateTokenCode(chars deviceid);
    static String CreateTimeCode();

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
