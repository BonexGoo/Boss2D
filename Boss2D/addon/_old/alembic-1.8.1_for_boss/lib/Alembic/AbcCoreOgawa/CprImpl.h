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

#ifndef Alembic_AbcCoreOgawa_CprImpl_h
#define Alembic_AbcCoreOgawa_CprImpl_h

#include BOSS_ALEMBIC_V_Alembic__AbcCoreOgawa__Foundation_h //original-code:<Alembic/AbcCoreOgawa/Foundation.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreOgawa__CprData_h //original-code:<Alembic/AbcCoreOgawa/CprData.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class CprImpl
    : public AbcA::CompoundPropertyReader
    , public Alembic::Util::enable_shared_from_this<CprImpl>
{
public:

    // For construction from a compound property reader
    CprImpl( AbcA::CompoundPropertyReaderPtr iParent,
             Ogawa::IGroupPtr iGroup,
             PropertyHeaderPtr iHeader,
             std::size_t iThreadId,
             const std::vector< AbcA::MetaData > & iIndexedMetaData );

    CprImpl( AbcA::ObjectReaderPtr iParent,
             CprDataPtr iData );

    virtual ~CprImpl();

    //-*************************************************************************
    // FROM ABSTRACT BasePropertyReader
    //-*************************************************************************
    virtual const AbcA::PropertyHeader & getHeader() const;

    virtual AbcA::ObjectReaderPtr getObject();

    virtual AbcA::CompoundPropertyReaderPtr getParent();

    virtual AbcA::CompoundPropertyReaderPtr asCompoundPtr();

    //-*************************************************************************
    // FROM ABSTRACT CompoundPropertyReader
    //-*************************************************************************
    virtual size_t getNumProperties();

    virtual const AbcA::PropertyHeader & getPropertyHeader( size_t i );

    virtual const AbcA::PropertyHeader *
    getPropertyHeader( const std::string &iName );

    virtual AbcA::ScalarPropertyReaderPtr
    getScalarProperty( const std::string &iName );

    virtual AbcA::ArrayPropertyReaderPtr
    getArrayProperty( const std::string &iName );

    virtual AbcA::CompoundPropertyReaderPtr
    getCompoundProperty( const std::string &iName );


private:

    // Pointer to parent.
    AbcA::CompoundPropertyReaderPtr m_parent;

    // My header
    PropertyHeaderPtr m_header;

    // My Object
    AbcA::ObjectReaderPtr m_object;

    CprDataPtr m_data;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
