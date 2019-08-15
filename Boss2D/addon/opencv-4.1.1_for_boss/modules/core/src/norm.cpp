// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html


#include "precomp.hpp"
#include "opencl_kernels_core.hpp"
#include "stat.hpp"

/****************************************************************************************\
*                                         norm                                           *
\****************************************************************************************/

namespace cv { namespace hal {

extern const uchar popCountTable[256] =
{
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

static const uchar popCountTable2[] =
{
    0, 1, 1, 1, 1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
    1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
    1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
    2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
    1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
    2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
    1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4,
    2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4
};

static const uchar popCountTable4[] =
{
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};


int normHamming(const uchar* a, int n, int cellSize)
{
    if( cellSize == 1 )
        return normHamming(a, n);
    const uchar* tab = 0;
    if( cellSize == 2 )
        tab = popCountTable2;
    else if( cellSize == 4 )
        tab = popCountTable4;
    else
        return -1;
    int i = 0;
    int result = 0;
#if CV_ENABLE_UNROLLED
    for( ; i <= n - 4; i += 4 )
        result += tab[a[i]] + tab[a[i+1]] + tab[a[i+2]] + tab[a[i+3]];
#endif
    for( ; i < n; i++ )
        result += tab[a[i]];
    return result;
}

int normHamming(const uchar* a, const uchar* b, int n, int cellSize)
{
    if( cellSize == 1 )
        return normHamming(a, b, n);
    const uchar* tab = 0;
    if( cellSize == 2 )
        tab = popCountTable2;
    else if( cellSize == 4 )
        tab = popCountTable4;
    else
        return -1;
    int i = 0;
    int result = 0;
#if CV_ENABLE_UNROLLED
    for( ; i <= n - 4; i += 4 )
        result += tab[a[i] ^ b[i]] + tab[a[i+1] ^ b[i+1]] +
                tab[a[i+2] ^ b[i+2]] + tab[a[i+3] ^ b[i+3]];
#endif
    for( ; i < n; i++ )
        result += tab[a[i] ^ b[i]];
    return result;
}

float normL2Sqr_(const float* a, const float* b, int n)
{
    int j = 0; float d = 0.f;
#if CV_SIMD
    v_float32 v_d = vx_setzero_f32();
    for (; j <= n - v_float32::nlanes; j += v_float32::nlanes)
    {
        v_float32 t = vx_load(a + j) - vx_load(b + j);
        v_d = v_muladd(t, t, v_d);
    }
    d = v_reduce_sum(v_d);
#endif
    for( ; j < n; j++ )
    {
        float t = a[j] - b[j];
        d += t*t;
    }
    return d;
}


float normL1_(const float* a, const float* b, int n)
{
    int j = 0; float d = 0.f;
#if CV_SIMD
    v_float32 v_d = vx_setzero_f32();
    for (; j <= n - v_float32::nlanes; j += v_float32::nlanes)
        v_d += v_absdiff(vx_load(a + j), vx_load(b + j));
    d = v_reduce_sum(v_d);
#endif
    for( ; j < n; j++ )
        d += std::abs(a[j] - b[j]);
    return d;
}

int normL1_(const uchar* a, const uchar* b, int n)
{
    int j = 0, d = 0;
#if CV_SIMD
    for (; j <= n - v_uint8::nlanes; j += v_uint8::nlanes)
        d += v_reduce_sad(vx_load(a + j), vx_load(b + j));
#endif
    for( ; j < n; j++ )
        d += std::abs(a[j] - b[j]);
    return d;
}

}} //cv::hal

//==================================================================================================

namespace cv
{

template<typename T, typename ST> int
normInf_(const T* src, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if( !mask )
    {
        result = std::max(result, normInf<T, ST>(src, len*cn));
    }
    else
    {
        for( int i = 0; i < len; i++, src += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                    result = std::max(result, ST(cv_abs(src[k])));
            }
    }
    *_result = result;
    return 0;
}

template<typename T, typename ST> int
normL1_(const T* src, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if( !mask )
    {
        result += normL1<T, ST>(src, len*cn);
    }
    else
    {
        for( int i = 0; i < len; i++, src += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                    result += cv_abs(src[k]);
            }
    }
    *_result = result;
    return 0;
}

template<typename T, typename ST> int
normL2_(const T* src, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if( !mask )
    {
        result += normL2Sqr<T, ST>(src, len*cn);
    }
    else
    {
        for( int i = 0; i < len; i++, src += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                {
                    T v = src[k];
                    result += (ST)v*v;
                }
            }
    }
    *_result = result;
    return 0;
}

template<typename T, typename ST> int
normDiffInf_(const T* src1, const T* src2, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if( !mask )
    {
        result = std::max(result, normInf<T, ST>(src1, src2, len*cn));
    }
    else
    {
        for( int i = 0; i < len; i++, src1 += cn, src2 += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                    result = std::max(result, (ST)std::abs(src1[k] - src2[k]));
            }
    }
    *_result = result;
    return 0;
}

template<typename T, typename ST> int
normDiffL1_(const T* src1, const T* src2, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if( !mask )
    {
        result += normL1<T, ST>(src1, src2, len*cn);
    }
    else
    {
        for( int i = 0; i < len; i++, src1 += cn, src2 += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                    result += std::abs(src1[k] - src2[k]);
            }
    }
    *_result = result;
    return 0;
}

template<typename T, typename ST> int
normDiffL2_(const T* src1, const T* src2, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if( !mask )
    {
        result += normL2Sqr<T, ST>(src1, src2, len*cn);
    }
    else
    {
        for( int i = 0; i < len; i++, src1 += cn, src2 += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                {
                    ST v = src1[k] - src2[k];
                    result += v*v;
                }
            }
    }
    *_result = result;
    return 0;
}

#define CV_DEF_NORM_FUNC(L, suffix, type, ntype) \
    static int norm##L##_##suffix(const type* src, const uchar* mask, ntype* r, int len, int cn) \
{ return norm##L##_(src, mask, r, len, cn); } \
    static int normDiff##L##_##suffix(const type* src1, const type* src2, \
    const uchar* mask, ntype* r, int len, int cn) \
{ return normDiff##L##_(src1, src2, mask, r, (int)len, cn); }

#define CV_DEF_NORM_ALL(suffix, type, inftype, l1type, l2type) \
    CV_DEF_NORM_FUNC(Inf, suffix, type, inftype) \
    CV_DEF_NORM_FUNC(L1, suffix, type, l1type) \
    CV_DEF_NORM_FUNC(L2, suffix, type, l2type)

CV_DEF_NORM_ALL(8u, uchar, int, int, int)
CV_DEF_NORM_ALL(8s, schar, int, int, int)
CV_DEF_NORM_ALL(16u, ushort, int, int, double)
CV_DEF_NORM_ALL(16s, short, int, int, double)
CV_DEF_NORM_ALL(32s, int, int, double, double)
CV_DEF_NORM_ALL(32f, float, float, double, double)
CV_DEF_NORM_ALL(64f, double, double, double, double)


typedef int (*NormFunc)(const uchar*, const uchar*, uchar*, int, int);
typedef int (*NormDiffFunc)(const uchar*, const uchar*, const uchar*, uchar*, int, int);

static NormFunc getNormFunc(int normType, int depth)
{
    static NormFunc normTab[3][8] =
    {
        {
            (NormFunc)GET_OPTIMIZED(normInf_8u), (NormFunc)GET_OPTIMIZED(normInf_8s), (NormFunc)GET_OPTIMIZED(normInf_16u), (NormFunc)GET_OPTIMIZED(normInf_16s),
            (NormFunc)GET_OPTIMIZED(normInf_32s), (NormFunc)GET_OPTIMIZED(normInf_32f), (NormFunc)normInf_64f, 0
        },
        {
            (NormFunc)GET_OPTIMIZED(normL1_8u), (NormFunc)GET_OPTIMIZED(normL1_8s), (NormFunc)GET_OPTIMIZED(normL1_16u), (NormFunc)GET_OPTIMIZED(normL1_16s),
            (NormFunc)GET_OPTIMIZED(normL1_32s), (NormFunc)GET_OPTIMIZED(normL1_32f), (NormFunc)normL1_64f, 0
        },
        {
            (NormFunc)GET_OPTIMIZED(normL2_8u), (NormFunc)GET_OPTIMIZED(normL2_8s), (NormFunc)GET_OPTIMIZED(normL2_16u), (NormFunc)GET_OPTIMIZED(normL2_16s),
            (NormFunc)GET_OPTIMIZED(normL2_32s), (NormFunc)GET_OPTIMIZED(normL2_32f), (NormFunc)normL2_64f, 0
        }
    };

    return normTab[normType][depth];
}

static NormDiffFunc getNormDiffFunc(int normType, int depth)
{
    static NormDiffFunc normDiffTab[3][8] =
    {
        {
            (NormDiffFunc)GET_OPTIMIZED(normDiffInf_8u), (NormDiffFunc)normDiffInf_8s,
            (NormDiffFunc)normDiffInf_16u, (NormDiffFunc)normDiffInf_16s,
            (NormDiffFunc)normDiffInf_32s, (NormDiffFunc)GET_OPTIMIZED(normDiffInf_32f),
            (NormDiffFunc)normDiffInf_64f, 0
        },
        {
            (NormDiffFunc)GET_OPTIMIZED(normDiffL1_8u), (NormDiffFunc)normDiffL1_8s,
            (NormDiffFunc)normDiffL1_16u, (NormDiffFunc)normDiffL1_16s,
            (NormDiffFunc)normDiffL1_32s, (NormDiffFunc)GET_OPTIMIZED(normDiffL1_32f),
            (NormDiffFunc)normDiffL1_64f, 0
        },
        {
            (NormDiffFunc)GET_OPTIMIZED(normDiffL2_8u), (NormDiffFunc)normDiffL2_8s,
            (NormDiffFunc)normDiffL2_16u, (NormDiffFunc)normDiffL2_16s,
            (NormDiffFunc)normDiffL2_32s, (NormDiffFunc)GET_OPTIMIZED(normDiffL2_32f),
            (NormDiffFunc)normDiffL2_64f, 0
        }
    };

    return normDiffTab[normType][depth];
}

#ifdef HAVE_OPENCL

static bool ocl_norm( InputArray _src, int normType, InputArray _mask, double & result )
{
    const ocl::Device & d = ocl::Device::getDefault();

#ifdef __ANDROID__
    if (d.isNVidia())
        return false;
#endif
    const int cn = _src.channels();
    if (cn > 4)
        return false;
    int type = _src.type(), depth = CV_MAT_DEPTH(type);
    bool doubleSupport = d.doubleFPConfig() > 0,
            haveMask = _mask.kind() != _InputArray::NONE;

    if ( !(normType == NORM_INF || normType == NORM_L1 || normType == NORM_L2 || normType == NORM_L2SQR) ||
         (!doubleSupport && depth == CV_64F))
        return false;

    UMat src = _src.getUMat();

    if (normType == NORM_INF)
    {
        if (!ocl_minMaxIdx(_src, NULL, &result, NULL, NULL, _mask,
                           std::max(depth, CV_32S), depth != CV_8U && depth != CV_16U))
            return false;
    }
    else if (normType == NORM_L1 || normType == NORM_L2 || normType == NORM_L2SQR)
    {
        Scalar sc;
        bool unstype = depth == CV_8U || depth == CV_16U;

        if ( !ocl_sum(haveMask ? src : src.reshape(1), sc, normType == NORM_L2 || normType == NORM_L2SQR ?
                    OCL_OP_SUM_SQR : (unstype ? OCL_OP_SUM : OCL_OP_SUM_ABS), _mask) )
            return false;

        double s = 0.0;
        for (int i = 0; i < (haveMask ? cn : 1); ++i)
            s += sc[i];

        result = normType == NORM_L1 || normType == NORM_L2SQR ? s : std::sqrt(s);
    }

    return true;
}

#endif

#ifdef HAVE_IPP
static bool ipp_norm(Mat &src, int normType, Mat &mask, double &result)
{
    CV_INSTRUMENT_REGION_IPP();

#if IPP_VERSION_X100 >= 700
    size_t total_size = src.total();
    int rows = src.size[0], cols = rows ? (int)(total_size/rows) : 0;

    if( (src.dims == 2 || (src.isContinuous() && mask.isContinuous()))
        && cols > 0 && (size_t)rows*cols == total_size )
    {
        if( !mask.empty() )
        {
            IppiSize sz = { cols, rows };
            int type = src.type();

            typedef IppStatus (CV_STDCALL* ippiMaskNormFuncC1)(const void *, int, const void *, int, IppiSize, Ipp64f *);
            ippiMaskNormFuncC1 ippiNorm_C1MR =
                normType == NORM_INF ?
                (type == CV_8UC1 ? (ippiMaskNormFuncC1)ippiNorm_Inf_8u_C1MR :
                type == CV_16UC1 ? (ippiMaskNormFuncC1)ippiNorm_Inf_16u_C1MR :
                type == CV_32FC1 ? (ippiMaskNormFuncC1)ippiNorm_Inf_32f_C1MR :
                0) :
            normType == NORM_L1 ?
                (type == CV_8UC1 ? (ippiMaskNormFuncC1)ippiNorm_L1_8u_C1MR :
                type == CV_16UC1 ? (ippiMaskNormFuncC1)ippiNorm_L1_16u_C1MR :
                type == CV_32FC1 ? (ippiMaskNormFuncC1)ippiNorm_L1_32f_C1MR :
                0) :
            normType == NORM_L2 || normType == NORM_L2SQR ?
                (type == CV_8UC1 ? (ippiMaskNormFuncC1)ippiNorm_L2_8u_C1MR :
                type == CV_16UC1 ? (ippiMaskNormFuncC1)ippiNorm_L2_16u_C1MR :
                type == CV_32FC1 ? (ippiMaskNormFuncC1)ippiNorm_L2_32f_C1MR :
                0) : 0;
            if( ippiNorm_C1MR )
            {
                Ipp64f norm;
                if( CV_INSTRUMENT_FUN_IPP(ippiNorm_C1MR, src.ptr(), (int)src.step[0], mask.ptr(), (int)mask.step[0], sz, &norm) >= 0 )
                {
                    result = (normType == NORM_L2SQR ? (double)(norm * norm) : (double)norm);
                    return true;
                }
            }
            typedef IppStatus (CV_STDCALL* ippiMaskNormFuncC3)(const void *, int, const void *, int, IppiSize, int, Ipp64f *);
            ippiMaskNormFuncC3 ippiNorm_C3CMR =
                normType == NORM_INF ?
                (type == CV_8UC3 ? (ippiMaskNormFuncC3)ippiNorm_Inf_8u_C3CMR :
                type == CV_16UC3 ? (ippiMaskNormFuncC3)ippiNorm_Inf_16u_C3CMR :
                type == CV_32FC3 ? (ippiMaskNormFuncC3)ippiNorm_Inf_32f_C3CMR :
                0) :
            normType == NORM_L1 ?
                (type == CV_8UC3 ? (ippiMaskNormFuncC3)ippiNorm_L1_8u_C3CMR :
                type == CV_16UC3 ? (ippiMaskNormFuncC3)ippiNorm_L1_16u_C3CMR :
                type == CV_32FC3 ? (ippiMaskNormFuncC3)ippiNorm_L1_32f_C3CMR :
                0) :
            normType == NORM_L2 || normType == NORM_L2SQR ?
                (type == CV_8UC3 ? (ippiMaskNormFuncC3)ippiNorm_L2_8u_C3CMR :
                type == CV_16UC3 ? (ippiMaskNormFuncC3)ippiNorm_L2_16u_C3CMR :
                type == CV_32FC3 ? (ippiMaskNormFuncC3)ippiNorm_L2_32f_C3CMR :
                0) : 0;
            if( ippiNorm_C3CMR )
            {
                Ipp64f norm1, norm2, norm3;
                if( CV_INSTRUMENT_FUN_IPP(ippiNorm_C3CMR, src.data, (int)src.step[0], mask.data, (int)mask.step[0], sz, 1, &norm1) >= 0 &&
                    CV_INSTRUMENT_FUN_IPP(ippiNorm_C3CMR, src.data, (int)src.step[0], mask.data, (int)mask.step[0], sz, 2, &norm2) >= 0 &&
                    CV_INSTRUMENT_FUN_IPP(ippiNorm_C3CMR, src.data, (int)src.step[0], mask.data, (int)mask.step[0], sz, 3, &norm3) >= 0)
                {
                    Ipp64f norm =
                        normType == NORM_INF ? std::max(std::max(norm1, norm2), norm3) :
                        normType == NORM_L1 ? norm1 + norm2 + norm3 :
                        normType == NORM_L2 || normType == NORM_L2SQR ? std::sqrt(norm1 * norm1 + norm2 * norm2 + norm3 * norm3) :
                        0;
                    result = (normType == NORM_L2SQR ? (double)(norm * norm) : (double)norm);
                    return true;
                }
            }
        }
        else
        {
            IppiSize sz = { cols*src.channels(), rows };
            int type = src.depth();

            typedef IppStatus (CV_STDCALL* ippiNormFuncHint)(const void *, int, IppiSize, Ipp64f *, IppHintAlgorithm hint);
            typedef IppStatus (CV_STDCALL* ippiNormFuncNoHint)(const void *, int, IppiSize, Ipp64f *);
            ippiNormFuncHint ippiNormHint =
                normType == NORM_L1 ?
                (type == CV_32FC1 ? (ippiNormFuncHint)ippiNorm_L1_32f_C1R :
                0) :
                normType == NORM_L2 || normType == NORM_L2SQR ?
                (type == CV_32FC1 ? (ippiNormFuncHint)ippiNorm_L2_32f_C1R :
                0) : 0;
            ippiNormFuncNoHint ippiNorm =
                normType == NORM_INF ?
                (type == CV_8UC1 ? (ippiNormFuncNoHint)ippiNorm_Inf_8u_C1R :
                type == CV_16UC1 ? (ippiNormFuncNoHint)ippiNorm_Inf_16u_C1R :
                type == CV_16SC1 ? (ippiNormFuncNoHint)ippiNorm_Inf_16s_C1R :
                type == CV_32FC1 ? (ippiNormFuncNoHint)ippiNorm_Inf_32f_C1R :
                0) :
                normType == NORM_L1 ?
                (type == CV_8UC1 ? (ippiNormFuncNoHint)ippiNorm_L1_8u_C1R :
                type == CV_16UC1 ? (ippiNormFuncNoHint)ippiNorm_L1_16u_C1R :
                type == CV_16SC1 ? (ippiNormFuncNoHint)ippiNorm_L1_16s_C1R :
                0) :
                normType == NORM_L2 || normType == NORM_L2SQR ?
                (type == CV_8UC1 ? (ippiNormFuncNoHint)ippiNorm_L2_8u_C1R :
                type == CV_16UC1 ? (ippiNormFuncNoHint)ippiNorm_L2_16u_C1R :
                type == CV_16SC1 ? (ippiNormFuncNoHint)ippiNorm_L2_16s_C1R :
                0) : 0;
            if( ippiNormHint || ippiNorm )
            {
                Ipp64f norm;
                IppStatus ret = ippiNormHint ? CV_INSTRUMENT_FUN_IPP(ippiNormHint, src.ptr(), (int)src.step[0], sz, &norm, ippAlgHintAccurate) :
                                CV_INSTRUMENT_FUN_IPP(ippiNorm, src.ptr(), (int)src.step[0], sz, &norm);
                if( ret >= 0 )
                {
                    result = (normType == NORM_L2SQR) ? norm * norm : norm;
                    return true;
                }
            }
        }
    }
#else
    CV_UNUSED(src); CV_UNUSED(normType); CV_UNUSED(mask); CV_UNUSED(result);
#endif
    return false;
}
#endif

} // cv::

double cv::norm( InputArray _src, int normType, InputArray _mask )
{
    CV_INSTRUMENT_REGION();

    normType &= NORM_TYPE_MASK;
    CV_Assert( normType == NORM_INF || normType == NORM_L1 ||
               normType == NORM_L2 || normType == NORM_L2SQR ||
               ((normType == NORM_HAMMING || normType == NORM_HAMMING2) && _src.type() == CV_8U) );

#if defined HAVE_OPENCL || defined HAVE_IPP
    double _result = 0;
#endif

#ifdef HAVE_OPENCL
    CV_OCL_RUN_(OCL_PERFORMANCE_CHECK(_src.isUMat()) && _src.dims() <= 2,
                ocl_norm(_src, normType, _mask, _result),
                _result)
#endif

    Mat src = _src.getMat(), mask = _mask.getMat();
    CV_IPP_RUN(IPP_VERSION_X100 >= 700, ipp_norm(src, normType, mask, _result), _result);

    int depth = src.depth(), cn = src.channels();
    if( src.isContinuous() && mask.empty() )
    {
        size_t len = src.total()*cn;
        if( len == (size_t)(int)len )
        {
            if( depth == CV_32F )
            {
                const float* data = src.ptr<float>();

                if( normType == NORM_L2 )
                {
                    double result = 0;
                    GET_OPTIMIZED(normL2_32f)(data, 0, &result, (int)len, 1);
                    return std::sqrt(result);
                }
                if( normType == NORM_L2SQR )
                {
                    double result = 0;
                    GET_OPTIMIZED(normL2_32f)(data, 0, &result, (int)len, 1);
                    return result;
                }
                if( normType == NORM_L1 )
                {
                    double result = 0;
                    GET_OPTIMIZED(normL1_32f)(data, 0, &result, (int)len, 1);
                    return result;
                }
                if( normType == NORM_INF )
                {
                    float result = 0;
                    GET_OPTIMIZED(normInf_32f)(data, 0, &result, (int)len, 1);
                    return result;
                }
            }
            if( depth == CV_8U )
            {
                const uchar* data = src.ptr<uchar>();

                if( normType == NORM_HAMMING )
                {
                    return hal::normHamming(data, (int)len);
                }

                if( normType == NORM_HAMMING2 )
                {
                    return hal::normHamming(data, (int)len, 2);
                }
            }
        }
    }

    CV_Assert( mask.empty() || mask.type() == CV_8U );

    if( normType == NORM_HAMMING || normType == NORM_HAMMING2 )
    {
        if( !mask.empty() )
        {
            Mat temp;
            bitwise_and(src, mask, temp);
            return norm(temp, normType);
        }
        int cellSize = normType == NORM_HAMMING ? 1 : 2;

        const Mat* arrays[] = {&src, 0};
        uchar* ptrs[1] = {};
        NAryMatIterator it(arrays, ptrs);
        int total = (int)it.size;
        int result = 0;

        for( size_t i = 0; i < it.nplanes; i++, ++it )
        {
            result += hal::normHamming(ptrs[0], total, cellSize);
        }

        return result;
    }

    NormFunc func = getNormFunc(normType >> 1, depth == CV_16F ? CV_32F : depth);
    CV_Assert( func != 0 );

    const Mat* arrays[] = {&src, &mask, 0};
    uchar* ptrs[2] = {};
    union
    {
        double d;
        int i;
        float f;
    }
    result;
    result.d = 0;
    NAryMatIterator it(arrays, ptrs);
    int j, total = (int)it.size, blockSize = total;
    bool blockSum = depth == CV_16F || (normType == NORM_L1 && depth <= CV_16S) ||
            ((normType == NORM_L2 || normType == NORM_L2SQR) && depth <= CV_8S);
    int isum = 0;
    int *ibuf = &result.i;
    AutoBuffer<float> fltbuf_;
    float* fltbuf = 0;
    size_t esz = 0;

    if( blockSum )
    {
        esz = src.elemSize();

        if( depth == CV_16F )
        {
            blockSize = std::min(blockSize, 1024);
            fltbuf_.allocate(blockSize);
            fltbuf = fltbuf_.data();
        }
        else
        {
            int intSumBlockSize = (normType == NORM_L1 && depth <= CV_8S ? (1 << 23) : (1 << 15))/cn;
            blockSize = std::min(blockSize, intSumBlockSize);
            ibuf = &isum;
        }
    }

    for( size_t i = 0; i < it.nplanes; i++, ++it )
    {
        for( j = 0; j < total; j += blockSize )
        {
            int bsz = std::min(total - j, blockSize);
            const uchar* data = ptrs[0];
            if( depth == CV_16F )
            {
                hal::cvt16f32f((const float16_t*)ptrs[0], fltbuf, bsz);
                data = (const uchar*)fltbuf;
            }
            func( data, ptrs[1], (uchar*)ibuf, bsz, cn );
            if( blockSum && depth != CV_16F )
            {
                result.d += isum;
                isum = 0;
            }
            ptrs[0] += bsz*esz;
            if( ptrs[1] )
                ptrs[1] += bsz;
        }
    }

    if( normType == NORM_INF )
    {
        if( depth == CV_64F )
            ;
        else if( depth == CV_32F )
            result.d = result.f;
        else
            result.d = result.i;
    }
    else if( normType == NORM_L2 )
        result.d = std::sqrt(result.d);

    return result.d;
}

//==================================================================================================

#ifdef HAVE_OPENCL

namespace cv {

static bool ocl_norm( InputArray _src1, InputArray _src2, int normType, InputArray _mask, double & result )
{
#ifdef __ANDROID__
    if (ocl::Device::getDefault().isNVidia())
        return false;
#endif

    Scalar sc1, sc2;
    int cn = _src1.channels();
    if (cn > 4)
        return false;
    int type = _src1.type(), depth = CV_MAT_DEPTH(type);
    bool relative = (normType & NORM_RELATIVE) != 0;
    normType &= ~NORM_RELATIVE;
    bool normsum = normType == NORM_L1 || normType == NORM_L2 || normType == NORM_L2SQR;

#ifdef __APPLE__
    if(normType == NORM_L1 && type == CV_16UC3 && !_mask.empty())
        return false;
#endif

    if (normsum)
    {
        if (!ocl_sum(_src1, sc1, normType == NORM_L2 || normType == NORM_L2SQR ?
                     OCL_OP_SUM_SQR : OCL_OP_SUM, _mask, _src2, relative, sc2))
            return false;
    }
    else
    {
        if (!ocl_minMaxIdx(_src1, NULL, &sc1[0], NULL, NULL, _mask, std::max(CV_32S, depth),
                           false, _src2, relative ? &sc2[0] : NULL))
            return false;
        cn = 1;
    }

    double s2 = 0;
    for (int i = 0; i < cn; ++i)
    {
        result += sc1[i];
        if (relative)
            s2 += sc2[i];
    }

    if (normType == NORM_L2)
    {
        result = std::sqrt(result);
        if (relative)
            s2 = std::sqrt(s2);
    }

    if (relative)
        result /= (s2 + DBL_EPSILON);

    return true;
}

}

#endif

#ifdef HAVE_IPP
namespace cv
{
static bool ipp_norm(InputArray _src1, InputArray _src2, int normType, InputArray _mask, double &result)
{
    CV_INSTRUMENT_REGION_IPP();

#if IPP_VERSION_X100 >= 700
    Mat src1 = _src1.getMat(), src2 = _src2.getMat(), mask = _mask.getMat();

    if( normType & CV_RELATIVE )
    {
        normType &= NORM_TYPE_MASK;

        size_t total_size = src1.total();
        int rows = src1.size[0], cols = rows ? (int)(total_size/rows) : 0;
        if( (src1.dims == 2 || (src1.isContinuous() && src2.isContinuous() && mask.isContinuous()))
            && cols > 0 && (size_t)rows*cols == total_size )
        {
            if( !mask.empty() )
            {
                IppiSize sz = { cols, rows };
                int type = src1.type();

                typedef IppStatus (CV_STDCALL* ippiMaskNormDiffFuncC1)(const void *, int, const void *, int, const void *, int, IppiSize, Ipp64f *);
                ippiMaskNormDiffFuncC1 ippiNormRel_C1MR =
                    normType == NORM_INF ?
                    (type == CV_8UC1 ? (ippiMaskNormDiffFuncC1)ippiNormRel_Inf_8u_C1MR :
                    type == CV_16UC1 ? (ippiMaskNormDiffFuncC1)ippiNormRel_Inf_16u_C1MR :
                    type == CV_32FC1 ? (ippiMaskNormDiffFuncC1)ippiNormRel_Inf_32f_C1MR :
                    0) :
                    normType == NORM_L1 ?
                    (type == CV_8UC1 ? (ippiMaskNormDiffFuncC1)ippiNormRel_L1_8u_C1MR :
                    type == CV_16UC1 ? (ippiMaskNormDiffFuncC1)ippiNormRel_L1_16u_C1MR :
                    type == CV_32FC1 ? (ippiMaskNormDiffFuncC1)ippiNormRel_L1_32f_C1MR :
                    0) :
                    normType == NORM_L2 || normType == NORM_L2SQR ?
                    (type == CV_8UC1 ? (ippiMaskNormDiffFuncC1)ippiNormRel_L2_8u_C1MR :
                    type == CV_16UC1 ? (ippiMaskNormDiffFuncC1)ippiNormRel_L2_16u_C1MR :
                    type == CV_32FC1 ? (ippiMaskNormDiffFuncC1)ippiNormRel_L2_32f_C1MR :
                    0) : 0;
                if( ippiNormRel_C1MR )
                {
                    Ipp64f norm;
                    if( CV_INSTRUMENT_FUN_IPP(ippiNormRel_C1MR, src1.ptr(), (int)src1.step[0], src2.ptr(), (int)src2.step[0], mask.ptr(), (int)mask.step[0], sz, &norm) >= 0 )
                    {
                        result = (normType == NORM_L2SQR ? (double)(norm * norm) : (double)norm);
                        return true;
                    }
                }
            }
            else
            {
                IppiSize sz = { cols*src1.channels(), rows };
                int type = src1.depth();

                typedef IppStatus (CV_STDCALL* ippiNormRelFuncHint)(const void *, int, const void *, int, IppiSize, Ipp64f *, IppHintAlgorithm hint);
                typedef IppStatus (CV_STDCALL* ippiNormRelFuncNoHint)(const void *, int, const void *, int, IppiSize, Ipp64f *);
                ippiNormRelFuncHint ippiNormRelHint =
                    normType == NORM_L1 ?
                    (type == CV_32F ? (ippiNormRelFuncHint)ippiNormRel_L1_32f_C1R :
                    0) :
                    normType == NORM_L2 || normType == NORM_L2SQR ?
                    (type == CV_32F ? (ippiNormRelFuncHint)ippiNormRel_L2_32f_C1R :
                    0) : 0;
                ippiNormRelFuncNoHint ippiNormRel =
                    normType == NORM_INF ?
                    (type == CV_8U ? (ippiNormRelFuncNoHint)ippiNormRel_Inf_8u_C1R :
                    type == CV_16U ? (ippiNormRelFuncNoHint)ippiNormRel_Inf_16u_C1R :
                    type == CV_16S ? (ippiNormRelFuncNoHint)ippiNormRel_Inf_16s_C1R :
                    type == CV_32F ? (ippiNormRelFuncNoHint)ippiNormRel_Inf_32f_C1R :
                    0) :
                    normType == NORM_L1 ?
                    (type == CV_8U ? (ippiNormRelFuncNoHint)ippiNormRel_L1_8u_C1R :
                    type == CV_16U ? (ippiNormRelFuncNoHint)ippiNormRel_L1_16u_C1R :
                    type == CV_16S ? (ippiNormRelFuncNoHint)ippiNormRel_L1_16s_C1R :
                    0) :
                    normType == NORM_L2 || normType == NORM_L2SQR ?
                    (type == CV_8U ? (ippiNormRelFuncNoHint)ippiNormRel_L2_8u_C1R :
                    type == CV_16U ? (ippiNormRelFuncNoHint)ippiNormRel_L2_16u_C1R :
                    type == CV_16S ? (ippiNormRelFuncNoHint)ippiNormRel_L2_16s_C1R :
                    0) : 0;
                if( ippiNormRelHint || ippiNormRel )
                {
                    Ipp64f norm;
                    IppStatus ret = ippiNormRelHint ? CV_INSTRUMENT_FUN_IPP(ippiNormRelHint, src1.ptr(), (int)src1.step[0], src2.ptr(), (int)src2.step[0], sz, &norm, ippAlgHintAccurate) :
                                    CV_INSTRUMENT_FUN_IPP(ippiNormRel, src1.ptr(), (int)src1.step[0], src2.ptr(), (int)src2.step[0], sz, &norm);
                    if( ret >= 0 )
                    {
                        result = (normType == NORM_L2SQR) ? norm * norm : norm;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    normType &= NORM_TYPE_MASK;

    size_t total_size = src1.total();
    int rows = src1.size[0], cols = rows ? (int)(total_size/rows) : 0;
    if( (src1.dims == 2 || (src1.isContinuous() && src2.isContinuous() && mask.isContinuous()))
        && cols > 0 && (size_t)rows*cols == total_size )
    {
        if( !mask.empty() )
        {
            IppiSize sz = { cols, rows };
            int type = src1.type();

            typedef IppStatus (CV_STDCALL* ippiMaskNormDiffFuncC1)(const void *, int, const void *, int, const void *, int, IppiSize, Ipp64f *);
            ippiMaskNormDiffFuncC1 ippiNormDiff_C1MR =
                normType == NORM_INF ?
                (type == CV_8UC1 ? (ippiMaskNormDiffFuncC1)ippiNormDiff_Inf_8u_C1MR :
                type == CV_16UC1 ? (ippiMaskNormDiffFuncC1)ippiNormDiff_Inf_16u_C1MR :
                type == CV_32FC1 ? (ippiMaskNormDiffFuncC1)ippiNormDiff_Inf_32f_C1MR :
                0) :
                normType == NORM_L1 ?
                (type == CV_8UC1 ? (ippiMaskNormDiffFuncC1)ippiNormDiff_L1_8u_C1MR :
                type == CV_16UC1 ? (ippiMaskNormDiffFuncC1)ippiNormDiff_L1_16u_C1MR :
                type == CV_32FC1 ? (ippiMaskNormDiffFuncC1)ippiNormDiff_L1_32f_C1MR :
                0) :
                normType == NORM_L2 || normType == NORM_L2SQR ?
                (type == CV_8UC1 ? (ippiMaskNormDiffFuncC1)ippiNormDiff_L2_8u_C1MR :
                type == CV_16UC1 ? (ippiMaskNormDiffFuncC1)ippiNormDiff_L2_16u_C1MR :
                type == CV_32FC1 ? (ippiMaskNormDiffFuncC1)ippiNormDiff_L2_32f_C1MR :
                0) : 0;
            if( ippiNormDiff_C1MR )
            {
                Ipp64f norm;
                if( CV_INSTRUMENT_FUN_IPP(ippiNormDiff_C1MR, src1.ptr(), (int)src1.step[0], src2.ptr(), (int)src2.step[0], mask.ptr(), (int)mask.step[0], sz, &norm) >= 0 )
                {
                    result = (normType == NORM_L2SQR ? (double)(norm * norm) : (double)norm);
                    return true;
                }
            }
            typedef IppStatus (CV_STDCALL* ippiMaskNormDiffFuncC3)(const void *, int, const void *, int, const void *, int, IppiSize, int, Ipp64f *);
            ippiMaskNormDiffFuncC3 ippiNormDiff_C3CMR =
                normType == NORM_INF ?
                (type == CV_8UC3 ? (ippiMaskNormDiffFuncC3)ippiNormDiff_Inf_8u_C3CMR :
                type == CV_16UC3 ? (ippiMaskNormDiffFuncC3)ippiNormDiff_Inf_16u_C3CMR :
                type == CV_32FC3 ? (ippiMaskNormDiffFuncC3)ippiNormDiff_Inf_32f_C3CMR :
                0) :
                normType == NORM_L1 ?
                (type == CV_8UC3 ? (ippiMaskNormDiffFuncC3)ippiNormDiff_L1_8u_C3CMR :
                type == CV_16UC3 ? (ippiMaskNormDiffFuncC3)ippiNormDiff_L1_16u_C3CMR :
                type == CV_32FC3 ? (ippiMaskNormDiffFuncC3)ippiNormDiff_L1_32f_C3CMR :
                0) :
                normType == NORM_L2 || normType == NORM_L2SQR ?
                (type == CV_8UC3 ? (ippiMaskNormDiffFuncC3)ippiNormDiff_L2_8u_C3CMR :
                type == CV_16UC3 ? (ippiMaskNormDiffFuncC3)ippiNormDiff_L2_16u_C3CMR :
                type == CV_32FC3 ? (ippiMaskNormDiffFuncC3)ippiNormDiff_L2_32f_C3CMR :
                0) : 0;
            if (cv::ipp::getIppTopFeatures() & (
#if IPP_VERSION_X100 >= 201700
                    ippCPUID_AVX512F |
#endif
                    ippCPUID_AVX2)
            ) // IPP_DISABLE_NORM_16UC3_mask_small (#11399)
            {
                if (normType == NORM_L1 && type == CV_16UC3 && sz.width < 16)
                    return false;
            }
            if( ippiNormDiff_C3CMR )
            {
                Ipp64f norm1, norm2, norm3;
                if( CV_INSTRUMENT_FUN_IPP(ippiNormDiff_C3CMR, src1.data, (int)src1.step[0], src2.data, (int)src2.step[0], mask.data, (int)mask.step[0], sz, 1, &norm1) >= 0 &&
                    CV_INSTRUMENT_FUN_IPP(ippiNormDiff_C3CMR, src1.data, (int)src1.step[0], src2.data, (int)src2.step[0], mask.data, (int)mask.step[0], sz, 2, &norm2) >= 0 &&
                    CV_INSTRUMENT_FUN_IPP(ippiNormDiff_C3CMR, src1.data, (int)src1.step[0], src2.data, (int)src2.step[0], mask.data, (int)mask.step[0], sz, 3, &norm3) >= 0)
                {
                    Ipp64f norm =
                        normType == NORM_INF ? std::max(std::max(norm1, norm2), norm3) :
                        normType == NORM_L1 ? norm1 + norm2 + norm3 :
                        normType == NORM_L2 || normType == NORM_L2SQR ? std::sqrt(norm1 * norm1 + norm2 * norm2 + norm3 * norm3) :
                        0;
                    result = (normType == NORM_L2SQR ? (double)(norm * norm) : (double)norm);
                    return true;
                }
            }
        }
        else
        {
            IppiSize sz = { cols*src1.channels(), rows };
            int type = src1.depth();

            typedef IppStatus (CV_STDCALL* ippiNormDiffFuncHint)(const void *, int, const void *, int, IppiSize, Ipp64f *, IppHintAlgorithm hint);
            typedef IppStatus (CV_STDCALL* ippiNormDiffFuncNoHint)(const void *, int, const void *, int, IppiSize, Ipp64f *);
            ippiNormDiffFuncHint ippiNormDiffHint =
                normType == NORM_L1 ?
                (type == CV_32F ? (ippiNormDiffFuncHint)ippiNormDiff_L1_32f_C1R :
                0) :
                normType == NORM_L2 || normType == NORM_L2SQR ?
                (type == CV_32F ? (ippiNormDiffFuncHint)ippiNormDiff_L2_32f_C1R :
                0) : 0;
            ippiNormDiffFuncNoHint ippiNormDiff =
                normType == NORM_INF ?
                (type == CV_8U ? (ippiNormDiffFuncNoHint)ippiNormDiff_Inf_8u_C1R :
                type == CV_16U ? (ippiNormDiffFuncNoHint)ippiNormDiff_Inf_16u_C1R :
                type == CV_16S ? (ippiNormDiffFuncNoHint)ippiNormDiff_Inf_16s_C1R :
                type == CV_32F ? (ippiNormDiffFuncNoHint)ippiNormDiff_Inf_32f_C1R :
                0) :
                normType == NORM_L1 ?
                (type == CV_8U ? (ippiNormDiffFuncNoHint)ippiNormDiff_L1_8u_C1R :
                type == CV_16U ? (ippiNormDiffFuncNoHint)ippiNormDiff_L1_16u_C1R :
                type == CV_16S ? (ippiNormDiffFuncNoHint)ippiNormDiff_L1_16s_C1R :
                0) :
                normType == NORM_L2 || normType == NORM_L2SQR ?
                (type == CV_8U ? (ippiNormDiffFuncNoHint)ippiNormDiff_L2_8u_C1R :
                type == CV_16U ? (ippiNormDiffFuncNoHint)ippiNormDiff_L2_16u_C1R :
                type == CV_16S ? (ippiNormDiffFuncNoHint)ippiNormDiff_L2_16s_C1R :
                0) : 0;
            if( ippiNormDiffHint || ippiNormDiff )
            {
                Ipp64f norm;
                IppStatus ret = ippiNormDiffHint ? CV_INSTRUMENT_FUN_IPP(ippiNormDiffHint, src1.ptr(), (int)src1.step[0], src2.ptr(), (int)src2.step[0], sz, &norm, ippAlgHintAccurate) :
                                CV_INSTRUMENT_FUN_IPP(ippiNormDiff, src1.ptr(), (int)src1.step[0], src2.ptr(), (int)src2.step[0], sz, &norm);
                if( ret >= 0 )
                {
                    result = (normType == NORM_L2SQR) ? norm * norm : norm;
                    return true;
                }
            }
        }
    }
#else
    CV_UNUSED(_src1); CV_UNUSED(_src2); CV_UNUSED(normType); CV_UNUSED(_mask); CV_UNUSED(result);
#endif
    return false;
}
}
#endif


double cv::norm( InputArray _src1, InputArray _src2, int normType, InputArray _mask )
{
    CV_INSTRUMENT_REGION();

    CV_Assert( _src1.sameSize(_src2) && _src1.type() == _src2.type() );

#if defined HAVE_OPENCL || defined HAVE_IPP
    double _result = 0;
#endif

#ifdef HAVE_OPENCL
    CV_OCL_RUN_(OCL_PERFORMANCE_CHECK(_src1.isUMat()),
                ocl_norm(_src1, _src2, normType, _mask, _result),
                _result)
#endif

    CV_IPP_RUN(IPP_VERSION_X100 >= 700, ipp_norm(_src1, _src2, normType, _mask, _result), _result);

    if( normType & CV_RELATIVE )
    {
        return norm(_src1, _src2, normType & ~CV_RELATIVE, _mask)/(norm(_src2, normType, _mask) + DBL_EPSILON);
    }

    Mat src1 = _src1.getMat(), src2 = _src2.getMat(), mask = _mask.getMat();
    int depth = src1.depth(), cn = src1.channels();

    normType &= 7;
    CV_Assert( normType == NORM_INF || normType == NORM_L1 ||
               normType == NORM_L2 || normType == NORM_L2SQR ||
              ((normType == NORM_HAMMING || normType == NORM_HAMMING2) && src1.type() == CV_8U) );

    if( src1.isContinuous() && src2.isContinuous() && mask.empty() )
    {
        size_t len = src1.total()*src1.channels();
        if( len == (size_t)(int)len )
        {
            if( src1.depth() == CV_32F )
            {
                const float* data1 = src1.ptr<float>();
                const float* data2 = src2.ptr<float>();

                if( normType == NORM_L2 )
                {
                    double result = 0;
                    GET_OPTIMIZED(normDiffL2_32f)(data1, data2, 0, &result, (int)len, 1);
                    return std::sqrt(result);
                }
                if( normType == NORM_L2SQR )
                {
                    double result = 0;
                    GET_OPTIMIZED(normDiffL2_32f)(data1, data2, 0, &result, (int)len, 1);
                    return result;
                }
                if( normType == NORM_L1 )
                {
                    double result = 0;
                    GET_OPTIMIZED(normDiffL1_32f)(data1, data2, 0, &result, (int)len, 1);
                    return result;
                }
                if( normType == NORM_INF )
                {
                    float result = 0;
                    GET_OPTIMIZED(normDiffInf_32f)(data1, data2, 0, &result, (int)len, 1);
                    return result;
                }
            }
        }
    }

    CV_Assert( mask.empty() || mask.type() == CV_8U );

    if( normType == NORM_HAMMING || normType == NORM_HAMMING2 )
    {
        if( !mask.empty() )
        {
            Mat temp;
            bitwise_xor(src1, src2, temp);
            bitwise_and(temp, mask, temp);
            return norm(temp, normType);
        }
        int cellSize = normType == NORM_HAMMING ? 1 : 2;

        const Mat* arrays[] = {&src1, &src2, 0};
        uchar* ptrs[2] = {};
        NAryMatIterator it(arrays, ptrs);
        int total = (int)it.size;
        int result = 0;

        for( size_t i = 0; i < it.nplanes; i++, ++it )
        {
            result += hal::normHamming(ptrs[0], ptrs[1], total, cellSize);
        }

        return result;
    }

    NormDiffFunc func = getNormDiffFunc(normType >> 1, depth == CV_16F ? CV_32F : depth);
    CV_Assert( func != 0 );

    const Mat* arrays[] = {&src1, &src2, &mask, 0};
    uchar* ptrs[3] = {};
    union
    {
        double d;
        float f;
        int i;
        unsigned u;
    }
    result;
    result.d = 0;
    NAryMatIterator it(arrays, ptrs);
    int j, total = (int)it.size, blockSize = total;
    bool blockSum = depth == CV_16F || (normType == NORM_L1 && depth <= CV_16S) ||
            ((normType == NORM_L2 || normType == NORM_L2SQR) && depth <= CV_8S);
    unsigned isum = 0;
    unsigned *ibuf = &result.u;
    AutoBuffer<float> fltbuf_;
    float* fltbuf = 0;
    size_t esz = 0;

    if( blockSum )
    {
        esz = src1.elemSize();

        if( depth == CV_16F )
        {
            blockSize = std::min(blockSize, 1024);
            fltbuf_.allocate(blockSize*2);
            fltbuf = fltbuf_.data();
        }
        else
        {
            int intSumBlockSize = (normType == NORM_L1 && depth <= CV_8S ? (1 << 23) : (1 << 15))/cn;
            blockSize = std::min(blockSize, intSumBlockSize);
            ibuf = &isum;
        }
    }

    for( size_t i = 0; i < it.nplanes; i++, ++it )
    {
        for( j = 0; j < total; j += blockSize )
        {
            int bsz = std::min(total - j, blockSize);
            const uchar *data0 = ptrs[0], *data1 = ptrs[1];
            if( depth == CV_16F )
            {
                hal::cvt16f32f((const float16_t*)ptrs[0], fltbuf, bsz);
                hal::cvt16f32f((const float16_t*)ptrs[1], fltbuf + bsz, bsz);
                data0 = (const uchar*)fltbuf;
                data1 = (const uchar*)(fltbuf + bsz);
            }
            func( data0, data1, ptrs[2], (uchar*)ibuf, bsz, cn );
            if( blockSum && depth != CV_16F )
            {
                result.d += isum;
                isum = 0;
            }
            ptrs[0] += bsz*esz;
            ptrs[1] += bsz*esz;
            if( ptrs[2] )
                ptrs[2] += bsz;
        }
    }

    if( normType == NORM_INF )
    {
        if( depth == CV_64F )
            ;
        else if( depth == CV_32F )
            result.d = result.f;
        else
            result.d = result.u;
    }
    else if( normType == NORM_L2 )
        result.d = std::sqrt(result.d);

    return result.d;
}

cv::Hamming::ResultType cv::Hamming::operator()( const unsigned char* a, const unsigned char* b, int size ) const
{
    return cv::hal::normHamming(a, b, size);
}

double cv::PSNR(InputArray _src1, InputArray _src2, double R)
{
    CV_INSTRUMENT_REGION();

    //Input arrays must have depth CV_8U
    CV_Assert( _src1.type() == _src2.type() );

    double diff = std::sqrt(norm(_src1, _src2, NORM_L2SQR)/(_src1.total()*_src1.channels()));
    return 20*log10(R/(diff+DBL_EPSILON));
}
