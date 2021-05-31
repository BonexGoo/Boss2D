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

#ifndef _AbcOpenGL_Foundation_h_
#define _AbcOpenGL_Foundation_h_

#define ABCOPENGL_VERSION_NS v1

#include BOSS_ALEMBIC_V_Alembic__Abc__All_h //original-code:<Alembic/Abc/All.h>
#include BOSS_ALEMBIC_V_Alembic__AbcGeom__All_h //original-code:<Alembic/AbcGeom/All.h>
#include BOSS_ALEMBIC_V_Alembic__AbcCoreFactory__All_h //original-code:<Alembic/AbcCoreFactory/All.h>
#include BOSS_ALEMBIC_V_Alembic__Util__All_h //original-code:<Alembic/Util/All.h>

#include BOSS_OPENCV_V_ImathMath_h //original-code:<ImathMath.h>
#include BOSS_OPENCV_V_ImathVec_h //original-code:<ImathVec.h>
#include BOSS_OPENCV_V_ImathMatrix_h //original-code:<ImathMatrix.h>
#include BOSS_OPENCV_V_ImathBox_h //original-code:<ImathBox.h>
#include BOSS_OPENCV_V_ImathQuat_h //original-code:<ImathQuat.h>
#include BOSS_OPENCV_V_ImathColor_h //original-code:<ImathColor.h>
#include BOSS_OPENCV_V_ImathFun_h //original-code:<ImathFun.h>
#include BOSS_OPENCV_V_ImathBoxAlgo_h //original-code:<ImathBoxAlgo.h>

#include <iostream>
#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <exception>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MAC INCLUDES
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
#ifdef PLATFORM_DARWIN

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// NON-MAC INCLUDES (Configured for multiple-context GLEW)
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
#else


#ifdef ABCVIEW_GLEW_MX

#define GLEW_MX
#include BOSS_FREEGLUT_V_GL__glew_h //original-code:<GL/glew.h>
extern "C" GLEWContext *glewGetContext();

#else

#include BOSS_FREEGLUT_V_GL__glew_h //original-code:<GL/glew.h>

#endif // ifdef ABCVIEW_GLEW_MX

#include <GL/gl.h>

#ifdef PLATFORM_WINDOWS
#include BOSS_FREEGLUT_V_GL__wglew_h //original-code:<GL/wglew.h>
#include BOSS_FREEGLUT_V_GL__glut_h //original-code:<GL/glut.h>
#else
#include BOSS_FREEGLUT_V_GL__glxew_h //original-code:<GL/glxew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include BOSS_FREEGLUT_V_GL__glut_h //original-code:<GL/glut.h>

#endif // ifdef WIN64

#endif // ifdef PLATFORM_DARWIN

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
namespace Abc = Alembic::AbcGeom;
using namespace Abc;

//-*****************************************************************************
//-*****************************************************************************
// FUNCTIONS
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
template <class T>
inline T degrees( const T &rads ) { return 180.0 * rads / M_PI; }

//-*****************************************************************************
template <class T>
inline T radians( const T &degs ) { return M_PI * degs / 180.0; }

//-*****************************************************************************
template <class T>
inline const T &clamp( const T &x, const T &lo, const T &hi )
{
    return x < lo ? lo : x > hi ? hi : x;
}


//-*****************************************************************************
//-*****************************************************************************
// GL ERROR CHECKING
//-*****************************************************************************
//-*****************************************************************************
inline void GL_CHECK( const std::string &header = "" )
{
    GLenum errCode = glGetError();
    if ( errCode != GL_NO_ERROR )
    {
        std::cerr << "GL ERROR in " << header << ": "
                  << ( const char * )gluErrorString( errCode )
                  << std::endl;

        GLint matrixStackDepth = 0;
        glGetIntegerv( GL_MODELVIEW_STACK_DEPTH, &matrixStackDepth );
        std::cout << "Stack depth: " << ( int )matrixStackDepth
                  << std::endl;
    }
}

//-*****************************************************************************
#define make_string(x) #x

#define GL_NOISY( CODE )                        \
do                                              \
{                                               \
    CODE ;                                      \
    std::string msg = "Code: ";                 \
    msg += make_string( CODE );                 \
    GL_CHECK( msg );                            \
}                                               \
while( 0 )

} // End namespace ABCOPENGL_VERSION_NS

using namespace ABCOPENGL_VERSION_NS;

} // End namespace AbcOpenGL

#endif
