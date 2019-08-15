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
// Copyright (C) 2017, Intel Corporation, all rights reserved.
// Copyright (c) 2016-2017 Fabian David Tschopp, all rights reserved.
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

#define CONCAT(A,B) A##_##B
#define TEMPLATE(name,type) CONCAT(name,type)
#define KERNEL_ARG_DTYPE float

#if defined(cl_khr_fp16)
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#endif

__kernel void ReLUForward(const int count, __global const T* in, __global T* out
#ifndef RELU_NO_SLOPE
, KERNEL_ARG_DTYPE negative_slope
#endif
) {
  int index = get_global_id(0);
  if(index < count)
#ifndef RELU_NO_SLOPE
  out[index] = in[index] > 0 ? in[index] : in[index] * negative_slope;
#else
  out[index] = in[index] > 0 ? in[index] : 0;
#endif
}

__kernel void ReLU6Forward(const int count, __global const T* in, __global T* out,
                           const KERNEL_ARG_DTYPE minValue, const KERNEL_ARG_DTYPE maxValue)
{
  int index = get_global_id(0);
  if(index < count)
  {
    T x = in[index];
    out[index] = clamp(x, convert_T(minValue), convert_T(maxValue));
  }
}

__kernel void PReLUForward(const int count, const int channels, const int plane_size,
                           __global const T* in, __global T* out,
                           __global const KERNEL_ARG_DTYPE* slope_data)
{
  int index = get_global_id(0);
  int c = (index / plane_size) % channels;
  if(index < count)
  out[index] = in[index] > 0 ? in[index] : in[index] * slope_data[c];
}

__kernel void TanHForward(const int count, __global T* in, __global T* out) {
  int index = get_global_id(0);
  if(index < count)
  out[index] = tanh(in[index]);
}

__kernel void SigmoidForward(const int count, __global const T* in, __global T* out) {
  int index = get_global_id(0);
  if(index < count)
  out[index] = 1.0f / (1.0f + exp(-in[index]));
}

__kernel void BNLLForward(const int n, __global const T* in, __global T* out) {
  int index = get_global_id(0);
  if (index < n) {
    T x = in[index];
    out[index] = x > 0 ? x + log(1.0f + exp(-x)) : log(1.0f + exp(x));
  }
}

__kernel void AbsValForward(const int n, __global const T* in, __global T* out) {
  int index = get_global_id(0);
  if (index < n)
    out[index] = fabs(in[index]);
}

__kernel void PowForward(const int n, __global const T* in, __global T* out,
                         const KERNEL_ARG_DTYPE power,
                         const KERNEL_ARG_DTYPE scale,
                         const KERNEL_ARG_DTYPE shift)
{
  int index = get_global_id(0);
  if (index < n)
    out[index] = pow(shift + scale * in[index], power);
}

__kernel void ELUForward(const int n, __global const T* in, __global T* out)
{
  int index = get_global_id(0);
  if (index < n)
  {
    T src = in[index];
    out[index] = (src >= 0.f) ? src : exp(src) - 1;
  }
}
