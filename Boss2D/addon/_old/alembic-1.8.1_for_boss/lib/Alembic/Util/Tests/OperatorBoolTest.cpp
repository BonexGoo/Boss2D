//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include BOSS_ALEMBIC_V_Alembic__Util__OperatorBool_h //original-code:<Alembic/Util/OperatorBool.h>
#include <iostream>

//-*****************************************************************************
struct blah
{
    typedef blah this_type;
    int a;
    
    ALEMBIC_OPERATOR_BOOL( a != 0 );
};

namespace Gotcha {

template <class T>
struct simba
{
    typedef T value_type;
    typedef simba<T> this_type;

    T a;

    ALEMBIC_OPERATOR_BOOL( a != ((T)0) );
};

} // End namespace Gotcha

int main( int argc, char *argv[] )
{
    blah b;
    b.a = 52;
    if ( b )
    {
        std::cout << "Whoa." << std::endl;
    }
    else
    {
        std::cout << "Not whoa." << std::endl;
    }

    Gotcha::simba<float> c;
    c.a = 7.2f;
    if ( !c )
    {
        std::cout << "Also." << std::endl;
    }
    else
    {
        std::cout << "Not also." << std::endl;
    }
    return 0;
}
