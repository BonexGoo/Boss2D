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
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Copyright (C) 2017, Intel Corporation, all rights reserved.
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

#include "../precomp.hpp"
#include "layers_common.hpp"
#include "../op_halide.hpp"
#include "../op_inf_engine.hpp"
#include <opencv2/dnn/shape_utils.hpp>

#ifdef HAVE_OPENCL
#include "opencl_kernels_dnn.hpp"
using namespace cv::dnn::ocl4dnn;
#endif

namespace cv
{
namespace dnn
{

class FullyConnectedLayerImpl CV_FINAL : public InnerProductLayer
{
public:
    enum { VEC_ALIGN = 8 };

#ifdef HAVE_OPENCL
    Ptr<OCL4DNNInnerProduct<float> > innerProductOp;
    std::vector<UMat> umat_blobs;
    std::vector<UMat> half_blobs;
#endif

    FullyConnectedLayerImpl(const LayerParams& params)
    {
        setParamsFrom(params);
        CV_Assert(1 <= blobs.size() && blobs.size() <= 2);

        int numOutput = params.get<int>("num_output");
        int innerSize = (int)blobs[0].total() / numOutput;
        bias = params.get<bool>("bias_term", true);
        axis = params.get<int>("axis", 1);

        CV_Assert(blobs[0].dims >= 2 && (size_t)(innerSize * numOutput) == blobs[0].total());
        CV_Assert(!bias || (blobs.size() == 2 && (size_t)numOutput == blobs[1].total()));

        weightsMat = blobs[0] = blobs[0].reshape(1, numOutput);
        int vecsize = weightsMat.cols;
        if( vecsize % VEC_ALIGN != 0 )
        {
            int vecsize_aligned = (int)alignSize(vecsize, VEC_ALIGN);
            Mat weightsBuf(weightsMat.rows, vecsize_aligned, weightsMat.type());
            Mat wpadding = weightsBuf.colRange(vecsize, vecsize_aligned);
            wpadding.setTo(Scalar::all(0.));
            weightsMat = weightsBuf.colRange(0, vecsize);
            blobs[0].copyTo(weightsMat);
        }

        if (bias)
            biasMat = blobs[1] = blobs[1].reshape(1, 1);
        else
            biasMat = Mat::zeros(1, numOutput, weightsMat.type());
    }

    bool getMemoryShapes(const std::vector<MatShape> &inputs,
                         const int requiredOutputs,
                         std::vector<MatShape> &outputs,
                         std::vector<MatShape> &) const CV_OVERRIDE
    {
        CV_Assert(inputs.size() == 1);
        CV_Assert(1 <= blobs.size() && blobs.size() <= 2);
        CV_Assert(blobs[0].dims == 2);

        int cAxis = clamp(axis, inputs[0]);
        int numOutput = blobs[0].size[0];
        MatShape outShape(cAxis + 1);
        for (int i = 0; i < cAxis; ++i)
            outShape[i] = inputs[0][i];
        outShape.back() = numOutput;

        outputs.resize(inputs.size(), outShape);

        CV_Assert(!bias || (size_t)numOutput == blobs[1].total());
        return false;
    }

    virtual bool supportBackend(int backendId) CV_OVERRIDE
    {
        return backendId == DNN_BACKEND_OPENCV ||
               (backendId == DNN_BACKEND_HALIDE && haveHalide() && axis == 1) ||
               (backendId == DNN_BACKEND_INFERENCE_ENGINE && haveInfEngine() && axis == 1);
    }

    virtual bool setActivation(const Ptr<ActivationLayer>& layer) CV_OVERRIDE
    {
        if (activ.empty() || layer.empty())
        {
            activ = layer;
            return !activ.empty();
        }
        else
            return false;
    }

    class FullyConnected : public ParallelLoopBody
    {
    public:
        FullyConnected() : srcMat(0), weights(0), biasMat(0), activ(0), dstMat(0), nstripes(0), useAVX(false), useAVX2(false), useAVX512(false) {}

        static void run(const Mat& srcMat, const Mat& weights, const Mat& biasMat,
                        Mat& dstMat, const ActivationLayer* activ, int nstripes)
        {
            CV_Assert( srcMat.dims == 2 && srcMat.cols == weights.cols &&
                       dstMat.rows == srcMat.rows && dstMat.cols == weights.rows &&
                       srcMat.type() == weights.type() && weights.type() == dstMat.type() &&
                       srcMat.type() == CV_32F &&
                       (biasMat.empty() || (biasMat.type() == srcMat.type() &&
                                           biasMat.isContinuous() && (int)biasMat.total() == dstMat.cols)) );

            FullyConnected p;

            p.srcMat = &srcMat;
            p.weights = &weights;
            p.biasMat = &biasMat;
            p.dstMat = &dstMat;
            p.nstripes = nstripes;
            p.activ = activ;
            p.useAVX = checkHardwareSupport(CPU_AVX);
            p.useAVX2 = checkHardwareSupport(CPU_AVX2);
            p.useAVX512 = CV_CPU_HAS_SUPPORT_AVX512_SKX;

            parallel_for_(Range(0, nstripes), p, nstripes);
        }

        void operator()(const Range& r) const CV_OVERRIDE
        {
            int valign = FullyConnectedLayerImpl::VEC_ALIGN;
            int nsamples = srcMat->rows;
            int nw0 = weights->rows;
            int k, vecsize = srcMat->cols;
            int vecsize_aligned = (int)alignSize(vecsize, VEC_ALIGN);
            size_t total = (size_t)nsamples*nw0;
            size_t stripeSize = (total + nstripes - 1)/nstripes;
            size_t stripeStart = r.start*stripeSize;
            size_t stripeEnd = r.end == nstripes ? total : std::min(r.end*stripeSize, total);
            size_t wstep = weights->step1();
            AutoBuffer<float> srcbuf(vecsize_aligned + valign);
            float* sptr = alignPtr(srcbuf.data(), (int)(valign*sizeof(float)));

            for( k = vecsize; k < vecsize_aligned; k++ )
                sptr[k] = 0.f;

            for( size_t ofs = stripeStart; ofs < stripeEnd; )
            {
                int sampleIdx = (int)(ofs / nw0);
                int delta = (int)(ofs - (size_t)sampleIdx*nw0);
                const float* sptr_ = srcMat->ptr<float>(sampleIdx);
                const float* wptr = weights->ptr<float>(delta);
                float* dptr = dstMat->ptr<float>(sampleIdx) + delta;
                const float* biasptr = biasMat->ptr<float>() + delta;
                int nw = std::min(nw0 - delta, (int)(stripeEnd - ofs));

                memcpy(sptr, sptr_, vecsize*sizeof(sptr[0]));

            #if CV_TRY_AVX512_SKX
                if( useAVX512 )
                    opt_AVX512_SKX::fastGEMM1T( sptr, wptr, wstep, biasptr, dptr, nw, vecsize);
                else
            #endif
            #if CV_TRY_AVX2
                if( useAVX2 )
                    opt_AVX2::fastGEMM1T( sptr, wptr, wstep, biasptr, dptr, nw, vecsize);
                else
            #endif
            #if CV_TRY_AVX
                if( useAVX )
                    opt_AVX::fastGEMM1T( sptr, wptr, wstep, biasptr, dptr, nw, vecsize);
                else
            #endif
                {
                    int i = 0;

            #if CV_SIMD128
                    for( ; i <= nw - 4; i += 4, wptr += 4*wstep )
                    {
                        v_float32x4 vs0 = v_setall_f32(0.f), vs1 = v_setall_f32(0.f);
                        v_float32x4 vs2 = v_setall_f32(0.f), vs3 = v_setall_f32(0.f);

                        for( k = 0; k < vecsize; k += 4 )
                        {
                            v_float32x4 v = v_load_aligned(sptr + k);
                            vs0 += v*v_load_aligned(wptr + k);
                            vs1 += v*v_load_aligned(wptr + wstep + k);
                            vs2 += v*v_load_aligned(wptr + wstep*2 + k);
                            vs3 += v*v_load_aligned(wptr + wstep*3 + k);
                        }

                        v_float32x4 s = v_reduce_sum4(vs0, vs1, vs2, vs3);
                        s += v_load(biasptr + i);
                        v_store(dptr + i, s);
                    }
            #endif

                    for( ; i < nw; i++, wptr += wstep )
                    {
                        float s0=biasptr[i];

                        for( k = 0; k < vecsize; k++ )
                        {
                            float v = sptr[k];
                            s0 += v*wptr[k];
                        }
                        dptr[i] = s0;
                    }
                }

                if(activ)
                    activ->forwardSlice(dptr, dptr, 1, 1, delta, delta + nw);

                ofs += nw;
            }
        }

        const Mat *srcMat, *weights, *biasMat;
        const ActivationLayer* activ;
        Mat* dstMat;
        int nstripes;
        bool useAVX;
        bool useAVX2;
        bool useAVX512;
    };

#ifdef HAVE_OPENCL
    virtual void finalize(InputArrayOfArrays, OutputArrayOfArrays) CV_OVERRIDE
    {
        innerProductOp.release();
        umat_blobs.clear();
        half_blobs.clear();
    }

    bool forward_ocl(InputArrayOfArrays inps, OutputArrayOfArrays outs, InputArrayOfArrays internals)
    {
        std::vector<UMat> inputs;
        std::vector<UMat> outputs;

        bool use_half = (inps.depth() == CV_16S);
        inps.getUMatVector(inputs);
        outs.getUMatVector(outputs);

        int axisCan = clamp(axis, inputs[0].dims);
        int numOutput = blobs[0].size[0];
        int innerSize = blobs[0].size[1];
        int outerSize = total(shape(inputs[0]), 0, axisCan);
        bool ret = true;

        if (innerProductOp.empty())
        {
            size_t n = blobs.size();
            umat_blobs.resize(n);
            for (int i = 0; i < n; i++) blobs[i].copyTo(umat_blobs[i]);

            OCL4DNNInnerProductConfig config;
            config.num_output = numOutput;
            config.bias_term = bias;
            config.M = outerSize;
            config.K = innerSize;
            config.use_half = use_half;

            if (use_half)
            {
                half_blobs.resize(umat_blobs.size());
                for (int i = 0; i < umat_blobs.size(); i++)
                {
                    if (!umat_blobs[i].empty())
                        convertFp16(umat_blobs[i], half_blobs[i]);
                }
            }

            innerProductOp = Ptr<OCL4DNNInnerProduct<float> >(new OCL4DNNInnerProduct<float>(config));
        }

        for (size_t i = 0; i < inputs.size(); i++)
        {
            MatShape inshape, outshape;
            inshape = shape(outerSize, innerSize);
            outshape = shape(outerSize, numOutput);

            UMat srcMat, dstMat;
            srcMat = inputs[i].reshape(1, inshape.size(), &inshape[0]);
            dstMat = outputs[i].reshape(1, outshape.size(), &outshape[0]);

            if (!innerProductOp->Forward(srcMat, (use_half) ? half_blobs[0] : umat_blobs[0],
                                         (bias) ? (use_half ? half_blobs[1] : umat_blobs[1]) : UMat(),
                                         dstMat))
            {
                ret = false;
                break;
            }

            if (!use_half && bias && (outerSize > 1))
            {
                UMat biasOnesMat = UMat::ones(outerSize, 1, umat_blobs[0].type());
                UMat& biases = umat_blobs[1];
                cv::gemm(biasOnesMat, biases, 1, dstMat, 1, dstMat, 0);
            }
        }

        if (ret) return true;

        UMat& weights = umat_blobs[0];
        for (size_t i = 0; i < inputs.size(); i++)
        {
            MatShape inshape, outshape;
            inshape = shape(outerSize, innerSize);
            outshape = shape(outerSize, numOutput);

            UMat srcMat, dstMat, srcMat_fp32, dstMat_fp32;
            srcMat = inputs[i].reshape(1, inshape.size(), &inshape[0]);
            dstMat = outputs[i].reshape(1, outshape.size(), &outshape[0]);

            if (use_half)
            {
                convertFp16(srcMat, srcMat_fp32);
                convertFp16(dstMat, dstMat_fp32);
            }
            else
            {
                srcMat_fp32 = srcMat;
                dstMat_fp32 = dstMat;
            }

            cv::gemm(srcMat_fp32, weights, 1, noArray(), 0, dstMat_fp32, GEMM_2_T);

            if (bias)
            {
                UMat biasOnesMat = UMat::ones(outerSize, 1, umat_blobs[0].type());
                UMat& biases = umat_blobs[1];
                cv::gemm(biasOnesMat, biases, 1, dstMat_fp32, 1, dstMat_fp32, 0);
            }
            if (use_half)
            {
                convertFp16(srcMat_fp32, srcMat);
                convertFp16(dstMat_fp32, dstMat);
            }
        }

        return true;
    }
#endif

    void forward(InputArrayOfArrays inputs_arr, OutputArrayOfArrays outputs_arr, OutputArrayOfArrays internals_arr) CV_OVERRIDE
    {
        CV_TRACE_FUNCTION();
        CV_TRACE_ARG_VALUE(name, "name", name.c_str());

        CV_OCL_RUN(IS_DNN_OPENCL_TARGET(preferableTarget),
                   forward_ocl(inputs_arr, outputs_arr, internals_arr))

        if (inputs_arr.depth() == CV_16S)
        {
            forward_fallback(inputs_arr, outputs_arr, internals_arr);
            return;
        }

        std::vector<Mat> input, output;
        inputs_arr.getMatVector(input);
        outputs_arr.getMatVector(output);

        int axisCan = clamp(axis, input[0].dims);
        int outerSize = input[0].total(0, axisCan);

        for (size_t i = 0; i < input.size(); i++)
        {
            Mat srcMat = input[i].reshape(1, outerSize);
            Mat dstMat = output[i].reshape(1, outerSize);

            const int nstripes = getNumThreads();
            FullyConnected::run(srcMat, weightsMat, biasMat, dstMat, activ.get(), nstripes);
        }
    }

    virtual Ptr<BackendNode> initHalide(const std::vector<Ptr<BackendWrapper> > &inputs) CV_OVERRIDE
    {
#ifdef HAVE_HALIDE
        int inW, inH, inC, inN, outC = blobs[0].size[0];
        Halide::Buffer<float> inputBuffer = halideBuffer(inputs[0]);
        getCanonicalSize(inputBuffer, &inW, &inH, &inC, &inN);
        auto weights = wrapToHalideBuffer(blobs[0], {inW, inH, inC, outC});

        Halide::Var x("x"), y("y"), c("c"), n("n");
        Halide::Func top = (name.empty() ? Halide::Func() : Halide::Func(name));
        Halide::RDom r(0, inW, 0, inH, 0, inC);
        Halide::Expr topExpr = sum(inputBuffer(r.x, r.y, r.z, n) *
                                   weights(r.x, r.y, r.z, c));
        if (bias)
        {
            Halide::Buffer<float> bias = wrapToHalideBuffer(blobs[1], {outC});
            topExpr += bias(c);
        }
        top(x, y, c, n) = topExpr;
        return Ptr<BackendNode>(new HalideBackendNode(top));
#endif  // HAVE_HALIDE
        return Ptr<BackendNode>();
    }

#ifdef HAVE_INF_ENGINE
    virtual Ptr<BackendNode> initInfEngine(const std::vector<Ptr<BackendWrapper> >&) CV_OVERRIDE
    {
        InferenceEngine::Builder::FullyConnectedLayer ieLayer(name);

        const int outNum = blobs[0].size[0];
        ieLayer.setOutputNum(outNum);

        InferenceEngine::Builder::Layer l = ieLayer;
        addConstantData("weights", wrapToInfEngineBlob(blobs[0], {(size_t)blobs[0].size[0], (size_t)blobs[0].size[1], 1, 1}, InferenceEngine::Layout::OIHW), l);
        if (blobs.size() > 1)
            addConstantData("biases", wrapToInfEngineBlob(blobs[1], {(size_t)outNum}, InferenceEngine::Layout::C), l);

        return Ptr<BackendNode>(new InfEngineBackendNode(l));
    }
#endif  // HAVE_INF_ENGINE

    virtual int64 getFLOPS(const std::vector<MatShape> &inputs,
                           const std::vector<MatShape> &outputs) const CV_OVERRIDE
    {
        CV_UNUSED(inputs); // suppress unused variable warning
        long flops = 0;

        int innerSize = blobs[0].size[1];
        for(int i = 0; i < outputs.size(); i++)
        {
            flops += CV_BIG_INT(3)*innerSize*total(outputs[i]);
        }

        return flops;

    }

    bool bias;
    Mat weightsMat, biasMat;
    Ptr<ActivationLayer> activ;
};

Ptr<InnerProductLayer> InnerProductLayer::create(const LayerParams& params)
{
    return Ptr<InnerProductLayer>(new FullyConnectedLayerImpl(params));
}

}
}
