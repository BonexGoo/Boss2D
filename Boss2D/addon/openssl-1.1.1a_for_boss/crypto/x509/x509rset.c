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
#include BOSS_OPENSSL_V_openssl__asn1_h //original-code:<openssl/asn1.h>
#include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
#include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
#include BOSS_OPENSSL_V_openssl__x509_h //original-code:<openssl/x509.h>
#include BOSS_OPENSSL_U_internal__x509_int_h //original-code:"internal/x509_int.h"

int X509_REQ_set_version(X509_REQ *x, long version)
{
    if (x == NULL)
        return 0;
    x->req_info.enc.modified = 1;
    return ASN1_INTEGER_set(x->req_info.version, version);
}

int X509_REQ_set_subject_name(X509_REQ *x, X509_NAME *name)
{
    if (x == NULL)
        return 0;
    x->req_info.enc.modified = 1;
    return X509_NAME_set(&x->req_info.subject, name);
}

int X509_REQ_set_pubkey(X509_REQ *x, EVP_PKEY *pkey)
{
    if (x == NULL)
        return 0;
    x->req_info.enc.modified = 1;
    return X509_PUBKEY_set(&x->req_info.pubkey, pkey);
}
