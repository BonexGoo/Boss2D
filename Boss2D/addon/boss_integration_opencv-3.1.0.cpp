﻿#include <boss.h>
#if BOSS_NEED_ADDON_ALPR | BOSS_NEED_ADDON_OPENCV

#include <iostream>
#include <functional>
#include <cctype>

#include "boss_integration_opencv-3.1.0.h"

#undef LPSECURITY_ATTRIBUTES

#pragma warning(disable : 4297)
#define __OPENCV_BUILD
#define APIENTRY
#define GUID_DEFINED
#include <addon/opencv-3.1.0_for_boss/modules/core/include/opencv2/core/base.hpp>
#define CV_Assert(expr) BOSS_ASSERT_ARG("OpenCV 내부의 에러입니다", expr, __FILE__, __LINE__, __FUNCTION__)

#include <addon/opencv-3.1.0_for_boss/modules/core/src/algorithm.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/alloc.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/arithm.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/array.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/command_line_parser.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/conjugate_gradient.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/convert.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/copy.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/cuda_gpu_mat.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/cuda_host_mem.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/cuda_info.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/cuda_stream.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/datastructs.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/directx.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/downhill_simplex.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/dxt.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/gl_core_3_1.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/glob.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/kmeans.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/lapack.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/lda.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/lpsolver.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/mathfuncs.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/mathfuncs_core.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/matmul.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/matop.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/matrix.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/matrix_decomp.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/merge.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/ocl.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/opengl.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/out.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/parallel.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/parallel_pthreads.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/pca.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/rand.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/split.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/stat.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/stl.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/system.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/tables.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/types.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/umatrix.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/va_intel.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/core/src/persistence.cpp>

#undef __OPENCV_PRECOMP_H__
#include <addon/opencv-3.1.0_for_boss/modules/cudabgsegm/src/mog.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/cudabgsegm/src/mog2.cpp>

#undef __OPENCV_PRECOMP_H__
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/accum.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/approx.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/blend.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/canny.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/clahe.cpp>
#define BLOCK_SIZE BLOCK_SIZE_color_BOSS
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/color.cpp>
#undef BLOCK_SIZE
#define argsort argsort_color_BOSS
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/colormap.cpp>
#undef argsort
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/connectedcomponents.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/contours.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/convhull.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/corner.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/cornersubpix.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/demosaicing.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/deriv.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/distransform.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/drawing.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/emd.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/featureselect.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/filter.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/floodfill.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/gabor.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/generalized_hough.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/geometry.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/grabcut.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/hershey_fonts.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/histogram.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/hough.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/intersection.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/linefit.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/lsd.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/matchcontours.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/min_enclosing_triangle.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/moments.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/morph.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/phasecorr.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/pyramids.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/rotcalipers.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/samplers.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/segmentation.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/shapedescr.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/smooth.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/spatialgradient.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/subdivision2d.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/sumpixels.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/tables.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/templmatch.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/thresh.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/undistort.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgproc/src/utils.cpp>

#undef __OPENCV_PRECOMP_H__
#include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/bitstrm.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_base.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_bmp.cpp>
// 현재 본 옵션 사용안함 - #include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_exr.cpp>
// 현재 본 옵션 사용안함 - #include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_gdal.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_hdr.cpp>
// 현재 본 옵션 사용안함 - #include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_jpeg.cpp>
// 현재 본 옵션 사용안함 - #include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_jpeg2000.cpp>
// 현재 본 옵션 사용안함 - #include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_png.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_pxm.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_sunras.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_tiff.cpp>
// 현재 본 옵션 사용안함 - #include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/grfmt_webp.cpp>
// 현재 본 옵션 사용안함 - #include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/jpeg_exif.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/loadsave.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/rgbe.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/imgcodecs/src/utils.cpp>

#undef __OPENCV_PRECOMP_H__
#include <addon/opencv-3.1.0_for_boss/modules/highgui/src/window.cpp>
#undef __OPENCV_PRECOMP_H__
#include <addon/opencv-3.1.0_for_boss/modules/objdetect/src/cascadedetect.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/objdetect/src/haar.cpp>
#undef __OPENCV_PRECOMP_H__
#include <addon/opencv-3.1.0_for_boss/modules/features2d/src/feature2d.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/features2d/src/agast_score.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/features2d/src/agast.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/features2d/src/fast_score.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/features2d/src/fast.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/features2d/src/keypoint.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/features2d/src/matchers.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/features2d/src/brisk.cpp>
#include <addon/opencv-3.1.0_for_boss/modules/features2d/src/draw.cpp>
#undef __OPENCV_PRECOMP_H__
#include <addon/opencv-3.1.0_for_boss/modules/flann/src/miniflann.cpp>
#undef __OPENCV_PRECOMP_H__
#include <addon/opencv-3.1.0_for_boss/modules/video/src/bgfg_gaussmix2.cpp>
#define defaultHistory2 defaultHistory2_bgfg_KNN_BOSS
#define defaultnShadowDetection2 defaultnShadowDetection2_bgfg_KNN_BOSS
#define defaultfTau defaultfTau_bgfg_KNN_BOSS
#include <addon/opencv-3.1.0_for_boss/modules/video/src/bgfg_KNN.cpp>

// ■ 대체구현(addon/opencv-3.1.0_for_boss/modules/imgproc/src/imgwarp.cpp)
static inline int clip(int x, int a, int b)
{
    return x >= a ? (x < b ? x : b-1) : a;
}

struct DecimateAlpha
{
    int si, di;
    float alpha;
};

typedef void (*ResizeFunc)( const Mat& src, Mat& dst,
                            const int* xofs, const void* alpha,
                            const int* yofs, const void* beta,
                            int xmin, int xmax, int ksize );

typedef void (*ResizeAreaFastFunc)( const Mat& src, Mat& dst,
                                    const int* ofs, const int *xofs,
                                    int scale_x, int scale_y );

typedef void (*ResizeAreaFunc)( const Mat& src, Mat& dst,
                                const DecimateAlpha* xtab, int xtab_size,
                                const DecimateAlpha* ytab, int ytab_size,
                                const int* yofs );

static const int MAX_ESIZE=16;

template <typename HResize, typename VResize>
class resizeGeneric_Invoker :
    public ParallelLoopBody
{
public:
    typedef typename HResize::value_type T;
    typedef typename HResize::buf_type WT;
    typedef typename HResize::alpha_type AT;

    resizeGeneric_Invoker(const Mat& _src, Mat &_dst, const int *_xofs, const int *_yofs,
        const AT* _alpha, const AT* __beta, const Size& _ssize, const Size &_dsize,
        int _ksize, int _xmin, int _xmax) :
        ParallelLoopBody(), src(_src), dst(_dst), xofs(_xofs), yofs(_yofs),
        alpha(_alpha), _beta(__beta), ssize(_ssize), dsize(_dsize),
        ksize(_ksize), xmin(_xmin), xmax(_xmax)
    {
        CV_Assert(ksize <= MAX_ESIZE);
    }

#if defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ == 8)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Warray-bounds"
#endif
    virtual void operator() (const Range& range) const
    {
        int dy, cn = src.channels();
        HResize hresize;
        VResize vresize;

        int bufstep = (int)alignSize(dsize.width, 16);
        AutoBuffer<WT> _buffer(bufstep*ksize);
        const T* srows[MAX_ESIZE]={0};
        WT* rows[MAX_ESIZE]={0};
        int prev_sy[MAX_ESIZE];

        for(int k = 0; k < ksize; k++ )
        {
            prev_sy[k] = -1;
            rows[k] = (WT*)_buffer + bufstep*k;
        }

        const AT* beta = _beta + ksize * range.start;

        for( dy = range.start; dy < range.end; dy++, beta += ksize )
        {
            int sy0 = yofs[dy], k0=ksize, k1=0, ksize2 = ksize/2;

            for(int k = 0; k < ksize; k++ )
            {
                int sy = clip(sy0 - ksize2 + 1 + k, 0, ssize.height);
                for( k1 = std::max(k1, k); k1 < ksize; k1++ )
                {
                    if( sy == prev_sy[k1] ) // if the sy-th row has been computed already, reuse it.
                    {
                        if( k1 > k )
                            memcpy( rows[k], rows[k1], bufstep*sizeof(rows[0][0]) );
                        break;
                    }
                }
                if( k1 == ksize )
                    k0 = std::min(k0, k); // remember the first row that needs to be computed
                srows[k] = src.template ptr<T>(sy);
                prev_sy[k] = sy;
            }

            if( k0 < ksize )
                hresize( (const T**)(srows + k0), (WT**)(rows + k0), ksize - k0, xofs, (const AT*)(alpha),
                        ssize.width, dsize.width, cn, xmin, xmax );
            vresize( (const WT**)rows, (T*)(dst.data + dst.step*dy), beta, dsize.width );
        }
    }
#if defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ == 8)
# pragma GCC diagnostic pop
#endif

private:
    Mat src;
    Mat dst;
    const int* xofs, *yofs;
    const AT* alpha, *_beta;
    Size ssize, dsize;
    const int ksize, xmin, xmax;

    resizeGeneric_Invoker& operator = (const resizeGeneric_Invoker&);
};

template<class HResize, class VResize>
static void resizeGeneric_( const Mat& src, Mat& dst,
                            const int* xofs, const void* _alpha,
                            const int* yofs, const void* _beta,
                            int xmin, int xmax, int ksize )
{
    typedef typename HResize::alpha_type AT;

    const AT* beta = (const AT*)_beta;
    Size ssize = src.size(), dsize = dst.size();
    int cn = src.channels();
    ssize.width *= cn;
    dsize.width *= cn;
    xmin *= cn;
    xmax *= cn;
    // image resize is a separable operation. In case of not too strong

    Range range(0, dsize.height);
    resizeGeneric_Invoker<HResize, VResize> invoker(src, dst, xofs, yofs, (const AT*)_alpha, beta,
        ssize, dsize, ksize, xmin, xmax);
    parallel_for_(range, invoker, dst.total()/(double)(1<<16));
}

template <typename T, typename WT, typename VecOp>
class resizeAreaFast_Invoker :
    public ParallelLoopBody
{
public:
    resizeAreaFast_Invoker(const Mat &_src, Mat &_dst,
        int _scale_x, int _scale_y, const int* _ofs, const int* _xofs) :
        ParallelLoopBody(), src(_src), dst(_dst), scale_x(_scale_x),
        scale_y(_scale_y), ofs(_ofs), xofs(_xofs)
    {
    }

    virtual void operator() (const Range& range) const
    {
        Size ssize = src.size(), dsize = dst.size();
        int cn = src.channels();
        int area = scale_x*scale_y;
        float scale = 1.f/(area);
        int dwidth1 = (ssize.width/scale_x)*cn;
        dsize.width *= cn;
        ssize.width *= cn;
        int dy, dx, k = 0;

        VecOp vop(scale_x, scale_y, src.channels(), (int)src.step/*, area_ofs*/);

        for( dy = range.start; dy < range.end; dy++ )
        {
            T* D = (T*)(dst.data + dst.step*dy);
            int sy0 = dy*scale_y;
            int w = sy0 + scale_y <= ssize.height ? dwidth1 : 0;

            if( sy0 >= ssize.height )
            {
                for( dx = 0; dx < dsize.width; dx++ )
                    D[dx] = 0;
                continue;
            }

            dx = vop(src.template ptr<T>(sy0), D, w);
            for( ; dx < w; dx++ )
            {
                const T* S = src.template ptr<T>(sy0) + xofs[dx];
                WT sum = 0;
                k = 0;
                #if CV_ENABLE_UNROLLED
                for( ; k <= area - 4; k += 4 )
                    sum += S[ofs[k]] + S[ofs[k+1]] + S[ofs[k+2]] + S[ofs[k+3]];
                #endif
                for( ; k < area; k++ )
                    sum += S[ofs[k]];

                D[dx] = saturate_cast<T>(sum * scale);
            }

            for( ; dx < dsize.width; dx++ )
            {
                WT sum = 0;
                int count = 0, sx0 = xofs[dx];
                if( sx0 >= ssize.width )
                    D[dx] = 0;

                for( int sy = 0; sy < scale_y; sy++ )
                {
                    if( sy0 + sy >= ssize.height )
                        break;
                    const T* S = src.template ptr<T>(sy0 + sy) + sx0;
                    for( int sx = 0; sx < scale_x*cn; sx += cn )
                    {
                        if( sx0 + sx >= ssize.width )
                            break;
                        sum += S[sx];
                        count++;
                    }
                }

                D[dx] = saturate_cast<T>((float)sum/count);
            }
        }
    }

private:
    Mat src;
    Mat dst;
    int scale_x, scale_y;
    const int *ofs, *xofs;
};

template<typename T, typename WT> class ResizeArea_Invoker :
    public ParallelLoopBody
{
public:
    ResizeArea_Invoker( const Mat& _src, Mat& _dst,
                        const DecimateAlpha* _xtab, int _xtab_size,
                        const DecimateAlpha* _ytab, int _ytab_size,
                        const int* _tabofs )
    {
        src = &_src;
        dst = &_dst;
        xtab0 = _xtab;
        xtab_size0 = _xtab_size;
        ytab = _ytab;
        ytab_size = _ytab_size;
        tabofs = _tabofs;
    }

    virtual void operator() (const Range& range) const
    {
        Size dsize = dst->size();
        int cn = dst->channels();
        dsize.width *= cn;
        AutoBuffer<WT> _buffer(dsize.width*2);
        const DecimateAlpha* xtab = xtab0;
        int xtab_size = xtab_size0;
        WT *buf = _buffer, *sum = buf + dsize.width;
        int j_start = tabofs[range.start], j_end = tabofs[range.end], j, k, dx, prev_dy = ytab[j_start].di;

        for( dx = 0; dx < dsize.width; dx++ )
            sum[dx] = (WT)0;

        for( j = j_start; j < j_end; j++ )
        {
            WT beta = ytab[j].alpha;
            int dy = ytab[j].di;
            int sy = ytab[j].si;

            {
                const T* S = src->template ptr<T>(sy);
                for( dx = 0; dx < dsize.width; dx++ )
                    buf[dx] = (WT)0;

                if( cn == 1 )
                    for( k = 0; k < xtab_size; k++ )
                    {
                        int dxn = xtab[k].di;
                        WT alpha = xtab[k].alpha;
                        buf[dxn] += S[xtab[k].si]*alpha;
                    }
                else if( cn == 2 )
                    for( k = 0; k < xtab_size; k++ )
                    {
                        int sxn = xtab[k].si;
                        int dxn = xtab[k].di;
                        WT alpha = xtab[k].alpha;
                        WT t0 = buf[dxn] + S[sxn]*alpha;
                        WT t1 = buf[dxn+1] + S[sxn+1]*alpha;
                        buf[dxn] = t0; buf[dxn+1] = t1;
                    }
                else if( cn == 3 )
                    for( k = 0; k < xtab_size; k++ )
                    {
                        int sxn = xtab[k].si;
                        int dxn = xtab[k].di;
                        WT alpha = xtab[k].alpha;
                        WT t0 = buf[dxn] + S[sxn]*alpha;
                        WT t1 = buf[dxn+1] + S[sxn+1]*alpha;
                        WT t2 = buf[dxn+2] + S[sxn+2]*alpha;
                        buf[dxn] = t0; buf[dxn+1] = t1; buf[dxn+2] = t2;
                    }
                else if( cn == 4 )
                {
                    for( k = 0; k < xtab_size; k++ )
                    {
                        int sxn = xtab[k].si;
                        int dxn = xtab[k].di;
                        WT alpha = xtab[k].alpha;
                        WT t0 = buf[dxn] + S[sxn]*alpha;
                        WT t1 = buf[dxn+1] + S[sxn+1]*alpha;
                        buf[dxn] = t0; buf[dxn+1] = t1;
                        t0 = buf[dxn+2] + S[sxn+2]*alpha;
                        t1 = buf[dxn+3] + S[sxn+3]*alpha;
                        buf[dxn+2] = t0; buf[dxn+3] = t1;
                    }
                }
                else
                {
                    for( k = 0; k < xtab_size; k++ )
                    {
                        int sxn = xtab[k].si;
                        int dxn = xtab[k].di;
                        WT alpha = xtab[k].alpha;
                        for( int c = 0; c < cn; c++ )
                            buf[dxn + c] += S[sxn + c]*alpha;
                    }
                }
            }

            if( dy != prev_dy )
            {
                T* D = dst->template ptr<T>(prev_dy);

                for( dx = 0; dx < dsize.width; dx++ )
                {
                    D[dx] = saturate_cast<T>(sum[dx]);
                    sum[dx] = beta*buf[dx];
                }
                prev_dy = dy;
            }
            else
            {
                for( dx = 0; dx < dsize.width; dx++ )
                    sum[dx] += beta*buf[dx];
            }
        }

        {
        T* D = dst->template ptr<T>(prev_dy);
        for( dx = 0; dx < dsize.width; dx++ )
            D[dx] = saturate_cast<T>(sum[dx]);
        }
    }

private:
    const Mat* src;
    Mat* dst;
    const DecimateAlpha* xtab0;
    const DecimateAlpha* ytab;
    int xtab_size0, ytab_size;
    const int* tabofs;
};

template <typename T, typename WT>
static void resizeArea_( const Mat& src, Mat& dst,
                         const DecimateAlpha* xtab, int xtab_size,
                         const DecimateAlpha* ytab, int ytab_size,
                         const int* tabofs )
{
    parallel_for_(Range(0, dst.rows),
                 ResizeArea_Invoker<T, WT>(src, dst, xtab, xtab_size, ytab, ytab_size, tabofs),
                 dst.total()/((double)(1 << 16)));
}

template<typename T, typename WT, typename VecOp>
static void resizeAreaFast_( const Mat& src, Mat& dst, const int* ofs, const int* xofs,
                             int scale_x, int scale_y )
{
    Range range(0, dst.rows);
    resizeAreaFast_Invoker<T, WT, VecOp> invoker(src, dst, scale_x,
        scale_y, ofs, xofs);
    parallel_for_(range, invoker, dst.total()/(double)(1<<16));
}

class resizeNNInvoker :
    public ParallelLoopBody
{
public:
    resizeNNInvoker(const Mat& _src, Mat &_dst, int *_x_ofs, int _pix_size4, double _ify) :
        ParallelLoopBody(), src(_src), dst(_dst), x_ofs(_x_ofs), pix_size4(_pix_size4),
        ify(_ify)
    {
    }

    virtual void operator() (const Range& range) const
    {
        Size ssize = src.size(), dsize = dst.size();
        int y, x, pix_size = (int)src.elemSize();

        for( y = range.start; y < range.end; y++ )
        {
            uchar* D = dst.data + dst.step*y;
            int sy = std::min(cvFloor(y*ify), ssize.height-1);
            const uchar* S = src.ptr(sy);

            switch( pix_size )
            {
            case 1:
                for( x = 0; x <= dsize.width - 2; x += 2 )
                {
                    uchar t0 = S[x_ofs[x]];
                    uchar t1 = S[x_ofs[x+1]];
                    D[x] = t0;
                    D[x+1] = t1;
                }

                for( ; x < dsize.width; x++ )
                    D[x] = S[x_ofs[x]];
                break;
            case 2:
                for( x = 0; x < dsize.width; x++ )
                    *(ushort*)(D + x*2) = *(ushort*)(S + x_ofs[x]);
                break;
            case 3:
                for( x = 0; x < dsize.width; x++, D += 3 )
                {
                    const uchar* _tS = S + x_ofs[x];
                    D[0] = _tS[0]; D[1] = _tS[1]; D[2] = _tS[2];
                }
                break;
            case 4:
                for( x = 0; x < dsize.width; x++ )
                    *(int*)(D + x*4) = *(int*)(S + x_ofs[x]);
                break;
            case 6:
                for( x = 0; x < dsize.width; x++, D += 6 )
                {
                    const ushort* _tS = (const ushort*)(S + x_ofs[x]);
                    ushort* _tD = (ushort*)D;
                    _tD[0] = _tS[0]; _tD[1] = _tS[1]; _tD[2] = _tS[2];
                }
                break;
            case 8:
                for( x = 0; x < dsize.width; x++, D += 8 )
                {
                    const int* _tS = (const int*)(S + x_ofs[x]);
                    int* _tD = (int*)D;
                    _tD[0] = _tS[0]; _tD[1] = _tS[1];
                }
                break;
            case 12:
                for( x = 0; x < dsize.width; x++, D += 12 )
                {
                    const int* _tS = (const int*)(S + x_ofs[x]);
                    int* _tD = (int*)D;
                    _tD[0] = _tS[0]; _tD[1] = _tS[1]; _tD[2] = _tS[2];
                }
                break;
            default:
                for( x = 0; x < dsize.width; x++, D += pix_size )
                {
                    const int* _tS = (const int*)(S + x_ofs[x]);
                    int* _tD = (int*)D;
                    for( int k = 0; k < pix_size4; k++ )
                        _tD[k] = _tS[k];
                }
            }
        }
    }

private:
    const Mat src;
    Mat dst;
    int* x_ofs, pix_size4;
    double ify;

    resizeNNInvoker(const resizeNNInvoker&);
    resizeNNInvoker& operator=(const resizeNNInvoker&);
};

static void
resizeNN( const Mat& src, Mat& dst, double fx, double fy )
{
    Size ssize = src.size(), dsize = dst.size();
    AutoBuffer<int> _x_ofs(dsize.width);
    int* x_ofs = _x_ofs;
    int pix_size = (int)src.elemSize();
    int pix_size4 = (int)(pix_size / sizeof(int));
    double ifx = 1./fx, ify = 1./fy;
    int x;

    for( x = 0; x < dsize.width; x++ )
    {
        int sx = cvFloor(x*ifx);
        x_ofs[x] = std::min(sx, ssize.width-1)*pix_size;
    }

    Range range(0, dsize.height);
    resizeNNInvoker invoker(src, dst, x_ofs, pix_size4, ify);
    parallel_for_(range, invoker, dst.total()/(double)(1<<16));
}

static int computeResizeAreaTab( int ssize, int dsize, int cn, double scale, DecimateAlpha* tab )
{
    int k = 0;
    for(int dx = 0; dx < dsize; dx++ )
    {
        double fsx1 = dx * scale;
        double fsx2 = fsx1 + scale;
        double cellWidth = std::min(scale, ssize - fsx1);

        int sx1 = cvCeil(fsx1), sx2 = cvFloor(fsx2);

        sx2 = std::min(sx2, ssize - 1);
        sx1 = std::min(sx1, sx2);

        if( sx1 - fsx1 > 1e-3 )
        {
            assert( k < ssize*2 );
            tab[k].di = dx * cn;
            tab[k].si = (sx1 - 1) * cn;
            tab[k++].alpha = (float)((sx1 - fsx1) / cellWidth);
        }

        for(int sx = sx1; sx < sx2; sx++ )
        {
            assert( k < ssize*2 );
            tab[k].di = dx * cn;
            tab[k].si = sx * cn;
            tab[k++].alpha = float(1.0 / cellWidth);
        }

        if( fsx2 - sx2 > 1e-3 )
        {
            assert( k < ssize*2 );
            tab[k].di = dx * cn;
            tab[k].si = sx2 * cn;
            tab[k++].alpha = (float)(std::min(std::min(fsx2 - sx2, 1.), cellWidth) / cellWidth);
        }
    }
    return k;
}

static inline void interpolateLinear( float x, float* coeffs )
{
    coeffs[0] = 1.f - x;
    coeffs[1] = x;
}

static inline void interpolateCubic( float x, float* coeffs )
{
    const float A = -0.75f;

    coeffs[0] = ((A*(x + 1) - 5*A)*(x + 1) + 8*A)*(x + 1) - 4*A;
    coeffs[1] = ((A + 2)*x - (A + 3))*x*x + 1;
    coeffs[2] = ((A + 2)*(1 - x) - (A + 3))*(1 - x)*(1 - x) + 1;
    coeffs[3] = 1.f - coeffs[0] - coeffs[1] - coeffs[2];
}

static inline void interpolateLanczos4( float x, float* coeffs )
{
    static const double s45 = 0.70710678118654752440084436210485;
    static const double cs[][2]=
    {{1, 0}, {-s45, -s45}, {0, 1}, {s45, -s45}, {-1, 0}, {s45, s45}, {0, -1}, {-s45, s45}};

    if( x < FLT_EPSILON )
    {
        for( int i = 0; i < 8; i++ )
            coeffs[i] = 0;
        coeffs[3] = 1;
        return;
    }

    float sum = 0;
    double y0=-(x+3)*CV_PI*0.25, s0 = sin(y0), c0=cos(y0);
    for(int i = 0; i < 8; i++ )
    {
        double y = -(x+3-i)*CV_PI*0.25;
        coeffs[i] = (float)((cs[i][0]*s0 + cs[i][1]*c0)/(y*y));
        sum += coeffs[i];
    }

    sum = 1.f/sum;
    for(int i = 0; i < 8; i++ )
        coeffs[i] *= sum;
}

template<typename T, typename WT, typename AT, int ONE, class VecOp>
struct HResizeLinear
{
    typedef T value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const T** src, WT** dst, int count,
                    const int* xofs, const AT* alpha,
                    int swidth, int dwidth, int cn, int xmin, int xmax ) const
    {
        int dx, k;
        VecOp vecOp;

        int dx0 = vecOp((const uchar**)src, (uchar**)dst, count,
            xofs, (const uchar*)alpha, swidth, dwidth, cn, xmin, xmax );

        for( k = 0; k <= count - 2; k++ )
        {
            const T *S0 = src[k], *S1 = src[k+1];
            WT *D0 = dst[k], *D1 = dst[k+1];
            for( dx = dx0; dx < xmax; dx++ )
            {
                int sx = xofs[dx];
                WT a0 = alpha[dx*2], a1 = alpha[dx*2+1];
                WT t0 = S0[sx]*a0 + S0[sx + cn]*a1;
                WT t1 = S1[sx]*a0 + S1[sx + cn]*a1;
                D0[dx] = t0; D1[dx] = t1;
            }

            for( ; dx < dwidth; dx++ )
            {
                int sx = xofs[dx];
                D0[dx] = WT(S0[sx]*ONE); D1[dx] = WT(S1[sx]*ONE);
            }
        }

        for( ; k < count; k++ )
        {
            const T *S = src[k];
            WT *D = dst[k];
            for( dx = 0; dx < xmax; dx++ )
            {
                int sx = xofs[dx];
                D[dx] = S[sx]*alpha[dx*2] + S[sx+cn]*alpha[dx*2+1];
            }

            for( ; dx < dwidth; dx++ )
                D[dx] = WT(S[xofs[dx]]*ONE);
        }
    }
};

template<typename T, typename WT, typename AT, class CastOp, class VecOp>
struct VResizeLinear
{
    typedef T value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const WT** src, T* dst, const AT* beta, int width ) const
    {
        WT b0 = beta[0], b1 = beta[1];
        const WT *S0 = src[0], *S1 = src[1];
        CastOp castOp;
        VecOp vecOp;

        int x = vecOp((const uchar**)src, (uchar*)dst, (const uchar*)beta, width);
        #if CV_ENABLE_UNROLLED
        for( ; x <= width - 4; x += 4 )
        {
            WT t0, t1;
            t0 = S0[x]*b0 + S1[x]*b1;
            t1 = S0[x+1]*b0 + S1[x+1]*b1;
            dst[x] = castOp(t0); dst[x+1] = castOp(t1);
            t0 = S0[x+2]*b0 + S1[x+2]*b1;
            t1 = S0[x+3]*b0 + S1[x+3]*b1;
            dst[x+2] = castOp(t0); dst[x+3] = castOp(t1);
        }
        #endif
        for( ; x < width; x++ )
            dst[x] = castOp(S0[x]*b0 + S1[x]*b1);
    }
};

struct VResizeNoVec
{
    int operator()(const uchar**, uchar*, const uchar*, int ) const { return 0; }
};

struct HResizeNoVec
{
    int operator()(const uchar**, uchar**, int, const int*,
        const uchar*, int, int, int, int, int) const { return 0; }
};

template<typename T, typename WT, typename AT>
struct HResizeCubic
{
    typedef T value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const T** src, WT** dst, int count,
                    const int* xofs, const AT* alpha,
                    int swidth, int dwidth, int cn, int xmin, int xmax ) const
    {
        for( int k = 0; k < count; k++ )
        {
            const T *S = src[k];
            WT *D = dst[k];
            int dx = 0, limit = xmin;
            for(;;)
            {
                for( ; dx < limit; dx++, alpha += 4 )
                {
                    int j, sx = xofs[dx] - cn;
                    WT v = 0;
                    for( j = 0; j < 4; j++ )
                    {
                        int sxj = sx + j*cn;
                        if( (unsigned)sxj >= (unsigned)swidth )
                        {
                            while( sxj < 0 )
                                sxj += cn;
                            while( sxj >= swidth )
                                sxj -= cn;
                        }
                        v += S[sxj]*alpha[j];
                    }
                    D[dx] = v;
                }
                if( limit == dwidth )
                    break;
                for( ; dx < xmax; dx++, alpha += 4 )
                {
                    int sx = xofs[dx];
                    D[dx] = S[sx-cn]*alpha[0] + S[sx]*alpha[1] +
                        S[sx+cn]*alpha[2] + S[sx+cn*2]*alpha[3];
                }
                limit = dwidth;
            }
            alpha -= dwidth*4;
        }
    }
};

template<typename T, typename WT, typename AT, class CastOp, class VecOp>
struct VResizeCubic
{
    typedef T value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const WT** src, T* dst, const AT* beta, int width ) const
    {
        WT b0 = beta[0], b1 = beta[1], b2 = beta[2], b3 = beta[3];
        const WT *S0 = src[0], *S1 = src[1], *S2 = src[2], *S3 = src[3];
        CastOp castOp;
        VecOp vecOp;

        int x = vecOp((const uchar**)src, (uchar*)dst, (const uchar*)beta, width);
        for( ; x < width; x++ )
            dst[x] = castOp(S0[x]*b0 + S1[x]*b1 + S2[x]*b2 + S3[x]*b3);
    }
};

template<typename T, typename WT, typename AT>
struct HResizeLanczos4
{
    typedef T value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const T** src, WT** dst, int count,
                    const int* xofs, const AT* alpha,
                    int swidth, int dwidth, int cn, int xmin, int xmax ) const
    {
        for( int k = 0; k < count; k++ )
        {
            const T *S = src[k];
            WT *D = dst[k];
            int dx = 0, limit = xmin;
            for(;;)
            {
                for( ; dx < limit; dx++, alpha += 8 )
                {
                    int j, sx = xofs[dx] - cn*3;
                    WT v = 0;
                    for( j = 0; j < 8; j++ )
                    {
                        int sxj = sx + j*cn;
                        if( (unsigned)sxj >= (unsigned)swidth )
                        {
                            while( sxj < 0 )
                                sxj += cn;
                            while( sxj >= swidth )
                                sxj -= cn;
                        }
                        v += S[sxj]*alpha[j];
                    }
                    D[dx] = v;
                }
                if( limit == dwidth )
                    break;
                for( ; dx < xmax; dx++, alpha += 8 )
                {
                    int sx = xofs[dx];
                    D[dx] = S[sx-cn*3]*alpha[0] + S[sx-cn*2]*alpha[1] +
                        S[sx-cn]*alpha[2] + S[sx]*alpha[3] +
                        S[sx+cn]*alpha[4] + S[sx+cn*2]*alpha[5] +
                        S[sx+cn*3]*alpha[6] + S[sx+cn*4]*alpha[7];
                }
                limit = dwidth;
            }
            alpha -= dwidth*8;
        }
    }
};

template<typename T, typename WT, typename AT, class CastOp, class VecOp>
struct VResizeLanczos4
{
    typedef T value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const WT** src, T* dst, const AT* beta, int width ) const
    {
        CastOp castOp;
        VecOp vecOp;
        int k, x = vecOp((const uchar**)src, (uchar*)dst, (const uchar*)beta, width);
        #if CV_ENABLE_UNROLLED
        for( ; x <= width - 4; x += 4 )
        {
            WT b = beta[0];
            const WT* S = src[0];
            WT s0 = S[x]*b, s1 = S[x+1]*b, s2 = S[x+2]*b, s3 = S[x+3]*b;

            for( k = 1; k < 8; k++ )
            {
                b = beta[k]; S = src[k];
                s0 += S[x]*b; s1 += S[x+1]*b;
                s2 += S[x+2]*b; s3 += S[x+3]*b;
            }

            dst[x] = castOp(s0); dst[x+1] = castOp(s1);
            dst[x+2] = castOp(s2); dst[x+3] = castOp(s3);
        }
        #endif
        for( ; x < width; x++ )
        {
            dst[x] = castOp(src[0][x]*beta[0] + src[1][x]*beta[1] +
                src[2][x]*beta[2] + src[3][x]*beta[3] + src[4][x]*beta[4] +
                src[5][x]*beta[5] + src[6][x]*beta[6] + src[7][x]*beta[7]);
        }
    }
};

template<typename T, typename SIMDVecOp>
struct ResizeAreaFastVec
{
    ResizeAreaFastVec(int _scale_x, int _scale_y, int _cn, int _step) :
        scale_x(_scale_x), scale_y(_scale_y), cn(_cn), step(_step), vecOp(_cn, _step)
    {
        fast_mode = scale_x == 2 && scale_y == 2 && (cn == 1 || cn == 3 || cn == 4);
    }

    int operator() (const T* S, T* D, int w) const
    {
        if (!fast_mode)
            return 0;

        const T* nextS = (const T*)((const uchar*)S + step);
        int dx = vecOp(S, D, w);

        if (cn == 1)
            for( ; dx < w; ++dx )
            {
                int index = dx*2;
                D[dx] = (T)((S[index] + S[index+1] + nextS[index] + nextS[index+1] + 2) >> 2);
            }
        else if (cn == 3)
            for( ; dx < w; dx += 3 )
            {
                int index = dx*2;
                D[dx] = (T)((S[index] + S[index+3] + nextS[index] + nextS[index+3] + 2) >> 2);
                D[dx+1] = (T)((S[index+1] + S[index+4] + nextS[index+1] + nextS[index+4] + 2) >> 2);
                D[dx+2] = (T)((S[index+2] + S[index+5] + nextS[index+2] + nextS[index+5] + 2) >> 2);
            }
        else
            {
                CV_Assert(cn == 4);
                for( ; dx < w; dx += 4 )
                {
                    int index = dx*2;
                    D[dx] = (T)((S[index] + S[index+4] + nextS[index] + nextS[index+4] + 2) >> 2);
                    D[dx+1] = (T)((S[index+1] + S[index+5] + nextS[index+1] + nextS[index+5] + 2) >> 2);
                    D[dx+2] = (T)((S[index+2] + S[index+6] + nextS[index+2] + nextS[index+6] + 2) >> 2);
                    D[dx+3] = (T)((S[index+3] + S[index+7] + nextS[index+3] + nextS[index+7] + 2) >> 2);
                }
            }

        return dx;
    }

private:
    int scale_x, scale_y;
    int cn;
    bool fast_mode;
    int step;
    SIMDVecOp vecOp;
};

template <typename T, typename WT>
struct ResizeAreaFastNoVec
{
    ResizeAreaFastNoVec(int, int) { }
    ResizeAreaFastNoVec(int, int, int, int) { }
    int operator() (const T*, T*, int) const
    { return 0; }
};

typedef VResizeNoVec VResizeLinearVec_32s8u;
typedef VResizeNoVec VResizeLinearVec_32f16u;
typedef VResizeNoVec VResizeLinearVec_32f16s;
typedef VResizeNoVec VResizeLinearVec_32f;

typedef VResizeNoVec VResizeCubicVec_32s8u;
typedef VResizeNoVec VResizeCubicVec_32f16u;
typedef VResizeNoVec VResizeCubicVec_32f16s;
typedef VResizeNoVec VResizeCubicVec_32f;

typedef VResizeNoVec VResizeLanczos4Vec_32f16u;
typedef VResizeNoVec VResizeLanczos4Vec_32f16s;
typedef VResizeNoVec VResizeLanczos4Vec_32f;

typedef HResizeNoVec HResizeLinearVec_8u32s;
typedef HResizeNoVec HResizeLinearVec_16u32f;
typedef HResizeNoVec HResizeLinearVec_16s32f;
typedef HResizeNoVec HResizeLinearVec_32f;
typedef HResizeNoVec HResizeLinearVec_64f;

typedef ResizeAreaFastNoVec<uchar, uchar> ResizeAreaFastVec_SIMD_8u;
typedef ResizeAreaFastNoVec<ushort, ushort> ResizeAreaFastVec_SIMD_16u;
typedef ResizeAreaFastNoVec<short, short> ResizeAreaFastVec_SIMD_16s;
typedef ResizeAreaFastNoVec<float, float> ResizeAreaFastVec_SIMD_32f;

const int INTER_RESIZE_COEF_BITS=11;
const int INTER_RESIZE_COEF_SCALE=1 << INTER_RESIZE_COEF_BITS;

void cv::resize( InputArray _src, OutputArray _dst, Size dsize,
                 double inv_scale_x, double inv_scale_y, int interpolation )
{
    static ResizeFunc linear_tab[] =
    {
        resizeGeneric_<
            HResizeLinear<uchar, int, short,
                INTER_RESIZE_COEF_SCALE,
                HResizeLinearVec_8u32s>,
            VResizeLinear<uchar, int, short,
                FixedPtCast<int, uchar, INTER_RESIZE_COEF_BITS*2>,
                VResizeLinearVec_32s8u> >,
        0,
        resizeGeneric_<
            HResizeLinear<ushort, float, float, 1,
                HResizeLinearVec_16u32f>,
            VResizeLinear<ushort, float, float, Cast<float, ushort>,
                VResizeLinearVec_32f16u> >,
        resizeGeneric_<
            HResizeLinear<short, float, float, 1,
                HResizeLinearVec_16s32f>,
            VResizeLinear<short, float, float, Cast<float, short>,
                VResizeLinearVec_32f16s> >,
        0,
        resizeGeneric_<
            HResizeLinear<float, float, float, 1,
                HResizeLinearVec_32f>,
            VResizeLinear<float, float, float, Cast<float, float>,
                VResizeLinearVec_32f> >,
        resizeGeneric_<
            HResizeLinear<double, double, float, 1,
                HResizeNoVec>,
            VResizeLinear<double, double, float, Cast<double, double>,
                VResizeNoVec> >,
        0
    };

    static ResizeFunc cubic_tab[] =
    {
        resizeGeneric_<
            HResizeCubic<uchar, int, short>,
            VResizeCubic<uchar, int, short,
                FixedPtCast<int, uchar, INTER_RESIZE_COEF_BITS*2>,
                VResizeCubicVec_32s8u> >,
        0,
        resizeGeneric_<
            HResizeCubic<ushort, float, float>,
            VResizeCubic<ushort, float, float, Cast<float, ushort>,
            VResizeCubicVec_32f16u> >,
        resizeGeneric_<
            HResizeCubic<short, float, float>,
            VResizeCubic<short, float, float, Cast<float, short>,
            VResizeCubicVec_32f16s> >,
        0,
        resizeGeneric_<
            HResizeCubic<float, float, float>,
            VResizeCubic<float, float, float, Cast<float, float>,
            VResizeCubicVec_32f> >,
        resizeGeneric_<
            HResizeCubic<double, double, float>,
            VResizeCubic<double, double, float, Cast<double, double>,
            VResizeNoVec> >,
        0
    };

    static ResizeFunc lanczos4_tab[] =
    {
        resizeGeneric_<HResizeLanczos4<uchar, int, short>,
            VResizeLanczos4<uchar, int, short,
            FixedPtCast<int, uchar, INTER_RESIZE_COEF_BITS*2>,
            VResizeNoVec> >,
        0,
        resizeGeneric_<HResizeLanczos4<ushort, float, float>,
            VResizeLanczos4<ushort, float, float, Cast<float, ushort>,
            VResizeLanczos4Vec_32f16u> >,
        resizeGeneric_<HResizeLanczos4<short, float, float>,
            VResizeLanczos4<short, float, float, Cast<float, short>,
            VResizeLanczos4Vec_32f16s> >,
        0,
        resizeGeneric_<HResizeLanczos4<float, float, float>,
            VResizeLanczos4<float, float, float, Cast<float, float>,
            VResizeLanczos4Vec_32f> >,
        resizeGeneric_<HResizeLanczos4<double, double, float>,
            VResizeLanczos4<double, double, float, Cast<double, double>,
            VResizeNoVec> >,
        0
    };

    static ResizeAreaFastFunc areafast_tab[] =
    {
        resizeAreaFast_<uchar, int, ResizeAreaFastVec<uchar, ResizeAreaFastVec_SIMD_8u> >,
        0,
        resizeAreaFast_<ushort, float, ResizeAreaFastVec<ushort, ResizeAreaFastVec_SIMD_16u> >,
        resizeAreaFast_<short, float, ResizeAreaFastVec<short, ResizeAreaFastVec_SIMD_16s> >,
        0,
        resizeAreaFast_<float, float, ResizeAreaFastVec_SIMD_32f>,
        resizeAreaFast_<double, double, ResizeAreaFastNoVec<double, double> >,
        0
    };

    static ResizeAreaFunc area_tab[] =
    {
        resizeArea_<uchar, float>, 0, resizeArea_<ushort, float>,
        resizeArea_<short, float>, 0, resizeArea_<float, float>,
        resizeArea_<double, double>, 0
    };

    Size ssize = _src.size();

    CV_Assert( ssize.area() > 0 );
    CV_Assert( dsize.area() > 0 || (inv_scale_x > 0 && inv_scale_y > 0) );
    if( dsize.area() == 0 )
    {
        dsize = Size(saturate_cast<int>(ssize.width*inv_scale_x),
                     saturate_cast<int>(ssize.height*inv_scale_y));
        CV_Assert( dsize.area() > 0 );
    }
    else
    {
        inv_scale_x = (double)dsize.width/ssize.width;
        inv_scale_y = (double)dsize.height/ssize.height;
    }


    int type = _src.type(), depth = CV_MAT_DEPTH(type), cn = CV_MAT_CN(type);
    double scale_x = 1./inv_scale_x, scale_y = 1./inv_scale_y;

    int iscale_x = saturate_cast<int>(scale_x);
    int iscale_y = saturate_cast<int>(scale_y);

    bool is_area_fast = std::abs(scale_x - iscale_x) < DBL_EPSILON &&
            std::abs(scale_y - iscale_y) < DBL_EPSILON;


    CV_OCL_RUN(_src.dims() <= 2 && _dst.isUMat() && _src.cols() > 10 && _src.rows() > 10,
               ocl_resize(_src, _dst, dsize, inv_scale_x, inv_scale_y, interpolation))

    Mat src = _src.getMat();
    _dst.create(dsize, src.type());
    Mat dst = _dst.getMat();

    if (dsize == ssize) {
      // Source and destination are of same size. Use simple copy.
      src.copyTo(dst);
      return;
    }

#ifdef HAVE_TEGRA_OPTIMIZATION
    if (tegra::useTegra() && tegra::resize(src, dst, (float)inv_scale_x, (float)inv_scale_y, interpolation))
        return;
#endif

#ifdef HAVE_IPP
    int mode = -1;
    if (interpolation == INTER_LINEAR && _src.rows() >= 2 && _src.cols() >= 2)
        mode = INTER_LINEAR;
    else if (interpolation == INTER_CUBIC && _src.rows() >= 4 && _src.cols() >= 4)
        mode = INTER_CUBIC;

    const double IPP_RESIZE_EPS = 1e-10;
    double ex = fabs((double)dsize.width / _src.cols()  - inv_scale_x) / inv_scale_x;
    double ey = fabs((double)dsize.height / _src.rows() - inv_scale_y) / inv_scale_y;
#endif
    CV_IPP_RUN(IPP_VERSION_X100 >= 710 && ((ex < IPP_RESIZE_EPS && ey < IPP_RESIZE_EPS && depth != CV_64F) || (ex == 0 && ey == 0 && depth == CV_64F)) &&
        (interpolation == INTER_LINEAR || interpolation == INTER_CUBIC) &&
        !(interpolation == INTER_LINEAR && is_area_fast && iscale_x == 2 && iscale_y == 2 && depth == CV_8U) &&
        mode >= 0 && (cn == 1 || cn == 3 || cn == 4) && (depth == CV_16U || depth == CV_16S || depth == CV_32F ||
        (depth == CV_64F && mode == INTER_LINEAR)), ipp_resize_mt(src, dst, inv_scale_x, inv_scale_y, interpolation))


    if( interpolation == INTER_NEAREST )
    {
        resizeNN( src, dst, inv_scale_x, inv_scale_y );
        return;
    }

    int k, sx, sy, dx, dy;


    {
        // in case of scale_x && scale_y is equal to 2
        // INTER_AREA (fast) also is equal to INTER_LINEAR
        if( interpolation == INTER_LINEAR && is_area_fast && iscale_x == 2 && iscale_y == 2 )
            interpolation = INTER_AREA;

        // true "area" interpolation is only implemented for the case (scale_x <= 1 && scale_y <= 1).
        // In other cases it is emulated using some variant of bilinear interpolation
        if( interpolation == INTER_AREA && scale_x >= 1 && scale_y >= 1 )
        {
            if( is_area_fast )
            {
                int area = iscale_x*iscale_y;
                size_t srcstep = src.step / src.elemSize1();
                AutoBuffer<int> _ofs(area + dsize.width*cn);
                int* ofs = _ofs;
                int* xofs = ofs + area;
                ResizeAreaFastFunc func = areafast_tab[depth];
                CV_Assert( func != 0 );

                for( sy = 0, k = 0; sy < iscale_y; sy++ )
                    for( sx = 0; sx < iscale_x; sx++ )
                        ofs[k++] = (int)(sy*srcstep + sx*cn);

                for( dx = 0; dx < dsize.width; dx++ )
                {
                    int j = dx * cn;
                    sx = iscale_x * j;
                    for( k = 0; k < cn; k++ )
                        xofs[j + k] = sx + k;
                }

                func( src, dst, ofs, xofs, iscale_x, iscale_y );
                return;
            }

            ResizeAreaFunc func = area_tab[depth];
            CV_Assert( func != 0 && cn <= 4 );

            AutoBuffer<DecimateAlpha> _xytab((ssize.width + ssize.height)*2);
            DecimateAlpha* xtab = _xytab, *ytab = xtab + ssize.width*2;

            int xtab_size = computeResizeAreaTab(ssize.width, dsize.width, cn, scale_x, xtab);
            int ytab_size = computeResizeAreaTab(ssize.height, dsize.height, 1, scale_y, ytab);

            AutoBuffer<int> _tabofs(dsize.height + 1);
            int* tabofs = _tabofs;
            for( k = 0, dy = 0; k < ytab_size; k++ )
            {
                if( k == 0 || ytab[k].di != ytab[k-1].di )
                {
                    assert( ytab[k].di == dy );
                    tabofs[dy++] = k;
                }
            }
            tabofs[dy] = ytab_size;

            func( src, dst, xtab, xtab_size, ytab, ytab_size, tabofs );
            return;
        }
    }

    int xmin = 0, xmax = dsize.width, width = dsize.width*cn;
    bool area_mode = interpolation == INTER_AREA;
    bool fixpt = depth == CV_8U;
    float fx, fy;
    ResizeFunc func=0;
    int ksize=0, ksize2;
    if( interpolation == INTER_CUBIC )
        ksize = 4, func = cubic_tab[depth];
    else if( interpolation == INTER_LANCZOS4 )
        ksize = 8, func = lanczos4_tab[depth];
    else if( interpolation == INTER_LINEAR || interpolation == INTER_AREA )
        ksize = 2, func = linear_tab[depth];
    else
        CV_Error( CV_StsBadArg, "Unknown interpolation method" );
    ksize2 = ksize/2;

    CV_Assert( func != 0 );

    AutoBuffer<uchar> _buffer((width + dsize.height)*(sizeof(int) + sizeof(float)*ksize));
    int* xofs = (int*)(uchar*)_buffer;
    int* yofs = xofs + width;
    float* alpha = (float*)(yofs + dsize.height);
    short* ialpha = (short*)alpha;
    float* beta = alpha + width*ksize;
    short* ibeta = ialpha + width*ksize;
    float cbuf[MAX_ESIZE];

    for( dx = 0; dx < dsize.width; dx++ )
    {
        if( !area_mode )
        {
            fx = (float)((dx+0.5)*scale_x - 0.5);
            sx = cvFloor(fx);
            fx -= sx;
        }
        else
        {
            sx = cvFloor(dx*scale_x);
            fx = (float)((dx+1) - (sx+1)*inv_scale_x);
            fx = fx <= 0 ? 0.f : fx - cvFloor(fx);
        }

        if( sx < ksize2-1 )
        {
            xmin = dx+1;
            if( sx < 0 && (interpolation != INTER_CUBIC && interpolation != INTER_LANCZOS4))
                fx = 0, sx = 0;
        }

        if( sx + ksize2 >= ssize.width )
        {
            xmax = std::min( xmax, dx );
            if( sx >= ssize.width-1 && (interpolation != INTER_CUBIC && interpolation != INTER_LANCZOS4))
                fx = 0, sx = ssize.width-1;
        }

        for( k = 0, sx *= cn; k < cn; k++ )
            xofs[dx*cn + k] = sx + k;

        if( interpolation == INTER_CUBIC )
            interpolateCubic( fx, cbuf );
        else if( interpolation == INTER_LANCZOS4 )
            interpolateLanczos4( fx, cbuf );
        else
        {
            cbuf[0] = 1.f - fx;
            cbuf[1] = fx;
        }
        if( fixpt )
        {
            for( k = 0; k < ksize; k++ )
                ialpha[dx*cn*ksize + k] = saturate_cast<short>(cbuf[k]*INTER_RESIZE_COEF_SCALE);
            for( ; k < cn*ksize; k++ )
                ialpha[dx*cn*ksize + k] = ialpha[dx*cn*ksize + k - ksize];
        }
        else
        {
            for( k = 0; k < ksize; k++ )
                alpha[dx*cn*ksize + k] = cbuf[k];
            for( ; k < cn*ksize; k++ )
                alpha[dx*cn*ksize + k] = alpha[dx*cn*ksize + k - ksize];
        }
    }

    for( dy = 0; dy < dsize.height; dy++ )
    {
        if( !area_mode )
        {
            fy = (float)((dy+0.5)*scale_y - 0.5);
            sy = cvFloor(fy);
            fy -= sy;
        }
        else
        {
            sy = cvFloor(dy*scale_y);
            fy = (float)((dy+1) - (sy+1)*inv_scale_y);
            fy = fy <= 0 ? 0.f : fy - cvFloor(fy);
        }

        yofs[dy] = sy;
        if( interpolation == INTER_CUBIC )
            interpolateCubic( fy, cbuf );
        else if( interpolation == INTER_LANCZOS4 )
            interpolateLanczos4( fy, cbuf );
        else
        {
            cbuf[0] = 1.f - fy;
            cbuf[1] = fy;
        }

        if( fixpt )
        {
            for( k = 0; k < ksize; k++ )
                ibeta[dy*ksize + k] = saturate_cast<short>(cbuf[k]*INTER_RESIZE_COEF_SCALE);
        }
        else
        {
            for( k = 0; k < ksize; k++ )
                beta[dy*ksize + k] = cbuf[k];
        }
    }

    func( src, dst, xofs, fixpt ? (void*)ialpha : (void*)alpha, yofs,
          fixpt ? (void*)ibeta : (void*)beta, xmin, xmax, ksize );
}

static uchar NNDeltaTab_i[INTER_TAB_SIZE2][2];
static float BilinearTab_f[INTER_TAB_SIZE2][2][2];
static short BilinearTab_i[INTER_TAB_SIZE2][2][2];
static float BicubicTab_f[INTER_TAB_SIZE2][4][4];
static short BicubicTab_i[INTER_TAB_SIZE2][4][4];
static float Lanczos4Tab_f[INTER_TAB_SIZE2][8][8];
static short Lanczos4Tab_i[INTER_TAB_SIZE2][8][8];
#if CV_SSE2 || CV_NEON
    static short BilinearTab_iC4_buf[INTER_TAB_SIZE2+2][2][8];
    static short (*BilinearTab_iC4)[2][8] = (short (*)[2][8])alignPtr(BilinearTab_iC4_buf, 16);
#endif

typedef void (*RemapNNFunc)(const Mat& _src, Mat& _dst, const Mat& _xy,
                            int borderType, const Scalar& _borderValue );

typedef void (*RemapFunc)(const Mat& _src, Mat& _dst, const Mat& _xy,
                          const Mat& _fxy, const void* _wtab,
                          int borderType, const Scalar& _borderValue);

class RemapInvoker :
    public ParallelLoopBody
{
public:
    RemapInvoker(const Mat& _src, Mat& _dst, const Mat *_m1,
                 const Mat *_m2, int _borderType, const Scalar &_borderValue,
                 int _planar_input, RemapNNFunc _nnfunc, RemapFunc _ifunc, const void *_ctab) :
        ParallelLoopBody(), src(&_src), dst(&_dst), m1(_m1), m2(_m2),
        borderType(_borderType), borderValue(_borderValue),
        planar_input(_planar_input), nnfunc(_nnfunc), ifunc(_ifunc), ctab(_ctab)
    {
    }

    virtual void operator() (const Range& range) const
    {
        int x, y, x1, y1;
        const int buf_size = 1 << 14;
        int brows0 = std::min(128, dst->rows), map_depth = m1->depth();
        int bcols0 = std::min(buf_size/brows0, dst->cols);
        brows0 = std::min(buf_size/bcols0, dst->rows);
    #if CV_SSE2
        bool useSIMD = checkHardwareSupport(CV_CPU_SSE2);
    #endif

        Mat _bufxy(brows0, bcols0, CV_16SC2), _bufa;
        if( !nnfunc )
            _bufa.create(brows0, bcols0, CV_16UC1);

        for( y = range.start; y < range.end; y += brows0 )
        {
            for( x = 0; x < dst->cols; x += bcols0 )
            {
                int brows = std::min(brows0, range.end - y);
                int bcols = std::min(bcols0, dst->cols - x);
                Mat dpart(*dst, Rect(x, y, bcols, brows));
                Mat bufxy(_bufxy, Rect(0, 0, bcols, brows));

                if( nnfunc )
                {
                    if( m1->type() == CV_16SC2 && m2->empty() ) // the data is already in the right format
                        bufxy = (*m1)(Rect(x, y, bcols, brows));
                    else if( map_depth != CV_32F )
                    {
                        for( y1 = 0; y1 < brows; y1++ )
                        {
                            short* XY = bufxy.ptr<short>(y1);
                            const short* sXY = m1->ptr<short>(y+y1) + x*2;
                            const ushort* sA = m2->ptr<ushort>(y+y1) + x;

                            for( x1 = 0; x1 < bcols; x1++ )
                            {
                                int a = sA[x1] & (INTER_TAB_SIZE2-1);
                                XY[x1*2] = sXY[x1*2] + NNDeltaTab_i[a][0];
                                XY[x1*2+1] = sXY[x1*2+1] + NNDeltaTab_i[a][1];
                            }
                        }
                    }
                    else if( !planar_input )
                        (*m1)(Rect(x, y, bcols, brows)).convertTo(bufxy, bufxy.depth());
                    else
                    {
                        for( y1 = 0; y1 < brows; y1++ )
                        {
                            short* XY = bufxy.ptr<short>(y1);
                            const float* sX = m1->ptr<float>(y+y1) + x;
                            const float* sY = m2->ptr<float>(y+y1) + x;
                            x1 = 0;

                        #if CV_SSE2
                            if( useSIMD )
                            {
                                for( ; x1 <= bcols - 8; x1 += 8 )
                                {
                                    __m128 fx0 = _mm_loadu_ps(sX + x1);
                                    __m128 fx1 = _mm_loadu_ps(sX + x1 + 4);
                                    __m128 fy0 = _mm_loadu_ps(sY + x1);
                                    __m128 fy1 = _mm_loadu_ps(sY + x1 + 4);
                                    __m128i ix0 = _mm_cvtps_epi32(fx0);
                                    __m128i ix1 = _mm_cvtps_epi32(fx1);
                                    __m128i iy0 = _mm_cvtps_epi32(fy0);
                                    __m128i iy1 = _mm_cvtps_epi32(fy1);
                                    ix0 = _mm_packs_epi32(ix0, ix1);
                                    iy0 = _mm_packs_epi32(iy0, iy1);
                                    ix1 = _mm_unpacklo_epi16(ix0, iy0);
                                    iy1 = _mm_unpackhi_epi16(ix0, iy0);
                                    _mm_storeu_si128((__m128i*)(XY + x1*2), ix1);
                                    _mm_storeu_si128((__m128i*)(XY + x1*2 + 8), iy1);
                                }
                            }
                        #endif

                            for( ; x1 < bcols; x1++ )
                            {
                                XY[x1*2] = saturate_cast<short>(sX[x1]);
                                XY[x1*2+1] = saturate_cast<short>(sY[x1]);
                            }
                        }
                    }
                    nnfunc( *src, dpart, bufxy, borderType, borderValue );
                    continue;
                }

                Mat bufa(_bufa, Rect(0, 0, bcols, brows));
                for( y1 = 0; y1 < brows; y1++ )
                {
                    short* XY = bufxy.ptr<short>(y1);
                    ushort* A = bufa.ptr<ushort>(y1);

                    if( m1->type() == CV_16SC2 && (m2->type() == CV_16UC1 || m2->type() == CV_16SC1) )
                    {
                        bufxy = (*m1)(Rect(x, y, bcols, brows));

                        const ushort* sA = m2->ptr<ushort>(y+y1) + x;
                        x1 = 0;

                    #if CV_NEON
                        uint16x8_t v_scale = vdupq_n_u16(INTER_TAB_SIZE2-1);
                        for ( ; x1 <= bcols - 8; x1 += 8)
                            vst1q_u16(A + x1, vandq_u16(vld1q_u16(sA + x1), v_scale));
                    #elif CV_SSE2
                        __m128i v_scale = _mm_set1_epi16(INTER_TAB_SIZE2-1);
                        for ( ; x1 <= bcols - 8; x1 += 8)
                            _mm_storeu_si128((__m128i *)(A + x1), _mm_and_si128(_mm_loadu_si128((const __m128i *)(sA + x1)), v_scale));
                    #endif

                        for( ; x1 < bcols; x1++ )
                            A[x1] = (ushort)(sA[x1] & (INTER_TAB_SIZE2-1));
                    }
                    else if( planar_input )
                    {
                        const float* sX = m1->ptr<float>(y+y1) + x;
                        const float* sY = m2->ptr<float>(y+y1) + x;

                        x1 = 0;
                    #if CV_SSE2
                        if( useSIMD )
                        {
                            __m128 scale = _mm_set1_ps((float)INTER_TAB_SIZE);
                            __m128i mask = _mm_set1_epi32(INTER_TAB_SIZE-1);
                            for( ; x1 <= bcols - 8; x1 += 8 )
                            {
                                __m128 fx0 = _mm_loadu_ps(sX + x1);
                                __m128 fx1 = _mm_loadu_ps(sX + x1 + 4);
                                __m128 fy0 = _mm_loadu_ps(sY + x1);
                                __m128 fy1 = _mm_loadu_ps(sY + x1 + 4);
                                __m128i ix0 = _mm_cvtps_epi32(_mm_mul_ps(fx0, scale));
                                __m128i ix1 = _mm_cvtps_epi32(_mm_mul_ps(fx1, scale));
                                __m128i iy0 = _mm_cvtps_epi32(_mm_mul_ps(fy0, scale));
                                __m128i iy1 = _mm_cvtps_epi32(_mm_mul_ps(fy1, scale));
                                __m128i mx0 = _mm_and_si128(ix0, mask);
                                __m128i mx1 = _mm_and_si128(ix1, mask);
                                __m128i my0 = _mm_and_si128(iy0, mask);
                                __m128i my1 = _mm_and_si128(iy1, mask);
                                mx0 = _mm_packs_epi32(mx0, mx1);
                                my0 = _mm_packs_epi32(my0, my1);
                                my0 = _mm_slli_epi16(my0, INTER_BITS);
                                mx0 = _mm_or_si128(mx0, my0);
                                _mm_storeu_si128((__m128i*)(A + x1), mx0);
                                ix0 = _mm_srai_epi32(ix0, INTER_BITS);
                                ix1 = _mm_srai_epi32(ix1, INTER_BITS);
                                iy0 = _mm_srai_epi32(iy0, INTER_BITS);
                                iy1 = _mm_srai_epi32(iy1, INTER_BITS);
                                ix0 = _mm_packs_epi32(ix0, ix1);
                                iy0 = _mm_packs_epi32(iy0, iy1);
                                ix1 = _mm_unpacklo_epi16(ix0, iy0);
                                iy1 = _mm_unpackhi_epi16(ix0, iy0);
                                _mm_storeu_si128((__m128i*)(XY + x1*2), ix1);
                                _mm_storeu_si128((__m128i*)(XY + x1*2 + 8), iy1);
                            }
                        }
                    #elif CV_NEON
                        float32x4_t v_scale = vdupq_n_f32((float)INTER_TAB_SIZE);
                        int32x4_t v_scale2 = vdupq_n_s32(INTER_TAB_SIZE - 1), v_scale3 = vdupq_n_s32(INTER_TAB_SIZE);

                        for( ; x1 <= bcols - 4; x1 += 4 )
                        {
                            int32x4_t v_sx = cv_vrndq_s32_f32(vmulq_f32(vld1q_f32(sX + x1), v_scale)),
                                      v_sy = cv_vrndq_s32_f32(vmulq_f32(vld1q_f32(sY + x1), v_scale));
                            int32x4_t v_v = vmlaq_s32(vandq_s32(v_sx, v_scale2), v_scale3,
                                                      vandq_s32(v_sy, v_scale2));
                            vst1_u16(A + x1, vqmovun_s32(v_v));

                            int16x4x2_t v_dst = vzip_s16(vqmovn_s32(vshrq_n_s32(v_sx, INTER_BITS)),
                                                         vqmovn_s32(vshrq_n_s32(v_sy, INTER_BITS)));
                            vst1q_s16(XY + (x1 << 1), vcombine_s16(v_dst.val[0], v_dst.val[1]));
                        }
                    #endif

                        for( ; x1 < bcols; x1++ )
                        {
                            int sx = cvRound(sX[x1]*INTER_TAB_SIZE);
                            int sy = cvRound(sY[x1]*INTER_TAB_SIZE);
                            int v = (sy & (INTER_TAB_SIZE-1))*INTER_TAB_SIZE + (sx & (INTER_TAB_SIZE-1));
                            XY[x1*2] = saturate_cast<short>(sx >> INTER_BITS);
                            XY[x1*2+1] = saturate_cast<short>(sy >> INTER_BITS);
                            A[x1] = (ushort)v;
                        }
                    }
                    else
                    {
                        const float* sXY = m1->ptr<float>(y+y1) + x*2;
                        x1 = 0;

                    #if CV_NEON
                        float32x4_t v_scale = vdupq_n_f32(INTER_TAB_SIZE);
                        int32x4_t v_scale2 = vdupq_n_s32(INTER_TAB_SIZE-1), v_scale3 = vdupq_n_s32(INTER_TAB_SIZE);

                        for( ; x1 <= bcols - 4; x1 += 4 )
                        {
                            float32x4x2_t v_src = vld2q_f32(sXY + (x1 << 1));
                            int32x4_t v_sx = cv_vrndq_s32_f32(vmulq_f32(v_src.val[0], v_scale));
                            int32x4_t v_sy = cv_vrndq_s32_f32(vmulq_f32(v_src.val[1], v_scale));
                            int32x4_t v_v = vmlaq_s32(vandq_s32(v_sx, v_scale2), v_scale3,
                                                      vandq_s32(v_sy, v_scale2));
                            vst1_u16(A + x1, vqmovun_s32(v_v));

                            int16x4x2_t v_dst = vzip_s16(vqmovn_s32(vshrq_n_s32(v_sx, INTER_BITS)),
                                                         vqmovn_s32(vshrq_n_s32(v_sy, INTER_BITS)));
                            vst1q_s16(XY + (x1 << 1), vcombine_s16(v_dst.val[0], v_dst.val[1]));
                        }
                    #endif

                        for( x1 = 0; x1 < bcols; x1++ )
                        {
                            int sx = cvRound(sXY[x1*2]*INTER_TAB_SIZE);
                            int sy = cvRound(sXY[x1*2+1]*INTER_TAB_SIZE);
                            int v = (sy & (INTER_TAB_SIZE-1))*INTER_TAB_SIZE + (sx & (INTER_TAB_SIZE-1));
                            XY[x1*2] = saturate_cast<short>(sx >> INTER_BITS);
                            XY[x1*2+1] = saturate_cast<short>(sy >> INTER_BITS);
                            A[x1] = (ushort)v;
                        }
                    }
                }
                ifunc(*src, dpart, bufxy, bufa, ctab, borderType, borderValue);
            }
        }
    }

private:
    const Mat* src;
    Mat* dst;
    const Mat *m1, *m2;
    int borderType;
    Scalar borderValue;
    int planar_input;
    RemapNNFunc nnfunc;
    RemapFunc ifunc;
    const void *ctab;
};

template<typename T>
static void remapNearest( const Mat& _src, Mat& _dst, const Mat& _xy,
                          int borderType, const Scalar& _borderValue )
{
    Size ssize = _src.size(), dsize = _dst.size();
    int cn = _src.channels();
    const T* S0 = _src.ptr<T>();
    size_t sstep = _src.step/sizeof(S0[0]);
    Scalar_<T> cval(saturate_cast<T>(_borderValue[0]),
        saturate_cast<T>(_borderValue[1]),
        saturate_cast<T>(_borderValue[2]),
        saturate_cast<T>(_borderValue[3]));
    int dx, dy;

    unsigned width1 = ssize.width, height1 = ssize.height;

    if( _dst.isContinuous() && _xy.isContinuous() )
    {
        dsize.width *= dsize.height;
        dsize.height = 1;
    }

    for( dy = 0; dy < dsize.height; dy++ )
    {
        T* D = _dst.ptr<T>(dy);
        const short* XY = _xy.ptr<short>(dy);

        if( cn == 1 )
        {
            for( dx = 0; dx < dsize.width; dx++ )
            {
                int sx = XY[dx*2], sy = XY[dx*2+1];
                if( (unsigned)sx < width1 && (unsigned)sy < height1 )
                    D[dx] = S0[sy*sstep + sx];
                else
                {
                    if( borderType == BORDER_REPLICATE )
                    {
                        sx = clip(sx, 0, ssize.width);
                        sy = clip(sy, 0, ssize.height);
                        D[dx] = S0[sy*sstep + sx];
                    }
                    else if( borderType == BORDER_CONSTANT )
                        D[dx] = cval[0];
                    else if( borderType != BORDER_TRANSPARENT )
                    {
                        sx = borderInterpolate(sx, ssize.width, borderType);
                        sy = borderInterpolate(sy, ssize.height, borderType);
                        D[dx] = S0[sy*sstep + sx];
                    }
                }
            }
        }
        else
        {
            for( dx = 0; dx < dsize.width; dx++, D += cn )
            {
                int sx = XY[dx*2], sy = XY[dx*2+1], k;
                const T *S;
                if( (unsigned)sx < width1 && (unsigned)sy < height1 )
                {
                    if( cn == 3 )
                    {
                        S = S0 + sy*sstep + sx*3;
                        D[0] = S[0], D[1] = S[1], D[2] = S[2];
                    }
                    else if( cn == 4 )
                    {
                        S = S0 + sy*sstep + sx*4;
                        D[0] = S[0], D[1] = S[1], D[2] = S[2], D[3] = S[3];
                    }
                    else
                    {
                        S = S0 + sy*sstep + sx*cn;
                        for( k = 0; k < cn; k++ )
                            D[k] = S[k];
                    }
                }
                else if( borderType != BORDER_TRANSPARENT )
                {
                    if( borderType == BORDER_REPLICATE )
                    {
                        sx = clip(sx, 0, ssize.width);
                        sy = clip(sy, 0, ssize.height);
                        S = S0 + sy*sstep + sx*cn;
                    }
                    else if( borderType == BORDER_CONSTANT )
                        S = &cval[0];
                    else
                    {
                        sx = borderInterpolate(sx, ssize.width, borderType);
                        sy = borderInterpolate(sy, ssize.height, borderType);
                        S = S0 + sy*sstep + sx*cn;
                    }
                    for( k = 0; k < cn; k++ )
                        D[k] = S[k];
                }
            }
        }
    }
}

template<class CastOp, class VecOp, typename AT>
static void remapBilinear( const Mat& _src, Mat& _dst, const Mat& _xy,
                           const Mat& _fxy, const void* _wtab,
                           int borderType, const Scalar& _borderValue )
{
    typedef typename CastOp::rtype T;
    typedef typename CastOp::type1 WT;
    Size ssize = _src.size(), dsize = _dst.size();
    int k, cn = _src.channels();
    const AT* wtab = (const AT*)_wtab;
    const T* S0 = _src.ptr<T>();
    size_t sstep = _src.step/sizeof(S0[0]);
    T cval[CV_CN_MAX];
    int dx, dy;
    CastOp castOp;
    VecOp vecOp;

    for( k = 0; k < cn; k++ )
        cval[k] = saturate_cast<T>(_borderValue[k & 3]);

    unsigned width1 = std::max(ssize.width-1, 0), height1 = std::max(ssize.height-1, 0);
    CV_Assert( ssize.area() > 0 );
#if CV_SSE2
    if( _src.type() == CV_8UC3 )
        width1 = std::max(ssize.width-2, 0);
#endif

    for( dy = 0; dy < dsize.height; dy++ )
    {
        T* D = _dst.ptr<T>(dy);
        const short* XY = _xy.ptr<short>(dy);
        const ushort* FXY = _fxy.ptr<ushort>(dy);
        int X0 = 0;
        bool prevInlier = false;

        for( dx = 0; dx <= dsize.width; dx++ )
        {
            bool curInlier = dx < dsize.width ?
                (unsigned)XY[dx*2] < width1 &&
                (unsigned)XY[dx*2+1] < height1 : !prevInlier;
            if( curInlier == prevInlier )
                continue;

            int X1 = dx;
            dx = X0;
            X0 = X1;
            prevInlier = curInlier;

            if( !curInlier )
            {
                int len = vecOp( _src, D, XY + dx*2, FXY + dx, wtab, X1 - dx );
                D += len*cn;
                dx += len;

                if( cn == 1 )
                {
                    for( ; dx < X1; dx++, D++ )
                    {
                        int sx = XY[dx*2], sy = XY[dx*2+1];
                        const AT* w = wtab + FXY[dx]*4;
                        const T* S = S0 + sy*sstep + sx;
                        *D = castOp(WT(S[0]*w[0] + S[1]*w[1] + S[sstep]*w[2] + S[sstep+1]*w[3]));
                    }
                }
                else if( cn == 2 )
                    for( ; dx < X1; dx++, D += 2 )
                    {
                        int sx = XY[dx*2], sy = XY[dx*2+1];
                        const AT* w = wtab + FXY[dx]*4;
                        const T* S = S0 + sy*sstep + sx*2;
                        WT t0 = S[0]*w[0] + S[2]*w[1] + S[sstep]*w[2] + S[sstep+2]*w[3];
                        WT t1 = S[1]*w[0] + S[3]*w[1] + S[sstep+1]*w[2] + S[sstep+3]*w[3];
                        D[0] = castOp(t0); D[1] = castOp(t1);
                    }
                else if( cn == 3 )
                    for( ; dx < X1; dx++, D += 3 )
                    {
                        int sx = XY[dx*2], sy = XY[dx*2+1];
                        const AT* w = wtab + FXY[dx]*4;
                        const T* S = S0 + sy*sstep + sx*3;
                        WT t0 = S[0]*w[0] + S[3]*w[1] + S[sstep]*w[2] + S[sstep+3]*w[3];
                        WT t1 = S[1]*w[0] + S[4]*w[1] + S[sstep+1]*w[2] + S[sstep+4]*w[3];
                        WT t2 = S[2]*w[0] + S[5]*w[1] + S[sstep+2]*w[2] + S[sstep+5]*w[3];
                        D[0] = castOp(t0); D[1] = castOp(t1); D[2] = castOp(t2);
                    }
                else if( cn == 4 )
                    for( ; dx < X1; dx++, D += 4 )
                    {
                        int sx = XY[dx*2], sy = XY[dx*2+1];
                        const AT* w = wtab + FXY[dx]*4;
                        const T* S = S0 + sy*sstep + sx*4;
                        WT t0 = S[0]*w[0] + S[4]*w[1] + S[sstep]*w[2] + S[sstep+4]*w[3];
                        WT t1 = S[1]*w[0] + S[5]*w[1] + S[sstep+1]*w[2] + S[sstep+5]*w[3];
                        D[0] = castOp(t0); D[1] = castOp(t1);
                        t0 = S[2]*w[0] + S[6]*w[1] + S[sstep+2]*w[2] + S[sstep+6]*w[3];
                        t1 = S[3]*w[0] + S[7]*w[1] + S[sstep+3]*w[2] + S[sstep+7]*w[3];
                        D[2] = castOp(t0); D[3] = castOp(t1);
                    }
                else
                    for( ; dx < X1; dx++, D += cn )
                    {
                        int sx = XY[dx*2], sy = XY[dx*2+1];
                        const AT* w = wtab + FXY[dx]*4;
                        const T* S = S0 + sy*sstep + sx*cn;
                        for( k = 0; k < cn; k++ )
                        {
                            WT t0 = S[k]*w[0] + S[k+cn]*w[1] + S[sstep+k]*w[2] + S[sstep+k+cn]*w[3];
                            D[k] = castOp(t0);
                        }
                    }
            }
            else
            {
                if( borderType == BORDER_TRANSPARENT && cn != 3 )
                {
                    D += (X1 - dx)*cn;
                    dx = X1;
                    continue;
                }

                if( cn == 1 )
                    for( ; dx < X1; dx++, D++ )
                    {
                        int sx = XY[dx*2], sy = XY[dx*2+1];
                        if( borderType == BORDER_CONSTANT &&
                            (sx >= ssize.width || sx+1 < 0 ||
                             sy >= ssize.height || sy+1 < 0) )
                        {
                            D[0] = cval[0];
                        }
                        else
                        {
                            int sx0, sx1, sy0, sy1;
                            T v0, v1, v2, v3;
                            const AT* w = wtab + FXY[dx]*4;
                            if( borderType == BORDER_REPLICATE )
                            {
                                sx0 = clip(sx, 0, ssize.width);
                                sx1 = clip(sx+1, 0, ssize.width);
                                sy0 = clip(sy, 0, ssize.height);
                                sy1 = clip(sy+1, 0, ssize.height);
                                v0 = S0[sy0*sstep + sx0];
                                v1 = S0[sy0*sstep + sx1];
                                v2 = S0[sy1*sstep + sx0];
                                v3 = S0[sy1*sstep + sx1];
                            }
                            else
                            {
                                sx0 = borderInterpolate(sx, ssize.width, borderType);
                                sx1 = borderInterpolate(sx+1, ssize.width, borderType);
                                sy0 = borderInterpolate(sy, ssize.height, borderType);
                                sy1 = borderInterpolate(sy+1, ssize.height, borderType);
                                v0 = sx0 >= 0 && sy0 >= 0 ? S0[sy0*sstep + sx0] : cval[0];
                                v1 = sx1 >= 0 && sy0 >= 0 ? S0[sy0*sstep + sx1] : cval[0];
                                v2 = sx0 >= 0 && sy1 >= 0 ? S0[sy1*sstep + sx0] : cval[0];
                                v3 = sx1 >= 0 && sy1 >= 0 ? S0[sy1*sstep + sx1] : cval[0];
                            }
                            D[0] = castOp(WT(v0*w[0] + v1*w[1] + v2*w[2] + v3*w[3]));
                        }
                    }
                else
                    for( ; dx < X1; dx++, D += cn )
                    {
                        int sx = XY[dx*2], sy = XY[dx*2+1];
                        if( borderType == BORDER_CONSTANT &&
                            (sx >= ssize.width || sx+1 < 0 ||
                             sy >= ssize.height || sy+1 < 0) )
                        {
                            for( k = 0; k < cn; k++ )
                                D[k] = cval[k];
                        }
                        else
                        {
                            int sx0, sx1, sy0, sy1;
                            const T *v0, *v1, *v2, *v3;
                            const AT* w = wtab + FXY[dx]*4;
                            if( borderType == BORDER_REPLICATE )
                            {
                                sx0 = clip(sx, 0, ssize.width);
                                sx1 = clip(sx+1, 0, ssize.width);
                                sy0 = clip(sy, 0, ssize.height);
                                sy1 = clip(sy+1, 0, ssize.height);
                                v0 = S0 + sy0*sstep + sx0*cn;
                                v1 = S0 + sy0*sstep + sx1*cn;
                                v2 = S0 + sy1*sstep + sx0*cn;
                                v3 = S0 + sy1*sstep + sx1*cn;
                            }
                            else if( borderType == BORDER_TRANSPARENT &&
                                ((unsigned)sx >= (unsigned)(ssize.width-1) ||
                                (unsigned)sy >= (unsigned)(ssize.height-1)))
                                continue;
                            else
                            {
                                sx0 = borderInterpolate(sx, ssize.width, borderType);
                                sx1 = borderInterpolate(sx+1, ssize.width, borderType);
                                sy0 = borderInterpolate(sy, ssize.height, borderType);
                                sy1 = borderInterpolate(sy+1, ssize.height, borderType);
                                v0 = sx0 >= 0 && sy0 >= 0 ? S0 + sy0*sstep + sx0*cn : &cval[0];
                                v1 = sx1 >= 0 && sy0 >= 0 ? S0 + sy0*sstep + sx1*cn : &cval[0];
                                v2 = sx0 >= 0 && sy1 >= 0 ? S0 + sy1*sstep + sx0*cn : &cval[0];
                                v3 = sx1 >= 0 && sy1 >= 0 ? S0 + sy1*sstep + sx1*cn : &cval[0];
                            }
                            for( k = 0; k < cn; k++ )
                                D[k] = castOp(WT(v0[k]*w[0] + v1[k]*w[1] + v2[k]*w[2] + v3[k]*w[3]));
                        }
                    }
            }
        }
    }
}

template<class CastOp, typename AT, int ONE>
static void remapBicubic( const Mat& _src, Mat& _dst, const Mat& _xy,
                          const Mat& _fxy, const void* _wtab,
                          int borderType, const Scalar& _borderValue )
{
    typedef typename CastOp::rtype T;
    typedef typename CastOp::type1 WT;
    Size ssize = _src.size(), dsize = _dst.size();
    int cn = _src.channels();
    const AT* wtab = (const AT*)_wtab;
    const T* S0 = _src.ptr<T>();
    size_t sstep = _src.step/sizeof(S0[0]);
    Scalar_<T> cval(saturate_cast<T>(_borderValue[0]),
        saturate_cast<T>(_borderValue[1]),
        saturate_cast<T>(_borderValue[2]),
        saturate_cast<T>(_borderValue[3]));
    int dx, dy;
    CastOp castOp;
    int borderType1 = borderType != BORDER_TRANSPARENT ? borderType : BORDER_REFLECT_101;

    unsigned width1 = std::max(ssize.width-3, 0), height1 = std::max(ssize.height-3, 0);

    if( _dst.isContinuous() && _xy.isContinuous() && _fxy.isContinuous() )
    {
        dsize.width *= dsize.height;
        dsize.height = 1;
    }

    for( dy = 0; dy < dsize.height; dy++ )
    {
        T* D = _dst.ptr<T>(dy);
        const short* XY = _xy.ptr<short>(dy);
        const ushort* FXY = _fxy.ptr<ushort>(dy);

        for( dx = 0; dx < dsize.width; dx++, D += cn )
        {
            int sx = XY[dx*2]-1, sy = XY[dx*2+1]-1;
            const AT* w = wtab + FXY[dx]*16;
            int i, k;
            if( (unsigned)sx < width1 && (unsigned)sy < height1 )
            {
                const T* S = S0 + sy*sstep + sx*cn;
                for( k = 0; k < cn; k++ )
                {
                    WT sum = S[0]*w[0] + S[cn]*w[1] + S[cn*2]*w[2] + S[cn*3]*w[3];
                    S += sstep;
                    sum += S[0]*w[4] + S[cn]*w[5] + S[cn*2]*w[6] + S[cn*3]*w[7];
                    S += sstep;
                    sum += S[0]*w[8] + S[cn]*w[9] + S[cn*2]*w[10] + S[cn*3]*w[11];
                    S += sstep;
                    sum += S[0]*w[12] + S[cn]*w[13] + S[cn*2]*w[14] + S[cn*3]*w[15];
                    S += 1 - sstep*3;
                    D[k] = castOp(sum);
                }
            }
            else
            {
                int x[4], y[4];
                if( borderType == BORDER_TRANSPARENT &&
                    ((unsigned)(sx+1) >= (unsigned)ssize.width ||
                    (unsigned)(sy+1) >= (unsigned)ssize.height) )
                    continue;

                if( borderType1 == BORDER_CONSTANT &&
                    (sx >= ssize.width || sx+4 <= 0 ||
                    sy >= ssize.height || sy+4 <= 0))
                {
                    for( k = 0; k < cn; k++ )
                        D[k] = cval[k];
                    continue;
                }

                for( i = 0; i < 4; i++ )
                {
                    x[i] = borderInterpolate(sx + i, ssize.width, borderType1)*cn;
                    y[i] = borderInterpolate(sy + i, ssize.height, borderType1);
                }

                for( k = 0; k < cn; k++, S0++, w -= 16 )
                {
                    WT cv = cval[k], sum = cv*ONE;
                    for( i = 0; i < 4; i++, w += 4 )
                    {
                        int yi = y[i];
                        const T* S = S0 + yi*sstep;
                        if( yi < 0 )
                            continue;
                        if( x[0] >= 0 )
                            sum += (S[x[0]] - cv)*w[0];
                        if( x[1] >= 0 )
                            sum += (S[x[1]] - cv)*w[1];
                        if( x[2] >= 0 )
                            sum += (S[x[2]] - cv)*w[2];
                        if( x[3] >= 0 )
                            sum += (S[x[3]] - cv)*w[3];
                    }
                    D[k] = castOp(sum);
                }
                S0 -= cn;
            }
        }
    }
}

template<class CastOp, typename AT, int ONE>
static void remapLanczos4( const Mat& _src, Mat& _dst, const Mat& _xy,
                           const Mat& _fxy, const void* _wtab,
                           int borderType, const Scalar& _borderValue )
{
    typedef typename CastOp::rtype T;
    typedef typename CastOp::type1 WT;
    Size ssize = _src.size(), dsize = _dst.size();
    int cn = _src.channels();
    const AT* wtab = (const AT*)_wtab;
    const T* S0 = _src.ptr<T>();
    size_t sstep = _src.step/sizeof(S0[0]);
    Scalar_<T> cval(saturate_cast<T>(_borderValue[0]),
        saturate_cast<T>(_borderValue[1]),
        saturate_cast<T>(_borderValue[2]),
        saturate_cast<T>(_borderValue[3]));
    int dx, dy;
    CastOp castOp;
    int borderType1 = borderType != BORDER_TRANSPARENT ? borderType : BORDER_REFLECT_101;

    unsigned width1 = std::max(ssize.width-7, 0), height1 = std::max(ssize.height-7, 0);

    if( _dst.isContinuous() && _xy.isContinuous() && _fxy.isContinuous() )
    {
        dsize.width *= dsize.height;
        dsize.height = 1;
    }

    for( dy = 0; dy < dsize.height; dy++ )
    {
        T* D = _dst.ptr<T>(dy);
        const short* XY = _xy.ptr<short>(dy);
        const ushort* FXY = _fxy.ptr<ushort>(dy);

        for( dx = 0; dx < dsize.width; dx++, D += cn )
        {
            int sx = XY[dx*2]-3, sy = XY[dx*2+1]-3;
            const AT* w = wtab + FXY[dx]*64;
            const T* S = S0 + sy*sstep + sx*cn;
            int i, k;
            if( (unsigned)sx < width1 && (unsigned)sy < height1 )
            {
                for( k = 0; k < cn; k++ )
                {
                    WT sum = 0;
                    for( int r = 0; r < 8; r++, S += sstep, w += 8 )
                        sum += S[0]*w[0] + S[cn]*w[1] + S[cn*2]*w[2] + S[cn*3]*w[3] +
                            S[cn*4]*w[4] + S[cn*5]*w[5] + S[cn*6]*w[6] + S[cn*7]*w[7];
                    w -= 64;
                    S -= sstep*8 - 1;
                    D[k] = castOp(sum);
                }
            }
            else
            {
                int x[8], y[8];
                if( borderType == BORDER_TRANSPARENT &&
                    ((unsigned)(sx+3) >= (unsigned)ssize.width ||
                    (unsigned)(sy+3) >= (unsigned)ssize.height) )
                    continue;

                if( borderType1 == BORDER_CONSTANT &&
                    (sx >= ssize.width || sx+8 <= 0 ||
                    sy >= ssize.height || sy+8 <= 0))
                {
                    for( k = 0; k < cn; k++ )
                        D[k] = cval[k];
                    continue;
                }

                for( i = 0; i < 8; i++ )
                {
                    x[i] = borderInterpolate(sx + i, ssize.width, borderType1)*cn;
                    y[i] = borderInterpolate(sy + i, ssize.height, borderType1);
                }

                for( k = 0; k < cn; k++, S0++, w -= 64 )
                {
                    WT cv = cval[k], sum = cv*ONE;
                    for( i = 0; i < 8; i++, w += 8 )
                    {
                        int yi = y[i];
                        const T* S1 = S0 + yi*sstep;
                        if( yi < 0 )
                            continue;
                        if( x[0] >= 0 )
                            sum += (S1[x[0]] - cv)*w[0];
                        if( x[1] >= 0 )
                            sum += (S1[x[1]] - cv)*w[1];
                        if( x[2] >= 0 )
                            sum += (S1[x[2]] - cv)*w[2];
                        if( x[3] >= 0 )
                            sum += (S1[x[3]] - cv)*w[3];
                        if( x[4] >= 0 )
                            sum += (S1[x[4]] - cv)*w[4];
                        if( x[5] >= 0 )
                            sum += (S1[x[5]] - cv)*w[5];
                        if( x[6] >= 0 )
                            sum += (S1[x[6]] - cv)*w[6];
                        if( x[7] >= 0 )
                            sum += (S1[x[7]] - cv)*w[7];
                    }
                    D[k] = castOp(sum);
                }
                S0 -= cn;
            }
        }
    }
}

const int INTER_REMAP_COEF_BITS=15;
const int INTER_REMAP_COEF_SCALE=1 << INTER_REMAP_COEF_BITS;

static void initInterTab1D(int method, float* tab, int tabsz)
{
    float scale = 1.f/tabsz;
    if( method == INTER_LINEAR )
    {
        for( int i = 0; i < tabsz; i++, tab += 2 )
            interpolateLinear( i*scale, tab );
    }
    else if( method == INTER_CUBIC )
    {
        for( int i = 0; i < tabsz; i++, tab += 4 )
            interpolateCubic( i*scale, tab );
    }
    else if( method == INTER_LANCZOS4 )
    {
        for( int i = 0; i < tabsz; i++, tab += 8 )
            interpolateLanczos4( i*scale, tab );
    }
    else
        CV_Error( CV_StsBadArg, "Unknown interpolation method" );
}

static const void* initInterTab2D( int method, bool fixpt )
{
    static bool inittab[INTER_MAX+1] = {false};
    float* tab = 0;
    short* itab = 0;
    int ksize = 0;
    if( method == INTER_LINEAR )
        tab = BilinearTab_f[0][0], itab = BilinearTab_i[0][0], ksize=2;
    else if( method == INTER_CUBIC )
        tab = BicubicTab_f[0][0], itab = BicubicTab_i[0][0], ksize=4;
    else if( method == INTER_LANCZOS4 )
        tab = Lanczos4Tab_f[0][0], itab = Lanczos4Tab_i[0][0], ksize=8;
    else
        CV_Error( CV_StsBadArg, "Unknown/unsupported interpolation type" );

    if( !inittab[method] )
    {
        AutoBuffer<float> _tab(8*INTER_TAB_SIZE);
        int i, j, k1, k2;
        initInterTab1D(method, _tab, INTER_TAB_SIZE);
        for( i = 0; i < INTER_TAB_SIZE; i++ )
            for( j = 0; j < INTER_TAB_SIZE; j++, tab += ksize*ksize, itab += ksize*ksize )
            {
                int isum = 0;
                NNDeltaTab_i[i*INTER_TAB_SIZE+j][0] = j < INTER_TAB_SIZE/2;
                NNDeltaTab_i[i*INTER_TAB_SIZE+j][1] = i < INTER_TAB_SIZE/2;

                for( k1 = 0; k1 < ksize; k1++ )
                {
                    float vy = _tab[i*ksize + k1];
                    for( k2 = 0; k2 < ksize; k2++ )
                    {
                        float v = vy*_tab[j*ksize + k2];
                        tab[k1*ksize + k2] = v;
                        isum += itab[k1*ksize + k2] = saturate_cast<short>(v*INTER_REMAP_COEF_SCALE);
                    }
                }

                if( isum != INTER_REMAP_COEF_SCALE )
                {
                    int diff = isum - INTER_REMAP_COEF_SCALE;
                    int ksize2 = ksize/2, Mk1=ksize2, Mk2=ksize2, mk1=ksize2, mk2=ksize2;
                    for( k1 = ksize2; k1 < ksize2+2; k1++ )
                        for( k2 = ksize2; k2 < ksize2+2; k2++ )
                        {
                            if( itab[k1*ksize+k2] < itab[mk1*ksize+mk2] )
                                mk1 = k1, mk2 = k2;
                            else if( itab[k1*ksize+k2] > itab[Mk1*ksize+Mk2] )
                                Mk1 = k1, Mk2 = k2;
                        }
                    if( diff < 0 )
                        itab[Mk1*ksize + Mk2] = (short)(itab[Mk1*ksize + Mk2] - diff);
                    else
                        itab[mk1*ksize + mk2] = (short)(itab[mk1*ksize + mk2] - diff);
                }
            }
        tab -= INTER_TAB_SIZE2*ksize*ksize;
        itab -= INTER_TAB_SIZE2*ksize*ksize;
#if CV_SSE2 || CV_NEON
        if( method == INTER_LINEAR )
        {
            for( i = 0; i < INTER_TAB_SIZE2; i++ )
                for( j = 0; j < 4; j++ )
                {
                    BilinearTab_iC4[i][0][j*2] = BilinearTab_i[i][0][0];
                    BilinearTab_iC4[i][0][j*2+1] = BilinearTab_i[i][0][1];
                    BilinearTab_iC4[i][1][j*2] = BilinearTab_i[i][1][0];
                    BilinearTab_iC4[i][1][j*2+1] = BilinearTab_i[i][1][1];
                }
        }
#endif
        inittab[method] = true;
    }
    return fixpt ? (const void*)itab : (const void*)tab;
}

struct RemapNoVec
{
    int operator()( const Mat&, void*, const short*, const ushort*,
                    const void*, int ) const { return 0; }
};

typedef RemapNoVec RemapVec_8u;

void cv::remap( InputArray _src, OutputArray _dst,
                InputArray _map1, InputArray _map2,
                int interpolation, int borderType, const Scalar& borderValue )
{
    static RemapNNFunc nn_tab[] =
    {
        remapNearest<uchar>, remapNearest<schar>, remapNearest<ushort>, remapNearest<short>,
        remapNearest<int>, remapNearest<float>, remapNearest<double>, 0
    };

    static RemapFunc linear_tab[] =
    {
        remapBilinear<FixedPtCast<int, uchar, INTER_REMAP_COEF_BITS>, RemapVec_8u, short>, 0,
        remapBilinear<Cast<float, ushort>, RemapNoVec, float>,
        remapBilinear<Cast<float, short>, RemapNoVec, float>, 0,
        remapBilinear<Cast<float, float>, RemapNoVec, float>,
        remapBilinear<Cast<double, double>, RemapNoVec, float>, 0
    };

    static RemapFunc cubic_tab[] =
    {
        remapBicubic<FixedPtCast<int, uchar, INTER_REMAP_COEF_BITS>, short, INTER_REMAP_COEF_SCALE>, 0,
        remapBicubic<Cast<float, ushort>, float, 1>,
        remapBicubic<Cast<float, short>, float, 1>, 0,
        remapBicubic<Cast<float, float>, float, 1>,
        remapBicubic<Cast<double, double>, float, 1>, 0
    };

    static RemapFunc lanczos4_tab[] =
    {
        remapLanczos4<FixedPtCast<int, uchar, INTER_REMAP_COEF_BITS>, short, INTER_REMAP_COEF_SCALE>, 0,
        remapLanczos4<Cast<float, ushort>, float, 1>,
        remapLanczos4<Cast<float, short>, float, 1>, 0,
        remapLanczos4<Cast<float, float>, float, 1>,
        remapLanczos4<Cast<double, double>, float, 1>, 0
    };

    CV_Assert( _map1.size().area() > 0 );
    CV_Assert( _map2.empty() || (_map2.size() == _map1.size()));

    CV_OCL_RUN(_src.dims() <= 2 && _dst.isUMat(),
               ocl_remap(_src, _dst, _map1, _map2, interpolation, borderType, borderValue))

    Mat src = _src.getMat(), map1 = _map1.getMat(), map2 = _map2.getMat();
    _dst.create( map1.size(), src.type() );
    Mat dst = _dst.getMat();
    if( dst.data == src.data )
        src = src.clone();

    if( interpolation == INTER_AREA )
        interpolation = INTER_LINEAR;

    int type = src.type(), depth = CV_MAT_DEPTH(type);

#if defined HAVE_IPP && !defined HAVE_IPP_ICV_ONLY && IPP_DISABLE_BLOCK
    CV_IPP_CHECK()
    {
        if ((interpolation == INTER_LINEAR || interpolation == INTER_CUBIC || interpolation == INTER_NEAREST) &&
                map1.type() == CV_32FC1 && map2.type() == CV_32FC1 &&
                (borderType == BORDER_CONSTANT || borderType == BORDER_TRANSPARENT))
        {
            int ippInterpolation =
                interpolation == INTER_NEAREST ? IPPI_INTER_NN :
                interpolation == INTER_LINEAR ? IPPI_INTER_LINEAR : IPPI_INTER_CUBIC;

            ippiRemap ippFunc =
                type == CV_8UC1 ? (ippiRemap)ippiRemap_8u_C1R :
                type == CV_8UC3 ? (ippiRemap)ippiRemap_8u_C3R :
                type == CV_8UC4 ? (ippiRemap)ippiRemap_8u_C4R :
                type == CV_16UC1 ? (ippiRemap)ippiRemap_16u_C1R :
                type == CV_16UC3 ? (ippiRemap)ippiRemap_16u_C3R :
                type == CV_16UC4 ? (ippiRemap)ippiRemap_16u_C4R :
                type == CV_32FC1 ? (ippiRemap)ippiRemap_32f_C1R :
                type == CV_32FC3 ? (ippiRemap)ippiRemap_32f_C3R :
                type == CV_32FC4 ? (ippiRemap)ippiRemap_32f_C4R : 0;

            if (ippFunc)
            {
                bool ok;
                IPPRemapInvoker invoker(src, dst, map1, map2, ippFunc, ippInterpolation,
                                        borderType, borderValue, &ok);
                Range range(0, dst.rows);
                parallel_for_(range, invoker, dst.total() / (double)(1 << 16));

                if (ok)
                {
                    CV_IMPL_ADD(CV_IMPL_IPP|CV_IMPL_MT);
                    return;
                }
                setIppErrorStatus();
            }
        }
    }
#endif

    RemapNNFunc nnfunc = 0;
    RemapFunc ifunc = 0;
    const void* ctab = 0;
    bool fixpt = depth == CV_8U;
    bool planar_input = false;

    if( interpolation == INTER_NEAREST )
    {
        nnfunc = nn_tab[depth];
        CV_Assert( nnfunc != 0 );
    }
    else
    {
        if( interpolation == INTER_LINEAR )
            ifunc = linear_tab[depth];
        else if( interpolation == INTER_CUBIC )
            ifunc = cubic_tab[depth];
        else if( interpolation == INTER_LANCZOS4 )
            ifunc = lanczos4_tab[depth];
        else
            CV_Error( CV_StsBadArg, "Unknown interpolation method" );
        CV_Assert( ifunc != 0 );
        ctab = initInterTab2D( interpolation, fixpt );
    }

    const Mat *m1 = &map1, *m2 = &map2;

    if( (map1.type() == CV_16SC2 && (map2.type() == CV_16UC1 || map2.type() == CV_16SC1 || map2.empty())) ||
        (map2.type() == CV_16SC2 && (map1.type() == CV_16UC1 || map1.type() == CV_16SC1 || map1.empty())) )
    {
        if( map1.type() != CV_16SC2 )
            std::swap(m1, m2);
    }
    else
    {
        CV_Assert( ((map1.type() == CV_32FC2 || map1.type() == CV_16SC2) && map2.empty()) ||
            (map1.type() == CV_32FC1 && map2.type() == CV_32FC1) );
        planar_input = map1.channels() == 1;
    }

    RemapInvoker invoker(src, dst, m1, m2,
                         borderType, borderValue, planar_input, nnfunc, ifunc,
                         ctab);
    parallel_for_(Range(0, dst.rows), invoker, dst.total()/(double)(1<<16));
}

void cv::convertMaps( InputArray _map1, InputArray _map2,
                      OutputArray _dstmap1, OutputArray _dstmap2,
                      int dstm1type, bool nninterpolate )
{
    Mat map1 = _map1.getMat(), map2 = _map2.getMat(), dstmap1, dstmap2;
    Size size = map1.size();
    const Mat *m1 = &map1, *m2 = &map2;
    int m1type = m1->type(), m2type = m2->type();

    CV_Assert( (m1type == CV_16SC2 && (nninterpolate || m2type == CV_16UC1 || m2type == CV_16SC1)) ||
               (m2type == CV_16SC2 && (nninterpolate || m1type == CV_16UC1 || m1type == CV_16SC1)) ||
               (m1type == CV_32FC1 && m2type == CV_32FC1) ||
               (m1type == CV_32FC2 && m2->empty()) );

    if( m2type == CV_16SC2 )
    {
        std::swap( m1, m2 );
        std::swap( m1type, m2type );
    }

    if( dstm1type <= 0 )
        dstm1type = m1type == CV_16SC2 ? CV_32FC2 : CV_16SC2;
    CV_Assert( dstm1type == CV_16SC2 || dstm1type == CV_32FC1 || dstm1type == CV_32FC2 );
    _dstmap1.create( size, dstm1type );
    dstmap1 = _dstmap1.getMat();

    if( !nninterpolate && dstm1type != CV_32FC2 )
    {
        _dstmap2.create( size, dstm1type == CV_16SC2 ? CV_16UC1 : CV_32FC1 );
        dstmap2 = _dstmap2.getMat();
    }
    else
        _dstmap2.release();

    if( m1type == dstm1type || (nninterpolate &&
        ((m1type == CV_16SC2 && dstm1type == CV_32FC2) ||
        (m1type == CV_32FC2 && dstm1type == CV_16SC2))) )
    {
        m1->convertTo( dstmap1, dstmap1.type() );
        if( !dstmap2.empty() && dstmap2.type() == m2->type() )
            m2->copyTo( dstmap2 );
        return;
    }

    if( m1type == CV_32FC1 && dstm1type == CV_32FC2 )
    {
        Mat vdata[] = { *m1, *m2 };
        merge( vdata, 2, dstmap1 );
        return;
    }

    if( m1type == CV_32FC2 && dstm1type == CV_32FC1 )
    {
        Mat mv[] = { dstmap1, dstmap2 };
        split( *m1, mv );
        return;
    }

    if( m1->isContinuous() && (m2->empty() || m2->isContinuous()) &&
        dstmap1.isContinuous() && (dstmap2.empty() || dstmap2.isContinuous()) )
    {
        size.width *= size.height;
        size.height = 1;
    }

#if CV_SSE2
    bool useSSE2 = checkHardwareSupport(CV_CPU_SSE2);
#endif
#if CV_SSE4_1
    bool useSSE4_1 = checkHardwareSupport(CV_CPU_SSE4_1);
#endif

    const float scale = 1.f/INTER_TAB_SIZE;
    int x, y;
    for( y = 0; y < size.height; y++ )
    {
        const float* src1f = m1->ptr<float>(y);
        const float* src2f = m2->ptr<float>(y);
        const short* src1 = (const short*)src1f;
        const ushort* src2 = (const ushort*)src2f;

        float* dst1f = dstmap1.ptr<float>(y);
        float* dst2f = dstmap2.ptr<float>(y);
        short* dst1 = (short*)dst1f;
        ushort* dst2 = (ushort*)dst2f;
        x = 0;

        if( m1type == CV_32FC1 && dstm1type == CV_16SC2 )
        {
            if( nninterpolate )
            {
                #if CV_NEON
                for( ; x <= size.width - 8; x += 8 )
                {
                    int16x8x2_t v_dst;
                    v_dst.val[0] = vcombine_s16(vqmovn_s32(cv_vrndq_s32_f32(vld1q_f32(src1f + x))),
                                                vqmovn_s32(cv_vrndq_s32_f32(vld1q_f32(src1f + x + 4))));
                    v_dst.val[1] = vcombine_s16(vqmovn_s32(cv_vrndq_s32_f32(vld1q_f32(src2f + x))),
                                                vqmovn_s32(cv_vrndq_s32_f32(vld1q_f32(src2f + x + 4))));

                    vst2q_s16(dst1 + (x << 1), v_dst);
                }
                #elif CV_SSE4_1
                if (useSSE4_1)
                {
                    for( ; x <= size.width - 16; x += 16 )
                    {
                        __m128i v_dst0 = _mm_packs_epi32(_mm_cvtps_epi32(_mm_loadu_ps(src1f + x)),
                                                         _mm_cvtps_epi32(_mm_loadu_ps(src1f + x + 4)));
                        __m128i v_dst1 = _mm_packs_epi32(_mm_cvtps_epi32(_mm_loadu_ps(src1f + x + 8)),
                                                         _mm_cvtps_epi32(_mm_loadu_ps(src1f + x + 12)));

                        __m128i v_dst2 = _mm_packs_epi32(_mm_cvtps_epi32(_mm_loadu_ps(src2f + x)),
                                                         _mm_cvtps_epi32(_mm_loadu_ps(src2f + x + 4)));
                        __m128i v_dst3 = _mm_packs_epi32(_mm_cvtps_epi32(_mm_loadu_ps(src2f + x + 8)),
                                                         _mm_cvtps_epi32(_mm_loadu_ps(src2f + x + 12)));

                        _mm_interleave_epi16(v_dst0, v_dst1, v_dst2, v_dst3);

                        _mm_storeu_si128((__m128i *)(dst1 + x * 2), v_dst0);
                        _mm_storeu_si128((__m128i *)(dst1 + x * 2 + 8), v_dst1);
                        _mm_storeu_si128((__m128i *)(dst1 + x * 2 + 16), v_dst2);
                        _mm_storeu_si128((__m128i *)(dst1 + x * 2 + 24), v_dst3);
                    }
                }
                #endif
                for( ; x < size.width; x++ )
                {
                    dst1[x*2] = saturate_cast<short>(src1f[x]);
                    dst1[x*2+1] = saturate_cast<short>(src2f[x]);
                }
            }
            else
            {
                #if CV_NEON
                float32x4_t v_scale = vdupq_n_f32((float)INTER_TAB_SIZE);
                int32x4_t v_mask = vdupq_n_s32(INTER_TAB_SIZE - 1);

                for( ; x <= size.width - 8; x += 8 )
                {
                    int32x4_t v_ix0 = cv_vrndq_s32_f32(vmulq_f32(vld1q_f32(src1f + x), v_scale));
                    int32x4_t v_ix1 = cv_vrndq_s32_f32(vmulq_f32(vld1q_f32(src1f + x + 4), v_scale));
                    int32x4_t v_iy0 = cv_vrndq_s32_f32(vmulq_f32(vld1q_f32(src2f + x), v_scale));
                    int32x4_t v_iy1 = cv_vrndq_s32_f32(vmulq_f32(vld1q_f32(src2f + x + 4), v_scale));

                    int16x8x2_t v_dst;
                    v_dst.val[0] = vcombine_s16(vqmovn_s32(vshrq_n_s32(v_ix0, INTER_BITS)),
                                                vqmovn_s32(vshrq_n_s32(v_ix1, INTER_BITS)));
                    v_dst.val[1] = vcombine_s16(vqmovn_s32(vshrq_n_s32(v_iy0, INTER_BITS)),
                                                vqmovn_s32(vshrq_n_s32(v_iy1, INTER_BITS)));

                    vst2q_s16(dst1 + (x << 1), v_dst);

                    uint16x4_t v_dst0 = vqmovun_s32(vaddq_s32(vshlq_n_s32(vandq_s32(v_iy0, v_mask), INTER_BITS),
                                                              vandq_s32(v_ix0, v_mask)));
                    uint16x4_t v_dst1 = vqmovun_s32(vaddq_s32(vshlq_n_s32(vandq_s32(v_iy1, v_mask), INTER_BITS),
                                                              vandq_s32(v_ix1, v_mask)));
                    vst1q_u16(dst2 + x, vcombine_u16(v_dst0, v_dst1));
                }
                #elif CV_SSE4_1
                if (useSSE4_1)
                {
                    __m128 v_its = _mm_set1_ps(INTER_TAB_SIZE);
                    __m128i v_its1 = _mm_set1_epi32(INTER_TAB_SIZE-1);

                    for( ; x <= size.width - 16; x += 16 )
                    {
                        __m128i v_ix0 = _mm_cvtps_epi32(_mm_mul_ps(_mm_loadu_ps(src1f + x), v_its));
                        __m128i v_ix1 = _mm_cvtps_epi32(_mm_mul_ps(_mm_loadu_ps(src1f + x + 4), v_its));
                        __m128i v_iy0 = _mm_cvtps_epi32(_mm_mul_ps(_mm_loadu_ps(src2f + x), v_its));
                        __m128i v_iy1 = _mm_cvtps_epi32(_mm_mul_ps(_mm_loadu_ps(src2f + x + 4), v_its));

                        __m128i v_dst10 = _mm_packs_epi32(_mm_srai_epi32(v_ix0, INTER_BITS),
                                                          _mm_srai_epi32(v_ix1, INTER_BITS));
                        __m128i v_dst12 = _mm_packs_epi32(_mm_srai_epi32(v_iy0, INTER_BITS),
                                                          _mm_srai_epi32(v_iy1, INTER_BITS));
                        __m128i v_dst20 = _mm_add_epi32(_mm_slli_epi32(_mm_and_si128(v_iy0, v_its1), INTER_BITS),
                                                        _mm_and_si128(v_ix0, v_its1));
                        __m128i v_dst21 = _mm_add_epi32(_mm_slli_epi32(_mm_and_si128(v_iy1, v_its1), INTER_BITS),
                                                        _mm_and_si128(v_ix1, v_its1));
                        _mm_storeu_si128((__m128i *)(dst2 + x), _mm_packus_epi32(v_dst20, v_dst21));

                        v_ix0 = _mm_cvtps_epi32(_mm_mul_ps(_mm_loadu_ps(src1f + x + 8), v_its));
                        v_ix1 = _mm_cvtps_epi32(_mm_mul_ps(_mm_loadu_ps(src1f + x + 12), v_its));
                        v_iy0 = _mm_cvtps_epi32(_mm_mul_ps(_mm_loadu_ps(src2f + x + 8), v_its));
                        v_iy1 = _mm_cvtps_epi32(_mm_mul_ps(_mm_loadu_ps(src2f + x + 12), v_its));

                        __m128i v_dst11 = _mm_packs_epi32(_mm_srai_epi32(v_ix0, INTER_BITS),
                                                          _mm_srai_epi32(v_ix1, INTER_BITS));
                        __m128i v_dst13 = _mm_packs_epi32(_mm_srai_epi32(v_iy0, INTER_BITS),
                                                          _mm_srai_epi32(v_iy1, INTER_BITS));
                        v_dst20 = _mm_add_epi32(_mm_slli_epi32(_mm_and_si128(v_iy0, v_its1), INTER_BITS),
                                                _mm_and_si128(v_ix0, v_its1));
                        v_dst21 = _mm_add_epi32(_mm_slli_epi32(_mm_and_si128(v_iy1, v_its1), INTER_BITS),
                                                _mm_and_si128(v_ix1, v_its1));
                        _mm_storeu_si128((__m128i *)(dst2 + x + 8), _mm_packus_epi32(v_dst20, v_dst21));

                        _mm_interleave_epi16(v_dst10, v_dst11, v_dst12, v_dst13);

                        _mm_storeu_si128((__m128i *)(dst1 + x * 2), v_dst10);
                        _mm_storeu_si128((__m128i *)(dst1 + x * 2 + 8), v_dst11);
                        _mm_storeu_si128((__m128i *)(dst1 + x * 2 + 16), v_dst12);
                        _mm_storeu_si128((__m128i *)(dst1 + x * 2 + 24), v_dst13);
                    }
                }
                #endif
                for( ; x < size.width; x++ )
                {
                    int ix = saturate_cast<int>(src1f[x]*INTER_TAB_SIZE);
                    int iy = saturate_cast<int>(src2f[x]*INTER_TAB_SIZE);
                    dst1[x*2] = saturate_cast<short>(ix >> INTER_BITS);
                    dst1[x*2+1] = saturate_cast<short>(iy >> INTER_BITS);
                    dst2[x] = (ushort)((iy & (INTER_TAB_SIZE-1))*INTER_TAB_SIZE + (ix & (INTER_TAB_SIZE-1)));
                }
            }
        }
        else if( m1type == CV_32FC2 && dstm1type == CV_16SC2 )
        {
            if( nninterpolate )
            {
                #if CV_NEON
                for( ; x <= (size.width << 1) - 8; x += 8 )
                    vst1q_s16(dst1 + x, vcombine_s16(vqmovn_s32(cv_vrndq_s32_f32(vld1q_f32(src1f + x))),
                                                     vqmovn_s32(cv_vrndq_s32_f32(vld1q_f32(src1f + x + 4)))));
                #elif CV_SSE2
                for( ; x <= (size.width << 1) - 8; x += 8 )
                {
                    _mm_storeu_si128((__m128i *)(dst1 + x), _mm_packs_epi32(_mm_cvtps_epi32(_mm_loadu_ps(src1f + x)),
                                                                            _mm_cvtps_epi32(_mm_loadu_ps(src1f + x + 4))));
                }
                #endif
                for( ; x < size.width; x++ )
                {
                    dst1[x*2] = saturate_cast<short>(src1f[x*2]);
                    dst1[x*2+1] = saturate_cast<short>(src1f[x*2+1]);
                }
            }
            else
            {
                #if CV_NEON
                float32x4_t v_scale = vdupq_n_f32((float)INTER_TAB_SIZE);
                int32x4_t v_mask = vdupq_n_s32(INTER_TAB_SIZE - 1);

                for( ; x <= size.width - 8; x += 8 )
                {
                    float32x4x2_t v_src0 = vld2q_f32(src1f + (x << 1)), v_src1 = vld2q_f32(src1f + (x << 1) + 8);
                    int32x4_t v_ix0 = cv_vrndq_s32_f32(vmulq_f32(v_src0.val[0], v_scale));
                    int32x4_t v_ix1 = cv_vrndq_s32_f32(vmulq_f32(v_src1.val[0], v_scale));
                    int32x4_t v_iy0 = cv_vrndq_s32_f32(vmulq_f32(v_src0.val[1], v_scale));
                    int32x4_t v_iy1 = cv_vrndq_s32_f32(vmulq_f32(v_src1.val[1], v_scale));

                    int16x8x2_t v_dst;
                    v_dst.val[0] = vcombine_s16(vqmovn_s32(vshrq_n_s32(v_ix0, INTER_BITS)),
                                                vqmovn_s32(vshrq_n_s32(v_ix1, INTER_BITS)));
                    v_dst.val[1] = vcombine_s16(vqmovn_s32(vshrq_n_s32(v_iy0, INTER_BITS)),
                                                vqmovn_s32(vshrq_n_s32(v_iy1, INTER_BITS)));

                    vst2q_s16(dst1 + (x << 1), v_dst);

                    uint16x4_t v_dst0 = vqmovun_s32(vaddq_s32(vshlq_n_s32(vandq_s32(v_iy0, v_mask), INTER_BITS),
                                                              vandq_s32(v_ix0, v_mask)));
                    uint16x4_t v_dst1 = vqmovun_s32(vaddq_s32(vshlq_n_s32(vandq_s32(v_iy1, v_mask), INTER_BITS),
                                                              vandq_s32(v_ix1, v_mask)));
                    vst1q_u16(dst2 + x, vcombine_u16(v_dst0, v_dst1));
                }
                #elif CV_SSE4_1
                if (useSSE4_1)
                {
                    __m128 v_its = _mm_set1_ps(INTER_TAB_SIZE);
                    __m128i v_its1 = _mm_set1_epi32(INTER_TAB_SIZE-1);
                    __m128i v_y_mask = _mm_set1_epi32((INTER_TAB_SIZE-1) << 16);

                    for( ; x <= size.width - 4; x += 4 )
                    {
                        __m128i v_src0 = _mm_cvtps_epi32(_mm_mul_ps(_mm_loadu_ps(src1f + x * 2), v_its));
                        __m128i v_src1 = _mm_cvtps_epi32(_mm_mul_ps(_mm_loadu_ps(src1f + x * 2 + 4), v_its));

                        __m128i v_dst1 = _mm_packs_epi32(_mm_srai_epi32(v_src0, INTER_BITS),
                                                         _mm_srai_epi32(v_src1, INTER_BITS));
                        _mm_storeu_si128((__m128i *)(dst1 + x * 2), v_dst1);

                        // x0 y0 x1 y1 . . .
                        v_src0 = _mm_packs_epi32(_mm_and_si128(v_src0, v_its1),
                                                 _mm_and_si128(v_src1, v_its1));
                        __m128i v_dst2 = _mm_or_si128(_mm_srli_epi32(_mm_and_si128(v_src0, v_y_mask), 16 - INTER_BITS), // y0 0 y1 0 . . .
                                                      _mm_and_si128(v_src0, v_its1)); // 0 x0 0 x1 . . .
                        _mm_storel_epi64((__m128i *)(dst2 + x), _mm_packus_epi32(v_dst2, v_dst2));
                    }
                }
                #endif
                for( ; x < size.width; x++ )
                {
                    int ix = saturate_cast<int>(src1f[x*2]*INTER_TAB_SIZE);
                    int iy = saturate_cast<int>(src1f[x*2+1]*INTER_TAB_SIZE);
                    dst1[x*2] = saturate_cast<short>(ix >> INTER_BITS);
                    dst1[x*2+1] = saturate_cast<short>(iy >> INTER_BITS);
                    dst2[x] = (ushort)((iy & (INTER_TAB_SIZE-1))*INTER_TAB_SIZE + (ix & (INTER_TAB_SIZE-1)));
                }
            }
        }
        else if( m1type == CV_16SC2 && dstm1type == CV_32FC1 )
        {
            #if CV_NEON
            uint16x8_t v_mask2 = vdupq_n_u16(INTER_TAB_SIZE2-1);
            uint32x4_t v_zero = vdupq_n_u32(0u), v_mask = vdupq_n_u32(INTER_TAB_SIZE-1);
            float32x4_t v_scale = vdupq_n_f32(scale);

            for( ; x <= size.width - 8; x += 8)
            {
                uint32x4_t v_fxy1, v_fxy2;
                if (src2)
                {
                    uint16x8_t v_src2 = vandq_u16(vld1q_u16(src2 + x), v_mask2);
                    v_fxy1 = vmovl_u16(vget_low_u16(v_src2));
                    v_fxy2 = vmovl_u16(vget_high_u16(v_src2));
                }
                else
                    v_fxy1 = v_fxy2 = v_zero;

                int16x8x2_t v_src = vld2q_s16(src1 + (x << 1));
                float32x4_t v_dst1 = vmlaq_f32(vcvtq_f32_s32(vmovl_s16(vget_low_s16(v_src.val[0]))),
                                               v_scale, vcvtq_f32_u32(vandq_u32(v_fxy1, v_mask)));
                float32x4_t v_dst2 = vmlaq_f32(vcvtq_f32_s32(vmovl_s16(vget_low_s16(v_src.val[1]))),
                                               v_scale, vcvtq_f32_u32(vshrq_n_u32(v_fxy1, INTER_BITS)));
                vst1q_f32(dst1f + x, v_dst1);
                vst1q_f32(dst2f + x, v_dst2);

                v_dst1 = vmlaq_f32(vcvtq_f32_s32(vmovl_s16(vget_high_s16(v_src.val[0]))),
                                   v_scale, vcvtq_f32_u32(vandq_u32(v_fxy2, v_mask)));
                v_dst2 = vmlaq_f32(vcvtq_f32_s32(vmovl_s16(vget_high_s16(v_src.val[1]))),
                                   v_scale, vcvtq_f32_u32(vshrq_n_u32(v_fxy2, INTER_BITS)));
                vst1q_f32(dst1f + x + 4, v_dst1);
                vst1q_f32(dst2f + x + 4, v_dst2);
            }
            #elif CV_SSE2
            __m128i v_mask2 = _mm_set1_epi16(INTER_TAB_SIZE2-1);
            __m128i v_zero = _mm_setzero_si128(), v_mask = _mm_set1_epi32(INTER_TAB_SIZE-1);
            __m128 v_scale = _mm_set1_ps(scale);

            for( ; x <= size.width - 16; x += 16)
            {
                __m128i v_src10 = _mm_loadu_si128((__m128i const *)(src1 + x * 2));
                __m128i v_src11 = _mm_loadu_si128((__m128i const *)(src1 + x * 2 + 8));
                __m128i v_src20 = _mm_loadu_si128((__m128i const *)(src1 + x * 2 + 16));
                __m128i v_src21 = _mm_loadu_si128((__m128i const *)(src1 + x * 2 + 24));

                _mm_deinterleave_epi16(v_src10, v_src11, v_src20, v_src21);

                __m128i v_fxy = src2 ? _mm_and_si128(_mm_loadu_si128((__m128i const *)(src2 + x)), v_mask2) : v_zero;
                __m128i v_fxy_p = _mm_unpacklo_epi16(v_fxy, v_zero);
                _mm_storeu_ps(dst1f + x, _mm_add_ps(_mm_cvtepi32_ps(_mm_srai_epi32(_mm_unpacklo_epi16(v_zero, v_src10), 16)),
                                                    _mm_mul_ps(v_scale, _mm_cvtepi32_ps(_mm_and_si128(v_fxy_p, v_mask)))));
                _mm_storeu_ps(dst2f + x, _mm_add_ps(_mm_cvtepi32_ps(_mm_srai_epi32(_mm_unpacklo_epi16(v_zero, v_src20), 16)),
                                                    _mm_mul_ps(v_scale, _mm_cvtepi32_ps(_mm_srli_epi32(v_fxy_p, INTER_BITS)))));
                v_fxy_p = _mm_unpackhi_epi16(v_fxy, v_zero);
                _mm_storeu_ps(dst1f + x + 4, _mm_add_ps(_mm_cvtepi32_ps(_mm_srai_epi32(_mm_unpackhi_epi16(v_zero, v_src10), 16)),
                                                        _mm_mul_ps(v_scale, _mm_cvtepi32_ps(_mm_and_si128(v_fxy_p, v_mask)))));
                _mm_storeu_ps(dst2f + x + 4, _mm_add_ps(_mm_cvtepi32_ps(_mm_srai_epi32(_mm_unpackhi_epi16(v_zero, v_src20), 16)),
                                                        _mm_mul_ps(v_scale, _mm_cvtepi32_ps(_mm_srli_epi32(v_fxy_p, INTER_BITS)))));

                v_fxy = src2 ? _mm_and_si128(_mm_loadu_si128((__m128i const *)(src2 + x + 8)), v_mask2) : v_zero;
                v_fxy_p = _mm_unpackhi_epi16(v_fxy, v_zero);
                _mm_storeu_ps(dst1f + x + 8, _mm_add_ps(_mm_cvtepi32_ps(_mm_srai_epi32(_mm_unpacklo_epi16(v_zero, v_src11), 16)),
                                                        _mm_mul_ps(v_scale, _mm_cvtepi32_ps(_mm_and_si128(v_fxy_p, v_mask)))));
                _mm_storeu_ps(dst2f + x + 8, _mm_add_ps(_mm_cvtepi32_ps(_mm_srai_epi32(_mm_unpacklo_epi16(v_zero, v_src21), 16)),
                                                        _mm_mul_ps(v_scale, _mm_cvtepi32_ps(_mm_srli_epi32(v_fxy_p, INTER_BITS)))));
                v_fxy_p = _mm_unpackhi_epi16(v_fxy, v_zero);
                _mm_storeu_ps(dst1f + x + 12, _mm_add_ps(_mm_cvtepi32_ps(_mm_srai_epi32(_mm_unpackhi_epi16(v_zero, v_src11), 16)),
                                                         _mm_mul_ps(v_scale, _mm_cvtepi32_ps(_mm_and_si128(v_fxy_p, v_mask)))));
                _mm_storeu_ps(dst2f + x + 12, _mm_add_ps(_mm_cvtepi32_ps(_mm_srai_epi32(_mm_unpackhi_epi16(v_zero, v_src21), 16)),
                                                         _mm_mul_ps(v_scale, _mm_cvtepi32_ps(_mm_srli_epi32(v_fxy_p, INTER_BITS)))));
            }
            #endif
            for( ; x < size.width; x++ )
            {
                int fxy = src2 ? src2[x] & (INTER_TAB_SIZE2-1) : 0;
                dst1f[x] = src1[x*2] + (fxy & (INTER_TAB_SIZE-1))*scale;
                dst2f[x] = src1[x*2+1] + (fxy >> INTER_BITS)*scale;
            }
        }
        else if( m1type == CV_16SC2 && dstm1type == CV_32FC2 )
        {
            #if CV_NEON
            int16x8_t v_mask2 = vdupq_n_s16(INTER_TAB_SIZE2-1);
            int32x4_t v_zero = vdupq_n_s32(0), v_mask = vdupq_n_s32(INTER_TAB_SIZE-1);
            float32x4_t v_scale = vdupq_n_f32(scale);

            for( ; x <= size.width - 8; x += 8)
            {
                int32x4_t v_fxy1, v_fxy2;
                if (src2)
                {
                    int16x8_t v_src2 = vandq_s16(vld1q_s16((short *)src2 + x), v_mask2);
                    v_fxy1 = vmovl_s16(vget_low_s16(v_src2));
                    v_fxy2 = vmovl_s16(vget_high_s16(v_src2));
                }
                else
                    v_fxy1 = v_fxy2 = v_zero;

                int16x8x2_t v_src = vld2q_s16(src1 + (x << 1));
                float32x4x2_t v_dst;
                v_dst.val[0] = vmlaq_f32(vcvtq_f32_s32(vmovl_s16(vget_low_s16(v_src.val[0]))),
                                         v_scale, vcvtq_f32_s32(vandq_s32(v_fxy1, v_mask)));
                v_dst.val[1] = vmlaq_f32(vcvtq_f32_s32(vmovl_s16(vget_low_s16(v_src.val[1]))),
                                         v_scale, vcvtq_f32_s32(vshrq_n_s32(v_fxy1, INTER_BITS)));
                vst2q_f32(dst1f + (x << 1), v_dst);

                v_dst.val[0] = vmlaq_f32(vcvtq_f32_s32(vmovl_s16(vget_high_s16(v_src.val[0]))),
                                         v_scale, vcvtq_f32_s32(vandq_s32(v_fxy2, v_mask)));
                v_dst.val[1] = vmlaq_f32(vcvtq_f32_s32(vmovl_s16(vget_high_s16(v_src.val[1]))),
                                         v_scale, vcvtq_f32_s32(vshrq_n_s32(v_fxy2, INTER_BITS)));
                vst2q_f32(dst1f + (x << 1) + 8, v_dst);
            }
            #elif CV_SSE2
            if (useSSE2)
            {
                __m128i v_mask2 = _mm_set1_epi16(INTER_TAB_SIZE2-1);
                __m128i v_zero = _mm_set1_epi32(0), v_mask = _mm_set1_epi32(INTER_TAB_SIZE-1);
                __m128 v_scale = _mm_set1_ps(scale);

                for ( ; x <= size.width - 8; x += 8)
                {
                    __m128i v_src = _mm_loadu_si128((__m128i const *)(src1 + x * 2));
                    __m128i v_fxy = src2 ? _mm_and_si128(_mm_loadu_si128((__m128i const *)(src2 + x)), v_mask2) : v_zero;
                    __m128i v_fxy1 = _mm_and_si128(v_fxy, v_mask);
                    __m128i v_fxy2 = _mm_srli_epi16(v_fxy, INTER_BITS);

                    __m128 v_add = _mm_mul_ps(_mm_cvtepi32_ps(_mm_unpacklo_epi16(v_fxy1, v_fxy2)), v_scale);
                    _mm_storeu_ps(dst1f + x * 2, _mm_add_ps(_mm_cvtepi32_ps(_mm_unpacklo_epi16(v_src, v_zero)), v_add));

                    v_add = _mm_mul_ps(_mm_cvtepi32_ps(_mm_unpackhi_epi16(v_fxy1, v_fxy2)), v_scale);
                    _mm_storeu_ps(dst1f + x * 2, _mm_add_ps(_mm_cvtepi32_ps(_mm_unpackhi_epi16(v_src, v_zero)), v_add));
                }
            }
            #endif
            for( ; x < size.width; x++ )
            {
                int fxy = src2 ? src2[x] & (INTER_TAB_SIZE2-1): 0;
                dst1f[x*2] = src1[x*2] + (fxy & (INTER_TAB_SIZE-1))*scale;
                dst1f[x*2+1] = src1[x*2+1] + (fxy >> INTER_BITS)*scale;
            }
        }
        else
            CV_Error( CV_StsNotImplemented, "Unsupported combination of input/output matrices" );
    }
}

class WarpAffineInvoker :
    public ParallelLoopBody
{
public:
    WarpAffineInvoker(const Mat &_src, Mat &_dst, int _interpolation, int _borderType,
                      const Scalar &_borderValue, int *_adelta, int *_bdelta, double *_M) :
        ParallelLoopBody(), src(_src), dst(_dst), interpolation(_interpolation),
        borderType(_borderType), borderValue(_borderValue), adelta(_adelta), bdelta(_bdelta),
        M(_M)
    {
    }

    virtual void operator() (const Range& range) const
    {
        const int BLOCK_SZ = 64;
        short XY[BLOCK_SZ*BLOCK_SZ*2], A[BLOCK_SZ*BLOCK_SZ];
        const int AB_BITS = MAX(10, (int)INTER_BITS);
        const int AB_SCALE = 1 << AB_BITS;
        int round_delta = interpolation == INTER_NEAREST ? AB_SCALE/2 : AB_SCALE/INTER_TAB_SIZE/2, x, y, x1, y1;
    #if CV_SSE2
        bool useSSE2 = checkHardwareSupport(CV_CPU_SSE2);
    #endif
    #if CV_SSE4_1
        bool useSSE4_1 = checkHardwareSupport(CV_CPU_SSE4_1);
    #endif

        int bh0 = std::min(BLOCK_SZ/2, dst.rows);
        int bw0 = std::min(BLOCK_SZ*BLOCK_SZ/bh0, dst.cols);
        bh0 = std::min(BLOCK_SZ*BLOCK_SZ/bw0, dst.rows);

        for( y = range.start; y < range.end; y += bh0 )
        {
            for( x = 0; x < dst.cols; x += bw0 )
            {
                int bw = std::min( bw0, dst.cols - x);
                int bh = std::min( bh0, range.end - y);

                Mat _XY(bh, bw, CV_16SC2, XY), matA;
                Mat dpart(dst, Rect(x, y, bw, bh));

                for( y1 = 0; y1 < bh; y1++ )
                {
                    short* xy = XY + y1*bw*2;
                    int X0 = saturate_cast<int>((M[1]*(y + y1) + M[2])*AB_SCALE) + round_delta;
                    int Y0 = saturate_cast<int>((M[4]*(y + y1) + M[5])*AB_SCALE) + round_delta;

                    if( interpolation == INTER_NEAREST )
                    {
                        x1 = 0;
                        #if CV_NEON
                        int32x4_t v_X0 = vdupq_n_s32(X0), v_Y0 = vdupq_n_s32(Y0);
                        for( ; x1 <= bw - 8; x1 += 8 )
                        {
                            int16x8x2_t v_dst;
                            v_dst.val[0] = vcombine_s16(vqmovn_s32(vshrq_n_s32(vaddq_s32(v_X0, vld1q_s32(adelta + x + x1)), AB_BITS)),
                                                        vqmovn_s32(vshrq_n_s32(vaddq_s32(v_X0, vld1q_s32(adelta + x + x1 + 4)), AB_BITS)));
                            v_dst.val[1] = vcombine_s16(vqmovn_s32(vshrq_n_s32(vaddq_s32(v_Y0, vld1q_s32(bdelta + x + x1)), AB_BITS)),
                                                        vqmovn_s32(vshrq_n_s32(vaddq_s32(v_Y0, vld1q_s32(bdelta + x + x1 + 4)), AB_BITS)));

                            vst2q_s16(xy + (x1 << 1), v_dst);
                        }
                        #elif CV_SSE4_1
                        if (useSSE4_1)
                        {
                            __m128i v_X0 = _mm_set1_epi32(X0);
                            __m128i v_Y0 = _mm_set1_epi32(Y0);
                            for ( ; x1 <= bw - 16; x1 += 16)
                            {
                                __m128i v_x0 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(v_X0, _mm_loadu_si128((__m128i const *)(adelta + x + x1))), AB_BITS),
                                                               _mm_srai_epi32(_mm_add_epi32(v_X0, _mm_loadu_si128((__m128i const *)(adelta + x + x1 + 4))), AB_BITS));
                                __m128i v_x1 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(v_X0, _mm_loadu_si128((__m128i const *)(adelta + x + x1 + 8))), AB_BITS),
                                                               _mm_srai_epi32(_mm_add_epi32(v_X0, _mm_loadu_si128((__m128i const *)(adelta + x + x1 + 12))), AB_BITS));

                                __m128i v_y0 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(v_Y0, _mm_loadu_si128((__m128i const *)(bdelta + x + x1))), AB_BITS),
                                                               _mm_srai_epi32(_mm_add_epi32(v_Y0, _mm_loadu_si128((__m128i const *)(bdelta + x + x1 + 4))), AB_BITS));
                                __m128i v_y1 = _mm_packs_epi32(_mm_srai_epi32(_mm_add_epi32(v_Y0, _mm_loadu_si128((__m128i const *)(bdelta + x + x1 + 8))), AB_BITS),
                                                               _mm_srai_epi32(_mm_add_epi32(v_Y0, _mm_loadu_si128((__m128i const *)(bdelta + x + x1 + 12))), AB_BITS));

                                _mm_interleave_epi16(v_x0, v_x1, v_y0, v_y1);

                                _mm_storeu_si128((__m128i *)(xy + x1 * 2), v_x0);
                                _mm_storeu_si128((__m128i *)(xy + x1 * 2 + 8), v_x1);
                                _mm_storeu_si128((__m128i *)(xy + x1 * 2 + 16), v_y0);
                                _mm_storeu_si128((__m128i *)(xy + x1 * 2 + 24), v_y1);
                            }
                        }
                        #endif
                        for( ; x1 < bw; x1++ )
                        {
                            int X = (X0 + adelta[x+x1]) >> AB_BITS;
                            int Y = (Y0 + bdelta[x+x1]) >> AB_BITS;
                            xy[x1*2] = saturate_cast<short>(X);
                            xy[x1*2+1] = saturate_cast<short>(Y);
                        }
                    }
                    else
                    {
                        short* alpha = A + y1*bw;
                        x1 = 0;
                    #if CV_SSE2
                        if( useSSE2 )
                        {
                            __m128i fxy_mask = _mm_set1_epi32(INTER_TAB_SIZE - 1);
                            __m128i XX = _mm_set1_epi32(X0), YY = _mm_set1_epi32(Y0);
                            for( ; x1 <= bw - 8; x1 += 8 )
                            {
                                __m128i tx0, tx1, ty0, ty1;
                                tx0 = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(adelta + x + x1)), XX);
                                ty0 = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(bdelta + x + x1)), YY);
                                tx1 = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(adelta + x + x1 + 4)), XX);
                                ty1 = _mm_add_epi32(_mm_loadu_si128((const __m128i*)(bdelta + x + x1 + 4)), YY);

                                tx0 = _mm_srai_epi32(tx0, AB_BITS - INTER_BITS);
                                ty0 = _mm_srai_epi32(ty0, AB_BITS - INTER_BITS);
                                tx1 = _mm_srai_epi32(tx1, AB_BITS - INTER_BITS);
                                ty1 = _mm_srai_epi32(ty1, AB_BITS - INTER_BITS);

                                __m128i fx_ = _mm_packs_epi32(_mm_and_si128(tx0, fxy_mask),
                                                            _mm_and_si128(tx1, fxy_mask));
                                __m128i fy_ = _mm_packs_epi32(_mm_and_si128(ty0, fxy_mask),
                                                            _mm_and_si128(ty1, fxy_mask));
                                tx0 = _mm_packs_epi32(_mm_srai_epi32(tx0, INTER_BITS),
                                                            _mm_srai_epi32(tx1, INTER_BITS));
                                ty0 = _mm_packs_epi32(_mm_srai_epi32(ty0, INTER_BITS),
                                                    _mm_srai_epi32(ty1, INTER_BITS));
                                fx_ = _mm_adds_epi16(fx_, _mm_slli_epi16(fy_, INTER_BITS));

                                _mm_storeu_si128((__m128i*)(xy + x1*2), _mm_unpacklo_epi16(tx0, ty0));
                                _mm_storeu_si128((__m128i*)(xy + x1*2 + 8), _mm_unpackhi_epi16(tx0, ty0));
                                _mm_storeu_si128((__m128i*)(alpha + x1), fx_);
                            }
                        }
                    #elif CV_NEON
                        int32x4_t v__X0 = vdupq_n_s32(X0), v__Y0 = vdupq_n_s32(Y0), v_mask = vdupq_n_s32(INTER_TAB_SIZE - 1);
                        for( ; x1 <= bw - 8; x1 += 8 )
                        {
                            int32x4_t v_X0 = vshrq_n_s32(vaddq_s32(v__X0, vld1q_s32(adelta + x + x1)), AB_BITS - INTER_BITS);
                            int32x4_t v_Y0 = vshrq_n_s32(vaddq_s32(v__Y0, vld1q_s32(bdelta + x + x1)), AB_BITS - INTER_BITS);
                            int32x4_t v_X1 = vshrq_n_s32(vaddq_s32(v__X0, vld1q_s32(adelta + x + x1 + 4)), AB_BITS - INTER_BITS);
                            int32x4_t v_Y1 = vshrq_n_s32(vaddq_s32(v__Y0, vld1q_s32(bdelta + x + x1 + 4)), AB_BITS - INTER_BITS);

                            int16x8x2_t v_xy;
                            v_xy.val[0] = vcombine_s16(vqmovn_s32(vshrq_n_s32(v_X0, INTER_BITS)), vqmovn_s32(vshrq_n_s32(v_X1, INTER_BITS)));
                            v_xy.val[1] = vcombine_s16(vqmovn_s32(vshrq_n_s32(v_Y0, INTER_BITS)), vqmovn_s32(vshrq_n_s32(v_Y1, INTER_BITS)));

                            vst2q_s16(xy + (x1 << 1), v_xy);

                            int16x4_t v_alpha0 = vmovn_s32(vaddq_s32(vshlq_n_s32(vandq_s32(v_Y0, v_mask), INTER_BITS),
                                                                     vandq_s32(v_X0, v_mask)));
                            int16x4_t v_alpha1 = vmovn_s32(vaddq_s32(vshlq_n_s32(vandq_s32(v_Y1, v_mask), INTER_BITS),
                                                                     vandq_s32(v_X1, v_mask)));
                            vst1q_s16(alpha + x1, vcombine_s16(v_alpha0, v_alpha1));
                        }
                    #endif
                        for( ; x1 < bw; x1++ )
                        {
                            int X = (X0 + adelta[x+x1]) >> (AB_BITS - INTER_BITS);
                            int Y = (Y0 + bdelta[x+x1]) >> (AB_BITS - INTER_BITS);
                            xy[x1*2] = saturate_cast<short>(X >> INTER_BITS);
                            xy[x1*2+1] = saturate_cast<short>(Y >> INTER_BITS);
                            alpha[x1] = (short)((Y & (INTER_TAB_SIZE-1))*INTER_TAB_SIZE +
                                    (X & (INTER_TAB_SIZE-1)));
                        }
                    }
                }

                if( interpolation == INTER_NEAREST )
                    remap( src, dpart, _XY, Mat(), interpolation, borderType, borderValue );
                else
                {
                    Mat _matA(bh, bw, CV_16U, A);
                    remap( src, dpart, _XY, _matA, interpolation, borderType, borderValue );
                }
            }
        }
    }

private:
    Mat src;
    Mat dst;
    int interpolation, borderType;
    Scalar borderValue;
    int *adelta, *bdelta;
    double *M;
};

void cv::warpAffine( InputArray _src, OutputArray _dst,
                     InputArray _M0, Size dsize,
                     int flags, int borderType, const Scalar& borderValue )
{
    CV_OCL_RUN(_src.dims() <= 2 && _dst.isUMat(),
               ocl_warpTransform(_src, _dst, _M0, dsize, flags, borderType,
                                 borderValue, OCL_OP_AFFINE))

    Mat src = _src.getMat(), M0 = _M0.getMat();
    _dst.create( dsize.area() == 0 ? src.size() : dsize, src.type() );
    Mat dst = _dst.getMat();
    CV_Assert( src.cols > 0 && src.rows > 0 );
    if( dst.data == src.data )
        src = src.clone();

    double M[6];
    Mat matM(2, 3, CV_64F, M);
    int interpolation = flags & INTER_MAX;
    if( interpolation == INTER_AREA )
        interpolation = INTER_LINEAR;

    CV_Assert( (M0.type() == CV_32F || M0.type() == CV_64F) && M0.rows == 2 && M0.cols == 3 );
    M0.convertTo(matM, matM.type());

#ifdef HAVE_TEGRA_OPTIMIZATION
    if( tegra::useTegra() && tegra::warpAffine(src, dst, M, flags, borderType, borderValue) )
        return;
#endif

    if( !(flags & WARP_INVERSE_MAP) )
    {
        double D = M[0]*M[4] - M[1]*M[3];
        D = D != 0 ? 1./D : 0;
        double A11 = M[4]*D, A22=M[0]*D;
        M[0] = A11; M[1] *= -D;
        M[3] *= -D; M[4] = A22;
        double b1 = -M[0]*M[2] - M[1]*M[5];
        double b2 = -M[3]*M[2] - M[4]*M[5];
        M[2] = b1; M[5] = b2;
    }

    int x;
    AutoBuffer<int> _abdelta(dst.cols*2);
    int* adelta = &_abdelta[0], *bdelta = adelta + dst.cols;
    const int AB_BITS = MAX(10, (int)INTER_BITS);
    const int AB_SCALE = 1 << AB_BITS;

#if defined (HAVE_IPP) && IPP_VERSION_X100 >= 810 && IPP_DISABLE_BLOCK
    CV_IPP_CHECK()
    {
        int type = src.type(), depth = CV_MAT_DEPTH(type), cn = CV_MAT_CN(type);
        if( ( depth == CV_8U || depth == CV_16U || depth == CV_32F ) &&
           ( cn == 1 || cn == 3 || cn == 4 ) &&
           ( interpolation == INTER_NEAREST || interpolation == INTER_LINEAR || interpolation == INTER_CUBIC) &&
           ( borderType == cv::BORDER_TRANSPARENT || borderType == cv::BORDER_CONSTANT) )
        {
            ippiWarpAffineBackFunc ippFunc = 0;
            if ((flags & WARP_INVERSE_MAP) != 0)
            {
                ippFunc =
                type == CV_8UC1 ? (ippiWarpAffineBackFunc)ippiWarpAffineBack_8u_C1R :
                type == CV_8UC3 ? (ippiWarpAffineBackFunc)ippiWarpAffineBack_8u_C3R :
                type == CV_8UC4 ? (ippiWarpAffineBackFunc)ippiWarpAffineBack_8u_C4R :
                type == CV_16UC1 ? (ippiWarpAffineBackFunc)ippiWarpAffineBack_16u_C1R :
                type == CV_16UC3 ? (ippiWarpAffineBackFunc)ippiWarpAffineBack_16u_C3R :
                type == CV_16UC4 ? (ippiWarpAffineBackFunc)ippiWarpAffineBack_16u_C4R :
                type == CV_32FC1 ? (ippiWarpAffineBackFunc)ippiWarpAffineBack_32f_C1R :
                type == CV_32FC3 ? (ippiWarpAffineBackFunc)ippiWarpAffineBack_32f_C3R :
                type == CV_32FC4 ? (ippiWarpAffineBackFunc)ippiWarpAffineBack_32f_C4R :
                0;
            }
            else
            {
                ippFunc =
                type == CV_8UC1 ? (ippiWarpAffineBackFunc)ippiWarpAffine_8u_C1R :
                type == CV_8UC3 ? (ippiWarpAffineBackFunc)ippiWarpAffine_8u_C3R :
                type == CV_8UC4 ? (ippiWarpAffineBackFunc)ippiWarpAffine_8u_C4R :
                type == CV_16UC1 ? (ippiWarpAffineBackFunc)ippiWarpAffine_16u_C1R :
                type == CV_16UC3 ? (ippiWarpAffineBackFunc)ippiWarpAffine_16u_C3R :
                type == CV_16UC4 ? (ippiWarpAffineBackFunc)ippiWarpAffine_16u_C4R :
                type == CV_32FC1 ? (ippiWarpAffineBackFunc)ippiWarpAffine_32f_C1R :
                type == CV_32FC3 ? (ippiWarpAffineBackFunc)ippiWarpAffine_32f_C3R :
                type == CV_32FC4 ? (ippiWarpAffineBackFunc)ippiWarpAffine_32f_C4R :
                0;
            }
            int mode =
            interpolation == INTER_LINEAR ? IPPI_INTER_LINEAR :
            interpolation == INTER_NEAREST ? IPPI_INTER_NN :
            interpolation == INTER_CUBIC ? IPPI_INTER_CUBIC :
            0;
            CV_Assert(mode && ippFunc);

            double coeffs[2][3];
            for( int i = 0; i < 2; i++ )
                for( int j = 0; j < 3; j++ )
                    coeffs[i][j] = matM.at<double>(i, j);

            bool ok;
            Range range(0, dst.rows);
            IPPWarpAffineInvoker invoker(src, dst, coeffs, mode, borderType, borderValue, ippFunc, &ok);
            parallel_for_(range, invoker, dst.total()/(double)(1<<16));
            if( ok )
            {
                CV_IMPL_ADD(CV_IMPL_IPP|CV_IMPL_MT);
                return;
            }
            setIppErrorStatus();
        }
    }
#endif

    for( x = 0; x < dst.cols; x++ )
    {
        adelta[x] = saturate_cast<int>(M[0]*x*AB_SCALE);
        bdelta[x] = saturate_cast<int>(M[3]*x*AB_SCALE);
    }

    Range range(0, dst.rows);
    WarpAffineInvoker invoker(src, dst, interpolation, borderType,
                              borderValue, adelta, bdelta, M);
    parallel_for_(range, invoker, dst.total()/(double)(1<<16));
}

class WarpPerspectiveInvoker :
    public ParallelLoopBody
{
public:
    WarpPerspectiveInvoker(const Mat &_src, Mat &_dst, double *_M, int _interpolation,
                           int _borderType, const Scalar &_borderValue) :
        ParallelLoopBody(), src(_src), dst(_dst), M(_M), interpolation(_interpolation),
        borderType(_borderType), borderValue(_borderValue)
    {
    }

    virtual void operator() (const Range& range) const
    {
        const int BLOCK_SZ = 32;
        short XY[BLOCK_SZ*BLOCK_SZ*2], A[BLOCK_SZ*BLOCK_SZ];
        int x, y, x1, y1, width = dst.cols, height = dst.rows;

        int bh0 = std::min(BLOCK_SZ/2, height);
        int bw0 = std::min(BLOCK_SZ*BLOCK_SZ/bh0, width);
        bh0 = std::min(BLOCK_SZ*BLOCK_SZ/bw0, height);

        #if CV_SSE4_1
        bool haveSSE4_1 = checkHardwareSupport(CV_CPU_SSE4_1);
        __m128d v_M0 = _mm_set1_pd(M[0]);
        __m128d v_M3 = _mm_set1_pd(M[3]);
        __m128d v_M6 = _mm_set1_pd(M[6]);
        __m128d v_intmax = _mm_set1_pd((double)INT_MAX);
        __m128d v_intmin = _mm_set1_pd((double)INT_MIN);
        __m128d v_2 = _mm_set1_pd(2),
                v_zero = _mm_setzero_pd(),
                v_1 = _mm_set1_pd(1),
                v_its = _mm_set1_pd(INTER_TAB_SIZE);
        __m128i v_itsi1 = _mm_set1_epi32(INTER_TAB_SIZE - 1);
        #endif

        for( y = range.start; y < range.end; y += bh0 )
        {
            for( x = 0; x < width; x += bw0 )
            {
                int bw = std::min( bw0, width - x);
                int bh = std::min( bh0, range.end - y); // height

                Mat _XY(bh, bw, CV_16SC2, XY), matA;
                Mat dpart(dst, Rect(x, y, bw, bh));

                for( y1 = 0; y1 < bh; y1++ )
                {
                    short* xy = XY + y1*bw*2;
                    double X0 = M[0]*x + M[1]*(y + y1) + M[2];
                    double Y0 = M[3]*x + M[4]*(y + y1) + M[5];
                    double W0 = M[6]*x + M[7]*(y + y1) + M[8];

                    if( interpolation == INTER_NEAREST )
                    {
                        x1 = 0;

                        #if CV_SSE4_1
                        if (haveSSE4_1)
                        {
                            __m128d v_X0d = _mm_set1_pd(X0);
                            __m128d v_Y0d = _mm_set1_pd(Y0);
                            __m128d v_W0 = _mm_set1_pd(W0);
                            __m128d v_x1 = _mm_set_pd(1, 0);

                            for( ; x1 <= bw - 16; x1 += 16 )
                            {
                                // 0-3
                                __m128i v_X0, v_Y0;
                                {
                                    __m128d v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_1, v_W));
                                    __m128d v_fX0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_1, v_W));
                                    __m128d v_fX1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_X0 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fX0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fX1))));
                                    v_Y0 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fY0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fY1))));
                                }

                                // 4-8
                                __m128i v_X1, v_Y1;
                                {
                                    __m128d v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_1, v_W));
                                    __m128d v_fX0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_1, v_W));
                                    __m128d v_fX1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_X1 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fX0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fX1))));
                                    v_Y1 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fY0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fY1))));
                                }

                                // 8-11
                                __m128i v_X2, v_Y2;
                                {
                                    __m128d v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_1, v_W));
                                    __m128d v_fX0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_1, v_W));
                                    __m128d v_fX1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_X2 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fX0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fX1))));
                                    v_Y2 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fY0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fY1))));
                                }

                                // 12-15
                                __m128i v_X3, v_Y3;
                                {
                                    __m128d v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_1, v_W));
                                    __m128d v_fX0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_1, v_W));
                                    __m128d v_fX1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_X3 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fX0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fX1))));
                                    v_Y3 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fY0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fY1))));
                                }

                                // convert to 16s
                                v_X0 = _mm_packs_epi32(v_X0, v_X1);
                                v_X1 = _mm_packs_epi32(v_X2, v_X3);
                                v_Y0 = _mm_packs_epi32(v_Y0, v_Y1);
                                v_Y1 = _mm_packs_epi32(v_Y2, v_Y3);

                                _mm_interleave_epi16(v_X0, v_X1, v_Y0, v_Y1);

                                _mm_storeu_si128((__m128i *)(xy + x1 * 2), v_X0);
                                _mm_storeu_si128((__m128i *)(xy + x1 * 2 + 8), v_X1);
                                _mm_storeu_si128((__m128i *)(xy + x1 * 2 + 16), v_Y0);
                                _mm_storeu_si128((__m128i *)(xy + x1 * 2 + 24), v_Y1);
                            }
                        }
                        #endif

                        for( ; x1 < bw; x1++ )
                        {
                            double W = W0 + M[6]*x1;
                            W = W ? 1./W : 0;
                            double fX = std::max((double)INT_MIN, std::min((double)INT_MAX, (X0 + M[0]*x1)*W));
                            double fY = std::max((double)INT_MIN, std::min((double)INT_MAX, (Y0 + M[3]*x1)*W));
                            int X = saturate_cast<int>(fX);
                            int Y = saturate_cast<int>(fY);

                            xy[x1*2] = saturate_cast<short>(X);
                            xy[x1*2+1] = saturate_cast<short>(Y);
                        }
                    }
                    else
                    {
                        short* alpha = A + y1*bw;
                        x1 = 0;

                        #if CV_SSE4_1
                        if (haveSSE4_1)
                        {
                            __m128d v_X0d = _mm_set1_pd(X0);
                            __m128d v_Y0d = _mm_set1_pd(Y0);
                            __m128d v_W0 = _mm_set1_pd(W0);
                            __m128d v_x1 = _mm_set_pd(1, 0);

                            for( ; x1 <= bw - 16; x1 += 16 )
                            {
                                // 0-3
                                __m128i v_X0, v_Y0;
                                {
                                    __m128d v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_its, v_W));
                                    __m128d v_fX0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_its, v_W));
                                    __m128d v_fX1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_X0 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fX0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fX1))));
                                    v_Y0 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fY0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fY1))));
                                }

                                // 4-8
                                __m128i v_X1, v_Y1;
                                {
                                    __m128d v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_its, v_W));
                                    __m128d v_fX0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_its, v_W));
                                    __m128d v_fX1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_X1 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fX0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fX1))));
                                    v_Y1 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fY0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fY1))));
                                }

                                // 8-11
                                __m128i v_X2, v_Y2;
                                {
                                    __m128d v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_its, v_W));
                                    __m128d v_fX0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_its, v_W));
                                    __m128d v_fX1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_X2 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fX0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fX1))));
                                    v_Y2 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fY0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fY1))));
                                }

                                // 12-15
                                __m128i v_X3, v_Y3;
                                {
                                    __m128d v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_its, v_W));
                                    __m128d v_fX0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY0 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_W = _mm_add_pd(_mm_mul_pd(v_M6, v_x1), v_W0);
                                    v_W = _mm_andnot_pd(_mm_cmpeq_pd(v_W, v_zero), _mm_div_pd(v_its, v_W));
                                    __m128d v_fX1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_X0d, _mm_mul_pd(v_M0, v_x1)), v_W)));
                                    __m128d v_fY1 = _mm_max_pd(v_intmin, _mm_min_pd(v_intmax, _mm_mul_pd(_mm_add_pd(v_Y0d, _mm_mul_pd(v_M3, v_x1)), v_W)));
                                    v_x1 = _mm_add_pd(v_x1, v_2);

                                    v_X3 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fX0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fX1))));
                                    v_Y3 = _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(_mm_cvtpd_epi32(v_fY0)),
                                                                          _mm_castsi128_ps(_mm_cvtpd_epi32(v_fY1))));
                                }

                                // store alpha
                                __m128i v_alpha0 = _mm_add_epi32(_mm_slli_epi32(_mm_and_si128(v_Y0, v_itsi1), INTER_BITS),
                                                                 _mm_and_si128(v_X0, v_itsi1));
                                __m128i v_alpha1 = _mm_add_epi32(_mm_slli_epi32(_mm_and_si128(v_Y1, v_itsi1), INTER_BITS),
                                                                 _mm_and_si128(v_X1, v_itsi1));
                                _mm_storeu_si128((__m128i *)(alpha + x1), _mm_packs_epi32(v_alpha0, v_alpha1));

                                v_alpha0 = _mm_add_epi32(_mm_slli_epi32(_mm_and_si128(v_Y2, v_itsi1), INTER_BITS),
                                                         _mm_and_si128(v_X2, v_itsi1));
                                v_alpha1 = _mm_add_epi32(_mm_slli_epi32(_mm_and_si128(v_Y3, v_itsi1), INTER_BITS),
                                                         _mm_and_si128(v_X3, v_itsi1));
                                _mm_storeu_si128((__m128i *)(alpha + x1 + 8), _mm_packs_epi32(v_alpha0, v_alpha1));

                                // convert to 16s
                                v_X0 = _mm_packs_epi32(_mm_srai_epi32(v_X0, INTER_BITS), _mm_srai_epi32(v_X1, INTER_BITS));
                                v_X1 = _mm_packs_epi32(_mm_srai_epi32(v_X2, INTER_BITS), _mm_srai_epi32(v_X3, INTER_BITS));
                                v_Y0 = _mm_packs_epi32(_mm_srai_epi32(v_Y0, INTER_BITS), _mm_srai_epi32(v_Y1, INTER_BITS));
                                v_Y1 = _mm_packs_epi32(_mm_srai_epi32(v_Y2, INTER_BITS), _mm_srai_epi32(v_Y3, INTER_BITS));

                                _mm_interleave_epi16(v_X0, v_X1, v_Y0, v_Y1);

                                _mm_storeu_si128((__m128i *)(xy + x1 * 2), v_X0);
                                _mm_storeu_si128((__m128i *)(xy + x1 * 2 + 8), v_X1);
                                _mm_storeu_si128((__m128i *)(xy + x1 * 2 + 16), v_Y0);
                                _mm_storeu_si128((__m128i *)(xy + x1 * 2 + 24), v_Y1);
                            }
                        }
                        #endif

                        for( ; x1 < bw; x1++ )
                        {
                            double W = W0 + M[6]*x1;
                            W = W ? INTER_TAB_SIZE/W : 0;
                            double fX = std::max((double)INT_MIN, std::min((double)INT_MAX, (X0 + M[0]*x1)*W));
                            double fY = std::max((double)INT_MIN, std::min((double)INT_MAX, (Y0 + M[3]*x1)*W));
                            int X = saturate_cast<int>(fX);
                            int Y = saturate_cast<int>(fY);

                            xy[x1*2] = saturate_cast<short>(X >> INTER_BITS);
                            xy[x1*2+1] = saturate_cast<short>(Y >> INTER_BITS);
                            alpha[x1] = (short)((Y & (INTER_TAB_SIZE-1))*INTER_TAB_SIZE +
                                                (X & (INTER_TAB_SIZE-1)));
                        }
                    }
                }

                if( interpolation == INTER_NEAREST )
                    remap( src, dpart, _XY, Mat(), interpolation, borderType, borderValue );
                else
                {
                    Mat _matA(bh, bw, CV_16U, A);
                    remap( src, dpart, _XY, _matA, interpolation, borderType, borderValue );
                }
            }
        }
    }

private:
    Mat src;
    Mat dst;
    double* M;
    int interpolation, borderType;
    Scalar borderValue;
};

void cv::warpPerspective( InputArray _src, OutputArray _dst, InputArray _M0,
                          Size dsize, int flags, int borderType, const Scalar& borderValue )
{
    CV_Assert( _src.total() > 0 );

    CV_OCL_RUN(_src.dims() <= 2 && _dst.isUMat(),
               ocl_warpTransform(_src, _dst, _M0, dsize, flags, borderType, borderValue,
                              OCL_OP_PERSPECTIVE))

    Mat src = _src.getMat(), M0 = _M0.getMat();
    _dst.create( dsize.area() == 0 ? src.size() : dsize, src.type() );
    Mat dst = _dst.getMat();

    if( dst.data == src.data )
        src = src.clone();

    double M[9];
    Mat matM(3, 3, CV_64F, M);
    int interpolation = flags & INTER_MAX;
    if( interpolation == INTER_AREA )
        interpolation = INTER_LINEAR;

    CV_Assert( (M0.type() == CV_32F || M0.type() == CV_64F) && M0.rows == 3 && M0.cols == 3 );
    M0.convertTo(matM, matM.type());

#ifdef HAVE_TEGRA_OPTIMIZATION
    if( tegra::useTegra() && tegra::warpPerspective(src, dst, M, flags, borderType, borderValue) )
        return;
#endif


#if defined (HAVE_IPP) && IPP_VERSION_X100 >= 810 && IPP_DISABLE_BLOCK
    CV_IPP_CHECK()
    {
        int type = src.type(), depth = CV_MAT_DEPTH(type), cn = CV_MAT_CN(type);
        if( (depth == CV_8U || depth == CV_16U || depth == CV_32F) &&
           (cn == 1 || cn == 3 || cn == 4) &&
           ( borderType == cv::BORDER_TRANSPARENT || borderType == cv::BORDER_CONSTANT ) &&
           (interpolation == INTER_NEAREST || interpolation == INTER_LINEAR || interpolation == INTER_CUBIC))
        {
            ippiWarpPerspectiveFunc ippFunc = 0;
            if ((flags & WARP_INVERSE_MAP) != 0)
            {
                ippFunc = type == CV_8UC1 ? (ippiWarpPerspectiveFunc)ippiWarpPerspectiveBack_8u_C1R :
                type == CV_8UC3 ? (ippiWarpPerspectiveFunc)ippiWarpPerspectiveBack_8u_C3R :
                type == CV_8UC4 ? (ippiWarpPerspectiveFunc)ippiWarpPerspectiveBack_8u_C4R :
                type == CV_16UC1 ? (ippiWarpPerspectiveFunc)ippiWarpPerspectiveBack_16u_C1R :
                type == CV_16UC3 ? (ippiWarpPerspectiveFunc)ippiWarpPerspectiveBack_16u_C3R :
                type == CV_16UC4 ? (ippiWarpPerspectiveFunc)ippiWarpPerspectiveBack_16u_C4R :
                type == CV_32FC1 ? (ippiWarpPerspectiveFunc)ippiWarpPerspectiveBack_32f_C1R :
                type == CV_32FC3 ? (ippiWarpPerspectiveFunc)ippiWarpPerspectiveBack_32f_C3R :
                type == CV_32FC4 ? (ippiWarpPerspectiveFunc)ippiWarpPerspectiveBack_32f_C4R : 0;
            }
            else
            {
                ippFunc = type == CV_8UC1 ? (ippiWarpPerspectiveFunc)ippiWarpPerspective_8u_C1R :
                type == CV_8UC3 ? (ippiWarpPerspectiveFunc)ippiWarpPerspective_8u_C3R :
                type == CV_8UC4 ? (ippiWarpPerspectiveFunc)ippiWarpPerspective_8u_C4R :
                type == CV_16UC1 ? (ippiWarpPerspectiveFunc)ippiWarpPerspective_16u_C1R :
                type == CV_16UC3 ? (ippiWarpPerspectiveFunc)ippiWarpPerspective_16u_C3R :
                type == CV_16UC4 ? (ippiWarpPerspectiveFunc)ippiWarpPerspective_16u_C4R :
                type == CV_32FC1 ? (ippiWarpPerspectiveFunc)ippiWarpPerspective_32f_C1R :
                type == CV_32FC3 ? (ippiWarpPerspectiveFunc)ippiWarpPerspective_32f_C3R :
                type == CV_32FC4 ? (ippiWarpPerspectiveFunc)ippiWarpPerspective_32f_C4R : 0;
            }
            int mode =
            interpolation == INTER_NEAREST ? IPPI_INTER_NN :
            interpolation == INTER_LINEAR ? IPPI_INTER_LINEAR :
            interpolation == INTER_CUBIC ? IPPI_INTER_CUBIC : 0;
            CV_Assert(mode && ippFunc);

            double coeffs[3][3];
            for( int i = 0; i < 3; i++ )
                for( int j = 0; j < 3; j++ )
                    coeffs[i][j] = matM.at<double>(i, j);

            bool ok;
            Range range(0, dst.rows);
            IPPWarpPerspectiveInvoker invoker(src, dst, coeffs, mode, borderType, borderValue, ippFunc, &ok);
            parallel_for_(range, invoker, dst.total()/(double)(1<<16));
            if( ok )
            {
                CV_IMPL_ADD(CV_IMPL_IPP|CV_IMPL_MT);
                return;
            }
            setIppErrorStatus();
        }
    }
#endif

    if( !(flags & WARP_INVERSE_MAP) )
        invert(matM, matM);

    Range range(0, dst.rows);
    WarpPerspectiveInvoker invoker(src, dst, M, interpolation, borderType, borderValue);
    parallel_for_(range, invoker, dst.total()/(double)(1<<16));
}

cv::Mat cv::getRotationMatrix2D( Point2f center, double angle, double scale )
{
    angle *= CV_PI/180;
    double alpha = cos(angle)*scale;
    double beta = sin(angle)*scale;

    Mat M(2, 3, CV_64F);
    double* m = M.ptr<double>();

    m[0] = alpha;
    m[1] = beta;
    m[2] = (1-alpha)*center.x - beta*center.y;
    m[3] = -beta;
    m[4] = alpha;
    m[5] = beta*center.x + (1-alpha)*center.y;

    return M;
}

cv::Mat cv::getPerspectiveTransform( const Point2f src[], const Point2f dst[] )
{
    Mat M(3, 3, CV_64F), X(8, 1, CV_64F, M.ptr());
    double a[8][8], b[8];
    Mat A(8, 8, CV_64F, a), B(8, 1, CV_64F, b);

    for( int i = 0; i < 4; ++i )
    {
        a[i][0] = a[i+4][3] = src[i].x;
        a[i][1] = a[i+4][4] = src[i].y;
        a[i][2] = a[i+4][5] = 1;
        a[i][3] = a[i][4] = a[i][5] =
        a[i+4][0] = a[i+4][1] = a[i+4][2] = 0;
        a[i][6] = -src[i].x*dst[i].x;
        a[i][7] = -src[i].y*dst[i].x;
        a[i+4][6] = -src[i].x*dst[i].y;
        a[i+4][7] = -src[i].y*dst[i].y;
        b[i] = dst[i].x;
        b[i+4] = dst[i].y;
    }

    solve( A, B, X, DECOMP_SVD );
    M.ptr<double>()[8] = 1.;

    return M;
}

cv::Mat cv::getPerspectiveTransform( InputArray _src, InputArray _dst )
{
    Mat src = _src.getMat(), dst = _dst.getMat();
    CV_Assert(src.checkVector(2, CV_32F) == 4 && dst.checkVector(2, CV_32F) == 4);
    return getPerspectiveTransform((const Point2f*)src.data, (const Point2f*)dst.data);
}

void cvResize( const CvArr* srcarr, CvArr* dstarr, int method )
{
    cv::Mat src = cv::cvarrToMat(srcarr), dst = cv::cvarrToMat(dstarr);
    CV_Assert( src.type() == dst.type() );
    cv::resize( src, dst, dst.size(), (double)dst.cols/src.cols,
        (double)dst.rows/src.rows, method );
}

#endif
