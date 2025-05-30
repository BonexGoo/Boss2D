/*
 * Copyright 2017-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL licenses, (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * https://www.openssl.org/source/license.html
 * or in the file LICENSE in the source distribution.
 */

#include BOSS_OPENSSL_U_internal__nelem_h //original-code:"internal/nelem.h"
#include "testutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM_REPEATS "1000000"

static int64_t num_repeats;
static int print_mode = 0;

#ifndef OPENSSL_NO_EC
# include BOSS_OPENSSL_V_openssl__ec_h //original-code:<openssl/ec.h>
# include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
# include BOSS_OPENSSL_V_openssl__obj_mac_h //original-code:<openssl/obj_mac.h>
# include BOSS_OPENSSL_V_openssl__objects_h //original-code:<openssl/objects.h>
# include BOSS_OPENSSL_V_openssl__rand_h //original-code:<openssl/rand.h>
# include BOSS_OPENSSL_V_openssl__bn_h //original-code:<openssl/bn.h>
# include BOSS_OPENSSL_V_openssl__opensslconf_h //original-code:<openssl/opensslconf.h>

static const char *kP256DefaultResult =
    "A1E24B223B8E81BC1FFF99BAFB909EDB895FACDE7D6DA5EF5E7B3255FB378E0F";

/*
 * Perform a deterministic walk on the curve, by starting from |point| and
 * using the X-coordinate of the previous point as the next scalar for
 * point multiplication.
 * Returns the X-coordinate of the end result or NULL on error.
 */
static BIGNUM *walk_curve(const EC_GROUP *group, EC_POINT *point, int64_t num)
{
    BIGNUM *scalar = NULL;
    int64_t i;

    if (!TEST_ptr(scalar = BN_new())
            || !TEST_true(EC_POINT_get_affine_coordinates(group, point, scalar,
                                                          NULL, NULL)))
        goto err;

    for (i = 0; i < num; i++) {
        if (!TEST_true(EC_POINT_mul(group, point, NULL, point, scalar, NULL))
                || !TEST_true(EC_POINT_get_affine_coordinates(group, point,
                                                              scalar,
                                                              NULL, NULL)))
            goto err;
    }
    return scalar;

err:
    BN_free(scalar);
    return NULL;
}

static int test_curve(void)
{
    EC_GROUP *group = NULL;
    EC_POINT *point = NULL;
    BIGNUM *result = NULL, *expected_result = NULL;
    int ret = 0;

    /*
     * We currently hard-code P-256, though adaptation to other curves.
     * would be straightforward.
     */
    if (!TEST_ptr(group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1))
            || !TEST_ptr(point = EC_POINT_dup(EC_GROUP_get0_generator(group),
                                              group))
            || !TEST_ptr(result = walk_curve(group, point, num_repeats)))
        return 0;

    if (print_mode) {
        BN_print(bio_out, result);
        BIO_printf(bio_out, "\n");
        ret = 1;
    } else {
        if (!TEST_true(BN_hex2bn(&expected_result, kP256DefaultResult))
                || !TEST_ptr(expected_result)
                || !TEST_BN_eq(result, expected_result))
            goto err;
        ret = 1;
    }

err:
    EC_GROUP_free(group);
    EC_POINT_free(point);
    BN_free(result);
    BN_free(expected_result);
    return ret;
}
#endif

static int atoi64(const char *in, int64_t *result)
{
    int64_t ret = 0;

    for ( ; *in != '\0'; in++) {
        char c = *in;

        if (!isdigit((unsigned char)c))
            return 0;
        ret *= 10;
        ret += (c - '0');
    }
    *result = ret;
    return 1;
}

/*
 * Stress test the curve. If the '-num' argument is given, runs the loop
 * |num| times and prints the resulting X-coordinate. Otherwise runs the test
 * the default number of times and compares against the expected result.
 */
int setup_tests(void)
{
    const char *p;

    if (!atoi64(NUM_REPEATS, &num_repeats)) {
        TEST_error("Cannot parse " NUM_REPEATS);
        return 0;
    }
    /*
     * TODO(openssl-team): code under test/ should be able to reuse the option
     * parsing framework currently in apps/.
     */
    p = test_get_option_argument("-num");
    if (p != NULL) {
        if (!atoi64(p, &num_repeats)
                || num_repeats < 0)
            return 0;
        print_mode = 1;
    }

#ifndef OPENSSL_NO_EC
    ADD_TEST(test_curve);
#endif
    return 1;
}
