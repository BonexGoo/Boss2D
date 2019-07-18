/*
 * Copyright 2002-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_V_openssl__crypto_h //original-code:<openssl/crypto.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_V_openssl__conf_h //original-code:<openssl/conf.h>
#include BOSS_OPENSSL_V_openssl__x509_h //original-code:<openssl/x509.h>
#include BOSS_OPENSSL_V_openssl__asn1_h //original-code:<openssl/asn1.h>
#include BOSS_OPENSSL_V_openssl__engine_h //original-code:<openssl/engine.h>

/* Load all OpenSSL builtin modules */

void OPENSSL_load_builtin_modules(void)
{
    /* Add builtin modules here */
    ASN1_add_oid_module();
    ASN1_add_stable_module();
#ifndef OPENSSL_NO_ENGINE
    ENGINE_add_conf_module();
#endif
    EVP_add_alg_module();
}
