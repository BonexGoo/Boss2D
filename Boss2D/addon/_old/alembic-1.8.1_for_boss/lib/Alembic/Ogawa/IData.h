//-*****************************************************************************
//
// Copyright (c) 2013,
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

#ifndef Alembic_Ogawa_IData_h
#define Alembic_Ogawa_IData_h

#include BOSS_ALEMBIC_V_Alembic__Util__Export_h //original-code:<Alembic/Util/Export.h>
#include BOSS_ALEMBIC_V_Alembic__Ogawa__Foundation_h //original-code:<Alembic/Ogawa/Foundation.h>
#include BOSS_ALEMBIC_V_Alembic__Ogawa__IStreams_h //original-code:<Alembic/Ogawa/IStreams.h>

namespace Alembic {
namespace Ogawa {
namespace ALEMBIC_VERSION_NS {

class ALEMBIC_EXPORT IData
{
public:

    ~IData();

    void read(Alembic::Util::uint64_t iSize, void * iData,
              Alembic::Util::uint64_t iOffset, std::size_t iThreadId);

    Alembic::Util::uint64_t getSize() const;

    // not really necessary for most workflows, it could be used by some
    // Ogawa utilities to detect when this IData is shared
    Alembic::Util::uint64_t getPos() const;

private:
    friend class IGroup;
    IData(IStreamsPtr iStreams, Alembic::Util::uint64_t iPos,
          std::size_t iThreadId);

    class PrivateData;
    Alembic::Util::unique_ptr< PrivateData > mData;
};

typedef Alembic::Util::shared_ptr< IData > IDataPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Ogawa

} // End namespace Alembic

#endif
