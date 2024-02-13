#pragma once
#include <service/boss_zay.hpp>
#include "zayclass.hpp"

class zaydataData : public ZayObject
{
public:
    zaydataData();
    ~zaydataData();

public:
    sint32 RenderValue(ZayPanel& panel, chars name, sint32 value);
    ZDToken* ValidToken(sint32 peerid, chars token);

public:
    void ServerStart();
    bool OnPacketOnce();
    void OnRecv_Login(sint32 peerid, const Context& json);
    void OnRecv_LoginUpdate(sint32 peerid, const Context& json);
    void OnRecv_Logout(sint32 peerid, const Context& json);
    void OnRecv_FocusProfile(sint32 peerid, const Context& json);
    void OnRecv_UnfocusProfile(sint32 peerid, const Context& json);
    void OnRecv_LockAsset(sint32 peerid, const Context& json);
    void OnRecv_UnlockAsset(sint32 peerid, const Context& json);
    void OnRecv_FocusAsset(sint32 peerid, const Context& json);
    void OnRecv_UnfocusAsset(sint32 peerid, const Context& json);
    void OnRecv_FocusRange(sint32 peerid, const Context& json);
    void OnRecv_UnfocusRange(sint32 peerid, const Context& json);
    void SendPacket(sint32 peerid, const Context& json);
    void SendError(sint32 peerid, const Context& json, chars text);

public:
    sint32 mPort;
    bool mUseWSS;
    id_server mServer;
    id_mutex mPacketMutex;
    Map<String> mPeerTokens; // [peerid:33]
    ZDTokens mTokens; // [token:ClInavrmjQ357437]
    ZDPrograms mPrograms; // [programid:ZayPro]
};
