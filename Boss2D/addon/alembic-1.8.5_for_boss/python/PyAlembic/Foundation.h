//-*****************************************************************************
//
// Copyright (c) 2009-2016,
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

#ifndef PyAlembic_Foundation_h_
#define PyAlembic_Foundation_h_

#include <Python.h>
#include BOSS_ALEMBIC_V_Alembic__Abc__All_h //original-code:<Alembic/Abc/All.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreAbstract__All_h //original-code:<Alembic/AbcCoreAbstract/All.h>
#include BOSS_ALEMBIC_V_Alembic__Util__Config_h //original-code:<Alembic/Util/Config.h>
#ifdef ALEMBIC_WITH_HDF5
#include BOSS_ALEMBIC_V_Alembic__AbcCoreHDF5__All_h //original-code:<Alembic/AbcCoreHDF5/All.h>
#endif
#include BOSS_ALEMBIC_V_Alembic__AbcCoreOgawa__All_h //original-code:<Alembic/AbcCoreOgawa/All.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreFactory__All_h //original-code:<Alembic/AbcCoreFactory/All.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCollection__All_h //original-code:<Alembic/AbcCollection/All.h>
#include BOSS_ALEMBIC_V_Alembic__AbcGeom__All_h //original-code:<Alembic/AbcGeom/All.h>
#include BOSS_ALEMBIC_V_Alembic__AbcMaterial__All_h //original-code:<Alembic/AbcMaterial/All.h>
#include <string>

#include <boost/version.hpp>

// After boost 1.53 boost provides this function for std::shared_ptr
#if (BOOST_VERSION < 105300)
namespace boost
{
template<class T>
inline T * get_pointer( Alembic::Util::shared_ptr<T> const & p )
{
        return p.get();
}

} // namespace boost
#endif

#if PY_MAJOR_VERSION < 3
#define ALEMBIC_PYTHON_BOOL_NAME "__nonzero__"
#define ALEMBIC_PYTHON_NEXT_NAME "next"
#else
#define ALEMBIC_PYTHON_BOOL_NAME "__bool__"
#define ALEMBIC_PYTHON_NEXT_NAME "__next__"
#endif

#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>

namespace Abc  = ::Alembic::Abc;
namespace AbcA = ::Alembic::AbcCoreAbstract;
namespace AbcF = ::Alembic::AbcCoreFactory;
#ifdef ALEMBIC_WITH_HDF5
namespace AbcH = ::Alembic::AbcCoreHDF5;
#endif
namespace AbcO = ::Alembic::AbcCoreOgawa;
namespace AbcC = ::Alembic::AbcCollection;
namespace AbcG = ::Alembic::AbcGeom;
namespace AbcU = ::Alembic::Util;
namespace AbcM = ::Alembic::AbcMaterial;

//-*****************************************************************************
inline void throwPythonException( const char* iMessage )
{
    PyErr_SetString( PyExc_RuntimeError, iMessage );
    throw boost::python::error_already_set();
}

inline void throwPythonKeyException( const char* iMessage )
{
    PyErr_SetString( PyExc_KeyError, iMessage );
    throw boost::python::error_already_set();
}

inline void throwPythonIndexException( const char* iMessage )
{
    PyErr_SetString( PyExc_IndexError, iMessage );
    throw boost::python::error_already_set();
}

#endif
