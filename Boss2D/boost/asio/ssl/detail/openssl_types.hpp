//
// ssl/detail/openssl_types.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_SSL_DETAIL_OPENSSL_TYPES_HPP
#define BOOST_ASIO_SSL_DETAIL_OPENSSL_TYPES_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include BOSS_OPENSSL_V_openssl__conf_h //original-code:<openssl/conf.h>
#include BOSS_OPENSSL_V_openssl__ssl_h //original-code:<openssl/ssl.h>
#if !defined(OPENSSL_NO_ENGINE)
# include BOSS_OPENSSL_V_openssl__engine_h //original-code:<openssl/engine.h>
#endif // !defined(OPENSSL_NO_ENGINE)
#include BOSS_OPENSSL_V_openssl__dh_h //original-code:<openssl/dh.h>
#include BOSS_OPENSSL_V_openssl__err_h //original-code:<openssl/err.h>
#include BOSS_OPENSSL_V_openssl__rsa_h //original-code:<openssl/rsa.h>
#include BOSS_OPENSSL_V_openssl__x509v3_h //original-code:<openssl/x509v3.h>

#endif // BOOST_ASIO_SSL_DETAIL_OPENSSL_TYPES_HPP
