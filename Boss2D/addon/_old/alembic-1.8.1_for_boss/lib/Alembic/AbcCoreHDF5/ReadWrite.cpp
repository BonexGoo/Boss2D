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

#include BOSS_ALEMBIC_V_Alembic__AbcCoreHDF5__ReadWrite_h //original-code:<Alembic/AbcCoreHDF5/ReadWrite.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreHDF5__Foundation_h //original-code:<Alembic/AbcCoreHDF5/Foundation.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreHDF5__AwImpl_h //original-code:<Alembic/AbcCoreHDF5/AwImpl.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreHDF5__ArImpl_h //original-code:<Alembic/AbcCoreHDF5/ArImpl.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreHDF5__CacheImpl_h //original-code:<Alembic/AbcCoreHDF5/CacheImpl.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
WriteArchive::WriteArchive()
{
    m_cacheHierarchy = false;
}

//-*****************************************************************************
WriteArchive::WriteArchive( bool iCacheHierarchy )
{
    m_cacheHierarchy = iCacheHierarchy;
}

//-*****************************************************************************
AbcA::ArchiveWriterPtr
WriteArchive::operator()( const std::string &iFileName,
                          const AbcA::MetaData &iMetaData ) const
{
    Alembic::Util::shared_ptr<AwImpl> archivePtr(
        new AwImpl( iFileName, iMetaData, m_cacheHierarchy ) );

    return archivePtr;
}

//-*****************************************************************************
AbcA::ReadArraySampleCachePtr
CreateCache()
{
    AbcA::ReadArraySampleCachePtr cachePtr( new CacheImpl() );
    return cachePtr;
}


//-*****************************************************************************
ReadArchive::ReadArchive()
{
    m_cacheHierarchy = false;
}

//-*****************************************************************************
ReadArchive::ReadArchive( bool iCacheHierarchy )
{
    m_cacheHierarchy = iCacheHierarchy;
}

//-*****************************************************************************
// This version creates a cache.
AbcA::ArchiveReaderPtr
ReadArchive::operator()( const std::string &iFileName ) const
{
    AbcA::ReadArraySampleCachePtr cachePtr = CreateCache();
    Alembic::Util::shared_ptr<ArImpl> archivePtr(
        new ArImpl( iFileName, cachePtr, m_cacheHierarchy ) );

    return archivePtr;
}

//-*****************************************************************************
// This version takes a cache from outside.
AbcA::ArchiveReaderPtr
ReadArchive::operator()( const std::string &iFileName,
                         AbcA::ReadArraySampleCachePtr iCachePtr ) const
{
    Alembic::Util::shared_ptr<ArImpl> archivePtr(
        new ArImpl( iFileName, iCachePtr, m_cacheHierarchy ) );
    return archivePtr;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreHDF5
} // End namespace Alembic
