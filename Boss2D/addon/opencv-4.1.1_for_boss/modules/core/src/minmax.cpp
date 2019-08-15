// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html


#include "precomp.hpp"
#include "opencl_kernels_core.hpp"
#include "opencv2/core/openvx/ovx_defs.hpp"
#include "stat.hpp"

#undef HAVE_IPP
#undef CV_IPP_RUN_FAST
#define CV_IPP_RUN_FAST(f, ...)
#undef CV_IPP_RUN
#define CV_IPP_RUN(c, f, ...)

#define IPP_DISABLE_MINMAXIDX_MANY_ROWS 1  // see Core_MinMaxIdx.rows_overflow test

/****************************************************************************************\
*                                       minMaxLoc                                        *
\****************************************************************************************/

namespace cv
{

template<typename T, typename WT> static void
minMaxIdx_( const T* src, const uchar* mask, WT* _minVal, WT* _maxVal,
            size_t* _minIdx, size_t* _maxIdx, int len, size_t startIdx )
{
    WT minVal = *_minVal, maxVal = *_maxVal;
    size_t minIdx = *_minIdx, maxIdx = *_maxIdx;

    if( !mask )
    {
        for( int i = 0; i < len; i++ )
        {
            T val = src[i];
            if( val < minVal )
            {
                minVal = val;
                minIdx = startIdx + i;
            }
            if( val > maxVal )
            {
                maxVal = val;
                maxIdx = startIdx + i;
            }
        }
    }
    else
    {
        for( int i = 0; i < len; i++ )
        {
            T val = src[i];
            if( mask[i] && val < minVal )
            {
                minVal = val;
                minIdx = startIdx + i;
            }
            if( mask[i] && val > maxVal )
            {
                maxVal = val;
                maxIdx = startIdx + i;
            }
        }
    }

    *_minIdx = minIdx;
    *_maxIdx = maxIdx;
    *_minVal = minVal;
    *_maxVal = maxVal;
}

static void minMaxIdx_8u(const uchar* src, const uchar* mask, int* minval, int* maxval,
                         size_t* minidx, size_t* maxidx, int len, size_t startidx )
{ minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx ); }

static void minMaxIdx_8s(const schar* src, const uchar* mask, int* minval, int* maxval,
                         size_t* minidx, size_t* maxidx, int len, size_t startidx )
{ minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx ); }

static void minMaxIdx_16u(const ushort* src, const uchar* mask, int* minval, int* maxval,
                          size_t* minidx, size_t* maxidx, int len, size_t startidx )
{ minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx ); }

static void minMaxIdx_16s(const short* src, const uchar* mask, int* minval, int* maxval,
                          size_t* minidx, size_t* maxidx, int len, size_t startidx )
{ minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx ); }

static void minMaxIdx_32s(const int* src, const uchar* mask, int* minval, int* maxval,
                          size_t* minidx, size_t* maxidx, int len, size_t startidx )
{ minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx ); }

static void minMaxIdx_32f(const float* src, const uchar* mask, float* minval, float* maxval,
                          size_t* minidx, size_t* maxidx, int len, size_t startidx )
{ minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx ); }

static void minMaxIdx_64f(const double* src, const uchar* mask, double* minval, double* maxval,
                          size_t* minidx, size_t* maxidx, int len, size_t startidx )
{ minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx ); }

typedef void (*MinMaxIdxFunc)(const uchar*, const uchar*, int*, int*, size_t*, size_t*, int, size_t);

static MinMaxIdxFunc getMinmaxTab(int depth)
{
    static MinMaxIdxFunc minmaxTab[] =
    {
        (MinMaxIdxFunc)GET_OPTIMIZED(minMaxIdx_8u), (MinMaxIdxFunc)GET_OPTIMIZED(minMaxIdx_8s),
        (MinMaxIdxFunc)GET_OPTIMIZED(minMaxIdx_16u), (MinMaxIdxFunc)GET_OPTIMIZED(minMaxIdx_16s),
        (MinMaxIdxFunc)GET_OPTIMIZED(minMaxIdx_32s),
        (MinMaxIdxFunc)GET_OPTIMIZED(minMaxIdx_32f), (MinMaxIdxFunc)GET_OPTIMIZED(minMaxIdx_64f),
        0
    };

    return minmaxTab[depth];
}

static void ofs2idx(const Mat& a, size_t ofs, int* idx)
{
    int i, d = a.dims;
    if( ofs > 0 )
    {
        ofs--;
        for( i = d-1; i >= 0; i-- )
        {
            int sz = a.size[i];
            idx[i] = (int)(ofs % sz);
            ofs /= sz;
        }
    }
    else
    {
        for( i = d-1; i >= 0; i-- )
            idx[i] = -1;
    }
}

#ifdef HAVE_OPENCL

#define MINMAX_STRUCT_ALIGNMENT 8 // sizeof double

template <typename T>
void getMinMaxRes(const Mat & db, double * minVal, double * maxVal,
                  int* minLoc, int* maxLoc,
                  int groupnum, int cols, double * maxVal2)
{
    uint index_max = std::numeric_limits<uint>::max();
    T minval = std::numeric_limits<T>::max();
    T maxval = std::numeric_limits<T>::min() > 0 ? -std::numeric_limits<T>::max() : std::numeric_limits<T>::min(), maxval2 = maxval;
    uint minloc = index_max, maxloc = index_max;

    size_t index = 0;
    const T * minptr = NULL, * maxptr = NULL, * maxptr2 = NULL;
    const uint * minlocptr = NULL, * maxlocptr = NULL;
    if (minVal || minLoc)
    {
        minptr = db.ptr<T>();
        index += sizeof(T) * groupnum;
        index = alignSize(index, MINMAX_STRUCT_ALIGNMENT);
    }
    if (maxVal || maxLoc)
    {
        maxptr = (const T *)(db.ptr() + index);
        index += sizeof(T) * groupnum;
        index = alignSize(index, MINMAX_STRUCT_ALIGNMENT);
    }
    if (minLoc)
    {
        minlocptr = (const uint *)(db.ptr() + index);
        index += sizeof(uint) * groupnum;
        index = alignSize(index, MINMAX_STRUCT_ALIGNMENT);
    }
    if (maxLoc)
    {
        maxlocptr = (const uint *)(db.ptr() + index);
        index += sizeof(uint) * groupnum;
        index = alignSize(index, MINMAX_STRUCT_ALIGNMENT);
    }
    if (maxVal2)
        maxptr2 = (const T *)(db.ptr() + index);

    for (int i = 0; i < groupnum; i++)
    {
        if (minptr && minptr[i] <= minval)
        {
            if (minptr[i] == minval)
            {
                if (minlocptr)
                    minloc = std::min(minlocptr[i], minloc);
            }
            else
            {
                if (minlocptr)
                    minloc = minlocptr[i];
                minval = minptr[i];
            }
        }
        if (maxptr && maxptr[i] >= maxval)
        {
            if (maxptr[i] == maxval)
            {
                if (maxlocptr)
                    maxloc = std::min(maxlocptr[i], maxloc);
            }
            else
            {
                if (maxlocptr)
                    maxloc = maxlocptr[i];
                maxval = maxptr[i];
            }
        }
        if (maxptr2 && maxptr2[i] > maxval2)
            maxval2 = maxptr2[i];
    }
    bool zero_mask = (minLoc && minloc == index_max) ||
            (maxLoc && maxloc == index_max);

    if (minVal)
        *minVal = zero_mask ? 0 : (double)minval;
    if (maxVal)
        *maxVal = zero_mask ? 0 : (double)maxval;
    if (maxVal2)
        *maxVal2 = zero_mask ? 0 : (double)maxval2;

    if (minLoc)
    {
        minLoc[0] = zero_mask ? -1 : minloc / cols;
        minLoc[1] = zero_mask ? -1 : minloc % cols;
    }
    if (maxLoc)
    {
        maxLoc[0] = zero_mask ? -1 : maxloc / cols;
        maxLoc[1] = zero_mask ? -1 : maxloc % cols;
    }
}

typedef void (*getMinMaxResFunc)(const Mat & db, double * minVal, double * maxVal,
                                 int * minLoc, int *maxLoc, int gropunum, int cols, double * maxVal2);

bool ocl_minMaxIdx( InputArray _src, double* minVal, double* maxVal, int* minLoc, int* maxLoc, InputArray _mask,
                           int ddepth, bool absValues, InputArray _src2, double * maxVal2)
{
    const ocl::Device & dev = ocl::Device::getDefault();

#ifdef __ANDROID__
    if (dev.isNVidia())
        return false;
#endif

    bool doubleSupport = dev.doubleFPConfig() > 0, haveMask = !_mask.empty(),
        haveSrc2 = _src2.kind() != _InputArray::NONE;
    int type = _src.type(), depth = CV_MAT_DEPTH(type), cn = CV_MAT_CN(type),
            kercn = haveMask ? cn : std::min(4, ocl::predictOptimalVectorWidth(_src, _src2));

    // disabled following modes since it occasionally fails on AMD devices (e.g. A10-6800K, sep. 2014)
    if ((haveMask || type == CV_32FC1) && dev.isAMD())
        return false;

    CV_Assert( (cn == 1 && (!haveMask || _mask.type() == CV_8U)) ||
              (cn >= 1 && !minLoc && !maxLoc) );

    if (ddepth < 0)
        ddepth = depth;

    CV_Assert(!haveSrc2 || _src2.type() == type);

    if (depth == CV_32S)
        return false;

    if ((depth == CV_64F || ddepth == CV_64F) && !doubleSupport)
        return false;

    int groupnum = dev.maxComputeUnits();
    size_t wgs = dev.maxWorkGroupSize();

    int wgs2_aligned = 1;
    while (wgs2_aligned < (int)wgs)
        wgs2_aligned <<= 1;
    wgs2_aligned >>= 1;

    bool needMinVal = minVal || minLoc, needMinLoc = minLoc != NULL,
            needMaxVal = maxVal || maxLoc, needMaxLoc = maxLoc != NULL;

    // in case of mask we must know whether mask is filled with zeros or not
    // so let's calculate min or max location, if it's undefined, so mask is zeros
    if (!(needMaxLoc || needMinLoc) && haveMask)
    {
        if (needMinVal)
            needMinLoc = true;
        else
            needMaxLoc = true;
    }

    char cvt[2][40];
    String opts = format("-D DEPTH_%d -D srcT1=%s%s -D WGS=%d -D srcT=%s"
                         " -D WGS2_ALIGNED=%d%s%s%s -D kercn=%d%s%s%s%s"
                         " -D dstT1=%s -D dstT=%s -D convertToDT=%s%s%s%s%s -D wdepth=%d -D convertFromU=%s"
                         " -D MINMAX_STRUCT_ALIGNMENT=%d",
                         depth, ocl::typeToStr(depth), haveMask ? " -D HAVE_MASK" : "", (int)wgs,
                         ocl::typeToStr(CV_MAKE_TYPE(depth, kercn)), wgs2_aligned,
                         doubleSupport ? " -D DOUBLE_SUPPORT" : "",
                         _src.isContinuous() ? " -D HAVE_SRC_CONT" : "",
                         _mask.isContinuous() ? " -D HAVE_MASK_CONT" : "", kercn,
                         needMinVal ? " -D NEED_MINVAL" : "", needMaxVal ? " -D NEED_MAXVAL" : "",
                         needMinLoc ? " -D NEED_MINLOC" : "", needMaxLoc ? " -D NEED_MAXLOC" : "",
                         ocl::typeToStr(ddepth), ocl::typeToStr(CV_MAKE_TYPE(ddepth, kercn)),
                         ocl::convertTypeStr(depth, ddepth, kercn, cvt[0]),
                         absValues ? " -D OP_ABS" : "",
                         haveSrc2 ? " -D HAVE_SRC2" : "", maxVal2 ? " -D OP_CALC2" : "",
                         haveSrc2 && _src2.isContinuous() ? " -D HAVE_SRC2_CONT" : "", ddepth,
                         depth <= CV_32S && ddepth == CV_32S ? ocl::convertTypeStr(CV_8U, ddepth, kercn, cvt[1]) : "noconvert",
                         MINMAX_STRUCT_ALIGNMENT);

    ocl::Kernel k("minmaxloc", ocl::core::minmaxloc_oclsrc, opts);
    if (k.empty())
        return false;

    int esz = CV_ELEM_SIZE(ddepth), esz32s = CV_ELEM_SIZE1(CV_32S),
            dbsize = groupnum * ((needMinVal ? esz : 0) + (needMaxVal ? esz : 0) +
                                 (needMinLoc ? esz32s : 0) + (needMaxLoc ? esz32s : 0) +
                                 (maxVal2 ? esz : 0))
                     + 5 * MINMAX_STRUCT_ALIGNMENT;
    UMat src = _src.getUMat(), src2 = _src2.getUMat(), db(1, dbsize, CV_8UC1), mask = _mask.getUMat();

    if (cn > 1 && !haveMask)
    {
        src = src.reshape(1);
        src2 = src2.reshape(1);
    }

    if (haveSrc2)
    {
        if (!haveMask)
            k.args(ocl::KernelArg::ReadOnlyNoSize(src), src.cols, (int)src.total(),
                   groupnum, ocl::KernelArg::PtrWriteOnly(db), ocl::KernelArg::ReadOnlyNoSize(src2));
        else
            k.args(ocl::KernelArg::ReadOnlyNoSize(src), src.cols, (int)src.total(),
                   groupnum, ocl::KernelArg::PtrWriteOnly(db), ocl::KernelArg::ReadOnlyNoSize(mask),
                   ocl::KernelArg::ReadOnlyNoSize(src2));
    }
    else
    {
        if (!haveMask)
            k.args(ocl::KernelArg::ReadOnlyNoSize(src), src.cols, (int)src.total(),
                   groupnum, ocl::KernelArg::PtrWriteOnly(db));
        else
            k.args(ocl::KernelArg::ReadOnlyNoSize(src), src.cols, (int)src.total(),
                   groupnum, ocl::KernelArg::PtrWriteOnly(db), ocl::KernelArg::ReadOnlyNoSize(mask));
    }

    size_t globalsize = groupnum * wgs;
    if (!k.run(1, &globalsize, &wgs, true))
        return false;

    static const getMinMaxResFunc functab[7] =
    {
        getMinMaxRes<uchar>,
        getMinMaxRes<char>,
        getMinMaxRes<ushort>,
        getMinMaxRes<short>,
        getMinMaxRes<int>,
        getMinMaxRes<float>,
        getMinMaxRes<double>
    };

    getMinMaxResFunc func = functab[ddepth];

    int locTemp[2];
    func(db.getMat(ACCESS_READ), minVal, maxVal,
         needMinLoc ? minLoc ? minLoc : locTemp : minLoc,
         needMaxLoc ? maxLoc ? maxLoc : locTemp : maxLoc,
         groupnum, src.cols, maxVal2);

    return true;
}

#endif

#ifdef HAVE_OPENVX
namespace ovx {
    template <> inline bool skipSmallImages<VX_KERNEL_MINMAXLOC>(int w, int h) { return w*h < 3840 * 2160; }
}
static bool openvx_minMaxIdx(Mat &src, double* minVal, double* maxVal, int* minIdx, int* maxIdx, Mat &mask)
{
    int stype = src.type();
    size_t total_size = src.total();
    int rows = src.size[0], cols = rows ? (int)(total_size / rows) : 0;
    if ((stype != CV_8UC1 && stype != CV_16SC1) || !mask.empty() ||
        (src.dims != 2 && !(src.isContinuous() && cols > 0 && (size_t)rows*cols == total_size))
        )
        return false;

    try
    {
        ivx::Context ctx = ovx::getOpenVXContext();
        ivx::Image
            ia = ivx::Image::createFromHandle(ctx, stype == CV_8UC1 ? VX_DF_IMAGE_U8 : VX_DF_IMAGE_S16,
                ivx::Image::createAddressing(cols, rows, stype == CV_8UC1 ? 1 : 2, (vx_int32)(src.step[0])), src.ptr());

        ivx::Scalar vxMinVal = ivx::Scalar::create(ctx, stype == CV_8UC1 ? VX_TYPE_UINT8 : VX_TYPE_INT16, 0);
        ivx::Scalar vxMaxVal = ivx::Scalar::create(ctx, stype == CV_8UC1 ? VX_TYPE_UINT8 : VX_TYPE_INT16, 0);
        ivx::Array vxMinInd, vxMaxInd;
        ivx::Scalar vxMinCount, vxMaxCount;
        if (minIdx)
        {
            vxMinInd = ivx::Array::create(ctx, VX_TYPE_COORDINATES2D, 1);
            vxMinCount = ivx::Scalar::create(ctx, VX_TYPE_UINT32, 0);
        }
        if (maxIdx)
        {
            vxMaxInd = ivx::Array::create(ctx, VX_TYPE_COORDINATES2D, 1);
            vxMaxCount = ivx::Scalar::create(ctx, VX_TYPE_UINT32, 0);
        }

        ivx::IVX_CHECK_STATUS(vxuMinMaxLoc(ctx, ia, vxMinVal, vxMaxVal, vxMinInd, vxMaxInd, vxMinCount, vxMaxCount));

        if (minVal)
        {
            *minVal = stype == CV_8UC1 ? vxMinVal.getValue<vx_uint8>() : vxMinVal.getValue<vx_int16>();
        }
        if (maxVal)
        {
            *maxVal = stype == CV_8UC1 ? vxMaxVal.getValue<vx_uint8>() : vxMaxVal.getValue<vx_int16>();
        }
        if (minIdx)
        {
            if(vxMinCount.getValue<vx_uint32>()<1) throw ivx::RuntimeError(VX_ERROR_INVALID_VALUE, std::string(__func__) + "(): minimum value location not found");
            vx_coordinates2d_t loc;
            vxMinInd.copyRangeTo(0, 1, &loc);
            size_t minidx = loc.y * cols + loc.x + 1;
            ofs2idx(src, minidx, minIdx);
        }
        if (maxIdx)
        {
            if (vxMaxCount.getValue<vx_uint32>()<1) throw ivx::RuntimeError(VX_ERROR_INVALID_VALUE, std::string(__func__) + "(): maximum value location not found");
            vx_coordinates2d_t loc;
            vxMaxInd.copyRangeTo(0, 1, &loc);
            size_t maxidx = loc.y * cols + loc.x + 1;
            ofs2idx(src, maxidx, maxIdx);
        }
    }
    catch (const ivx::RuntimeError & e)
    {
        VX_DbgThrow(e.what());
    }
    catch (const ivx::WrapperError & e)
    {
        VX_DbgThrow(e.what());
    }

    return true;
}
#endif

#ifdef HAVE_IPP
static IppStatus ipp_minMaxIndex_wrap(const void* pSrc, int srcStep, IppiSize size, IppDataType dataType,
    float* pMinVal, float* pMaxVal, IppiPoint* pMinIndex, IppiPoint* pMaxIndex, const Ipp8u*, int)
{
    switch(dataType)
    {
    case ipp8u:  return CV_INSTRUMENT_FUN_IPP(ippiMinMaxIndx_8u_C1R, (const Ipp8u*)pSrc, srcStep, size, pMinVal, pMaxVal, pMinIndex, pMaxIndex);
    case ipp16u: return CV_INSTRUMENT_FUN_IPP(ippiMinMaxIndx_16u_C1R, (const Ipp16u*)pSrc, srcStep, size, pMinVal, pMaxVal, pMinIndex, pMaxIndex);
    case ipp32f: return CV_INSTRUMENT_FUN_IPP(ippiMinMaxIndx_32f_C1R, (const Ipp32f*)pSrc, srcStep, size, pMinVal, pMaxVal, pMinIndex, pMaxIndex);
    default:     return ippStsDataTypeErr;
    }
}

static IppStatus ipp_minMaxIndexMask_wrap(const void* pSrc, int srcStep, IppiSize size, IppDataType dataType,
    float* pMinVal, float* pMaxVal, IppiPoint* pMinIndex, IppiPoint* pMaxIndex, const Ipp8u* pMask, int maskStep)
{
    switch(dataType)
    {
    case ipp8u:  return CV_INSTRUMENT_FUN_IPP(ippiMinMaxIndx_8u_C1MR, (const Ipp8u*)pSrc, srcStep, pMask, maskStep, size, pMinVal, pMaxVal, pMinIndex, pMaxIndex);
    case ipp16u: return CV_INSTRUMENT_FUN_IPP(ippiMinMaxIndx_16u_C1MR, (const Ipp16u*)pSrc, srcStep, pMask, maskStep, size, pMinVal, pMaxVal, pMinIndex, pMaxIndex);
    case ipp32f: return CV_INSTRUMENT_FUN_IPP(ippiMinMaxIndx_32f_C1MR, (const Ipp32f*)pSrc, srcStep, pMask, maskStep, size, pMinVal, pMaxVal, pMinIndex, pMaxIndex);
    default:     return ippStsDataTypeErr;
    }
}

static IppStatus ipp_minMax_wrap(const void* pSrc, int srcStep, IppiSize size, IppDataType dataType,
    float* pMinVal, float* pMaxVal, IppiPoint*, IppiPoint*, const Ipp8u*, int)
{
    IppStatus status;

    switch(dataType)
    {
#if IPP_VERSION_X100 > 201701 // wrong min values
    case ipp8u:
    {
        Ipp8u val[2];
        status = CV_INSTRUMENT_FUN_IPP(ippiMinMax_8u_C1R, (const Ipp8u*)pSrc, srcStep, size, &val[0], &val[1]);
        *pMinVal = val[0];
        *pMaxVal = val[1];
        return status;
    }
#endif
    case ipp16u:
    {
        Ipp16u val[2];
        status = CV_INSTRUMENT_FUN_IPP(ippiMinMax_16u_C1R, (const Ipp16u*)pSrc, srcStep, size, &val[0], &val[1]);
        *pMinVal = val[0];
        *pMaxVal = val[1];
        return status;
    }
    case ipp16s:
    {
        Ipp16s val[2];
        status = CV_INSTRUMENT_FUN_IPP(ippiMinMax_16s_C1R, (const Ipp16s*)pSrc, srcStep, size, &val[0], &val[1]);
        *pMinVal = val[0];
        *pMaxVal = val[1];
        return status;
    }
    case ipp32f: return CV_INSTRUMENT_FUN_IPP(ippiMinMax_32f_C1R, (const Ipp32f*)pSrc, srcStep, size, pMinVal, pMaxVal);
    default:     return ipp_minMaxIndex_wrap(pSrc, srcStep, size, dataType, pMinVal, pMaxVal, NULL, NULL, NULL, 0);
    }
}

static IppStatus ipp_minIdx_wrap(const void* pSrc, int srcStep, IppiSize size, IppDataType dataType,
    float* pMinVal, float*, IppiPoint* pMinIndex, IppiPoint*, const Ipp8u*, int)
{
    IppStatus status;

    switch(dataType)
    {
    case ipp8u:
    {
        Ipp8u val;
        status = CV_INSTRUMENT_FUN_IPP(ippiMinIndx_8u_C1R, (const Ipp8u*)pSrc, srcStep, size, &val, &pMinIndex->x, &pMinIndex->y);
        *pMinVal = val;
        return status;
    }
    case ipp16u:
    {
        Ipp16u val;
        status = CV_INSTRUMENT_FUN_IPP(ippiMinIndx_16u_C1R, (const Ipp16u*)pSrc, srcStep, size, &val, &pMinIndex->x, &pMinIndex->y);
        *pMinVal = val;
        return status;
    }
    case ipp16s:
    {
        Ipp16s val;
        status = CV_INSTRUMENT_FUN_IPP(ippiMinIndx_16s_C1R, (const Ipp16s*)pSrc, srcStep, size, &val, &pMinIndex->x, &pMinIndex->y);
        *pMinVal = val;
        return status;
    }
    case ipp32f: return CV_INSTRUMENT_FUN_IPP(ippiMinIndx_32f_C1R, (const Ipp32f*)pSrc, srcStep, size, pMinVal, &pMinIndex->x, &pMinIndex->y);
    default:     return ipp_minMaxIndex_wrap(pSrc, srcStep, size, dataType, pMinVal, NULL, pMinIndex, NULL, NULL, 0);
    }
}

static IppStatus ipp_maxIdx_wrap(const void* pSrc, int srcStep, IppiSize size, IppDataType dataType,
    float*, float* pMaxVal, IppiPoint*, IppiPoint* pMaxIndex, const Ipp8u*, int)
{
    IppStatus status;

    switch(dataType)
    {
    case ipp8u:
    {
        Ipp8u val;
        status = CV_INSTRUMENT_FUN_IPP(ippiMaxIndx_8u_C1R, (const Ipp8u*)pSrc, srcStep, size, &val, &pMaxIndex->x, &pMaxIndex->y);
        *pMaxVal = val;
        return status;
    }
    case ipp16u:
    {
        Ipp16u val;
        status = CV_INSTRUMENT_FUN_IPP(ippiMaxIndx_16u_C1R, (const Ipp16u*)pSrc, srcStep, size, &val, &pMaxIndex->x, &pMaxIndex->y);
        *pMaxVal = val;
        return status;
    }
    case ipp16s:
    {
        Ipp16s val;
        status = CV_INSTRUMENT_FUN_IPP(ippiMaxIndx_16s_C1R, (const Ipp16s*)pSrc, srcStep, size, &val, &pMaxIndex->x, &pMaxIndex->y);
        *pMaxVal = val;
        return status;
    }
    case ipp32f: return CV_INSTRUMENT_FUN_IPP(ippiMaxIndx_32f_C1R, (const Ipp32f*)pSrc, srcStep, size, pMaxVal, &pMaxIndex->x, &pMaxIndex->y);
    default:     return ipp_minMaxIndex_wrap(pSrc, srcStep, size, dataType, NULL, pMaxVal, NULL, pMaxIndex, NULL, 0);
    }
}

typedef IppStatus (*IppMinMaxSelector)(const void* pSrc, int srcStep, IppiSize size, IppDataType dataType,
    float* pMinVal, float* pMaxVal, IppiPoint* pMinIndex, IppiPoint* pMaxIndex, const Ipp8u* pMask, int maskStep);

static bool ipp_minMaxIdx(Mat &src, double* _minVal, double* _maxVal, int* _minIdx, int* _maxIdx, Mat &mask)
{
#if IPP_VERSION_X100 >= 700
    CV_INSTRUMENT_REGION_IPP();

#if IPP_VERSION_X100 < 201800
    // cv::minMaxIdx problem with NaN input
    // Disable 32F processing only
    if(src.depth() == CV_32F && cv::ipp::getIppTopFeatures() == ippCPUID_SSE42)
        return false;
#endif

#if IPP_VERSION_X100 < 201801
    // cv::minMaxIdx problem with index positions on AVX
    if(!mask.empty() && _maxIdx && cv::ipp::getIppTopFeatures() != ippCPUID_SSE42)
        return false;
#endif

    IppStatus   status;
    IppDataType dataType = ippiGetDataType(src.depth());
    float       minVal = 0;
    float       maxVal = 0;
    IppiPoint   minIdx = {-1, -1};
    IppiPoint   maxIdx = {-1, -1};

    float       *pMinVal = (_minVal || _minIdx)?&minVal:NULL;
    float       *pMaxVal = (_maxVal || _maxIdx)?&maxVal:NULL;
    IppiPoint   *pMinIdx = (_minIdx)?&minIdx:NULL;
    IppiPoint   *pMaxIdx = (_maxIdx)?&maxIdx:NULL;

    IppMinMaxSelector ippMinMaxFun = ipp_minMaxIndexMask_wrap;
    if(mask.empty())
    {
        if(_maxVal && _maxIdx && !_minVal && !_minIdx)
            ippMinMaxFun = ipp_maxIdx_wrap;
        else if(!_maxVal && !_maxIdx && _minVal && _minIdx)
            ippMinMaxFun = ipp_minIdx_wrap;
        else if(_maxVal && !_maxIdx && _minVal && !_minIdx)
            ippMinMaxFun = ipp_minMax_wrap;
        else if(!_maxVal && !_maxIdx && !_minVal && !_minIdx)
            return false;
        else
            ippMinMaxFun = ipp_minMaxIndex_wrap;
    }

    if(src.dims <= 2)
    {
        IppiSize size = ippiSize(src.size());
#if defined(_WIN32) && !defined(_WIN64) && IPP_VERSION_X100 == 201900 && IPP_DISABLE_MINMAXIDX_MANY_ROWS
        if (size.height > 65536)
            return false;  // test: Core_MinMaxIdx.rows_overflow
#endif
        size.width *= src.channels();

        status = ippMinMaxFun(src.ptr(), (int)src.step, size, dataType, pMinVal, pMaxVal, pMinIdx, pMaxIdx, (Ipp8u*)mask.ptr(), (int)mask.step);
        if(status < 0)
            return false;
        if(_minVal)
            *_minVal = minVal;
        if(_maxVal)
            *_maxVal = maxVal;
        if(_minIdx)
        {
#if IPP_VERSION_X100 < 201801
            // Should be just ippStsNoOperation check, but there is a bug in the function so we need additional checks
            if(status == ippStsNoOperation && !mask.empty() && !pMinIdx->x && !pMinIdx->y)
#else
            if(status == ippStsNoOperation)
#endif
            {
                _minIdx[0] = -1;
                _minIdx[1] = -1;
            }
            else
            {
                _minIdx[0] = minIdx.y;
                _minIdx[1] = minIdx.x;
            }
        }
        if(_maxIdx)
        {
#if IPP_VERSION_X100 < 201801
            // Should be just ippStsNoOperation check, but there is a bug in the function so we need additional checks
            if(status == ippStsNoOperation && !mask.empty() && !pMaxIdx->x && !pMaxIdx->y)
#else
            if(status == ippStsNoOperation)
#endif
            {
                _maxIdx[0] = -1;
                _maxIdx[1] = -1;
            }
            else
            {
                _maxIdx[0] = maxIdx.y;
                _maxIdx[1] = maxIdx.x;
            }
        }
    }
    else
    {
        const Mat *arrays[] = {&src, mask.empty()?NULL:&mask, NULL};
        uchar     *ptrs[3]  = {NULL};
        NAryMatIterator it(arrays, ptrs);
        IppiSize size = ippiSize(it.size*src.channels(), 1);
        int srcStep      = (int)(size.width*src.elemSize1());
        int maskStep     = size.width;
        size_t idxPos    = 1;
        size_t minIdxAll = 0;
        size_t maxIdxAll = 0;
        float  minValAll = IPP_MAXABS_32F;
        float  maxValAll = -IPP_MAXABS_32F;

        for(size_t i = 0; i < it.nplanes; i++, ++it, idxPos += size.width)
        {
            status = ippMinMaxFun(ptrs[0], srcStep, size, dataType, pMinVal, pMaxVal, pMinIdx, pMaxIdx, ptrs[1], maskStep);
            if(status < 0)
                return false;
#if IPP_VERSION_X100 > 201701
            // Zero-mask check, function should return ippStsNoOperation warning
            if(status == ippStsNoOperation)
                    continue;
#else
            // Crude zero-mask check, waiting for fix in IPP function
            if(ptrs[1])
            {
                Mat localMask(Size(size.width, 1), CV_8U, ptrs[1], maskStep);
                if(!cv::countNonZero(localMask))
                    continue;
            }
#endif

            if(_minVal && minVal < minValAll)
            {
                minValAll = minVal;
                minIdxAll = idxPos+minIdx.x;
            }
            if(_maxVal && maxVal > maxValAll)
            {
                maxValAll = maxVal;
                maxIdxAll = idxPos+maxIdx.x;
            }
        }
        if(!src.empty() && mask.empty())
        {
            if(minIdxAll == 0)
                minIdxAll = 1;
            if(maxValAll == 0)
                maxValAll = 1;
        }

        if(_minVal)
            *_minVal = minValAll;
        if(_maxVal)
            *_maxVal = maxValAll;
        if(_minIdx)
            ofs2idx(src, minIdxAll, _minIdx);
        if(_maxIdx)
            ofs2idx(src, maxIdxAll, _maxIdx);
    }

    return true;
#else
    CV_UNUSED(src); CV_UNUSED(minVal); CV_UNUSED(maxVal); CV_UNUSED(minIdx); CV_UNUSED(maxIdx); CV_UNUSED(mask);
    return false;
#endif
}
#endif

}

void cv::minMaxIdx(InputArray _src, double* minVal,
                   double* maxVal, int* minIdx, int* maxIdx,
                   InputArray _mask)
{
    CV_INSTRUMENT_REGION();

    int type = _src.type(), depth = CV_MAT_DEPTH(type), cn = CV_MAT_CN(type);
    CV_Assert( (cn == 1 && (_mask.empty() || _mask.type() == CV_8U)) ||
        (cn > 1 && _mask.empty() && !minIdx && !maxIdx) );

    CV_OCL_RUN(OCL_PERFORMANCE_CHECK(_src.isUMat()) && _src.dims() <= 2  && (_mask.empty() || _src.size() == _mask.size()),
               ocl_minMaxIdx(_src, minVal, maxVal, minIdx, maxIdx, _mask))

    Mat src = _src.getMat(), mask = _mask.getMat();

    if (src.dims <= 2)
        CALL_HAL(minMaxIdx, cv_hal_minMaxIdx, src.data, src.step, src.cols, src.rows, src.depth(), minVal, maxVal,
                 minIdx, maxIdx, mask.data);

    CV_OVX_RUN(!ovx::skipSmallImages<VX_KERNEL_MINMAXLOC>(src.cols, src.rows),
               openvx_minMaxIdx(src, minVal, maxVal, minIdx, maxIdx, mask))

    CV_IPP_RUN_FAST(ipp_minMaxIdx(src, minVal, maxVal, minIdx, maxIdx, mask))

    MinMaxIdxFunc func = getMinmaxTab(depth);
    CV_Assert( func != 0 );

    const Mat* arrays[] = {&src, &mask, 0};
    uchar* ptrs[2] = {};
    NAryMatIterator it(arrays, ptrs);

    size_t minidx = 0, maxidx = 0;
    int iminval = INT_MAX, imaxval = INT_MIN;
    float  fminval = std::numeric_limits<float>::infinity(),  fmaxval = -fminval;
    double dminval = std::numeric_limits<double>::infinity(), dmaxval = -dminval;
    size_t startidx = 1;
    int *minval = &iminval, *maxval = &imaxval;
    int planeSize = (int)it.size*cn;

    if( depth == CV_32F )
        minval = (int*)&fminval, maxval = (int*)&fmaxval;
    else if( depth == CV_64F )
        minval = (int*)&dminval, maxval = (int*)&dmaxval;

    for( size_t i = 0; i < it.nplanes; i++, ++it, startidx += planeSize )
        func( ptrs[0], ptrs[1], minval, maxval, &minidx, &maxidx, planeSize, startidx );

    if (!src.empty() && mask.empty())
    {
        if( minidx == 0 )
             minidx = 1;
         if( maxidx == 0 )
             maxidx = 1;
    }

    if( minidx == 0 )
        dminval = dmaxval = 0;
    else if( depth == CV_32F )
        dminval = fminval, dmaxval = fmaxval;
    else if( depth <= CV_32S )
        dminval = iminval, dmaxval = imaxval;

    if( minVal )
        *minVal = dminval;
    if( maxVal )
        *maxVal = dmaxval;

    if( minIdx )
        ofs2idx(src, minidx, minIdx);
    if( maxIdx )
        ofs2idx(src, maxidx, maxIdx);
}

void cv::minMaxLoc( InputArray _img, double* minVal, double* maxVal,
                    Point* minLoc, Point* maxLoc, InputArray mask )
{
    CV_INSTRUMENT_REGION();

    CV_Assert(_img.dims() <= 2);

    minMaxIdx(_img, minVal, maxVal, (int*)minLoc, (int*)maxLoc, mask);
    if( minLoc )
        std::swap(minLoc->x, minLoc->y);
    if( maxLoc )
        std::swap(maxLoc->x, maxLoc->y);
}
