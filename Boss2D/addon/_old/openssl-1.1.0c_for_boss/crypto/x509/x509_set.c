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

int X509_set_version(X509 *x, long version)
{
    if (x == NULL)
        return (0);
    if (version == 0) {
        ASN1_INTEGER_free(x->cert_info.version);
        x->cert_info.version = NULL;
        return (1);
    }
    if (x->cert_info.version == NULL) {
        if ((x->cert_info.version = ASN1_INTEGER_new()) == NULL)
            return (0);
    }
    return (ASN1_INTEGER_set(x->cert_info.version, version));
}

int X509_set_serialNumber(X509 *x, ASN1_INTEGER *serial)
{
    ASN1_INTEGER *in;

    if (x == NULL)
        return 0;
    in = &x->cert_info.serialNumber;
    if (in != serial)
        return ASN1_STRING_copy(in, serial);
    return 1;
}

int X509_set_issuer_name(X509 *x, X509_NAME *name)
{
    if (x == NULL)
        return (0);
    return (X509_NAME_set(&x->cert_info.issuer, name));
}

int X509_set_subject_name(X509 *x, X509_NAME *name)
{
    if (x == NULL)
        return (0);
    return (X509_NAME_set(&x->cert_info.subject, name));
}

int x509_set1_time(ASN1_TIME **ptm, const ASN1_TIME *tm)
{
    ASN1_TIME *in;
    in = *ptm;
    if (in != tm) {
        in = ASN1_STRING_dup(tm);
        if (in != NULL) {
            ASN1_TIME_free(*ptm);
            *ptm = in;
        }
    }
    return (in != NULL);
}

int X509_set1_notBefore(X509 *x, const ASN1_TIME *tm)
{
    if (x == NULL)
        return 0;
    return x509_set1_time(&x->cert_info.validity.notBefore, tm);
}

int X509_set1_notAfter(X509 *x, const ASN1_TIME *tm)
{
    if (x == NULL)
        return 0;
    return x509_set1_time(&x->cert_info.validity.notAfter, tm);
}

int X509_set_pubkey(X509 *x, EVP_PKEY *pkey)
{
    if (x == NULL)
        return (0);
    return (X509_PUBKEY_set(&(x->cert_info.key), pkey));
}

int X509_up_ref(X509 *x)
{
    int i;

    if (CRYPTO_atomic_add(&x->references, 1, &i, x->lock) <= 0)
        return 0;

    REF_PRINT_COUNT("X509", x);
    REF_ASSERT_ISNT(i < 2);
    return ((i > 1) ? 1 : 0);
}

long X509_get_version(const X509 *x)
{
    return ASN1_INTEGER_get(x->cert_info.version);
}

const ASN1_TIME *X509_get0_notBefore(const X509 *x)
{
    return x->cert_info.validity.notBefore;
}

const ASN1_TIME *X509_get0_notAfter(const X509 *x)
{
    return x->cert_info.validity.notAfter;
}

ASN1_TIME *X509_getm_notBefore(const X509 *x)
{
    return x->cert_info.validity.notBefore;
}

ASN1_TIME *X509_getm_notAfter(const X509 *x)
{
    return x->cert_info.validity.notAfter;
}

int X509_get_signature_type(const X509 *x)
{
    return EVP_PKEY_type(OBJ_obj2nid(x->sig_alg.algorithm));
}

X509_PUBKEY *X509_get_X509_PUBKEY(const X509 *x)
{
    return x->cert_info.key;
}

const STACK_OF(X509_EXTENSION) *X509_get0_extensions(const X509 *x)
{
    return x->cert_info.extensions;
}

void X509_get0_uids(const X509 *x, const ASN1_BIT_STRING **piuid,
                    const ASN1_BIT_STRING **psuid)
{
    if (piuid != NULL)
        *piuid = x->cert_info.issuerUID;
    if (psuid != NULL)
        *psuid = x->cert_info.subjectUID;
}

const X509_ALGOR *X509_get0_tbs_sigalg(const X509 *x)
{
    return &x->cert_info.signature;
}
