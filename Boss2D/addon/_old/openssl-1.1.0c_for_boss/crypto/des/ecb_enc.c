/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "des_locl.h"
#include BOSS_OPENSSL_V_openssl__opensslv_h //original-code:<openssl/opensslv.h>
#include BOSS_OPENSSL_V_openssl__bio_h //original-code:<openssl/bio.h>


const char *DES_options(void)
{
    static int init = 1;
    static char buf[32];

    if (init) {
        const char *size;

        if (sizeof(DES_LONG) != sizeof(long))
            size = "int";
        else
            size = "long";
        BIO_snprintf(buf, sizeof buf, "des(%s)", size);
        init = 0;
    }
    return (buf);
}

void DES_ecb_encrypt(const_DES_cblock *input, DES_cblock *output,
                     DES_key_schedule *ks, int enc)
{
    register DES_LONG l;
    DES_LONG ll[2];
    const unsigned char *in = &(*input)[0];
    unsigned char *out = &(*output)[0];

    c2l(in, l);
    ll[0] = l;
    c2l(in, l);
    ll[1] = l;
    DES_encrypt1(ll, ks, enc);
    l = ll[0];
    l2c(l, out);
    l = ll[1];
    l2c(l, out);
    l = ll[0] = ll[1] = 0;
}
