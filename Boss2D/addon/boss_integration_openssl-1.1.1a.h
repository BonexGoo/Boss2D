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
#define OPENSSL_RAND_SEED_OS

#define DECLARE_DEPRECATED(...)

#define OPENSSLDIR "demoCA"
#define RC4_INT int

////////////////////////////////////////////////////////////////////////////////////////////////////
// _BOSS_BATCH_COMMAND_ {"type" : "include-alias", "prm" : "BOSS_OPENSSL_", "restore-comment" : " //original-code:"}
#define BOSS_OPENSSL_V_openssl__aes_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/aes.h>
#define BOSS_OPENSSL_V_openssl__asn1_h                <addon/openssl-1.1.1a_for_boss/include/openssl/asn1.h>
#define BOSS_OPENSSL_V_openssl__asn1_mac_h            <addon/openssl-1.1.1a_for_boss/include/openssl/asn1_mac.h>
#define BOSS_OPENSSL_V_openssl__asn1err_h             <addon/openssl-1.1.1a_for_boss/include/openssl/asn1err.h>
#define BOSS_OPENSSL_V_openssl__asn1t_h               <addon/openssl-1.1.1a_for_boss/include/openssl/asn1t.h>
#define BOSS_OPENSSL_V_openssl__async_h               <addon/openssl-1.1.1a_for_boss/include/openssl/async.h>
#define BOSS_OPENSSL_V_openssl__asyncerr_h            <addon/openssl-1.1.1a_for_boss/include/openssl/asyncerr.h>
#define BOSS_OPENSSL_V_openssl__bio_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/bio.h>
#define BOSS_OPENSSL_V_openssl__bioerr_h              <addon/openssl-1.1.1a_for_boss/include/openssl/bioerr.h>
#define BOSS_OPENSSL_V_openssl__blowfish_h            <addon/openssl-1.1.1a_for_boss/include/openssl/blowfish.h>
#define BOSS_OPENSSL_V_openssl__bn_h                  <addon/openssl-1.1.1a_for_boss/include/openssl/bn.h>
#define BOSS_OPENSSL_V_openssl__bnerr_h               <addon/openssl-1.1.1a_for_boss/include/openssl/bnerr.h>
#define BOSS_OPENSSL_V_openssl__buffer_h              <addon/openssl-1.1.1a_for_boss/include/openssl/buffer.h>
#define BOSS_OPENSSL_V_openssl__buffererr_h           <addon/openssl-1.1.1a_for_boss/include/openssl/buffererr.h>
#define BOSS_OPENSSL_V_openssl__camellia_h            <addon/openssl-1.1.1a_for_boss/include/openssl/camellia.h>
#define BOSS_OPENSSL_V_openssl__cast_h                <addon/openssl-1.1.1a_for_boss/include/openssl/cast.h>
#define BOSS_OPENSSL_V_openssl__cmac_h                <addon/openssl-1.1.1a_for_boss/include/openssl/cmac.h>
#define BOSS_OPENSSL_V_openssl__cms_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/cms.h>
#define BOSS_OPENSSL_V_openssl__cmserr_h              <addon/openssl-1.1.1a_for_boss/include/openssl/cmserr.h>
#define BOSS_OPENSSL_V_openssl__comp_h                <addon/openssl-1.1.1a_for_boss/include/openssl/comp.h>
#define BOSS_OPENSSL_V_openssl__comperr_h             <addon/openssl-1.1.1a_for_boss/include/openssl/comperr.h>
#define BOSS_OPENSSL_V_openssl__conf_h                <addon/openssl-1.1.1a_for_boss/include/openssl/conf.h>
#define BOSS_OPENSSL_V_openssl__conf_api_h            <addon/openssl-1.1.1a_for_boss/include/openssl/conf_api.h>
#define BOSS_OPENSSL_V_openssl__conferr_h             <addon/openssl-1.1.1a_for_boss/include/openssl/conferr.h>
#define BOSS_OPENSSL_V_openssl__crypto_h              <addon/openssl-1.1.1a_for_boss/include/openssl/crypto.h>
#define BOSS_OPENSSL_V_openssl__cryptoerr_h           <addon/openssl-1.1.1a_for_boss/include/openssl/cryptoerr.h>
#define BOSS_OPENSSL_V_openssl__ct_h                  <addon/openssl-1.1.1a_for_boss/include/openssl/ct.h>
#define BOSS_OPENSSL_V_openssl__cterr_h               <addon/openssl-1.1.1a_for_boss/include/openssl/cterr.h>
#define BOSS_OPENSSL_V_openssl__des_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/des.h>
#define BOSS_OPENSSL_V_openssl__dh_h                  <addon/openssl-1.1.1a_for_boss/include/openssl/dh.h>
#define BOSS_OPENSSL_V_openssl__dherr_h               <addon/openssl-1.1.1a_for_boss/include/openssl/dherr.h>
#define BOSS_OPENSSL_V_openssl__dsa_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/dsa.h>
#define BOSS_OPENSSL_V_openssl__dsaerr_h              <addon/openssl-1.1.1a_for_boss/include/openssl/dsaerr.h>
#define BOSS_OPENSSL_V_openssl__dtls1_h               <addon/openssl-1.1.1a_for_boss/include/openssl/dtls1.h>
#define BOSS_OPENSSL_V_openssl__e_os2_h               <addon/openssl-1.1.1a_for_boss/include/openssl/e_os2.h>
#define BOSS_OPENSSL_V_openssl__ebcdic_h              <addon/openssl-1.1.1a_for_boss/include/openssl/ebcdic.h>
#define BOSS_OPENSSL_V_openssl__ec_h                  <addon/openssl-1.1.1a_for_boss/include/openssl/ec.h>
#define BOSS_OPENSSL_V_openssl__ecdh_h                <addon/openssl-1.1.1a_for_boss/include/openssl/ecdh.h>
#define BOSS_OPENSSL_V_openssl__ecdsa_h               <addon/openssl-1.1.1a_for_boss/include/openssl/ecdsa.h>
#define BOSS_OPENSSL_V_openssl__ecerr_h               <addon/openssl-1.1.1a_for_boss/include/openssl/ecerr.h>
#define BOSS_OPENSSL_V_openssl__engine_h              <addon/openssl-1.1.1a_for_boss/include/openssl/engine.h>
#define BOSS_OPENSSL_V_openssl__engineerr_h           <addon/openssl-1.1.1a_for_boss/include/openssl/engineerr.h>
#define BOSS_OPENSSL_V_openssl__err_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/err.h>
#define BOSS_OPENSSL_V_openssl__evp_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/evp.h>
#define BOSS_OPENSSL_V_openssl__evperr_h              <addon/openssl-1.1.1a_for_boss/include/openssl/evperr.h>
#define BOSS_OPENSSL_V_openssl__hmac_h                <addon/openssl-1.1.1a_for_boss/include/openssl/hmac.h>
#define BOSS_OPENSSL_V_openssl__idea_h                <addon/openssl-1.1.1a_for_boss/include/openssl/idea.h>
#define BOSS_OPENSSL_V_openssl__kdf_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/kdf.h>
#define BOSS_OPENSSL_V_openssl__kdferr_h              <addon/openssl-1.1.1a_for_boss/include/openssl/kdferr.h>
#define BOSS_OPENSSL_V_openssl__lhash_h               <addon/openssl-1.1.1a_for_boss/include/openssl/lhash.h>
#define BOSS_OPENSSL_V_openssl__md2_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/md2.h>
#define BOSS_OPENSSL_V_openssl__md4_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/md4.h>
#define BOSS_OPENSSL_V_openssl__md5_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/md5.h>
#define BOSS_OPENSSL_V_openssl__mdc2_h                <addon/openssl-1.1.1a_for_boss/include/openssl/mdc2.h>
#define BOSS_OPENSSL_V_openssl__modes_h               <addon/openssl-1.1.1a_for_boss/include/openssl/modes.h>
#define BOSS_OPENSSL_V_openssl__obj_mac_h             <addon/openssl-1.1.1a_for_boss/include/openssl/obj_mac.h>
#define BOSS_OPENSSL_V_openssl__objects_h             <addon/openssl-1.1.1a_for_boss/include/openssl/objects.h>
#define BOSS_OPENSSL_V_openssl__objectserr_h          <addon/openssl-1.1.1a_for_boss/include/openssl/objectserr.h>
#define BOSS_OPENSSL_V_openssl__ocsp_h                <addon/openssl-1.1.1a_for_boss/include/openssl/ocsp.h>
#define BOSS_OPENSSL_V_openssl__ocsperr_h             <addon/openssl-1.1.1a_for_boss/include/openssl/ocsperr.h>
#define BOSS_OPENSSL_V_openssl__opensslconf_h         <addon/openssl-1.1.1a_for_boss/include/openssl/opensslconf.h>
#define BOSS_OPENSSL_V_openssl__opensslv_h            <addon/openssl-1.1.1a_for_boss/include/openssl/opensslv.h>
#define BOSS_OPENSSL_V_openssl__ossl_typ_h            <addon/openssl-1.1.1a_for_boss/include/openssl/ossl_typ.h>
#define BOSS_OPENSSL_V_openssl__pem_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/pem.h>
#define BOSS_OPENSSL_V_openssl__pem2_h                <addon/openssl-1.1.1a_for_boss/include/openssl/pem2.h>
#define BOSS_OPENSSL_V_openssl__pemerr_h              <addon/openssl-1.1.1a_for_boss/include/openssl/pemerr.h>
#define BOSS_OPENSSL_V_openssl__pkcs12_h              <addon/openssl-1.1.1a_for_boss/include/openssl/pkcs12.h>
#define BOSS_OPENSSL_V_openssl__pkcs12err_h           <addon/openssl-1.1.1a_for_boss/include/openssl/pkcs12err.h>
#define BOSS_OPENSSL_V_openssl__pkcs7_h               <addon/openssl-1.1.1a_for_boss/include/openssl/pkcs7.h>
#define BOSS_OPENSSL_V_openssl__pkcs7err_h            <addon/openssl-1.1.1a_for_boss/include/openssl/pkcs7err.h>
#define BOSS_OPENSSL_V_openssl__rand_h                <addon/openssl-1.1.1a_for_boss/include/openssl/rand.h>
#define BOSS_OPENSSL_V_openssl__rand_drbg_h           <addon/openssl-1.1.1a_for_boss/include/openssl/rand_drbg.h>
#define BOSS_OPENSSL_V_openssl__randerr_h             <addon/openssl-1.1.1a_for_boss/include/openssl/randerr.h>
#define BOSS_OPENSSL_V_openssl__rc2_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/rc2.h>
#define BOSS_OPENSSL_V_openssl__rc4_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/rc4.h>
#define BOSS_OPENSSL_V_openssl__rc5_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/rc5.h>
#define BOSS_OPENSSL_V_openssl__ripemd_h              <addon/openssl-1.1.1a_for_boss/include/openssl/ripemd.h>
#define BOSS_OPENSSL_V_openssl__rsa_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/rsa.h>
#define BOSS_OPENSSL_V_openssl__rsaerr_h              <addon/openssl-1.1.1a_for_boss/include/openssl/rsaerr.h>
#define BOSS_OPENSSL_V_openssl__safestack_h           <addon/openssl-1.1.1a_for_boss/include/openssl/safestack.h>
#define BOSS_OPENSSL_V_openssl__seed_h                <addon/openssl-1.1.1a_for_boss/include/openssl/seed.h>
#define BOSS_OPENSSL_V_openssl__sha_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/sha.h>
#define BOSS_OPENSSL_V_openssl__srp_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/srp.h>
#define BOSS_OPENSSL_V_openssl__srtp_h                <addon/openssl-1.1.1a_for_boss/include/openssl/srtp.h>
#define BOSS_OPENSSL_V_openssl__ssl_h                 <addon/openssl-1.1.1a_for_boss/include/openssl/ssl.h>
#define BOSS_OPENSSL_V_openssl__ssl2_h                <addon/openssl-1.1.1a_for_boss/include/openssl/ssl2.h>
#define BOSS_OPENSSL_V_openssl__ssl3_h                <addon/openssl-1.1.1a_for_boss/include/openssl/ssl3.h>
#define BOSS_OPENSSL_V_openssl__sslerr_h              <addon/openssl-1.1.1a_for_boss/include/openssl/sslerr.h>
#define BOSS_OPENSSL_V_openssl__stack_h               <addon/openssl-1.1.1a_for_boss/include/openssl/stack.h>
#define BOSS_OPENSSL_V_openssl__store_h               <addon/openssl-1.1.1a_for_boss/include/openssl/store.h>
#define BOSS_OPENSSL_V_openssl__storeerr_h            <addon/openssl-1.1.1a_for_boss/include/openssl/storeerr.h>
#define BOSS_OPENSSL_V_openssl__symhacks_h            <addon/openssl-1.1.1a_for_boss/include/openssl/symhacks.h>
#define BOSS_OPENSSL_V_openssl__tls1_h                <addon/openssl-1.1.1a_for_boss/include/openssl/tls1.h>
#define BOSS_OPENSSL_V_openssl__ts_h                  <addon/openssl-1.1.1a_for_boss/include/openssl/ts.h>
#define BOSS_OPENSSL_V_openssl__tserr_h               <addon/openssl-1.1.1a_for_boss/include/openssl/tserr.h>
#define BOSS_OPENSSL_V_openssl__txt_db_h              <addon/openssl-1.1.1a_for_boss/include/openssl/txt_db.h>
#define BOSS_OPENSSL_V_openssl__ui_h                  <addon/openssl-1.1.1a_for_boss/include/openssl/ui.h>
#define BOSS_OPENSSL_V_openssl__uierr_h               <addon/openssl-1.1.1a_for_boss/include/openssl/uierr.h>
#define BOSS_OPENSSL_V_openssl__whrlpool_h            <addon/openssl-1.1.1a_for_boss/include/openssl/whrlpool.h>
#define BOSS_OPENSSL_V_openssl__x509_h                <addon/openssl-1.1.1a_for_boss/include/openssl/x509.h>
#define BOSS_OPENSSL_V_openssl__x509_vfy_h            <addon/openssl-1.1.1a_for_boss/include/openssl/x509_vfy.h>
#define BOSS_OPENSSL_V_openssl__x509err_h             <addon/openssl-1.1.1a_for_boss/include/openssl/x509err.h>
#define BOSS_OPENSSL_V_openssl__x509v3_h              <addon/openssl-1.1.1a_for_boss/include/openssl/x509v3.h>
#define BOSS_OPENSSL_V_openssl__x509v3err_h           <addon/openssl-1.1.1a_for_boss/include/openssl/x509v3err.h>
#define BOSS_OPENSSL_V_e_os_h                         <addon/openssl-1.1.1a_for_boss/e_os.h>
#define BOSS_OPENSSL_V_internal__cryptlib_int_h       <addon/openssl-1.1.1a_for_boss/crypto/include/internal/cryptlib_int.h>
#define BOSS_OPENSSL_V_internal__err_int_h            <addon/openssl-1.1.1a_for_boss/crypto/include/internal/err_int.h>
#define BOSS_OPENSSL_V_internal__x509_int_h           <addon/openssl-1.1.1a_for_boss/crypto/include/internal/x509_int.h>
#define BOSS_OPENSSL_V_internal__objects_h            <addon/openssl-1.1.1a_for_boss/crypto/include/internal/objects.h>
#define BOSS_OPENSSL_V_internal__evp_int_h            <addon/openssl-1.1.1a_for_boss/crypto/include/internal/evp_int.h>
#define BOSS_OPENSSL_V_internal__async_h              <addon/openssl-1.1.1a_for_boss/crypto/include/internal/async.h>
#define BOSS_OPENSSL_V_internal__rand_h               <addon/openssl-1.1.1a_for_boss/crypto/include/internal/rand.h>
#define BOSS_OPENSSL_V_internal__engine_h             <addon/openssl-1.1.1a_for_boss/crypto/include/internal/engine.h>
#define BOSS_OPENSSL_V_internal__bn_srp_h             <addon/openssl-1.1.1a_for_boss/crypto/include/internal/bn_srp.h>
#define BOSS_OPENSSL_V_internal__bio_h                <addon/openssl-1.1.1a_for_boss/include/internal/bio.h>
#define BOSS_OPENSSL_V_internal__comp_h               <addon/openssl-1.1.1a_for_boss/include/internal/comp.h>
#define BOSS_OPENSSL_V_internal__conf_h               <addon/openssl-1.1.1a_for_boss/include/internal/conf.h>
#define BOSS_OPENSSL_V_internal__constant_time_locl_h <addon/openssl-1.1.1a_for_boss/include/internal/constant_time_locl.h>
#define BOSS_OPENSSL_V_internal__cryptlib_h           <addon/openssl-1.1.1a_for_boss/include/internal/cryptlib.h>
#define BOSS_OPENSSL_V_internal__dane_h               <addon/openssl-1.1.1a_for_boss/include/internal/dane.h>
#define BOSS_OPENSSL_V_internal__dso_h                <addon/openssl-1.1.1a_for_boss/include/internal/dso.h>
#define BOSS_OPENSSL_V_internal__dsoerr_h             <addon/openssl-1.1.1a_for_boss/include/internal/dsoerr.h>
#define BOSS_OPENSSL_V_internal__err_h                <addon/openssl-1.1.1a_for_boss/include/internal/err.h>
#define BOSS_OPENSSL_V_internal__nelem_h              <addon/openssl-1.1.1a_for_boss/include/internal/nelem.h>
#define BOSS_OPENSSL_V_internal__numbers_h            <addon/openssl-1.1.1a_for_boss/include/internal/numbers.h>
#define BOSS_OPENSSL_V_internal__o_dir_h              <addon/openssl-1.1.1a_for_boss/include/internal/o_dir.h>
#define BOSS_OPENSSL_V_internal__o_str_h              <addon/openssl-1.1.1a_for_boss/include/internal/o_str.h>
#define BOSS_OPENSSL_V_internal__refcount_h           <addon/openssl-1.1.1a_for_boss/include/internal/refcount.h>
#define BOSS_OPENSSL_V_internal__sockets_h            <addon/openssl-1.1.1a_for_boss/include/internal/sockets.h>
#define BOSS_OPENSSL_V_internal__sslconf_h            <addon/openssl-1.1.1a_for_boss/include/internal/sslconf.h>
#define BOSS_OPENSSL_V_internal__thread_once_h        <addon/openssl-1.1.1a_for_boss/include/internal/thread_once.h>
#define BOSS_OPENSSL_V_internal__tsan_assist_h        <addon/openssl-1.1.1a_for_boss/include/internal/tsan_assist.h>

#define BOSS_OPENSSL_U_internal__aria_h               "addon/openssl-1.1.1a_for_boss/crypto/include/internal/aria.h"
#define BOSS_OPENSSL_U_internal__asn1_int_h           "addon/openssl-1.1.1a_for_boss/crypto/include/internal/asn1_int.h"
#define BOSS_OPENSSL_U_internal__async_h              "addon/openssl-1.1.1a_for_boss/crypto/include/internal/async.h"
#define BOSS_OPENSSL_U_internal__bn_conf_h            "addon/openssl-1.1.1a_for_boss/crypto/include/internal/bn_conf.h"
#define BOSS_OPENSSL_U_internal__bn_dh_h              "addon/openssl-1.1.1a_for_boss/crypto/include/internal/bn_dh.h"
#define BOSS_OPENSSL_U_internal__bn_int_h             "addon/openssl-1.1.1a_for_boss/crypto/include/internal/bn_int.h"
#define BOSS_OPENSSL_U_internal__bn_srp_h             "addon/openssl-1.1.1a_for_boss/crypto/include/internal/bn_srp.h"
#define BOSS_OPENSSL_U_internal__chacha_h             "addon/openssl-1.1.1a_for_boss/crypto/include/internal/chacha.h"
#define BOSS_OPENSSL_U_internal__cryptlib_int_h       "addon/openssl-1.1.1a_for_boss/crypto/include/internal/cryptlib_int.h"
#define BOSS_OPENSSL_U_internal__ctype_h              "addon/openssl-1.1.1a_for_boss/crypto/include/internal/ctype.h"
#define BOSS_OPENSSL_U_internal__dso_conf_h           "addon/openssl-1.1.1a_for_boss/crypto/include/internal/dso_conf.h"
#define BOSS_OPENSSL_U_internal__ec_int_h             "addon/openssl-1.1.1a_for_boss/crypto/include/internal/ec_int.h"
#define BOSS_OPENSSL_U_internal__engine_h             "addon/openssl-1.1.1a_for_boss/crypto/include/internal/engine.h"
#define BOSS_OPENSSL_U_internal__err_int_h            "addon/openssl-1.1.1a_for_boss/crypto/include/internal/err_int.h"
#define BOSS_OPENSSL_U_internal__evp_int_h            "addon/openssl-1.1.1a_for_boss/crypto/include/internal/evp_int.h"
#define BOSS_OPENSSL_U_internal__lhash_h              "addon/openssl-1.1.1a_for_boss/crypto/include/internal/lhash.h"
#define BOSS_OPENSSL_U_internal__md32_common_h        "addon/openssl-1.1.1a_for_boss/crypto/include/internal/md32_common.h"
#define BOSS_OPENSSL_U_internal__objects_h            "addon/openssl-1.1.1a_for_boss/crypto/include/internal/objects.h"
#define BOSS_OPENSSL_U_internal__poly1305_h           "addon/openssl-1.1.1a_for_boss/crypto/include/internal/poly1305.h"
#define BOSS_OPENSSL_U_internal__rand_int_h           "addon/openssl-1.1.1a_for_boss/crypto/include/internal/rand_int.h"
#define BOSS_OPENSSL_U_internal__sha_h                "addon/openssl-1.1.1a_for_boss/crypto/include/internal/sha.h"
#define BOSS_OPENSSL_U_internal__siphash_h            "addon/openssl-1.1.1a_for_boss/crypto/include/internal/siphash.h"
#define BOSS_OPENSSL_U_internal__sm2_h                "addon/openssl-1.1.1a_for_boss/crypto/include/internal/sm2.h"
#define BOSS_OPENSSL_U_internal__sm2err_h             "addon/openssl-1.1.1a_for_boss/crypto/include/internal/sm2err.h"
#define BOSS_OPENSSL_U_internal__sm3_h                "addon/openssl-1.1.1a_for_boss/crypto/include/internal/sm3.h"
#define BOSS_OPENSSL_U_internal__sm4_h                "addon/openssl-1.1.1a_for_boss/crypto/include/internal/sm4.h"
#define BOSS_OPENSSL_U_internal__store_h              "addon/openssl-1.1.1a_for_boss/crypto/include/internal/store.h"
#define BOSS_OPENSSL_U_internal__store_int_h          "addon/openssl-1.1.1a_for_boss/crypto/include/internal/store_int.h"
#define BOSS_OPENSSL_U_internal__x509_int_h           "addon/openssl-1.1.1a_for_boss/crypto/include/internal/x509_int.h"
#define BOSS_OPENSSL_U_internal__bio_h                "addon/openssl-1.1.1a_for_boss/include/internal/bio.h"
#define BOSS_OPENSSL_U_internal__comp_h               "addon/openssl-1.1.1a_for_boss/include/internal/comp.h"
#define BOSS_OPENSSL_U_internal__conf_h               "addon/openssl-1.1.1a_for_boss/include/internal/conf.h"
#define BOSS_OPENSSL_U_internal__constant_time_locl_h "addon/openssl-1.1.1a_for_boss/include/internal/constant_time_locl.h"
#define BOSS_OPENSSL_U_internal__cryptlib_h           "addon/openssl-1.1.1a_for_boss/include/internal/cryptlib.h"
#define BOSS_OPENSSL_U_internal__dane_h               "addon/openssl-1.1.1a_for_boss/include/internal/dane.h"
#define BOSS_OPENSSL_U_internal__dso_h                "addon/openssl-1.1.1a_for_boss/include/internal/dso.h"
#define BOSS_OPENSSL_U_internal__dsoerr_h             "addon/openssl-1.1.1a_for_boss/include/internal/dsoerr.h"
#define BOSS_OPENSSL_U_internal__err_h                "addon/openssl-1.1.1a_for_boss/include/internal/err.h"
#define BOSS_OPENSSL_U_internal__nelem_h              "addon/openssl-1.1.1a_for_boss/include/internal/nelem.h"
#define BOSS_OPENSSL_U_internal__numbers_h            "addon/openssl-1.1.1a_for_boss/include/internal/numbers.h"
#define BOSS_OPENSSL_U_internal__o_dir_h              "addon/openssl-1.1.1a_for_boss/include/internal/o_dir.h"
#define BOSS_OPENSSL_U_internal__o_str_h              "addon/openssl-1.1.1a_for_boss/include/internal/o_str.h"
#define BOSS_OPENSSL_U_internal__refcount_h           "addon/openssl-1.1.1a_for_boss/include/internal/refcount.h"
#define BOSS_OPENSSL_U_internal__sockets_h            "addon/openssl-1.1.1a_for_boss/include/internal/sockets.h"
#define BOSS_OPENSSL_U_internal__sslconf_h            "addon/openssl-1.1.1a_for_boss/include/internal/sslconf.h"
#define BOSS_OPENSSL_U_internal__thread_once_h        "addon/openssl-1.1.1a_for_boss/include/internal/thread_once.h"
#define BOSS_OPENSSL_U_internal__tsan_assist_h        "addon/openssl-1.1.1a_for_boss/include/internal/tsan_assist.h"
#define BOSS_OPENSSL_U_openssl__ebcdic_h              "addon/openssl-1.1.1a_for_boss/include/openssl/ebcdic.h"
#define BOSS_OPENSSL_U_openssl__opensslconf_h         "addon/openssl-1.1.1a_for_boss/include/openssl/opensslconf.h"
#define BOSS_OPENSSL_U_openssl__e_os2_h               "addon/openssl-1.1.1a_for_boss/include/openssl/e_os2.h"
#define BOSS_OPENSSL_U_e_os_h                         "addon/openssl-1.1.1a_for_boss/e_os.h"
#define BOSS_OPENSSL_U_modes_lcl_h                    "addon/openssl-1.1.1a_for_boss/crypto/modes/modes_lcl.h"
#define BOSS_OPENSSL_U_ui_locl_h                      "addon/openssl-1.1.1a_for_boss/crypto/ui/ui_locl.h"
#define BOSS_OPENSSL_U_arch_intrinsics_h              "addon/openssl-1.1.1a_for_boss/crypto/ec/curve448/arch_32/arch_intrinsics.h"
#define BOSS_OPENSSL_U_f_impl_h                       "addon/openssl-1.1.1a_for_boss/crypto/ec/curve448/arch_32/f_impl.h"
#define BOSS_OPENSSL_U_field_h                        "addon/openssl-1.1.1a_for_boss/crypto/ec/curve448/field.h"

#ifndef BOSS_OPENSSL_DETOUR
    #ifdef __cplusplus
        extern "C" {
    #endif
        #include <addon/openssl-1.1.1a_for_boss/crypto/bio/bio_lcl.h>
        #include <addon/openssl-1.1.1a_for_boss/include/openssl/evp.h>
        #include <addon/openssl-1.1.1a_for_boss/crypto/evp/evp_locl.h>
        #include <addon/openssl-1.1.1a_for_boss/include/openssl/srtp.h>
    #ifdef __cplusplus
        }
    #endif
#else
    #ifdef __cplusplus
        extern "C" {
    #endif
        #include <addon/openssl-1.1.1a_for_boss/include/openssl/ssl.h>
        const SSL_METHOD *DTLSv1_2_client_method(void);
        const SSL_METHOD *TLSv1_2_client_method(void);
        const SSL_METHOD *DTLSv1_client_method(void);
        const SSL_METHOD *DTLSv1_server_method(void);
        const SSL_METHOD *TLSv1_client_method(void);
        const SSL_METHOD *TLSv1_server_method(void);
        unsigned char *ASN1_STRING_data(ASN1_STRING *x);
    #ifdef __cplusplus
        }
    #endif
#endif
