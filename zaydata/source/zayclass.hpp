#pragma once
#include <boss.hpp>

typedef Map<String> MapStrings;
typedef Map<class ZDProfile> ZDProfiles;
typedef Map<class ZDAsset> ZDAssets;
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
    void VersionUp(chars programid, id_server server, sint32 peerid);
    void SendPacket(id_server server, sint32 peerid);
    void SendPacketAll(id_server server);

protected:
    virtual void SaveFile(chars programid) const = 0;
    virtual String DataDir(chars programid) const = 0;
    virtual String BuildPacket() const = 0;

public:
    String mAuthor; // BonexGoo
    String mVersion; // w1_t20240129T102834Z_a210034020003
    Context mData;

protected:
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
    void ValidStatus(id_server server, bool entered);
    void SaveFile(chars programid) const override;

public:
    static String MakeDataDir(chars programid, chars author);

private:
    String DataDir(chars programid) const override;
    String BuildPacket() const override;

public:
    bool mEntered {false};
    String mPassword;
    sint32 mWritten {0};
    sint32 mLike {0};
};

////////////////////////////////////////////////////////////////////////////////
// ZDAsset
class ZDAsset : public ZDSubscribe
{
public:
    ZDAsset();
    ~ZDAsset() override;

public:
    bool Locking(id_server server, chars author);
    void SaveFile(chars programid) const override;

public:
    static String MakeDataDir(chars programid, chars route);

private:
    String DataDir(chars programid) const override;
    String BuildPacket() const override;

public:
    bool mLocked {false};
    String mRoute; // board.post.33, board.[first], board.[last], board.[next]
};

////////////////////////////////////////////////////////////////////////////////
// ZDProgram
class ZDProgram
{
public:
    static String CreateTokenCode(chars deviceid);
    static String CreateTimeTag();

public:
    ZDProfile* ValidProfile(chars programid, chars author, bool entering);
    String ValidAssetRoute(chars programid, chars route_requested);
    Strings EnumAssetRoutes(chars programid, chars route, sint32 maxcount, sint32& totalcount);
    ZDAsset* ValidAsset(chars programid, chars route);

public:
    MapStrings mFastLogin; // [deviceid/ClInavrmjQ] → author
    ZDProfiles mProfiles; // [author/BonexGoo] → profile
    ZDAssets mAssets; // [route/board.post.33] → asset
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
    MapStrings mLockedRoutes; // [lockid/123456] → board.post.33

private:
    uint64 mExpiryMsec {0}; // 만료시각
};
