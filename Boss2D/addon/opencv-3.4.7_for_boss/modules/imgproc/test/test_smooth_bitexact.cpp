// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#include "test_precomp.hpp"

namespace opencv_test { namespace {

    static const int fixedShiftU8 = 8;
    static const int64_t fixedOne = (1L << fixedShiftU8);

    int64_t v[][9] = {
        { fixedOne }, // size 1, sigma 0
        { fixedOne >> 2, fixedOne >> 1, fixedOne >> 2 }, // size 3, sigma 0
        { fixedOne >> 4, fixedOne >> 2, 6 * (fixedOne >> 4), fixedOne >> 2, fixedOne >> 4 }, // size 5, sigma 0
        { fixedOne >> 5, 7 * (fixedOne >> 6), 7 * (fixedOne >> 5), 9 * (fixedOne >> 5), 7 * (fixedOne >> 5), 7 * (fixedOne >> 6), fixedOne >> 5 }, // size 7, sigma 0
        { 4, 13, 30, 51, 61, 51, 30, 13, 4 }, // size 9, sigma 0
        { 81, 95, 81 }, // size 3, sigma 1.75
        { 65, 125, 65 }, // size 3, sigma 0.875
        { 0, 7, 242, 7, 0 }, // size 5, sigma 0.375
        { 4, 56, 136, 56, 4 } // size 5, sigma 0.75
    };

    template <typename T, int fixedShift>
    T eval(Mat src, vector<int64_t> kernelx, vector<int64_t> kernely)
    {
        static const int64_t fixedRound = ((1LL << (fixedShift * 2)) >> 1);
        int64_t val = 0;
        for (size_t j = 0; j < kernely.size(); j++)
        {
            int64_t lineval = 0;
            for (size_t i = 0; i < kernelx.size(); i++)
                lineval += src.at<T>((int)j, (int)i) * kernelx[i];
            val += lineval * kernely[j];
        }
        return saturate_cast<T>((val + fixedRound) >> (fixedShift * 2));
    }

TEST(GaussianBlur_Bitexact, Linear8U)
{
    struct testmode
    {
        int type;
        Size sz;
        Size kernel;
        double sigma_x;
        double sigma_y;
        vector<int64_t> kernel_x;
        vector<int64_t> kernel_y;
    } modes[] = {
        { CV_8UC1, Size(   1,   1), Size(3, 3), 0, 0, vector<int64_t>(v[1], v[1]+3), vector<int64_t>(v[1], v[1]+3) },
        { CV_8UC1, Size(   2,   2), Size(3, 3), 0, 0, vector<int64_t>(v[1], v[1]+3), vector<int64_t>(v[1], v[1]+3) },
        { CV_8UC1, Size(   3,   1), Size(3, 3), 0, 0, vector<int64_t>(v[1], v[1]+3), vector<int64_t>(v[1], v[1]+3) },
        { CV_8UC1, Size(   1,   3), Size(3, 3), 0, 0, vector<int64_t>(v[1], v[1]+3), vector<int64_t>(v[1], v[1]+3) },
        { CV_8UC1, Size(   3,   3), Size(3, 3), 0, 0, vector<int64_t>(v[1], v[1]+3), vector<int64_t>(v[1], v[1]+3) },
        { CV_8UC1, Size(   3,   3), Size(5, 5), 0, 0, vector<int64_t>(v[2], v[2]+5), vector<int64_t>(v[2], v[2]+5) },
        { CV_8UC1, Size(   3,   3), Size(7, 7), 0, 0, vector<int64_t>(v[3], v[3]+7), vector<int64_t>(v[3], v[3]+7) },
        { CV_8UC1, Size(   5,   5), Size(3, 3), 0, 0, vector<int64_t>(v[1], v[1]+3), vector<int64_t>(v[1], v[1]+3) },
        { CV_8UC1, Size(   5,   5), Size(5, 5), 0, 0, vector<int64_t>(v[2], v[2]+5), vector<int64_t>(v[2], v[2]+5) },
        { CV_8UC1, Size(   3,   5), Size(5, 5), 0, 0, vector<int64_t>(v[2], v[2]+5), vector<int64_t>(v[2], v[2]+5) },
        { CV_8UC1, Size(   5,   5), Size(5, 5), 0, 0, vector<int64_t>(v[2], v[2]+5), vector<int64_t>(v[2], v[2]+5) },
        { CV_8UC1, Size(   5,   5), Size(7, 7), 0, 0, vector<int64_t>(v[3], v[3]+7), vector<int64_t>(v[3], v[3]+7) },
        { CV_8UC1, Size(   7,   7), Size(7, 7), 0, 0, vector<int64_t>(v[3], v[3]+7), vector<int64_t>(v[3], v[3]+7) },
        { CV_8UC1, Size( 256, 128), Size(3, 3), 0, 0, vector<int64_t>(v[1], v[1]+3), vector<int64_t>(v[1], v[1]+3) },
        { CV_8UC2, Size( 256, 128), Size(3, 3), 0, 0, vector<int64_t>(v[1], v[1]+3), vector<int64_t>(v[1], v[1]+3) },
        { CV_8UC3, Size( 256, 128), Size(3, 3), 0, 0, vector<int64_t>(v[1], v[1]+3), vector<int64_t>(v[1], v[1]+3) },
        { CV_8UC4, Size( 256, 128), Size(3, 3), 0, 0, vector<int64_t>(v[1], v[1]+3), vector<int64_t>(v[1], v[1]+3) },
        { CV_8UC1, Size( 256, 128), Size(5, 5), 0, 0, vector<int64_t>(v[2], v[2]+5), vector<int64_t>(v[2], v[2]+5) },
        { CV_8UC1, Size( 256, 128), Size(7, 7), 0, 0, vector<int64_t>(v[3], v[3]+7), vector<int64_t>(v[3], v[3]+7) },
        { CV_8UC1, Size( 256, 128), Size(9, 9), 0, 0, vector<int64_t>(v[4], v[4]+9), vector<int64_t>(v[4], v[4]+9) },
        { CV_8UC1, Size( 256, 128), Size(3, 3), 1.75, 0.875, vector<int64_t>(v[5], v[5]+3), vector<int64_t>(v[6], v[6]+3) },
        { CV_8UC2, Size( 256, 128), Size(3, 3), 1.75, 0.875, vector<int64_t>(v[5], v[5]+3), vector<int64_t>(v[6], v[6]+3) },
        { CV_8UC3, Size( 256, 128), Size(3, 3), 1.75, 0.875, vector<int64_t>(v[5], v[5]+3), vector<int64_t>(v[6], v[6]+3) },
        { CV_8UC4, Size( 256, 128), Size(3, 3), 1.75, 0.875, vector<int64_t>(v[5], v[5]+3), vector<int64_t>(v[6], v[6]+3) },
        { CV_8UC1, Size( 256, 128), Size(5, 5), 0.375, 0.75, vector<int64_t>(v[7], v[7]+5), vector<int64_t>(v[8], v[8]+5) }
    };

    int bordermodes[] = {
        BORDER_CONSTANT | BORDER_ISOLATED,
        BORDER_REPLICATE | BORDER_ISOLATED,
        BORDER_REFLECT | BORDER_ISOLATED,
        BORDER_WRAP | BORDER_ISOLATED,
        BORDER_REFLECT_101 | BORDER_ISOLATED
//        BORDER_CONSTANT,
//        BORDER_REPLICATE,
//        BORDER_REFLECT,
//        BORDER_WRAP,
//        BORDER_REFLECT_101
    };

    for (int modeind = 0, _modecnt = sizeof(modes) / sizeof(modes[0]); modeind < _modecnt; ++modeind)
    {
        int type = modes[modeind].type, depth = CV_MAT_DEPTH(type), cn = CV_MAT_CN(type);
        int dcols = modes[modeind].sz.width, drows = modes[modeind].sz.height;
        Size kernel = modes[modeind].kernel;

        int rows = drows + 20, cols = dcols + 20;
        Mat src(rows, cols, type), refdst(drows, dcols, type), dst;
        for (int j = 0; j < rows; j++)
        {
            uint8_t* line = src.ptr(j);
            for (int i = 0; i < cols; i++)
                for (int c = 0; c < cn; c++)
                {
                    RNG rnd(0x123456789abcdefULL);
                    double val = j < rows / 2 ? (i < cols / 2 ? ((sin((i + 1)*CV_PI / 256.)*sin((j + 1)*CV_PI / 256.)*sin((cn + 4)*CV_PI / 8.) + 1.)*128.) :
                        (((i / 128 + j / 128) % 2) * 250 + (j / 128) % 2)) :
                        (i < cols / 2 ? ((i / 128) * (85 - j / 256 * 40) * ((j / 128) % 2) + (7 - i / 128) * (85 - j / 256 * 40) * ((j / 128 + 1) % 2)) :
                        ((uchar)rnd));
                    if (depth == CV_8U)
                        line[i*cn + c] = (uint8_t)val;
                    else if (depth == CV_16U)
                        ((uint16_t*)line)[i*cn + c] = (uint16_t)val;
                    else if (depth == CV_16S)
                        ((int16_t*)line)[i*cn + c] = (int16_t)val;
                    else if (depth == CV_32S)
                        ((int32_t*)line)[i*cn + c] = (int32_t)val;
                    else
                        CV_Assert(0);
                }
        }
        Mat src_roi = src(Rect(10, 10, dcols, drows));


        for (int borderind = 0, _bordercnt = sizeof(bordermodes) / sizeof(bordermodes[0]); borderind < _bordercnt; ++borderind)
        {
            Mat src_border;
            cv::copyMakeBorder(src_roi, src_border, kernel.height / 2, kernel.height / 2, kernel.width / 2, kernel.width / 2, bordermodes[borderind]);
            for (int c = 0; c < src_border.channels(); c++)
            {
                int fromTo[2] = { c, 0 };
                int toFrom[2] = { 0, c };
                Mat src_chan(src_border.size(), CV_MAKETYPE(src_border.depth(),1));
                Mat dst_chan(refdst.size(), CV_MAKETYPE(refdst.depth(), 1));
                mixChannels(src_border, src_chan, fromTo, 1);
                for (int j = 0; j < drows; j++)
                    for (int i = 0; i < dcols; i++)
                    {
                        if (depth == CV_8U)
                            dst_chan.at<uint8_t>(j, i) = eval<uint8_t, fixedShiftU8>(src_chan(Rect(i,j,kernel.width,kernel.height)), modes[modeind].kernel_x, modes[modeind].kernel_y);
                        else if (depth == CV_16U)
                            dst_chan.at<uint16_t>(j, i) = eval<uint16_t, fixedShiftU8>(src_chan(Rect(i, j, kernel.width, kernel.height)), modes[modeind].kernel_x, modes[modeind].kernel_y);
                        else if (depth == CV_16S)
                            dst_chan.at<int16_t>(j, i) = eval<int16_t, fixedShiftU8>(src_chan(Rect(i, j, kernel.width, kernel.height)), modes[modeind].kernel_x, modes[modeind].kernel_y);
                        else if (depth == CV_32S)
                            dst_chan.at<int32_t>(j, i) = eval<int32_t, fixedShiftU8>(src_chan(Rect(i, j, kernel.width, kernel.height)), modes[modeind].kernel_x, modes[modeind].kernel_y);
                        else
                            CV_Assert(0);
                    }
                mixChannels(dst_chan, refdst, toFrom, 1);
            }

            cv::GaussianBlur(src_roi, dst, kernel, modes[modeind].sigma_x, modes[modeind].sigma_y, bordermodes[borderind]);

            EXPECT_GE(0, cvtest::norm(refdst, dst, cv::NORM_L1))
                << "GaussianBlur " << cn << "-chan mat " << drows << "x" << dcols << " by kernel " << kernel << " sigma(" << modes[modeind].sigma_x << ";" << modes[modeind].sigma_y << ") failed with max diff " << cvtest::norm(refdst, dst, cv::NORM_INF);
        }
    }
}

TEST(GaussianBlur_Bitexact, regression_15015)
{
    Mat src(100,100,CV_8UC3,Scalar(255,255,255));
    Mat dst;
    GaussianBlur(src, dst, Size(5, 5), 9);
    ASSERT_EQ(0.0, cvtest::norm(dst, src, NORM_INF));
}

}} // namespace
