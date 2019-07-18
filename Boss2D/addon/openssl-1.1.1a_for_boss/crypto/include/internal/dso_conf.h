// author BOSS

// removed by BOSS: {- join("\n",map { "/* $_ */" } @autowarntext) -}
/*
 * Copyright 2016-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef HEADER_DSO_CONF_H
# define HEADER_DSO_CONF_H
// removed by BOSS: {- output_off() if $disabled{dso} -}
// removed by BOSS: {-  # The DSO code currently always implements all functions so that no
// removed by BOSS:     # applications will have to worry about that from a compilation point
// removed by BOSS:     # of view. However, the "method"s may return zero unless that platform
// removed by BOSS:     # has support compiled in for them. Currently each method is enabled
// removed by BOSS:     # by a define "DSO_<name>" ... we translate the "dso_scheme" config
// removed by BOSS:     # string entry into using the following logic;
// removed by BOSS:     my $scheme = uc $target{dso_scheme};
// removed by BOSS:     my @macros = ( "DSO_$scheme" );
// removed by BOSS:     if ($scheme eq 'DLFCN') {
// removed by BOSS:         @macros = ( "DSO_DLFCN", "HAVE_DLFCN_H" );
// removed by BOSS:     } elsif ($scheme eq "DLFCN_NO_H") {
// removed by BOSS:         @macros = ( "DSO_DLFCN" );
// removed by BOSS:     }
// removed by BOSS:     join("\n", map { "# define $_" } @macros); -}
// removed by BOSS: # define DSO_EXTENSION "{- $target{dso_extension} -}"
// removed by BOSS: {- output_on() if $disabled{dso} -}
#endif
