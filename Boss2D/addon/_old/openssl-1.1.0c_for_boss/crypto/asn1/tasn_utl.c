/*
 * Copyright 2000-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stddef.h>
#include <string.h>
#include BOSS_OPENSSL_V_internal__cryptlib_h //original-code:<internal/cryptlib.h>
#include BOSS_OPENSSL_V_openssl__asn1_h //original-code:<openssl/asn1.h>
#include BOSS_OPENSSL_V_openssl__asn1t_h //original-code:<openssl/asn1t.h>
#include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#include "asn1_locl.h"

/* Utility functions for manipulating fields and offsets */

/* Add 'offset' to 'addr' */
#define offset2ptr(addr, offset) (void *)(((char *) addr) + offset)

/*
 * Given an ASN1_ITEM CHOICE type return the selector value
 */

int asn1_get_choice_selector(ASN1_VALUE **pval, const ASN1_ITEM *it)
{
    int *sel = offset2ptr(*pval, it->utype);
    return *sel;
}

/*
 * Given an ASN1_ITEM CHOICE type set the selector value, return old value.
 */

int asn1_set_choice_selector(ASN1_VALUE **pval, int value,
                             const ASN1_ITEM *it)
{
    int *sel, ret;
    sel = offset2ptr(*pval, it->utype);
    ret = *sel;
    *sel = value;
    return ret;
}

/*
 * Do atomic reference counting. The value 'op' decides what to do.
 * If it is +1 then the count is incremented.
 * If |op| is 0, lock is initialised and count is set to 1.
 * If |op| is -1, count is decremented and the return value is the current
 * reference count or 0 if no reference count is active.
 * It returns -1 on initialisation error.
 * Used by ASN1_SEQUENCE construct of X509, X509_REQ, X509_CRL objects
 */
int asn1_do_lock(ASN1_VALUE **pval, int op, const ASN1_ITEM *it)
{
    const ASN1_AUX *aux;
    int *lck, ret;
    CRYPTO_RWLOCK **lock;
    if ((it->itype != ASN1_ITYPE_SEQUENCE)
        && (it->itype != ASN1_ITYPE_NDEF_SEQUENCE))
        return 0;
    aux = it->funcs;
    if (!aux || !(aux->flags & ASN1_AFLG_REFCOUNT))
        return 0;
    lck = offset2ptr(*pval, aux->ref_offset);
    lock = offset2ptr(*pval, aux->ref_lock);
    if (op == 0) {
        *lck = 1;
        *lock = CRYPTO_THREAD_lock_new();
        if (*lock == NULL) {
            ASN1err(ASN1_F_ASN1_DO_LOCK, ERR_R_MALLOC_FAILURE);
            return -1;
        }
        return 1;
    }
    if (CRYPTO_atomic_add(lck, op, &ret, *lock) < 0)
        return -1;  /* failed */
#ifdef REF_PRINT
    fprintf(stderr, "%p:%4d:%s\n", it, *lck, it->sname);
#endif
    REF_ASSERT_ISNT(ret < 0);
    if (ret == 0) {
        CRYPTO_THREAD_lock_free(*lock);
        *lock = NULL;
    }
    return ret;
}

static ASN1_ENCODING *asn1_get_enc_ptr(ASN1_VALUE **pval, const ASN1_ITEM *it)
{
    const ASN1_AUX *aux;
    if (!pval || !*pval)
        return NULL;
    aux = it->funcs;
    if (!aux || !(aux->flags & ASN1_AFLG_ENCODING))
        return NULL;
    return offset2ptr(*pval, aux->enc_offset);
}

void asn1_enc_init(ASN1_VALUE **pval, const ASN1_ITEM *it)
{
    ASN1_ENCODING *enc;
    enc = asn1_get_enc_ptr(pval, it);
    if (enc) {
        enc->enc = NULL;
        enc->len = 0;
        enc->modified = 1;
    }
}

void asn1_enc_free(ASN1_VALUE **pval, const ASN1_ITEM *it)
{
    ASN1_ENCODING *enc;
    enc = asn1_get_enc_ptr(pval, it);
    if (enc) {
        OPENSSL_free(enc->enc);
        enc->enc = NULL;
        enc->len = 0;
        enc->modified = 1;
    }
}

int asn1_enc_save(ASN1_VALUE **pval, const unsigned char *in, int inlen,
                  const ASN1_ITEM *it)
{
    ASN1_ENCODING *enc;
    enc = asn1_get_enc_ptr(pval, it);
    if (!enc)
        return 1;

    OPENSSL_free(enc->enc);
    enc->enc = OPENSSL_malloc(inlen);
    if (enc->enc == NULL)
        return 0;
    memcpy(enc->enc, in, inlen);
    enc->len = inlen;
    enc->modified = 0;

    return 1;
}

int asn1_enc_restore(int *len, unsigned char **out, ASN1_VALUE **pval,
                     const ASN1_ITEM *it)
{
    ASN1_ENCODING *enc;
    enc = asn1_get_enc_ptr(pval, it);
    if (!enc || enc->modified)
        return 0;
    if (out) {
        memcpy(*out, enc->enc, enc->len);
        *out += enc->len;
    }
    if (len)
        *len = enc->len;
    return 1;
}

/* Given an ASN1_TEMPLATE get a pointer to a field */
ASN1_VALUE **asn1_get_field_ptr(ASN1_VALUE **pval, const ASN1_TEMPLATE *tt)
{
    ASN1_VALUE **pvaltmp;
    pvaltmp = offset2ptr(*pval, tt->offset);
    /*
     * NOTE for BOOLEAN types the field is just a plain int so we can't
     * return int **, so settle for (int *).
     */
    return pvaltmp;
}

/*
 * Handle ANY DEFINED BY template, find the selector, look up the relevant
 * ASN1_TEMPLATE in the table and return it.
 */

const ASN1_TEMPLATE *asn1_do_adb(ASN1_VALUE **pval, const ASN1_TEMPLATE *tt,
                                 int nullerr)
{
    const ASN1_ADB *adb;
    const ASN1_ADB_TABLE *atbl;
    long selector;
    ASN1_VALUE **sfld;
    int i;
    if (!(tt->flags & ASN1_TFLG_ADB_MASK))
        return tt;

    /* Else ANY DEFINED BY ... get the table */
    adb = ASN1_ADB_ptr(tt->item);

    /* Get the selector field */
    sfld = offset2ptr(*pval, adb->offset);

    /* Check if NULL */
    if (*sfld == NULL) {
        if (!adb->null_tt)
            goto err;
        return adb->null_tt;
    }

    /*
     * Convert type to a long: NB: don't check for NID_undef here because it
     * might be a legitimate value in the table
     */
    if (tt->flags & ASN1_TFLG_ADB_OID)
        selector = OBJ_obj2nid((ASN1_OBJECT *)*sfld);
    else
        selector = ASN1_INTEGER_get((ASN1_INTEGER *)*sfld);

    /* Let application callback translate value */
    if (adb->adb_cb != NULL && adb->adb_cb(&selector) == 0) {
        ASN1err(ASN1_F_ASN1_DO_ADB, ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE);
        return NULL;
    }

    /*
     * Try to find matching entry in table Maybe should check application
     * types first to allow application override? Might also be useful to
     * have a flag which indicates table is sorted and we can do a binary
     * search. For now stick to a linear search.
     */

    for (atbl = adb->tbl, i = 0; i < adb->tblcount; i++, atbl++)
        if (atbl->value == selector)
            return &atbl->tt;

    /* FIXME: need to search application table too */

    /* No match, return default type */
    if (!adb->default_tt)
        goto err;
    return adb->default_tt;

 err:
    /* FIXME: should log the value or OID of unsupported type */
    if (nullerr)
        ASN1err(ASN1_F_ASN1_DO_ADB, ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE);
    return NULL;
}
