// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

// Copyright (C) 2018-2019, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.


#include "test_precomp.hpp"
#include "npy_blob.hpp"
#include <opencv2/dnn/shape_utils.hpp>
namespace opencv_test { namespace {

template<typename TString>
static std::string _tf(TString filename, bool required = true)
{
    return findDataFile(std::string("dnn/onnx/") + filename, required);
}

class Test_ONNX_layers : public DNNTestLayer
{
public:
    bool required;

    Test_ONNX_layers() : required(true) { }

    enum Extension
    {
        npy,
        pb
    };

    void testONNXModels(const String& basename, const Extension ext = npy,
                        const double l1 = 0, const float lInf = 0, const bool useSoftmax = false,
                        bool checkNoFallbacks = true)
    {
        String onnxmodel = _tf("models/" + basename + ".onnx", required);
        Mat inp, ref;
        if (ext == npy) {
            inp = blobFromNPY(_tf("data/input_" + basename + ".npy"));
            ref = blobFromNPY(_tf("data/output_" + basename + ".npy"));
        }
        else if (ext == pb) {
            inp = readTensorFromONNX(_tf("data/input_" + basename + ".pb"));
            ref = readTensorFromONNX(_tf("data/output_" + basename + ".pb"));
        }
        else
            CV_Error(Error::StsUnsupportedFormat, "Unsupported extension");

        checkBackend(&inp, &ref);
        Net net = readNetFromONNX(onnxmodel);
        ASSERT_FALSE(net.empty());

        net.setPreferableBackend(backend);
        net.setPreferableTarget(target);

        net.setInput(inp);
        Mat out = net.forward("");

        if (useSoftmax)
        {
            LayerParams lp;
            Net netSoftmax;
            netSoftmax.addLayerToPrev("softmaxLayer", "SoftMax", lp);
            netSoftmax.setPreferableBackend(DNN_BACKEND_OPENCV);

            netSoftmax.setInput(out);
            out = netSoftmax.forward();

            netSoftmax.setInput(ref);
            ref = netSoftmax.forward();
        }
        normAssert(ref, out, "", l1 ? l1 : default_l1, lInf ? lInf : default_lInf);
        if (checkNoFallbacks)
            expectNoFallbacksFromIE(net);
    }
};

TEST_P(Test_ONNX_layers, InstanceNorm)
{
    if (target == DNN_TARGET_MYRIAD)
        testONNXModels("instancenorm", npy, 0, 0, false, false);
    else
        testONNXModels("instancenorm", npy);
}

TEST_P(Test_ONNX_layers, MaxPooling)
{
    testONNXModels("maxpooling");
    testONNXModels("two_maxpooling");
}

TEST_P(Test_ONNX_layers, Convolution)
{
    testONNXModels("convolution");
}

TEST_P(Test_ONNX_layers, Convolution3D)
{
#if defined(INF_ENGINE_RELEASE) && INF_ENGINE_VER_MAJOR_LT(2019010000)
    throw SkipTestException("Test is enabled starts from 2019R1");
#endif
    if (target != DNN_TARGET_CPU)
        throw SkipTestException("Only CPU is supported");
    testONNXModels("conv3d");
    testONNXModels("conv3d_bias");
}

TEST_P(Test_ONNX_layers, Two_convolution)
{
#if defined(INF_ENGINE_RELEASE)
    if (backend == DNN_BACKEND_INFERENCE_ENGINE && target == DNN_TARGET_MYRIAD
        && getInferenceEngineVPUType() == CV_DNN_INFERENCE_ENGINE_VPU_TYPE_MYRIAD_X
    )
        applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD_X);
#endif
    // Reference output values are in range [-0.855, 0.611]
    testONNXModels("two_convolution");
}

TEST_P(Test_ONNX_layers, Deconvolution)
{
    testONNXModels("deconvolution", npy, 0, 0, false, false);
    testONNXModels("two_deconvolution", npy, 0, 0, false, false);
    testONNXModels("deconvolution_group", npy, 0, 0, false, false);
    testONNXModels("deconvolution_output_shape", npy, 0, 0, false, false);
    testONNXModels("deconv_adjpad_2d", npy, 0, 0, false, false);
}

TEST_P(Test_ONNX_layers, Deconvolution3D)
{
#if defined(INF_ENGINE_RELEASE)
    applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_2018R5);
#endif
    if (backend != DNN_BACKEND_INFERENCE_ENGINE || target != DNN_TARGET_CPU)
        throw SkipTestException("Only DLIE backend on CPU is supported");
    testONNXModels("deconv3d");
    testONNXModels("deconv3d_bias");
    testONNXModels("deconv3d_pad");
    testONNXModels("deconv3d_adjpad");
}

TEST_P(Test_ONNX_layers, Dropout)
{
    testONNXModels("dropout");
}

TEST_P(Test_ONNX_layers, Linear)
{
    if (backend == DNN_BACKEND_OPENCV && target == DNN_TARGET_OPENCL_FP16)
        applyTestTag(CV_TEST_TAG_DNN_SKIP_OPENCL_FP16);
    testONNXModels("linear");
}

TEST_P(Test_ONNX_layers, ReLU)
{
    testONNXModels("ReLU");
}

TEST_P(Test_ONNX_layers, Clip)
{
    testONNXModels("clip", npy);
}

TEST_P(Test_ONNX_layers, ReduceMean)
{
    testONNXModels("reduce_mean");
}

TEST_P(Test_ONNX_layers, ReduceMean3D)
{
    if (target != DNN_TARGET_CPU)
        throw SkipTestException("Only CPU is supported");
    testONNXModels("reduce_mean3d");
}

TEST_P(Test_ONNX_layers, MaxPooling_Sigmoid)
{
    testONNXModels("maxpooling_sigmoid");
}

TEST_P(Test_ONNX_layers, Concatenation)
{
    if (backend == DNN_BACKEND_INFERENCE_ENGINE)
    {
        if (target == DNN_TARGET_OPENCL_FP16) applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL_FP16);
        if (target == DNN_TARGET_OPENCL)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL);
        if (target == DNN_TARGET_MYRIAD)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD);
    }
    testONNXModels("concatenation");
}

TEST_P(Test_ONNX_layers, Eltwise3D)
{
#if defined(INF_ENGINE_RELEASE) && INF_ENGINE_VER_MAJOR_LT(2019010000)
    throw SkipTestException("Test is enabled starts from 2019R1");
#endif
    if (backend == DNN_BACKEND_INFERENCE_ENGINE && target != DNN_TARGET_CPU)
        throw SkipTestException("Only CPU on DLIE backend is supported");
    testONNXModels("eltwise3d");
}

TEST_P(Test_ONNX_layers, AveragePooling)
{
    testONNXModels("average_pooling");
}

TEST_P(Test_ONNX_layers, MaxPooling3D)
{
#if defined(INF_ENGINE_RELEASE) && INF_ENGINE_VER_MAJOR_LT(2019010000)
    throw SkipTestException("Test is enabled starts from 2019R1");
#endif
    if (target != DNN_TARGET_CPU)
        throw SkipTestException("Only CPU is supported");
    testONNXModels("max_pool3d");
}

TEST_P(Test_ONNX_layers, AvePooling3D)
{
#if defined(INF_ENGINE_RELEASE) && INF_ENGINE_VER_MAJOR_LT(2019010000)
    throw SkipTestException("Test is enabled starts from 2019R1");
#endif
    if (target != DNN_TARGET_CPU)
        throw SkipTestException("Only CPU is supported");
    testONNXModels("ave_pool3d");
}

TEST_P(Test_ONNX_layers, PoolConv3D)
{
#if defined(INF_ENGINE_RELEASE) && INF_ENGINE_VER_MAJOR_LT(2019010000)
    throw SkipTestException("Test is enabled starts from 2019R1");
#endif
    if (target != DNN_TARGET_CPU)
        throw SkipTestException("Only CPU is supported");
    testONNXModels("pool_conv_3d");
}

TEST_P(Test_ONNX_layers, BatchNormalization)
{
    testONNXModels("batch_norm");
}

TEST_P(Test_ONNX_layers, BatchNormalization3D)
{
    if (backend == DNN_BACKEND_INFERENCE_ENGINE)
    {
        if (target == DNN_TARGET_OPENCL_FP16) applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL_FP16);
        if (target == DNN_TARGET_OPENCL)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL);
        if (target == DNN_TARGET_MYRIAD)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD);
    }
    testONNXModels("batch_norm_3d");
}

TEST_P(Test_ONNX_layers, Transpose)
{
    if (backend == DNN_BACKEND_INFERENCE_ENGINE)
    {
        if (target == DNN_TARGET_OPENCL_FP16) applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL_FP16);
        if (target == DNN_TARGET_OPENCL)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL);
        if (target == DNN_TARGET_MYRIAD)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD);
    }
    testONNXModels("transpose");
}

TEST_P(Test_ONNX_layers, Multiplication)
{
    if (backend == DNN_BACKEND_OPENCV && target == DNN_TARGET_OPENCL_FP16)
        applyTestTag(CV_TEST_TAG_DNN_SKIP_OPENCL_FP16);
    if (backend == DNN_BACKEND_INFERENCE_ENGINE && target == DNN_TARGET_MYRIAD)
        applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD);
    testONNXModels("mul");
}

TEST_P(Test_ONNX_layers, Constant)
{
#if defined(INF_ENGINE_RELEASE) && INF_ENGINE_VER_MAJOR_EQ(2018050000)
    if (backend == DNN_BACKEND_INFERENCE_ENGINE && target == DNN_TARGET_MYRIAD
            && getInferenceEngineVPUType() == CV_DNN_INFERENCE_ENGINE_VPU_TYPE_MYRIAD_X)
       applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD_X, CV_TEST_TAG_DNN_SKIP_IE_2018R5);
#endif
    testONNXModels("constant");
}

TEST_P(Test_ONNX_layers, Padding)
{
#if defined(INF_ENGINE_RELEASE) && INF_ENGINE_VER_MAJOR_LT(2019010000)
    testONNXModels("padding", npy, 0, 0, false, false);
#else
    testONNXModels("padding");
#endif
}

TEST_P(Test_ONNX_layers, Resize)
{
    testONNXModels("resize_nearest");
}

TEST_P(Test_ONNX_layers, MultyInputs)
{
    const String model =  _tf("models/multy_inputs.onnx");

    Net net = readNetFromONNX(model);
    ASSERT_FALSE(net.empty());

    net.setPreferableBackend(backend);
    net.setPreferableTarget(target);

    Mat inp1 = blobFromNPY(_tf("data/input_multy_inputs_0.npy"));
    Mat inp2 = blobFromNPY(_tf("data/input_multy_inputs_1.npy"));
    Mat ref  = blobFromNPY(_tf("data/output_multy_inputs.npy"));
    checkBackend(&inp1, &ref);

    net.setInput(inp1, "0");
    net.setInput(inp2, "1");
    Mat out = net.forward();

    normAssert(ref, out, "", default_l1,  default_lInf);
    expectNoFallbacksFromIE(net);
}

TEST_P(Test_ONNX_layers, DynamicReshape)
{
    if (backend == DNN_BACKEND_INFERENCE_ENGINE)
    {
        if (target == DNN_TARGET_OPENCL_FP16) applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL_FP16);
        if (target == DNN_TARGET_OPENCL)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL);
    }
    testONNXModels("dynamic_reshape");
}

TEST_P(Test_ONNX_layers, Reshape)
{
    testONNXModels("unsqueeze");
}

TEST_P(Test_ONNX_layers, Slice)
{
#if defined(INF_ENGINE_RELEASE) && INF_ENGINE_VER_MAJOR_LT(2019010000)
    testONNXModels("slice", npy, 0, 0, false, false);
#else
    testONNXModels("slice");
#endif
}

TEST_P(Test_ONNX_layers, Softmax)
{
    testONNXModels("softmax");
    testONNXModels("log_softmax", npy, 0, 0, false, false);
}

INSTANTIATE_TEST_CASE_P(/*nothing*/, Test_ONNX_layers, dnnBackendsAndTargets());

class Test_ONNX_nets : public Test_ONNX_layers
{
public:
    Test_ONNX_nets() { required = false; }
};

TEST_P(Test_ONNX_nets, Alexnet)
{
    applyTestTag(target == DNN_TARGET_CPU ? CV_TEST_TAG_MEMORY_512MB : CV_TEST_TAG_MEMORY_1GB);
    const String model =  _tf("models/alexnet.onnx", false);

    Net net = readNetFromONNX(model);
    ASSERT_FALSE(net.empty());

    net.setPreferableBackend(backend);
    net.setPreferableTarget(target);

    Mat inp = imread(_tf("../grace_hopper_227.png"));
    Mat ref = blobFromNPY(_tf("../caffe_alexnet_prob.npy"));
    checkBackend(&inp, &ref);

    net.setInput(blobFromImage(inp, 1.0f, Size(227, 227), Scalar(), false));
    ASSERT_FALSE(net.empty());
    Mat out = net.forward();

    normAssert(out, ref, "", default_l1,  default_lInf);
    expectNoFallbacksFromIE(net);
}

TEST_P(Test_ONNX_nets, Squeezenet)
{
    testONNXModels("squeezenet", pb);
}

TEST_P(Test_ONNX_nets, Googlenet)
{
    if (backend == DNN_BACKEND_INFERENCE_ENGINE)
        applyTestTag(CV_TEST_TAG_DNN_SKIP_IE);

    const String model = _tf("models/googlenet.onnx", false);

    Net net = readNetFromONNX(model);
    ASSERT_FALSE(net.empty());

    net.setPreferableBackend(backend);
    net.setPreferableTarget(target);

    std::vector<Mat> images;
    images.push_back( imread(_tf("../googlenet_0.png")) );
    images.push_back( imread(_tf("../googlenet_1.png")) );
    Mat inp = blobFromImages(images, 1.0f, Size(), Scalar(), false);
    Mat ref = blobFromNPY(_tf("../googlenet_prob.npy"));
    checkBackend(&inp, &ref);

    net.setInput(inp);
    ASSERT_FALSE(net.empty());
    Mat out = net.forward();

    normAssert(ref, out, "", default_l1,  default_lInf);
    expectNoFallbacksFromIE(net);
}

TEST_P(Test_ONNX_nets, CaffeNet)
{
    applyTestTag(target == DNN_TARGET_CPU ? CV_TEST_TAG_MEMORY_512MB : CV_TEST_TAG_MEMORY_1GB);
    testONNXModels("caffenet", pb);
}

TEST_P(Test_ONNX_nets, RCNN_ILSVRC13)
{
    applyTestTag(target == DNN_TARGET_CPU ? CV_TEST_TAG_MEMORY_512MB : CV_TEST_TAG_MEMORY_1GB);

    // Reference output values are in range [-4.992, -1.161]
    testONNXModels("rcnn_ilsvrc13", pb, 0.0045);
}

TEST_P(Test_ONNX_nets, VGG16)
{
    applyTestTag(CV_TEST_TAG_MEMORY_6GB);  // > 2.3Gb

    // output range: [-69; 72], after Softmax [0; 0.96]
    testONNXModels("vgg16", pb, default_l1, default_lInf, true);
}

TEST_P(Test_ONNX_nets, VGG16_bn)
{
    applyTestTag(CV_TEST_TAG_MEMORY_6GB);  // > 2.3Gb

    // output range: [-16; 27], after Softmax [0; 0.67]
    const double lInf = (target == DNN_TARGET_MYRIAD) ? 0.038 : default_lInf;
    testONNXModels("vgg16-bn", pb, default_l1, lInf, true);
}

TEST_P(Test_ONNX_nets, ZFNet)
{
    applyTestTag(CV_TEST_TAG_MEMORY_2GB);
    testONNXModels("zfnet512", pb);
}

TEST_P(Test_ONNX_nets, ResNet18v1)
{
    applyTestTag(CV_TEST_TAG_MEMORY_512MB);

    // output range: [-16; 22], after Softmax [0, 0.51]
    testONNXModels("resnet18v1", pb, default_l1, default_lInf, true, target != DNN_TARGET_MYRIAD);
}

TEST_P(Test_ONNX_nets, ResNet50v1)
{
    applyTestTag(CV_TEST_TAG_MEMORY_512MB);

    // output range: [-67; 75], after Softmax [0, 0.98]
    testONNXModels("resnet50v1", pb, default_l1, default_lInf, true, target != DNN_TARGET_MYRIAD);
}

TEST_P(Test_ONNX_nets, ResNet101_DUC_HDC)
{
    applyTestTag(CV_TEST_TAG_VERYLONG);

#if defined(INF_ENGINE_RELEASE) && INF_ENGINE_VER_MAJOR_GE(2019010000)
    if (backend == DNN_BACKEND_INFERENCE_ENGINE)
        applyTestTag(CV_TEST_TAG_DNN_SKIP_IE, CV_TEST_TAG_DNN_SKIP_IE_2019R1, CV_TEST_TAG_DNN_SKIP_IE_2019R1_1);
#endif
#if defined(INF_ENGINE_RELEASE)
    if (backend == DNN_BACKEND_INFERENCE_ENGINE && target == DNN_TARGET_MYRIAD)
        applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD);
#endif
    if (target == DNN_TARGET_OPENCL_FP16 || target == DNN_TARGET_OPENCL)
    {
        if (backend == DNN_BACKEND_OPENCV)
            applyTestTag(target == DNN_TARGET_OPENCL ? CV_TEST_TAG_DNN_SKIP_OPENCL : CV_TEST_TAG_DNN_SKIP_OPENCL_FP16);
        throw SkipTestException("Test is disabled for OpenCL targets");
    }
    testONNXModels("resnet101_duc_hdc", pb);
}

TEST_P(Test_ONNX_nets, TinyYolov2)
{
    applyTestTag(CV_TEST_TAG_MEMORY_512MB);

    if (cvtest::skipUnstableTests)
        throw SkipTestException("Skip unstable test");
#if defined(INF_ENGINE_RELEASE)
    if (backend == DNN_BACKEND_INFERENCE_ENGINE
            && (target == DNN_TARGET_OPENCL || target == DNN_TARGET_OPENCL_FP16)
    )
        applyTestTag(target == DNN_TARGET_OPENCL ? CV_TEST_TAG_DNN_SKIP_IE_OPENCL : CV_TEST_TAG_DNN_SKIP_IE_OPENCL_FP16);

    if (backend == DNN_BACKEND_INFERENCE_ENGINE && target == DNN_TARGET_MYRIAD
            && getInferenceEngineVPUType() == CV_DNN_INFERENCE_ENGINE_VPU_TYPE_MYRIAD_X
    )
        applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD_X);
#endif

    // output range: [-11; 8]
    double l1 = (target == DNN_TARGET_OPENCL_FP16 || target == DNN_TARGET_MYRIAD) ? 0.017 : default_l1;
    double lInf = (target == DNN_TARGET_OPENCL_FP16 || target == DNN_TARGET_MYRIAD) ? 0.14 : default_lInf;
    testONNXModels("tiny_yolo2", pb, l1, lInf);
}

TEST_P(Test_ONNX_nets, CNN_MNIST)
{
    // output range: [-1952; 6574], after Softmax [0; 1]
    testONNXModels("cnn_mnist", pb, default_l1, default_lInf, true);
}

TEST_P(Test_ONNX_nets, MobileNet_v2)
{
    // output range: [-166; 317], after Softmax [0; 1]
    testONNXModels("mobilenetv2", pb, default_l1, default_lInf, true);
}

TEST_P(Test_ONNX_nets, LResNet100E_IR)
{
    applyTestTag(
        (target == DNN_TARGET_CPU ? CV_TEST_TAG_MEMORY_512MB : CV_TEST_TAG_MEMORY_1GB),
        CV_TEST_TAG_DEBUG_LONG
    );
    if (backend == DNN_BACKEND_INFERENCE_ENGINE)
    {
        if (target == DNN_TARGET_OPENCL_FP16) applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL_FP16);
        if (target == DNN_TARGET_OPENCL)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL);
        if (target == DNN_TARGET_MYRIAD)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD);
    }

    double l1 = default_l1;
    double lInf = default_lInf;
    // output range: [-3; 3]
    if (backend == DNN_BACKEND_OPENCV && target == DNN_TARGET_OPENCL_FP16) {
        l1 = 0.009;
        lInf = 0.035;
    }
    else if (backend == DNN_BACKEND_INFERENCE_ENGINE && target == DNN_TARGET_CPU) {
        l1 = 4.6e-5;
        lInf = 1.9e-4;
    }
    testONNXModels("LResNet100E_IR", pb, l1, lInf);
}

TEST_P(Test_ONNX_nets, Emotion_ferplus)
{
#if defined(INF_ENGINE_RELEASE)
    if (backend == DNN_BACKEND_INFERENCE_ENGINE && target == DNN_TARGET_MYRIAD
            && getInferenceEngineVPUType() == CV_DNN_INFERENCE_ENGINE_VPU_TYPE_MYRIAD_X
    )
        applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD_X);
#endif

    double l1 = default_l1;
    double lInf = default_lInf;

    // Output values are in range [-2.011, 2.111]
    if (backend == DNN_BACKEND_OPENCV && target == DNN_TARGET_OPENCL_FP16)
        l1 = 0.007;
    else if (backend == DNN_BACKEND_INFERENCE_ENGINE && target == DNN_TARGET_OPENCL_FP16)
    {
        l1 = 0.021;
        lInf = 0.034;
    }
    else if (backend == DNN_BACKEND_INFERENCE_ENGINE && (target == DNN_TARGET_CPU || target == DNN_TARGET_OPENCL)) {
        l1 = 2.4e-4;
        lInf = 6e-4;
    }
    testONNXModels("emotion_ferplus", pb, l1, lInf);
}

TEST_P(Test_ONNX_nets, Inception_v2)
{
    testONNXModels("inception_v2", pb, default_l1, default_lInf, true);
}

TEST_P(Test_ONNX_nets, DenseNet121)
{
    applyTestTag(CV_TEST_TAG_MEMORY_512MB);

    // output range: [-87; 138], after Softmax [0; 1]
    testONNXModels("densenet121", pb, default_l1, default_lInf, true, target != DNN_TARGET_MYRIAD);
}

TEST_P(Test_ONNX_nets, Inception_v1)
{
#if defined(INF_ENGINE_RELEASE)
    if (backend == DNN_BACKEND_INFERENCE_ENGINE && target == DNN_TARGET_MYRIAD)
        applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD);
#endif
    testONNXModels("inception_v1", pb);
}

TEST_P(Test_ONNX_nets, Shufflenet)
{
    if (backend == DNN_BACKEND_INFERENCE_ENGINE)
    {
        if (target == DNN_TARGET_OPENCL_FP16) applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL_FP16);
        if (target == DNN_TARGET_OPENCL)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_OPENCL);
        if (target == DNN_TARGET_MYRIAD)      applyTestTag(CV_TEST_TAG_DNN_SKIP_IE_MYRIAD);
    }
    testONNXModels("shufflenet", pb);
}

TEST_P(Test_ONNX_nets, Resnet34_kinetics)
{
#if defined(INF_ENGINE_RELEASE) && INF_ENGINE_VER_MAJOR_LT(2019010000)
    throw SkipTestException("Test is enabled starts from 2019R1");
#endif
    if (target != DNN_TARGET_CPU)
        throw SkipTestException("Only CPU is supported");

    String onnxmodel = findDataFile("dnn/resnet-34_kinetics.onnx", false);
    Mat image0 = imread(findDataFile("dnn/dog416.png"));
    Mat image1 = imread(findDataFile("dnn/street.png"));

    Mat ref0 = blobFromNPY(_tf("data/output_kinetics0.npy"));
    Mat ref1 = blobFromNPY(_tf("data/output_kinetics1.npy"));

    std::vector<Mat> images_0(16, image0);
    std::vector<Mat> images_1(16, image1);
    Mat blob0 = blobFromImages(images_0, 1.0, Size(112, 112), Scalar(114.7748, 107.7354, 99.4750), true, true);
    Mat blob1 = blobFromImages(images_1, 1.0, Size(112, 112), Scalar(114.7748, 107.7354, 99.4750), true, true);

    Net permute;
    LayerParams lp;
    int order[] = {1, 0, 2, 3};
    lp.set("order", DictValue::arrayInt<int*>(&order[0], 4));
    permute.addLayerToPrev("perm", "Permute", lp);

    permute.setInput(blob0);
    Mat input0 = permute.forward().clone();

    permute.setInput(blob1);
    Mat input1 = permute.forward().clone();

    int dims[] = {1, 3, 16, 112, 112};
    input0 = input0.reshape(0, 5, &dims[0]);
    input1 = input1.reshape(0, 5, &dims[0]);

    Net net = readNetFromONNX(onnxmodel);
    ASSERT_FALSE(net.empty());
    net.setPreferableBackend(backend);
    net.setPreferableTarget(target);

    // output range [-5, 11]
    float l1 = 0.0013;
    float lInf = 0.009;

    checkBackend(&input0, &ref0);
    net.setInput(input0);
    Mat out = net.forward().clone();
    normAssert(ref0, out, "", l1, lInf);

    checkBackend(&input1, &ref1);
    net.setInput(input1);
    out = net.forward().clone();
    normAssert(ref1, out, "", l1, lInf);

    expectNoFallbacksFromIE(net);
}

INSTANTIATE_TEST_CASE_P(/**/, Test_ONNX_nets, dnnBackendsAndTargets());

}} // namespace
