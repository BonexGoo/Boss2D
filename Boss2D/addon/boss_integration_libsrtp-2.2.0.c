#include <boss.h>
#if BOSS_NEED_ADDON_WEBRTC

#include "boss_integration_libsrtp-2.2.0.h"

#include <addon/libsrtp-2.2.0_for_boss/crypto/include/err.h>
#include <addon/libsrtp-2.2.0_for_boss/crypto/include/cipher_types.h>

#include <addon/libsrtp-2.2.0_for_boss/srtp/ekt.c>
#include <addon/libsrtp-2.2.0_for_boss/srtp/srtp.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/cipher/aes.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/cipher/aes_gcm_ossl.c>
//#include <addon/libsrtp-2.2.0_for_boss/crypto/cipher/aes_icm.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/cipher/aes_icm_ossl.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/cipher/cipher.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/cipher/null_cipher.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/hash/auth.c>
//#include <addon/libsrtp-2.2.0_for_boss/crypto/hash/hmac.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/hash/hmac_ossl.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/hash/null_auth.c>
//#include <addon/libsrtp-2.2.0_for_boss/crypto/hash/sha1.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/kernel/alloc.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/kernel/crypto_kernel.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/kernel/err.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/kernel/key.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/math/datatypes.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/math/stat.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/replay/rdb.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/replay/rdbx.c>
#include <addon/libsrtp-2.2.0_for_boss/crypto/replay/ut_sim.c>

#endif
