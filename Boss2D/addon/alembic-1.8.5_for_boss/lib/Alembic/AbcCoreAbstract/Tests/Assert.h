//-*****************************************************************************
//
// Copyright (c) 2009-2012,
//  Sony Pictures Imageworks Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic, nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#ifndef Alembic_AbcCoreAbstract_Tests_Assert_h
#define Alembic_AbcCoreAbstract_Tests_Assert_h

#include <iostream>
#include <stdexcept>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include BOSS_OPENCV_V_ImathMath_h //original-code:<ImathMath.h>
#include <limits>

//-*****************************************************************************
static const double VAL_EPSILON =
    std::numeric_limits<double>::epsilon() * 1024.0;

bool almostEqual( const double &a, const double &b,
                  const double &epsilon = VAL_EPSILON )
{
    return Imath::equalWithAbsError( a, b, epsilon );
}

//-*****************************************************************************
#define TESTING_ASSERT( TEST )                                          \
do                                                                      \
{                                                                       \
    if ( !( TEST ) )                                                    \
    {                                                                   \
        std::stringstream strm;                                         \
        strm << "ERROR: Failed Test in File: " << __FILE__ << " Line: " \
             << __LINE__;                                               \
        throw std::runtime_error( strm.str() );                         \
    }                                                                   \
}                                                                       \
while( 0 )

//-*****************************************************************************
#define TESTING_MESSAGE_ASSERT( TEST, MSG )                             \
    do                                                                  \
    {                                                                   \
        if ( !( TEST ) )                                                \
        {                                                               \
            std::stringstream strm;                                     \
            strm << "ERROR: Failed Test in File: " << __FILE__          \
                 << " Line: " << __LINE__ << " " << MSG;                \
            throw std::runtime_error( strm.str() );                     \
        }                                                               \
    }                                                                   \
    while( 0 )

//-*****************************************************************************
#define TESTING_ASSERT_THROW( TEST, EXCEPT )                            \
do                                                                      \
{                                                                       \
    bool passed = false;                                                \
    try                                                                 \
    {                                                                   \
        TEST ;                                                          \
    }                                                                   \
    catch ( const EXCEPT & )                                            \
    {                                                                   \
        passed = true;                                                  \
    }                                                                   \
                                                                        \
    if ( !passed )                                                      \
    {                                                                   \
        std::stringstream strm;                                         \
        strm << "ERROR: Failed Throw in File: " << __FILE__             \
             << " Line: " << __LINE__;                                  \
        throw std::runtime_error( strm.str() );                         \
    }                                                                   \
}                                                                       \
while( 0 )

//-*****************************************************************************
#define TESTING_RUN( TEST, VAL )                                        \
do                                                                      \
{                                                                       \
    try                                                                 \
    {                                                                   \
        {                                                               \
            TEST ;                                                      \
        }                                                               \
                                                                        \
                                                                        \
    }                                                                   \
    catch ( std::exception &exc )                                       \
    {                                                                   \
        std::cerr << "ERROR: EXCEPTION: " << exc.what() << std::endl;   \
        VAL ++;                                                         \
    }                                                                   \
    catch ( ... )                                                       \
    {                                                                   \
        std::cerr << "ERROR: UNKNOWN EXCEPTION" << std::endl;           \
        VAL ++;                                                         \
    }                                                                   \
}                                                                       \
while( 0 )

#endif
