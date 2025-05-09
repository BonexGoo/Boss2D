//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#ifndef Alembic_AbcGeom_ICamera_h
#define Alembic_AbcGeom_ICamera_h

#include BOSS_ALEMBIC_V_Alembic__Util__Export_h //original-code:<Alembic/Util/Export.h>
#include BOSS_ALEMBIC_V_Alembic__AbcGeom__Foundation_h //original-code:<Alembic/AbcGeom/Foundation.h>
#include BOSS_ALEMBIC_V_Alembic__AbcGeom__SchemaInfoDeclarations_h //original-code:<Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include BOSS_ALEMBIC_V_Alembic__AbcGeom__CameraSample_h //original-code:<Alembic/AbcGeom/CameraSample.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ALEMBIC_EXPORT ICameraSchema : public Abc::ISchema<CameraSchemaInfo>
{
    //-*************************************************************************
    // CAMERA SCHEMA
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef ICameraSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OCameraMeshSchema
    //! ...
    ICameraSchema() {}

    //! This constructor creates a new camera reader.
    //! The first argument is the parent ICompoundProperty, from which the
    //! error handler policy for is derived.  The second argument is the name
    //! of the ICompoundProperty that contains this schemas properties.  The
    //! remaining optional arguments can be used to override the
    //! ErrorHandlerPolicy and to specify schema interpretation matching.
    ICameraSchema( const ICompoundProperty & iParent,
                   const std::string &iName,

                   const Abc::Argument &iArg0 = Abc::Argument(),
                   const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<CameraSchemaInfo>( iParent, iName, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! This constructor wraps an existing ICompoundProperty as the camera
    //! reader, and the error handler policy is derived from it.
    //! The  remaining optional arguments can be used to override the
    //! ErrorHandlerPolicy and to specify schema interpretation matching.
    ICameraSchema( const ICompoundProperty & iProp,
                   const Abc::Argument &iArg0 = Abc::Argument(),
                   const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<CameraSchemaInfo>( iProp, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Default assignment and copy operator used.

    //! Return the time sampling.
    AbcA::TimeSamplingPtr getTimeSampling() const
    { return m_coreProperties.getTimeSampling(); }

    //! Return the number of samples contained in the property.
    //! This can be any number, including zero.
    //! This returns the number of samples that were written, independently
    //! of whether or not they were constant.
    size_t getNumSamples() const
    { return m_coreProperties.getNumSamples(); }

    //! Ask if we're constant - no change in value amongst samples,
    //! regardless of the time sampling.
    bool isConstant() const;

    void get( CameraSample &oSample,
              const Abc::ISampleSelector &iSS = Abc::ISampleSelector() ) const;

    CameraSample getValue(
        const Abc::ISampleSelector &iSS = Abc::ISampleSelector() ) const
    {
        CameraSample smp;
        get( smp, iSS );
        return smp;
    }

    // compound property to use as parent for any arbitrary GeomParams
    // underneath it
    ICompoundProperty getArbGeomParams() const { return m_arbGeomParams; }
    ICompoundProperty getUserProperties() const { return m_userProperties; }

    Abc::IBox3dProperty getChildBoundsProperty() const { return m_childBoundsProperty; }

    //! Reset returns this function set to an empty, default
    //! state.
    void reset()
    {
        m_coreProperties.reset();
        m_childBoundsProperty.reset();
        m_arbGeomParams.reset();
        m_userProperties.reset();
        m_ops.clear();
        Abc::ISchema<CameraSchemaInfo>::reset();
    }

    //! Returns whether this function set is valid.
    bool valid() const
    {
        return ( Abc::ISchema<CameraSchemaInfo>::valid() &&
                 m_coreProperties.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( ICameraSchema::valid() );

protected:
    void init( const Abc::Argument &iArg0,
               const Abc::Argument &iArg1 );

    Abc::IScalarProperty m_coreProperties;

    Abc::IBox3dProperty m_childBoundsProperty;

    Abc::ICompoundProperty m_arbGeomParams;
    Abc::ICompoundProperty m_userProperties;

    Abc::IScalarProperty m_smallFilmBackChannels;
    Abc::IDoubleArrayProperty m_largeFilmBackChannels;

private:
    std::vector < FilmBackXformOp > m_ops;

};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::ISchemaObject<ICameraSchema> ICamera;

typedef Util::shared_ptr< ICamera > ICameraPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
