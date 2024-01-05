//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#ifndef _AbcOpenGL_DrawContext_h_
#define _AbcOpenGL_DrawContext_h_

#include "Foundation.h"
#include "ColorOverride.h"

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
class DrawContext
{
public:
    DrawContext()
    {
        m_worldToCamera.makeIdentity();
        m_pointSize = 3.0f;
        m_visibleOnly = false;
        m_boundsOnly = false;
    }

    // Default copy & assign.

    // Get/Set world-to-camera
    const M44d &getWorldToCamera() const { return m_worldToCamera; }
    void setWorldToCamera( const M44d & iXf ) { m_worldToCamera = iXf; }

    // Get/Set point size
    float getPointSize() const { return m_pointSize; }
    void setPointSize( float iPs ) { m_pointSize = iPs; }

    // Get/Set visibility toggle - don't draw objs w/ visible=0
    bool visibleOnly() const { return m_visibleOnly; }
    void setVisibleOnly( bool visibleOnly ) { m_visibleOnly = visibleOnly; }

    // Get/Set draw bounds toggle - draw object bounds only
    bool boundsOnly() const { return m_boundsOnly; }
    void setBoundsOnly( bool boundsOnly ) { m_boundsOnly = boundsOnly; }
    
    // Get/Set colour override map
    const ColorOverride &getColorOverrides() const{ return m_ctx_color_overrides; }
    void setColorOverrides(const ColorOverride & color_overrides){ m_ctx_color_overrides = color_overrides; }

    //added by BOSS
    const M44d& getZayMatrix() const { return m_zayMatrix; }
    void setZayMatrix(const double* m16)
    {
        M44d NewMatrix(
            m16[0], m16[1], m16[2], m16[3],
            m16[4], m16[5], m16[6], m16[7],
            m16[8], m16[9], m16[10], m16[11],
            m16[12], m16[13], m16[14], m16[15]);
        m_zayMatrix = NewMatrix;
    }

protected:
    M44d m_worldToCamera;
    float m_pointSize;
    bool m_visibleOnly;
    bool m_boundsOnly;
    ColorOverride m_ctx_color_overrides;
    M44d m_zayMatrix; //added by BOSS
};

} // End namespace ABCOPENGL_VERSION_NS

using namespace ABCOPENGL_VERSION_NS;

} // End namespace AbcOpenGL

#endif
