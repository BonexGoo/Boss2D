/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_V_openssl__asn1t_h //original-code:<openssl/asn1t.h>
#include BOSS_OPENSSL_V_openssl__x509_h //original-code:<openssl/x509.h>

ASN1_SEQUENCE(X509_VAL) = {
        ASN1_SIMPLE(X509_VAL, notBefore, ASN1_TIME),
        ASN1_SIMPLE(X509_VAL, notAfter, ASN1_TIME)
} ASN1_SEQUENCE_END(X509_VAL)

IMPLEMENT_ASN1_FUNCTIONS(X509_VAL)
