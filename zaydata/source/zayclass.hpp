#pragma once
#include <boss.hpp>

////////////////////////////////////////////////////////////////////////////////
// ZDRoute
class ZDRoute
{
public:
    String mPath; // board/post/33
    String mNormal; // board.post<33> → <first>/<last>/<prev>/<next>는 불가
};
typedef Map<ZDRoute> ZDRoutes;

////////////////////////////////////////////////////////////////////////////////
// ZDFocusable
class ZDFocusable
{
public:
    ZDFocusable();
    virtual ~ZDFocusable();

public:
    void Bind(sint32 peerid);
    void Unbind(sint32 peerid);
    void SendPacket(id_server server, sint32 peerid);
    void SendPacketAll(id_server server);

protected:
    virtual void SaveFile(chars programid) const = 0;
    virtual String BuildPacket() const = 0;
    virtual String DataDir(chars programid) const = 0;

public:
    static String MakeProfileDir(chars programid, chars author);
    static String MakeAssetDir(chars programid, chars path);

protected:
    sint32s mPeerIDs;
};

////////////////////////////////////////////////////////////////////////////////
// ZDRange
class ZDRange : public ZDFocusable
{
public:
    ZDRange();
    ~ZDRange() override;

public:
    void SaveFile(chars programid) const override;

private:
    String BuildPacket() const override;
    String DataDir(chars programid) const override;

public:
    ZDRoute mRoute;
    sint32 mFirst {1};
    sint32 mLast {-1};
};
typedef Map<ZDRange> ZDRanges;

////////////////////////////////////////////////////////////////////////////////
// ZDRecordable
class ZDRecordable : public ZDFocusable
{
public:
    ZDRecordable();
    ~ZDRecordable() override;

public:
    void VersionUp(chars programid, id_server server, sint32 peerid);

public:
    String mAuthor; // BonexGoo
    String mVersion; // w1_t20240129T102834Z_a210034020003
    Context mData;
};

////////////////////////////////////////////////////////////////////////////////
// ZDProfile
class ZDProfile : public ZDRecordable
{
public:
    ZDProfile();
    ~ZDProfile() override;

public:
    void ValidStatus(id_server server, bool entered);
    void SaveFile(chars programid) const override;

private:
    String BuildPacket() const override;
    String DataDir(chars programid) const override;

public:
    bool mEntered {false};
    String mPassword;
    sint32 mWritten {0};
    sint32 mLike {0};
};
typedef Map<ZDProfile> ZDProfiles;

////////////////////////////////////////////////////////////////////////////////
// ZDAsset
class ZDAsset : public ZDRecordable
{
public:
    ZDAsset();
    ~ZDAsset() override;

public:
    bool Locking(id_server server, chars author);
    void SaveFile(chars programid) const override;

private:
    String BuildPacket() const override;
    String DataDir(chars programid) const override;

public:
    bool mLocked {false};
    ZDRoute mRoute;
};
typedef Map<ZDAsset> ZDAssets;

////////////////////////////////////////////////////////////////////////////////
// ZDProgram
class ZDProgram
{
public:
    static String CreateTokenCode(chars deviceid);
    static String CreateTimeTag();

public:
    ZDProfile* GetProfile(chars programid, chars author, bool entering);
    ZDAsset* GetAsset(chars programid, const ZDRoute& route);
    ZDRange& ValidRange(chars programid, const ZDRoute& route);
    ZDRoute ParseRoute(chars programid, chars route, id_server writable_server = nullptr);

public:
    Map<String> mFastLogin; // [deviceid:ClInavrmjQ] → author
    ZDProfiles mProfiles; // [author:BonexGoo] → profile
    ZDAssets mAssets; // [path:board/post/33] → asset
private:
    ZDRanges mRanges; // [path:board/post/33] → peers
};
typedef Map<ZDProgram> ZDPrograms;

////////////////////////////////////////////////////////////////////////////////
// ZDToken
class ZDToken
{
public:
    void UpdateExpiry();
    bool HasExpiry(uint64 now);
    void UploadOnce(chars path, sint32 total, sint32 offset, sint32 size, bytes data);
    bool UploadFlush(chars path);

public:
    String mProgramID; // ZayPro
    String mAuthor; // BonexGoo
    String mDeviceID; // ClInavrmjQ
    ZDRoutes mLockedRoutes; // [lockid:123456] → route

private:
    uint64 mExpiryMsec {0}; // 만료시각
    Map<uint08s> mUploadFiles; // [path:board/post/33/python/a.py] → FileData
};
typedef Map<ZDToken> ZDTokens;
