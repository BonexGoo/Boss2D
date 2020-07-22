/*
 * Copyright 2010-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stddef.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_V_openssl__asn1_h //original-code:<openssl/asn1.h>
#include BOSS_OPENSSL_V_openssl__asn1t_h //original-code:<openssl/asn1t.h>
#include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
#include BOSS_OPENSSL_V_openssl__buffer_h //original-code:<openssl/buffer.h>
#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#include BOSS_OPENSSL_V_openssl__x509v3_h //original-code:<openssl/x509v3.h>
#include "asn1_locl.h"

/*
 * General ASN1 structure recursive scanner: iterate through all fields
 * passing details to a callback.
 */

ASN1_SCTX *ASN1_SCTX_new(int (*scan_cb) (ASN1_SCTX *ctx))
{
    ASN1_SCTX *ret = OPENSSL_zalloc(sizeof(*ret));

    if (ret == NULL) {
        ASN1err(ASN1_F_ASN1_SCTX_NEW, ERR_R_MALLOC_FAILURE);
        return NULL;
    }
    ret->scan_cb = scan_cb;
    return ret;
}

void ASN1_SCTX_free(ASN1_SCTX *p)
{
    OPENSSL_free(p);
}

const ASN1_ITEM *ASN1_SCTX_get_item(ASN1_SCTX *p)
{
    return p->it;
}

const ASN1_TEMPLATE *ASN1_SCTX_get_template(ASN1_SCTX *p)
{
    return p->tt;
}

unsigned long ASN1_SCTX_get_flags(ASN1_SCTX *p)
{
    return p->flags;
}

void ASN1_SCTX_set_app_data(ASN1_SCTX *p, void *data)
{
    p->app_data = data;
}

void *ASN1_SCTX_get_app_data(ASN1_SCTX *p)
{
    return p->app_data;
}
