// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Copyright (C) 2018-2019 Intel Corporation


#include "../test_precomp.hpp"
#include "../common/gapi_core_tests.hpp"
#include <opencv2/gapi/cpu/core.hpp>

namespace
{
#define CORE_CPU [] () { return cv::compile_args(cv::gapi::core::cpu::kernels()); }
}  // anonymous namespace

namespace opencv_test
{

// FIXME: Wut? See MulTestCPU/MathOpTest below (duplicate?)
INSTANTIATE_TEST_CASE_P(AddTestCPU, MathOpTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values( -1, CV_8U, CV_16U, CV_32F ),
                                Values(CORE_CPU),
                                Values(ADD, MUL),
                                testing::Bool(),
                                Values(1.0),
                                Values(false)));

INSTANTIATE_TEST_CASE_P(MulTestCPU, MathOpTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values( -1, CV_8U, CV_16U, CV_32F ),
                                Values(CORE_CPU),
                                Values(MUL),
                                testing::Bool(),
                                Values(1.0, 0.5, 2.0),
                                Values(false)));

INSTANTIATE_TEST_CASE_P(SubTestCPU, MathOpTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values( -1, CV_8U, CV_16U, CV_32F ),
                                Values(CORE_CPU),
                                Values(SUB),
                                testing::Bool(),
                                Values (1.0),
                                testing::Bool()));

INSTANTIATE_TEST_CASE_P(DivTestCPU, MathOpTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values( -1, CV_8U, CV_16U, CV_32F ),
                                Values(CORE_CPU),
                                Values(DIV),
                                testing::Bool(),
                                Values (1.0, 0.5, 2.0),
                                testing::Bool()));

INSTANTIATE_TEST_CASE_P(MulTestCPU, MulDoubleTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values( -1, CV_8U, CV_16U, CV_32F ),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(DivTestCPU, DivTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values( -1, CV_8U, CV_16U, CV_32F ),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(DivCTestCPU, DivCTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values( -1, CV_8U, CV_16U, CV_32F ),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(MeanTestCPU, MeanTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(MaskTestCPU, MaskTest,
                        Combine(Values(CV_8UC1, CV_16UC1, CV_16SC1),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(SelectTestCPU, SelectTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(Polar2CartCPU, Polar2CartTest,
                        Combine(Values(CV_32FC1),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(CV_32FC1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(Cart2PolarCPU, Cart2PolarTest,
                        Combine(Values(CV_32FC1),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(CV_32FC1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(PhaseCPU, PhaseTest,
                        Combine(Values(CV_32F, CV_32FC3),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU),
         /* angle_in_degrees */ testing::Bool()));

INSTANTIATE_TEST_CASE_P(SqrtCPU, SqrtTest,
                        Combine(Values(CV_32F, CV_32FC3),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(CompareTestCPU, CmpTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(CV_8U),
                                Values(CORE_CPU),
                                Values(CMP_EQ, CMP_GE, CMP_NE, CMP_GT, CMP_LT, CMP_LE),
                                testing::Bool()));

INSTANTIATE_TEST_CASE_P(BitwiseTestCPU, BitwiseTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(AND, OR, XOR)));

INSTANTIATE_TEST_CASE_P(BitwiseNotTestCPU, NotTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(MinTestCPU, MinTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(MaxTestCPU, MaxTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(SumTestCPU, SumTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                //Values(1e-5),
                                Values(CORE_CPU),
                                Values(AbsToleranceScalar(1e-5).to_compare_obj())));

INSTANTIATE_TEST_CASE_P(AbsDiffTestCPU, AbsDiffTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(AbsDiffCTestCPU, AbsDiffCTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(AddWeightedTestCPU, AddWeightedTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values( -1, CV_8U, CV_16U, CV_32F ),
                                Values(CORE_CPU),
                                Values(Tolerance_FloatRel_IntAbs(1e-6, 1).to_compare_obj())));

INSTANTIATE_TEST_CASE_P(NormTestCPU, NormTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(AbsToleranceScalar(1e-5).to_compare_obj()),
                                Values(NORM_INF, NORM_L1, NORM_L2)));

INSTANTIATE_TEST_CASE_P(IntegralTestCPU, IntegralTest,
                        Combine(Values( CV_8UC1, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(ThresholdTestCPU, ThresholdTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(cv::THRESH_BINARY, cv::THRESH_BINARY_INV, cv::THRESH_TRUNC,
                                    cv::THRESH_TOZERO, cv::THRESH_TOZERO_INV)));

INSTANTIATE_TEST_CASE_P(ThresholdTestCPU, ThresholdOTTest,
                        Combine(Values(CV_8UC1),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(cv::THRESH_OTSU, cv::THRESH_TRIANGLE)));


INSTANTIATE_TEST_CASE_P(InRangeTestCPU, InRangeTest,
                        Combine(Values(CV_8UC1, CV_16UC1, CV_16SC1, CV_32FC1),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(Split3TestCPU, Split3Test,
                        Combine(Values(CV_8UC3),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(CV_8UC1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(Split4TestCPU, Split4Test,
                        Combine(Values(CV_8UC4),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(CV_8UC1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(ResizeTestCPU, ResizeTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(AbsSimilarPoints(2, 0.05).to_compare_obj()),
                                Values(cv::INTER_NEAREST, cv::INTER_LINEAR, cv::INTER_AREA),
                                Values(cv::Size(64,64),
                                       cv::Size(30,30))));

INSTANTIATE_TEST_CASE_P(ResizePTestCPU, ResizePTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(AbsSimilarPoints(2, 0.05).to_compare_obj()),
                                Values(cv::INTER_LINEAR),
                                Values(cv::Size(64,64),
                                       cv::Size(30,30))));

INSTANTIATE_TEST_CASE_P(ResizeTestCPU, ResizeTestFxFy,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(AbsSimilarPoints(2, 0.05).to_compare_obj()),
                                Values(cv::INTER_NEAREST, cv::INTER_LINEAR, cv::INTER_AREA),
                                Values(0.5, 0.1),
                                Values(0.5, 0.1)));

INSTANTIATE_TEST_CASE_P(Merge3TestCPU, Merge3Test,
                        Combine(Values(CV_8UC1),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(CV_8UC3),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(Merge4TestCPU, Merge4Test,
                        Combine(Values(CV_8UC1),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(CV_8UC4),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(RemapTestCPU, RemapTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(FlipTestCPU, FlipTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(0,1,-1)));

INSTANTIATE_TEST_CASE_P(CropTestCPU, CropTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(cv::Rect(10, 8, 20, 35), cv::Rect(4, 10, 37, 50))));

INSTANTIATE_TEST_CASE_P(LUTTestCPU, LUTTest,
                        Combine(Values(CV_8UC1, CV_8UC3),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(CV_8UC1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(LUTTestCustomCPU, LUTTest,
                        Combine(Values(CV_8UC3),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(CV_8UC3),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(ConvertToCPU, ConvertToTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(CV_8U, CV_16U, CV_16S, CV_32F),
                                Values(CORE_CPU),
                                Values(AbsExact().to_compare_obj()),
                                Values(2.5, 1.0, -1.0),
                                Values(250.0, 0.0, -128.0)));

INSTANTIATE_TEST_CASE_P(ConcatHorTestCPU, ConcatHorTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(ConcatVertTestCPU, ConcatVertTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(ConcatVertVecTestCPU, ConcatVertVecTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(ConcatHorVecTestCPU, ConcatHorVecTest,
                        Combine(Values( CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1 ),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480),
                                       cv::Size(128, 128)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(NormalizeTestCPU, NormalizeTest,
                        Combine(Values(CV_8UC1, CV_8UC3, CV_16UC1, CV_16SC1, CV_32FC1),
                                Values(cv::Size(1280, 720),
                                       cv::Size(640, 480)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(AbsExact().to_compare_obj()),
                                Values(0.0, 15.0),
                                Values(1.0, 120.0, 255.0),
                                Values(NORM_MINMAX, NORM_INF, NORM_L1, NORM_L2),
                                Values(-1, CV_8U, CV_16U, CV_16S, CV_32F)));

INSTANTIATE_TEST_CASE_P(BackendOutputAllocationTestCPU, BackendOutputAllocationTest,
                        Combine(Values(CV_8UC3, CV_16SC2, CV_32FC1),
                                Values(cv::Size(50, 50)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(BackendOutputAllocationLargeSizeWithCorrectSubmatrixTestCPU,
                        BackendOutputAllocationLargeSizeWithCorrectSubmatrixTest,
                        Combine(Values(CV_8UC3, CV_16SC2, CV_32FC1),
                                Values(cv::Size(50, 50)),
                                Values(-1),
                                Values(CORE_CPU)));

INSTANTIATE_TEST_CASE_P(ReInitOutTestCPU, ReInitOutTest,
                        Combine(Values(CV_8UC3, CV_16SC4, CV_32FC1),
                                Values(cv::Size(640, 480)),
                                Values(-1),
                                Values(CORE_CPU),
                                Values(cv::Size(640, 400),
                                       cv::Size(10, 480))));
}
