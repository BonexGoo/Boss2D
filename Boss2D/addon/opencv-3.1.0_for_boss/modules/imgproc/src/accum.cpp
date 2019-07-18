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
// Copyright (C) 2014, Itseez Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
/
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

#include "precomp.hpp"
#include BOSS_OPENCV_U_opencl_kernels_imgproc_hpp //original-code:"opencl_kernels_imgproc.hpp"

namespace cv
{

template <typename T, typename AT>
struct Acc_SIMD
{
    int operator() (const T *, AT *, const uchar *, int, int) const
    {
        return 0;
    }
};

template <typename T, typename AT>
struct AccSqr_SIMD
{
    int operator() (const T *, AT *, const uchar *, int, int) const
    {
        return 0;
    }
};

template <typename T, typename AT>
struct AccProd_SIMD
{
    int operator() (const T *, const T *, AT *, const uchar *, int, int) const
    {
        return 0;
    }
};

template <typename T, typename AT>
struct AccW_SIMD
{
    int operator() (const T *, AT *, const uchar *, int, int, AT) const
    {
        return 0;
    }
};

#if CV_NEON

template <>
struct Acc_SIMD<uchar, float>
{
    int operator() (const uchar * src, float * dst, const uchar * mask, int len, int cn) const
    {
        int x = 0;

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 16; x += 16)
            {
                uint8x16_t v_src = vld1q_u8(src + x);
                uint16x8_t v_src0 = vmovl_u8(vget_low_u8(v_src)), v_src1 = vmovl_u8(vget_high_u8(v_src));

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src0)))));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src0)))));
                vst1q_f32(dst + x + 8, vaddq_f32(vld1q_f32(dst + x + 8), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src1)))));
                vst1q_f32(dst + x + 12, vaddq_f32(vld1q_f32(dst + x + 12), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src1)))));
            }
        }
        else if (cn == 1)
        {
            uint8x16_t v_255 = vdupq_n_u8(255), v_0 = vdupq_n_u8(0);

            for ( ; x <= len - 16; x += 16)
            {
                uint8x16_t v_src = vandq_u8(vld1q_u8(src + x), veorq_u8(v_255, vceqq_u8(vld1q_u8(mask + x), v_0)));
                uint16x8_t v_src0 = vmovl_u8(vget_low_u8(v_src)), v_src1 = vmovl_u8(vget_high_u8(v_src));

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src0)))));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src0)))));
                vst1q_f32(dst + x + 8, vaddq_f32(vld1q_f32(dst + x + 8), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src1)))));
                vst1q_f32(dst + x + 12, vaddq_f32(vld1q_f32(dst + x + 12), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src1)))));
            }
        }

        return x;
    }
};

template <>
struct Acc_SIMD<ushort, float>
{
    int operator() (const ushort * src, float * dst, const uchar * mask, int len, int cn) const
    {
        int x = 0;

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 8; x += 8)
            {
                uint16x8_t v_src = vld1q_u16(src + x);
                uint32x4_t v_src0 = vmovl_u16(vget_low_u16(v_src)), v_src1 = vmovl_u16(vget_high_u16(v_src));

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(v_src0)));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(v_src1)));
            }
        }

        return x;
    }
};

template <>
struct Acc_SIMD<float, float>
{
    int operator() (const float * src, float * dst, const uchar * mask, int len, int cn) const
    {
        int x = 0;

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 8; x += 8)
            {
                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vld1q_f32(src + x)));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vld1q_f32(src + x + 4)));
            }
        }

        return x;
    }
};

template <>
struct AccSqr_SIMD<uchar, float>
{
    int operator() (const uchar * src, float * dst, const uchar * mask, int len, int cn) const
    {
        int x = 0;

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 16; x += 16)
            {
                uint8x16_t v_src = vld1q_u8(src + x);
                uint8x8_t v_src_0 = vget_low_u8(v_src), v_src_1 = vget_high_u8(v_src);
                uint16x8_t v_src0 = vmull_u8(v_src_0, v_src_0), v_src1 = vmull_u8(v_src_1, v_src_1);

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src0)))));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src0)))));
                vst1q_f32(dst + x + 8, vaddq_f32(vld1q_f32(dst + x + 8), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src1)))));
                vst1q_f32(dst + x + 12, vaddq_f32(vld1q_f32(dst + x + 12), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src1)))));
            }
        }
        else if (cn == 1)
        {
            uint8x16_t v_255 = vdupq_n_u8(255), v_0 = vdupq_n_u8(0);

            for ( ; x <= len - 16; x += 16)
            {
                uint8x16_t v_src = vandq_u8(vld1q_u8(src + x), veorq_u8(v_255, vceqq_u8(vld1q_u8(mask + x), v_0)));
                uint8x8_t v_src_0 = vget_low_u8(v_src), v_src_1 = vget_high_u8(v_src);
                uint16x8_t v_src0 = vmull_u8(v_src_0, v_src_0), v_src1 = vmull_u8(v_src_1, v_src_1);

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src0)))));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src0)))));
                vst1q_f32(dst + x + 8, vaddq_f32(vld1q_f32(dst + x + 8), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src1)))));
                vst1q_f32(dst + x + 12, vaddq_f32(vld1q_f32(dst + x + 12), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src1)))));
            }
        }

        return x;
    }
};

template <>
struct AccSqr_SIMD<ushort, float>
{
    int operator() (const ushort * src, float * dst, const uchar * mask, int len, int cn) const
    {
        int x = 0;

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 8; x += 8)
            {
                uint16x8_t v_src = vld1q_u16(src + x);
                uint16x4_t v_src_0 = vget_low_u16(v_src), v_src_1 = vget_high_u16(v_src);
                uint32x4_t v_src0 = vmull_u16(v_src_0, v_src_0), v_src1 = vmull_u16(v_src_1, v_src_1);

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(v_src0)));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(v_src1)));
            }
        }
        else if (cn == 1)
        {
            uint8x8_t v_255 = vdup_n_u8(255), v_0 = vdup_n_u8(0);

            for ( ; x <= len - 8; x += 8)
            {
                uint8x8_t v_mask_src = veor_u8(v_255, vceq_u8(vld1_u8(mask + x), v_0));
                uint8x8x2_t v_mask_zp = vzip_u8(v_mask_src, v_mask_src);
                uint16x8_t v_mask = vreinterpretq_u16_u8(vcombine_u8(v_mask_zp.val[0], v_mask_zp.val[1])),
                           v_src = vandq_u16(vld1q_u16(src + x), v_mask);

                uint16x4_t v_src_0 = vget_low_u16(v_src), v_src_1 = vget_high_u16(v_src);
                uint32x4_t v_src0 = vmull_u16(v_src_0, v_src_0), v_src1 = vmull_u16(v_src_1, v_src_1);

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(v_src0)));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(v_src1)));
            }
        }

        return x;
    }
};

template <>
struct AccSqr_SIMD<float, float>
{
    int operator() (const float * src, float * dst, const uchar * mask, int len, int cn) const
    {
        int x = 0;

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 8; x += 8)
            {
                float32x4_t v_src = vld1q_f32(src + x);
                vst1q_f32(dst + x, vmlaq_f32(vld1q_f32(dst + x), v_src, v_src));

                v_src = vld1q_f32(src + x + 4);
                vst1q_f32(dst + x + 4, vmlaq_f32(vld1q_f32(dst + x + 4), v_src, v_src));
            }
        }

        return x;
    }
};

template <>
struct AccProd_SIMD<uchar, float>
{
    int operator() (const uchar * src1, const uchar * src2, float * dst, const uchar * mask, int len, int cn) const
    {
        int x = 0;

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 16; x += 16)
            {
                uint8x16_t v_1src = vld1q_u8(src1 + x), v_2src = vld1q_u8(src2 + x);
                uint16x8_t v_src0 = vmull_u8(vget_low_u8(v_1src), vget_low_u8(v_2src)),
                           v_src1 = vmull_u8(vget_high_u8(v_1src), vget_high_u8(v_2src));

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src0)))));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src0)))));
                vst1q_f32(dst + x + 8, vaddq_f32(vld1q_f32(dst + x + 8), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src1)))));
                vst1q_f32(dst + x + 12, vaddq_f32(vld1q_f32(dst + x + 12), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src1)))));
            }
        }
        else if (cn == 1)
        {
            uint8x16_t v_255 = vdupq_n_u8(255), v_0 = vdupq_n_u8(0);

            for ( ; x <= len - 16; x += 16)
            {
                uint8x16_t v_mask = veorq_u8(v_255, vceqq_u8(vld1q_u8(mask + x), v_0));
                uint8x16_t v_1src = vandq_u8(vld1q_u8(src1 + x), v_mask), v_2src = vandq_u8(vld1q_u8(src2 + x), v_mask);
                uint16x8_t v_src0 = vmull_u8(vget_low_u8(v_1src), vget_low_u8(v_2src)),
                           v_src1 = vmull_u8(vget_high_u8(v_1src), vget_high_u8(v_2src));

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src0)))));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src0)))));
                vst1q_f32(dst + x + 8, vaddq_f32(vld1q_f32(dst + x + 8), vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src1)))));
                vst1q_f32(dst + x + 12, vaddq_f32(vld1q_f32(dst + x + 12), vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src1)))));
            }
        }

        return x;
    }
};

template <>
struct AccProd_SIMD<ushort, float>
{
    int operator() (const ushort * src1, const ushort * src2, float * dst, const uchar * mask, int len, int cn) const
    {
        int x = 0;

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 8; x += 8)
            {
                uint16x8_t v_1src = vld1q_u16(src1 + x), v_2src = vld1q_u16(src2 + x);
                uint32x4_t v_src0 = vmull_u16(vget_low_u16(v_1src), vget_low_u16(v_2src)),
                           v_src1 = vmull_u16(vget_high_u16(v_1src), vget_high_u16(v_2src));

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(v_src0)));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(v_src1)));
            }
        }
        else if (cn == 1)
        {
            uint8x8_t v_255 = vdup_n_u8(255), v_0 = vdup_n_u8(0);

            for ( ; x <= len - 8; x += 8)
            {
                uint8x8_t v_mask_src = veor_u8(v_255, vceq_u8(vld1_u8(mask + x), v_0));
                uint8x8x2_t v_mask_zp = vzip_u8(v_mask_src, v_mask_src);
                uint16x8_t v_mask = vreinterpretq_u16_u8(vcombine_u8(v_mask_zp.val[0], v_mask_zp.val[1])),
                           v_1src = vandq_u16(vld1q_u16(src1 + x), v_mask),
                           v_2src = vandq_u16(vld1q_u16(src2 + x), v_mask);

                uint32x4_t v_src0 = vmull_u16(vget_low_u16(v_1src), vget_low_u16(v_2src)),
                           v_src1 = vmull_u16(vget_high_u16(v_1src), vget_high_u16(v_2src));

                vst1q_f32(dst + x, vaddq_f32(vld1q_f32(dst + x), vcvtq_f32_u32(v_src0)));
                vst1q_f32(dst + x + 4, vaddq_f32(vld1q_f32(dst + x + 4), vcvtq_f32_u32(v_src1)));
            }
        }

        return x;
    }
};

template <>
struct AccProd_SIMD<float, float>
{
    int operator() (const float * src1, const float * src2, float * dst, const uchar * mask, int len, int cn) const
    {
        int x = 0;

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 8; x += 8)
            {
                vst1q_f32(dst + x, vmlaq_f32(vld1q_f32(dst + x), vld1q_f32(src1 + x), vld1q_f32(src2 + x)));
                vst1q_f32(dst + x + 4, vmlaq_f32(vld1q_f32(dst + x + 4), vld1q_f32(src1 + x + 4), vld1q_f32(src2 + x + 4)));
            }
        }

        return x;
    }
};

template <>
struct AccW_SIMD<uchar, float>
{
    int operator() (const uchar * src, float * dst, const uchar * mask, int len, int cn, float alpha) const
    {
        int x = 0;
        float32x4_t v_alpha = vdupq_n_f32(alpha), v_beta = vdupq_n_f32(1.0f - alpha);

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 16; x += 16)
            {
                uint8x16_t v_src = vld1q_u8(src + x);
                uint16x8_t v_src0 = vmovl_u8(vget_low_u8(v_src)), v_src1 = vmovl_u8(vget_high_u8(v_src));

                vst1q_f32(dst + x, vmlaq_f32(vmulq_f32(vld1q_f32(dst + x), v_beta),
                                             vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src0))), v_alpha));
                vst1q_f32(dst + x + 4, vmlaq_f32(vmulq_f32(vld1q_f32(dst + x + 4), v_beta),
                                             vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src0))), v_alpha));
                vst1q_f32(dst + x + 8, vmlaq_f32(vmulq_f32(vld1q_f32(dst + x + 8), v_beta),
                                                 vcvtq_f32_u32(vmovl_u16(vget_low_u16(v_src1))), v_alpha));
                vst1q_f32(dst + x + 12, vmlaq_f32(vmulq_f32(vld1q_f32(dst + x + 12), v_beta),
                                                  vcvtq_f32_u32(vmovl_u16(vget_high_u16(v_src1))), v_alpha));
            }
        }

        return x;
    }
};

template <>
struct AccW_SIMD<ushort, float>
{
    int operator() (const ushort * src, float * dst, const uchar * mask, int len, int cn, float alpha) const
    {
        int x = 0;
        float32x4_t v_alpha = vdupq_n_f32(alpha), v_beta = vdupq_n_f32(1.0f - alpha);

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 8; x += 8)
            {
                uint16x8_t v_src = vld1q_u16(src + x);
                uint32x4_t v_src0 = vmovl_u16(vget_low_u16(v_src)), v_src1 = vmovl_u16(vget_high_u16(v_src));

                vst1q_f32(dst + x, vmlaq_f32(vmulq_f32(vld1q_f32(dst + x), v_beta), vcvtq_f32_u32(v_src0), v_alpha));
                vst1q_f32(dst + x + 4, vmlaq_f32(vmulq_f32(vld1q_f32(dst + x + 4), v_beta), vcvtq_f32_u32(v_src1), v_alpha));
            }
        }

        return x;
    }
};

template <>
struct AccW_SIMD<float, float>
{
    int operator() (const float * src, float * dst, const uchar * mask, int len, int cn, float alpha) const
    {
        int x = 0;
        float32x4_t v_alpha = vdupq_n_f32(alpha), v_beta = vdupq_n_f32(1.0f - alpha);

        if (!mask)
        {
            len *= cn;
            for ( ; x <= len - 8; x += 8)
            {
                vst1q_f32(dst + x, vmlaq_f32(vmulq_f32(vld1q_f32(dst + x), v_beta), vld1q_f32(src + x), v_alpha));
                vst1q_f32(dst + x + 4, vmlaq_f32(vmulq_f32(vld1q_f32(dst + x + 4), v_beta), vld1q_f32(src + x + 4), v_alpha));
            }
        }

        return x;
    }
};

#endif

template<typename T, typename AT> void
acc_( const T* src, AT* dst, const uchar* mask, int len, int cn )
{
    int i = Acc_SIMD<T, AT>()(src, dst, mask, len, cn);

    if( !mask )
    {
        len *= cn;
        #if CV_ENABLE_UNROLLED
        for( ; i <= len - 4; i += 4 )
        {
            AT t0, t1;
            t0 = src[i] + dst[i];
            t1 = src[i+1] + dst[i+1];
            dst[i] = t0; dst[i+1] = t1;

            t0 = src[i+2] + dst[i+2];
            t1 = src[i+3] + dst[i+3];
            dst[i+2] = t0; dst[i+3] = t1;
        }
        #endif
        for( ; i < len; i++ )
            dst[i] += src[i];
    }
    else if( cn == 1 )
    {
        for( ; i < len; i++ )
        {
            if( mask[i] )
                dst[i] += src[i];
        }
    }
    else if( cn == 3 )
    {
        for( ; i < len; i++, src += 3, dst += 3 )
        {
            if( mask[i] )
            {
                AT t0 = src[0] + dst[0];
                AT t1 = src[1] + dst[1];
                AT t2 = src[2] + dst[2];

                dst[0] = t0; dst[1] = t1; dst[2] = t2;
            }
        }
    }
    else
    {
        for( ; i < len; i++, src += cn, dst += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                    dst[k] += src[k];
            }
    }
}


template<typename T, typename AT> void
accSqr_( const T* src, AT* dst, const uchar* mask, int len, int cn )
{
    int i = AccSqr_SIMD<T, AT>()(src, dst, mask, len, cn);

    if( !mask )
    {
        len *= cn;
         #if CV_ENABLE_UNROLLED
        for( ; i <= len - 4; i += 4 )
        {
            AT t0, t1;
            t0 = (AT)src[i]*src[i] + dst[i];
            t1 = (AT)src[i+1]*src[i+1] + dst[i+1];
            dst[i] = t0; dst[i+1] = t1;

            t0 = (AT)src[i+2]*src[i+2] + dst[i+2];
            t1 = (AT)src[i+3]*src[i+3] + dst[i+3];
            dst[i+2] = t0; dst[i+3] = t1;
        }
        #endif
        for( ; i < len; i++ )
            dst[i] += (AT)src[i]*src[i];
    }
    else if( cn == 1 )
    {
        for( ; i < len; i++ )
        {
            if( mask[i] )
                dst[i] += (AT)src[i]*src[i];
        }
    }
    else if( cn == 3 )
    {
        for( ; i < len; i++, src += 3, dst += 3 )
        {
            if( mask[i] )
            {
                AT t0 = (AT)src[0]*src[0] + dst[0];
                AT t1 = (AT)src[1]*src[1] + dst[1];
                AT t2 = (AT)src[2]*src[2] + dst[2];

                dst[0] = t0; dst[1] = t1; dst[2] = t2;
            }
        }
    }
    else
    {
        for( ; i < len; i++, src += cn, dst += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                    dst[k] += (AT)src[k]*src[k];
            }
    }
}


template<typename T, typename AT> void
accProd_( const T* src1, const T* src2, AT* dst, const uchar* mask, int len, int cn )
{
    int i = AccProd_SIMD<T, AT>()(src1, src2, dst, mask, len, cn);

    if( !mask )
    {
        len *= cn;
        #if CV_ENABLE_UNROLLED
        for( ; i <= len - 4; i += 4 )
        {
            AT t0, t1;
            t0 = (AT)src1[i]*src2[i] + dst[i];
            t1 = (AT)src1[i+1]*src2[i+1] + dst[i+1];
            dst[i] = t0; dst[i+1] = t1;

            t0 = (AT)src1[i+2]*src2[i+2] + dst[i+2];
            t1 = (AT)src1[i+3]*src2[i+3] + dst[i+3];
            dst[i+2] = t0; dst[i+3] = t1;
        }
        #endif
        for( ; i < len; i++ )
            dst[i] += (AT)src1[i]*src2[i];
    }
    else if( cn == 1 )
    {
        for( ; i < len; i++ )
        {
            if( mask[i] )
                dst[i] += (AT)src1[i]*src2[i];
        }
    }
    else if( cn == 3 )
    {
        for( ; i < len; i++, src1 += 3, src2 += 3, dst += 3 )
        {
            if( mask[i] )
            {
                AT t0 = (AT)src1[0]*src2[0] + dst[0];
                AT t1 = (AT)src1[1]*src2[1] + dst[1];
                AT t2 = (AT)src1[2]*src2[2] + dst[2];

                dst[0] = t0; dst[1] = t1; dst[2] = t2;
            }
        }
    }
    else
    {
        for( ; i < len; i++, src1 += cn, src2 += cn, dst += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                    dst[k] += (AT)src1[k]*src2[k];
            }
    }
}


template<typename T, typename AT> void
accW_( const T* src, AT* dst, const uchar* mask, int len, int cn, double alpha )
{
    AT a = (AT)alpha, b = 1 - a;
    int i = AccW_SIMD<T, AT>()(src, dst, mask, len, cn, a);

    if( !mask )
    {
        len *= cn;
        #if CV_ENABLE_UNROLLED
        for( ; i <= len - 4; i += 4 )
        {
            AT t0, t1;
            t0 = src[i]*a + dst[i]*b;
            t1 = src[i+1]*a + dst[i+1]*b;
            dst[i] = t0; dst[i+1] = t1;

            t0 = src[i+2]*a + dst[i+2]*b;
            t1 = src[i+3]*a + dst[i+3]*b;
            dst[i+2] = t0; dst[i+3] = t1;
        }
        #endif
        for( ; i < len; i++ )
            dst[i] = src[i]*a + dst[i]*b;
    }
    else if( cn == 1 )
    {
        for( ; i < len; i++ )
        {
            if( mask[i] )
                dst[i] = src[i]*a + dst[i]*b;
        }
    }
    else if( cn == 3 )
    {
        for( ; i < len; i++, src += 3, dst += 3 )
        {
            if( mask[i] )
            {
                AT t0 = src[0]*a + dst[0]*b;
                AT t1 = src[1]*a + dst[1]*b;
                AT t2 = src[2]*a + dst[2]*b;

                dst[0] = t0; dst[1] = t1; dst[2] = t2;
            }
        }
    }
    else
    {
        for( ; i < len; i++, src += cn, dst += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                    dst[k] = src[k]*a + dst[k]*b;
            }
    }
}


#define DEF_ACC_FUNCS(suffix, type, acctype) \
static void acc_##suffix(const type* src, acctype* dst, \
                         const uchar* mask, int len, int cn) \
{ acc_(src, dst, mask, len, cn); } \
\
static void accSqr_##suffix(const type* src, acctype* dst, \
                            const uchar* mask, int len, int cn) \
{ accSqr_(src, dst, mask, len, cn); } \
\
static void accProd_##suffix(const type* src1, const type* src2, \
                             acctype* dst, const uchar* mask, int len, int cn) \
{ accProd_(src1, src2, dst, mask, len, cn); } \
\
static void accW_##suffix(const type* src, acctype* dst, \
                          const uchar* mask, int len, int cn, double alpha) \
{ accW_(src, dst, mask, len, cn, alpha); }


DEF_ACC_FUNCS(8u32f, uchar, float)
DEF_ACC_FUNCS(8u64f, uchar, double)
DEF_ACC_FUNCS(16u32f, ushort, float)
DEF_ACC_FUNCS(16u64f, ushort, double)
DEF_ACC_FUNCS(32f, float, float)
DEF_ACC_FUNCS(32f64f, float, double)
DEF_ACC_FUNCS(64f, double, double)


typedef void (*AccFunc)(const uchar*, uchar*, const uchar*, int, int);
typedef void (*AccProdFunc)(const uchar*, const uchar*, uchar*, const uchar*, int, int);
typedef void (*AccWFunc)(const uchar*, uchar*, const uchar*, int, int, double);

static AccFunc accTab[] =
{
    (AccFunc)acc_8u32f, (AccFunc)acc_8u64f,
    (AccFunc)acc_16u32f, (AccFunc)acc_16u64f,
    (AccFunc)acc_32f, (AccFunc)acc_32f64f,
    (AccFunc)acc_64f
};

static AccFunc accSqrTab[] =
{
    (AccFunc)accSqr_8u32f, (AccFunc)accSqr_8u64f,
    (AccFunc)accSqr_16u32f, (AccFunc)accSqr_16u64f,
    (AccFunc)accSqr_32f, (AccFunc)accSqr_32f64f,
    (AccFunc)accSqr_64f
};

static AccProdFunc accProdTab[] =
{
    (AccProdFunc)accProd_8u32f, (AccProdFunc)accProd_8u64f,
    (AccProdFunc)accProd_16u32f, (AccProdFunc)accProd_16u64f,
    (AccProdFunc)accProd_32f, (AccProdFunc)accProd_32f64f,
    (AccProdFunc)accProd_64f
};

static AccWFunc accWTab[] =
{
    (AccWFunc)accW_8u32f, (AccWFunc)accW_8u64f,
    (AccWFunc)accW_16u32f, (AccWFunc)accW_16u64f,
    (AccWFunc)accW_32f, (AccWFunc)accW_32f64f,
    (AccWFunc)accW_64f
};

inline int getAccTabIdx(int sdepth, int ddepth)
{
    return sdepth == CV_8U && ddepth == CV_32F ? 0 :
           sdepth == CV_8U && ddepth == CV_64F ? 1 :
           sdepth == CV_16U && ddepth == CV_32F ? 2 :
           sdepth == CV_16U && ddepth == CV_64F ? 3 :
           sdepth == CV_32F && ddepth == CV_32F ? 4 :
           sdepth == CV_32F && ddepth == CV_64F ? 5 :
           sdepth == CV_64F && ddepth == CV_64F ? 6 : -1;
}

#ifdef HAVE_OPENCL

enum
{
    ACCUMULATE = 0,
    ACCUMULATE_SQUARE = 1,
    ACCUMULATE_PRODUCT = 2,
    ACCUMULATE_WEIGHTED = 3
};

static bool ocl_accumulate( InputArray _src, InputArray _src2, InputOutputArray _dst, double alpha,
                            InputArray _mask, int op_type )
{
    CV_Assert(op_type == ACCUMULATE || op_type == ACCUMULATE_SQUARE ||
              op_type == ACCUMULATE_PRODUCT || op_type == ACCUMULATE_WEIGHTED);

    const ocl::Device & dev = ocl::Device::getDefault();
    bool haveMask = !_mask.empty(), doubleSupport = dev.doubleFPConfig() > 0;
    int stype = _src.type(), sdepth = CV_MAT_DEPTH(stype), cn = CV_MAT_CN(stype), ddepth = _dst.depth();
    int kercn = haveMask ? cn : ocl::predictOptimalVectorWidthMax(_src, _src2, _dst), rowsPerWI = dev.isIntel() ? 4 : 1;

    if (!doubleSupport && (sdepth == CV_64F || ddepth == CV_64F))
        return false;

    const char * const opMap[4] = { "ACCUMULATE", "ACCUMULATE_SQUARE", "ACCUMULATE_PRODUCT",
                                   "ACCUMULATE_WEIGHTED" };

    char cvt[40];
    ocl::Kernel k("accumulate", ocl::imgproc::accumulate_oclsrc,
                  format("-D %s%s -D srcT1=%s -D cn=%d -D dstT1=%s%s -D rowsPerWI=%d -D convertToDT=%s",
                         opMap[op_type], haveMask ? " -D HAVE_MASK" : "",
                         ocl::typeToStr(sdepth), kercn, ocl::typeToStr(ddepth),
                         doubleSupport ? " -D DOUBLE_SUPPORT" : "", rowsPerWI,
                         ocl::convertTypeStr(sdepth, ddepth, 1, cvt)));
    if (k.empty())
        return false;

    UMat src = _src.getUMat(), src2 = _src2.getUMat(), dst = _dst.getUMat(), mask = _mask.getUMat();

    ocl::KernelArg srcarg = ocl::KernelArg::ReadOnlyNoSize(src),
            src2arg = ocl::KernelArg::ReadOnlyNoSize(src2),
            dstarg = ocl::KernelArg::ReadWrite(dst, cn, kercn),
            maskarg = ocl::KernelArg::ReadOnlyNoSize(mask);

    int argidx = k.set(0, srcarg);
    if (op_type == ACCUMULATE_PRODUCT)
        argidx = k.set(argidx, src2arg);
    argidx = k.set(argidx, dstarg);
    if (op_type == ACCUMULATE_WEIGHTED)
    {
        if (ddepth == CV_32F)
            argidx = k.set(argidx, (float)alpha);
        else
            argidx = k.set(argidx, alpha);
    }
    if (haveMask)
        k.set(argidx, maskarg);

    size_t globalsize[2] = { (size_t)src.cols * cn / kercn, ((size_t)src.rows + rowsPerWI - 1) / rowsPerWI };
    return k.run(2, globalsize, NULL, false);
}

#endif

}

#if defined(HAVE_IPP)
namespace cv
{
static bool ipp_accumulate(InputArray _src, InputOutputArray _dst, InputArray _mask)
{
    int stype = _src.type(), sdepth = CV_MAT_DEPTH(stype), scn = CV_MAT_CN(stype);
    int dtype = _dst.type(), ddepth = CV_MAT_DEPTH(dtype);

    Mat src = _src.getMat(), dst = _dst.getMat(), mask = _mask.getMat();

    if (src.dims <= 2 || (src.isContinuous() && dst.isContinuous() && (mask.empty() || mask.isContinuous())))
    {
        typedef IppStatus (CV_STDCALL * ippiAdd)(const void * pSrc, int srcStep, Ipp32f * pSrcDst, int srcdstStep, IppiSize roiSize);
        typedef IppStatus (CV_STDCALL * ippiAddMask)(const void * pSrc, int srcStep, const Ipp8u * pMask, int maskStep, Ipp32f * pSrcDst,
                                                    int srcDstStep, IppiSize roiSize);
        ippiAdd ippFunc = 0;
        ippiAddMask ippFuncMask = 0;

        if (mask.empty())
        {
            CV_SUPPRESS_DEPRECATED_START
            ippFunc = sdepth == CV_8U && ddepth == CV_32F ? (ippiAdd)ippiAdd_8u32f_C1IR :
                sdepth == CV_16U && ddepth == CV_32F ? (ippiAdd)ippiAdd_16u32f_C1IR :
                sdepth == CV_32F && ddepth == CV_32F ? (ippiAdd)ippiAdd_32f_C1IR : 0;
            CV_SUPPRESS_DEPRECATED_END
        }
        else if (scn == 1)
        {
            ippFuncMask = sdepth == CV_8U && ddepth == CV_32F ? (ippiAddMask)ippiAdd_8u32f_C1IMR :
                sdepth == CV_16U && ddepth == CV_32F ? (ippiAddMask)ippiAdd_16u32f_C1IMR :
                sdepth == CV_32F && ddepth == CV_32F ? (ippiAddMask)ippiAdd_32f_C1IMR : 0;
        }

        if (ippFunc || ippFuncMask)
        {
            IppStatus status = ippStsErr;

            Size size = src.size();
            int srcstep = (int)src.step, dststep = (int)dst.step, maskstep = (int)mask.step;
            if (src.isContinuous() && dst.isContinuous() && mask.isContinuous())
            {
                srcstep = static_cast<int>(src.total() * src.elemSize());
                dststep = static_cast<int>(dst.total() * dst.elemSize());
                maskstep = static_cast<int>(mask.total() * mask.elemSize());
                size.width = static_cast<int>(src.total());
                size.height = 1;
            }
            size.width *= scn;

            if (ippFunc)
                status = ippFunc(src.ptr(), srcstep, dst.ptr<Ipp32f>(), dststep, ippiSize(size.width, size.height));
            else if(ippFuncMask)
                status = ippFuncMask(src.ptr(), srcstep, mask.ptr<Ipp8u>(), maskstep,
                                        dst.ptr<Ipp32f>(), dststep, ippiSize(size.width, size.height));

            if (status >= 0)
                return true;
        }
    }
    return false;
}
}
#endif

void cv::accumulate( InputArray _src, InputOutputArray _dst, InputArray _mask )
{
    int stype = _src.type(), sdepth = CV_MAT_DEPTH(stype), scn = CV_MAT_CN(stype);
    int dtype = _dst.type(), ddepth = CV_MAT_DEPTH(dtype), dcn = CV_MAT_CN(dtype);

    CV_Assert( _src.sameSize(_dst) && dcn == scn );
    CV_Assert( _mask.empty() || (_src.sameSize(_mask) && _mask.type() == CV_8U) );

    CV_OCL_RUN(_src.dims() <= 2 && _dst.isUMat(),
               ocl_accumulate(_src, noArray(), _dst, 0.0, _mask, ACCUMULATE))

    CV_IPP_RUN((_src.dims() <= 2 || (_src.isContinuous() && _dst.isContinuous() && (_mask.empty() || _mask.isContinuous()))),
        ipp_accumulate(_src, _dst, _mask));

    Mat src = _src.getMat(), dst = _dst.getMat(), mask = _mask.getMat();


    int fidx = getAccTabIdx(sdepth, ddepth);
    AccFunc func = fidx >= 0 ? accTab[fidx] : 0;
    CV_Assert( func != 0 );

    const Mat* arrays[] = {&src, &dst, &mask, 0};
    uchar* ptrs[3];
    NAryMatIterator it(arrays, ptrs);
    int len = (int)it.size;

    for( size_t i = 0; i < it.nplanes; i++, ++it )
        func(ptrs[0], ptrs[1], ptrs[2], len, scn);
}

#if defined(HAVE_IPP)
namespace cv
{
static bool ipp_accumulate_square(InputArray _src, InputOutputArray _dst, InputArray _mask)
{
    int stype = _src.type(), sdepth = CV_MAT_DEPTH(stype), scn = CV_MAT_CN(stype);
    int dtype = _dst.type(), ddepth = CV_MAT_DEPTH(dtype);

    Mat src = _src.getMat(), dst = _dst.getMat(), mask = _mask.getMat();

    if (src.dims <= 2 || (src.isContinuous() && dst.isContinuous() && (mask.empty() || mask.isContinuous())))
    {
        typedef IppStatus (CV_STDCALL * ippiAddSquare)(const void * pSrc, int srcStep, Ipp32f * pSrcDst, int srcdstStep, IppiSize roiSize);
        typedef IppStatus (CV_STDCALL * ippiAddSquareMask)(const void * pSrc, int srcStep, const Ipp8u * pMask, int maskStep, Ipp32f * pSrcDst,
                                                            int srcDstStep, IppiSize roiSize);
        ippiAddSquare ippFunc = 0;
        ippiAddSquareMask ippFuncMask = 0;

        if (mask.empty())
        {
            ippFunc = sdepth == CV_8U && ddepth == CV_32F ? (ippiAddSquare)ippiAddSquare_8u32f_C1IR :
                sdepth == CV_16U && ddepth == CV_32F ? (ippiAddSquare)ippiAddSquare_16u32f_C1IR :
                sdepth == CV_32F && ddepth == CV_32F ? (ippiAddSquare)ippiAddSquare_32f_C1IR : 0;
        }
        else if (scn == 1)
        {
            ippFuncMask = sdepth == CV_8U && ddepth == CV_32F ? (ippiAddSquareMask)ippiAddSquare_8u32f_C1IMR :
                sdepth == CV_16U && ddepth == CV_32F ? (ippiAddSquareMask)ippiAddSquare_16u32f_C1IMR :
                sdepth == CV_32F && ddepth == CV_32F ? (ippiAddSquareMask)ippiAddSquare_32f_C1IMR : 0;
        }

        if (ippFunc || ippFuncMask)
        {
            IppStatus status = ippStsErr;

            Size size = src.size();
            int srcstep = (int)src.step, dststep = (int)dst.step, maskstep = (int)mask.step;
            if (src.isContinuous() && dst.isContinuous() && mask.isContinuous())
            {
                srcstep = static_cast<int>(src.total() * src.elemSize());
                dststep = static_cast<int>(dst.total() * dst.elemSize());
                maskstep = static_cast<int>(mask.total() * mask.elemSize());
                size.width = static_cast<int>(src.total());
                size.height = 1;
            }
            size.width *= scn;

            if (ippFunc)
                status = ippFunc(src.ptr(), srcstep, dst.ptr<Ipp32f>(), dststep, ippiSize(size.width, size.height));
            else if(ippFuncMask)
                status = ippFuncMask(src.ptr(), srcstep, mask.ptr<Ipp8u>(), maskstep,
                                        dst.ptr<Ipp32f>(), dststep, ippiSize(size.width, size.height));

            if (status >= 0)
                return true;
        }
    }
    return false;
}
}
#endif

void cv::accumulateSquare( InputArray _src, InputOutputArray _dst, InputArray _mask )
{
    int stype = _src.type(), sdepth = CV_MAT_DEPTH(stype), scn = CV_MAT_CN(stype);
    int dtype = _dst.type(), ddepth = CV_MAT_DEPTH(dtype), dcn = CV_MAT_CN(dtype);

    CV_Assert( _src.sameSize(_dst) && dcn == scn );
    CV_Assert( _mask.empty() || (_src.sameSize(_mask) && _mask.type() == CV_8U) );

    CV_OCL_RUN(_src.dims() <= 2 && _dst.isUMat(),
               ocl_accumulate(_src, noArray(), _dst, 0.0, _mask, ACCUMULATE_SQUARE))

    CV_IPP_RUN((_src.dims() <= 2 || (_src.isContinuous() && _dst.isContinuous() && (_mask.empty() || _mask.isContinuous()))),
        ipp_accumulate_square(_src, _dst, _mask));

    Mat src = _src.getMat(), dst = _dst.getMat(), mask = _mask.getMat();

    int fidx = getAccTabIdx(sdepth, ddepth);
    AccFunc func = fidx >= 0 ? accSqrTab[fidx] : 0;
    CV_Assert( func != 0 );

    const Mat* arrays[] = {&src, &dst, &mask, 0};
    uchar* ptrs[3];
    NAryMatIterator it(arrays, ptrs);
    int len = (int)it.size;

    for( size_t i = 0; i < it.nplanes; i++, ++it )
        func(ptrs[0], ptrs[1], ptrs[2], len, scn);
}

#if defined(HAVE_IPP)
namespace cv
{
static bool ipp_accumulate_product(InputArray _src1, InputArray _src2,
                            InputOutputArray _dst, InputArray _mask)
{
    int stype = _src1.type(), sdepth = CV_MAT_DEPTH(stype), scn = CV_MAT_CN(stype);
    int dtype = _dst.type(), ddepth = CV_MAT_DEPTH(dtype);

    Mat src1 = _src1.getMat(), src2 = _src2.getMat(), dst = _dst.getMat(), mask = _mask.getMat();

    if (src1.dims <= 2 || (src1.isContinuous() && src2.isContinuous() && dst.isContinuous()))
    {
        typedef IppStatus (CV_STDCALL * ippiAddProduct)(const void * pSrc1, int src1Step, const void * pSrc2,
                                                        int src2Step, Ipp32f * pSrcDst, int srcDstStep, IppiSize roiSize);
        typedef IppStatus (CV_STDCALL * ippiAddProductMask)(const void * pSrc1, int src1Step, const void * pSrc2, int src2Step,
                                                            const Ipp8u * pMask, int maskStep, Ipp32f * pSrcDst, int srcDstStep, IppiSize roiSize);
        ippiAddProduct ippFunc = 0;
        ippiAddProductMask ippFuncMask = 0;

        if (mask.empty())
        {
            ippFunc = sdepth == CV_8U && ddepth == CV_32F ? (ippiAddProduct)ippiAddProduct_8u32f_C1IR :
                sdepth == CV_16U && ddepth == CV_32F ? (ippiAddProduct)ippiAddProduct_16u32f_C1IR :
                sdepth == CV_32F && ddepth == CV_32F ? (ippiAddProduct)ippiAddProduct_32f_C1IR : 0;
        }
        else if (scn == 1)
        {
            ippFuncMask = sdepth == CV_8U && ddepth == CV_32F ? (ippiAddProductMask)ippiAddProduct_8u32f_C1IMR :
                sdepth == CV_16U && ddepth == CV_32F ? (ippiAddProductMask)ippiAddProduct_16u32f_C1IMR :
                sdepth == CV_32F && ddepth == CV_32F ? (ippiAddProductMask)ippiAddProduct_32f_C1IMR : 0;
        }

        if (ippFunc || ippFuncMask)
        {
            IppStatus status = ippStsErr;

            Size size = src1.size();
            int src1step = (int)src1.step, src2step = (int)src2.step, dststep = (int)dst.step, maskstep = (int)mask.step;
            if (src1.isContinuous() && src2.isContinuous() && dst.isContinuous() && mask.isContinuous())
            {
                src1step = static_cast<int>(src1.total() * src1.elemSize());
                src2step = static_cast<int>(src2.total() * src2.elemSize());
                dststep = static_cast<int>(dst.total() * dst.elemSize());
                maskstep = static_cast<int>(mask.total() * mask.elemSize());
                size.width = static_cast<int>(src1.total());
                size.height = 1;
            }
            size.width *= scn;

            if (ippFunc)
                status = ippFunc(src1.ptr(), src1step, src2.ptr(), src2step, dst.ptr<Ipp32f>(),
                                    dststep, ippiSize(size.width, size.height));
            else if(ippFuncMask)
                status = ippFuncMask(src1.ptr(), src1step, src2.ptr(), src2step, mask.ptr<Ipp8u>(), maskstep,
                                        dst.ptr<Ipp32f>(), dststep, ippiSize(size.width, size.height));

            if (status >= 0)
                return true;
        }
    }
    return false;
}
}
#endif



void cv::accumulateProduct( InputArray _src1, InputArray _src2,
                            InputOutputArray _dst, InputArray _mask )
{
    int stype = _src1.type(), sdepth = CV_MAT_DEPTH(stype), scn = CV_MAT_CN(stype);
    int dtype = _dst.type(), ddepth = CV_MAT_DEPTH(dtype), dcn = CV_MAT_CN(dtype);

    CV_Assert( _src1.sameSize(_src2) && stype == _src2.type() );
    CV_Assert( _src1.sameSize(_dst) && dcn == scn );
    CV_Assert( _mask.empty() || (_src1.sameSize(_mask) && _mask.type() == CV_8U) );

    CV_OCL_RUN(_src1.dims() <= 2 && _dst.isUMat(),
               ocl_accumulate(_src1, _src2, _dst, 0.0, _mask, ACCUMULATE_PRODUCT))

    CV_IPP_RUN( (_src1.dims() <= 2 || (_src1.isContinuous() && _src2.isContinuous() && _dst.isContinuous())),
        ipp_accumulate_product(_src1, _src2, _dst, _mask));

    Mat src1 = _src1.getMat(), src2 = _src2.getMat(), dst = _dst.getMat(), mask = _mask.getMat();

    int fidx = getAccTabIdx(sdepth, ddepth);
    AccProdFunc func = fidx >= 0 ? accProdTab[fidx] : 0;
    CV_Assert( func != 0 );

    const Mat* arrays[] = {&src1, &src2, &dst, &mask, 0};
    uchar* ptrs[4];
    NAryMatIterator it(arrays, ptrs);
    int len = (int)it.size;

    for( size_t i = 0; i < it.nplanes; i++, ++it )
        func(ptrs[0], ptrs[1], ptrs[2], ptrs[3], len, scn);
}

#if defined(HAVE_IPP)
namespace cv
{
static bool ipp_accumulate_weighted( InputArray _src, InputOutputArray _dst,
                             double alpha, InputArray _mask )
{
    int stype = _src.type(), sdepth = CV_MAT_DEPTH(stype), scn = CV_MAT_CN(stype);
    int dtype = _dst.type(), ddepth = CV_MAT_DEPTH(dtype);

    Mat src = _src.getMat(), dst = _dst.getMat(), mask = _mask.getMat();

    if (src.dims <= 2 || (src.isContinuous() && dst.isContinuous() && mask.isContinuous()))
    {
        typedef IppStatus (CV_STDCALL * ippiAddWeighted)(const void * pSrc, int srcStep, Ipp32f * pSrcDst, int srcdstStep,
                                                            IppiSize roiSize, Ipp32f alpha);
        typedef IppStatus (CV_STDCALL * ippiAddWeightedMask)(const void * pSrc, int srcStep, const Ipp8u * pMask,
                                                                int maskStep, Ipp32f * pSrcDst,
                                                                int srcDstStep, IppiSize roiSize, Ipp32f alpha);
        ippiAddWeighted ippFunc = 0;
        ippiAddWeightedMask ippFuncMask = 0;

        if (mask.empty())
        {
            ippFunc = sdepth == CV_8U && ddepth == CV_32F ? (ippiAddWeighted)ippiAddWeighted_8u32f_C1IR :
                sdepth == CV_16U && ddepth == CV_32F ? (ippiAddWeighted)ippiAddWeighted_16u32f_C1IR :
                sdepth == CV_32F && ddepth == CV_32F ? (ippiAddWeighted)ippiAddWeighted_32f_C1IR : 0;
        }
        else if (scn == 1)
        {
            ippFuncMask = sdepth == CV_8U && ddepth == CV_32F ? (ippiAddWeightedMask)ippiAddWeighted_8u32f_C1IMR :
                sdepth == CV_16U && ddepth == CV_32F ? (ippiAddWeightedMask)ippiAddWeighted_16u32f_C1IMR :
                sdepth == CV_32F && ddepth == CV_32F ? (ippiAddWeightedMask)ippiAddWeighted_32f_C1IMR : 0;
        }

        if (ippFunc || ippFuncMask)
        {
            IppStatus status = ippStsErr;

            Size size = src.size();
            int srcstep = (int)src.step, dststep = (int)dst.step, maskstep = (int)mask.step;
            if (src.isContinuous() && dst.isContinuous() && mask.isContinuous())
            {
                srcstep = static_cast<int>(src.total() * src.elemSize());
                dststep = static_cast<int>(dst.total() * dst.elemSize());
                maskstep = static_cast<int>(mask.total() * mask.elemSize());
                size.width = static_cast<int>((int)src.total());
                size.height = 1;
            }
            size.width *= scn;

            if (ippFunc)
                status = ippFunc(src.ptr(), srcstep, dst.ptr<Ipp32f>(), dststep, ippiSize(size.width, size.height), (Ipp32f)alpha);
            else if(ippFuncMask)
                status = ippFuncMask(src.ptr(), srcstep, mask.ptr<Ipp8u>(), maskstep,
                                        dst.ptr<Ipp32f>(), dststep, ippiSize(size.width, size.height), (Ipp32f)alpha);

            if (status >= 0)
                return true;
        }
    }
    return false;
}
}
#endif

void cv::accumulateWeighted( InputArray _src, InputOutputArray _dst,
                             double alpha, InputArray _mask )
{
    int stype = _src.type(), sdepth = CV_MAT_DEPTH(stype), scn = CV_MAT_CN(stype);
    int dtype = _dst.type(), ddepth = CV_MAT_DEPTH(dtype), dcn = CV_MAT_CN(dtype);

    CV_Assert( _src.sameSize(_dst) && dcn == scn );
    CV_Assert( _mask.empty() || (_src.sameSize(_mask) && _mask.type() == CV_8U) );

    CV_OCL_RUN(_src.dims() <= 2 && _dst.isUMat(),
               ocl_accumulate(_src, noArray(), _dst, alpha, _mask, ACCUMULATE_WEIGHTED))

    CV_IPP_RUN((_src.dims() <= 2 || (_src.isContinuous() && _dst.isContinuous() && _mask.isContinuous())), ipp_accumulate_weighted(_src, _dst, alpha, _mask));


    Mat src = _src.getMat(), dst = _dst.getMat(), mask = _mask.getMat();


    int fidx = getAccTabIdx(sdepth, ddepth);
    AccWFunc func = fidx >= 0 ? accWTab[fidx] : 0;
    CV_Assert( func != 0 );

    const Mat* arrays[] = {&src, &dst, &mask, 0};
    uchar* ptrs[3];
    NAryMatIterator it(arrays, ptrs);
    int len = (int)it.size;

    for( size_t i = 0; i < it.nplanes; i++, ++it )
        func(ptrs[0], ptrs[1], ptrs[2], len, scn, alpha);
}


CV_IMPL void
cvAcc( const void* arr, void* sumarr, const void* maskarr )
{
    cv::Mat src = cv::cvarrToMat(arr), dst = cv::cvarrToMat(sumarr), mask;
    if( maskarr )
        mask = cv::cvarrToMat(maskarr);
    cv::accumulate( src, dst, mask );
}

CV_IMPL void
cvSquareAcc( const void* arr, void* sumarr, const void* maskarr )
{
    cv::Mat src = cv::cvarrToMat(arr), dst = cv::cvarrToMat(sumarr), mask;
    if( maskarr )
        mask = cv::cvarrToMat(maskarr);
    cv::accumulateSquare( src, dst, mask );
}

CV_IMPL void
cvMultiplyAcc( const void* arr1, const void* arr2,
               void* sumarr, const void* maskarr )
{
    cv::Mat src1 = cv::cvarrToMat(arr1), src2 = cv::cvarrToMat(arr2);
    cv::Mat dst = cv::cvarrToMat(sumarr), mask;
    if( maskarr )
        mask = cv::cvarrToMat(maskarr);
    cv::accumulateProduct( src1, src2, dst, mask );
}

CV_IMPL void
cvRunningAvg( const void* arr, void* sumarr, double alpha, const void* maskarr )
{
    cv::Mat src = cv::cvarrToMat(arr), dst = cv::cvarrToMat(sumarr), mask;
    if( maskarr )
        mask = cv::cvarrToMat(maskarr);
    cv::accumulateWeighted( src, dst, alpha, mask );
}

/* End of file. */
