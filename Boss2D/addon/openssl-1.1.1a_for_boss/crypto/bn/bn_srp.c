// author BOSS

/*
 * Copyright 2014-2017 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "bn_lcl.h"
#include BOSS_OPENSSL_U_internal__nelem_h //original-code:"internal/nelem.h"

#ifndef OPENSSL_NO_SRP

#include BOSS_OPENSSL_V_openssl__srp_h //original-code:<openssl/srp.h>
#include BOSS_OPENSSL_U_internal__bn_srp_h //original-code:"internal/bn_srp.h"

# if (BN_BYTES == 8)
// removed by BOSS:#  if (defined(_WIN32) || defined(_WIN64)) && !defined(__MINGW32__)
#  if BOSS_WINDOWS && !defined(__MINGW32__) // added by BOSS
#   define bn_pack4(a1,a2,a3,a4) ((a1##UI64<<48)|(a2##UI64<<32)|(a3##UI64<<16)|a4##UI64)
#  elif defined(__arch64__)
#   define bn_pack4(a1,a2,a3,a4) ((a1##UL<<48)|(a2##UL<<32)|(a3##UL<<16)|a4##UL)
#  else
#   define bn_pack4(a1,a2,a3,a4) ((a1##ULL<<48)|(a2##ULL<<32)|(a3##ULL<<16)|a4##ULL)
#  endif
# elif (BN_BYTES == 4)
#  define bn_pack4(a1,a2,a3,a4)  ((a3##UL<<16)|a4##UL), ((a1##UL<<16)|a2##UL)
# else
#  error "unsupported BN_BYTES"
# endif

static const BN_ULONG bn_group_1024_value[] = {
    bn_pack4(0x9FC6, 0x1D2F, 0xC0EB, 0x06E3),
    bn_pack4(0xFD51, 0x38FE, 0x8376, 0x435B),
    bn_pack4(0x2FD4, 0xCBF4, 0x976E, 0xAA9A),
    bn_pack4(0x68ED, 0xBC3C, 0x0572, 0x6CC0),
    bn_pack4(0xC529, 0xF566, 0x660E, 0x57EC),
    bn_pack4(0x8255, 0x9B29, 0x7BCF, 0x1885),
    bn_pack4(0xCE8E, 0xF4AD, 0x69B1, 0x5D49),
    bn_pack4(0x5DC7, 0xD7B4, 0x6154, 0xD6B6),
    bn_pack4(0x8E49, 0x5C1D, 0x6089, 0xDAD1),
    bn_pack4(0xE0D5, 0xD8E2, 0x50B9, 0x8BE4),
    bn_pack4(0x383B, 0x4813, 0xD692, 0xC6E0),
    bn_pack4(0xD674, 0xDF74, 0x96EA, 0x81D3),
    bn_pack4(0x9EA2, 0x314C, 0x9C25, 0x6576),
    bn_pack4(0x6072, 0x6187, 0x75FF, 0x3C0B),
    bn_pack4(0x9C33, 0xF80A, 0xFA8F, 0xC5E8),
    bn_pack4(0xEEAF, 0x0AB9, 0xADB3, 0x8DD6)
};

const BIGNUM bn_group_1024 = {
    (BN_ULONG *)bn_group_1024_value,
    OSSL_NELEM(bn_group_1024_value),
    OSSL_NELEM(bn_group_1024_value),
    0,
    BN_FLG_STATIC_DATA
};

static const BN_ULONG bn_group_1536_value[] = {
    bn_pack4(0xCF76, 0xE3FE, 0xD135, 0xF9BB),
    bn_pack4(0x1518, 0x0F93, 0x499A, 0x234D),
    bn_pack4(0x8CE7, 0xA28C, 0x2442, 0xC6F3),
    bn_pack4(0x5A02, 0x1FFF, 0x5E91, 0x479E),
    bn_pack4(0x7F8A, 0x2FE9, 0xB8B5, 0x292E),
    bn_pack4(0x837C, 0x264A, 0xE3A9, 0xBEB8),
    bn_pack4(0xE442, 0x734A, 0xF7CC, 0xB7AE),
    bn_pack4(0x6577, 0x2E43, 0x7D6C, 0x7F8C),
    bn_pack4(0xDB2F, 0xD53D, 0x24B7, 0xC486),
    bn_pack4(0x6EDF, 0x0195, 0x3934, 0x9627),
    bn_pack4(0x158B, 0xFD3E, 0x2B9C, 0x8CF5),
    bn_pack4(0x764E, 0x3F4B, 0x53DD, 0x9DA1),
    bn_pack4(0x4754, 0x8381, 0xDBC5, 0xB1FC),
    bn_pack4(0x9B60, 0x9E0B, 0xE3BA, 0xB63D),
    bn_pack4(0x8134, 0xB1C8, 0xB979, 0x8914),
    bn_pack4(0xDF02, 0x8A7C, 0xEC67, 0xF0D0),
    bn_pack4(0x80B6, 0x55BB, 0x9A22, 0xE8DC),
    bn_pack4(0x1558, 0x903B, 0xA0D0, 0xF843),
    bn_pack4(0x51C6, 0xA94B, 0xE460, 0x7A29),
    bn_pack4(0x5F4F, 0x5F55, 0x6E27, 0xCBDE),
    bn_pack4(0xBEEE, 0xA961, 0x4B19, 0xCC4D),
    bn_pack4(0xDBA5, 0x1DF4, 0x99AC, 0x4C80),
    bn_pack4(0xB1F1, 0x2A86, 0x17A4, 0x7BBB),
    bn_pack4(0x9DEF, 0x3CAF, 0xB939, 0x277A)
};

const BIGNUM bn_group_1536 = {
    (BN_ULONG *)bn_group_1536_value,
    OSSL_NELEM(bn_group_1536_value),
    OSSL_NELEM(bn_group_1536_value),
    0,
    BN_FLG_STATIC_DATA
};

static const BN_ULONG bn_group_2048_value[] = {
    bn_pack4(0x0FA7, 0x111F, 0x9E4A, 0xFF73),
    bn_pack4(0x9B65, 0xE372, 0xFCD6, 0x8EF2),
    bn_pack4(0x35DE, 0x236D, 0x525F, 0x5475),
    bn_pack4(0x94B5, 0xC803, 0xD89F, 0x7AE4),
    bn_pack4(0x71AE, 0x35F8, 0xE9DB, 0xFBB6),
    bn_pack4(0x2A56, 0x98F3, 0xA8D0, 0xC382),
    bn_pack4(0x9CCC, 0x041C, 0x7BC3, 0x08D8),
    bn_pack4(0xAF87, 0x4E73, 0x03CE, 0x5329),
    bn_pack4(0x6160, 0x2790, 0x04E5, 0x7AE6),
    bn_pack4(0x032C, 0xFBDB, 0xF52F, 0xB378),
    bn_pack4(0x5EA7, 0x7A27, 0x75D2, 0xECFA),
    bn_pack4(0x5445, 0x23B5, 0x24B0, 0xD57D),
    bn_pack4(0x5B9D, 0x32E6, 0x88F8, 0x7748),
    bn_pack4(0xF1D2, 0xB907, 0x8717, 0x461A),
    bn_pack4(0x76BD, 0x207A, 0x436C, 0x6481),
    bn_pack4(0xCA97, 0xB43A, 0x23FB, 0x8016),
    bn_pack4(0x1D28, 0x1E44, 0x6B14, 0x773B),
    bn_pack4(0x7359, 0xD041, 0xD5C3, 0x3EA7),
    bn_pack4(0xA80D, 0x740A, 0xDBF4, 0xFF74),
    bn_pack4(0x55F9, 0x7993, 0xEC97, 0x5EEA),
    bn_pack4(0x2918, 0xA996, 0x2F0B, 0x93B8),
    bn_pack4(0x661A, 0x05FB, 0xD5FA, 0xAAE8),
    bn_pack4(0xCF60, 0x9517, 0x9A16, 0x3AB3),
    bn_pack4(0xE808, 0x3969, 0xEDB7, 0x67B0),
    bn_pack4(0xCD7F, 0x48A9, 0xDA04, 0xFD50),
    bn_pack4(0xD523, 0x12AB, 0x4B03, 0x310D),
    bn_pack4(0x8193, 0xE075, 0x7767, 0xA13D),
    bn_pack4(0xA373, 0x29CB, 0xB4A0, 0x99ED),
    bn_pack4(0xFC31, 0x9294, 0x3DB5, 0x6050),
    bn_pack4(0xAF72, 0xB665, 0x1987, 0xEE07),
    bn_pack4(0xF166, 0xDE5E, 0x1389, 0x582F),
    bn_pack4(0xAC6B, 0xDB41, 0x324A, 0x9A9B)
};

const BIGNUM bn_group_2048 = {
    (BN_ULONG *)bn_group_2048_value,
    OSSL_NELEM(bn_group_2048_value),
    OSSL_NELEM(bn_group_2048_value),
    0,
    BN_FLG_STATIC_DATA
};

static const BN_ULONG bn_group_3072_value[] = {
    bn_pack4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF),
    bn_pack4(0x4B82, 0xD120, 0xA93A, 0xD2CA),
    bn_pack4(0x43DB, 0x5BFC, 0xE0FD, 0x108E),
    bn_pack4(0x08E2, 0x4FA0, 0x74E5, 0xAB31),
    bn_pack4(0x7709, 0x88C0, 0xBAD9, 0x46E2),
    bn_pack4(0xBBE1, 0x1757, 0x7A61, 0x5D6C),
    bn_pack4(0x521F, 0x2B18, 0x177B, 0x200C),
    bn_pack4(0xD876, 0x0273, 0x3EC8, 0x6A64),
    bn_pack4(0xF12F, 0xFA06, 0xD98A, 0x0864),
    bn_pack4(0xCEE3, 0xD226, 0x1AD2, 0xEE6B),
    bn_pack4(0x1E8C, 0x94E0, 0x4A25, 0x619D),
    bn_pack4(0xABF5, 0xAE8C, 0xDB09, 0x33D7),
    bn_pack4(0xB397, 0x0F85, 0xA6E1, 0xE4C7),
    bn_pack4(0x8AEA, 0x7157, 0x5D06, 0x0C7D),
    bn_pack4(0xECFB, 0x8504, 0x58DB, 0xEF0A),
    bn_pack4(0xA855, 0x21AB, 0xDF1C, 0xBA64),
    bn_pack4(0xAD33, 0x170D, 0x0450, 0x7A33),
    bn_pack4(0x1572, 0x8E5A, 0x8AAA, 0xC42D),
    bn_pack4(0x15D2, 0x2618, 0x98FA, 0x0510),
    bn_pack4(0x3995, 0x497C, 0xEA95, 0x6AE5),
    bn_pack4(0xDE2B, 0xCBF6, 0x9558, 0x1718),
    bn_pack4(0xB5C5, 0x5DF0, 0x6F4C, 0x52C9),
    bn_pack4(0x9B27, 0x83A2, 0xEC07, 0xA28F),
    bn_pack4(0xE39E, 0x772C, 0x180E, 0x8603),
    bn_pack4(0x3290, 0x5E46, 0x2E36, 0xCE3B),
    bn_pack4(0xF174, 0x6C08, 0xCA18, 0x217C),
    bn_pack4(0x670C, 0x354E, 0x4ABC, 0x9804),
    bn_pack4(0x9ED5, 0x2907, 0x7096, 0x966D),
    bn_pack4(0x1C62, 0xF356, 0x2085, 0x52BB),
    bn_pack4(0x8365, 0x5D23, 0xDCA3, 0xAD96),
    bn_pack4(0x6916, 0x3FA8, 0xFD24, 0xCF5F),
    bn_pack4(0x98DA, 0x4836, 0x1C55, 0xD39A),
    bn_pack4(0xC200, 0x7CB8, 0xA163, 0xBF05),
    bn_pack4(0x4928, 0x6651, 0xECE4, 0x5B3D),
    bn_pack4(0xAE9F, 0x2411, 0x7C4B, 0x1FE6),
    bn_pack4(0xEE38, 0x6BFB, 0x5A89, 0x9FA5),
    bn_pack4(0x0BFF, 0x5CB6, 0xF406, 0xB7ED),
    bn_pack4(0xF44C, 0x42E9, 0xA637, 0xED6B),
    bn_pack4(0xE485, 0xB576, 0x625E, 0x7EC6),
    bn_pack4(0x4FE1, 0x356D, 0x6D51, 0xC245),
    bn_pack4(0x302B, 0x0A6D, 0xF25F, 0x1437),
    bn_pack4(0xEF95, 0x19B3, 0xCD3A, 0x431B),
    bn_pack4(0x514A, 0x0879, 0x8E34, 0x04DD),
    bn_pack4(0x020B, 0xBEA6, 0x3B13, 0x9B22),
    bn_pack4(0x2902, 0x4E08, 0x8A67, 0xCC74),
    bn_pack4(0xC4C6, 0x628B, 0x80DC, 0x1CD1),
    bn_pack4(0xC90F, 0xDAA2, 0x2168, 0xC234),
    bn_pack4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF)
};

const BIGNUM bn_group_3072 = {
    (BN_ULONG *)bn_group_3072_value,
    OSSL_NELEM(bn_group_3072_value),
    OSSL_NELEM(bn_group_3072_value),
    0,
    BN_FLG_STATIC_DATA
};

static const BN_ULONG bn_group_4096_value[] = {
    bn_pack4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF),
    bn_pack4(0x4DF4, 0x35C9, 0x3406, 0x3199),
    bn_pack4(0x86FF, 0xB7DC, 0x90A6, 0xC08F),
    bn_pack4(0x93B4, 0xEA98, 0x8D8F, 0xDDC1),
    bn_pack4(0xD006, 0x9127, 0xD5B0, 0x5AA9),
    bn_pack4(0xB81B, 0xDD76, 0x2170, 0x481C),
    bn_pack4(0x1F61, 0x2970, 0xCEE2, 0xD7AF),
    bn_pack4(0x233B, 0xA186, 0x515B, 0xE7ED),
    bn_pack4(0x99B2, 0x964F, 0xA090, 0xC3A2),
    bn_pack4(0x287C, 0x5947, 0x4E6B, 0xC05D),
    bn_pack4(0x2E8E, 0xFC14, 0x1FBE, 0xCAA6),
    bn_pack4(0xDBBB, 0xC2DB, 0x04DE, 0x8EF9),
    bn_pack4(0x2583, 0xE9CA, 0x2AD4, 0x4CE8),
    bn_pack4(0x1A94, 0x6834, 0xB615, 0x0BDA),
    bn_pack4(0x99C3, 0x2718, 0x6AF4, 0xE23C),
    bn_pack4(0x8871, 0x9A10, 0xBDBA, 0x5B26),
    bn_pack4(0x1A72, 0x3C12, 0xA787, 0xE6D7),
    bn_pack4(0x4B82, 0xD120, 0xA921, 0x0801),
    bn_pack4(0x43DB, 0x5BFC, 0xE0FD, 0x108E),
    bn_pack4(0x08E2, 0x4FA0, 0x74E5, 0xAB31),
    bn_pack4(0x7709, 0x88C0, 0xBAD9, 0x46E2),
    bn_pack4(0xBBE1, 0x1757, 0x7A61, 0x5D6C),
    bn_pack4(0x521F, 0x2B18, 0x177B, 0x200C),
    bn_pack4(0xD876, 0x0273, 0x3EC8, 0x6A64),
    bn_pack4(0xF12F, 0xFA06, 0xD98A, 0x0864),
    bn_pack4(0xCEE3, 0xD226, 0x1AD2, 0xEE6B),
    bn_pack4(0x1E8C, 0x94E0, 0x4A25, 0x619D),
    bn_pack4(0xABF5, 0xAE8C, 0xDB09, 0x33D7),
    bn_pack4(0xB397, 0x0F85, 0xA6E1, 0xE4C7),
    bn_pack4(0x8AEA, 0x7157, 0x5D06, 0x0C7D),
    bn_pack4(0xECFB, 0x8504, 0x58DB, 0xEF0A),
    bn_pack4(0xA855, 0x21AB, 0xDF1C, 0xBA64),
    bn_pack4(0xAD33, 0x170D, 0x0450, 0x7A33),
    bn_pack4(0x1572, 0x8E5A, 0x8AAA, 0xC42D),
    bn_pack4(0x15D2, 0x2618, 0x98FA, 0x0510),
    bn_pack4(0x3995, 0x497C, 0xEA95, 0x6AE5),
    bn_pack4(0xDE2B, 0xCBF6, 0x9558, 0x1718),
    bn_pack4(0xB5C5, 0x5DF0, 0x6F4C, 0x52C9),
    bn_pack4(0x9B27, 0x83A2, 0xEC07, 0xA28F),
    bn_pack4(0xE39E, 0x772C, 0x180E, 0x8603),
    bn_pack4(0x3290, 0x5E46, 0x2E36, 0xCE3B),
    bn_pack4(0xF174, 0x6C08, 0xCA18, 0x217C),
    bn_pack4(0x670C, 0x354E, 0x4ABC, 0x9804),
    bn_pack4(0x9ED5, 0x2907, 0x7096, 0x966D),
    bn_pack4(0x1C62, 0xF356, 0x2085, 0x52BB),
    bn_pack4(0x8365, 0x5D23, 0xDCA3, 0xAD96),
    bn_pack4(0x6916, 0x3FA8, 0xFD24, 0xCF5F),
    bn_pack4(0x98DA, 0x4836, 0x1C55, 0xD39A),
    bn_pack4(0xC200, 0x7CB8, 0xA163, 0xBF05),
    bn_pack4(0x4928, 0x6651, 0xECE4, 0x5B3D),
    bn_pack4(0xAE9F, 0x2411, 0x7C4B, 0x1FE6),
    bn_pack4(0xEE38, 0x6BFB, 0x5A89, 0x9FA5),
    bn_pack4(0x0BFF, 0x5CB6, 0xF406, 0xB7ED),
    bn_pack4(0xF44C, 0x42E9, 0xA637, 0xED6B),
    bn_pack4(0xE485, 0xB576, 0x625E, 0x7EC6),
    bn_pack4(0x4FE1, 0x356D, 0x6D51, 0xC245),
    bn_pack4(0x302B, 0x0A6D, 0xF25F, 0x1437),
    bn_pack4(0xEF95, 0x19B3, 0xCD3A, 0x431B),
    bn_pack4(0x514A, 0x0879, 0x8E34, 0x04DD),
    bn_pack4(0x020B, 0xBEA6, 0x3B13, 0x9B22),
    bn_pack4(0x2902, 0x4E08, 0x8A67, 0xCC74),
    bn_pack4(0xC4C6, 0x628B, 0x80DC, 0x1CD1),
    bn_pack4(0xC90F, 0xDAA2, 0x2168, 0xC234),
    bn_pack4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF)
};

const BIGNUM bn_group_4096 = {
    (BN_ULONG *)bn_group_4096_value,
    OSSL_NELEM(bn_group_4096_value),
    OSSL_NELEM(bn_group_4096_value),
    0,
    BN_FLG_STATIC_DATA
};

static const BN_ULONG bn_group_6144_value[] = {
    bn_pack4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF),
    bn_pack4(0xE694, 0xF91E, 0x6DCC, 0x4024),
    bn_pack4(0x12BF, 0x2D5B, 0x0B74, 0x74D6),
    bn_pack4(0x043E, 0x8F66, 0x3F48, 0x60EE),
    bn_pack4(0x387F, 0xE8D7, 0x6E3C, 0x0468),
    bn_pack4(0xDA56, 0xC9EC, 0x2EF2, 0x9632),
    bn_pack4(0xEB19, 0xCCB1, 0xA313, 0xD55C),
    bn_pack4(0xF550, 0xAA3D, 0x8A1F, 0xBFF0),
    bn_pack4(0x06A1, 0xD58B, 0xB7C5, 0xDA76),
    bn_pack4(0xA797, 0x15EE, 0xF29B, 0xE328),
    bn_pack4(0x14CC, 0x5ED2, 0x0F80, 0x37E0),
    bn_pack4(0xCC8F, 0x6D7E, 0xBF48, 0xE1D8),
    bn_pack4(0x4BD4, 0x07B2, 0x2B41, 0x54AA),
    bn_pack4(0x0F1D, 0x45B7, 0xFF58, 0x5AC5),
    bn_pack4(0x23A9, 0x7A7E, 0x36CC, 0x88BE),
    bn_pack4(0x59E7, 0xC97F, 0xBEC7, 0xE8F3),
    bn_pack4(0xB5A8, 0x4031, 0x900B, 0x1C9E),
    bn_pack4(0xD55E, 0x702F, 0x4698, 0x0C82),
    bn_pack4(0xF482, 0xD7CE, 0x6E74, 0xFEF6),
    bn_pack4(0xF032, 0xEA15, 0xD172, 0x1D03),
    bn_pack4(0x5983, 0xCA01, 0xC64B, 0x92EC),
    bn_pack4(0x6FB8, 0xF401, 0x378C, 0xD2BF),
    bn_pack4(0x3320, 0x5151, 0x2BD7, 0xAF42),
    bn_pack4(0xDB7F, 0x1447, 0xE6CC, 0x254B),
    bn_pack4(0x44CE, 0x6CBA, 0xCED4, 0xBB1B),
    bn_pack4(0xDA3E, 0xDBEB, 0xCF9B, 0x14ED),
    bn_pack4(0x1797, 0x27B0, 0x865A, 0x8918),
    bn_pack4(0xB06A, 0x53ED, 0x9027, 0xD831),
    bn_pack4(0xE5DB, 0x382F, 0x4130, 0x01AE),
    bn_pack4(0xF8FF, 0x9406, 0xAD9E, 0x530E),
    bn_pack4(0xC975, 0x1E76, 0x3DBA, 0x37BD),
    bn_pack4(0xC1D4, 0xDCB2, 0x6026, 0x46DE),
    bn_pack4(0x36C3, 0xFAB4, 0xD27C, 0x7026),
    bn_pack4(0x4DF4, 0x35C9, 0x3402, 0x8492),
    bn_pack4(0x86FF, 0xB7DC, 0x90A6, 0xC08F),
    bn_pack4(0x93B4, 0xEA98, 0x8D8F, 0xDDC1),
    bn_pack4(0xD006, 0x9127, 0xD5B0, 0x5AA9),
    bn_pack4(0xB81B, 0xDD76, 0x2170, 0x481C),
    bn_pack4(0x1F61, 0x2970, 0xCEE2, 0xD7AF),
    bn_pack4(0x233B, 0xA186, 0x515B, 0xE7ED),
    bn_pack4(0x99B2, 0x964F, 0xA090, 0xC3A2),
    bn_pack4(0x287C, 0x5947, 0x4E6B, 0xC05D),
    bn_pack4(0x2E8E, 0xFC14, 0x1FBE, 0xCAA6),
    bn_pack4(0xDBBB, 0xC2DB, 0x04DE, 0x8EF9),
    bn_pack4(0x2583, 0xE9CA, 0x2AD4, 0x4CE8),
    bn_pack4(0x1A94, 0x6834, 0xB615, 0x0BDA),
    bn_pack4(0x99C3, 0x2718, 0x6AF4, 0xE23C),
    bn_pack4(0x8871, 0x9A10, 0xBDBA, 0x5B26),
    bn_pack4(0x1A72, 0x3C12, 0xA787, 0xE6D7),
    bn_pack4(0x4B82, 0xD120, 0xA921, 0x0801),
    bn_pack4(0x43DB, 0x5BFC, 0xE0FD, 0x108E),
    bn_pack4(0x08E2, 0x4FA0, 0x74E5, 0xAB31),
    bn_pack4(0x7709, 0x88C0, 0xBAD9, 0x46E2),
    bn_pack4(0xBBE1, 0x1757, 0x7A61, 0x5D6C),
    bn_pack4(0x521F, 0x2B18, 0x177B, 0x200C),
    bn_pack4(0xD876, 0x0273, 0x3EC8, 0x6A64),
    bn_pack4(0xF12F, 0xFA06, 0xD98A, 0x0864),
    bn_pack4(0xCEE3, 0xD226, 0x1AD2, 0xEE6B),
    bn_pack4(0x1E8C, 0x94E0, 0x4A25, 0x619D),
    bn_pack4(0xABF5, 0xAE8C, 0xDB09, 0x33D7),
    bn_pack4(0xB397, 0x0F85, 0xA6E1, 0xE4C7),
    bn_pack4(0x8AEA, 0x7157, 0x5D06, 0x0C7D),
    bn_pack4(0xECFB, 0x8504, 0x58DB, 0xEF0A),
    bn_pack4(0xA855, 0x21AB, 0xDF1C, 0xBA64),
    bn_pack4(0xAD33, 0x170D, 0x0450, 0x7A33),
    bn_pack4(0x1572, 0x8E5A, 0x8AAA, 0xC42D),
    bn_pack4(0x15D2, 0x2618, 0x98FA, 0x0510),
    bn_pack4(0x3995, 0x497C, 0xEA95, 0x6AE5),
    bn_pack4(0xDE2B, 0xCBF6, 0x9558, 0x1718),
    bn_pack4(0xB5C5, 0x5DF0, 0x6F4C, 0x52C9),
    bn_pack4(0x9B27, 0x83A2, 0xEC07, 0xA28F),
    bn_pack4(0xE39E, 0x772C, 0x180E, 0x8603),
    bn_pack4(0x3290, 0x5E46, 0x2E36, 0xCE3B),
    bn_pack4(0xF174, 0x6C08, 0xCA18, 0x217C),
    bn_pack4(0x670C, 0x354E, 0x4ABC, 0x9804),
    bn_pack4(0x9ED5, 0x2907, 0x7096, 0x966D),
    bn_pack4(0x1C62, 0xF356, 0x2085, 0x52BB),
    bn_pack4(0x8365, 0x5D23, 0xDCA3, 0xAD96),
    bn_pack4(0x6916, 0x3FA8, 0xFD24, 0xCF5F),
    bn_pack4(0x98DA, 0x4836, 0x1C55, 0xD39A),
    bn_pack4(0xC200, 0x7CB8, 0xA163, 0xBF05),
    bn_pack4(0x4928, 0x6651, 0xECE4, 0x5B3D),
    bn_pack4(0xAE9F, 0x2411, 0x7C4B, 0x1FE6),
    bn_pack4(0xEE38, 0x6BFB, 0x5A89, 0x9FA5),
    bn_pack4(0x0BFF, 0x5CB6, 0xF406, 0xB7ED),
    bn_pack4(0xF44C, 0x42E9, 0xA637, 0xED6B),
    bn_pack4(0xE485, 0xB576, 0x625E, 0x7EC6),
    bn_pack4(0x4FE1, 0x356D, 0x6D51, 0xC245),
    bn_pack4(0x302B, 0x0A6D, 0xF25F, 0x1437),
    bn_pack4(0xEF95, 0x19B3, 0xCD3A, 0x431B),
    bn_pack4(0x514A, 0x0879, 0x8E34, 0x04DD),
    bn_pack4(0x020B, 0xBEA6, 0x3B13, 0x9B22),
    bn_pack4(0x2902, 0x4E08, 0x8A67, 0xCC74),
    bn_pack4(0xC4C6, 0x628B, 0x80DC, 0x1CD1),
    bn_pack4(0xC90F, 0xDAA2, 0x2168, 0xC234),
    bn_pack4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF)
};

const BIGNUM bn_group_6144 = {
    (BN_ULONG *)bn_group_6144_value,
    OSSL_NELEM(bn_group_6144_value),
    OSSL_NELEM(bn_group_6144_value),
    0,
    BN_FLG_STATIC_DATA
};

static const BN_ULONG bn_group_8192_value[] = {
    bn_pack4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF),
    bn_pack4(0x60C9, 0x80DD, 0x98ED, 0xD3DF),
    bn_pack4(0xC81F, 0x56E8, 0x80B9, 0x6E71),
    bn_pack4(0x9E30, 0x50E2, 0x7656, 0x94DF),
    bn_pack4(0x9558, 0xE447, 0x5677, 0xE9AA),
    bn_pack4(0xC919, 0x0DA6, 0xFC02, 0x6E47),
    bn_pack4(0x889A, 0x002E, 0xD5EE, 0x382B),
    bn_pack4(0x4009, 0x438B, 0x481C, 0x6CD7),
    bn_pack4(0x3590, 0x46F4, 0xEB87, 0x9F92),
    bn_pack4(0xFAF3, 0x6BC3, 0x1ECF, 0xA268),
    bn_pack4(0xB1D5, 0x10BD, 0x7EE7, 0x4D73),
    bn_pack4(0xF9AB, 0x4819, 0x5DED, 0x7EA1),
    bn_pack4(0x64F3, 0x1CC5, 0x0846, 0x851D),
    bn_pack4(0x4597, 0xE899, 0xA025, 0x5DC1),
    bn_pack4(0xDF31, 0x0EE0, 0x74AB, 0x6A36),
    bn_pack4(0x6D2A, 0x13F8, 0x3F44, 0xF82D),
    bn_pack4(0x062B, 0x3CF5, 0xB3A2, 0x78A6),
    bn_pack4(0x7968, 0x3303, 0xED5B, 0xDD3A),
    bn_pack4(0xFA9D, 0x4B7F, 0xA2C0, 0x87E8),
    bn_pack4(0x4BCB, 0xC886, 0x2F83, 0x85DD),
    bn_pack4(0x3473, 0xFC64, 0x6CEA, 0x306B),
    bn_pack4(0x13EB, 0x57A8, 0x1A23, 0xF0C7),
    bn_pack4(0x2222, 0x2E04, 0xA403, 0x7C07),
    bn_pack4(0xE3FD, 0xB8BE, 0xFC84, 0x8AD9),
    bn_pack4(0x238F, 0x16CB, 0xE39D, 0x652D),
    bn_pack4(0x3423, 0xB474, 0x2BF1, 0xC978),
    bn_pack4(0x3AAB, 0x639C, 0x5AE4, 0xF568),
    bn_pack4(0x2576, 0xF693, 0x6BA4, 0x2466),
    bn_pack4(0x741F, 0xA7BF, 0x8AFC, 0x47ED),
    bn_pack4(0x3BC8, 0x32B6, 0x8D9D, 0xD300),
    bn_pack4(0xD8BE, 0xC4D0, 0x73B9, 0x31BA),
    bn_pack4(0x3877, 0x7CB6, 0xA932, 0xDF8C),
    bn_pack4(0x74A3, 0x926F, 0x12FE, 0xE5E4),
    bn_pack4(0xE694, 0xF91E, 0x6DBE, 0x1159),
    bn_pack4(0x12BF, 0x2D5B, 0x0B74, 0x74D6),
    bn_pack4(0x043E, 0x8F66, 0x3F48, 0x60EE),
    bn_pack4(0x387F, 0xE8D7, 0x6E3C, 0x0468),
    bn_pack4(0xDA56, 0xC9EC, 0x2EF2, 0x9632),
    bn_pack4(0xEB19, 0xCCB1, 0xA313, 0xD55C),
    bn_pack4(0xF550, 0xAA3D, 0x8A1F, 0xBFF0),
    bn_pack4(0x06A1, 0xD58B, 0xB7C5, 0xDA76),
    bn_pack4(0xA797, 0x15EE, 0xF29B, 0xE328),
    bn_pack4(0x14CC, 0x5ED2, 0x0F80, 0x37E0),
    bn_pack4(0xCC8F, 0x6D7E, 0xBF48, 0xE1D8),
    bn_pack4(0x4BD4, 0x07B2, 0x2B41, 0x54AA),
    bn_pack4(0x0F1D, 0x45B7, 0xFF58, 0x5AC5),
    bn_pack4(0x23A9, 0x7A7E, 0x36CC, 0x88BE),
    bn_pack4(0x59E7, 0xC97F, 0xBEC7, 0xE8F3),
    bn_pack4(0xB5A8, 0x4031, 0x900B, 0x1C9E),
    bn_pack4(0xD55E, 0x702F, 0x4698, 0x0C82),
    bn_pack4(0xF482, 0xD7CE, 0x6E74, 0xFEF6),
    bn_pack4(0xF032, 0xEA15, 0xD172, 0x1D03),
    bn_pack4(0x5983, 0xCA01, 0xC64B, 0x92EC),
    bn_pack4(0x6FB8, 0xF401, 0x378C, 0xD2BF),
    bn_pack4(0x3320, 0x5151, 0x2BD7, 0xAF42),
    bn_pack4(0xDB7F, 0x1447, 0xE6CC, 0x254B),
    bn_pack4(0x44CE, 0x6CBA, 0xCED4, 0xBB1B),
    bn_pack4(0xDA3E, 0xDBEB, 0xCF9B, 0x14ED),
    bn_pack4(0x1797, 0x27B0, 0x865A, 0x8918),
    bn_pack4(0xB06A, 0x53ED, 0x9027, 0xD831),
    bn_pack4(0xE5DB, 0x382F, 0x4130, 0x01AE),
    bn_pack4(0xF8FF, 0x9406, 0xAD9E, 0x530E),
    bn_pack4(0xC975, 0x1E76, 0x3DBA, 0x37BD),
    bn_pack4(0xC1D4, 0xDCB2, 0x6026, 0x46DE),
    bn_pack4(0x36C3, 0xFAB4, 0xD27C, 0x7026),
    bn_pack4(0x4DF4, 0x35C9, 0x3402, 0x8492),
    bn_pack4(0x86FF, 0xB7DC, 0x90A6, 0xC08F),
    bn_pack4(0x93B4, 0xEA98, 0x8D8F, 0xDDC1),
    bn_pack4(0xD006, 0x9127, 0xD5B0, 0x5AA9),
    bn_pack4(0xB81B, 0xDD76, 0x2170, 0x481C),
    bn_pack4(0x1F61, 0x2970, 0xCEE2, 0xD7AF),
    bn_pack4(0x233B, 0xA186, 0x515B, 0xE7ED),
    bn_pack4(0x99B2, 0x964F, 0xA090, 0xC3A2),
    bn_pack4(0x287C, 0x5947, 0x4E6B, 0xC05D),
    bn_pack4(0x2E8E, 0xFC14, 0x1FBE, 0xCAA6),
    bn_pack4(0xDBBB, 0xC2DB, 0x04DE, 0x8EF9),
    bn_pack4(0x2583, 0xE9CA, 0x2AD4, 0x4CE8),
    bn_pack4(0x1A94, 0x6834, 0xB615, 0x0BDA),
    bn_pack4(0x99C3, 0x2718, 0x6AF4, 0xE23C),
    bn_pack4(0x8871, 0x9A10, 0xBDBA, 0x5B26),
    bn_pack4(0x1A72, 0x3C12, 0xA787, 0xE6D7),
    bn_pack4(0x4B82, 0xD120, 0xA921, 0x0801),
    bn_pack4(0x43DB, 0x5BFC, 0xE0FD, 0x108E),
    bn_pack4(0x08E2, 0x4FA0, 0x74E5, 0xAB31),
    bn_pack4(0x7709, 0x88C0, 0xBAD9, 0x46E2),
    bn_pack4(0xBBE1, 0x1757, 0x7A61, 0x5D6C),
    bn_pack4(0x521F, 0x2B18, 0x177B, 0x200C),
    bn_pack4(0xD876, 0x0273, 0x3EC8, 0x6A64),
    bn_pack4(0xF12F, 0xFA06, 0xD98A, 0x0864),
    bn_pack4(0xCEE3, 0xD226, 0x1AD2, 0xEE6B),
    bn_pack4(0x1E8C, 0x94E0, 0x4A25, 0x619D),
    bn_pack4(0xABF5, 0xAE8C, 0xDB09, 0x33D7),
    bn_pack4(0xB397, 0x0F85, 0xA6E1, 0xE4C7),
    bn_pack4(0x8AEA, 0x7157, 0x5D06, 0x0C7D),
    bn_pack4(0xECFB, 0x8504, 0x58DB, 0xEF0A),
    bn_pack4(0xA855, 0x21AB, 0xDF1C, 0xBA64),
    bn_pack4(0xAD33, 0x170D, 0x0450, 0x7A33),
    bn_pack4(0x1572, 0x8E5A, 0x8AAA, 0xC42D),
    bn_pack4(0x15D2, 0x2618, 0x98FA, 0x0510),
    bn_pack4(0x3995, 0x497C, 0xEA95, 0x6AE5),
    bn_pack4(0xDE2B, 0xCBF6, 0x9558, 0x1718),
    bn_pack4(0xB5C5, 0x5DF0, 0x6F4C, 0x52C9),
    bn_pack4(0x9B27, 0x83A2, 0xEC07, 0xA28F),
    bn_pack4(0xE39E, 0x772C, 0x180E, 0x8603),
    bn_pack4(0x3290, 0x5E46, 0x2E36, 0xCE3B),
    bn_pack4(0xF174, 0x6C08, 0xCA18, 0x217C),
    bn_pack4(0x670C, 0x354E, 0x4ABC, 0x9804),
    bn_pack4(0x9ED5, 0x2907, 0x7096, 0x966D),
    bn_pack4(0x1C62, 0xF356, 0x2085, 0x52BB),
    bn_pack4(0x8365, 0x5D23, 0xDCA3, 0xAD96),
    bn_pack4(0x6916, 0x3FA8, 0xFD24, 0xCF5F),
    bn_pack4(0x98DA, 0x4836, 0x1C55, 0xD39A),
    bn_pack4(0xC200, 0x7CB8, 0xA163, 0xBF05),
    bn_pack4(0x4928, 0x6651, 0xECE4, 0x5B3D),
    bn_pack4(0xAE9F, 0x2411, 0x7C4B, 0x1FE6),
    bn_pack4(0xEE38, 0x6BFB, 0x5A89, 0x9FA5),
    bn_pack4(0x0BFF, 0x5CB6, 0xF406, 0xB7ED),
    bn_pack4(0xF44C, 0x42E9, 0xA637, 0xED6B),
    bn_pack4(0xE485, 0xB576, 0x625E, 0x7EC6),
    bn_pack4(0x4FE1, 0x356D, 0x6D51, 0xC245),
    bn_pack4(0x302B, 0x0A6D, 0xF25F, 0x1437),
    bn_pack4(0xEF95, 0x19B3, 0xCD3A, 0x431B),
    bn_pack4(0x514A, 0x0879, 0x8E34, 0x04DD),
    bn_pack4(0x020B, 0xBEA6, 0x3B13, 0x9B22),
    bn_pack4(0x2902, 0x4E08, 0x8A67, 0xCC74),
    bn_pack4(0xC4C6, 0x628B, 0x80DC, 0x1CD1),
    bn_pack4(0xC90F, 0xDAA2, 0x2168, 0xC234),
    bn_pack4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF)
};

const BIGNUM bn_group_8192 = {
    (BN_ULONG *)bn_group_8192_value,
    OSSL_NELEM(bn_group_8192_value),
    OSSL_NELEM(bn_group_8192_value),
    0,
    BN_FLG_STATIC_DATA
};

static const BN_ULONG bn_generator_19_value[] = { 19 };

const BIGNUM bn_generator_19 = {
    (BN_ULONG *)bn_generator_19_value,
    1,
    1,
    0,
    BN_FLG_STATIC_DATA
};
static const BN_ULONG bn_generator_5_value[] = { 5 };

const BIGNUM bn_generator_5 = {
    (BN_ULONG *)bn_generator_5_value,
    1,
    1,
    0,
    BN_FLG_STATIC_DATA
};
static const BN_ULONG bn_generator_2_value[] = { 2 };

const BIGNUM bn_generator_2 = {
    (BN_ULONG *)bn_generator_2_value,
    1,
    1,
    0,
    BN_FLG_STATIC_DATA
};

#endif
