/*
 * Copyright 2017-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include BOSS_OPENSSL_V_openssl__bio_h //original-code:<openssl/bio.h>
#include BOSS_OPENSSL_V_openssl__evp_h //original-code:<openssl/evp.h>
#include BOSS_OPENSSL_V_openssl__bn_h //original-code:<openssl/bn.h>
#include BOSS_OPENSSL_V_openssl__crypto_h //original-code:<openssl/crypto.h>
#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#include BOSS_OPENSSL_V_openssl__rand_h //original-code:<openssl/rand.h>
#include "testutil.h"

#ifndef OPENSSL_NO_SM2

# include BOSS_OPENSSL_U_internal__sm2_h //original-code:"internal/sm2.h"

static RAND_METHOD fake_rand;
static const RAND_METHOD *saved_rand;

static uint8_t *fake_rand_bytes = NULL;
static size_t fake_rand_bytes_offset = 0;
static size_t fake_rand_size = 0;

static int get_faked_bytes(unsigned char *buf, int num)
{
    int i;

    if (fake_rand_bytes == NULL)
        return saved_rand->bytes(buf, num);

    if (!TEST_size_t_le(fake_rand_bytes_offset + num, fake_rand_size))
        return 0;

    for (i = 0; i != num; ++i)
        buf[i] = fake_rand_bytes[fake_rand_bytes_offset + i];
    fake_rand_bytes_offset += num;
    return 1;
}

static int start_fake_rand(const char *hex_bytes)
{
    /* save old rand method */
    if (!TEST_ptr(saved_rand = RAND_get_rand_method()))
        return 0;

    fake_rand = *saved_rand;
    /* use own random function */
    fake_rand.bytes = get_faked_bytes;

    fake_rand_bytes = OPENSSL_hexstr2buf(hex_bytes, NULL);
    fake_rand_bytes_offset = 0;
    fake_rand_size = strlen(hex_bytes) / 2;

    /* set new RAND_METHOD */
    if (!TEST_true(RAND_set_rand_method(&fake_rand)))
        return 0;
    return 1;
}

static int restore_rand(void)
{
    OPENSSL_free(fake_rand_bytes);
    fake_rand_bytes = NULL;
    fake_rand_bytes_offset = 0;
    if (!TEST_true(RAND_set_rand_method(saved_rand)))
        return 0;
    return 1;
}

static EC_GROUP *create_EC_group(const char *p_hex, const char *a_hex,
                                 const char *b_hex, const char *x_hex,
                                 const char *y_hex, const char *order_hex,
                                 const char *cof_hex)
{
    BIGNUM *p = NULL;
    BIGNUM *a = NULL;
    BIGNUM *b = NULL;
    BIGNUM *g_x = NULL;
    BIGNUM *g_y = NULL;
    BIGNUM *order = NULL;
    BIGNUM *cof = NULL;
    EC_POINT *generator = NULL;
    EC_GROUP *group = NULL;
    int ok = 0;

    if (!TEST_true(BN_hex2bn(&p, p_hex))
            || !TEST_true(BN_hex2bn(&a, a_hex))
            || !TEST_true(BN_hex2bn(&b, b_hex)))
        goto done;

    group = EC_GROUP_new_curve_GFp(p, a, b, NULL);
    if (!TEST_ptr(group))
        goto done;

    generator = EC_POINT_new(group);
    if (!TEST_ptr(generator))
        goto done;

    if (!TEST_true(BN_hex2bn(&g_x, x_hex))
            || !TEST_true(BN_hex2bn(&g_y, y_hex))
            || !TEST_true(EC_POINT_set_affine_coordinates(group, generator, g_x,
                                                          g_y, NULL)))
        goto done;

    if (!TEST_true(BN_hex2bn(&order, order_hex))
            || !TEST_true(BN_hex2bn(&cof, cof_hex))
            || !TEST_true(EC_GROUP_set_generator(group, generator, order, cof)))
        goto done;

    ok = 1;
done:
    BN_free(p);
    BN_free(a);
    BN_free(b);
    BN_free(g_x);
    BN_free(g_y);
    EC_POINT_free(generator);
    BN_free(order);
    BN_free(cof);
    if (!ok) {
        EC_GROUP_free(group);
        group = NULL;
    }

    return group;
}

static int test_sm2_crypt(const EC_GROUP *group,
                          const EVP_MD *digest,
                          const char *privkey_hex,
                          const char *message,
                          const char *k_hex, const char *ctext_hex)
{
    const size_t msg_len = strlen(message);
    BIGNUM *priv = NULL;
    EC_KEY *key = NULL;
    EC_POINT *pt = NULL;
    unsigned char *expected = OPENSSL_hexstr2buf(ctext_hex, NULL);
    size_t ctext_len = 0;
    size_t ptext_len = 0;
    uint8_t *ctext = NULL;
    uint8_t *recovered = NULL;
    size_t recovered_len = msg_len;
    int rc = 0;

    if (!TEST_ptr(expected)
            || !TEST_true(BN_hex2bn(&priv, privkey_hex)))
        goto done;

    key = EC_KEY_new();
    if (!TEST_ptr(key)
            || !TEST_true(EC_KEY_set_group(key, group))
            || !TEST_true(EC_KEY_set_private_key(key, priv)))
        goto done;

    pt = EC_POINT_new(group);
    if (!TEST_ptr(pt)
            || !TEST_true(EC_POINT_mul(group, pt, priv, NULL, NULL, NULL))
            || !TEST_true(EC_KEY_set_public_key(key, pt))
            || !TEST_true(sm2_ciphertext_size(key, digest, msg_len, &ctext_len)))
        goto done;

    ctext = OPENSSL_zalloc(ctext_len);
    if (!TEST_ptr(ctext))
        goto done;

    start_fake_rand(k_hex);
    if (!TEST_true(sm2_encrypt(key, digest, (const uint8_t *)message, msg_len,
                               ctext, &ctext_len))
            || !TEST_size_t_eq(fake_rand_bytes_offset, fake_rand_size)) {
        restore_rand();
        goto done;
    }
    restore_rand();

    if (!TEST_mem_eq(ctext, ctext_len, expected, ctext_len))
        goto done;

    if (!TEST_true(sm2_plaintext_size(key, digest, ctext_len, &ptext_len))
            || !TEST_int_eq(ptext_len, msg_len))
        goto done;

    recovered = OPENSSL_zalloc(ptext_len);
    if (!TEST_ptr(recovered)
            || !TEST_true(sm2_decrypt(key, digest, ctext, ctext_len, recovered, &recovered_len))
            || !TEST_int_eq(recovered_len, msg_len)
            || !TEST_mem_eq(recovered, recovered_len, message, msg_len))
        goto done;

    rc = 1;
 done:
    BN_free(priv);
    EC_POINT_free(pt);
    OPENSSL_free(ctext);
    OPENSSL_free(recovered);
    OPENSSL_free(expected);
    EC_KEY_free(key);
    return rc;
}

static int sm2_crypt_test(void)
{
    int testresult = 0;
    EC_GROUP *test_group =
        create_EC_group
        ("8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3",
         "787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498",
         "63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A",
         "421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D",
         "0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2",
         "8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7",
         "1");

    if (!TEST_ptr(test_group))
        goto done;

    if (!test_sm2_crypt(
            test_group,
            EVP_sm3(),
            "1649AB77A00637BD5E2EFE283FBF353534AA7F7CB89463F208DDBC2920BB0DA0",
            "encryption standard",
            "004C62EEFD6ECFC2B95B92FD6C3D9575148AFA17425546D49018E5388D49DD7B4F"
            "0092e8ff62146873c258557548500ab2df2a365e0609ab67640a1f6d57d7b17820"
            "008349312695a3e1d2f46905f39a766487f2432e95d6be0cb009fe8c69fd8825a7",
            "307B0220245C26FB68B1DDDDB12C4B6BF9F2B6D5FE60A383B0D18D1C4144ABF1"
            "7F6252E7022076CB9264C2A7E88E52B19903FDC47378F605E36811F5C07423A2"
            "4B84400F01B804209C3D7360C30156FAB7C80A0276712DA9D8094A634B766D3A"
            "285E07480653426D0413650053A89B41C418B0C3AAD00D886C00286467"))
        goto done;

    /* Same test as above except using SHA-256 instead of SM3 */
    if (!test_sm2_crypt(
            test_group,
            EVP_sha256(),
            "1649AB77A00637BD5E2EFE283FBF353534AA7F7CB89463F208DDBC2920BB0DA0",
            "encryption standard",
            "004C62EEFD6ECFC2B95B92FD6C3D9575148AFA17425546D49018E5388D49DD7B4F"
            "003da18008784352192d70f22c26c243174a447ba272fec64163dd4742bae8bc98"
            "00df17605cf304e9dd1dfeb90c015e93b393a6f046792f790a6fa4228af67d9588",
            "307B0220245C26FB68B1DDDDB12C4B6BF9F2B6D5FE60A383B0D18D1C4144ABF17F"
            "6252E7022076CB9264C2A7E88E52B19903FDC47378F605E36811F5C07423A24B84"
            "400F01B80420BE89139D07853100EFA763F60CBE30099EA3DF7F8F364F9D10A5E9"
            "88E3C5AAFC0413229E6C9AEE2BB92CAD649FE2C035689785DA33"))
        goto done;

    testresult = 1;
 done:
    EC_GROUP_free(test_group);

    return testresult;
}

static int test_sm2_sign(const EC_GROUP *group,
                         const char *userid,
                         const char *privkey_hex,
                         const char *message,
                         const char *k_hex,
                         const char *r_hex,
                         const char *s_hex)
{
    const size_t msg_len = strlen(message);
    int ok = 0;
    BIGNUM *priv = NULL;
    EC_POINT *pt = NULL;
    EC_KEY *key = NULL;
    ECDSA_SIG *sig = NULL;
    const BIGNUM *sig_r = NULL;
    const BIGNUM *sig_s = NULL;
    BIGNUM *r = NULL;
    BIGNUM *s = NULL;

    if (!TEST_true(BN_hex2bn(&priv, privkey_hex)))
        goto done;

    key = EC_KEY_new();
    if (!TEST_ptr(key)
            || !TEST_true(EC_KEY_set_group(key, group))
            || !TEST_true(EC_KEY_set_private_key(key, priv)))
        goto done;

    pt = EC_POINT_new(group);
    if (!TEST_ptr(pt)
            || !TEST_true(EC_POINT_mul(group, pt, priv, NULL, NULL, NULL))
            || !TEST_true(EC_KEY_set_public_key(key, pt)))
        goto done;

    start_fake_rand(k_hex);
    sig = sm2_do_sign(key, EVP_sm3(), (const uint8_t *)userid, strlen(userid),
                      (const uint8_t *)message, msg_len);
    if (!TEST_ptr(sig)
            || !TEST_size_t_eq(fake_rand_bytes_offset, fake_rand_size)) {
        restore_rand();
        goto done;
    }
    restore_rand();

    ECDSA_SIG_get0(sig, &sig_r, &sig_s);

    if (!TEST_true(BN_hex2bn(&r, r_hex))
            || !TEST_true(BN_hex2bn(&s, s_hex))
            || !TEST_BN_eq(r, sig_r)
            || !TEST_BN_eq(s, sig_s))
        goto done;

    ok = sm2_do_verify(key, EVP_sm3(), sig, (const uint8_t *)userid,
                       strlen(userid), (const uint8_t *)message, msg_len);

    /* We goto done whether this passes or fails */
    TEST_true(ok);

 done:
    ECDSA_SIG_free(sig);
    EC_POINT_free(pt);
    EC_KEY_free(key);
    BN_free(priv);
    BN_free(r);
    BN_free(s);

    return ok;
}

static int sm2_sig_test(void)
{
    int testresult = 0;
    /* From draft-shen-sm2-ecdsa-02 */
    EC_GROUP *test_group =
        create_EC_group
        ("8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3",
         "787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498",
         "63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A",
         "421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D",
         "0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2",
         "8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7",
         "1");

    if (!TEST_ptr(test_group))
        goto done;

    if (!TEST_true(test_sm2_sign(
                        test_group,
                        "ALICE123@YAHOO.COM",
                        "128B2FA8BD433C6C068C8D803DFF79792A519A55171B1B650C23661D15897263",
                        "message digest",
                        "006CB28D99385C175C94F94E934817663FC176D925DD72B727260DBAAE1FB2F96F"
                        "007c47811054c6f99613a578eb8453706ccb96384fe7df5c171671e760bfa8be3a",
                        "40F1EC59F793D9F49E09DCEF49130D4194F79FB1EED2CAA55BACDB49C4E755D1",
                        "6FC6DAC32C5D5CF10C77DFB20F7C2EB667A457872FB09EC56327A67EC7DEEBE7")))
        goto done;

    testresult = 1;

 done:
    EC_GROUP_free(test_group);

    return testresult;
}

#endif

int setup_tests(void)
{
#ifdef OPENSSL_NO_SM2
    TEST_note("SM2 is disabled.");
#else
    ADD_TEST(sm2_crypt_test);
    ADD_TEST(sm2_sig_test);
#endif
    return 1;
}
