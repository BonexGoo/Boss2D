#include <boss.h>

#if !defined(BOSS_NEED_ADDON_WEBSOCKET) || (BOSS_NEED_ADDON_WEBSOCKET != 0 && BOSS_NEED_ADDON_WEBSOCKET != 1)
    #error BOSS_NEED_ADDON_WEBSOCKET macro is invalid use
#endif
bool __LINK_ADDON_WEBSOCKET__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_WEBSOCKET

#include "boss_addon_websocket.hpp"

#include <boss.hpp>
#include <platform/boss_platform.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(WebSocket, Create, id_websocket, chars)
    BOSS_DECLARE_ADDON_FUNCTION(WebSocket, Release, void, id_websocket)
    BOSS_DECLARE_ADDON_FUNCTION(WebSocket, IsConnected, bool, id_websocket)
    BOSS_DECLARE_ADDON_FUNCTION(WebSocket, SendString, void, id_websocket, chars)
    BOSS_DECLARE_ADDON_FUNCTION(WebSocket, SendBinary, void, id_websocket, bytes, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(WebSocket, GetRecvCount, sint32, id_websocket)
    BOSS_DECLARE_ADDON_FUNCTION(WebSocket, RecvStringOnce, chars, id_websocket)

    static autorun Bind_AddOn_WebSocket()
    {
        Core_AddOn_WebSocket_Create() = Customized_AddOn_WebSocket_Create;
        Core_AddOn_WebSocket_Release() = Customized_AddOn_WebSocket_Release;
        Core_AddOn_WebSocket_IsConnected() = Customized_AddOn_WebSocket_IsConnected;
        Core_AddOn_WebSocket_SendString() = Customized_AddOn_WebSocket_SendString;
        Core_AddOn_WebSocket_SendBinary() = Customized_AddOn_WebSocket_SendBinary;
        Core_AddOn_WebSocket_GetRecvCount() = Customized_AddOn_WebSocket_GetRecvCount;
        Core_AddOn_WebSocket_RecvStringOnce() = Customized_AddOn_WebSocket_RecvStringOnce;
        return true;
    }
    static autorun _ = Bind_AddOn_WebSocket();
}

// 구현부
namespace BOSS
{
    id_websocket Customized_AddOn_WebSocket_Create(chars url)
    {
        auto NewWebSocket = Buffer::Alloc<WebSocketPrivate>(BOSS_DBG 1);
        if(((WebSocketPrivate*) NewWebSocket)->Connect(url))
            return (id_websocket) NewWebSocket;

        Buffer::Free((buffer) NewWebSocket);
        return (id_websocket) nullptr;
    }

    void Customized_AddOn_WebSocket_Release(id_websocket websocket)
    {
        if(websocket)
            ((WebSocketPrivate*) websocket)->DestroyMe([](void* me)
                {Buffer::Free((buffer) me);});
    }

    bool Customized_AddOn_WebSocket_IsConnected(id_websocket websocket)
    {
        if(!websocket) return false;
        auto CurWebSocket = (WebSocketPrivate*) websocket;
        return CurWebSocket->IsConnected();
    }

    void Customized_AddOn_WebSocket_SendString(id_websocket websocket, chars text)
    {
        if(websocket)
        {
            auto CurWebSocket = (WebSocketPrivate*) websocket;
            CurWebSocket->SendString(text);
        }
    }

    void Customized_AddOn_WebSocket_SendBinary(id_websocket websocket, bytes data, sint32 len)
    {
        if(websocket)
        {
            auto CurWebSocket = (WebSocketPrivate*) websocket;
            CurWebSocket->SendBinary(data, len);
        }
    }

    sint32 Customized_AddOn_WebSocket_GetRecvCount(id_websocket websocket)
    {
        if(!websocket) return 0;
        auto CurWebSocket = (WebSocketPrivate*) websocket;
        return CurWebSocket->GetRecvCount();
    }

    chars Customized_AddOn_WebSocket_RecvStringOnce(id_websocket websocket)
    {
        if(!websocket) return nullptr;
        auto CurWebSocket = (WebSocketPrivate*) websocket;
        const String* LastString = CurWebSocket->RecvStringOnce();
        if(LastString)
        {
            CurWebSocket->mTempForRecv = *LastString;
            delete LastString;
            return (chars) CurWebSocket->mTempForRecv;
        }
        return nullptr;
    }
}

#endif
