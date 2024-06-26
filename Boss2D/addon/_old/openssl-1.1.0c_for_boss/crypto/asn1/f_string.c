/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <ctype.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_V_openssl__buffer_h //original-code:<openssl/buffer.h>
#include BOSS_OPENSSL_V_openssl__asn1_h //original-code:<openssl/asn1.h>

int i2a_ASN1_STRING(BIO *bp, const ASN1_STRING *a, int type)
{
    int i, n = 0;
    static const char *h = "0123456789ABCDEF";
    char buf[2];

    if (a == NULL)
        return (0);

    if (a->length == 0) {
        if (BIO_write(bp, "0", 1) != 1)
            goto err;
        n = 1;
    } else {
        for (i = 0; i < a->length; i++) {
            if ((i != 0) && (i % 35 == 0)) {
                if (BIO_write(bp, "\\\n", 2) != 2)
                    goto err;
                n += 2;
            }
            buf[0] = h[((unsigned char)a->data[i] >> 4) & 0x0f];
            buf[1] = h[((unsigned char)a->data[i]) & 0x0f];
            if (BIO_write(bp, buf, 2) != 2)
                goto err;
            n += 2;
        }
    }
    return (n);
 err:
    return (-1);
}

int a2i_ASN1_STRING(BIO *bp, ASN1_STRING *bs, char *buf, int size)
{
    int i, j, k, m, n, again, bufsize, spec_char;
    unsigned char *s = NULL, *sp;
    unsigned char *bufp;
    int num = 0, slen = 0, first = 1;

    bufsize = BIO_gets(bp, buf, size);
    for (;;) {
        if (bufsize < 1) {
            if (first)
                break;
            else
                goto err;
        }
        first = 0;

        i = bufsize;
        if (buf[i - 1] == '\n')
            buf[--i] = '\0';
        if (i == 0)
            goto err;
        if (buf[i - 1] == '\r')
            buf[--i] = '\0';
        if (i == 0)
            goto err;
        again = (buf[i - 1] == '\\');

        for (j = i - 1; j > 0; j--) {
#ifndef CHARSET_EBCDIC
            spec_char = (!(((buf[j] >= '0') && (buf[j] <= '9')) ||
                  ((buf[j] >= 'a') && (buf[j] <= 'f')) ||
                  ((buf[j] >= 'A') && (buf[j] <= 'F'))));
#else
            /*
             * This #ifdef is not strictly necessary, since the characters
             * A...F a...f 0...9 are contiguous (yes, even in EBCDIC - but
             * not the whole alphabet). Nevertheless, isxdigit() is faster.
             */
            spec_char = (!isxdigit(buf[j]));
#endif
            if (spec_char) {
                i = j;
                break;
            }
        }
        buf[i] = '\0';
        /*
         * We have now cleared all the crap off the end of the line
         */
        if (i < 2)
            goto err;

        bufp = (unsigned char *)buf;

        k = 0;
        i -= again;
        if (i % 2 != 0) {
            ASN1err(ASN1_F_A2I_ASN1_STRING, ASN1_R_ODD_NUMBER_OF_CHARS);
            OPENSSL_free(s);
            return 0;
        }
        i /= 2;
        if (num + i > slen) {
            sp = OPENSSL_realloc(s, (unsigned int)num + i * 2);
            if (sp == NULL) {
                ASN1err(ASN1_F_A2I_ASN1_STRING, ERR_R_MALLOC_FAILURE);
                OPENSSL_free(s);
                return 0;
            }
            s = sp;
            slen = num + i * 2;
        }
        for (j = 0; j < i; j++, k += 2) {
            for (n = 0; n < 2; n++) {
                m = OPENSSL_hexchar2int(bufp[k + n]);
                if (m < 0) {
                    ASN1err(ASN1_F_A2I_ASN1_STRING,
                            ASN1_R_NON_HEX_CHARACTERS);
                    OPENSSL_free(s);
                    return 0;
                }
                s[num + j] <<= 4;
                s[num + j] |= m;
            }
        }
        num += i;
        if (again)
            bufsize = BIO_gets(bp, buf, size);
        else
            break;
    }
    bs->length = num;
    bs->data = s;
    return 1;

 err:
    ASN1err(ASN1_F_A2I_ASN1_STRING, ASN1_R_SHORT_LINE);
    OPENSSL_free(s);
    return 0;
}
