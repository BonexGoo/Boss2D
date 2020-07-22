#pragma once
#include <boss.h>
#include <addon/boss_fakewin.h>
#include <addon/boss_integration_openssl-1.1.0c.h>

#define OPENSSL
#define HAVE_CONFIG_H
#include <addon/libsrtp-2.2.0_for_boss/[gen]/config.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// _BOSS_BATCH_COMMAND_ {"type" : "include-alias", "prm" : "BOSS_LIBSRTP_", "restore-comment" : " //original-code:"}
#define BOSS_LIBSRTP_V_config_h                                                     "addon/libsrtp-2.2.0_for_boss/[gen]/config.h"
#define BOSS_LIBSRTP_U_config_h                                                     "addon/libsrtp-2.2.0_for_boss/[gen]/config.h"

#define BOSS_LIBSRTP_U_ekt_h                                                        "addon/libsrtp-2.2.0_for_boss/include/ekt.h"
#define BOSS_LIBSRTP_U_getopt_s_h                                                   "addon/libsrtp-2.2.0_for_boss/include/getopt_s.h"
#define BOSS_LIBSRTP_U_srtp_h                                                       "addon/libsrtp-2.2.0_for_boss/include/srtp.h"
#define BOSS_LIBSRTP_U_srtp_priv_h                                                  "addon/libsrtp-2.2.0_for_boss/include/srtp_priv.h"
#define BOSS_LIBSRTP_U_ut_sim_h                                                     "addon/libsrtp-2.2.0_for_boss/include/ut_sim.h"

#define BOSS_LIBSRTP_U_aes_h                                                        "addon/libsrtp-2.2.0_for_boss/crypto/include/aes.h"
#define BOSS_LIBSRTP_U_aes_gcm_ossl_h                                               "addon/libsrtp-2.2.0_for_boss/crypto/include/aes_gcm_ossl.h"
#define BOSS_LIBSRTP_U_aes_icm_h                                                    "addon/libsrtp-2.2.0_for_boss/crypto/include/aes_icm.h"
#define BOSS_LIBSRTP_U_aes_icm_ossl_h                                               "addon/libsrtp-2.2.0_for_boss/crypto/include/aes_icm_ossl.h"
#define BOSS_LIBSRTP_U_alloc_h                                                      "addon/libsrtp-2.2.0_for_boss/crypto/include/alloc.h"
#define BOSS_LIBSRTP_U_auth_h                                                       "addon/libsrtp-2.2.0_for_boss/crypto/include/auth.h"
#define BOSS_LIBSRTP_U_cipher_h                                                     "addon/libsrtp-2.2.0_for_boss/crypto/include/cipher.h"
#define BOSS_LIBSRTP_U_cipher_types_h                                               "addon/libsrtp-2.2.0_for_boss/crypto/include/cipher_types.h"
#define BOSS_LIBSRTP_U_crypto_kernel_h                                              "addon/libsrtp-2.2.0_for_boss/crypto/include/crypto_kernel.h"
#define BOSS_LIBSRTP_U_crypto_types_h                                               "addon/libsrtp-2.2.0_for_boss/crypto/include/crypto_types.h"
#define BOSS_LIBSRTP_U_datatypes_h                                                  "addon/libsrtp-2.2.0_for_boss/crypto/include/datatypes.h"
#define BOSS_LIBSRTP_U_err_h                                                        "addon/libsrtp-2.2.0_for_boss/crypto/include/err.h"
#define BOSS_LIBSRTP_U_hmac_h                                                       "addon/libsrtp-2.2.0_for_boss/crypto/include/hmac.h"
#define BOSS_LIBSRTP_U_integers_h                                                   "addon/libsrtp-2.2.0_for_boss/crypto/include/integers.h"
#define BOSS_LIBSRTP_U_key_h                                                        "addon/libsrtp-2.2.0_for_boss/crypto/include/key.h"
#define BOSS_LIBSRTP_U_null_auth_h                                                  "addon/libsrtp-2.2.0_for_boss/crypto/include/null_auth.h"
#define BOSS_LIBSRTP_U_null_cipher_h                                                "addon/libsrtp-2.2.0_for_boss/crypto/include/null_cipher.h"
#define BOSS_LIBSRTP_U_rdb_h                                                        "addon/libsrtp-2.2.0_for_boss/crypto/include/rdb.h"
#define BOSS_LIBSRTP_U_rdbx_h                                                       "addon/libsrtp-2.2.0_for_boss/crypto/include/rdbx.h"
#define BOSS_LIBSRTP_U_sha1_h                                                       "addon/libsrtp-2.2.0_for_boss/crypto/include/sha1.h"
#define BOSS_LIBSRTP_U_stat_h                                                       "addon/libsrtp-2.2.0_for_boss/crypto/include/stat.h"

// 외부지원
#define BOSS_LIBSRTP_U_third_party__libsrtp__include__srtp_h                        "addon/libsrtp-2.2.0_for_boss/include/srtp.h"
#define BOSS_LIBSRTP_U_third_party__libsrtp__include__srtp_priv_h                   "addon/libsrtp-2.2.0_for_boss/include/srtp_priv.h"
#define BOSS_LIBSRTP_U_third_party__libsrtp__crypto__include__auth_h                "addon/libsrtp-2.2.0_for_boss/crypto/include/auth.h"
#define BOSS_LIBSRTP_U_third_party__libsrtp__crypto__include__crypto_kernel_h       "addon/libsrtp-2.2.0_for_boss/crypto/include/crypto_kernel.h"
