/*
 * Copyright 2000-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include BOSS_OPENSSL_U_internal__cryptlib_h //original-code:"internal/cryptlib.h"
#include BOSS_OPENSSL_V_openssl__asn1t_h //original-code:<openssl/asn1t.h>
#include BOSS_OPENSSL_V_openssl__pkcs12_h //original-code:<openssl/pkcs12.h>
#include "p12_lcl.h"

/* PKCS#12 ASN1 module */

ASN1_SEQUENCE(PKCS12) = {
        ASN1_SIMPLE(PKCS12, version, ASN1_INTEGER),
        ASN1_SIMPLE(PKCS12, authsafes, PKCS7),
        ASN1_OPT(PKCS12, mac, PKCS12_MAC_DATA)
} ASN1_SEQUENCE_END(PKCS12)

IMPLEMENT_ASN1_FUNCTIONS(PKCS12)

ASN1_SEQUENCE(PKCS12_MAC_DATA) = {
        ASN1_SIMPLE(PKCS12_MAC_DATA, dinfo, X509_SIG),
        ASN1_SIMPLE(PKCS12_MAC_DATA, salt, ASN1_OCTET_STRING),
        ASN1_OPT(PKCS12_MAC_DATA, iter, ASN1_INTEGER)
} ASN1_SEQUENCE_END(PKCS12_MAC_DATA)

IMPLEMENT_ASN1_FUNCTIONS(PKCS12_MAC_DATA)

ASN1_ADB_TEMPLATE(bag_default) = ASN1_EXP(PKCS12_BAGS, value.other, ASN1_ANY, 0);

ASN1_ADB(PKCS12_BAGS) = {
        ADB_ENTRY(NID_x509Certificate, ASN1_EXP(PKCS12_BAGS, value.x509cert, ASN1_OCTET_STRING, 0)),
        ADB_ENTRY(NID_x509Crl, ASN1_EXP(PKCS12_BAGS, value.x509crl, ASN1_OCTET_STRING, 0)),
        ADB_ENTRY(NID_sdsiCertificate, ASN1_EXP(PKCS12_BAGS, value.sdsicert, ASN1_IA5STRING, 0)),
} ASN1_ADB_END(PKCS12_BAGS, 0, type, 0, &bag_default_tt, NULL);

ASN1_SEQUENCE(PKCS12_BAGS) = {
        ASN1_SIMPLE(PKCS12_BAGS, type, ASN1_OBJECT),
        ASN1_ADB_OBJECT(PKCS12_BAGS),
} ASN1_SEQUENCE_END(PKCS12_BAGS)

IMPLEMENT_ASN1_FUNCTIONS(PKCS12_BAGS)

ASN1_ADB_TEMPLATE(safebag_default) = ASN1_EXP(PKCS12_SAFEBAG, value.other, ASN1_ANY, 0);

ASN1_ADB(PKCS12_SAFEBAG) = {
        ADB_ENTRY(NID_keyBag, ASN1_EXP(PKCS12_SAFEBAG, value.keybag, PKCS8_PRIV_KEY_INFO, 0)),
        ADB_ENTRY(NID_pkcs8ShroudedKeyBag, ASN1_EXP(PKCS12_SAFEBAG, value.shkeybag, X509_SIG, 0)),
        ADB_ENTRY(NID_safeContentsBag, ASN1_EXP_SEQUENCE_OF(PKCS12_SAFEBAG, value.safes, PKCS12_SAFEBAG, 0)),
        ADB_ENTRY(NID_certBag, ASN1_EXP(PKCS12_SAFEBAG, value.bag, PKCS12_BAGS, 0)),
        ADB_ENTRY(NID_crlBag, ASN1_EXP(PKCS12_SAFEBAG, value.bag, PKCS12_BAGS, 0)),
        ADB_ENTRY(NID_secretBag, ASN1_EXP(PKCS12_SAFEBAG, value.bag, PKCS12_BAGS, 0))
} ASN1_ADB_END(PKCS12_SAFEBAG, 0, type, 0, &safebag_default_tt, NULL);

ASN1_SEQUENCE(PKCS12_SAFEBAG) = {
        ASN1_SIMPLE(PKCS12_SAFEBAG, type, ASN1_OBJECT),
        ASN1_ADB_OBJECT(PKCS12_SAFEBAG),
        ASN1_SET_OF_OPT(PKCS12_SAFEBAG, attrib, X509_ATTRIBUTE)
} ASN1_SEQUENCE_END(PKCS12_SAFEBAG)

IMPLEMENT_ASN1_FUNCTIONS(PKCS12_SAFEBAG)

/* SEQUENCE OF SafeBag */
ASN1_ITEM_TEMPLATE(PKCS12_SAFEBAGS) =
        ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_SEQUENCE_OF, 0, PKCS12_SAFEBAGS, PKCS12_SAFEBAG)
ASN1_ITEM_TEMPLATE_END(PKCS12_SAFEBAGS)

/* Authsafes: SEQUENCE OF PKCS7 */
ASN1_ITEM_TEMPLATE(PKCS12_AUTHSAFES) =
        ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_SEQUENCE_OF, 0, PKCS12_AUTHSAFES, PKCS7)
ASN1_ITEM_TEMPLATE_END(PKCS12_AUTHSAFES)
