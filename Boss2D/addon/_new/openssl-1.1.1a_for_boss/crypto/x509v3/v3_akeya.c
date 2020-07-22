/*
 * Copyright 2001-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_V_openssl__conf_h //original-code:<openssl/conf.h>
#include BOSS_OPENSSL_V_openssl__asn1_h //original-code:<openssl/asn1.h>
#include BOSS_OPENSSL_V_openssl__asn1t_h //original-code:<openssl/asn1t.h>
#include BOSS_OPENSSL_V_openssl__x509v3_h //original-code:<openssl/x509v3.h>

ASN1_SEQUENCE(AUTHORITY_KEYID) = {
        ASN1_IMP_OPT(AUTHORITY_KEYID, keyid, ASN1_OCTET_STRING, 0),
        ASN1_IMP_SEQUENCE_OF_OPT(AUTHORITY_KEYID, issuer, GENERAL_NAME, 1),
        ASN1_IMP_OPT(AUTHORITY_KEYID, serial, ASN1_INTEGER, 2)
} ASN1_SEQUENCE_END(AUTHORITY_KEYID)

IMPLEMENT_ASN1_FUNCTIONS(AUTHORITY_KEYID)
