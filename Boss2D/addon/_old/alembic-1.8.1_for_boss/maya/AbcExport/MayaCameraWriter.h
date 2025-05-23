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

#ifndef AbcExport_MayaCameraWriter_h
#define AbcExport_MayaCameraWriter_h

#include "Foundation.h"
#include "AttributesWriter.h"
#include "MayaTransformWriter.h"

#include BOSS_ALEMBIC_V_Alembic__AbcGeom__OCamera_h //original-code:<Alembic/AbcGeom/OCamera.h>

// Writes an MFnCamera
class MayaCameraWriter
{
  public:

    MayaCameraWriter(MDagPath & iDag, Alembic::Abc::OObject & iParent,
        Alembic::Util::uint32_t iTimeIndex, const JobArgs & iArgs);
    AttributesWriterPtr getAttrs() {return mAttrs;};
    void write();
    bool isAnimated() const;

  private:

    bool mIsAnimated;
    MDagPath mDagPath;
    bool mUseRenderShutter;
    double mShutterOpen;
    double mShutterClose;
    Alembic::AbcGeom::OCameraSchema mSchema;
    AttributesWriterPtr mAttrs;

    Alembic::AbcGeom::CameraSample mSamp;
    MFnCamera::FilmFit mFilmFit;
};

typedef Alembic::Util::shared_ptr < MayaCameraWriter > MayaCameraWriterPtr;

#endif  // AbcExport_MayaCameraWriter_h
