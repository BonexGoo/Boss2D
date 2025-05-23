//-*****************************************************************************
//
// Copyright (c) 2009-2011,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#include <AbcClients/WFObjConvert/All.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreHDF5__All_h //original-code:<Alembic/AbcCoreHDF5/All.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

namespace OBJ = AbcClients::WFObjConvert;
namespace Abc = Alembic::AbcGeom;

class MyReader : public OBJ::AbcReader
{
public:
    MyReader( Abc::OObject &iParentObj )
      : OBJ::AbcReader( iParentObj ) {}
    
    MyReader( Abc::OObject &iParentObj,
              const std::string &iDfltName )
      : OBJ::AbcReader( iParentObj, iDfltName ) {}
    
    virtual void parsingError( const std::string &iStreamName,
                               const std::string &iErrorDesc,
                               size_t iErrorLine )
    {
        std::cerr << iErrorDesc << std::endl;
        exit( -1 );
    }
    
    virtual void parsingEnd( const std::string &iStreamName,
                             size_t iNumLines )
    {
        OBJ::AbcReader::parsingEnd( iStreamName, iNumLines );
        
        std::cout << "Parsed OBJ File: " << iStreamName
                  << ", num lines parsed: " << iNumLines << std::endl;
    }
};

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    if ( argc != 3 )
    {
        std::cerr << "USAGE: " << argv[0] << " <objFile> <abcFile>"
                  << std::endl;
        return -1;
    }

    try
    {
        Abc::OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                               argv[2] );
        Abc::OObject topObj( archive, Abc::kTop );

        MyReader reader( topObj );
        
        OBJ::ParseOBJ( reader, argv[1] );
    }
    catch ( std::exception &exc )
    {
        std::cerr << "EXCEPTION: " << exc.what() << std::endl;
        exit( -1 );
    }

    return 0;
}
