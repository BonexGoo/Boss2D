#include <boss.h>
#if BOSS_NEED_ADDON_GIT

#include "boss_integration_libssh2-1.6.0.h"

#if BOSS_WINDOWS
    #define HAVE_IOCTLSOCKET
    #if BOSS_NEED_FORCED_FAKEWIN
        #define HAVE_DISABLED_NONBLOCKING
    #else
        #include <ws2tcpip.h>
    #endif
#else
    #define HAVE_DISABLED_NONBLOCKING
#endif

#include BOSS_OPENSSL_V_openssl__rsa_h
#include BOSS_OPENSSL_V_openssl__md5_h
#include <addon/libssh2-1.6.0_for_boss/include/libssh2.h>

#include <addon/libssh2-1.6.0_for_boss/src/openssl.c>
#include <addon/libssh2-1.6.0_for_boss/src/session.c>
#include <addon/libssh2-1.6.0_for_boss/src/hostkey.c>
#include <addon/libssh2-1.6.0_for_boss/src/userauth.c>
#include <addon/libssh2-1.6.0_for_boss/src/channel.c>
#include <addon/libssh2-1.6.0_for_boss/src/agent.c>
#include <addon/libssh2-1.6.0_for_boss/src/transport.c>
#include <addon/libssh2-1.6.0_for_boss/src/packet.c>
#include <addon/libssh2-1.6.0_for_boss/src/global.c>
#include <addon/libssh2-1.6.0_for_boss/src/kex.c>
#include <addon/libssh2-1.6.0_for_boss/src/comp.c>
#include <addon/libssh2-1.6.0_for_boss/src/mac.c>
#include <addon/libssh2-1.6.0_for_boss/src/crypt.c>
#include <addon/libssh2-1.6.0_for_boss/src/misc.c>
#include <addon/libssh2-1.6.0_for_boss/src/keepalive.c>

#endif
