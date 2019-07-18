/*
 * Copyright 1995-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_U_internal__err_int_h //original-code:"internal/err_int.h"
#include BOSS_OPENSSL_V_openssl__asn1err_h //original-code:<openssl/asn1err.h>
#include BOSS_OPENSSL_V_openssl__bnerr_h //original-code:<openssl/bnerr.h>
#include BOSS_OPENSSL_V_openssl__ecerr_h //original-code:<openssl/ecerr.h>
#include BOSS_OPENSSL_V_openssl__buffererr_h //original-code:<openssl/buffererr.h>
#include BOSS_OPENSSL_V_openssl__bioerr_h //original-code:<openssl/bioerr.h>
#include BOSS_OPENSSL_V_openssl__comperr_h //original-code:<openssl/comperr.h>
#include BOSS_OPENSSL_V_openssl__rsaerr_h //original-code:<openssl/rsaerr.h>
#include BOSS_OPENSSL_V_openssl__dherr_h //original-code:<openssl/dherr.h>
#include BOSS_OPENSSL_V_openssl__dsaerr_h //original-code:<openssl/dsaerr.h>
#include BOSS_OPENSSL_V_openssl__evperr_h //original-code:<openssl/evperr.h>
#include BOSS_OPENSSL_V_openssl__objectserr_h //original-code:<openssl/objectserr.h>
#include BOSS_OPENSSL_V_openssl__pemerr_h //original-code:<openssl/pemerr.h>
#include BOSS_OPENSSL_V_openssl__pkcs7err_h //original-code:<openssl/pkcs7err.h>
#include BOSS_OPENSSL_V_openssl__x509err_h //original-code:<openssl/x509err.h>
#include BOSS_OPENSSL_V_openssl__x509v3err_h //original-code:<openssl/x509v3err.h>
#include BOSS_OPENSSL_V_openssl__conferr_h //original-code:<openssl/conferr.h>
#include BOSS_OPENSSL_V_openssl__pkcs12err_h //original-code:<openssl/pkcs12err.h>
#include BOSS_OPENSSL_V_openssl__randerr_h //original-code:<openssl/randerr.h>
#include BOSS_OPENSSL_U_internal__dso_h //original-code:"internal/dso.h"
#include BOSS_OPENSSL_V_openssl__engineerr_h //original-code:<openssl/engineerr.h>
#include BOSS_OPENSSL_V_openssl__uierr_h //original-code:<openssl/uierr.h>
#include BOSS_OPENSSL_V_openssl__ocsperr_h //original-code:<openssl/ocsperr.h>
#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#include BOSS_OPENSSL_V_openssl__tserr_h //original-code:<openssl/tserr.h>
#include BOSS_OPENSSL_V_openssl__cmserr_h //original-code:<openssl/cmserr.h>
#include BOSS_OPENSSL_V_openssl__cterr_h //original-code:<openssl/cterr.h>
#include BOSS_OPENSSL_V_openssl__asyncerr_h //original-code:<openssl/asyncerr.h>
#include BOSS_OPENSSL_V_openssl__kdferr_h //original-code:<openssl/kdferr.h>
#include BOSS_OPENSSL_V_openssl__storeerr_h //original-code:<openssl/storeerr.h>

int err_load_crypto_strings_int(void)
{
    if (
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
        ERR_load_UI_strings() == 0 ||
# ifndef OPENSSL_NO_CMS
        ERR_load_CMS_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_CT
        ERR_load_CT_strings() == 0 ||
# endif
        ERR_load_ASYNC_strings() == 0 ||
#endif
        ERR_load_KDF_strings() == 0 ||
        ERR_load_OSSL_STORE_strings() == 0)
        return 0;

    return 1;
}
