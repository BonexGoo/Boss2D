/*
 * Copyright 1999-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef HEADER_PKCS12_H
# define HEADER_PKCS12_H

# include BOSS_OPENSSL_V_openssl__bio_h //original-code:<openssl/bio.h>
# include BOSS_OPENSSL_V_openssl__x509_h //original-code:<openssl/x509.h>

#ifdef __cplusplus
extern "C" {
#endif

# define PKCS12_KEY_ID   1
# define PKCS12_IV_ID    2
# define PKCS12_MAC_ID   3

/* Default iteration count */
# ifndef PKCS12_DEFAULT_ITER
#  define PKCS12_DEFAULT_ITER     PKCS5_DEFAULT_ITER
# endif

# define PKCS12_MAC_KEY_LENGTH 20

# define PKCS12_SALT_LEN 8

/* It's not clear if these are actually needed... */
# define PKCS12_key_gen PKCS12_key_gen_utf8
# define PKCS12_add_friendlyname PKCS12_add_friendlyname_utf8

/* MS key usage constants */

# define KEY_EX  0x10
# define KEY_SIG 0x80

typedef struct PKCS12_MAC_DATA_st PKCS12_MAC_DATA;

typedef struct PKCS12_st PKCS12;

typedef struct PKCS12_SAFEBAG_st PKCS12_SAFEBAG;

DEFINE_STACK_OF(PKCS12_SAFEBAG)

typedef struct pkcs12_bag_st PKCS12_BAGS;

# define PKCS12_ERROR    0
# define PKCS12_OK       1

/* Compatibility macros */

#if OPENSSL_API_COMPAT < 0x10100000L

# define M_PKCS12_bag_type PKCS12_bag_type
# define M_PKCS12_cert_bag_type PKCS12_cert_bag_type
# define M_PKCS12_crl_bag_type PKCS12_cert_bag_type

# define PKCS12_certbag2x509 PKCS12_SAFEBAG_get1_cert
# define PKCS12_certbag2scrl PKCS12_SAFEBAG_get1_crl
# define PKCS12_bag_type PKCS12_SAFEBAG_get_nid
# define PKCS12_cert_bag_type PKCS12_SAFEBAG_get_bag_nid
# define PKCS12_x5092certbag PKCS12_SAFEBAG_create_cert
# define PKCS12_x509crl2certbag PKCS12_SAFEBAG_create_crl
# define PKCS12_MAKE_KEYBAG PKCS12_SAFEBAG_create0_p8inf
# define PKCS12_MAKE_SHKEYBAG PKCS12_SAFEBAG_create_pkcs8_encrypt

#endif

DEPRECATEDIN_1_1_0(ASN1_TYPE *PKCS12_get_attr(const PKCS12_SAFEBAG *bag, int attr_nid))

ASN1_TYPE *PKCS8_get_attr(PKCS8_PRIV_KEY_INFO *p8, int attr_nid);
int PKCS12_mac_present(const PKCS12 *p12);
void PKCS12_get0_mac(const ASN1_OCTET_STRING **pmac,
                     const X509_ALGOR **pmacalg,
                     const ASN1_OCTET_STRING **psalt,
                     const ASN1_INTEGER **piter,
                     const PKCS12 *p12);

const ASN1_TYPE *PKCS12_SAFEBAG_get0_attr(const PKCS12_SAFEBAG *bag,
                                          int attr_nid);
const ASN1_OBJECT *PKCS12_SAFEBAG_get0_type(const PKCS12_SAFEBAG *bag);
int PKCS12_SAFEBAG_get_nid(const PKCS12_SAFEBAG *bag);
int PKCS12_SAFEBAG_get_bag_nid(const PKCS12_SAFEBAG *bag);

X509 *PKCS12_SAFEBAG_get1_cert(const PKCS12_SAFEBAG *bag);
X509_CRL *PKCS12_SAFEBAG_get1_crl(const PKCS12_SAFEBAG *bag);
const STACK_OF(PKCS12_SAFEBAG) *
PKCS12_SAFEBAG_get0_safes(const PKCS12_SAFEBAG *bag);
const PKCS8_PRIV_KEY_INFO *PKCS12_SAFEBAG_get0_p8inf(const PKCS12_SAFEBAG *bag);
const X509_SIG *PKCS12_SAFEBAG_get0_pkcs8(const PKCS12_SAFEBAG *bag);

PKCS12_SAFEBAG *PKCS12_SAFEBAG_create_cert(X509 *x509);
PKCS12_SAFEBAG *PKCS12_SAFEBAG_create_crl(X509_CRL *crl);
PKCS12_SAFEBAG *PKCS12_SAFEBAG_create0_p8inf(PKCS8_PRIV_KEY_INFO *p8);
PKCS12_SAFEBAG *PKCS12_SAFEBAG_create0_pkcs8(X509_SIG *p8);
PKCS12_SAFEBAG *PKCS12_SAFEBAG_create_pkcs8_encrypt(int pbe_nid,
                                                    const char *pass,
                                                    int passlen,
                                                    unsigned char *salt,
                                                    int saltlen, int iter,
                                                    PKCS8_PRIV_KEY_INFO *p8inf);

PKCS12_SAFEBAG *PKCS12_item_pack_safebag(void *obj, const ASN1_ITEM *it,
                                         int nid1, int nid2);
PKCS8_PRIV_KEY_INFO *PKCS8_decrypt(const X509_SIG *p8, const char *pass,
                                   int passlen);
PKCS8_PRIV_KEY_INFO *PKCS12_decrypt_skey(const PKCS12_SAFEBAG *bag,
                                         const char *pass, int passlen);
X509_SIG *PKCS8_encrypt(int pbe_nid, const EVP_CIPHER *cipher,
                        const char *pass, int passlen, unsigned char *salt,
                        int saltlen, int iter, PKCS8_PRIV_KEY_INFO *p8);
X509_SIG *PKCS8_set0_pbe(const char *pass, int passlen,
                        PKCS8_PRIV_KEY_INFO *p8inf, X509_ALGOR *pbe);
PKCS7 *PKCS12_pack_p7data(STACK_OF(PKCS12_SAFEBAG) *sk);
STACK_OF(PKCS12_SAFEBAG) *PKCS12_unpack_p7data(PKCS7 *p7);
PKCS7 *PKCS12_pack_p7encdata(int pbe_nid, const char *pass, int passlen,
                             unsigned char *salt, int saltlen, int iter,
                             STACK_OF(PKCS12_SAFEBAG) *bags);
STACK_OF(PKCS12_SAFEBAG) *PKCS12_unpack_p7encdata(PKCS7 *p7, const char *pass,
                                                  int passlen);

int PKCS12_pack_authsafes(PKCS12 *p12, STACK_OF(PKCS7) *safes);
STACK_OF(PKCS7) *PKCS12_unpack_authsafes(const PKCS12 *p12);

int PKCS12_add_localkeyid(PKCS12_SAFEBAG *bag, unsigned char *name,
                          int namelen);
int PKCS12_add_friendlyname_asc(PKCS12_SAFEBAG *bag, const char *name,
                                int namelen);
int PKCS12_add_friendlyname_utf8(PKCS12_SAFEBAG *bag, const char *name,
                                 int namelen);
int PKCS12_add_CSPName_asc(PKCS12_SAFEBAG *bag, const char *name,
                           int namelen);
int PKCS12_add_friendlyname_uni(PKCS12_SAFEBAG *bag,
                                const unsigned char *name, int namelen);
int PKCS8_add_keyusage(PKCS8_PRIV_KEY_INFO *p8, int usage);
ASN1_TYPE *PKCS12_get_attr_gen(const STACK_OF(X509_ATTRIBUTE) *attrs,
                               int attr_nid);
char *PKCS12_get_friendlyname(PKCS12_SAFEBAG *bag);
const STACK_OF(X509_ATTRIBUTE) *
PKCS12_SAFEBAG_get0_attrs(const PKCS12_SAFEBAG *bag);
unsigned char *PKCS12_pbe_crypt(const X509_ALGOR *algor,
                                const char *pass, int passlen,
                                const unsigned char *in, int inlen,
                                unsigned char **data, int *datalen,
                                int en_de);
void *PKCS12_item_decrypt_d2i(const X509_ALGOR *algor, const ASN1_ITEM *it,
                              const char *pass, int passlen,
                              const ASN1_OCTET_STRING *oct, int zbuf);
ASN1_OCTET_STRING *PKCS12_item_i2d_encrypt(X509_ALGOR *algor,
                                           const ASN1_ITEM *it,
                                           const char *pass, int passlen,
                                           void *obj, int zbuf);
PKCS12 *PKCS12_init(int mode);
int PKCS12_key_gen_asc(const char *pass, int passlen, unsigned char *salt,
                       int saltlen, int id, int iter, int n,
                       unsigned char *out, const EVP_MD *md_type);
int PKCS12_key_gen_uni(unsigned char *pass, int passlen, unsigned char *salt,
                       int saltlen, int id, int iter, int n,
                       unsigned char *out, const EVP_MD *md_type);
int PKCS12_key_gen_utf8(const char *pass, int passlen, unsigned char *salt,
                        int saltlen, int id, int iter, int n,
                        unsigned char *out, const EVP_MD *md_type);
int PKCS12_PBE_keyivgen(EVP_CIPHER_CTX *ctx, const char *pass, int passlen,
                        ASN1_TYPE *param, const EVP_CIPHER *cipher,
                        const EVP_MD *md_type, int en_de);
int PKCS12_gen_mac(PKCS12 *p12, const char *pass, int passlen,
                   unsigned char *mac, unsigned int *maclen);
int PKCS12_verify_mac(PKCS12 *p12, const char *pass, int passlen);
int PKCS12_set_mac(PKCS12 *p12, const char *pass, int passlen,
                   unsigned char *salt, int saltlen, int iter,
                   const EVP_MD *md_type);
int PKCS12_setup_mac(PKCS12 *p12, int iter, unsigned char *salt,
                     int saltlen, const EVP_MD *md_type);
unsigned char *OPENSSL_asc2uni(const char *asc, int asclen,
                               unsigned char **uni, int *unilen);
char *OPENSSL_uni2asc(const unsigned char *uni, int unilen);
unsigned char *OPENSSL_utf82uni(const char *asc, int asclen,
                                unsigned char **uni, int *unilen);
char *OPENSSL_uni2utf8(const unsigned char *uni, int unilen);

DECLARE_ASN1_FUNCTIONS(PKCS12)
DECLARE_ASN1_FUNCTIONS(PKCS12_MAC_DATA)
DECLARE_ASN1_FUNCTIONS(PKCS12_SAFEBAG)
DECLARE_ASN1_FUNCTIONS(PKCS12_BAGS)

DECLARE_ASN1_ITEM(PKCS12_SAFEBAGS)
DECLARE_ASN1_ITEM(PKCS12_AUTHSAFES)

void PKCS12_PBE_add(void);
int PKCS12_parse(PKCS12 *p12, const char *pass, EVP_PKEY **pkey, X509 **cert,
                 STACK_OF(X509) **ca);
PKCS12 *PKCS12_create(const char *pass, const char *name, EVP_PKEY *pkey,
                      X509 *cert, STACK_OF(X509) *ca, int nid_key, int nid_cert,
                      int iter, int mac_iter, int keytype);

PKCS12_SAFEBAG *PKCS12_add_cert(STACK_OF(PKCS12_SAFEBAG) **pbags, X509 *cert);
PKCS12_SAFEBAG *PKCS12_add_key(STACK_OF(PKCS12_SAFEBAG) **pbags,
                               EVP_PKEY *key, int key_usage, int iter,
                               int key_nid, const char *pass);
int PKCS12_add_safe(STACK_OF(PKCS7) **psafes, STACK_OF(PKCS12_SAFEBAG) *bags,
                    int safe_nid, int iter, const char *pass);
PKCS12 *PKCS12_add_safes(STACK_OF(PKCS7) *safes, int p7_nid);

int i2d_PKCS12_bio(BIO *bp, PKCS12 *p12);
# ifndef OPENSSL_NO_STDIO
int i2d_PKCS12_fp(FILE *fp, PKCS12 *p12);
# endif
PKCS12 *d2i_PKCS12_bio(BIO *bp, PKCS12 **p12);
# ifndef OPENSSL_NO_STDIO
PKCS12 *d2i_PKCS12_fp(FILE *fp, PKCS12 **p12);
# endif
int PKCS12_newpass(PKCS12 *p12, const char *oldpass, const char *newpass);

/* BEGIN ERROR CODES */
/*
 * The following lines are auto generated by the script mkerr.pl. Any changes
 * made after this point may be overwritten when the script is next run.
 */

int ERR_load_PKCS12_strings(void);

/* Error codes for the PKCS12 functions. */

/* Function codes. */
# define PKCS12_F_PKCS12_CREATE                           105
# define PKCS12_F_PKCS12_GEN_MAC                          107
# define PKCS12_F_PKCS12_INIT                             109
# define PKCS12_F_PKCS12_ITEM_DECRYPT_D2I                 106
# define PKCS12_F_PKCS12_ITEM_I2D_ENCRYPT                 108
# define PKCS12_F_PKCS12_ITEM_PACK_SAFEBAG                117
# define PKCS12_F_PKCS12_KEY_GEN_ASC                      110
# define PKCS12_F_PKCS12_KEY_GEN_UNI                      111
# define PKCS12_F_PKCS12_KEY_GEN_UTF8                     116
# define PKCS12_F_PKCS12_NEWPASS                          128
# define PKCS12_F_PKCS12_PACK_P7DATA                      114
# define PKCS12_F_PKCS12_PACK_P7ENCDATA                   115
# define PKCS12_F_PKCS12_PARSE                            118
# define PKCS12_F_PKCS12_PBE_CRYPT                        119
# define PKCS12_F_PKCS12_PBE_KEYIVGEN                     120
# define PKCS12_F_PKCS12_SAFEBAG_CREATE0_P8INF            112
# define PKCS12_F_PKCS12_SAFEBAG_CREATE0_PKCS8            113
# define PKCS12_F_PKCS12_SAFEBAG_CREATE_PKCS8_ENCRYPT     133
# define PKCS12_F_PKCS12_SETUP_MAC                        122
# define PKCS12_F_PKCS12_SET_MAC                          123
# define PKCS12_F_PKCS12_UNPACK_AUTHSAFES                 130
# define PKCS12_F_PKCS12_UNPACK_P7DATA                    131
# define PKCS12_F_PKCS12_VERIFY_MAC                       126
# define PKCS12_F_PKCS8_ENCRYPT                           125
# define PKCS12_F_PKCS8_SET0_PBE                          132

/* Reason codes. */
# define PKCS12_R_CANT_PACK_STRUCTURE                     100
# define PKCS12_R_CONTENT_TYPE_NOT_DATA                   121
# define PKCS12_R_DECODE_ERROR                            101
# define PKCS12_R_ENCODE_ERROR                            102
# define PKCS12_R_ENCRYPT_ERROR                           103
# define PKCS12_R_ERROR_SETTING_ENCRYPTED_DATA_TYPE       120
# define PKCS12_R_INVALID_NULL_ARGUMENT                   104
# define PKCS12_R_INVALID_NULL_PKCS12_POINTER             105
# define PKCS12_R_IV_GEN_ERROR                            106
# define PKCS12_R_KEY_GEN_ERROR                           107
# define PKCS12_R_MAC_ABSENT                              108
# define PKCS12_R_MAC_GENERATION_ERROR                    109
# define PKCS12_R_MAC_SETUP_ERROR                         110
# define PKCS12_R_MAC_STRING_SET_ERROR                    111
# define PKCS12_R_MAC_VERIFY_FAILURE                      113
# define PKCS12_R_PARSE_ERROR                             114
# define PKCS12_R_PKCS12_ALGOR_CIPHERINIT_ERROR           115
# define PKCS12_R_PKCS12_CIPHERFINAL_ERROR                116
# define PKCS12_R_PKCS12_PBE_CRYPT_ERROR                  117
# define PKCS12_R_UNKNOWN_DIGEST_ALGORITHM                118
# define PKCS12_R_UNSUPPORTED_PKCS12_MODE                 119

# ifdef  __cplusplus
}
# endif
#endif
