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

#ifndef __OPENCV_PRECOMP_H__
#define __OPENCV_PRECOMP_H__

#include BOSS_OPENCV_U_opencv2__objdetect_hpp //original-code:"opencv2/objdetect.hpp"
#include BOSS_OPENCV_U_opencv2__imgproc_hpp //original-code:"opencv2/imgproc.hpp"

#include BOSS_OPENCV_U_opencv2__ml_hpp //original-code:"opencv2/ml.hpp"

#include BOSS_OPENCV_U_opencv2__core__utility_hpp //original-code:"opencv2/core/utility.hpp"
#include BOSS_OPENCV_U_opencv2__core__ocl_hpp //original-code:"opencv2/core/ocl.hpp"

#include BOSS_OPENCV_U_opencv2__opencv_modules_hpp //original-code:"opencv2/opencv_modules.hpp"
#ifdef HAVE_OPENCV_HIGHGUI
#  include BOSS_OPENCV_U_opencv2__highgui_hpp //original-code:"opencv2/highgui.hpp"
#endif

#include BOSS_OPENCV_U_opencv2__core__private_hpp //original-code:"opencv2/core/private.hpp"

#ifdef HAVE_TEGRA_OPTIMIZATION
#include "opencv2/objdetect/objdetect_tegra.hpp"
#endif

#endif
