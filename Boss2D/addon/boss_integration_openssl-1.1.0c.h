#pragma once
#include <addon/boss_fakewin.h>

#if BOSS_X64
    #define SIXTY_FOUR_BIT
    //#define SIXTY_FOUR_BIT_LONG
#else
    #define THIRTY_TWO_BIT
#endif

#if !BOSS_NDEBUG
    #define LEVITTE_DEBUG_MEM
#endif

#if BOSS_WINDOWS & !BOSS_WINDOWS_MINGW
    typedef __int64 i64;
    typedef unsigned __int64 u64;
    #define SHA_LONG64 unsigned __int64
    #define U64(C)     C##UI64
#elif defined(__arch64__)
    typedef long i64;
    typedef unsigned long u64;
    #define SHA_LONG64 unsigned long
    #define U64(C)     C##UL
#else
    typedef long long i64;
    typedef unsigned long long u64;
    #define SHA_LONG64 unsigned long long
    #define U64(C)     C##ULL
#endif

#define _WINDEF_H

#define OPENSSL_NO_ASM // No assembly for iphone-simulator
#define OPENSSL_NO_MD2
#define OPENSSL_NO_MD4
//#define OPENSSL_NO_MD5
#define OPENSSL_NO_MDC2
//#define OPENSSL_NO_RSA
//#define OPENSSL_NO_DSA
#define OPENSSL_NO_RIPEMD
//#define OPENSSL_NO_AES
#define OPENSSL_NO_BF
#define OPENSSL_NO_RC2
//#define OPENSSL_NO_RC4
#define OPENSSL_NO_RC5
#define OPENSSL_NO_CAST
//#define OPENSSL_NO_DES
#define OPENSSL_NO_ENGINE
//#define OPENSSL_NO_DH
#define OPENSSL_NO_CMS
#define OPENSSL_NO_CT
#define OPENSSL_NO_IDEA
#define OPENSSL_NO_SEED
#define OPENSSL_NO_CAMELLIA
#define OPENSSL_NO_EC_NISTP_64_GCC_128
#define OPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE
#define OPENSSL_NO_SCTP
#define OPENSSL_USE_DEPRECATED
#define OPENSSL_SMALL_FOOTPRINT
#define NO_WINDOWS_BRAINDEATH
#define OPENSSL_USE_NODELETE

#define DECLARE_DEPRECATED(...)

#define OPENSSLDIR "demoCA"
#define RC4_INT int

////////////////////////////////////////////////////////////////////////////////////////////////////
// _BOSS_BATCH_COMMAND_ {"type" : "include-alias", "prm" : "BOSS_OPENSSL_", "restore-comment" : " //original-code:"}
#define BOSS_OPENSSL_V_openssl__ssl_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/ssl.h>
#define BOSS_OPENSSL_V_openssl__engine_h              <addon/openssl-1.1.0c_for_boss/include/openssl/engine.h>
#define BOSS_OPENSSL_V_openssl__rsa_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/rsa.h>
#define BOSS_OPENSSL_V_openssl__dsa_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/dsa.h>
#define BOSS_OPENSSL_V_openssl__aes_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/aes.h>
#define BOSS_OPENSSL_V_openssl__des_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/des.h>
#define BOSS_OPENSSL_V_openssl__rc4_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/rc4.h>
#define BOSS_OPENSSL_V_openssl__cms_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/cms.h>
#define BOSS_OPENSSL_V_openssl__ssl2_h                <addon/openssl-1.1.0c_for_boss/include/openssl/ssl2.h>
#define BOSS_OPENSSL_V_openssl__ssl3_h                <addon/openssl-1.1.0c_for_boss/include/openssl/ssl3.h>
#define BOSS_OPENSSL_V_openssl__dh_h                  <addon/openssl-1.1.0c_for_boss/include/openssl/dh.h>
#define BOSS_OPENSSL_V_openssl__tls1_h                <addon/openssl-1.1.0c_for_boss/include/openssl/tls1.h>
#define BOSS_OPENSSL_V_openssl__dtls1_h               <addon/openssl-1.1.0c_for_boss/include/openssl/dtls1.h>
#define BOSS_OPENSSL_V_openssl__srtp_h                <addon/openssl-1.1.0c_for_boss/include/openssl/srtp.h>
#define BOSS_OPENSSL_V_openssl__comp_h                <addon/openssl-1.1.0c_for_boss/include/openssl/comp.h>
#define BOSS_OPENSSL_V_openssl__crypto_h              <addon/openssl-1.1.0c_for_boss/include/openssl/crypto.h>
#define BOSS_OPENSSL_V_openssl__e_os2_h               <addon/openssl-1.1.0c_for_boss/include/openssl/e_os2.h>
#define BOSS_OPENSSL_V_openssl__opensslconf_h         <addon/openssl-1.1.0c_for_boss/include/openssl/opensslconf.h>
#define BOSS_OPENSSL_V_openssl__stack_h               <addon/openssl-1.1.0c_for_boss/include/openssl/stack.h>
#define BOSS_OPENSSL_V_openssl__safestack_h           <addon/openssl-1.1.0c_for_boss/include/openssl/safestack.h>
#define BOSS_OPENSSL_V_openssl__opensslv_h            <addon/openssl-1.1.0c_for_boss/include/openssl/opensslv.h>
#define BOSS_OPENSSL_V_openssl__ossl_typ_h            <addon/openssl-1.1.0c_for_boss/include/openssl/ossl_typ.h>
#define BOSS_OPENSSL_V_openssl__symhacks_h            <addon/openssl-1.1.0c_for_boss/include/openssl/symhacks.h>
#define BOSS_OPENSSL_V_openssl__buffer_h              <addon/openssl-1.1.0c_for_boss/include/openssl/buffer.h>
#define BOSS_OPENSSL_V_openssl__bio_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/bio.h>
#define BOSS_OPENSSL_V_openssl__err_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/err.h>
#define BOSS_OPENSSL_V_openssl__lhash_h               <addon/openssl-1.1.0c_for_boss/include/openssl/lhash.h>
#define BOSS_OPENSSL_V_openssl__bn_h                  <addon/openssl-1.1.0c_for_boss/include/openssl/bn.h>
#define BOSS_OPENSSL_V_openssl__asn1_h                <addon/openssl-1.1.0c_for_boss/include/openssl/asn1.h>
#define BOSS_OPENSSL_V_openssl__rand_h                <addon/openssl-1.1.0c_for_boss/include/openssl/rand.h>
#define BOSS_OPENSSL_V_openssl__evp_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/evp.h>
#define BOSS_OPENSSL_V_openssl__objects_h             <addon/openssl-1.1.0c_for_boss/include/openssl/objects.h>
#define BOSS_OPENSSL_V_openssl__obj_mac_h             <addon/openssl-1.1.0c_for_boss/include/openssl/obj_mac.h>
#define BOSS_OPENSSL_V_openssl__x509_h                <addon/openssl-1.1.0c_for_boss/include/openssl/x509.h>
#define BOSS_OPENSSL_V_openssl__x509_vfy_h            <addon/openssl-1.1.0c_for_boss/include/openssl/x509_vfy.h>
#define BOSS_OPENSSL_V_openssl__ec_h                  <addon/openssl-1.1.0c_for_boss/include/openssl/ec.h>
#define BOSS_OPENSSL_V_openssl__ecdsa_h               <addon/openssl-1.1.0c_for_boss/include/openssl/ecdsa.h>
#define BOSS_OPENSSL_V_openssl__ecdh_h                <addon/openssl-1.1.0c_for_boss/include/openssl/ecdh.h>
#define BOSS_OPENSSL_V_openssl__sha_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/sha.h>
#define BOSS_OPENSSL_V_openssl__pkcs7_h               <addon/openssl-1.1.0c_for_boss/include/openssl/pkcs7.h>
#define BOSS_OPENSSL_V_openssl__pem_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/pem.h>
#define BOSS_OPENSSL_V_openssl__pem2_h                <addon/openssl-1.1.0c_for_boss/include/openssl/pem2.h>
#define BOSS_OPENSSL_V_openssl__pkcs12_h              <addon/openssl-1.1.0c_for_boss/include/openssl/pkcs12.h>
#define BOSS_OPENSSL_V_openssl__asn1t_h               <addon/openssl-1.1.0c_for_boss/include/openssl/asn1t.h>
#define BOSS_OPENSSL_V_openssl__ocsp_h                <addon/openssl-1.1.0c_for_boss/include/openssl/ocsp.h>
#define BOSS_OPENSSL_V_openssl__x509v3_h              <addon/openssl-1.1.0c_for_boss/include/openssl/x509v3.h>
#define BOSS_OPENSSL_V_openssl__conf_h                <addon/openssl-1.1.0c_for_boss/include/openssl/conf.h>
#define BOSS_OPENSSL_V_openssl__conf_api_h            <addon/openssl-1.1.0c_for_boss/include/openssl/conf_api.h>
#define BOSS_OPENSSL_V_openssl__dso_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/dso.h>
#define BOSS_OPENSSL_V_openssl__md5_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/md5.h>
#define BOSS_OPENSSL_V_openssl__ui_h                  <addon/openssl-1.1.0c_for_boss/include/openssl/ui.h>
#define BOSS_OPENSSL_V_openssl__hmac_h                <addon/openssl-1.1.0c_for_boss/include/openssl/hmac.h>
#define BOSS_OPENSSL_V_openssl__cmac_h                <addon/openssl-1.1.0c_for_boss/include/openssl/cmac.h>
#define BOSS_OPENSSL_V_openssl__ripemd_h              <addon/openssl-1.1.0c_for_boss/include/openssl/ripemd.h>
#define BOSS_OPENSSL_V_openssl__whrlpool_h            <addon/openssl-1.1.0c_for_boss/include/openssl/whrlpool.h>
#define BOSS_OPENSSL_V_openssl__modes_h               <addon/openssl-1.1.0c_for_boss/include/openssl/modes.h>
#define BOSS_OPENSSL_V_openssl__async_h               <addon/openssl-1.1.0c_for_boss/include/openssl/async.h>
#define BOSS_OPENSSL_V_openssl__ct_h                  <addon/openssl-1.1.0c_for_boss/include/openssl/ct.h>
#define BOSS_OPENSSL_V_openssl__kdf_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/kdf.h>
#define BOSS_OPENSSL_V_openssl__srp_h                 <addon/openssl-1.1.0c_for_boss/include/openssl/srp.h>
#define BOSS_OPENSSL_V_openssl__ts_h                  <addon/openssl-1.1.0c_for_boss/include/openssl/ts.h>
#define BOSS_OPENSSL_V_openssl__txt_db_h              <addon/openssl-1.1.0c_for_boss/include/openssl/txt_db.h>
#define BOSS_OPENSSL_V_e_os_h                         <addon/openssl-1.1.0c_for_boss/e_os.h>
#define BOSS_OPENSSL_V_internal__cryptlib_h           <addon/openssl-1.1.0c_for_boss/crypto/include/internal/cryptlib.h>
#define BOSS_OPENSSL_V_internal__cryptlib_int_h       <addon/openssl-1.1.0c_for_boss/crypto/include/internal/cryptlib_int.h>
#define BOSS_OPENSSL_V_internal__err_int_h            <addon/openssl-1.1.0c_for_boss/crypto/include/internal/err_int.h>
#define BOSS_OPENSSL_V_internal__x509_int_h           <addon/openssl-1.1.0c_for_boss/crypto/include/internal/x509_int.h>
#define BOSS_OPENSSL_V_internal__objects_h            <addon/openssl-1.1.0c_for_boss/crypto/include/internal/objects.h>
#define BOSS_OPENSSL_V_internal__evp_int_h            <addon/openssl-1.1.0c_for_boss/crypto/include/internal/evp_int.h>
#define BOSS_OPENSSL_V_internal__async_h              <addon/openssl-1.1.0c_for_boss/crypto/include/internal/async.h>
#define BOSS_OPENSSL_V_internal__rand_h               <addon/openssl-1.1.0c_for_boss/crypto/include/internal/rand.h>
#define BOSS_OPENSSL_V_internal__engine_h             <addon/openssl-1.1.0c_for_boss/crypto/include/internal/engine.h>
#define BOSS_OPENSSL_V_internal__bn_srp_h             <addon/openssl-1.1.0c_for_boss/crypto/include/internal/bn_srp.h>
#define BOSS_OPENSSL_V_internal__numbers_h            <addon/openssl-1.1.0c_for_boss/include/internal/numbers.h>
#define BOSS_OPENSSL_V_internal__err_h                <addon/openssl-1.1.0c_for_boss/include/internal/err.h>
#define BOSS_OPENSSL_V_internal__thread_once_h        <addon/openssl-1.1.0c_for_boss/include/internal/thread_once.h>
#define BOSS_OPENSSL_V_internal__bio_h                <addon/openssl-1.1.0c_for_boss/include/internal/bio.h>
#define BOSS_OPENSSL_V_internal__dane_h               <addon/openssl-1.1.0c_for_boss/include/internal/dane.h>
#define BOSS_OPENSSL_V_internal__conf_h               <addon/openssl-1.1.0c_for_boss/include/internal/conf.h>
#define BOSS_OPENSSL_V_internal__comp_h               <addon/openssl-1.1.0c_for_boss/include/internal/comp.h>
#define BOSS_OPENSSL_V_internal__dso_h                <addon/openssl-1.1.0c_for_boss/include/internal/dso.h>

#define BOSS_OPENSSL_U_internal__cryptlib_h           "addon/openssl-1.1.0c_for_boss/crypto/include/internal/cryptlib.h"
#define BOSS_OPENSSL_U_internal__bn_int_h             "addon/openssl-1.1.0c_for_boss/crypto/include/internal/bn_int.h"
#define BOSS_OPENSSL_U_internal__asn1_int_h           "addon/openssl-1.1.0c_for_boss/crypto/include/internal/asn1_int.h"
#define BOSS_OPENSSL_U_internal__x509_int_h           "addon/openssl-1.1.0c_for_boss/crypto/include/internal/x509_int.h"
#define BOSS_OPENSSL_U_internal__evp_int_h            "addon/openssl-1.1.0c_for_boss/crypto/include/internal/evp_int.h"
#define BOSS_OPENSSL_U_internal__md32_common_h        "addon/openssl-1.1.0c_for_boss/crypto/include/internal/md32_common.h"
#define BOSS_OPENSSL_U_internal__ct_int_h             "addon/openssl-1.1.0c_for_boss/crypto/include/internal/ct_int.h"
#define BOSS_OPENSSL_U_internal__cryptlib_int_h       "addon/openssl-1.1.0c_for_boss/crypto/include/internal/cryptlib_int.h"
#define BOSS_OPENSSL_U_internal__objects_h            "addon/openssl-1.1.0c_for_boss/crypto/include/internal/objects.h"
#define BOSS_OPENSSL_U_internal__bn_conf_h            "addon/openssl-1.1.0c_for_boss/crypto/include/internal/bn_conf.h"
#define BOSS_OPENSSL_U_internal__rand_h               "addon/openssl-1.1.0c_for_boss/crypto/include/internal/rand.h"
#define BOSS_OPENSSL_U_internal__dso_conf_h           "addon/openssl-1.1.0c_for_boss/crypto/include/internal/dso_conf.h"
#define BOSS_OPENSSL_U_internal__bn_srp_h             "addon/openssl-1.1.0c_for_boss/crypto/include/internal/bn_srp.h"
#define BOSS_OPENSSL_U_internal__err_int_h            "addon/openssl-1.1.0c_for_boss/crypto/include/internal/err_int.h"
#define BOSS_OPENSSL_U_internal__chacha_h             "addon/openssl-1.1.0c_for_boss/crypto/include/internal/chacha.h"
#define BOSS_OPENSSL_U_internal__poly1305_h           "addon/openssl-1.1.0c_for_boss/crypto/include/internal/poly1305.h"
#define BOSS_OPENSSL_U_internal__bn_dh_h              "addon/openssl-1.1.0c_for_boss/crypto/include/internal/bn_dh.h"
#define BOSS_OPENSSL_U_internal__dane_h               "addon/openssl-1.1.0c_for_boss/include/internal/dane.h"
#define BOSS_OPENSSL_U_internal__o_dir_h              "addon/openssl-1.1.0c_for_boss/include/internal/o_dir.h"
#define BOSS_OPENSSL_U_internal__o_str_h              "addon/openssl-1.1.0c_for_boss/include/internal/o_str.h"
#define BOSS_OPENSSL_U_internal__err_h                "addon/openssl-1.1.0c_for_boss/include/internal/err.h"
#define BOSS_OPENSSL_U_internal__thread_once_h        "addon/openssl-1.1.0c_for_boss/include/internal/thread_once.h"
#define BOSS_OPENSSL_U_internal__numbers_h            "addon/openssl-1.1.0c_for_boss/include/internal/numbers.h"
#define BOSS_OPENSSL_U_internal__constant_time_locl_h "addon/openssl-1.1.0c_for_boss/include/internal/constant_time_locl.h"
#define BOSS_OPENSSL_U_internal__bio_h                "addon/openssl-1.1.0c_for_boss/include/internal/bio.h"
#define BOSS_OPENSSL_U_internal__conf_h               "addon/openssl-1.1.0c_for_boss/include/internal/conf.h"
#define BOSS_OPENSSL_U_internal__dso_h                "addon/openssl-1.1.0c_for_boss/include/internal/dso.h"
#define BOSS_OPENSSL_U_internal__comp_h               "addon/openssl-1.1.0c_for_boss/include/internal/comp.h"
#define BOSS_OPENSSL_U_e_os_h                         "addon/openssl-1.1.0c_for_boss/e_os.h"
#define BOSS_OPENSSL_U_modes_lcl_h                    "addon/openssl-1.1.0c_for_boss/crypto/modes/modes_lcl.h"
#define BOSS_OPENSSL_U_ui_locl_h                      "addon/openssl-1.1.0c_for_boss/crypto/ui/ui_locl.h"

#ifndef BOSS_OPENSSL_DETOUR
    #ifdef __cplusplus
        extern "C" {
    #endif
        #include <addon/openssl-1.1.0c_for_boss/crypto/bio/bio_lcl.h>
        #include <addon/openssl-1.1.0c_for_boss/include/openssl/evp.h>
        #include <addon/openssl-1.1.0c_for_boss/crypto/evp/evp_locl.h>
        #include <addon/openssl-1.1.0c_for_boss/include/openssl/srtp.h>
    #ifdef __cplusplus
        }
    #endif
#endif
