// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Copyright (C) 2018 Intel Corporation

#if !defined(GAPI_STANDALONE)

#include "precomp.hpp"

#include <opencv2/gapi/own/assert.hpp>
#include <opencv2/core/traits.hpp>
#include <opencv2/core/hal/hal.hpp>
#include <opencv2/core/hal/intrin.hpp>

#include <opencv2/gapi/core.hpp>

#include <opencv2/gapi/fluid/gfluidbuffer.hpp>
#include <opencv2/gapi/fluid/gfluidkernel.hpp>
#include <opencv2/gapi/fluid/core.hpp>

#include "gfluidbuffer_priv.hpp"
#include "gfluidbackend.hpp"
#include "gfluidutils.hpp"

#include <cassert>
#include <cmath>
#include <cstdlib>

namespace cv {
namespace gapi {
namespace fluid {

//---------------------
//
// Arithmetic functions
//
//---------------------

template<typename DST, typename SRC1, typename SRC2>
static inline DST absdiff(SRC1 x, SRC2 y)
{
    auto result = x > y? x - y: y - x;
    return saturate<DST>(result, roundf);
}

template<typename DST, typename SRC1, typename SRC2>
static inline DST addWeighted(SRC1 src1, SRC2 src2, float alpha, float beta, float gamma)
{
    float dst = src1*alpha + src2*beta + gamma;
    return saturate<DST>(dst, roundf);
}

template<typename DST, typename SRC1, typename SRC2>
static inline DST add(SRC1 x, SRC2 y)
{
    return saturate<DST>(x + y, roundf);
}

template<typename DST, typename SRC1, typename SRC2>
static inline DST sub(SRC1 x, SRC2 y)
{
    return saturate<DST>(x - y, roundf);
}

template<typename DST, typename SRC1, typename SRC2>
static inline DST subr(SRC1 x, SRC2 y)
{
    return saturate<DST>(y - x, roundf); // reverse: y - x
}

template<typename DST, typename SRC1, typename SRC2>
static inline DST mul(SRC1 x, SRC2 y, float scale=1)
{
    auto result = scale * x * y;
    return saturate<DST>(result, rintf);
}

template<typename DST, typename SRC1, typename SRC2>
static inline DST div(SRC1 x, SRC2 y, float scale=1)
{
    // like OpenCV: returns 0, if y=0
    auto result = y? scale * x / y: 0;
    return saturate<DST>(result, rintf);
}

template<typename DST, typename SRC1, typename SRC2>
static inline DST divr(SRC1 x, SRC2 y, float scale=1)
{
    auto result = x? scale * y / x: 0; // reverse: y / x
    return saturate<DST>(result, rintf);
}

//---------------------------
//
// Fluid kernels: addWeighted
//
//---------------------------

template<typename DST, typename SRC1, typename SRC2>
static void run_addweighted(Buffer &dst, const View &src1, const View &src2,
                            double alpha, double beta, double gamma)
{
    static_assert(std::is_same<SRC1, SRC2>::value, "wrong types");

    const auto *in1 = src1.InLine<SRC1>(0);
    const auto *in2 = src2.InLine<SRC2>(0);
          auto *out = dst.OutLine<DST>();

    int width  = dst.length();
    int chan   = dst.meta().chan;
    int length = width * chan;

    // NB: assume in/out types are not 64-bits
    auto _alpha = static_cast<float>( alpha );
    auto _beta  = static_cast<float>( beta  );
    auto _gamma = static_cast<float>( gamma );

    for (int l=0; l < length; l++)
        out[l] = addWeighted<DST>(in1[l], in2[l], _alpha, _beta, _gamma);
}

GAPI_FLUID_KERNEL(GFluidAddW, cv::gapi::core::GAddW, false)
{
    static const int Window = 1;

    static void run(const View &src1, double alpha, const View &src2,
                                      double beta, double gamma, int /*dtype*/,
                        Buffer &dst)
    {
        //      DST     SRC1    SRC2    OP               __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_addweighted, dst, src1, src2, alpha, beta, gamma);
        BINARY_(uchar , ushort, ushort, run_addweighted, dst, src1, src2, alpha, beta, gamma);
        BINARY_(uchar ,  short,  short, run_addweighted, dst, src1, src2, alpha, beta, gamma);
        BINARY_( short,  short,  short, run_addweighted, dst, src1, src2, alpha, beta, gamma);
        BINARY_(ushort, ushort, ushort, run_addweighted, dst, src1, src2, alpha, beta, gamma);
        BINARY_( float, uchar , uchar , run_addweighted, dst, src1, src2, alpha, beta, gamma);
        BINARY_( float, ushort, ushort, run_addweighted, dst, src1, src2, alpha, beta, gamma);
        BINARY_( float,  short,  short, run_addweighted, dst, src1, src2, alpha, beta, gamma);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//--------------------------
//
// Fluid kernels: +, -, *, /
//
//--------------------------

enum Arithm { ARITHM_ABSDIFF, ARITHM_ADD, ARITHM_SUBTRACT, ARITHM_MULTIPLY, ARITHM_DIVIDE };

template<typename DST, typename SRC1, typename SRC2>
static void run_arithm(Buffer &dst, const View &src1, const View &src2, Arithm arithm,
                       double scale=1)
{
    static_assert(std::is_same<SRC1, SRC2>::value, "wrong types");

    const auto *in1 = src1.InLine<SRC1>(0);
    const auto *in2 = src2.InLine<SRC2>(0);
          auto *out = dst.OutLine<DST>();

    int width  = dst.length();
    int chan   = dst.meta().chan;
    int length = width * chan;

    // NB: assume in/out types are not 64-bits
    float _scale = static_cast<float>( scale );

    switch (arithm)
    {
    case ARITHM_ABSDIFF:
        for (int l=0; l < length; l++)
            out[l] = absdiff<DST>(in1[l], in2[l]);
        break;
    case ARITHM_ADD:
        for (int l=0; l < length; l++)
            out[l] = add<DST>(in1[l], in2[l]);
        break;
    case ARITHM_SUBTRACT:
        for (int l=0; l < length; l++)
            out[l] = sub<DST>(in1[l], in2[l]);
        break;
    case ARITHM_MULTIPLY:
        for (int l=0; l < length; l++)
            out[l] = mul<DST>(in1[l], in2[l], _scale);
        break;
    case ARITHM_DIVIDE:
        for (int l=0; l < length; l++)
            out[l] = div<DST>(in1[l], in2[l], _scale);
        break;
    default: CV_Error(cv::Error::StsBadArg, "unsupported arithmetic operation");
    }
}

GAPI_FLUID_KERNEL(GFluidAdd, cv::gapi::core::GAdd, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, int /*dtype*/, Buffer &dst)
    {
        //      DST     SRC1    SRC2    OP          __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_arithm, dst, src1, src2, ARITHM_ADD);
        BINARY_(uchar ,  short,  short, run_arithm, dst, src1, src2, ARITHM_ADD);
        BINARY_(uchar ,  float,  float, run_arithm, dst, src1, src2, ARITHM_ADD);
        BINARY_( short,  short,  short, run_arithm, dst, src1, src2, ARITHM_ADD);
        BINARY_( float, uchar , uchar , run_arithm, dst, src1, src2, ARITHM_ADD);
        BINARY_( float,  short,  short, run_arithm, dst, src1, src2, ARITHM_ADD);
        BINARY_( float,  float,  float, run_arithm, dst, src1, src2, ARITHM_ADD);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidSub, cv::gapi::core::GSub, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, int /*dtype*/, Buffer &dst)
    {
        //      DST     SRC1    SRC2    OP          __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_arithm, dst, src1, src2, ARITHM_SUBTRACT);
        BINARY_(uchar ,  short,  short, run_arithm, dst, src1, src2, ARITHM_SUBTRACT);
        BINARY_(uchar ,  float,  float, run_arithm, dst, src1, src2, ARITHM_SUBTRACT);
        BINARY_( short,  short,  short, run_arithm, dst, src1, src2, ARITHM_SUBTRACT);
        BINARY_( float, uchar , uchar , run_arithm, dst, src1, src2, ARITHM_SUBTRACT);
        BINARY_( float,  short,  short, run_arithm, dst, src1, src2, ARITHM_SUBTRACT);
        BINARY_( float,  float,  float, run_arithm, dst, src1, src2, ARITHM_SUBTRACT);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidMul, cv::gapi::core::GMul, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, double scale, int /*dtype*/, Buffer &dst)
    {
        //      DST     SRC1    SRC2    OP          __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_arithm, dst, src1, src2, ARITHM_MULTIPLY, scale);
        BINARY_(uchar ,  short,  short, run_arithm, dst, src1, src2, ARITHM_MULTIPLY, scale);
        BINARY_(uchar ,  float,  float, run_arithm, dst, src1, src2, ARITHM_MULTIPLY, scale);
        BINARY_( short,  short,  short, run_arithm, dst, src1, src2, ARITHM_MULTIPLY, scale);
        BINARY_( float, uchar , uchar , run_arithm, dst, src1, src2, ARITHM_MULTIPLY, scale);
        BINARY_( float,  short,  short, run_arithm, dst, src1, src2, ARITHM_MULTIPLY, scale);
        BINARY_( float,  float,  float, run_arithm, dst, src1, src2, ARITHM_MULTIPLY, scale);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidDiv, cv::gapi::core::GDiv, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, double scale, int /*dtype*/, Buffer &dst)
    {
        //      DST     SRC1    SRC2    OP          __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_arithm, dst, src1, src2, ARITHM_DIVIDE, scale);
        BINARY_(uchar ,  short,  short, run_arithm, dst, src1, src2, ARITHM_DIVIDE, scale);
        BINARY_(uchar ,  float,  float, run_arithm, dst, src1, src2, ARITHM_DIVIDE, scale);
        BINARY_( short,  short,  short, run_arithm, dst, src1, src2, ARITHM_DIVIDE, scale);
        BINARY_( float, uchar , uchar , run_arithm, dst, src1, src2, ARITHM_DIVIDE, scale);
        BINARY_( float,  short,  short, run_arithm, dst, src1, src2, ARITHM_DIVIDE, scale);
        BINARY_( float,  float,  float, run_arithm, dst, src1, src2, ARITHM_DIVIDE, scale);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidAbsDiff, cv::gapi::core::GAbsDiff, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {
        //      DST     SRC1    SRC2    OP          __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_arithm, dst, src1, src2, ARITHM_ABSDIFF);
        BINARY_(ushort, ushort, ushort, run_arithm, dst, src1, src2, ARITHM_ABSDIFF);
        BINARY_( short,  short,  short, run_arithm, dst, src1, src2, ARITHM_ABSDIFF);
        BINARY_( float,  float,  float, run_arithm, dst, src1, src2, ARITHM_ABSDIFF);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//--------------------------------------
//
// Fluid kernels: +, -, *, / with Scalar
//
//--------------------------------------

static inline v_uint16x8  v_add_16u(const v_uint16x8 &x, const v_uint16x8 &y) { return x + y; }
static inline v_uint16x8  v_sub_16u(const v_uint16x8 &x, const v_uint16x8 &y) { return x - y; }
static inline v_uint16x8 v_subr_16u(const v_uint16x8 &x, const v_uint16x8 &y) { return y - x; }

static inline v_float32x4  v_add_32f(const v_float32x4 &x, const v_float32x4 &y) { return x + y; }
static inline v_float32x4  v_sub_32f(const v_float32x4 &x, const v_float32x4 &y) { return x - y; }
static inline v_float32x4 v_subr_32f(const v_float32x4 &x, const v_float32x4 &y) { return y - x; }

static inline int  s_add_8u(uchar x, uchar y) { return x + y; }
static inline int  s_sub_8u(uchar x, uchar y) { return x - y; }
static inline int s_subr_8u(uchar x, uchar y) { return y - x; }

static inline float  s_add_32f(float x, float y) { return x + y; }
static inline float  s_sub_32f(float x, float y) { return x - y; }
static inline float s_subr_32f(float x, float y) { return y - x; }

// manual SIMD if important case 8UC3
static void run_arithm_s3(uchar out[], const uchar in[], int width, const uchar scalar[],
                          v_uint16x8 (*v_op)(const v_uint16x8&, const v_uint16x8&),
                          int (*s_op)(uchar, uchar))
{
    int w = 0;

#if CV_SIMD128
    for (; w <= width-16; w+=16)
    {
        v_uint8x16 x, y, z;
        v_load_deinterleave(&in[3*w], x, y, z);

        v_uint16x8 r0, r1;

        v_expand(x, r0, r1);
        r0 = v_op(r0, v_setall_u16(scalar[0])); // x + scalar[0]
        r1 = v_op(r1, v_setall_u16(scalar[0]));
        x = v_pack(r0, r1);

        v_expand(y, r0, r1);
        r0 = v_op(r0, v_setall_u16(scalar[1])); // y + scalar[1]
        r1 = v_op(r1, v_setall_u16(scalar[1]));
        y = v_pack(r0, r1);

        v_expand(z, r0, r1);
        r0 = v_op(r0, v_setall_u16(scalar[2])); // z + scalar[2]
        r1 = v_op(r1, v_setall_u16(scalar[2]));
        z = v_pack(r0, r1);

        v_store_interleave(&out[3*w], x, y, z);
    }
#endif
    cv::util::suppress_unused_warning(v_op);
    for (; w < width; w++)
    {
        out[3*w    ] = saturate<uchar>( s_op(in[3*w    ], scalar[0]) );
        out[3*w + 1] = saturate<uchar>( s_op(in[3*w + 1], scalar[1]) );
        out[3*w + 2] = saturate<uchar>( s_op(in[3*w + 2], scalar[2]) );
    }
}

// manually SIMD if rounding 32F into 8U, single channel
static void run_arithm_s1(uchar out[], const float in[], int width, const float scalar[],
                          v_float32x4 (*v_op)(const v_float32x4&, const v_float32x4&),
                          float (*s_op)(float, float))
{
    int w = 0;

#if CV_SIMD128
    for (; w <= width-16; w+=16)
    {
        v_float32x4 r0, r1, r2, r3;
        r0 = v_load(&in[w     ]);
        r1 = v_load(&in[w +  4]);
        r2 = v_load(&in[w +  8]);
        r3 = v_load(&in[w + 12]);

        r0 = v_op(r0, v_setall_f32(scalar[0])); // r + scalar[0]
        r1 = v_op(r1, v_setall_f32(scalar[0]));
        r2 = v_op(r2, v_setall_f32(scalar[0]));
        r3 = v_op(r3, v_setall_f32(scalar[0]));

        v_int32x4 i0, i1, i2, i3;
        i0 = v_round(r0);
        i1 = v_round(r1);
        i2 = v_round(r2);
        i3 = v_round(r3);

        v_uint16x8 us0, us1;
        us0 = v_pack_u(i0, i1);
        us1 = v_pack_u(i2, i3);

        v_uint8x16 uc;
        uc = v_pack(us0, us1);

        v_store(&out[w], uc);
    }
#endif
    cv::util::suppress_unused_warning(v_op);
    for (; w < width; w++)
    {
        out[w] = saturate<uchar>(s_op(in[w], scalar[0]), std::roundf);
    }
}

static void run_arithm_s_add3(uchar out[], const uchar in[], int width, const uchar scalar[])
{
    run_arithm_s3(out, in, width, scalar, v_add_16u, s_add_8u);
}

static void run_arithm_s_sub3(uchar out[], const uchar in[], int width, const uchar scalar[])
{
    run_arithm_s3(out, in, width, scalar, v_sub_16u, s_sub_8u);
}

static void run_arithm_s_subr3(uchar out[], const uchar in[], int width, const uchar scalar[])
{
    run_arithm_s3(out, in, width, scalar, v_subr_16u, s_subr_8u); // reverse: subr
}

static void run_arithm_s_add1(uchar out[], const float in[], int width, const float scalar[])
{
    run_arithm_s1(out, in, width, scalar, v_add_32f, s_add_32f);
}

static void run_arithm_s_sub1(uchar out[], const float in[], int width, const float scalar[])
{
    run_arithm_s1(out, in, width, scalar, v_sub_32f, s_sub_32f);
}

static void run_arithm_s_subr1(uchar out[], const float in[], int width, const float scalar[])
{
    run_arithm_s1(out, in, width, scalar, v_subr_32f, s_subr_32f); // reverse: subr
}

// manually unroll the inner cycle by channels
template<typename DST, typename SRC, typename SCALAR, typename FUNC>
static void run_arithm_s(DST out[], const SRC in[], int width, int chan,
                         const SCALAR scalar[4], FUNC func)
{
    if (chan == 4)
    {
        for (int w=0; w < width; w++)
        {
            out[4*w + 0] = func(in[4*w + 0], scalar[0]);
            out[4*w + 1] = func(in[4*w + 1], scalar[1]);
            out[4*w + 2] = func(in[4*w + 2], scalar[2]);
            out[4*w + 3] = func(in[4*w + 3], scalar[3]);
        }
    }
    else
    if (chan == 3)
    {
        for (int w=0; w < width; w++)
        {
            out[3*w + 0] = func(in[3*w + 0], scalar[0]);
            out[3*w + 1] = func(in[3*w + 1], scalar[1]);
            out[3*w + 2] = func(in[3*w + 2], scalar[2]);
        }
    }
    else
    if (chan == 2)
    {
        for (int w=0; w < width; w++)
        {
            out[2*w + 0] = func(in[2*w + 0], scalar[0]);
            out[2*w + 1] = func(in[2*w + 1], scalar[1]);
        }
    }
    else
    if (chan == 1)
    {
        for (int w=0; w < width; w++)
        {
            out[w] = func(in[w], scalar[0]);
        }
    }
    else
        CV_Error(cv::Error::StsBadArg, "unsupported number of channels");
}

template<typename DST, typename SRC>
static void run_arithm_s(Buffer &dst, const View &src, const float scalar[4], Arithm arithm,
                         float scale=1)
{
    const auto *in  = src.InLine<SRC>(0);
          auto *out = dst.OutLine<DST>();

    int width  = dst.length();
    int chan   = dst.meta().chan;

    // What if we cast the scalar into the SRC type?
    const SRC myscal[4] = { static_cast<SRC>(scalar[0]), static_cast<SRC>(scalar[1]),
                            static_cast<SRC>(scalar[2]), static_cast<SRC>(scalar[3]) };
    bool usemyscal = (myscal[0] == scalar[0]) && (myscal[1] == scalar[1]) &&
                     (myscal[2] == scalar[2]) && (myscal[3] == scalar[3]);

    switch (arithm)
    {
    case ARITHM_ABSDIFF:
        for (int w=0; w < width; w++)
            for (int c=0; c < chan; c++)
                out[chan*w + c] = absdiff<DST>(in[chan*w + c], scalar[c]);
        break;
    case ARITHM_ADD:
        if (usemyscal)
        {
            if (std::is_same<DST,uchar>::value &&
                std::is_same<SRC,uchar>::value &&
                chan == 3)
                run_arithm_s_add3((uchar*)out, (const uchar*)in, width, (const uchar*)myscal);
            else if (std::is_same<DST,uchar>::value &&
                     std::is_same<SRC,float>::value &&
                     chan == 1)
                run_arithm_s_add1((uchar*)out, (const float*)in, width, (const float*)myscal);
            else
                run_arithm_s(out, in, width, chan, myscal, add<DST,SRC,SRC>);
        }
        else
            run_arithm_s(out, in, width, chan, scalar, add<DST,SRC,float>);
        break;
    case ARITHM_SUBTRACT:
        if (usemyscal)
        {
            if (std::is_same<DST,uchar>::value &&
                std::is_same<SRC,uchar>::value &&
                chan == 3)
                run_arithm_s_sub3((uchar*)out, (const uchar*)in, width, (const uchar*)myscal);
            else if (std::is_same<DST,uchar>::value &&
                     std::is_same<SRC,float>::value &&
                     chan == 1)
                run_arithm_s_sub1((uchar*)out, (const float*)in, width, (const float*)myscal);
            else
                run_arithm_s(out, in, width, chan, myscal, sub<DST,SRC,SRC>);
        }
        else
            run_arithm_s(out, in, width, chan, scalar, sub<DST,SRC,float>);
        break;
    // TODO: optimize miltiplication and division
    case ARITHM_MULTIPLY:
        for (int w=0; w < width; w++)
            for (int c=0; c < chan; c++)
                out[chan*w + c] = mul<DST>(in[chan*w + c], scalar[c], scale);
        break;
    case ARITHM_DIVIDE:
        for (int w=0; w < width; w++)
            for (int c=0; c < chan; c++)
                out[chan*w + c] = div<DST>(in[chan*w + c], scalar[c], scale);
        break;
    default: CV_Error(cv::Error::StsBadArg, "unsupported arithmetic operation");
    }
}

template<typename DST, typename SRC>
static void run_arithm_rs(Buffer &dst, const View &src, const float scalar[4], Arithm arithm,
                          float scale=1)
{
    const auto *in  = src.InLine<SRC>(0);
          auto *out = dst.OutLine<DST>();

    int width  = dst.length();
    int chan   = dst.meta().chan;

    // What if we cast the scalar into the SRC type?
    const SRC myscal[4] = { static_cast<SRC>(scalar[0]), static_cast<SRC>(scalar[1]),
                            static_cast<SRC>(scalar[2]), static_cast<SRC>(scalar[3]) };
    bool usemyscal = (myscal[0] == scalar[0]) && (myscal[1] == scalar[1]) &&
                     (myscal[2] == scalar[2]) && (myscal[3] == scalar[3]);

    switch (arithm)
    {
    case ARITHM_SUBTRACT:
        if (usemyscal)
        {
            if (std::is_same<DST,uchar>::value &&
                std::is_same<SRC,uchar>::value &&
                chan == 3)
                run_arithm_s_subr3((uchar*)out, (const uchar*)in, width, (const uchar*)myscal);
            else if (std::is_same<DST,uchar>::value &&
                     std::is_same<SRC,float>::value &&
                     chan == 1)
                run_arithm_s_subr1((uchar*)out, (const float*)in, width, (const float*)myscal);
            else
                run_arithm_s(out, in, width, chan, myscal, subr<DST,SRC,SRC>);
        }
        else
            run_arithm_s(out, in, width, chan, scalar, subr<DST,SRC,float>);
        break;
    // TODO: optimize division
    case ARITHM_DIVIDE:
        for (int w=0; w < width; w++)
            for (int c=0; c < chan; c++)
                out[chan*w + c] = div<DST>(scalar[c], in[chan*w + c], scale);
        break;
    default: CV_Error(cv::Error::StsBadArg, "unsupported arithmetic operation");
    }
}

GAPI_FLUID_KERNEL(GFluidAbsDiffC, cv::gapi::core::GAbsDiffC, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &_scalar, Buffer &dst)
    {
        const float scalar[4] = {
            static_cast<float>(_scalar[0]),
            static_cast<float>(_scalar[1]),
            static_cast<float>(_scalar[2]),
            static_cast<float>(_scalar[3])
        };

        //     DST     SRC     OP            __VA_ARGS__
        UNARY_(uchar , uchar , run_arithm_s, dst, src, scalar, ARITHM_ABSDIFF);
        UNARY_(ushort, ushort, run_arithm_s, dst, src, scalar, ARITHM_ABSDIFF);
        UNARY_( short,  short, run_arithm_s, dst, src, scalar, ARITHM_ABSDIFF);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidAddC, cv::gapi::core::GAddC, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &_scalar, int /*dtype*/, Buffer &dst)
    {
        const float scalar[4] = {
            static_cast<float>(_scalar[0]),
            static_cast<float>(_scalar[1]),
            static_cast<float>(_scalar[2]),
            static_cast<float>(_scalar[3])
        };

        //     DST     SRC     OP            __VA_ARGS__
        UNARY_(uchar , uchar , run_arithm_s, dst, src, scalar, ARITHM_ADD);
        UNARY_(uchar ,  short, run_arithm_s, dst, src, scalar, ARITHM_ADD);
        UNARY_(uchar ,  float, run_arithm_s, dst, src, scalar, ARITHM_ADD);
        UNARY_( short,  short, run_arithm_s, dst, src, scalar, ARITHM_ADD);
        UNARY_( float, uchar , run_arithm_s, dst, src, scalar, ARITHM_ADD);
        UNARY_( float,  short, run_arithm_s, dst, src, scalar, ARITHM_ADD);
        UNARY_( float,  float, run_arithm_s, dst, src, scalar, ARITHM_ADD);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidSubC, cv::gapi::core::GSubC, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &_scalar, int /*dtype*/, Buffer &dst)
    {
        const float scalar[4] = {
            static_cast<float>(_scalar[0]),
            static_cast<float>(_scalar[1]),
            static_cast<float>(_scalar[2]),
            static_cast<float>(_scalar[3])
        };

        //     DST     SRC     OP            __VA_ARGS__
        UNARY_(uchar , uchar , run_arithm_s, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_(uchar ,  short, run_arithm_s, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_(uchar ,  float, run_arithm_s, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_( short,  short, run_arithm_s, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_( float, uchar , run_arithm_s, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_( float,  short, run_arithm_s, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_( float,  float, run_arithm_s, dst, src, scalar, ARITHM_SUBTRACT);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidSubRC, cv::gapi::core::GSubRC, false)
{
    static const int Window = 1;

    static void run(const cv::Scalar &_scalar, const View &src, int /*dtype*/, Buffer &dst)
    {
        const float scalar[4] = {
            static_cast<float>(_scalar[0]),
            static_cast<float>(_scalar[1]),
            static_cast<float>(_scalar[2]),
            static_cast<float>(_scalar[3])
        };

        //     DST     SRC     OP             __VA_ARGS__
        UNARY_(uchar , uchar , run_arithm_rs, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_(uchar ,  short, run_arithm_rs, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_(uchar ,  float, run_arithm_rs, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_( short,  short, run_arithm_rs, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_( float, uchar , run_arithm_rs, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_( float,  short, run_arithm_rs, dst, src, scalar, ARITHM_SUBTRACT);
        UNARY_( float,  float, run_arithm_rs, dst, src, scalar, ARITHM_SUBTRACT);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidMulC, cv::gapi::core::GMulC, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &_scalar, int /*dtype*/, Buffer &dst)
    {
        const float scalar[4] = {
            static_cast<float>(_scalar[0]),
            static_cast<float>(_scalar[1]),
            static_cast<float>(_scalar[2]),
            static_cast<float>(_scalar[3])
        };
        const float scale = 1.f;

        //     DST     SRC     OP            __VA_ARGS__
        UNARY_(uchar , uchar , run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_(uchar ,  short, run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_(uchar ,  float, run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_( short,  short, run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_( float, uchar , run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_( float,  short, run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_( float,  float, run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidMulCOld, cv::gapi::core::GMulCOld, false)
{
    static const int Window = 1;

    static void run(const View &src, double _scalar, int /*dtype*/, Buffer &dst)
    {
        const float scalar[4] = {
            static_cast<float>(_scalar),
            static_cast<float>(_scalar),
            static_cast<float>(_scalar),
            static_cast<float>(_scalar)
        };
        const float scale = 1.f;

        //     DST     SRC     OP            __VA_ARGS__
        UNARY_(uchar , uchar , run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_(uchar ,  short, run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_(uchar ,  float, run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_( short,  short, run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_( float, uchar , run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_( float,  short, run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);
        UNARY_( float,  float, run_arithm_s, dst, src, scalar, ARITHM_MULTIPLY, scale);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidDivC, cv::gapi::core::GDivC, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &_scalar, double _scale, int /*dtype*/,
                    Buffer &dst)
    {
        const float scalar[4] = {
            static_cast<float>(_scalar[0]),
            static_cast<float>(_scalar[1]),
            static_cast<float>(_scalar[2]),
            static_cast<float>(_scalar[3])
        };
        const float scale = static_cast<float>(_scale);

        //     DST     SRC     OP            __VA_ARGS__
        UNARY_(uchar , uchar , run_arithm_s, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_(uchar ,  short, run_arithm_s, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_(uchar ,  float, run_arithm_s, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_( short,  short, run_arithm_s, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_( float, uchar , run_arithm_s, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_( float,  short, run_arithm_s, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_( float,  float, run_arithm_s, dst, src, scalar, ARITHM_DIVIDE, scale);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidDivRC, cv::gapi::core::GDivRC, false)
{
    static const int Window = 1;

    static void run(const cv::Scalar &_scalar, const View &src, double _scale, int /*dtype*/,
                    Buffer &dst)
    {
        const float scalar[4] = {
            static_cast<float>(_scalar[0]),
            static_cast<float>(_scalar[1]),
            static_cast<float>(_scalar[2]),
            static_cast<float>(_scalar[3])
        };
        const float scale = static_cast<float>(_scale);

        //     DST     SRC     OP             __VA_ARGS__
        UNARY_(uchar , uchar , run_arithm_rs, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_(uchar ,  short, run_arithm_rs, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_(uchar ,  float, run_arithm_rs, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_( short,  short, run_arithm_rs, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_( float, uchar , run_arithm_rs, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_( float,  short, run_arithm_rs, dst, src, scalar, ARITHM_DIVIDE, scale);
        UNARY_( float,  float, run_arithm_rs, dst, src, scalar, ARITHM_DIVIDE, scale);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//----------------------------
//
// Fluid math kernels: bitwise
//
//----------------------------

enum Bitwise { BW_AND, BW_OR, BW_XOR, BW_NOT };

template<typename DST, typename SRC1, typename SRC2>
static void run_bitwise2(Buffer &dst, const View &src1, const View &src2, Bitwise bitwise)
{
    static_assert(std::is_same<DST, SRC1>::value, "wrong types");
    static_assert(std::is_same<DST, SRC2>::value, "wrong types");

    const auto *in1 = src1.InLine<SRC1>(0);
    const auto *in2 = src2.InLine<SRC2>(0);
          auto *out = dst.OutLine<DST>();

    int width  = dst.length();
    int chan   = dst.meta().chan;
    int length = width * chan;

    switch (bitwise)
    {
    case BW_AND:
        for (int l=0; l < length; l++)
            out[l] = in1[l] & in2[l];
        break;
    case BW_OR:
        for (int l=0; l < length; l++)
            out[l] = in1[l] | in2[l];
        break;
    case BW_XOR:
        for (int l=0; l < length; l++)
            out[l] = in1[l] ^ in2[l];
        break;
    default: CV_Error(cv::Error::StsBadArg, "unsupported bitwise operation");
    }
}

template<typename DST, typename SRC>
static void run_bitwise1(Buffer &dst, const View &src, Bitwise bitwise)
{
    static_assert(std::is_same<DST, SRC>::value, "wrong types");

    const auto *in  = src.InLine<SRC>(0);
          auto *out = dst.OutLine<DST>();

    int width  = dst.length();
    int chan   = dst.meta().chan;
    int length = width * chan;

    switch (bitwise)
    {
    case BW_NOT:
        for (int l=0; l < length; l++)
            out[l] = ~in[l];
        break;
    default: CV_Error(cv::Error::StsBadArg, "unsupported bitwise operation");
    }
}

GAPI_FLUID_KERNEL(GFluidAnd, cv::gapi::core::GAnd, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {

        //      DST     SRC1    SRC2    OP            __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_bitwise2, dst, src1, src2, BW_AND);
        BINARY_(ushort, ushort, ushort, run_bitwise2, dst, src1, src2, BW_AND);
        BINARY_( short,  short,  short, run_bitwise2, dst, src1, src2, BW_AND);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidOr, cv::gapi::core::GOr, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {

        //      DST     SRC1    SRC2    OP            __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_bitwise2, dst, src1, src2, BW_OR);
        BINARY_(ushort, ushort, ushort, run_bitwise2, dst, src1, src2, BW_OR);
        BINARY_( short,  short,  short, run_bitwise2, dst, src1, src2, BW_OR);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidXor, cv::gapi::core::GXor, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {

        //      DST     SRC1    SRC2    OP            __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_bitwise2, dst, src1, src2, BW_XOR);
        BINARY_(ushort, ushort, ushort, run_bitwise2, dst, src1, src2, BW_XOR);
        BINARY_( short,  short,  short, run_bitwise2, dst, src1, src2, BW_XOR);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidNot, cv::gapi::core::GNot, false)
{
    static const int Window = 1;

    static void run(const View &src, Buffer &dst)
    {
        //     DST     SRC     OP            __VA_ARGS__
        UNARY_(uchar , uchar , run_bitwise1, dst, src, BW_NOT);
        UNARY_(ushort, ushort, run_bitwise1, dst, src, BW_NOT);
        UNARY_( short,  short, run_bitwise1, dst, src, BW_NOT);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//-------------------
//
// Fluid kernels: LUT
//
//-------------------

GAPI_FLUID_KERNEL(GFluidLUT, cv::gapi::core::GLUT, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Mat& lut, Buffer &dst)
    {
        GAPI_Assert(CV_8U == dst.meta().depth);
        GAPI_Assert(CV_8U == src.meta().depth);

        GAPI_DbgAssert(CV_8U == lut.type());
        GAPI_DbgAssert(256 == lut.cols * lut.rows);
        GAPI_DbgAssert(dst.length() == src.length());
        GAPI_DbgAssert(dst.meta().chan == src.meta().chan);

        const auto *in  = src.InLine<uchar>(0);
              auto *out = dst.OutLine<uchar>();

        int width  = dst.length();
        int chan   = dst.meta().chan;
        int length = width * chan;

        for (int l=0; l < length; l++)
            out[l] = lut.data[ in[l] ];
    }
};

//-------------------------
//
// Fluid kernels: convertTo
//
//-------------------------

template<typename DST, typename SRC>
static void run_convertto(Buffer &dst, const View &src, double _alpha, double _beta)
{
    const auto *in  = src.InLine<SRC>(0);
          auto *out = dst.OutLine<DST>();

    int width  = dst.length();
    int chan   = dst.meta().chan;
    int length = width * chan;

    // NB: don't do this if SRC or DST is 64-bit
    auto alpha = static_cast<float>( _alpha );
    auto beta  = static_cast<float>( _beta  );

    // compute faster if no alpha no beta
    if (alpha == 1 && beta == 0)
    {
        // manual SIMD if need rounding
        if (std::is_integral<DST>::value && std::is_floating_point<SRC>::value)
        {
            GAPI_Assert(( std::is_same<SRC,float>::value ));

            int l = 0; // cycle index

        #if CV_SIMD128
            if (std::is_same<DST,uchar>::value)
            {
                for (; l <= length-16; l+=16)
                {
                    v_int32x4 i0, i1, i2, i3;
                    i0 = v_round( v_load( (float*)& in[l     ] ) );
                    i1 = v_round( v_load( (float*)& in[l +  4] ) );
                    i2 = v_round( v_load( (float*)& in[l +  8] ) );
                    i3 = v_round( v_load( (float*)& in[l + 12] ) );

                    v_uint16x8 us0, us1;
                    us0 = v_pack_u(i0, i1);
                    us1 = v_pack_u(i2, i3);

                    v_uint8x16 uc;
                    uc = v_pack(us0, us1);
                    v_store((uchar*)& out[l], uc);
                }
            }
            if (std::is_same<DST,ushort>::value)
            {
                for (; l <= length-8; l+=8)
                {
                    v_int32x4 i0, i1;
                    i0 = v_round( v_load( (float*)& in[l     ] ) );
                    i1 = v_round( v_load( (float*)& in[l +  4] ) );

                    v_uint16x8 us;
                    us = v_pack_u(i0, i1);
                    v_store((ushort*)& out[l], us);
                }
            }
        #endif

            // tail of SIMD cycle
            for (; l < length; l++)
            {
                out[l] = saturate<DST>(in[l], rintf);
            }
        }
        else if (std::is_integral<DST>::value) // here SRC is integral
        {
            for (int l=0; l < length; l++)
            {
                out[l] = saturate<DST>(in[l]);
            }
        }
        else // DST is floating-point, SRC is any
        {
            for (int l=0; l < length; l++)
            {
                out[l] = static_cast<DST>(in[l]);
            }
        }
    }
    else // if alpha or beta is non-trivial
    {
        // TODO: optimize if alpha and beta and data are integral
        for (int l=0; l < length; l++)
        {
            out[l] = saturate<DST>(in[l]*alpha + beta, rintf);
        }
    }
}

GAPI_FLUID_KERNEL(GFluidConvertTo, cv::gapi::core::GConvertTo, false)
{
    static const int Window = 1;

    static void run(const View &src, int /*rtype*/, double alpha, double beta, Buffer &dst)
    {
        //     DST     SRC     OP             __VA_ARGS__
        UNARY_(uchar , uchar , run_convertto, dst, src, alpha, beta);
        UNARY_(uchar , ushort, run_convertto, dst, src, alpha, beta);
        UNARY_(uchar ,  float, run_convertto, dst, src, alpha, beta);
        UNARY_(ushort, uchar , run_convertto, dst, src, alpha, beta);
        UNARY_(ushort, ushort, run_convertto, dst, src, alpha, beta);
        UNARY_(ushort,  float, run_convertto, dst, src, alpha, beta);
        UNARY_( float, uchar , run_convertto, dst, src, alpha, beta);
        UNARY_( float, ushort, run_convertto, dst, src, alpha, beta);
        UNARY_( float,  float, run_convertto, dst, src, alpha, beta);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//-----------------------------
//
// Fluid math kernels: min, max
//
//-----------------------------

enum Minmax { MM_MIN, MM_MAX };

template<typename DST, typename SRC1, typename SRC2>
static void run_minmax(Buffer &dst, const View &src1, const View &src2, Minmax minmax)
{
    static_assert(std::is_same<DST, SRC1>::value, "wrong types");
    static_assert(std::is_same<DST, SRC2>::value, "wrong types");

    const auto *in1 = src1.InLine<SRC1>(0);
    const auto *in2 = src2.InLine<SRC2>(0);
          auto *out = dst.OutLine<DST>();

    int width = dst.length();
    int chan  = dst.meta().chan;

    int length = width * chan;

    switch (minmax)
    {
    case MM_MIN:
        for (int l=0; l < length; l++)
            out[l] = in1[l] < in2[l]? in1[l]: in2[l];
        break;
    case MM_MAX:
        for (int l=0; l < length; l++)
            out[l] = in1[l] > in2[l]? in1[l]: in2[l];
        break;
    default: CV_Error(cv::Error::StsBadArg, "unsupported min/max operation");
    }
}

GAPI_FLUID_KERNEL(GFluidMin, cv::gapi::core::GMin, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {
        //      DST     SRC1    SRC2    OP          __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_minmax, dst, src1, src2, MM_MIN);
        BINARY_(ushort, ushort, ushort, run_minmax, dst, src1, src2, MM_MIN);
        BINARY_( short,  short,  short, run_minmax, dst, src1, src2, MM_MIN);
        BINARY_( float,  float,  float, run_minmax, dst, src1, src2, MM_MIN);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidMax, cv::gapi::core::GMax, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {
        //      DST     SRC1    SRC2    OP          __VA_ARGS__
        BINARY_(uchar , uchar , uchar , run_minmax, dst, src1, src2, MM_MAX);
        BINARY_(ushort, ushort, ushort, run_minmax, dst, src1, src2, MM_MAX);
        BINARY_( short,  short,  short, run_minmax, dst, src1, src2, MM_MAX);
        BINARY_( float,  float,  float, run_minmax, dst, src1, src2, MM_MAX);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//-----------------------
//
// Fluid kernels: compare
//
//-----------------------

enum Compare { CMP_EQ, CMP_NE, CMP_GE, CMP_GT, CMP_LE, CMP_LT };

template<typename DST, typename SRC1, typename SRC2>
static void run_cmp(Buffer &dst, const View &src1, const View &src2, Compare compare)
{
    static_assert(std::is_same<SRC1, SRC2>::value, "wrong types");
    static_assert(std::is_same<DST, uchar>::value, "wrong types");

    const auto *in1 = src1.InLine<SRC1>(0);
    const auto *in2 = src2.InLine<SRC2>(0);
          auto *out = dst.OutLine<DST>();

    int width = dst.length();
    int chan  = dst.meta().chan;

    int length = width * chan;

    switch (compare)
    {
    case CMP_EQ:
        for (int l=0; l < length; l++)
            out[l] = in1[l] == in2[l]? 255: 0;
        break;
    case CMP_NE:
        for (int l=0; l < length; l++)
            out[l] = in1[l] != in2[l]? 255: 0;
        break;
    case CMP_GE:
        for (int l=0; l < length; l++)
            out[l] = in1[l] >= in2[l]? 255: 0;
        break;
    case CMP_LE:
        for (int l=0; l < length; l++)
            out[l] = in1[l] <= in2[l]? 255: 0;
        break;
    case CMP_GT:
        for (int l=0; l < length; l++)
            out[l] = in1[l] > in2[l]? 255: 0;
        break;
    case CMP_LT:
        for (int l=0; l < length; l++)
            out[l] = in1[l] < in2[l]? 255: 0;
        break;
    default:
        CV_Error(cv::Error::StsBadArg, "unsupported compare operation");
    }
}

GAPI_FLUID_KERNEL(GFluidCmpEQ, cv::gapi::core::GCmpEQ, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {
        //      DST    SRC1    SRC2    OP       __VA_ARGS__
        BINARY_(uchar, uchar , uchar , run_cmp, dst, src1, src2, CMP_EQ);
        BINARY_(uchar,  short,  short, run_cmp, dst, src1, src2, CMP_EQ);
        BINARY_(uchar,  float,  float, run_cmp, dst, src1, src2, CMP_EQ);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidCmpNE, cv::gapi::core::GCmpNE, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {
        //      DST    SRC1    SRC2    OP       __VA_ARGS__
        BINARY_(uchar, uchar , uchar , run_cmp, dst, src1, src2, CMP_NE);
        BINARY_(uchar,  short,  short, run_cmp, dst, src1, src2, CMP_NE);
        BINARY_(uchar,  float,  float, run_cmp, dst, src1, src2, CMP_NE);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidCmpGE, cv::gapi::core::GCmpGE, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {
        //      DST    SRC1    SRC2    OP       __VA_ARGS__
        BINARY_(uchar, uchar , uchar , run_cmp, dst, src1, src2, CMP_GE);
        BINARY_(uchar,  short,  short, run_cmp, dst, src1, src2, CMP_GE);
        BINARY_(uchar,  float,  float, run_cmp, dst, src1, src2, CMP_GE);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidCmpGT, cv::gapi::core::GCmpGT, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {
        //      DST    SRC1    SRC2    OP       __VA_ARGS__
        BINARY_(uchar, uchar , uchar , run_cmp, dst, src1, src2, CMP_GT);
        BINARY_(uchar,  short,  short, run_cmp, dst, src1, src2, CMP_GT);
        BINARY_(uchar,  float,  float, run_cmp, dst, src1, src2, CMP_GT);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidCmpLE, cv::gapi::core::GCmpLE, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {
        //      DST    SRC1    SRC2    OP       __VA_ARGS__
        BINARY_(uchar, uchar , uchar , run_cmp, dst, src1, src2, CMP_LE);
        BINARY_(uchar,  short,  short, run_cmp, dst, src1, src2, CMP_LE);
        BINARY_(uchar,  float,  float, run_cmp, dst, src1, src2, CMP_LE);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidCmpLT, cv::gapi::core::GCmpLT, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, Buffer &dst)
    {
        //      DST    SRC1    SRC2    OP       __VA_ARGS__
        BINARY_(uchar, uchar , uchar , run_cmp, dst, src1, src2, CMP_LT);
        BINARY_(uchar,  short,  short, run_cmp, dst, src1, src2, CMP_LT);
        BINARY_(uchar,  float,  float, run_cmp, dst, src1, src2, CMP_LT);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//---------------------
//
// Compare with GScalar
//
//---------------------

template<typename DST, typename SRC, typename SCALAR=double>
static void run_cmp(DST out[], const SRC in[], int length, Compare compare, SCALAR s)
{
    switch (compare)
    {
    case CMP_EQ:
        for (int l=0; l < length; l++)
            out[l] = in[l] == s? 255: 0;
        break;
    case CMP_NE:
        for (int l=0; l < length; l++)
            out[l] = in[l] != s? 255: 0;
        break;
    case CMP_GE:
        for (int l=0; l < length; l++)
            out[l] = in[l] >= s? 255: 0;
        break;
    case CMP_LE:
        for (int l=0; l < length; l++)
            out[l] = in[l] <= s? 255: 0;
        break;
    case CMP_GT:
        for (int l=0; l < length; l++)
            out[l] = in[l] > s? 255: 0;
        break;
    case CMP_LT:
        for (int l=0; l < length; l++)
            out[l] = in[l] < s? 255: 0;
        break;
    default:
        CV_Error(cv::Error::StsBadArg, "unsupported compare operation");
    }
}

template<typename DST, typename SRC>
static void run_cmp(Buffer &dst, const View &src, Compare compare, const cv::Scalar &scalar)
{
    static_assert(std::is_same<DST, uchar>::value, "wrong types");

    const auto *in  = src.InLine<SRC>(0);
          auto *out = dst.OutLine<DST>();

    int width = dst.length();
    int chan  = dst.meta().chan;

    int length = width * chan;

    // compute faster if scalar rounds to SRC
    double d =                   scalar[0]  ;
    SRC    s = static_cast<SRC>( scalar[0] );

    if (s == d)
        run_cmp(out, in, length, compare, s);
    else
        run_cmp(out, in, length, compare, d);
}

GAPI_FLUID_KERNEL(GFluidCmpEQScalar, cv::gapi::core::GCmpEQScalar, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &scalar, Buffer &dst)
    {
        //     DST    SRC     OP       __VA_ARGS__
        UNARY_(uchar, uchar , run_cmp, dst, src, CMP_EQ, scalar);
        UNARY_(uchar,  short, run_cmp, dst, src, CMP_EQ, scalar);
        UNARY_(uchar,  float, run_cmp, dst, src, CMP_EQ, scalar);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidCmpNEScalar, cv::gapi::core::GCmpNEScalar, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &scalar, Buffer &dst)
    {
        //     DST    SRC     OP       __VA_ARGS__
        UNARY_(uchar, uchar , run_cmp, dst, src, CMP_NE, scalar);
        UNARY_(uchar,  short, run_cmp, dst, src, CMP_NE, scalar);
        UNARY_(uchar,  float, run_cmp, dst, src, CMP_NE, scalar);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidCmpGEScalar, cv::gapi::core::GCmpGEScalar, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &scalar, Buffer &dst)
    {
        //     DST    SRC     OP       __VA_ARGS__
        UNARY_(uchar, uchar , run_cmp, dst, src, CMP_GE, scalar);
        UNARY_(uchar,  short, run_cmp, dst, src, CMP_GE, scalar);
        UNARY_(uchar,  float, run_cmp, dst, src, CMP_GE, scalar);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidCmpGTScalar, cv::gapi::core::GCmpGTScalar, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &scalar, Buffer &dst)
    {
        //     DST    SRC     OP       __VA_ARGS__
        UNARY_(uchar, uchar , run_cmp, dst, src, CMP_GT, scalar);
        UNARY_(uchar,  short, run_cmp, dst, src, CMP_GT, scalar);
        UNARY_(uchar,  float, run_cmp, dst, src, CMP_GT, scalar);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidCmpLEScalar, cv::gapi::core::GCmpLEScalar, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &scalar, Buffer &dst)
    {
        //     DST    SRC     OP       __VA_ARGS__
        UNARY_(uchar, uchar , run_cmp, dst, src, CMP_LE, scalar);
        UNARY_(uchar,  short, run_cmp, dst, src, CMP_LE, scalar);
        UNARY_(uchar,  float, run_cmp, dst, src, CMP_LE, scalar);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

GAPI_FLUID_KERNEL(GFluidCmpLTScalar, cv::gapi::core::GCmpLTScalar, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &scalar, Buffer &dst)
    {
        //     DST    SRC     OP       __VA_ARGS__
        UNARY_(uchar, uchar , run_cmp, dst, src, CMP_LT, scalar);
        UNARY_(uchar,  short, run_cmp, dst, src, CMP_LT, scalar);
        UNARY_(uchar,  float, run_cmp, dst, src, CMP_LT, scalar);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//-------------------------
//
// Fluid kernels: threshold
//
//-------------------------

template<typename DST, typename SRC>
static void run_threshold(Buffer &dst, const View &src, const cv::Scalar &thresh,
                                                        const cv::Scalar &maxval,
                                                                     int  type)
{
    static_assert(std::is_same<DST, SRC>::value, "wrong types");

    const auto *in  = src.InLine<SRC>(0);
          auto *out = dst.OutLine<DST>();

    int width = dst.length();
    int chan  = dst.meta().chan;

    int length = width * chan;

    DST thresh_ = saturate<DST>(thresh[0], floord);
    DST threshd = saturate<DST>(thresh[0], roundd);
    DST maxvald = saturate<DST>(maxval[0], roundd);

    switch (type)
    {
    case cv::THRESH_BINARY:
        for (int l=0; l < length; l++)
            out[l] = in[l] > thresh_? maxvald: 0;
        break;
    case cv::THRESH_BINARY_INV:
        for (int l=0; l < length; l++)
            out[l] = in[l] > thresh_? 0: maxvald;
        break;
    case cv::THRESH_TRUNC:
        for (int l=0; l < length; l++)
            out[l] = in[l] > thresh_? threshd: in[l];
        break;
    case cv::THRESH_TOZERO:
        for (int l=0; l < length; l++)
            out[l] = in[l] > thresh_? in[l]: 0;
        break;
    case cv::THRESH_TOZERO_INV:
        for (int l=0; l < length; l++)
            out[l] = in[l] > thresh_? 0: in[l];
        break;
    default: CV_Error(cv::Error::StsBadArg, "unsupported threshold type");
    }
}

GAPI_FLUID_KERNEL(GFluidThreshold, cv::gapi::core::GThreshold, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &thresh,
                                     const cv::Scalar &maxval,
                                                  int  type,
                        Buffer &dst)
    {
        //     DST     SRC     OP             __VA_ARGS__
        UNARY_(uchar , uchar , run_threshold, dst, src, thresh, maxval, type);
        UNARY_(ushort, ushort, run_threshold, dst, src, thresh, maxval, type);
        UNARY_( short,  short, run_threshold, dst, src, thresh, maxval, type);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//------------------------
//
// Fluid kernels: in-range
//
//------------------------

static void run_inrange3(uchar out[], const uchar in[], int width,
                         const uchar lower[], const uchar upper[])
{
    int w = 0; // cycle index

#if CV_SIMD128
    for (; w <= width-16; w+=16)
    {
        v_uint8x16 i0, i1, i2;
        v_load_deinterleave(&in[3*w], i0, i1, i2);

        v_uint8x16 o;
        o = (i0 >= v_setall_u8(lower[0])) & (i0 <= v_setall_u8(upper[0])) &
            (i1 >= v_setall_u8(lower[1])) & (i1 <= v_setall_u8(upper[1])) &
            (i2 >= v_setall_u8(lower[2])) & (i2 <= v_setall_u8(upper[2]));

        v_store(&out[w], o);
    }
#endif

    for (; w < width; w++)
    {
        out[w] = in[3*w  ] >= lower[0] && in[3*w  ] <= upper[0] &&
                 in[3*w+1] >= lower[1] && in[3*w+1] <= upper[1] &&
                 in[3*w+2] >= lower[2] && in[3*w+2] <= upper[2] ? 255: 0;
    }
}

template<typename DST, typename SRC>
static void run_inrange(Buffer &dst, const View &src, const cv::Scalar &upperb,
                                                      const cv::Scalar &lowerb)
{
    static_assert(std::is_same<DST, uchar>::value, "wrong types");

    const auto *in  = src.InLine<SRC>(0);
          auto *out = dst.OutLine<DST>();

    int width = src.length();
    int chan  = src.meta().chan;
    GAPI_Assert(dst.meta().chan == 1);

    SRC lower[4], upper[4];
    for (int c=0; c < chan; c++)
    {
        if (std::is_integral<SRC>::value)
        {
            // for integral input, in[i] >= lower equals in[i] >= ceil(lower)
            // so we can optimize compare operations by rounding lower/upper
            lower[c] = saturate<SRC>(lowerb[c],  ceild);
            upper[c] = saturate<SRC>(upperb[c], floord);
        }
        else
        {
            // FIXME: now values used in comparison are floats (while they
            // have double precision initially). Comparison float/float
            // may differ from float/double (how it should work in this case)
            //
            // Example: threshold=1/3 (or 1/10)
            lower[c] = static_cast<SRC>(lowerb[c]);
            upper[c] = static_cast<SRC>(upperb[c]);
        }
    }

    // manually SIMD for important case if RGB/BGR
    if (std::is_same<SRC,uchar>::value && chan==3)
    {
        run_inrange3((uchar*)out, (const uchar*)in, width,
                     (const uchar*)lower, (const uchar*)upper);
        return;
    }

    // TODO: please manually SIMD if multiple channels:
    // modern compilers would perfectly vectorize this code if one channel,
    // but may need help with de-interleaving channels if RGB/BGR image etc
    switch (chan)
    {
    case 1:
        for (int w=0; w < width; w++)
            out[w] = in[w] >= lower[0] && in[w] <= upper[0]? 255: 0;
        break;
    case 2:
        for (int w=0; w < width; w++)
            out[w] = in[2*w  ] >= lower[0] && in[2*w  ] <= upper[0] &&
                     in[2*w+1] >= lower[1] && in[2*w+1] <= upper[1] ? 255: 0;
        break;
    case 3:
        for (int w=0; w < width; w++)
            out[w] = in[3*w  ] >= lower[0] && in[3*w  ] <= upper[0] &&
                     in[3*w+1] >= lower[1] && in[3*w+1] <= upper[1] &&
                     in[3*w+2] >= lower[2] && in[3*w+2] <= upper[2] ? 255: 0;
        break;
    case 4:
        for (int w=0; w < width; w++)
            out[w] = in[4*w  ] >= lower[0] && in[4*w  ] <= upper[0] &&
                     in[4*w+1] >= lower[1] && in[4*w+1] <= upper[1] &&
                     in[4*w+2] >= lower[2] && in[4*w+2] <= upper[2] &&
                     in[4*w+3] >= lower[3] && in[4*w+3] <= upper[3] ? 255: 0;
        break;
    default: CV_Error(cv::Error::StsBadArg, "unsupported number of channels");
    }
}

GAPI_FLUID_KERNEL(GFluidInRange, cv::gapi::core::GInRange, false)
{
    static const int Window = 1;

    static void run(const View &src, const cv::Scalar &lowerb, const cv::Scalar& upperb,
                        Buffer &dst)
    {
        //       DST     SRC    OP           __VA_ARGS__
        INRANGE_(uchar, uchar , run_inrange, dst, src, upperb, lowerb);
        INRANGE_(uchar, ushort, run_inrange, dst, src, upperb, lowerb);
        INRANGE_(uchar,  short, run_inrange, dst, src, upperb, lowerb);
        INRANGE_(uchar,  float, run_inrange, dst, src, upperb, lowerb);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//----------------------
//
// Fluid kernels: select
//
//----------------------

// manually vectored function for important case if RGB/BGR image
static void run_select_row3(int width, uchar out[], uchar in1[], uchar in2[], uchar in3[])
{
    int w = 0; // cycle index

#if CV_SIMD128
    for (; w <= width-16; w+=16)
    {
        v_uint8x16 a1, b1, c1;
        v_uint8x16 a2, b2, c2;
        v_uint8x16 mask;
        v_uint8x16 a, b, c;

        v_load_deinterleave(&in1[3*w], a1, b1, c1);
        v_load_deinterleave(&in2[3*w], a2, b2, c2);

        mask = v_load(&in3[w]);
        mask = mask != v_setzero_u8();

        a = v_select(mask, a1, a2);
        b = v_select(mask, b1, b2);
        c = v_select(mask, c1, c2);

        v_store_interleave(&out[3*w], a, b, c);
    }
#endif

    for (; w < width; w++)
    {
        out[3*w    ] = in3[w]? in1[3*w    ]: in2[3*w    ];
        out[3*w + 1] = in3[w]? in1[3*w + 1]: in2[3*w + 1];
        out[3*w + 2] = in3[w]? in1[3*w + 2]: in2[3*w + 2];
    }
}

// parameter chan is compile-time known constant, normally chan=1..4
template<int chan, typename DST, typename SRC1, typename SRC2, typename SRC3>
static void run_select_row(int width, DST out[], SRC1 in1[], SRC2 in2[], SRC3 in3[])
{
    if (std::is_same<DST,uchar>::value && chan==3)
    {
        // manually vectored function for important case if RGB/BGR image
        run_select_row3(width, (uchar*)out, (uchar*)in1, (uchar*)in2, (uchar*)in3);
        return;
    }

    // because `chan` is template parameter, its value is known at compilation time,
    // so that modern compilers would efficiently vectorize this cycle if chan==1
    // (if chan>1, compilers may need help with de-interleaving of the channels)
    for (int w=0; w < width; w++)
    {
        for (int c=0; c < chan; c++)
        {
            out[w*chan + c] = in3[w]? in1[w*chan + c]: in2[w*chan + c];
        }
    }
}

template<typename DST, typename SRC1, typename SRC2, typename SRC3>
static void run_select(Buffer &dst, const View &src1, const View &src2, const View &src3)
{
    static_assert(std::is_same<DST ,  SRC1>::value, "wrong types");
    static_assert(std::is_same<DST ,  SRC2>::value, "wrong types");
    static_assert(std::is_same<uchar, SRC3>::value, "wrong types");

    auto *out = dst.OutLine<DST>();

    const auto *in1 = src1.InLine<SRC1>(0);
    const auto *in2 = src2.InLine<SRC2>(0);
    const auto *in3 = src3.InLine<SRC3>(0);

    int width = dst.length();
    int chan  = dst.meta().chan;

    switch (chan)
    {
    case 1: run_select_row<1>(width, out, in1, in2, in3); break;
    case 2: run_select_row<2>(width, out, in1, in2, in3); break;
    case 3: run_select_row<3>(width, out, in1, in2, in3); break;
    case 4: run_select_row<4>(width, out, in1, in2, in3); break;
    default: CV_Error(cv::Error::StsBadArg, "unsupported number of channels");
    }
}

GAPI_FLUID_KERNEL(GFluidSelect, cv::gapi::core::GSelect, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, const View &src3, Buffer &dst)
    {
        //      DST     SRC1    SRC2    SRC3   OP          __VA_ARGS__
        SELECT_(uchar , uchar , uchar , uchar, run_select, dst, src1, src2, src3);
        SELECT_(ushort, ushort, ushort, uchar, run_select, dst, src1, src2, src3);
        SELECT_( short,  short,  short, uchar, run_select, dst, src1, src2, src3);

        CV_Error(cv::Error::StsBadArg, "unsupported combination of types");
    }
};

//----------------------------------------------------
//
// Fluid kernels: split, merge, polat2cart, cart2polar
//
//----------------------------------------------------

GAPI_FLUID_KERNEL(GFluidSplit3, cv::gapi::core::GSplit3, false)
{
    static const int Window = 1;

    static void run(const View &src, Buffer &dst1, Buffer &dst2, Buffer &dst3)
    {
        const auto *in   =  src.InLine<uchar>(0);
              auto *out1 = dst1.OutLine<uchar>();
              auto *out2 = dst2.OutLine<uchar>();
              auto *out3 = dst3.OutLine<uchar>();

        GAPI_Assert(3 == src.meta().chan);
        int width = src.length();

        int w = 0; // cycle counter

    #if CV_SIMD128
        for (; w <= width-16; w+=16)
        {
            v_uint8x16 a, b, c;
            v_load_deinterleave(&in[3*w], a, b, c);
            v_store(&out1[w], a);
            v_store(&out2[w], b);
            v_store(&out3[w], c);
        }
    #endif

        for (; w < width; w++)
        {
            out1[w] = in[3*w    ];
            out2[w] = in[3*w + 1];
            out3[w] = in[3*w + 2];
        }
    }
};

GAPI_FLUID_KERNEL(GFluidSplit4, cv::gapi::core::GSplit4, false)
{
    static const int Window = 1;

    static void run(const View &src, Buffer &dst1, Buffer &dst2, Buffer &dst3, Buffer &dst4)
    {
        const auto *in   =  src.InLine<uchar>(0);
              auto *out1 = dst1.OutLine<uchar>();
              auto *out2 = dst2.OutLine<uchar>();
              auto *out3 = dst3.OutLine<uchar>();
              auto *out4 = dst4.OutLine<uchar>();

        GAPI_Assert(4 == src.meta().chan);
        int width = src.length();

        int w = 0; // cycle counter

    #if CV_SIMD128
        for (; w <= width-16; w+=16)
        {
            v_uint8x16 a, b, c, d;
            v_load_deinterleave(&in[4*w], a, b, c, d);
            v_store(&out1[w], a);
            v_store(&out2[w], b);
            v_store(&out3[w], c);
            v_store(&out4[w], d);
        }
    #endif

        for (; w < width; w++)
        {
            out1[w] = in[4*w    ];
            out2[w] = in[4*w + 1];
            out3[w] = in[4*w + 2];
            out4[w] = in[4*w + 3];
        }
    }
};

GAPI_FLUID_KERNEL(GFluidMerge3, cv::gapi::core::GMerge3, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, const View &src3, Buffer &dst)
    {
        const auto *in1 = src1.InLine<uchar>(0);
        const auto *in2 = src2.InLine<uchar>(0);
        const auto *in3 = src3.InLine<uchar>(0);
              auto *out = dst.OutLine<uchar>();

        GAPI_Assert(3 == dst.meta().chan);
        int width = dst.length();

        int w = 0; // cycle counter

    #if CV_SIMD128
        for (; w <= width-16; w+=16)
        {
            v_uint8x16 a, b, c;
            a = v_load(&in1[w]);
            b = v_load(&in2[w]);
            c = v_load(&in3[w]);
            v_store_interleave(&out[3*w], a, b, c);
        }
    #endif

        for (; w < width; w++)
        {
            out[3*w    ] = in1[w];
            out[3*w + 1] = in2[w];
            out[3*w + 2] = in3[w];
        }
    }
};

GAPI_FLUID_KERNEL(GFluidMerge4, cv::gapi::core::GMerge4, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, const View &src3, const View &src4,
                    Buffer &dst)
    {
        const auto *in1 = src1.InLine<uchar>(0);
        const auto *in2 = src2.InLine<uchar>(0);
        const auto *in3 = src3.InLine<uchar>(0);
        const auto *in4 = src4.InLine<uchar>(0);
              auto *out = dst.OutLine<uchar>();

        GAPI_Assert(4 == dst.meta().chan);
        int width = dst.length();

        int w = 0; // cycle counter

    #if CV_SIMD128
        for (; w <= width-16; w+=16)
        {
            v_uint8x16 a, b, c, d;
            a = v_load(&in1[w]);
            b = v_load(&in2[w]);
            c = v_load(&in3[w]);
            d = v_load(&in4[w]);
            v_store_interleave(&out[4*w], a, b, c, d);
        }
    #endif

        for (; w < width; w++)
        {
            out[4*w    ] = in1[w];
            out[4*w + 1] = in2[w];
            out[4*w + 2] = in3[w];
            out[4*w + 3] = in4[w];
        }
    }
};

GAPI_FLUID_KERNEL(GFluidPolarToCart, cv::gapi::core::GPolarToCart, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, bool angleInDegrees,
                    Buffer &dst1, Buffer &dst2)
    {
        GAPI_Assert(src1.meta().depth == CV_32F);
        GAPI_Assert(src2.meta().depth == CV_32F);
        GAPI_Assert(dst1.meta().depth == CV_32F);
        GAPI_Assert(dst2.meta().depth == CV_32F);

        const auto * in1 = src1.InLine<float>(0);
        const auto * in2 = src2.InLine<float>(0);
              auto *out1 = dst1.OutLine<float>();
              auto *out2 = dst2.OutLine<float>();

        int width = src1.length();
        int chan  = src2.meta().chan;
        int length = width * chan;

        // SIMD: compiler vectoring!
        for (int l=0; l < length; l++)
        {
            float angle = angleInDegrees?
                          in2[l] * static_cast<float>(CV_PI / 180):
                          in2[l];
            float magnitude = in1[l];
            float x = magnitude * std::cos(angle);
            float y = magnitude * std::sin(angle);
            out1[l] = x;
            out2[l] = y;
        }
    }
};

GAPI_FLUID_KERNEL(GFluidCartToPolar, cv::gapi::core::GCartToPolar, false)
{
    static const int Window = 1;

    static void run(const View &src1, const View &src2, bool angleInDegrees,
                    Buffer &dst1, Buffer &dst2)
    {
        GAPI_Assert(src1.meta().depth == CV_32F);
        GAPI_Assert(src2.meta().depth == CV_32F);
        GAPI_Assert(dst1.meta().depth == CV_32F);
        GAPI_Assert(dst2.meta().depth == CV_32F);

        const auto * in1 = src1.InLine<float>(0);
        const auto * in2 = src2.InLine<float>(0);
              auto *out1 = dst1.OutLine<float>();
              auto *out2 = dst2.OutLine<float>();

        int width = src1.length();
        int chan  = src2.meta().chan;
        int length = width * chan;

        // SIMD: compiler vectoring!
        for (int l=0; l < length; l++)
        {
            float x = in1[l];
            float y = in2[l];
            float magnitude = std::hypot(y, x);
            float angle_rad = std::atan2(y, x);
            float angle = angleInDegrees?
                          angle_rad * static_cast<float>(180 / CV_PI):
                          angle_rad;
            out1[l] = magnitude;
            out2[l] = angle;
        }
    }
};

GAPI_FLUID_KERNEL(GFluidPhase, cv::gapi::core::GPhase, false)
{
    static const int Window = 1;

    static void run(const View &src_x,
                    const View &src_y,
                    bool angleInDegrees,
                    Buffer &dst)
    {
        const auto w = dst.length() * dst.meta().chan;
        if (src_x.meta().depth == CV_32F && src_y.meta().depth == CV_32F)
        {
            hal::fastAtan32f(src_y.InLine<float>(0),
                             src_x.InLine<float>(0),
                             dst.OutLine<float>(),
                             w,
                             angleInDegrees);
        }
        else if (src_x.meta().depth == CV_64F && src_y.meta().depth == CV_64F)
        {
            hal::fastAtan64f(src_y.InLine<double>(0),
                             src_x.InLine<double>(0),
                             dst.OutLine<double>(),
                             w,
                             angleInDegrees);
        } else GAPI_Assert(false && !"Phase supports 32F/64F input only!");
    }
};

GAPI_FLUID_KERNEL(GFluidResize, cv::gapi::core::GResize, true)
{
    static const int Window = 1;
    static const auto Kind = GFluidKernel::Kind::Resize;

    constexpr static const int INTER_RESIZE_COEF_BITS = 11;
    constexpr static const int INTER_RESIZE_COEF_SCALE = 1 << INTER_RESIZE_COEF_BITS;
    constexpr static const short ONE = INTER_RESIZE_COEF_SCALE;

    struct ResizeUnit
    {
        short alpha0;
        short alpha1;
        int   s0;
        int   s1;
    };

    static ResizeUnit map(double ratio, int start, int max, int outCoord)
    {
        float f = static_cast<float>((outCoord + 0.5f) * ratio - 0.5f);
        int s = cvFloor(f);
        f -= s;

        ResizeUnit ru;

        ru.s0 = std::max(s - start, 0);
        ru.s1 = ((f == 0.0) || s + 1 >= max) ? s - start : s - start + 1;

        ru.alpha0 = saturate_cast<short>((1.0f - f) * INTER_RESIZE_COEF_SCALE);
        ru.alpha1 = saturate_cast<short>((f) * INTER_RESIZE_COEF_SCALE);

        return ru;
    }

    static void initScratch(const cv::GMatDesc& in,
                            cv::Size outSz, double /*fx*/, double /*fy*/, int /*interp*/,
                            cv::gapi::fluid::Buffer &scratch)
    {
        CV_Assert(in.depth == CV_8U && in.chan == 3);

        cv::Size scratch_size{static_cast<int>(outSz.width * sizeof(ResizeUnit)), 1};

        cv::GMatDesc desc;
        desc.chan  = 1;
        desc.depth = CV_8UC1;
        desc.size  = to_own(scratch_size);

        cv::gapi::fluid::Buffer buffer(desc);
        scratch = std::move(buffer);

        ResizeUnit* mapX = scratch.OutLine<ResizeUnit>();
        double hRatio = (double)in.size.width / outSz.width;

        for (int x = 0, w = outSz.width; x < w; x++)
        {
            mapX[x] = map(hRatio, 0, in.size.width, x);
        }
    }

    static void resetScratch(cv::gapi::fluid::Buffer& /*scratch*/)
    {}

    static void run(const cv::gapi::fluid::View& in, cv::Size /*sz*/, double /*fx*/, double /*fy*/, int /*interp*/,
                    cv::gapi::fluid::Buffer& out, cv::gapi::fluid::Buffer &scratch)
    {
        double vRatio = (double)in.meta().size.height / out.meta().size.height;
        auto mapY = map(vRatio, in.y(), in.meta().size.height, out.y());

        auto beta0 = mapY.alpha0;
        auto beta1 = mapY.alpha1;

        const auto src0 = in.InLine <unsigned char>(mapY.s0);
        const auto src1 = in.InLine <unsigned char>(mapY.s1);

        auto dst = out.OutLine<unsigned char>();

        ResizeUnit* mapX = scratch.OutLine<ResizeUnit>();

        for (int x = 0; x < out.length(); x++)
        {
            short alpha0 = mapX[x].alpha0;
            short alpha1 = mapX[x].alpha1;
            int sx0 = mapX[x].s0;
            int sx1 = mapX[x].s1;

            int res00 = src0[3*sx0    ]*alpha0 + src0[3*(sx1)    ]*alpha1;
            int res10 = src1[3*sx0    ]*alpha0 + src1[3*(sx1)    ]*alpha1;

            int res01 = src0[3*sx0 + 1]*alpha0 + src0[3*(sx1) + 1]*alpha1;
            int res11 = src1[3*sx0 + 1]*alpha0 + src1[3*(sx1) + 1]*alpha1;

            int res02 = src0[3*sx0 + 2]*alpha0 + src0[3*(sx1) + 2]*alpha1;
            int res12 = src1[3*sx0 + 2]*alpha0 + src1[3*(sx1) + 2]*alpha1;

            dst[3*x    ] = uchar(( ((beta0 * (res00 >> 4)) >> 16) + ((beta1 * (res10 >> 4)) >> 16) + 2)>>2);
            dst[3*x + 1] = uchar(( ((beta0 * (res01 >> 4)) >> 16) + ((beta1 * (res11 >> 4)) >> 16) + 2)>>2);
            dst[3*x + 2] = uchar(( ((beta0 * (res02 >> 4)) >> 16) + ((beta1 * (res12 >> 4)) >> 16) + 2)>>2);
        }
    }
};

GAPI_FLUID_KERNEL(GFluidSqrt, cv::gapi::core::GSqrt, false)
{
    static const int Window = 1;

    static void run(const View &in, Buffer &out)
    {
        const auto w = out.length() * out.meta().chan;
        if (in.meta().depth == CV_32F)
        {
            hal::sqrt32f(in.InLine<float>(0),
                         out.OutLine<float>(0),
                         w);
        }
        else if (in.meta().depth == CV_64F)
        {
            hal::sqrt64f(in.InLine<double>(0),
                         out.OutLine<double>(0),
                         w);
        } else GAPI_Assert(false && !"Sqrt supports 32F/64F input only!");
    }
};

} // namespace fliud
} // namespace gapi
} // namespace cv

cv::gapi::GKernelPackage cv::gapi::core::fluid::kernels()
{
    using namespace cv::gapi::fluid;

    return cv::gapi::kernels
     <       GFluidAdd
            ,GFluidSub
            ,GFluidMul
            ,GFluidDiv
            ,GFluidAbsDiff
            ,GFluidAnd
            ,GFluidOr
            ,GFluidXor
            ,GFluidMin
            ,GFluidMax
            ,GFluidCmpGT
            ,GFluidCmpGE
            ,GFluidCmpLE
            ,GFluidCmpLT
            ,GFluidCmpEQ
            ,GFluidCmpNE
            ,GFluidAddW
            ,GFluidNot
            ,GFluidLUT
            ,GFluidConvertTo
            ,GFluidSplit3
            ,GFluidSplit4
            ,GFluidMerge3
            ,GFluidMerge4
            ,GFluidSelect
            ,GFluidPolarToCart
            ,GFluidCartToPolar
            ,GFluidPhase
            ,GFluidAddC
            ,GFluidSubC
            ,GFluidSubRC
            ,GFluidMulC
            ,GFluidMulCOld
            ,GFluidDivC
            ,GFluidDivRC
            ,GFluidAbsDiffC
            ,GFluidCmpGTScalar
            ,GFluidCmpGEScalar
            ,GFluidCmpLEScalar
            ,GFluidCmpLTScalar
            ,GFluidCmpEQScalar
            ,GFluidCmpNEScalar
            ,GFluidThreshold
            ,GFluidInRange
            ,GFluidResize
            ,GFluidSqrt
        #if 0
            ,GFluidMean        -- not fluid
            ,GFluidSum         -- not fluid
            ,GFluidNormL1      -- not fluid
            ,GFluidNormL2      -- not fluid
            ,GFluidNormInf     -- not fluid
            ,GFluidIntegral    -- not fluid
            ,GFluidThresholdOT -- not fluid
            ,GFluidResize      -- not fluid (?)
            ,GFluidRemap       -- not fluid
            ,GFluidFlip        -- not fluid
            ,GFluidCrop        -- not fluid
            ,GFluidConcatHor
            ,GFluidConcatVert  -- not fluid
        #endif
        >();
}

#endif // !defined(GAPI_STANDALONE)
