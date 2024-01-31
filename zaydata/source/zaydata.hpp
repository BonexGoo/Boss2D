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
    void CheckToken(sint32 peerid, chars token);

public:
    void ServerStart();
    bool OnPacketOnce();
    void OnRecv_Login(sint32 peerid, const Context& json);
    void OnRecv_LoginUpdate(sint32 peerid, const Context& json);
    void OnRecv_Logout(sint32 peerid, const Context& json);
    void OnRecv_FocusProfile(sint32 peerid, const Context& json);
    void OnRecv_UnfocusProfile(sint32 peerid, const Context& json);
    void OnRecv_LockData(sint32 peerid, const Context& json);
    void OnRecv_UnlockData(sint32 peerid, const Context& json);
    void OnRecv_FocusData(sint32 peerid, const Context& json);
    void OnRecv_UnfocusData(sint32 peerid, const Context& json);
    void OnRecv_EnumData(sint32 peerid, const Context& json);
    void SendPacket(sint32 peerid, const Context& json);
    void SendError(sint32 peerid, const Context& json, chars text);

public:
    sint32 mPort;
    bool mUseWSS;
    id_server mServer;
    id_mutex mPacketMutex;
    MapStrings mPeerTokens; // [peerid/33]
    ZDTokens mTokens; // [token/ClInavrmjQ357437]
    ZDPrograms mPrograms; // [programid/ZayPro]
};
