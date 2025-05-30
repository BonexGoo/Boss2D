/*
 * Copyright 2001-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include BOSS_OPENSSL_V_openssl__crypto_h //original-code:<openssl/crypto.h>
#include BOSS_OPENSSL_V_openssl__opensslconf_h //original-code:<openssl/opensslconf.h>

#if defined(__OpenBSD__) || (defined(__FreeBSD__) && __FreeBSD__ > 2) || defined(__DragonFly__)

# include OPENSSL_UNISTD

int OPENSSL_issetugid(void)
{
    return issetugid();
}

#elif defined(OPENSSL_SYS_WIN32) || defined(OPENSSL_SYS_VXWORKS) || defined(OPENSSL_SYS_UEFI)

int OPENSSL_issetugid(void)
{
    return 0;
}

#else

# include OPENSSL_UNISTD
# include <sys/types.h>

# if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#  if __GLIBC_PREREQ(2, 16)
#   include <sys/auxv.h>
#  endif
# endif

int OPENSSL_issetugid(void)
{
# ifdef AT_SECURE
    return getauxval(AT_SECURE) != 0;
# else
    return getuid() != geteuid() || getgid() != getegid();
# endif
}
#endif
