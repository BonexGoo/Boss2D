/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <time.h>
#include <errno.h>

#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_V_openssl__lhash_h //original-code:<openssl/lhash.h>
#include BOSS_OPENSSL_V_openssl__buffer_h //original-code:<openssl/buffer.h>
#include BOSS_OPENSSL_V_openssl__x509_h //original-code:<openssl/x509.h>
#include BOSS_OPENSSL_V_openssl__pem_h //original-code:<openssl/pem.h>
#include "x509_lcl.h"

static int by_file_ctrl(X509_LOOKUP *ctx, int cmd, const char *argc,
                        long argl, char **ret);
static X509_LOOKUP_METHOD x509_file_lookup = {
    "Load file into cache",
    NULL,                       /* new */
    NULL,                       /* free */
    NULL,                       /* init */
    NULL,                       /* shutdown */
    by_file_ctrl,               /* ctrl */
    NULL,                       /* get_by_subject */
    NULL,                       /* get_by_issuer_serial */
    NULL,                       /* get_by_fingerprint */
    NULL,                       /* get_by_alias */
};

X509_LOOKUP_METHOD *X509_LOOKUP_file(void)
{
    return (&x509_file_lookup);
}

static int by_file_ctrl(X509_LOOKUP *ctx, int cmd, const char *argp,
                        long argl, char **ret)
{
    int ok = 0;
    char *file;

    switch (cmd) {
    case X509_L_FILE_LOAD:
        if (argl == X509_FILETYPE_DEFAULT) {
            file = (char *)getenv(X509_get_default_cert_file_env());
            if (file)
                ok = (X509_load_cert_crl_file(ctx, file,
                                              X509_FILETYPE_PEM) != 0);

            else
                ok = (X509_load_cert_crl_file
                      (ctx, X509_get_default_cert_file(),
                       X509_FILETYPE_PEM) != 0);

            if (!ok) {
                X509err(X509_F_BY_FILE_CTRL, X509_R_LOADING_DEFAULTS);
            }
        } else {
            if (argl == X509_FILETYPE_PEM)
                ok = (X509_load_cert_crl_file(ctx, argp,
                                              X509_FILETYPE_PEM) != 0);
            else
                ok = (X509_load_cert_file(ctx, argp, (int)argl) != 0);
        }
        break;
    }
    return (ok);
}

int X509_load_cert_file(X509_LOOKUP *ctx, const char *file, int type)
{
    int ret = 0;
    BIO *in = NULL;
    int i, count = 0;
    X509 *x = NULL;

    if (file == NULL)
        return (1);
    in = BIO_new(BIO_s_file());

    if ((in == NULL) || (BIO_read_filename(in, file) <= 0)) {
        X509err(X509_F_X509_LOAD_CERT_FILE, ERR_R_SYS_LIB);
        goto err;
    }

    if (type == X509_FILETYPE_PEM) {
        for (;;) {
            x = PEM_read_bio_X509_AUX(in, NULL, NULL, NULL);
            if (x == NULL) {
                if ((ERR_GET_REASON(ERR_peek_last_error()) ==
                     PEM_R_NO_START_LINE) && (count > 0)) {
                    ERR_clear_error();
                    break;
                } else {
                    X509err(X509_F_X509_LOAD_CERT_FILE, ERR_R_PEM_LIB);
                    goto err;
                }
            }
            i = X509_STORE_add_cert(ctx->store_ctx, x);
            if (!i)
                goto err;
            count++;
            X509_free(x);
            x = NULL;
        }
        ret = count;
    } else if (type == X509_FILETYPE_ASN1) {
        x = d2i_X509_bio(in, NULL);
        if (x == NULL) {
            X509err(X509_F_X509_LOAD_CERT_FILE, ERR_R_ASN1_LIB);
            goto err;
        }
        i = X509_STORE_add_cert(ctx->store_ctx, x);
        if (!i)
            goto err;
        ret = i;
    } else {
        X509err(X509_F_X509_LOAD_CERT_FILE, X509_R_BAD_X509_FILETYPE);
        goto err;
    }
 err:
    X509_free(x);
    BIO_free(in);
    return (ret);
}

int X509_load_crl_file(X509_LOOKUP *ctx, const char *file, int type)
{
    int ret = 0;
    BIO *in = NULL;
    int i, count = 0;
    X509_CRL *x = NULL;

    if (file == NULL)
        return (1);
    in = BIO_new(BIO_s_file());

    if ((in == NULL) || (BIO_read_filename(in, file) <= 0)) {
        X509err(X509_F_X509_LOAD_CRL_FILE, ERR_R_SYS_LIB);
        goto err;
    }

    if (type == X509_FILETYPE_PEM) {
        for (;;) {
            x = PEM_read_bio_X509_CRL(in, NULL, NULL, NULL);
            if (x == NULL) {
                if ((ERR_GET_REASON(ERR_peek_last_error()) ==
                     PEM_R_NO_START_LINE) && (count > 0)) {
                    ERR_clear_error();
                    break;
                } else {
                    X509err(X509_F_X509_LOAD_CRL_FILE, ERR_R_PEM_LIB);
                    goto err;
                }
            }
            i = X509_STORE_add_crl(ctx->store_ctx, x);
            if (!i)
                goto err;
            count++;
            X509_CRL_free(x);
            x = NULL;
        }
        ret = count;
    } else if (type == X509_FILETYPE_ASN1) {
        x = d2i_X509_CRL_bio(in, NULL);
        if (x == NULL) {
            X509err(X509_F_X509_LOAD_CRL_FILE, ERR_R_ASN1_LIB);
            goto err;
        }
        i = X509_STORE_add_crl(ctx->store_ctx, x);
        if (!i)
            goto err;
        ret = i;
    } else {
        X509err(X509_F_X509_LOAD_CRL_FILE, X509_R_BAD_X509_FILETYPE);
        goto err;
    }
 err:
    X509_CRL_free(x);
    BIO_free(in);
    return (ret);
}

int X509_load_cert_crl_file(X509_LOOKUP *ctx, const char *file, int type)
{
    STACK_OF(X509_INFO) *inf;
    X509_INFO *itmp;
    BIO *in;
    int i, count = 0;
    if (type != X509_FILETYPE_PEM)
        return X509_load_cert_file(ctx, file, type);
    in = BIO_new_file(file, "r");
    if (!in) {
        X509err(X509_F_X509_LOAD_CERT_CRL_FILE, ERR_R_SYS_LIB);
        return 0;
    }
    inf = PEM_X509_INFO_read_bio(in, NULL, NULL, NULL);
    BIO_free(in);
    if (!inf) {
        X509err(X509_F_X509_LOAD_CERT_CRL_FILE, ERR_R_PEM_LIB);
        return 0;
    }
    for (i = 0; i < sk_X509_INFO_num(inf); i++) {
        itmp = sk_X509_INFO_value(inf, i);
        if (itmp->x509) {
            X509_STORE_add_cert(ctx->store_ctx, itmp->x509);
            count++;
        }
        if (itmp->crl) {
            X509_STORE_add_crl(ctx->store_ctx, itmp->crl);
            count++;
        }
    }
    sk_X509_INFO_pop_free(inf, X509_INFO_free);
    return count;
}
