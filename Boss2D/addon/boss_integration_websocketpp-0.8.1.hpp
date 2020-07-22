#pragma once
#include <boss.h>

#define UNICODE
#define _UNICODE
#include <addon/boss_fakewin.h>
#define BOSS_OPENSSL_DETOUR
#include <addon/boss_integration_openssl-1.1.1a.h>

#include <boss.hpp>

namespace BOSS
{
    class WebSocketPrivate
    {
        BOSS_DECLARE_NONCOPYABLE_CLASS(WebSocketPrivate)

    public:
        WebSocketPrivate();
        ~WebSocketPrivate();

    public:
        typedef void (*DestroyCB)(void* me);

    public:
        bool Connect(chars url);
        bool IsConnected() const;
        void DestroyMe(DestroyCB cb);
        void SendString(chars text);
        void SendBinary(bytes data, sint32 len);
        sint32 GetRecvCount() const;
        const String* RecvStringOnce();

    public:
        enum State {State_Null, State_Connecting, State_Connected, State_Disconnected};
        class SendData
        {
        public:
            SendData(chars text)
            {
                mText = new String(text);
                mData = nullptr;
            }
            SendData(bytes data, sint32 len)
            {
                mText = nullptr;
                mData = new uint08s();
                Memory::Copy(mData->AtDumpingAdded(len), data, len);
            }
            ~SendData() {delete mText; delete mData;}
        public:
            String* mText;
            uint08s* mData;
        };

    public:
        State mState;
        bool mIsTls;
        void* mClient;
        DestroyCB mDestroyCB;
        Queue<const SendData*> mSendQueue;
        Queue<const String*> mRecvQueue;
        String mTempForRecv;
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// _BOSS_BATCH_COMMAND_ {"type" : "include-alias", "prm" : "BOSS_WEBSOCKET_", "restore-comment" : " //original-code:"}
#define BOSS_WEBSOCKET_V_websocketpp__client_hpp                                       "addon/websocketpp-0.8.1_for_boss/websocketpp/client.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__close_hpp                                        "addon/websocketpp-0.8.1_for_boss/websocketpp/close.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__connection_hpp                                   "addon/websocketpp-0.8.1_for_boss/websocketpp/connection.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__connection_base_hpp                              "addon/websocketpp-0.8.1_for_boss/websocketpp/connection_base.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__endpoint_hpp                                     "addon/websocketpp-0.8.1_for_boss/websocketpp/endpoint.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__endpoint_base_hpp                                "addon/websocketpp-0.8.1_for_boss/websocketpp/endpoint_base.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__error_hpp                                        "addon/websocketpp-0.8.1_for_boss/websocketpp/error.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__frame_hpp                                        "addon/websocketpp-0.8.1_for_boss/websocketpp/frame.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__server_hpp                                       "addon/websocketpp-0.8.1_for_boss/websocketpp/server.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__uri_hpp                                          "addon/websocketpp-0.8.1_for_boss/websocketpp/uri.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__utf8_validator_hpp                               "addon/websocketpp-0.8.1_for_boss/websocketpp/utf8_validator.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__utilities_hpp                                    "addon/websocketpp-0.8.1_for_boss/websocketpp/utilities.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__version_hpp                                      "addon/websocketpp-0.8.1_for_boss/websocketpp/version.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__base64__base64_hpp                               "addon/websocketpp-0.8.1_for_boss/websocketpp/base64/base64.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__asio_hpp                                 "addon/websocketpp-0.8.1_for_boss/websocketpp/common/asio.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__asio_ssl_hpp                             "addon/websocketpp-0.8.1_for_boss/websocketpp/common/asio_ssl.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__chrono_hpp                               "addon/websocketpp-0.8.1_for_boss/websocketpp/common/chrono.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__connection_hdl_hpp                       "addon/websocketpp-0.8.1_for_boss/websocketpp/common/connection_hdl.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__cpp11_hpp                                "addon/websocketpp-0.8.1_for_boss/websocketpp/common/cpp11.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__functional_hpp                           "addon/websocketpp-0.8.1_for_boss/websocketpp/common/functional.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__md5_hpp                                  "addon/websocketpp-0.8.1_for_boss/websocketpp/common/md5.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__memory_hpp                               "addon/websocketpp-0.8.1_for_boss/websocketpp/common/memory.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__network_hpp                              "addon/websocketpp-0.8.1_for_boss/websocketpp/common/network.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__platforms_hpp                            "addon/websocketpp-0.8.1_for_boss/websocketpp/common/platforms.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__random_hpp                               "addon/websocketpp-0.8.1_for_boss/websocketpp/common/random.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__regex_hpp                                "addon/websocketpp-0.8.1_for_boss/websocketpp/common/regex.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__stdint_hpp                               "addon/websocketpp-0.8.1_for_boss/websocketpp/common/stdint.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__system_error_hpp                         "addon/websocketpp-0.8.1_for_boss/websocketpp/common/system_error.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__thread_hpp                               "addon/websocketpp-0.8.1_for_boss/websocketpp/common/thread.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__time_hpp                                 "addon/websocketpp-0.8.1_for_boss/websocketpp/common/time.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__common__type_traits_hpp                          "addon/websocketpp-0.8.1_for_boss/websocketpp/common/type_traits.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__concurrency__basic_hpp                           "addon/websocketpp-0.8.1_for_boss/websocketpp/concurrency/basic.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__concurrency__none_hpp                            "addon/websocketpp-0.8.1_for_boss/websocketpp/concurrency/none.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__asio_hpp                                 "addon/websocketpp-0.8.1_for_boss/websocketpp/config/asio.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__asio_client_hpp                          "addon/websocketpp-0.8.1_for_boss/websocketpp/config/asio_client.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__asio_no_tls_hpp                          "addon/websocketpp-0.8.1_for_boss/websocketpp/config/asio_no_tls.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__asio_no_tls_client_hpp                   "addon/websocketpp-0.8.1_for_boss/websocketpp/config/asio_no_tls_client.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__boost_config_hpp                         "addon/websocketpp-0.8.1_for_boss/websocketpp/config/boost_config.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__core_hpp                                 "addon/websocketpp-0.8.1_for_boss/websocketpp/config/core.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__core_client_hpp                          "addon/websocketpp-0.8.1_for_boss/websocketpp/config/core_client.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__debug_hpp                                "addon/websocketpp-0.8.1_for_boss/websocketpp/config/debug.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__debug_asio_hpp                           "addon/websocketpp-0.8.1_for_boss/websocketpp/config/debug_asio.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__debug_asio_no_tls_hpp                    "addon/websocketpp-0.8.1_for_boss/websocketpp/config/debug_asio_no_tls.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__minimal_client_hpp                       "addon/websocketpp-0.8.1_for_boss/websocketpp/config/minimal_client.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__config__minimal_server_hpp                       "addon/websocketpp-0.8.1_for_boss/websocketpp/config/minimal_server.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__extensions__extension_hpp                        "addon/websocketpp-0.8.1_for_boss/websocketpp/extensions/extension.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__extensions__permessage_deflate__disabled_hpp     "addon/websocketpp-0.8.1_for_boss/websocketpp/extensions/permessage_deflate/disabled.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__extensions__permessage_deflate__enabled_hpp      "addon/websocketpp-0.8.1_for_boss/websocketpp/extensions/permessage_deflate/enabled.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__http__constants_hpp                              "addon/websocketpp-0.8.1_for_boss/websocketpp/http/constants.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__http__parser_hpp                                 "addon/websocketpp-0.8.1_for_boss/websocketpp/http/parser.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__http__request_hpp                                "addon/websocketpp-0.8.1_for_boss/websocketpp/http/request.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__http__response_hpp                               "addon/websocketpp-0.8.1_for_boss/websocketpp/http/response.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__http__impl__parser_hpp                           "addon/websocketpp-0.8.1_for_boss/websocketpp/http/impl/parser.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__http__impl__request_hpp                          "addon/websocketpp-0.8.1_for_boss/websocketpp/http/impl/request.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__http__impl__response_hpp                         "addon/websocketpp-0.8.1_for_boss/websocketpp/http/impl/response.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__impl__connection_impl_hpp                        "addon/websocketpp-0.8.1_for_boss/websocketpp/impl/connection_impl.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__impl__endpoint_impl_hpp                          "addon/websocketpp-0.8.1_for_boss/websocketpp/impl/endpoint_impl.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__impl__utilities_impl_hpp                         "addon/websocketpp-0.8.1_for_boss/websocketpp/impl/utilities_impl.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__logger__basic_hpp                                "addon/websocketpp-0.8.1_for_boss/websocketpp/logger/basic.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__logger__levels_hpp                               "addon/websocketpp-0.8.1_for_boss/websocketpp/logger/levels.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__logger__stub_hpp                                 "addon/websocketpp-0.8.1_for_boss/websocketpp/logger/stub.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__logger__syslog_hpp                               "addon/websocketpp-0.8.1_for_boss/websocketpp/logger/syslog.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__message_buffer__alloc_hpp                        "addon/websocketpp-0.8.1_for_boss/websocketpp/message_buffer/alloc.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__message_buffer__message_hpp                      "addon/websocketpp-0.8.1_for_boss/websocketpp/message_buffer/message.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__message_buffer__pool_hpp                         "addon/websocketpp-0.8.1_for_boss/websocketpp/message_buffer/pool.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__processors__base_hpp                             "addon/websocketpp-0.8.1_for_boss/websocketpp/processors/base.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__processors__hybi00_hpp                           "addon/websocketpp-0.8.1_for_boss/websocketpp/processors/hybi00.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__processors__hybi07_hpp                           "addon/websocketpp-0.8.1_for_boss/websocketpp/processors/hybi07.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__processors__hybi08_hpp                           "addon/websocketpp-0.8.1_for_boss/websocketpp/processors/hybi08.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__processors__hybi13_hpp                           "addon/websocketpp-0.8.1_for_boss/websocketpp/processors/hybi13.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__processors__processor_hpp                        "addon/websocketpp-0.8.1_for_boss/websocketpp/processors/processor.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__random__none_hpp                                 "addon/websocketpp-0.8.1_for_boss/websocketpp/random/none.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__random__random_device_hpp                        "addon/websocketpp-0.8.1_for_boss/websocketpp/random/random_device.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__roles__client_endpoint_hpp                       "addon/websocketpp-0.8.1_for_boss/websocketpp/roles/client_endpoint.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__roles__server_endpoint_hpp                       "addon/websocketpp-0.8.1_for_boss/websocketpp/roles/server_endpoint.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__sha1__sha1_hpp                                   "addon/websocketpp-0.8.1_for_boss/websocketpp/sha1/sha1.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__asio__base_hpp                        "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/asio/base.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__asio__connection_hpp                  "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/asio/connection.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__asio__endpoint_hpp                    "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/asio/endpoint.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__asio__security__base_hpp              "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/asio/security/base.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__asio__security__none_hpp              "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/asio/security/none.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__asio__security__tls_hpp               "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/asio/security/tls.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__base__connection_hpp                  "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/base/connection.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__base__endpoint_hpp                    "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/base/endpoint.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__debug__base_hpp                       "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/debug/base.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__debug__connection_hpp                 "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/debug/connection.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__debug__endpoint_hpp                   "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/debug/endpoint.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__iostream__base_hpp                    "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/iostream/base.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__iostream__connection_hpp              "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/iostream/connection.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__iostream__endpoint_hpp                "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/iostream/endpoint.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__stub__base_hpp                        "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/stub/base.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__stub__connection_hpp                  "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/stub/connection.hpp"
#define BOSS_WEBSOCKET_V_websocketpp__transport__stub__endpoint_hpp                    "addon/websocketpp-0.8.1_for_boss/websocketpp/transport/stub/endpoint.hpp"
