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

#include BOSS_ALEMBIC_V_Alembic__AbcCoreOgawa__ReadWrite_h //original-code:<Alembic/AbcCoreOgawa/ReadWrite.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreOgawa__Foundation_h //original-code:<Alembic/AbcCoreOgawa/Foundation.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreOgawa__AwImpl_h //original-code:<Alembic/AbcCoreOgawa/AwImpl.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreOgawa__ArImpl_h //original-code:<Alembic/AbcCoreOgawa/ArImpl.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
WriteArchive::WriteArchive()
{
}

//-*****************************************************************************
AbcA::ArchiveWriterPtr
WriteArchive::operator()( const std::string &iFileName,
                          const AbcA::MetaData &iMetaData ) const
{
    Alembic::Util::shared_ptr<AwImpl> archivePtr(
        new AwImpl( iFileName, iMetaData ) );
    return archivePtr;
}

//-*****************************************************************************
AbcA::ArchiveWriterPtr
WriteArchive::operator()( std::ostream * iStream,
                          const AbcA::MetaData &iMetaData ) const
{
    Alembic::Util::shared_ptr<AwImpl> archivePtr(
        new AwImpl( iStream, iMetaData ) );
    return archivePtr;
}

//-*****************************************************************************
ReadArchive::ReadArchive()
{
    m_numStreams = 1;
    m_useMMap = true;
}

//-*****************************************************************************
ReadArchive::ReadArchive( size_t iNumStreams, bool iUseMMap )
{
    m_numStreams = iNumStreams;
    m_useMMap = iUseMMap;
}

//-*****************************************************************************
ReadArchive::ReadArchive( const std::vector< std::istream * > & iStreams )
    : m_numStreams( 1 ), m_useMMap(true), m_streams( iStreams )
{
}

//-*****************************************************************************
AbcA::ArchiveReaderPtr
ReadArchive::operator()( const std::string &iFileName ) const
{
    AbcA::ArchiveReaderPtr archivePtr;

    if ( m_streams.empty() )
    {
        archivePtr = Alembic::Util::shared_ptr<ArImpl>(
            new ArImpl( iFileName, m_numStreams, m_useMMap ) );
    }
    else
    {
        archivePtr = Alembic::Util::shared_ptr<ArImpl>(
            new ArImpl( m_streams ) );
    }
    return archivePtr;
}

//-*****************************************************************************
// The given cache is ignored.
AbcA::ArchiveReaderPtr
ReadArchive::operator()( const std::string &iFileName,
            AbcA::ReadArraySampleCachePtr iCache ) const
{
    AbcA::ArchiveReaderPtr archivePtr;

    if ( m_streams.empty() )
    {
        archivePtr = Alembic::Util::shared_ptr<ArImpl> (
            new ArImpl( iFileName, m_numStreams, m_useMMap ) );
    }
    else
    {
        archivePtr = Alembic::Util::shared_ptr<ArImpl> (
            new ArImpl( m_streams ) );
    }
    return archivePtr;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreOgawa
} // End namespace Alembic
