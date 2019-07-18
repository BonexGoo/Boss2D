// author BOSS

/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Copyright (C) 2015, Itseez Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __OPENCV_CORE_FAST_MATH_HPP__
#define __OPENCV_CORE_FAST_MATH_HPP__

#include BOSS_OPENCV_U_opencv2__core__cvdef_h //original-code:"opencv2/core/cvdef.h"

//! @addtogroup core_utils
//! @{

/****************************************************************************************\
*                                      fast math                                         *
\****************************************************************************************/

#if defined __BORLANDC__
#  include <fastmath.h>
#elif defined __cplusplus
#  include <cmath>
#else
#  include <math.h>
#endif

#ifdef HAVE_TEGRA_OPTIMIZATION
#  include "tegra_round.hpp"
#endif

#if CV_VFP
    // 1. general scheme
    #define ARM_ROUND(_value, _asm_string) \
        int res; \
        float temp; \
        asm(_asm_string : [res] "=r" (res), [temp] "=w" (temp) : [value] "w" (_value)); \
        return res
    // 2. version for double
    #ifdef __clang__
        #define ARM_ROUND_DBL(value) ARM_ROUND(value, "vcvtr.s32.f64 %[temp], %[value] \n vmov %[res], %[temp]")
    #else
        #define ARM_ROUND_DBL(value) ARM_ROUND(value, "vcvtr.s32.f64 %[temp], %P[value] \n vmov %[res], %[temp]")
    #endif
    // 3. version for float
    #define ARM_ROUND_FLT(value) ARM_ROUND(value, "vcvtr.s32.f32 %[temp], %[value]\n vmov %[res], %[temp]")
#endif // CV_VFP

/** @brief Rounds floating-point number to the nearest integer

 @param value floating-point number. If the value is outside of INT_MIN ... INT_MAX range, the
 result is not defined.
 */
CV_INLINE int
cvRound( double value )
{
    return (int)(value + (value >= 0 ? 0.5 : -0.5)); // added by BOSS

// removed by BOSS: #if ((defined _MSC_VER && defined _M_X64) || (defined __GNUC__ && defined __x86_64__ \
// removed by BOSS:     && defined __SSE2__ && !defined __APPLE__)) && !defined(__CUDACC__)
// removed by BOSS:     __m128d t = _mm_set_sd( value );
// removed by BOSS:     return _mm_cvtsd_si32(t);
// removed by BOSS: #elif defined _MSC_VER && defined _M_IX86
// removed by BOSS:     int t;
// removed by BOSS:     __asm
// removed by BOSS:     {
// removed by BOSS:         fld value;
// removed by BOSS:         fistp t;
// removed by BOSS:     }
// removed by BOSS:     return t;
// removed by BOSS: #elif ((defined _MSC_VER && defined _M_ARM) || defined CV_ICC || \
// removed by BOSS:         defined __GNUC__) && defined HAVE_TEGRA_OPTIMIZATION
// removed by BOSS:     TEGRA_ROUND_DBL(value);
// removed by BOSS: #elif defined CV_ICC || defined __GNUC__
// removed by BOSS: # if CV_VFP
// removed by BOSS:     ARM_ROUND_DBL(value);
// removed by BOSS: # else
// removed by BOSS:     return (int)lrint(value);
// removed by BOSS: # endif
// removed by BOSS: #else
// removed by BOSS:     /* it's ok if round does not comply with IEEE754 standard;
// removed by BOSS:        the tests should allow +/-1 difference when the tested functions use round */
// removed by BOSS:     return (int)(value + (value >= 0 ? 0.5 : -0.5));
// removed by BOSS: #endif
}


/** @brief Rounds floating-point number to the nearest integer not larger than the original.

 The function computes an integer i such that:
 \f[i \le \texttt{value} < i+1\f]
 @param value floating-point number. If the value is outside of INT_MIN ... INT_MAX range, the
 result is not defined.
 */
CV_INLINE int cvFloor( double value )
{
    int i = cvRound(value); // added by BOSS
    float diff = (float)(value - i); // added by BOSS
    return i - (diff < 0); // added by BOSS

// removed by BOSS: #if (defined _MSC_VER && defined _M_X64 || (defined __GNUC__ && defined __SSE2__ && !defined __APPLE__)) && !defined(__CUDACC__)
// removed by BOSS:     __m128d t = _mm_set_sd( value );
// removed by BOSS:     int i = _mm_cvtsd_si32(t);
// removed by BOSS:     return i - _mm_movemask_pd(_mm_cmplt_sd(t, _mm_cvtsi32_sd(t,i)));
// removed by BOSS: #elif defined __GNUC__
// removed by BOSS:     int i = (int)value;
// removed by BOSS:     return i - (i > value);
// removed by BOSS: #else
// removed by BOSS:     int i = cvRound(value);
// removed by BOSS:     float diff = (float)(value - i);
// removed by BOSS:     return i - (diff < 0);
// removed by BOSS: #endif
}

/** @brief Rounds floating-point number to the nearest integer not smaller than the original.

 The function computes an integer i such that:
 \f[i \le \texttt{value} < i+1\f]
 @param value floating-point number. If the value is outside of INT_MIN ... INT_MAX range, the
 result is not defined.
 */
CV_INLINE int cvCeil( double value )
{
    int i = cvRound(value); // added by BOSS
    float diff = (float)(i - value); // added by BOSS
    return i + (diff < 0); // added by BOSS

// removed by BOSS: #if (defined _MSC_VER && defined _M_X64 || (defined __GNUC__ && defined __SSE2__&& !defined __APPLE__)) && !defined(__CUDACC__)
// removed by BOSS:     __m128d t = _mm_set_sd( value );
// removed by BOSS:     int i = _mm_cvtsd_si32(t);
// removed by BOSS:     return i + _mm_movemask_pd(_mm_cmplt_sd(_mm_cvtsi32_sd(t,i), t));
// removed by BOSS: #elif defined __GNUC__
// removed by BOSS:     int i = (int)value;
// removed by BOSS:     return i + (i < value);
// removed by BOSS: #else
// removed by BOSS:     int i = cvRound(value);
// removed by BOSS:     float diff = (float)(i - value);
// removed by BOSS:     return i + (diff < 0);
// removed by BOSS: #endif
}

/** @brief Determines if the argument is Not A Number.

 @param value The input floating-point value

 The function returns 1 if the argument is Not A Number (as defined by IEEE754 standard), 0
 otherwise. */
CV_INLINE int cvIsNaN( double value )
{
    Cv64suf ieee754;
    ieee754.f = value;
    return ((unsigned)(ieee754.u >> 32) & 0x7fffffff) +
           ((unsigned)ieee754.u != 0) > 0x7ff00000;
}

/** @brief Determines if the argument is Infinity.

 @param value The input floating-point value

 The function returns 1 if the argument is a plus or minus infinity (as defined by IEEE754 standard)
 and 0 otherwise. */
CV_INLINE int cvIsInf( double value )
{
    Cv64suf ieee754;
    ieee754.f = value;
    return ((unsigned)(ieee754.u >> 32) & 0x7fffffff) == 0x7ff00000 &&
            (unsigned)ieee754.u == 0;
}

#ifdef __cplusplus

/** @overload */
CV_INLINE int cvRound(float value)
{
    {return (int)(value + (value >= 0 ? 0.5f : -0.5f));} // added by BOSS

// removed by BOSS: #if ((defined _MSC_VER && defined _M_X64) || (defined __GNUC__ && defined __x86_64__ && \
// removed by BOSS:       defined __SSE2__ && !defined __APPLE__)) && !defined(__CUDACC__)
// removed by BOSS:     __m128 t = _mm_set_ss( value );
// removed by BOSS:     return _mm_cvtss_si32(t);
// removed by BOSS: #elif defined _MSC_VER && defined _M_IX86
// removed by BOSS:     int t;
// removed by BOSS:     __asm
// removed by BOSS:     {
// removed by BOSS:         fld value;
// removed by BOSS:         fistp t;
// removed by BOSS:     }
// removed by BOSS:     return t;
// removed by BOSS: #elif ((defined _MSC_VER && defined _M_ARM) || defined CV_ICC || \
// removed by BOSS:         defined __GNUC__) && defined HAVE_TEGRA_OPTIMIZATION
// removed by BOSS:     TEGRA_ROUND_FLT(value);
// removed by BOSS: #elif defined CV_ICC || defined __GNUC__
// removed by BOSS: # if CV_VFP
// removed by BOSS:     ARM_ROUND_FLT(value);
// removed by BOSS: # else
// removed by BOSS:     return (int)lrintf(value);
// removed by BOSS: # endif
// removed by BOSS: #else
// removed by BOSS:     /* it's ok if round does not comply with IEEE754 standard;
// removed by BOSS:      the tests should allow +/-1 difference when the tested functions use round */
// removed by BOSS:     return (int)(value + (value >= 0 ? 0.5f : -0.5f));
// removed by BOSS: #endif
}

/** @overload */
CV_INLINE int cvRound( int value )
{
    return value;
}

/** @overload */
CV_INLINE int cvFloor( float value )
{
    int i = cvRound(value); // added by BOSS
    float diff = (float)(value - i); // added by BOSS
    return i - (diff < 0); // added by BOSS

// removed by BOSS: #if (defined _MSC_VER && defined _M_X64 || (defined __GNUC__ && defined __SSE2__ && !defined __APPLE__)) && !defined(__CUDACC__)
// removed by BOSS:     __m128 t = _mm_set_ss( value );
// removed by BOSS:     int i = _mm_cvtss_si32(t);
// removed by BOSS:     return i - _mm_movemask_ps(_mm_cmplt_ss(t, _mm_cvtsi32_ss(t,i)));
// removed by BOSS: #elif defined __GNUC__
// removed by BOSS:     int i = (int)value;
// removed by BOSS:     return i - (i > value);
// removed by BOSS: #else
// removed by BOSS:     int i = cvRound(value);
// removed by BOSS:     float diff = (float)(value - i);
// removed by BOSS:     return i - (diff < 0);
// removed by BOSS: #endif
}

/** @overload */
CV_INLINE int cvFloor( int value )
{
    return value;
}

/** @overload */
CV_INLINE int cvCeil( float value )
{
    int i = cvRound(value); // added by BOSS
    float diff = (float)(i - value); // added by BOSS
    return i + (diff < 0); // added by BOSS

// removed by BOSS: #if (defined _MSC_VER && defined _M_X64 || (defined __GNUC__ && defined __SSE2__&& !defined __APPLE__)) && !defined(__CUDACC__)
// removed by BOSS:     __m128 t = _mm_set_ss( value );
// removed by BOSS:     int i = _mm_cvtss_si32(t);
// removed by BOSS:     return i + _mm_movemask_ps(_mm_cmplt_ss(_mm_cvtsi32_ss(t,i), t));
// removed by BOSS: #elif defined __GNUC__
// removed by BOSS:     int i = (int)value;
// removed by BOSS:     return i + (i < value);
// removed by BOSS: #else
// removed by BOSS:     int i = cvRound(value);
// removed by BOSS:     float diff = (float)(i - value);
// removed by BOSS:     return i + (diff < 0);
// removed by BOSS: #endif
}

/** @overload */
CV_INLINE int cvCeil( int value )
{
    return value;
}

/** @overload */
CV_INLINE int cvIsNaN( float value )
{
    Cv32suf ieee754;
    ieee754.f = value;
    return (ieee754.u & 0x7fffffff) > 0x7f800000;
}

/** @overload */
CV_INLINE int cvIsInf( float value )
{
    Cv32suf ieee754;
    ieee754.f = value;
    return (ieee754.u & 0x7fffffff) == 0x7f800000;
}

#endif // __cplusplus

//! @} core_utils

#endif
