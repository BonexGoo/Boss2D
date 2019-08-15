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
#include "../op_vkcom.hpp"
#include "opencv2/core/hal/hal.hpp"
#include "opencv2/core/hal/intrin.hpp"
#include <iostream>
#include <numeric>

#ifdef HAVE_OPENCL
#include "opencl_kernels_dnn.hpp"
using namespace cv::dnn::ocl4dnn;
#endif

namespace cv
{
namespace dnn
{

class BaseConvolutionLayerImpl : public ConvolutionLayer
{
public:
    bool fusedWeights, fusedBias;
    std::vector<double> weightsMultipliers;
    BaseConvolutionLayerImpl(const LayerParams &params)
    {
        setParamsFrom(params);
        getConvolutionKernelParams(params, kernel_size, pads_begin, pads_end, strides, dilations, padMode, adjust_pads);

        numOutput = params.get<int>("num_output");
        int ngroups = params.get<int>("group", 1);
        CV_Assert(numOutput % ngroups == 0);

        if (kernel_size.size() == 2) {
            kernel = Size(kernel_size[1], kernel_size[0]);
            stride = Size(strides[1], strides[0]);
            for (int i = 0; i < pads_begin.size(); i++) {
                if (pads_begin[i] != pads_end[i])
                    CV_Error(Error::StsNotImplemented, "Unsupported asymmetric padding in convolution layer");
            }
            pad = Size(pads_begin[1], pads_begin[0]);
            dilation = Size(dilations[1], dilations[0]);

            adjustPad.height = adjust_pads[0];
            adjustPad.width = adjust_pads[1];
        }

        for (int i = 0; i < adjust_pads.size(); i++) {
            CV_Assert(adjust_pads[i] < strides[i]);
        }

        fusedWeights = false;
        fusedBias = false;
    }

    virtual void finalize(InputArrayOfArrays inputs_arr, OutputArrayOfArrays outputs_arr) CV_OVERRIDE
    {
        std::vector<Mat> inputs, outputs;
        inputs_arr.getMatVector(inputs);
        outputs_arr.getMatVector(outputs);

        CV_Assert(inputs.size() > 0);

        CV_Assert(blobs.size() == 1 || blobs.size() == 2);
        CV_Assert(inputs[0].dims == outputs[0].dims);
        CV_Assert(blobs[0].dims == kernel_size.size() + 2);
        for (int i = 0; i < kernel_size.size(); i++) {
            CV_Assert(blobs[0].size[i + 2] == kernel_size[i]);
        }

        const Mat &input = inputs[0];
        CV_Assert((input.dims == 4 || input.dims == 5) && (input.type() == CV_32F || input.type() == CV_16S));
        for (size_t i = 0; i < inputs.size(); i++)
        {
            CV_Assert(inputs[i].type() == input.type());
            CV_Assert((inputs[i].dims == 4 || inputs[i].dims == 5) && inputs[i].size[1] == input.size[1]);
            for (int j = 0; j < inputs[i].dims; j++) {
                CV_Assert(inputs[i].size[j] == input.size[j]);
            }
        }

        std::vector<int> inpShape;
        std::vector<int> outShape;
        for (int i = 2; i < inputs[0].dims; i++) {
            inpShape.push_back(inputs[0].size[i]);
            outShape.push_back(outputs[0].size[i]);
        }
        getConvPoolPaddings(inpShape, kernel_size, strides, padMode, pads_begin, pads_end);
        if (pads_begin.size() == 2) {
            for (int i = 0; i < pads_begin.size(); i++) {
                if (pads_begin[i] != pads_end[i])
                    CV_Error(Error::StsNotImplemented, "Unsupported asymmetric padding in convolution layer");
            }
            pad = Size(pads_begin[1], pads_begin[0]);
        }
        fusedWeights = false;
        fusedBias = false;
    }

    bool hasBias() const
    {
        return blobs.size() >= 2;
    }

    virtual MatShape computeColRowShape(const MatShape &inpShape, const MatShape &outShape) const = 0;
    bool is1x1() const
    {
        return (kernel.height == 1 && kernel.width == 1) &&
               (stride.height == 1 && stride.width == 1) &&
               (dilation.height == 1 && dilation.width == 1);
    }

    virtual bool tryFuse(Ptr<Layer>& top) CV_OVERRIDE
    {
        Mat w, b;
        top->getScaleShift(w, b);
        if (!w.empty() || !b.empty())
        {
            fuseWeights(w, b);
            fusedWeights = fusedWeights || !w.empty();
            fusedBias = fusedBias || (hasBias() && !w.empty()) || !b.empty();
            return true;
        }
        return false;
    }

    virtual void fuseWeights(const Mat& w_, const Mat& b_) = 0;

    virtual void applyHalideScheduler(Ptr<BackendNode>& node,
                                      const std::vector<Mat*> &inputs,
                                      const std::vector<Mat> &outputs,
                                      int targetId) const CV_OVERRIDE
    {
#ifdef HAVE_HALIDE
        if (targetId != DNN_TARGET_CPU)
        {
            Layer::applyHalideScheduler(node, inputs, outputs, targetId);
            return;
        }
        Halide::Var x("x"), y("y"), c("c"), n("n"), tile("tile"), yi("yi"), yo("yo"), co("co"), ci("ci");
        Halide::Func& top = node.dynamicCast<HalideBackendNode>()->funcs[1];
        Halide::Func& padded_input = node.dynamicCast<HalideBackendNode>()->funcs[0];

        int outW, outH, outC, outN;
        getCanonicalSize(outputs[0].size, &outW, &outH, &outC, &outN);

        if (outW == 1 || outH <= 2)
            return;

        if (is1x1() || outC <= 16)
            top.reorder(x, c, y)
               .split(y, yo, yi, 2)
               .fuse(yo, n, tile)
               .parallel(tile)
               .unroll(yi)
               .vectorize(x, outW >= 16 ? 16 : outW);
        else
            top.reorder(x, c, y)
               .split(y, yo, yi, 2)
               .split(c, co, ci, 16)
               .fuse(yo, co, tile).fuse(n, tile, tile)
               .parallel(tile)
               .unroll(yi)
               .vectorize(x, outW >= 16 ? 16 : outW);
        padded_input.compute_at(top, yi);
#endif  // HAVE_HALIDE
    }
};


#define IS_POWER_LAYER(layer) \
            (!layer.empty() && !layer->type.compare("Power"))
//TODO: simultaneously convolution and bias addition for cache optimization
class ConvolutionLayerImpl CV_FINAL : public BaseConvolutionLayerImpl
{
public:
    enum { VEC_ALIGN = 8, DFT_TYPE = CV_32F };
    Mat weightsMat;
    std::vector<float> biasvec;
    std::vector<float> reluslope;
    Ptr<ActivationLayer> activ;

#ifdef HAVE_OPENCL
    Ptr<OCL4DNNConvSpatial<float> > convolutionOp;
    std::vector<UMat> umat_blobs;
    bool newActiv;
    ocl4dnnFusedActiv_t activType;
    float power;
#endif
    ConvolutionLayerImpl(const LayerParams &params) : BaseConvolutionLayerImpl(params)
    {
#ifdef HAVE_OPENCL
        newActiv = false;
        activType = OCL4DNN_CONV_FUSED_ACTIV_NONE;
        power = 0.f;
#endif
    }

    MatShape computeColRowShape(const MatShape &inpShape, const MatShape &outShape) const CV_OVERRIDE
    {
        Size out(outShape[3], outShape[2]);
        int inpGroupCn = blobs[0].size[1];
        int ksize = inpGroupCn * kernel.height * kernel.width;
        return shape(out.area(), ksize);
    }

    virtual bool supportBackend(int backendId) CV_OVERRIDE
    {
#ifdef HAVE_INF_ENGINE
        if (backendId == DNN_BACKEND_INFERENCE_ENGINE)
        {
            if (kernel_size.size() == 3)
                return preferableTarget == DNN_TARGET_CPU;
            return (preferableTarget != DNN_TARGET_MYRIAD || dilation.width == dilation.height);
        }
        else
#endif
        {
            if (kernel_size.size() == 3)
                return (preferableTarget == DNN_TARGET_CPU && backendId == DNN_BACKEND_OPENCV);
            else if (kernel_size.size() == 2)
                return backendId == DNN_BACKEND_OPENCV ||
                       backendId == DNN_BACKEND_HALIDE ||
                       (backendId == DNN_BACKEND_VKCOM && haveVulkan());
            else
                return false;
        }
    }

    bool getMemoryShapes(const std::vector<MatShape> &inputs,
                         const int requiredOutputs,
                         std::vector<MatShape> &outputs,
                         std::vector<MatShape> &internals) const CV_OVERRIDE
    {
        CV_Assert(blobs.size() != 0);
        CV_Assert(!hasBias() || blobs[1].total() == (size_t)blobs[0].size[0]);
        CV_Assert(inputs.size() == (size_t)1);

        internals.clear();

        CV_Assert(inputs.size() != 0);
        std::vector<int> inpShape(inputs[0].begin() + 2, inputs[0].end());

        int outCn = blobs[0].size[0];
        std::vector<int> outShape;
        outShape.push_back(inputs[0][0]);
        outShape.push_back(outCn);

        int inpCn = inputs[0][1];
        if (padMode.empty())
        {
            for (int i = 0; i < inpShape.size(); i++)
                outShape.push_back((inpShape[i] + pads_begin[i] + pads_end[i] - dilations[i] * (kernel_size[i] - 1) - 1) / strides[i] + 1);
        }
        else
        {
            getConvPoolOutParams(inpShape, kernel_size, strides, padMode, dilations, outShape);
        }

        int ngroups = inpCn / blobs[0].size[1];
        if (ngroups == 0 || ngroups * blobs[0].size[1] != inpCn)
            CV_Error(Error::StsError, format("Number of input channels should "
                     "be multiple of %d but got %d", blobs[0].size[1], inpCn));
        CV_Assert(ngroups > 0 && inpCn % ngroups == 0 && outCn % ngroups == 0);

        outputs.resize(1, outShape);

        return false;
    }

    virtual void finalize(InputArrayOfArrays inputs_arr, OutputArrayOfArrays outputs_arr) CV_OVERRIDE
    {
        BaseConvolutionLayerImpl::finalize(inputs_arr, outputs_arr);

        CV_Assert(!blobs.empty());
        const int outCn = blobs[0].size[0];
        // prepare weightsMat where each row is aligned and has enough zero padding on the right to
        // use vectorized (i.e. with intrinsics) loops without tail processing
        Mat wm = blobs[0].reshape(1, outCn);
        if( wm.step1() % VEC_ALIGN != 0 )
        {
            int newcols = (int)alignSize(wm.step1(), VEC_ALIGN);
            Mat wm_buffer = Mat(outCn, newcols, wm.type());
            Mat wm_padding = wm_buffer.colRange(wm.cols, newcols);
            wm_padding.setTo(Scalar::all(0.));
            Mat wm_aligned = wm_buffer.colRange(0, wm.cols);
            wm.copyTo(wm_aligned);
            wm = wm_aligned;
        }
        weightsMat = wm;
        weightsMultipliers.assign(outCn, 1.0);

        Mat biasMat = hasBias() ? blobs[1].reshape(1, outCn) : Mat();
        biasvec.resize(outCn+2);
        if( biasMat.empty() )
        {
            for(int i = 0; i < outCn; i++ )
                biasvec[i] = 0.f;
        }
        else
        {
            for(int i = 0; i < outCn; i++ )
                biasvec[i] = biasMat.at<float>(i);
        }
#ifdef HAVE_OPENCL
        convolutionOp.release();
#endif
    }

    bool setActivation(const Ptr<ActivationLayer>& layer) CV_OVERRIDE
    {
        if (!activ.empty() && !layer.empty())
            return false;

        activ = layer;
        if (activ.empty())
            reluslope.clear();
#ifdef HAVE_OPENCL
        newActiv = true;
        activType = OCL4DNN_CONV_FUSED_ACTIV_NONE;

        if (IS_DNN_OPENCL_TARGET(preferableTarget))
        {
            Ptr<PowerLayer> activ_power = activ.dynamicCast<PowerLayer>();
            if (!activ_power.empty())
            {
                if (activ_power->scale != 1.f || activ_power->shift != 0.f)
                {
                    const int outCh = blobs[0].size[0];
                    fuseWeights(Mat(1, outCh, CV_32F, Scalar(activ_power->scale)),
                                Mat(1, outCh, CV_32F, Scalar(activ_power->shift)));
                }

                power = activ_power->power;
                activType = OCL4DNN_CONV_FUSED_ACTIV_POWER;
            }
            Ptr<TanHLayer> activ_tanh = activ.dynamicCast<TanHLayer>();
            if (!activ_tanh.empty())
            {
                activType = OCL4DNN_CONV_FUSED_ACTIV_TANH;
            }
        }
#endif
        return !activ.empty();
    }

    void fuseWeights(const Mat& w_, const Mat& b_) CV_OVERRIDE
    {
        // Convolution weights have OIHW data layout. Parameters fusion in case of
        // (conv(I) + b1 ) * w + b2
        // means to replace convolution's weights to [w*conv(I)] and bias to [b1 * w + b2]
        const int outCn = weightsMat.size[0];
        Mat w = w_.total() == 1 ? Mat(1, outCn, CV_32F, Scalar(w_.at<float>(0))) : w_;
        Mat b = b_.total() == 1 ? Mat(1, outCn, CV_32F, Scalar(b_.at<float>(0))) : b_;
        CV_Assert_N(!weightsMat.empty(), biasvec.size() == outCn + 2,
                    w.empty() || outCn == w.total(), b.empty() || outCn == b.total());

        if (!w.empty())
        {
            // Keep origin weights unchanged.
            if (weightsMat.data == blobs[0].data)
                weightsMat = weightsMat.clone();

            Mat originWeights = blobs[0].reshape(1, outCn);
            for (int i = 0; i < outCn; ++i)
            {
                double wi = w.at<float>(i);
                weightsMultipliers[i] *= wi;
                cv::multiply(originWeights.row(i), weightsMultipliers[i], weightsMat.row(i));
                biasvec[i] *= wi;
            }
        }

        if (!b.empty())
        {
            for (int i = 0; i < outCn; ++i)
                biasvec[i] += b.at<float>(i);
        }
        biasvec[outCn] = biasvec[outCn+1] = biasvec[outCn-1];
    }

    virtual Ptr<BackendNode> initVkCom(const std::vector<Ptr<BackendWrapper> > &inputs) CV_OVERRIDE
    {
#ifdef HAVE_VULKAN
        int out_channel = blobs[0].size[0];
        bool has_bias = hasBias() || fusedBias;
        int filter_size[2] = {kernel.height, kernel.width};
        int pad_size[2] = {pad.height, pad.width};
        int stride_size[2] = {stride.height, stride.width};
        int dilation_size[2] = {dilation.height, dilation.width};
        int activation = 0;
        vkcom::Tensor input_tensor = VkComTensor(inputs[0]);
        int in_channel = input_tensor.dimSize(1);
        int group = in_channel / blobs[0].size[1];

        // TODO: support group > 1
        if (group != 1)
            return Ptr<BackendNode>();

        int padding_mode;
        if (padMode.empty())
        {
            padding_mode = vkcom::kPaddingModeCaffe;
        }
        else if (padMode == "VALID")
        {
            padding_mode = vkcom::kPaddingModeValid;
        }
        else if (padMode == "SAME")
        {
            padding_mode = vkcom::kPaddingModeSame;
        }
        else
            CV_Error(Error::StsError, "Unsupported padding mode " + padMode);

        std::shared_ptr<vkcom::OpBase> op(new vkcom::OpConv(out_channel, has_bias,
                    filter_size, pad_size,
                    stride_size, dilation_size,
                    activation, group,
                    padding_mode));

        std::vector<Ptr<BackendWrapper> > blobsWrapper;

        if (fusedWeights)
        {
            Mat wm;
            weightsMat.copyTo(wm); // to handle the case of isContinuous() == false
            wm = wm.reshape(1, blobs[0].dims, blobs[0].size);
            blobsWrapper.push_back(Ptr<BackendWrapper>(new VkComBackendWrapper(wm)));
        }
        else
        {
            blobsWrapper.push_back(Ptr<BackendWrapper>(new VkComBackendWrapper(blobs[0])));
        }

        if (has_bias)
        {
            Mat biasesMat({out_channel}, CV_32F, &biasvec[0]);
            blobsWrapper.push_back(Ptr<BackendWrapper>(new VkComBackendWrapper(biasesMat)));
        }

        return Ptr<BackendNode>(new VkComBackendNode(inputs, op, blobsWrapper));
#endif  // HAVE_VULKAN
        return Ptr<BackendNode>();
    }



    virtual Ptr<BackendNode> initHalide(const std::vector<Ptr<BackendWrapper> > &inputs) CV_OVERRIDE
    {
#ifdef HAVE_HALIDE
        Halide::Buffer<float> inputBuffer = halideBuffer(inputs[0]);

        const int inpCn = inputBuffer.channels();
        const int outCn = blobs[0].size[0];
        const int inpGroupCn = blobs[0].size[1];
        const int group = inpCn / inpGroupCn;
        const int outGroupCn = outCn / group;

        Halide::Buffer<float> weights = wrapToHalideBuffer(blobs[0]);

        Halide::Var x("x"), y("y"), c("c"), n("n");
        Halide::Func top = (name.empty() ? Halide::Func() : Halide::Func(name));
        Halide::Func padded_input(name + "_constant_exterior");
        if (pad.width || pad.height)
        {
            Halide::Func bounded =
                Halide::BoundaryConditions::constant_exterior(inputBuffer, 0);
            padded_input(x, y, c, n) = bounded(x, y, c, n);
        }
        else
        {
            padded_input(x, y, c, n) = inputBuffer(x, y, c, n);
        }

        Halide::RDom r(0, kernel.width, 0, kernel.height, 0, inpGroupCn);
        Halide::Expr kx = x * stride.width - pad.width + r.x * dilation.width;
        Halide::Expr ky = y * stride.height - pad.height + r.y * dilation.height;
        Halide::Expr kc = r.z;
        for (int i = 1; i < group; ++i)
        {
            kc = select(c < outGroupCn * i, kc, inpGroupCn * i + r.z);
        }
        Halide::Expr topExpr = sum(padded_input(kx, ky, kc, n) *
                                   weights(r.x, r.y, r.z, c));
        if (hasBias())
        {
            Halide::Buffer<float> bias = wrapToHalideBuffer(blobs[1], {outCn});
            topExpr += bias(c);
        }
        top(x, y, c, n) = topExpr;
        return Ptr<BackendNode>(new HalideBackendNode({ padded_input, top }));
#endif  // HAVE_HALIDE
        return Ptr<BackendNode>();
    }

#ifdef HAVE_INF_ENGINE
    virtual Ptr<BackendNode> initInfEngine(const std::vector<Ptr<BackendWrapper> > &inputs) CV_OVERRIDE
    {
        InferenceEngine::DataPtr input = infEngineDataNode(inputs[0]);
        CV_Assert(input->dims.size() == 4 || input->dims.size() == 5);

        const int inpCn = input->dims[input->dims.size() - 2];  // NOTE: input->dims are reversed (WHIO or WHDIO)
        const int outCn = blobs[0].size[0];
        const int inpGroupCn = blobs[0].size[1];
        const int group = inpCn / inpGroupCn;

        InferenceEngine::Layout layout = (input->dims.size() == 4) ? InferenceEngine::Layout::OIHW :
                                                                     InferenceEngine::Layout::NCDHW;

        auto ieWeights = wrapToInfEngineBlob(blobs[0], layout);
        if (fusedWeights)
        {
            if (weightsMat.isContinuous())
            {
                Mat cvWeights = weightsMat.reshape(1, blobs[0].dims, blobs[0].size);
                ieWeights = wrapToInfEngineBlob(cvWeights, layout);
            }
            else
            {
                ieWeights = InferenceEngine::make_shared_blob<float>(
                                    InferenceEngine::Precision::FP32, layout,
                                    ieWeights->dims());
                ieWeights->allocate();

                Mat newWeights = infEngineBlobToMat(ieWeights).reshape(1, outCn);
                Mat cvWeights = weightsMat.colRange(0, newWeights.cols);
                cvWeights.copyTo(newWeights);
            }
        }
        InferenceEngine::Blob::Ptr ieBiases;
        if (hasBias() || fusedBias)
        {
            Mat biasesMat({outCn}, CV_32F, &biasvec[0]);
            ieBiases = wrapToInfEngineBlob(biasesMat, {(size_t)outCn}, InferenceEngine::Layout::C);
        }

        InferenceEngine::Builder::ConvolutionLayer ieLayer(name);

        ieLayer.setKernel(kernel_size);
        ieLayer.setStrides(strides);
        ieLayer.setDilation(dilations);
        ieLayer.setPaddingsBegin(pads_begin);
        ieLayer.setPaddingsEnd(pads_end);
        ieLayer.setGroup((size_t)group);
        ieLayer.setOutDepth((size_t)outCn);

        InferenceEngine::Builder::Layer l = ieLayer;
        addConstantData("weights", ieWeights, l);
        if (ieBiases)
            addConstantData("biases", ieBiases, l);

        if (!padMode.empty())
            l.getParameters()["auto_pad"] = padMode == "VALID" ? std::string("valid") : std::string("same_upper");

        return Ptr<BackendNode>(new InfEngineBackendNode(l));
    }
#endif  // HAVE_INF_ENGINE

    class ParallelConv : public cv::ParallelLoopBody
    {
    public:
        enum { BLK_SIZE = 32, BLK_SIZE_CN = 64 };

        const Mat* input_;
        const Mat* weights_;
        Mat* output_;
        int outShape[4]; // used only for conv2d
        std::vector<size_t> kernel_size, pads_begin, pads_end, strides, dilations;
        int ngroups_, nstripes_;
        std::vector<int> ofstab_;
        const std::vector<float>* biasvec_;
        const std::vector<float>* reluslope_;
        const ActivationLayer* activ_;
        bool is1x1_;
        bool useAVX;
        bool useAVX2;
        bool useAVX512;

        ParallelConv()
            : input_(0), weights_(0), output_(0), ngroups_(0), nstripes_(0),
              biasvec_(0), reluslope_(0), activ_(0), is1x1_(false), useAVX(false), useAVX2(false), useAVX512(false)
        {}

        static void run( const Mat& input, Mat& output, const Mat& weights,
                         const std::vector<float>& biasvec,
                         const std::vector<float>& reluslope,
                         const std::vector<size_t>& kernel_size, const std::vector<size_t>& strides,
                         const std::vector<size_t>& pads_begin, const std::vector<size_t>& pads_end,
                         const std::vector<size_t>& dilations,
                         const ActivationLayer* activ, int ngroups, int nstripes )
        {
            size_t karea = std::accumulate(kernel_size.begin(), kernel_size.end(),
                                           1, std::multiplies<size_t>());
            CV_Assert_N(
                       (input.dims == 4 || input.dims == 5) && (input.dims == output.dims),
                       input.size[0] == output.size[0],
                       weights.rows == output.size[1],
                       weights.cols == (input.size[1]/ngroups)*karea,
                       input.type() == output.type(),
                       input.type() == weights.type(),
                       input.type() == CV_32FC1,
                       input.isContinuous(),
                       output.isContinuous(),
                       biasvec.size() == (size_t)output.size[1]+2);
            ParallelConv p;

            p.input_ = &input;
            p.weights_ = &weights;
            p.output_ = &output;
            for( int i = 0; i < 4; i++ ) p.outShape[i] = output.size[i];
            p.outShape[1] /= ngroups;

            p.kernel_size = kernel_size; p.strides = strides; p.dilations = dilations;
            p.pads_begin = pads_begin; p.pads_end = pads_end;

            p.ngroups_ = ngroups;
            p.nstripes_ = nstripes;

            int inpCnAll = input.size[1];
            int depth = (input.dims == 5) ? input.size[2] : 1;
            int width = input.size[input.dims - 1];
            int height = input.size[input.dims - 2];
            int inpCn = inpCnAll / ngroups;

            bool isConv2D = kernel_size.size() == 2;

            p.is1x1_ = isConv2D && kernel_size[0] == 1 && kernel_size[1] == 1 &&
                       pads_begin[0] == 0  && pads_begin[1] == 0;

            p.useAVX    = checkHardwareSupport(CPU_AVX)  && isConv2D;
            p.useAVX2   = checkHardwareSupport(CPU_AVX2) && isConv2D;
            p.useAVX512 = CV_CPU_HAS_SUPPORT_AVX512_SKX  && isConv2D;

            int ncn = std::min(inpCn, (int)BLK_SIZE_CN);

            int kernel_d = !isConv2D? kernel_size[0] : 1;
            int kernel_h = kernel_size[kernel_size.size() - 2];
            int kernel_w = kernel_size.back();

            int dil_d = !isConv2D? dilations[0] : 1;
            int dil_h = dilations[dilations.size() - 2];
            int dil_w = dilations.back();

            p.ofstab_.resize(karea * ncn);
            int* ofstab = &p.ofstab_[0];

            if (isConv2D)
            {
                for( int k = 0; k < ncn; k++ )
                    for( int k_r = 0; k_r < kernel_h; k_r++ )
                        for( int k_c = 0; k_c < kernel_w; k_c++ )
                            ofstab[(k*kernel_h + k_r)*kernel_w + k_c] =
                                   (k*height + k_r*dil_h)*width + k_c*dil_w;
            }
            else
            {
                for( int k = 0; k < ncn; k++ )
                    for (int k_d = 0; k_d < kernel_d; k_d++)
                        for( int k_r = 0; k_r < kernel_h; k_r++ )
                            for( int k_c = 0; k_c < kernel_w; k_c++ )
                                ofstab[(k*kernel_d*kernel_h + k_d*kernel_h + k_r)*kernel_w + k_c] =
                                       (k*depth*height + k_d*dil_d*height + k_r*dil_h)*width + k_c*dil_w;
            }

            p.biasvec_ = &biasvec;
            p.reluslope_ = &reluslope;
            p.activ_ = p.reluslope_->empty() ? activ : 0;

            parallel_for_(Range(0, nstripes), p, nstripes);
        }

        virtual void operator ()(const Range &r0) const CV_OVERRIDE
        {
            const int valign = ConvolutionLayerImpl::VEC_ALIGN;
            int ngroups = ngroups_, batchSize = input_->size[0]*ngroups;
            bool isConv2D = input_->dims == 4;

            int outW = output_->size[output_->dims - 1];
            int outH = output_->size[output_->dims - 2];
            int outCn = output_->size[1]/ngroups;

            int depth = !isConv2D? input_->size[2] : 1;
            int height = input_->size[input_->dims - 2];
            int width = input_->size[input_->dims - 1];
            int inpCn = input_->size[1]/ngroups;

            const int nstripes = nstripes_;

            int kernel_d = !isConv2D? kernel_size[0] : 1;
            int kernel_h = kernel_size[kernel_size.size() - 2];
            int kernel_w = kernel_size.back();
            int karea = kernel_w*kernel_h*kernel_d;

            int pad_d = !isConv2D? pads_begin[0] : 0;
            int pad_t = pads_begin[pads_begin.size() - 2];
            int pad_l = pads_begin.back();

            int stride_d = !isConv2D? strides[0] : 0;
            int stride_h = strides[strides.size() - 2];
            int stride_w = strides.back();

            int dilation_d = !isConv2D? dilations[0] : 1;
            int dilation_h = dilations[dilations.size() - 2];
            int dilation_w = dilations.back();

            int i, j, k, d;
            size_t inpPlaneSize = input_->total(2);
            size_t outPlaneSize = output_->total(2);
            bool is1x1 = is1x1_;

            int stripesPerSample;
            size_t stripeSize;
            Range r = r0;

            if( nstripes >= batchSize*2 )
            {
                stripesPerSample = nstripes/batchSize;
                stripeSize = alignSize((outPlaneSize + stripesPerSample - 1)/stripesPerSample, valign);
                stripeSize = std::min(stripeSize, outPlaneSize);
            }
            else
            {
                stripesPerSample = 1;
                int samplesPerStripe = std::max((batchSize + nstripes - 1)/nstripes, 1);
                r.start *= samplesPerStripe;
                r.end *= samplesPerStripe;
                stripeSize = outPlaneSize;
            }

            const float* data_inp0_ = input_->ptr<float>();
            const int* ofstab = &ofstab_[0];
            const float* wptr_orig_ = weights_->ptr<float>();
            size_t wstep = weights_->step1();
            const float* biasptr_ = &biasvec_->at(0);
            const float* reluptr_ = reluslope_->empty() ? 0 : &reluslope_->at(0);
            float* data_out0_ = output_->ptr<float>();
            size_t rowbufsz = (size_t)karea*BLK_SIZE_CN*BLK_SIZE;
            AutoBuffer<float> rowbuf0_(rowbufsz + valign);
            float* rowbuf0 = alignPtr(rowbuf0_.data(), (int)(valign*sizeof(float)));

            // we clear the buffer once; ultimately, it lets us to avoid
            // tail processing after running the unrolled/vectorized loop.
            // the main idea is to make sure that the tail (a.k.a. padding) of each row
            // (i.e. the elements with indices between vsz=karea*ncn and vsz_a)
            // does not contain NaNs or Infs. Because the padding in the weights
            // matrix is explicitly initialized with 0's, we handle all other
            // cases nicely, i.e. we can skip expliciting re-initialization
            // of the padding - we just retain elements from the previous iteration
            // of the loop over channels (cn0).
            memset(rowbuf0, 0, rowbufsz*sizeof(rowbuf0[0]) );

            for( int stripe = r.start; stripe < r.end; stripe++ )
            {
                int subsampleIdx = stripe/stripesPerSample;
                if( subsampleIdx >= batchSize )
                    break;
                int stripeStart = (int)((stripe - subsampleIdx*stripesPerSample)*stripeSize);
                int stripeEnd = (int)std::min(stripeStart + stripeSize, outPlaneSize);
                const float* data_inp0 = data_inp0_ + subsampleIdx*inpPlaneSize*inpCn;
                float* data_out0 = data_out0_ + subsampleIdx*outPlaneSize*outCn;
                int startOutCn = (subsampleIdx % ngroups)*outCn;
                const float* wptr_orig = wptr_orig_ + wstep*startOutCn;
                const float* biasptr = biasptr_ + startOutCn;

                for( int cn0 = 0; cn0 < inpCn; cn0 += BLK_SIZE_CN )
                {
                    int cn1 = std::min(cn0 + BLK_SIZE_CN, inpCn);
                    int ncn = cn1 - cn0, vsz = karea*ncn;
                    int vsz_a = (int)alignSize(vsz, valign);
                    const float* wptr = wptr_orig + cn0*karea;
                    // we apply [Channels][P]ReLU (if any) during the final pass only.
                    const float* relu = cn1 == inpCn && reluptr_ ? reluptr_ + startOutCn : 0;

                    for( int ofs0 = stripeStart; ofs0 < stripeEnd; ofs0 += BLK_SIZE )
                    {
                        int ofs, ofs1 = std::min(ofs0 + BLK_SIZE, stripeEnd);

                        int out_d = ofs0 / (outH * outW);
                        int out_i = (ofs0 - out_d * outH * outW) / outW;
                        int out_j = ofs0 % outW;

                        // do im2row for a part of input tensor
                        float* rowbuf = rowbuf0;

                        if (isConv2D)
                        {
                            for( ofs = ofs0; ofs < ofs1; out_j = 0, ++out_i )
                            {
                                int delta = std::min(ofs1 - ofs, outW - out_j);
                                int out_j1 = out_j + delta;

                                int in_i = out_i * stride_h - pad_t;
                                int in_j = out_j * stride_w - pad_l;
                                const float* imgptr = data_inp0 + (cn0*height + in_i)*width + in_j;
                                ofs += delta;

                                // do im2row for a part of input tensor
                                if( is1x1 )
                                {
                                    for( ; out_j < out_j1; out_j++, rowbuf += vsz_a, imgptr += stride_w )
                                    {
                                        for( k = 0; k < vsz; k++ )
                                            rowbuf[k] = imgptr[k*inpPlaneSize];
                                    }
                                }
                                else
                                {
                                    bool ok_i = 0 <= in_i && in_i < height - (kernel_h-1)*dilation_h;
                                    int i0 = std::max(0, (-in_i + dilation_h-1)/dilation_h);
                                    int i1 = std::min(kernel_h, (height - in_i + dilation_h-1)/dilation_h);

                                    for( ; out_j < out_j1; out_j++, rowbuf += vsz_a, imgptr += stride_w, in_j += stride_w )
                                    {
                                        // this condition should be true for most of the tensor elements, i.e.
                                        // most of the time the kernel aperture is inside the tensor X-Y plane.
                                        if( ok_i && out_j + 2 <= out_j1 && 0 <= in_j && in_j + stride_w*2 <= width - (kernel_w-1)*dilation_w )
                                        {
                                            for( k = 0; k < vsz; k++ )
                                            {
                                                int k1 = ofstab[k];
                                                float v0 = imgptr[k1];
                                                float v1 = imgptr[k1 + stride_w];
                                                rowbuf[k] = v0;
                                                rowbuf[k+vsz_a] = v1;
                                            }
                                            out_j++;
                                            rowbuf += vsz_a;
                                            imgptr += stride_w;
                                            in_j += stride_w;
                                        }
                                        else
                                        {
                                            int j0 = std::max(0, (-in_j + dilation_w-1)/dilation_w);
                                            int j1 = std::min(kernel_w, (width - in_j + dilation_w-1)/dilation_w);

                                            // here some non-continuous sub-row of the row will not be
                                            // filled from the tensor; we need to make sure that the uncovered
                                            // elements are explicitly set to 0's. the easiest way is to
                                            // set all the elements to 0's before the loop.
                                            memset(rowbuf, 0, vsz*sizeof(rowbuf[0]));
                                            for( k = 0; k < ncn; k++ )
                                            {
                                                for( i = i0; i < i1; i++ )
                                                {
                                                    for( j = j0; j < j1; j++ )
                                                    {
                                                        int imgofs = k*(width*height) + i*(dilation_h*width) + j*dilation_w;
                                                        rowbuf[(k*kernel_h + i)*kernel_w + j] = imgptr[imgofs];
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            for( ofs = ofs0; ofs < ofs1; out_d += (out_i + 1) / outH, out_i = (out_i + 1) % outH, out_j = 0 )
                            {
                                int delta = std::min(ofs1 - ofs, outW - out_j);
                                int out_j1 = out_j + delta;

                                int in_d = out_d * stride_d - pad_d;
                                int in_i = out_i * stride_h - pad_t;
                                int in_j = out_j * stride_w - pad_l;
                                const float* imgptr = data_inp0 + (cn0*depth*height + in_d*height + in_i)*width + in_j;
                                ofs += delta;

                                int d0 = std::max(0, (-in_d + dilation_d - 1) / dilation_d);
                                int d1 = std::min(kernel_d, (depth - in_d + dilation_d - 1) / dilation_d);

                                int i0 = std::max(0, (-in_i + dilation_h-1)/dilation_h);
                                int i1 = std::min(kernel_h, (height - in_i + dilation_h-1)/dilation_h);

                                for( ; out_j < out_j1; out_j++, rowbuf += vsz_a, imgptr += stride_w, in_j += stride_w )
                                {
                                    int j0 = std::max(0, (-in_j + dilation_w-1)/dilation_w);
                                    int j1 = std::min(kernel_w, (width - in_j + dilation_w-1)/dilation_w);

                                    // here some non-continuous sub-row of the row will not be
                                    // filled from the tensor; we need to make sure that the uncovered
                                    // elements are explicitly set to 0's. the easiest way is to
                                    // set all the elements to 0's before the loop.
                                    memset(rowbuf, 0, vsz*sizeof(rowbuf[0]));
                                    for( k = 0; k < ncn; k++ )
                                    {
                                        for ( d = d0; d < d1; d++)
                                        {
                                            for( i = i0; i < i1; i++ )
                                            {
                                                for( j = j0; j < j1; j++ )
                                                {
                                                    int imgofs = k*(depth*width*height) + d*dilation_d*width*height + i*(dilation_h*width) + j*dilation_w;
                                                    rowbuf[(k*kernel_d*kernel_h + d*kernel_h + i)*kernel_w + j] = imgptr[imgofs];
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // now compute dot product of the weights
                        // and im2row-transformed part of the tensor
                        int bsz = ofs1 - ofs0;
                    #if CV_TRY_AVX512_SKX
                        /* AVX512 convolution requires an alignment of 16, and ROI is only there for larger vector sizes */
                        if(useAVX512)
                            opt_AVX512_SKX::fastConv(wptr, wstep, biasptr, rowbuf0, data_out0 + ofs0,
                                          outShape, bsz, vsz, vsz_a, relu, cn0 == 0);
                        else
                    #endif
                    #if CV_TRY_AVX2
                        if(useAVX2)
                            opt_AVX2::fastConv(wptr, wstep, biasptr, rowbuf0, data_out0 + ofs0,
                                          outShape, bsz, vsz, vsz_a, relu, cn0 == 0);
                        else
                    #endif
                    #if CV_TRY_AVX
                        if(useAVX)
                            opt_AVX::fastConv(wptr, wstep, biasptr, rowbuf0, data_out0 + ofs0,
                                         outShape, bsz, vsz, vsz_a, relu, cn0 == 0);
                        else
                    #endif
                        for( int i = 0; i < outCn; i += 2 )
                        {
                            const float* wptr0 = wptr + i*wstep;
                            const float* wptr1 = wptr0 + wstep;
                            float* outptr0 = data_out0 + ofs0 + i*outPlaneSize;
                            float* outptr1 = outptr0 + outPlaneSize;
                            float bias0 = biasptr[i], bias1 = biasptr[i+1];
                            float r0 = 1.f, r1 = 1.f;

                            if( i+1 >= outCn )
                            {
                                wptr1 = wptr0;
                                outptr1 = outptr0;
                                bias1 = bias0;
                            }

                            if( relu )
                            {
                                r0 = relu[i]; r1 = relu[i+1];
                                if( i+1 >= outCn )
                                    r1 = r0;
                            }

                            int j = 0;
                        #if CV_SIMD128
                            v_float32x4 vr0 = v_setall_f32(r0), vr1 = v_setall_f32(r1), z = v_setzero_f32();

                            for( ; j <= bsz - 4; j += 4 )
                            {
                                const float* rptr = rowbuf0 + j*vsz_a;
                                v_float32x4 s0, s1;

                                if( cn0 == 0 )
                                {
                                    s0 = v_setall_f32(bias0);
                                    s1 = v_setall_f32(bias1);
                                }
                                else
                                {
                                    s0 = v_load(outptr0 + j);
                                    s1 = v_load(outptr1 + j);
                                }

                                v_float32x4 vs00 = v_setzero_f32(), vs01 = v_setzero_f32(),
                                            vs02 = v_setzero_f32(), vs03 = v_setzero_f32(),
                                            vs10 = v_setzero_f32(), vs11 = v_setzero_f32(),
                                            vs12 = v_setzero_f32(), vs13 = v_setzero_f32();
                                for( k = 0; k < vsz; k += 4, rptr += 4 )
                                {
                                    v_float32x4 w0 = v_load_aligned(wptr0 + k), w1 = v_load_aligned(wptr1 + k);
                                    v_float32x4 r0 = v_load_aligned(rptr), r1 = v_load_aligned(rptr + vsz_a),
                                                r2 = v_load_aligned(rptr + vsz_a*2), r3 = v_load_aligned(rptr + vsz_a*3);

                                    vs00 += w0*r0;
                                    vs01 += w0*r1;
                                    vs02 += w0*r2;
                                    vs03 += w0*r3;

                                    vs10 += w1*r0;
                                    vs11 += w1*r1;
                                    vs12 += w1*r2;
                                    vs13 += w1*r3;
                                }
                                s0 += v_reduce_sum4(vs00, vs01, vs02, vs03);
                                s1 += v_reduce_sum4(vs10, vs11, vs12, vs13);
                                if( relu )
                                {
                                    s0 = v_select(s0 > z, s0, s0*vr0);
                                    s1 = v_select(s1 > z, s1, s1*vr1);
                                }

                                v_store(outptr0 + j, s0);
                                v_store(outptr1 + j, s1);
                            }
                        #endif
                            for( ; j < bsz; j++ )
                            {
                                const float* rptr = rowbuf0 + j*vsz_a;
                                float s00, s10;

                                if( cn0 == 0 )
                                {
                                    s00 = bias0;
                                    s10 = bias1;
                                }
                                else
                                {
                                    s00 = outptr0[j];
                                    s10 = outptr1[j];
                                }

                                for( k = 0; k < vsz; k++ )
                                {
                                    float r0 = rptr[k];
                                    s00 += wptr0[k]*r0;
                                    s10 += wptr1[k]*r0;
                                }
                                if( relu )
                                {
                                    s00 = s00 > 0.f ? s00 : s00*r0;
                                    s10 = s10 > 0.f ? s10 : s10*r1;
                                }

                                outptr0[j] = s00;
                                outptr1[j] = s10;
                            }
                        }
                    }
                }

                if( activ_ )
                    activ_->forwardSlice(data_out0 + stripeStart, data_out0 + stripeStart,
                                         (int)(stripeEnd - stripeStart),
                                         outPlaneSize, startOutCn, startOutCn + outCn);
            }
        }
    };

#ifdef HAVE_OPENCL
    bool forward_ocl(InputArrayOfArrays inps, OutputArrayOfArrays outs, OutputArrayOfArrays internals)
    {
        std::vector<UMat> inputs;
        std::vector<UMat> outputs;

        bool use_half = (inps.depth() == CV_16S);
        inps.getUMatVector(inputs);
        outs.getUMatVector(outputs);

        CV_Assert(outputs.size() == 1);
        for (int i = 0; i < inputs.size(); ++i)
            CV_Assert(inputs[i].u != outputs[0].u);

        if (umat_blobs.empty())
        {
            size_t n = blobs.size();
            umat_blobs.resize(n);
            for (size_t i = 0; i < n; i++)
            {
                blobs[i].copyTo(umat_blobs[i]);
            }
        }

        if (convolutionOp.empty())
        {
            OCL4DNNConvConfig config;
            config.in_shape = shape(inputs[0]);
            config.out_shape = shape(outputs[0]);
            config.kernel = kernel;
            config.pad = pad;
            config.stride = stride;
            config.dilation = dilation;
            config.group = inputs[0].size[1] / umat_blobs[0].size[1];
            config.bias_term = (hasBias()) ? true : false;
            config.use_half = use_half;

            convolutionOp = Ptr<OCL4DNNConvSpatial<float> >(new OCL4DNNConvSpatial<float>(config));
        }

        int outCn = umat_blobs[0].size[0];

        reluslope.clear();
        if( activ )
        {
            Ptr<ReLULayer> activ_relu = activ.dynamicCast<ReLULayer>();
            if( !activ_relu.empty() )
            {
                reluslope.assign(outCn+2, activ_relu->negativeSlope);
                activType = OCL4DNN_CONV_FUSED_ACTIV_RELU;
            }

            Ptr<ReLU6Layer> activ_relu6 = activ.dynamicCast<ReLU6Layer>();
            if( !activ_relu6.empty() )
            {
                reluslope.resize(2);
                reluslope[0] = activ_relu6->minValue;
                reluslope[1] = activ_relu6->maxValue;
                activType = OCL4DNN_CONV_FUSED_ACTIV_RELU6;
            }

            Ptr<ChannelsPReLULayer> activ_chprelu = activ.dynamicCast<ChannelsPReLULayer>();
            if( !activ_chprelu.empty() )
            {
                const Mat& m = activ_chprelu->blobs[0];
                CV_Assert(m.isContinuous() && m.type() == CV_32F && (int)m.total() == outCn);
                const float* mdata = m.ptr<float>();
                reluslope.resize(outCn+2);
                std::copy(mdata, mdata + outCn, reluslope.begin());
                reluslope[outCn] = reluslope[outCn+1] = reluslope[outCn-1];
                activType = OCL4DNN_CONV_FUSED_ACTIV_PRELU;
            }
        }

        if (fusedWeights)
        {
            weightsMat.copyTo(umat_blobs[0]);
            fusedWeights = false;
        }
        if (fusedBias)
        {
            if ( umat_blobs.size() < 2 )
                umat_blobs.resize(2);
            umat_blobs[1] = UMat(biasvec, true);
            convolutionOp->setBias(true);
            fusedBias = false;
        }

        if ( newActiv )
        {
            if ( activType == OCL4DNN_CONV_FUSED_ACTIV_RELU )
            {
                CV_Assert(!reluslope.empty());
                convolutionOp->setActivReLU(true, reluslope[0]);
            }
            else if ( activType == OCL4DNN_CONV_FUSED_ACTIV_PRELU)
            {
                CV_Assert(!reluslope.empty());
                convolutionOp->setActivPReLU(true, reluslope);
            }
            else if ( activType == OCL4DNN_CONV_FUSED_ACTIV_POWER)
            {
                convolutionOp->setActivPower(true, power);
            }
            else if ( activType == OCL4DNN_CONV_FUSED_ACTIV_TANH)
            {
                convolutionOp->setActivTanh(true);
            }
            else if ( activType == OCL4DNN_CONV_FUSED_ACTIV_RELU6)
            {
                convolutionOp->setActivReLU6(true, reluslope[0], reluslope[1]);
            }
            else
            {
                convolutionOp->setActivReLU(false, 0);
                convolutionOp->setActivPReLU(false, reluslope);
                convolutionOp->setActivPower(false, 1.f);
                convolutionOp->setActivTanh(false);
                convolutionOp->setActivReLU6(false, 0, 0);
            }
            newActiv = false;
        }

        UMat& inpMat = inputs[0];
        UMat& outMat = outputs[0];
        int batch_size = inpMat.size[0];

        return convolutionOp->Forward(inpMat,
                                      inputs.size() == 2 ? inputs[1] : UMat(),
                                      umat_blobs[0],
                                      umat_blobs.size() > 1 ? umat_blobs[1] : UMat(),
                                      outMat,
                                      batch_size);
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

        std::vector<Mat> inputs, outputs;
        inputs_arr.getMatVector(inputs);
        outputs_arr.getMatVector(outputs);

        /*printf("conv %s: input (%d x %d x %d x %d), kernel (%d x %d), pad (%d x %d), stride (%d x %d), dilation (%d x %d)\n",
               name.c_str(), inputs[0].size[0], inputs[0].size[1], inputs[0].size[2], inputs[0].size[3],
               kernel.width, kernel.height, pad.width, pad.height,
               stride.width, stride.height, dilation.width, dilation.height);*/
        CV_Assert_N(inputs.size() == (size_t)1, inputs[0].size[1] % blobs[0].size[1] == 0,
                    outputs.size() == 1, inputs[0].data != outputs[0].data);

        int ngroups = inputs[0].size[1]/blobs[0].size[1];
        CV_Assert(outputs[0].size[1] % ngroups == 0);
        int outCn = blobs[0].size[0];

        reluslope.clear();
        if( activ )
        {
            Ptr<ReLULayer> activ_relu = activ.dynamicCast<ReLULayer>();
            if( !activ_relu.empty() )
            {
                reluslope.assign(outCn+2, activ_relu->negativeSlope);
            }

            Ptr<ChannelsPReLULayer> activ_chprelu = activ.dynamicCast<ChannelsPReLULayer>();
            if( !activ_chprelu.empty() )
            {
                const Mat& m = activ_chprelu->blobs[0];
                CV_Assert(m.isContinuous() && m.type() == CV_32F && (int)m.total() == outCn);
                const float* mdata = m.ptr<float>();
                reluslope.resize(outCn+2);
                std::copy(mdata, mdata + outCn, reluslope.begin());
                reluslope[outCn] = reluslope[outCn+1] = reluslope[outCn-1];
            }
        }

        int nstripes = std::max(getNumThreads(), 1);

        ParallelConv::run(inputs[0], outputs[0], weightsMat, biasvec, reluslope,
                          kernel_size, strides, pads_begin, pads_end, dilations, activ.get(), ngroups, nstripes);
    }

    virtual int64 getFLOPS(const std::vector<MatShape> &inputs,
                           const std::vector<MatShape> &outputs) const CV_OVERRIDE
    {
        CV_Assert(inputs.size() == outputs.size());

        int64 flops = 0;
        int karea = std::accumulate(kernel_size.begin(), kernel_size.end(), 1, std::multiplies<size_t>());
        for (int i = 0; i < inputs.size(); i++)
        {
            flops += total(outputs[i])*(CV_BIG_INT(2)*karea*inputs[i][1] + 1);
        }

        return flops;
    }
};

class DeConvolutionLayerImpl CV_FINAL : public BaseConvolutionLayerImpl
{
public:
    Mat weightsMat, biasesMat;
    UMat umat_weights;
    UMat umat_biases;

    DeConvolutionLayerImpl(const LayerParams& params) : BaseConvolutionLayerImpl(params) {}

    MatShape computeColRowShape(const MatShape &inpShape, const MatShape &outShape) const CV_OVERRIDE
    {
        int inpCn = inpShape[1];
        int inpH = inpShape[2];
        int inpW = inpShape[3];
        int outCn = outShape[1];
        int ngroups = inpCn / blobs[0].size[0];
        int outGroupCn = outCn / ngroups;
        int ksize = outGroupCn * kernel.height * kernel.width;
        return shape(ksize, inpH * inpW);
    }

    virtual bool supportBackend(int backendId) CV_OVERRIDE
    {
#ifdef HAVE_INF_ENGINE
        const int outGroupCn = blobs[0].size[1];  // Weights are in IOHW or IODHW layout
        const int group = numOutput / outGroupCn;

        if (backendId == DNN_BACKEND_INFERENCE_ENGINE)
        {
            if (kernel_size.size() == 3 && preferableTarget != DNN_TARGET_CPU) {
                return false;
            }

            if (std::accumulate(adjust_pads.begin(), adjust_pads.end(), 0, std::plus<size_t>()) > 0)
            {
                if (padMode.empty())
                {
                    if (preferableTarget != DNN_TARGET_CPU && group != 1)
                    {
                        for (int i = 0; i < adjust_pads.size(); i++) {
                            if (adjust_pads[i] && pads_begin[i])
                                return false;
                        }
                    }
                    for (int i = 0; i < adjust_pads.size(); i++) {
                        if (pads_end[i] < adjust_pads[i])
                            return false;
                    }
                    return true;
                }
                else if (padMode == "SAME")
                {
                    for (int i = 0; i < adjust_pads.size(); i++) {
                        if (kernel_size[i] < pads_begin[i] + 1 + adjust_pads[i])
                            return false;
                    }
                    return true;
                }
                else if (padMode == "VALID")
                    return false;
            }

            if (group != 1)
            {
                return preferableTarget == DNN_TARGET_CPU;
            }
            if (preferableTarget == DNN_TARGET_OPENCL || preferableTarget == DNN_TARGET_OPENCL_FP16)
                return std::accumulate(dilations.begin(), dilations.end(), 1, std::multiplies<size_t>()) == 1;
            return true;
        }
        else
#endif  // HAVE_INF_ENGINE
            return kernel_size.size() == 2 && (backendId == DNN_BACKEND_OPENCV || backendId == DNN_BACKEND_HALIDE);
    }

    bool getMemoryShapes(const std::vector<MatShape> &inputs,
                         const int requiredOutputs,
                         std::vector<MatShape> &outputs,
                         std::vector<MatShape> &internals) const CV_OVERRIDE
    {
        CV_Assert(!hasBias() || blobs[1].total() == (size_t)numOutput);
        CV_Assert(inputs.size() != 0);

        int outCn = numOutput;
        std::vector<int> outShape;
        outShape.push_back(inputs[0][0]);  // batch
        outShape.push_back(outCn);
        if (padMode.empty())
        {
            for (int i = 0; i < kernel_size.size(); i++)
                outShape.push_back(strides[i] * (inputs[0][2 + i] - 1) + kernel_size[i] - pads_begin[i] - pads_end[i] + adjust_pads[i]);
        }
        else if (padMode == "VALID")
        {
            for (int i = 0; i < kernel_size.size(); i++)
                outShape.push_back(strides[i] * (inputs[0][2 + i] - 1) + kernel_size[i] + adjust_pads[i]);
        }
        else if (padMode == "SAME")
        {
            for (int i = 0; i < kernel_size.size(); i++)
                outShape.push_back(strides[i] * (inputs[0][2 + i] - 1) + 1 + adjust_pads[i]);
        }
        else
            CV_Error(Error::StsError, "Unsupported padding mode " + padMode);

        CV_Assert(outCn % blobs[0].size[1] == 0);
        int ngroups = outCn / blobs[0].size[1];

        int inpCn = inputs[0][1];
        CV_Assert(inpCn % ngroups == 0 && outCn % ngroups == 0);
        CV_Assert(blobs[0].size[0] == inpCn);

        outputs.resize(1, outShape);

        if (!is1x1())
            internals.push_back(computeColRowShape(inputs[0], outputs[0]));

        return false;
    }

    void finalize(InputArrayOfArrays inputs_arr, OutputArrayOfArrays outputs_arr) CV_OVERRIDE
    {
        BaseConvolutionLayerImpl::finalize(inputs_arr, outputs_arr);

        std::vector<Mat> inputs, outputs;
        inputs_arr.getMatVector(inputs);
        outputs_arr.getMatVector(outputs);

        std::vector<int> inpShape;
        std::vector<int> outShape;
        for (int i = 2; i < inputs[0].dims; i++) {
            inpShape.push_back(inputs[0].size[i]);
            outShape.push_back(outputs[0].size[i]);
        }
        getConvPoolPaddings(outShape, kernel_size, strides, padMode, pads_begin, pads_end);
        if (pads_begin.size() == 2) {
            for (int i = 0; i < pads_begin.size(); i++) {
                if (pads_begin[i] != pads_end[i])
                    CV_Error(Error::StsNotImplemented, "Unsupported asymmetric padding in deconvolution layer");
            }
            pad = Size(pads_begin[1], pads_begin[0]);
        }

        weightsMultipliers.assign(numOutput, 1.0);
        if (weightsMat.empty())
        {
            transpose(blobs[0].reshape(1, blobs[0].size[0]), weightsMat);
            biasesMat = hasBias() ? blobs[1].reshape(1, numOutput)
                                  : Mat::zeros(numOutput, 1, CV_32F);
        }
    }

    void fuseWeights(const Mat& w_, const Mat& b_) CV_OVERRIDE
    {
        Mat w = w_.total() == 1 ? Mat(1, numOutput, CV_32F, Scalar(w_.at<float>(0))) : w_;
        Mat b = b_.total() == 1 ? Mat(1, numOutput, CV_32F, Scalar(b_.at<float>(0))) : b_;

        CV_Assert_N(!weightsMat.empty(),
                     w.empty() || numOutput == w.total(),
                     b.empty() || numOutput == b.total());

        if (!w.empty())
        {
            transpose(blobs[0].reshape(1, blobs[0].size[0]), weightsMat);
            weightsMat = weightsMat.reshape(1, numOutput);
            for (int i = 0; i < numOutput; ++i)
            {
                double wi = w.at<float>(i);
                weightsMultipliers[i] *= wi;
                cv::multiply(weightsMat.row(i), weightsMultipliers[i], weightsMat.row(i));
                biasesMat.at<float>(i) *= wi;
            }
            weightsMat = weightsMat.reshape(1, weightsMat.total() / blobs[0].size[0]);
        }

        if (!b.empty())
        {
            cv::add(biasesMat, b.reshape(1, numOutput), biasesMat);
        }
    }

    class MatMulInvoker : public ParallelLoopBody
    {
    public:
        MatMulInvoker(const Mat& a, const Mat& b, Mat& c, int nstripes)
        {
            a_ = &a;
            b_ = &b;
            c_ = &c;
            nstripes_ = nstripes;
            useAVX = checkHardwareSupport(CPU_AVX);
            useAVX2 = checkHardwareSupport(CPU_AVX2);
            useAVX512 = CV_CPU_HAS_SUPPORT_AVX512_SKX;
        }

        void operator()(const Range& range_) const CV_OVERRIDE
        {
            int stripeSize = (int)alignSize((b_->cols + nstripes_ - 1)/nstripes_, 16);
            Range range(range_.start*stripeSize, std::min(range_.end*stripeSize, b_->cols));
            int mmax = a_->rows;
            int nmax = range.end - range.start;
            int kmax = a_->cols;
            int m, n, k;
            const float* aptr = a_->ptr<float>();
            const float* bptr = b_->ptr<float>() + range.start;
            float* cptr = c_->ptr<float>() + range.start;
            size_t astep = a_->step1();
            size_t bstep = b_->step1();
            size_t cstep = c_->step1();

        #if CV_TRY_AVX512_SKX
            if( useAVX512 )
                opt_AVX512_SKX::fastGEMM( aptr, astep, bptr, bstep, cptr, cstep, mmax, kmax, nmax );
            else
        #endif
        #if CV_TRY_AVX2
            if( useAVX2 )
                opt_AVX2::fastGEMM( aptr, astep, bptr, bstep, cptr, cstep, mmax, kmax, nmax );
            else
        #endif
        #if CV_TRY_AVX
            if( useAVX )
                opt_AVX::fastGEMM( aptr, astep, bptr, bstep, cptr, cstep, mmax, kmax, nmax );
            else
        #endif
            for( m = 0; m < mmax; m += 2 )
            {
                float* dst0 = cptr + cstep*m;
                float* dst1 = cptr + cstep*std::min(m+1, mmax-1);
                const float* aptr0 = aptr + astep*m;
                const float* aptr1 = aptr + astep*std::min(m+1, mmax-1);

                for( n = 0; n < nmax; n++ )
                {
                    dst0[n] = 0.f;
                    dst1[n] = 0.f;
                }

                for( k = 0; k < kmax; k += 4 )
                {
                    float alpha00 = aptr0[k];
                    float alpha01 = aptr1[k];
                    float alpha10 = 0.f, alpha11 = 0.f;
                    float alpha20 = 0.f, alpha21 = 0.f;
                    float alpha30 = 0.f, alpha31 = 0.f;
                    const float* bptr0 = bptr + k*bstep;
                    const float* bptr1 = bptr0;
                    const float* bptr2 = bptr0;
                    const float* bptr3 = bptr0;

                    if( k+1 < kmax )
                    {
                        alpha10 = aptr0[k+1];
                        alpha11 = aptr1[k+1];
                        bptr1 = bptr0 + bstep;
                        if( k+2 < kmax )
                        {
                            alpha20 = aptr0[k+2];
                            alpha21 = aptr1[k+2];
                            bptr2 = bptr1 + bstep;
                            if( k+3 < kmax )
                            {
                                alpha30 = aptr0[k+3];
                                alpha31 = aptr1[k+3];
                                bptr3 = bptr2 + bstep;
                            }
                        }
                    }
                    n = 0;

                #if CV_SIMD128
                    v_float32x4 a00 = v_setall_f32(alpha00);
                    v_float32x4 a01 = v_setall_f32(alpha01);
                    v_float32x4 a10 = v_setall_f32(alpha10);
                    v_float32x4 a11 = v_setall_f32(alpha11);
                    v_float32x4 a20 = v_setall_f32(alpha20);
                    v_float32x4 a21 = v_setall_f32(alpha21);
                    v_float32x4 a30 = v_setall_f32(alpha30);
                    v_float32x4 a31 = v_setall_f32(alpha31);

                    for( ; n <= nmax - 4; n += 4 )
                    {
                        v_float32x4 b0 = v_load(bptr0 + n);
                        v_float32x4 b1 = v_load(bptr1 + n);
                        v_float32x4 b2 = v_load(bptr2 + n);
                        v_float32x4 b3 = v_load(bptr3 + n);
                        v_float32x4 d0 = v_load(dst0 + n);
                        v_float32x4 d1 = v_load(dst1 + n);
                        d0 += b0*a00;
                        d1 += b0*a01;
                        d0 += b1*a10;
                        d1 += b1*a11;
                        d0 += b2*a20;
                        d1 += b2*a21;
                        d0 += b3*a30;
                        d1 += b3*a31;
                        v_store(dst0 + n, d0);
                        v_store(dst1 + n, d1);
                    }
                #endif

                    for( ; n < nmax; n++ )
                    {
                        float b0 = bptr0[n], b1 = bptr1[n];
                        float b2 = bptr2[n], b3 = bptr3[n];
                        float d0 = dst0[n] + alpha00*b0 + alpha10*b1 + alpha20*b2 + alpha30*b3;
                        float d1 = dst1[n] + alpha01*b0 + alpha11*b1 + alpha21*b2 + alpha31*b3;
                        dst0[n] = d0;
                        dst1[n] = d1;
                    }
                }
            }
        }

        const Mat *a_, *b_;
        Mat* c_;
        int nstripes_;
        bool useAVX;
        bool useAVX2;
        bool useAVX512;
    };

    class Col2ImInvoker : public cv::ParallelLoopBody
    {
    public:
        const float* data_col;
        const float* biasvec;
        int channels, height, width;
        int kernel_h, kernel_w;
        int pad_h, pad_w;
        int stride_h, stride_w;
        float* data_im;
        int height_col, width_col;
        int nstripes;
        bool is1x1;

        Col2ImInvoker()
            : data_col(0), biasvec(0), channels(0), height(0), width(0),
              kernel_h(0), kernel_w(0), pad_h(0), pad_w(0), stride_h(0), stride_w(0), data_im(0),
              height_col(0), width_col(0), nstripes(0), is1x1(0)
        {}

        static void run(const float* data_col,
                        int channels, int height, int width,
                        int kernel_h, int kernel_w,
                        int pad_h, int pad_w,
                        int stride_h, int stride_w,
                        int height_col, int width_col,
                        float* data_im,
                        const float* biasvec,
                        bool is1x1)
        {
            const int nstripes = getNumThreads();

            Col2ImInvoker t;
            t.data_col = data_col;
            t.data_im = data_im;
            t.channels = channels; t.height = height; t.width = width;
            t.kernel_h = kernel_h; t.kernel_w = kernel_w;
            t.pad_h = pad_h; t.pad_w = pad_w;
            t.stride_h = stride_h; t.stride_w = stride_w;
            t.height_col = height_col;
            t.width_col = width_col;
            t.nstripes = nstripes;
            t.is1x1 = is1x1;
            t.biasvec = biasvec;

            parallel_for_(Range(0, nstripes), t, nstripes);
        }

        virtual void operator ()(const Range &r) const CV_OVERRIDE
        {
            const float* data_col_ = data_col;
            float* data_im_ = data_im;
            int coeff_h = (1 - stride_h * kernel_w * height_col) * width_col;
            int coeff_w = (1 - stride_w * height_col * width_col);
            size_t total = (size_t)channels * height * width;
            size_t stripeSize = (total + nstripes - 1)/nstripes;
            size_t startIndex = r.start*stripeSize;
            size_t endIndex = std::min(r.end*stripeSize, total);
            int w = (int)(startIndex % width + pad_w);
            int h = (int)((startIndex / width) % height + pad_h);
            int c = (int)(startIndex / (width * height));
            int h_col_start = (h < kernel_h) ? 0 : (h - kernel_h) / stride_h + 1;
            int h_col_end = std::min(h / stride_h + 1, height_col);
            int plane_size_col = height_col * width_col;
            int offset = (c * kernel_h * kernel_w + h * kernel_w + w) * plane_size_col;
            bool is1x1_ = is1x1;
            const float* biasvec_ = biasvec;

            for (size_t index = startIndex; index < endIndex; index++)
            {
                // compute the start and end of the output
                int w_col_start = (w < kernel_w) ? 0 : (w - kernel_w) / stride_w + 1;
                int w_col_end = std::min(w / stride_w + 1, width_col);
                float val;

                if( is1x1_ )
                    val = data_im_[index];
                else
                {
                    val = 0.f;
                    for (int h_col = h_col_start; h_col < h_col_end; ++h_col) {
                        for (int w_col = w_col_start; w_col < w_col_end; ++w_col) {
                            val += data_col_[offset + h_col * coeff_h + w_col * coeff_w];
                        }
                    }
                }
                data_im_[index] = val + biasvec_[c];

                offset += plane_size_col;
                if( ++w >= width + pad_w )
                {
                    w = (int)((index + 1)% width + pad_w);
                    h = (int)(((index + 1) / width) % height + pad_h);
                    c = (int)((index + 1) / (width * height));
                    h_col_start = (h < kernel_h) ? 0 : (h - kernel_h) / stride_h + 1;
                    h_col_end = std::min(h / stride_h + 1, height_col);
                    offset = (c * kernel_h * kernel_w + h * kernel_w + w) * plane_size_col;
                }
            }
        }
    };

#ifdef HAVE_OPENCL
    bool forward_ocl(InputArrayOfArrays inputs_, OutputArrayOfArrays outputs_, OutputArrayOfArrays internals_)
    {
        std::vector<UMat> inputs;
        std::vector<UMat> outputs;
        std::vector<UMat> internals;

        if (inputs_.depth() == CV_16S)
            return false;

        inputs_.getUMatVector(inputs);
        outputs_.getUMatVector(outputs);
        internals_.getUMatVector(internals);

        int outCn = numOutput;
        int inpCn = inputs[0].size[1];

        if (is1x1())
            return false;

        if (umat_weights.empty())
        {
            if (fusedWeights)
                weightsMat.copyTo(umat_weights);
            else
                transpose(blobs[0].reshape(1, inpCn), umat_weights);

            if (fusedBias)
                biasesMat.copyTo(umat_biases);
            else
            {
                if (hasBias())
                    blobs[1].reshape(1, outCn).copyTo(umat_biases);
                else
                    umat_biases = UMat::zeros(outCn, 1, CV_32F);
            }
        }

        String buildopt = format("-DT=%s ", ocl::typeToStr(inputs[0].type()));
        buildopt += format("-DPAD_H=%d -DPAD_W=%d -DKERNEL_H=%d -DKERNEL_W=%d -DSTRIDE_H=%d -DSTRIDE_W=%d ",
                           pad.height, pad.width, kernel.height, kernel.width, stride.height, stride.width);

        for (size_t ii = 0; ii < outputs.size(); ii++)
        {
            int ngroups = outCn / blobs[0].size[1];
            int inpGroupCn = inpCn / ngroups;
            int outGroupCn = blobs[0].size[1];
            const UMat& inp = inputs[ii];
            UMat& out = outputs[ii];
            int numImg = inp.size[0];
            int inpH = inp.size[2], inpW = inp.size[3];
            int outH = out.size[2], outW = out.size[3];

            MatShape inpshape = shape(numImg*inpCn, inpH*inpW);
            MatShape outshape = shape(numImg*outCn, outH*outW);
            UMat convBlob = inputs[ii].reshape(1, inpshape.size(), &inpshape[0]);
            UMat decnBlob = out.reshape(1, outshape.size(), &outshape[0]);
            int rows = internals[0].rows / ngroups;

            for (int n = 0; n < numImg; n++)
            {
                for (int g = 0; g < ngroups; g++)
                {
                    UMat colMat = internals[0].rowRange(_Range(g * rows, rows));
                    UMat convMat = convBlob.rowRange(_Range((g + n * ngroups) * inpGroupCn, inpGroupCn));
                    UMat wghtMat = umat_weights.colRange(_Range(g * inpGroupCn, inpGroupCn));
                    gemm(wghtMat, convMat, 1, noArray(), 0, colMat, 0);
                }

                for (int g = 0; g < ngroups; g++)
                {
                    int total = outGroupCn * decnBlob.cols;
                    int index = 0;
                    int height_col = inpH;
                    int width_col = inpW;
                    int coeff_h = (1 - stride.height * kernel.width * height_col) * width_col;
                    int coeff_w = (1 - stride.width * height_col * width_col);

                    ocl::Kernel k("col2im", ocl::dnn::col2im_oclsrc, buildopt);
                    k.set(index++, total);
                    k.set(index++, ocl::KernelArg::PtrReadOnly(internals[0]));
                    k.set(index++, (int)(g * rows * internals[0].cols));
                    k.set(index++, outGroupCn);
                    k.set(index++, outH);
                    k.set(index++, outW);
                    k.set(index++, height_col);
                    k.set(index++, width_col);
                    k.set(index++, coeff_h);
                    k.set(index++, coeff_w);
                    k.set(index++, ocl::KernelArg::PtrReadOnly(umat_biases));
                    k.set(index++, (int)(g * outGroupCn * umat_biases.cols));
                    k.set(index++, ocl::KernelArg::PtrWriteOnly(decnBlob));
                    k.set(index++, (int)((g + n * ngroups) * outGroupCn * decnBlob.cols));

                    size_t global[] = { (size_t)total };
                    bool ret = k.run(1, global, NULL, false);
                    if (!ret)
                        return false;
                }
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
                   forward_ocl(inputs_arr, outputs_arr, internals_arr));

        if (inputs_arr.depth() == CV_16S)
        {
            forward_fallback(inputs_arr, outputs_arr, internals_arr);
            return;
        }

        std::vector<Mat> inputs, outputs, internals;
        inputs_arr.getMatVector(inputs);
        outputs_arr.getMatVector(outputs);
        internals_arr.getMatVector(internals);

        int outCn = numOutput;
        int inpCn = inputs[0].size[1];
        bool is1x1flag = is1x1();
        int nstripes = getNumThreads();

        if( weightsMat.empty() )
        {
            transpose(blobs[0].reshape(1, inpCn), weightsMat);
            biasesMat = hasBias() ? blobs[1].reshape(1, outCn) : Mat::zeros(outCn, 1, CV_32F);
        }

        for (size_t ii = 0; ii < outputs.size(); ii++)
        {
            int ngroups = outCn / blobs[0].size[1];
            int inpGroupCn = inpCn / ngroups;
            int outGroupCn = blobs[0].size[1];
            const Mat& inp = inputs[ii];
            Mat& out = outputs[ii];
            int numImg = inp.size[0];
            int inpH = inp.size[2], inpW = inp.size[3];
            int outH = out.size[2], outW = out.size[3];

            Mat convBlob = inputs[ii].reshape(1, numImg*inpCn);
            Mat decnBlob = out.reshape(1, numImg*outCn);

            for (int n = 0; n < numImg; n++)
            {
                for (int g = 0; g < ngroups; g++)
                {
                    Mat dstMat = decnBlob.rowRange(_Range((g + n * ngroups) * outGroupCn, outGroupCn));
                    Mat &colMat = is1x1flag ? dstMat : internals[0];

                    Mat convMat = convBlob.rowRange(_Range((g + n * ngroups) * inpGroupCn, inpGroupCn));
                    Mat wghtMat = weightsMat.colRange(_Range(g * inpGroupCn, inpGroupCn));
                    Mat curBiasMat = biasesMat.rowRange(_Range(g * outGroupCn, outGroupCn));

                    //gemm(wghtMat, convMat, 1, colMat, 0, colMat, 0);
                    MatMulInvoker mminvoker(wghtMat, convMat, colMat, nstripes);
                    parallel_for_(Range(0, nstripes), mminvoker, nstripes);

                    Col2ImInvoker::run(colMat.ptr<float>(), outGroupCn, outH, outW,
                                       kernel.height, kernel.width, pad.height, pad.width,
                                       stride.height, stride.width, inpH, inpW, dstMat.ptr<float>(),
                                       curBiasMat.ptr<float>(), is1x1flag);
                }
            }
        }
    }

    virtual Ptr<BackendNode> initHalide(const std::vector<Ptr<BackendWrapper> > &inputs) CV_OVERRIDE
    {
#ifdef HAVE_HALIDE
        Halide::Buffer<float> inputBuffer = halideBuffer(inputs[0]);

        int inW, inH, inC, inN;
        getCanonicalSize(inputBuffer, &inW, &inH, &inC, &inN);
        const int outGroupCn = blobs[0].size[1];
        const int group = numOutput / outGroupCn;
        const int inpGroupCn = blobs[0].size[0] / group;

        Halide::Var x("x"), y("y"), c("c"), n("n");
        Halide::Func top = (name.empty() ? Halide::Func() : Halide::Func(name));
        Halide::Func padded_input(name + "_constant_exterior");
        auto weights = wrapToHalideBuffer(blobs[0]);

        Halide::Func dilated_input("dilated_input");
        dilated_input(x, y, c, n) = 0.0f;
        Halide::RDom r1(0, inW, 0, inH);
        dilated_input(r1.x * stride.width, r1.y * stride.height, c, n) =
              inputBuffer(r1.x, r1.y, c, n);
        dilated_input.compute_root();

        Halide::Func bounded =
            Halide::BoundaryConditions::constant_exterior(dilated_input, 0,
                                                          0, (inW - 1) * stride.width + 1,
                                                          0, (inH - 1) * stride.height + 1,
                                                          0, inC, 0, inN);
        padded_input(x, y, c, n) = bounded(x, y, c, n);

        Halide::RDom r(0, kernel.width, 0, kernel.height, 0, inpGroupCn);
        Halide::Expr kx = x + pad.width - r.x;
        Halide::Expr ky = y + pad.height - r.y;
        Halide::Expr kInC = r.z;
        Halide::Expr kOutC = c;
        for (int i = 1; i < group; ++i)
        {
            kInC = select(c < outGroupCn * i, kInC, inpGroupCn * i + r.z);
            kOutC = select(c < outGroupCn * i, kOutC, c - outGroupCn * i);
        }
        Halide::Expr topExpr = sum(padded_input(kx, ky, kInC, n) *
                                   weights(r.x, r.y, kOutC, kInC));
        if (hasBias())
        {
            auto bias = wrapToHalideBuffer(blobs[1], {numOutput});
            topExpr += bias(c);
        }
        top(x, y, c, n) = topExpr;
        return Ptr<BackendNode>(new HalideBackendNode({ padded_input, top }));
#endif  // HAVE_HALIDE
        return Ptr<BackendNode>();
    }

#ifdef HAVE_INF_ENGINE
    virtual Ptr<BackendNode> initInfEngine(const std::vector<Ptr<BackendWrapper> > &) CV_OVERRIDE
    {
        InferenceEngine::Layout layout = blobs[0].dims == 5? InferenceEngine::Layout::NCDHW :
                                                             InferenceEngine::Layout::OIHW;

        auto ieWeights = wrapToInfEngineBlob(blobs[0], layout);
        if (fusedWeights)
        {
            ieWeights = InferenceEngine::make_shared_blob<float>(
                                InferenceEngine::Precision::FP32, layout,
                                ieWeights->dims());
            ieWeights->allocate();

            int inpCn = blobs[0].size[0];
            Mat newWeights = infEngineBlobToMat(ieWeights).reshape(1, inpCn);
            transpose(weightsMat, newWeights);
        }

        const int outGroupCn = blobs[0].size[1];  // Weights are in IOHW or OIDHW layout
        const int group = numOutput / outGroupCn;

        InferenceEngine::Builder::DeconvolutionLayer ieLayer(name);

        ieLayer.setKernel(kernel_size);
        ieLayer.setStrides(strides);
        ieLayer.setDilation(dilations);
        ieLayer.setPaddingsBegin(pads_begin);

        if (padMode.empty())
        {
            std::vector<size_t> paddings_end;
            for (int i = 0; i < pads_end.size(); i++) {
                paddings_end.push_back(pads_end[i] - adjust_pads[i]);
            }
            ieLayer.setPaddingsEnd(paddings_end);
        }
        else if (padMode == "SAME")
        {
            std::vector<size_t> paddings_end;
            for (int i = 0; i < pads_begin.size(); i++) {
                paddings_end.push_back(kernel_size[i] - pads_begin[i] - 1 - adjust_pads[i]);
            }
            ieLayer.setPaddingsEnd(paddings_end);
        }
        ieLayer.setGroup((size_t)group);
        ieLayer.setOutDepth((size_t)numOutput);

        InferenceEngine::Builder::Layer l = ieLayer;
        addConstantData("weights", ieWeights, l);
        if (hasBias())
            addConstantData("biases", wrapToInfEngineBlob(biasesMat, {(size_t)numOutput}, InferenceEngine::Layout::C), l);
        return Ptr<BackendNode>(new InfEngineBackendNode(l));
    }
#endif  // HAVE_INF_ENGINE

    virtual int64 getFLOPS(const std::vector<MatShape> &inputs,
                           const std::vector<MatShape> &outputs) const CV_OVERRIDE
    {
        CV_Assert(inputs.size() == outputs.size());

        float flops = 0;
        int outChannels = blobs[0].size[0];
        size_t karea = std::accumulate(kernel_size.begin(), kernel_size.end(),
                                       1, std::multiplies<size_t>());

        for (int i = 0; i < inputs.size(); i++)
        {
            flops += CV_BIG_INT(2)*outChannels*karea*total(inputs[i]);
        }

        return flops;
    }
};

Ptr<BaseConvolutionLayer> ConvolutionLayer::create(const LayerParams &params)
{
    Ptr<ConvolutionLayerImpl> l(new ConvolutionLayerImpl(params));
    return l;
}

Ptr<BaseConvolutionLayer> DeconvolutionLayer::create(const LayerParams &params)
{
    return Ptr<BaseConvolutionLayer>(new DeConvolutionLayerImpl(params));
}

}
}
