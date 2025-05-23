/*
 * Copyright 1999-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_V_openssl__pkcs12_h //original-code:<openssl/pkcs12.h>
#include "p12_lcl.h"

/* Initialise a PKCS12 structure to take data */

PKCS12 *PKCS12_init(int mode)
{
    PKCS12 *pkcs12;

    if ((pkcs12 = PKCS12_new()) == NULL) {
        PKCS12err(PKCS12_F_PKCS12_INIT, ERR_R_MALLOC_FAILURE);
        return NULL;
    }
    ASN1_INTEGER_set(pkcs12->version, 3);
    pkcs12->authsafes->type = OBJ_nid2obj(mode);
    switch (mode) {
    case NID_pkcs7_data:
        if ((pkcs12->authsafes->d.data = ASN1_OCTET_STRING_new()) == NULL) {
            PKCS12err(PKCS12_F_PKCS12_INIT, ERR_R_MALLOC_FAILURE);
            goto err;
        }
        break;
    default:
        PKCS12err(PKCS12_F_PKCS12_INIT, PKCS12_R_UNSUPPORTED_PKCS12_MODE);
        goto err;
    }
    return pkcs12;

 err:
    PKCS12_free(pkcs12);
    return NULL;
}
