/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_U_internal__err_int_h //original-code:"internal/err_int.h"
#include BOSS_OPENSSL_V_openssl__asn1_h //original-code:<openssl/asn1.h>
#include BOSS_OPENSSL_V_openssl__bn_h //original-code:<openssl/bn.h>
#include BOSS_OPENSSL_V_openssl__ec_h //original-code:<openssl/ec.h>
#include BOSS_OPENSSL_V_openssl__buffer_h //original-code:<openssl/buffer.h>
#include BOSS_OPENSSL_V_openssl__bio_h //original-code:<openssl/bio.h>
#include BOSS_OPENSSL_V_openssl__comp_h //original-code:<openssl/comp.h>
#include BOSS_OPENSSL_V_openssl__rsa_h //original-code:<openssl/rsa.h>
#include BOSS_OPENSSL_V_openssl__dh_h //original-code:<openssl/dh.h>
#include BOSS_OPENSSL_V_openssl__dsa_h //original-code:<openssl/dsa.h>
#include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
#include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
#include BOSS_OPENSSL_V_openssl__pem2_h //original-code:<openssl/pem2.h>
#include BOSS_OPENSSL_V_openssl__x509_h //original-code:<openssl/x509.h>
#include BOSS_OPENSSL_V_openssl__x509v3_h //original-code:<openssl/x509v3.h>
#include BOSS_OPENSSL_V_openssl__conf_h //original-code:<openssl/conf.h>
#include BOSS_OPENSSL_V_openssl__pkcs12_h //original-code:<openssl/pkcs12.h>
#include BOSS_OPENSSL_V_openssl__rand_h //original-code:<openssl/rand.h>
#include BOSS_OPENSSL_U_internal__dso_h //original-code:"internal/dso.h"
#include BOSS_OPENSSL_V_openssl__engine_h //original-code:<openssl/engine.h>
#include BOSS_OPENSSL_V_openssl__ui_h //original-code:<openssl/ui.h>
#include BOSS_OPENSSL_V_openssl__ocsp_h //original-code:<openssl/ocsp.h>
#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#ifdef OPENSSL_FIPS
# include <openssl/fips.h>
#endif
#include BOSS_OPENSSL_V_openssl__ts_h //original-code:<openssl/ts.h>
#include BOSS_OPENSSL_V_openssl__cms_h //original-code:<openssl/cms.h>
#include BOSS_OPENSSL_V_openssl__ct_h //original-code:<openssl/ct.h>
#include BOSS_OPENSSL_V_openssl__async_h //original-code:<openssl/async.h>
#include BOSS_OPENSSL_V_openssl__kdf_h //original-code:<openssl/kdf.h>

int err_load_crypto_strings_int(void)
{
    if (
#ifdef OPENSSL_FIPS
        FIPS_set_error_callbacks(ERR_put_error, ERR_add_error_vdata) == 0 ||
#endif
#ifndef OPENSSL_NO_ERR
        ERR_load_ERR_strings() == 0 ||    /* include error strings for SYSerr */
        ERR_load_BN_strings() == 0 ||
# ifndef OPENSSL_NO_RSA
        ERR_load_RSA_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_DH
        ERR_load_DH_strings() == 0 ||
# endif
        ERR_load_EVP_strings() == 0 ||
        ERR_load_BUF_strings() == 0 ||
        ERR_load_OBJ_strings() == 0 ||
        ERR_load_PEM_strings() == 0 ||
# ifndef OPENSSL_NO_DSA
        ERR_load_DSA_strings() == 0 ||
# endif
        ERR_load_X509_strings() == 0 ||
        ERR_load_ASN1_strings() == 0 ||
        ERR_load_CONF_strings() == 0 ||
        ERR_load_CRYPTO_strings() == 0 ||
# ifndef OPENSSL_NO_COMP
        ERR_load_COMP_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_EC
        ERR_load_EC_strings() == 0 ||
# endif
        /* skip ERR_load_SSL_strings() because it is not in this library */
        ERR_load_BIO_strings() == 0 ||
        ERR_load_PKCS7_strings() == 0 ||
        ERR_load_X509V3_strings() == 0 ||
        ERR_load_PKCS12_strings() == 0 ||
        ERR_load_RAND_strings() == 0 ||
        ERR_load_DSO_strings() == 0 ||
# ifndef OPENSSL_NO_TS
        ERR_load_TS_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_ENGINE
        ERR_load_ENGINE_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_OCSP
        ERR_load_OCSP_strings() == 0 ||
# endif
#ifndef OPENSSL_NO_UI
        ERR_load_UI_strings() == 0 ||
#endif
# ifdef OPENSSL_FIPS
        ERR_load_FIPS_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_CMS
        ERR_load_CMS_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_CT
        ERR_load_CT_strings() == 0 ||
# endif
        ERR_load_ASYNC_strings() == 0 ||
#endif
        ERR_load_KDF_strings() == 0)
        return 0;

    return 1;
}
