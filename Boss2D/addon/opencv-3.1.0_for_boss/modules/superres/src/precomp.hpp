/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
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

#include <vector>
#include <limits>

#include BOSS_OPENCV_U_opencv2__opencv_modules_hpp //original-code:"opencv2/opencv_modules.hpp"
#include BOSS_OPENCV_U_opencv2__core_hpp //original-code:"opencv2/core.hpp"
#include BOSS_OPENCV_U_opencv2__core__cuda_hpp //original-code:"opencv2/core/cuda.hpp"
#include BOSS_OPENCV_U_opencv2__core__opengl_hpp //original-code:"opencv2/core/opengl.hpp"
#include BOSS_OPENCV_U_opencv2__core__utility_hpp //original-code:"opencv2/core/utility.hpp"
#include BOSS_OPENCV_U_opencv2__imgproc_hpp //original-code:"opencv2/imgproc.hpp"
#include BOSS_OPENCV_U_opencv2__video__tracking_hpp //original-code:"opencv2/video/tracking.hpp"
#include BOSS_OPENCV_U_opencv2__core__private_hpp //original-code:"opencv2/core/private.hpp"

#include BOSS_OPENCV_U_opencv2__core__private_D_cuda_hpp //original-code:"opencv2/core/private.cuda.hpp"
#include BOSS_OPENCV_U_opencv2__core__ocl_hpp //original-code:"opencv2/core/ocl.hpp"

#ifdef HAVE_OPENCV_CUDAARITHM
#  include "opencv2/cudaarithm.hpp"
#endif

#ifdef HAVE_OPENCV_CUDAWARPING
#  include "opencv2/cudawarping.hpp"
#endif

#ifdef HAVE_OPENCV_CUDAFILTERS
#  include "opencv2/cudafilters.hpp"
#endif

#ifdef HAVE_OPENCV_CUDAIMGPROC
#  include "opencv2/cudaimgproc.hpp"
#endif

#ifdef HAVE_OPENCV_CUDAOPTFLOW
#  include "opencv2/cudaoptflow.hpp"
#endif

#ifdef HAVE_OPENCV_CUDACODEC
#  include "opencv2/cudacodec.hpp"
#endif

#ifdef HAVE_OPENCV_VIDEOIO
    #include BOSS_OPENCV_U_opencv2__videoio_hpp //original-code:"opencv2/videoio.hpp"
#endif

#include "opencv2/superres.hpp"
#include "opencv2/superres/optical_flow.hpp"
#include "input_array_utility.hpp"

#include "ring_buffer.hpp"

#include BOSS_OPENCV_U_opencv2__core__private_hpp //original-code:"opencv2/core/private.hpp"

#endif /* __OPENCV_PRECOMP_H__ */
