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

#include "test_precomp.hpp"
#include "opencv2/videoio/videoio_c.h"

namespace opencv_test
{

class Videoio_Test_Base
{
protected:
    string ext;
    string video_file;
    VideoCaptureAPIs apiPref;
protected:
    Videoio_Test_Base() {}
    virtual ~Videoio_Test_Base() {}
    virtual void writeVideo() {}
    virtual void checkFrameContent(Mat &, int) {}
    virtual void checkFrameCount(int &) {}
    void checkFrameRead(int idx, VideoCapture & cap)
    {
        //int frameID = (int)cap.get(CAP_PROP_POS_FRAMES);
        Mat img;
        ASSERT_NO_THROW(cap >> img);
        //std::cout << "idx=" << idx << " img=" << img.size() << " frameID=" << frameID << std::endl;
        ASSERT_FALSE(img.empty()) << "idx=" << idx;
        checkFrameContent(img, idx);
    }
    void checkFrameSeek(int idx, VideoCapture & cap)
    {
        bool canSeek = false;
        ASSERT_NO_THROW(canSeek = cap.set(CAP_PROP_POS_FRAMES, idx));
        if (!canSeek)
        {
            std::cout << "Seek to frame '" << idx << "' is not supported. SKIP." << std::endl;
            return;
        }
        EXPECT_EQ(idx, (int)cap.get(CAP_PROP_POS_FRAMES));
        checkFrameRead(idx, cap);
    }
public:
    void doTest()
    {
        if (!videoio_registry::hasBackend(apiPref))
            throw SkipTestException(cv::String("Backend is not available/disabled: ") + cv::videoio_registry::getBackendName(apiPref));
        writeVideo();
        VideoCapture cap;
        ASSERT_NO_THROW(cap.open(video_file, apiPref));
        if (!cap.isOpened())
        {
            std::cout << "SKIP test: backend " << apiPref << " can't open the video: " << video_file << std::endl;
            return;
        }
        int n_frames = -1;
        EXPECT_NO_THROW(n_frames = (int)cap.get(CAP_PROP_FRAME_COUNT));
        if (n_frames > 0)
        {
            ASSERT_GT(n_frames, 0);
            checkFrameCount(n_frames);
        }
        else
        {
            std::cout << "CAP_PROP_FRAME_COUNT is not supported by backend. Assume 50 frames." << std::endl;
            n_frames = 50;
        }

        {
            SCOPED_TRACE("consecutive read");
            if (apiPref == CAP_GSTREAMER)
            {
                // This workaround is for GStreamer 1.3.1.1 and older.
                // Old Gstreamer has a bug which handles the total duration 1 frame shorter
                // Old Gstreamer are used in Ubuntu 14.04, so the following code could be removed after it's EOL
                n_frames--;
            }
            for (int k = 0; k < n_frames; ++k)
            {
                checkFrameRead(k, cap);
            }
        }
        bool canSeek = false;
        EXPECT_NO_THROW(canSeek = cap.set(CAP_PROP_POS_FRAMES, 0));
        if (!canSeek)
        {
            std::cout << "Seek to frame '0' is not supported. SKIP all 'seek' tests." << std::endl;
            return;
        }

        if (ext != "wmv" && ext != "h264" && ext != "h265")
        {
            SCOPED_TRACE("progressive seek");
            bool res = false;
            EXPECT_NO_THROW(res = cap.set(CAP_PROP_POS_FRAMES, 0));
            ASSERT_TRUE(res);
            for (int k = 0; k < n_frames; k += 20)
            {
                checkFrameSeek(k, cap);
            }
        }

        if (ext != "mpg" && ext != "wmv" && ext != "h264" && ext != "h265")
        {
            SCOPED_TRACE("random seek");
            bool res = false;
            EXPECT_NO_THROW(res = cap.set(CAP_PROP_POS_FRAMES, 0));
            ASSERT_TRUE(res);
            for (int k = 0; k < 10; ++k)
            {
                checkFrameSeek(cvtest::TS::ptr()->get_rng().uniform(0, n_frames), cap);
            }
        }
    }
};

//==================================================================================================
typedef tuple<string, VideoCaptureAPIs> Backend_Type_Params;

class videoio_bunny : public Videoio_Test_Base, public testing::TestWithParam<Backend_Type_Params>
{
    BunnyParameters bunny_param;
public:
    videoio_bunny()
    {
        ext = get<0>(GetParam());
        apiPref = get<1>(GetParam());
        video_file = BunnyParameters::getFilename(String(".") + ext);
    }
    void doFrameCountTest()
    {
        if (!videoio_registry::hasBackend(apiPref))
            throw SkipTestException(cv::String("Backend is not available/disabled: ") + cv::videoio_registry::getBackendName(apiPref));
        VideoCapture cap;
        EXPECT_NO_THROW(cap.open(video_file, apiPref));
        if (!cap.isOpened())
        {
            std::cout << "SKIP test: backend " << apiPref << " can't open the video: " << video_file << std::endl;
            return;
        }

        Size actual;
        EXPECT_NO_THROW(actual = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
                                      (int)cap.get(CAP_PROP_FRAME_HEIGHT)));
        EXPECT_EQ(bunny_param.getWidth(), actual.width);
        EXPECT_EQ(bunny_param.getHeight(), actual.height);

        double fps_prop = 0;
        EXPECT_NO_THROW(fps_prop = cap.get(CAP_PROP_FPS));
        if (fps_prop > 0)
            EXPECT_NEAR(fps_prop, bunny_param.getFps(), 1);
        else
            std::cout << "FPS is not available. SKIP check." << std::endl;

        int count_prop = 0;
        EXPECT_NO_THROW(count_prop = (int)cap.get(CAP_PROP_FRAME_COUNT));
        // mpg file reports 5.08 sec * 24 fps => property returns 122 frames
        // but actual number of frames returned is 125
        if (ext != "mpg")
        {
            if (count_prop > 0)
            {
                EXPECT_EQ(bunny_param.getCount(), count_prop);
            }
        }

        int count_actual = 0;
        while (cap.isOpened())
        {
            Mat frame;
            EXPECT_NO_THROW(cap >> frame);
            if (frame.empty())
                break;
            EXPECT_EQ(bunny_param.getWidth(), frame.cols);
            EXPECT_EQ(bunny_param.getHeight(), frame.rows);
            count_actual += 1;
        }
        if (count_prop > 0)
        {
            EXPECT_NEAR(bunny_param.getCount(), count_actual, 1);
        }
        else
            std::cout << "Frames counter is not available. Actual frames: " << count_actual << ". SKIP check." << std::endl;
    }
};

//==================================================================================================

struct Ext_Fourcc_PSNR
{
    const char* ext;
    const char* fourcc;
    float PSNR;
    VideoCaptureAPIs api;
};
typedef tuple<Size, Ext_Fourcc_PSNR> Size_Ext_Fourcc_PSNR;

class videoio_synthetic : public Videoio_Test_Base, public testing::TestWithParam<Size_Ext_Fourcc_PSNR>
{
    Size frame_size;
    int fourcc;
    float PSNR_GT;
    int frame_count;
    double fps;
public:
    videoio_synthetic()
    {
        frame_size = get<0>(GetParam());
        const Ext_Fourcc_PSNR p = get<1>(GetParam());
        ext = p.ext;
        fourcc = fourccFromString(p.fourcc);
        PSNR_GT = p.PSNR;
        video_file = cv::tempfile((fourccToString(fourcc) + "." + ext).c_str());
        frame_count = 100;
        fps = 25.;
        apiPref = p.api;
    }
    void TearDown()
    {
        remove(video_file.c_str());
    }
    virtual void writeVideo()
    {
        Mat img(frame_size, CV_8UC3);
        VideoWriter writer;
        EXPECT_NO_THROW(writer.open(video_file, apiPref, fourcc, fps, frame_size, true));
        ASSERT_TRUE(writer.isOpened());
        for(int i = 0; i < frame_count; ++i )
        {
            generateFrame(i, frame_count, img);
            EXPECT_NO_THROW(writer << img);
        }
        EXPECT_NO_THROW(writer.release());
    }
    virtual void checkFrameContent(Mat & img, int idx)
    {
        Mat imgGT(frame_size, CV_8UC3);
        generateFrame(idx, frame_count, imgGT);
        double psnr = cvtest::PSNR(img, imgGT);
        ASSERT_GT(psnr, PSNR_GT) << "frame " << idx;
    }
    virtual void checkFrameCount(int &actual)
    {
        Range expected_frame_count = Range(frame_count, frame_count);

        // Hack! Newer FFmpeg versions in this combination produce a file
        // whose reported duration is one frame longer than needed, and so
        // the calculated frame count is also off by one. Ideally, we'd want
        // to fix both writing (to produce the correct duration) and reading
        // (to correctly report frame count for such files), but I don't know
        // how to do either, so this is a workaround for now.
        if (fourcc == VideoWriter::fourcc('M', 'P', 'E', 'G') && ext == "mkv")
            expected_frame_count.end += 1;

        // Workaround for some gstreamer pipelines
        if (apiPref == CAP_GSTREAMER)
            expected_frame_count.start -= 1;

        ASSERT_LE(expected_frame_count.start, actual);
        ASSERT_GE(expected_frame_count.end, actual);

        actual = expected_frame_count.start; // adjust actual frame boundary to possible minimum
    }
};

//==================================================================================================

static const VideoCaptureAPIs backend_params[] = {
#ifdef HAVE_AVFOUNDATION
   CAP_AVFOUNDATION,
#endif

#ifdef HAVE_MSMF
    CAP_MSMF,
#endif

    CAP_GSTREAMER,
    CAP_FFMPEG,

#ifdef HAVE_XINE
    CAP_XINE,
#endif

    CAP_OPENCV_MJPEG
    // CAP_INTEL_MFX
};

static const string bunny_params[] = {
    string("wmv"),
    string("mov"),
    string("mp4"),
    string("mpg"),
    string("avi"),
    string("h264"),
    string("h265"),
    string("mjpg.avi")
};

TEST_P(videoio_bunny, read_position) { doTest(); }

TEST_P(videoio_bunny, frame_count) { doFrameCountTest(); }

INSTANTIATE_TEST_CASE_P(videoio, videoio_bunny,
                          testing::Combine(
                              testing::ValuesIn(bunny_params),
                              testing::ValuesIn(backend_params)));


inline static std::ostream &operator<<(std::ostream &out, const Ext_Fourcc_PSNR &p)
{
    out << "FOURCC(" << p.fourcc << "), ." << p.ext << ", " << p.api << ", " << p.PSNR << "dB"; return out;
}

static Ext_Fourcc_PSNR synthetic_params[] = {

#ifdef HAVE_MSMF
#if !defined(_M_ARM)
    {"wmv", "WMV1", 30.f, CAP_MSMF},
    {"wmv", "WMV2", 30.f, CAP_MSMF},
#endif
    {"wmv", "WMV3", 30.f, CAP_MSMF},
    {"wmv", "WVC1", 30.f, CAP_MSMF},
    {"mov", "H264", 30.f, CAP_MSMF},
#endif

#ifdef HAVE_AVFOUNDATION
   {"mov", "H264", 30.f, CAP_AVFOUNDATION},
   {"mov", "MJPG", 30.f, CAP_AVFOUNDATION},
   {"mp4", "H264", 30.f, CAP_AVFOUNDATION},
   {"mp4", "MJPG", 30.f, CAP_AVFOUNDATION},
   {"m4v", "H264", 30.f, CAP_AVFOUNDATION},
   {"m4v", "MJPG", 30.f, CAP_AVFOUNDATION},
#endif

    {"avi", "XVID", 30.f, CAP_FFMPEG},
    {"avi", "MPEG", 30.f, CAP_FFMPEG},
    {"avi", "IYUV", 30.f, CAP_FFMPEG},
    {"avi", "MJPG", 30.f, CAP_FFMPEG},

    {"mkv", "XVID", 30.f, CAP_FFMPEG},
    {"mkv", "MPEG", 30.f, CAP_FFMPEG},
    {"mkv", "MJPG", 30.f, CAP_FFMPEG},

    {"avi", "MPEG", 30.f, CAP_GSTREAMER},
    {"avi", "MJPG", 30.f, CAP_GSTREAMER},
    {"avi", "H264", 30.f, CAP_GSTREAMER},

    {"mkv", "MPEG", 30.f, CAP_GSTREAMER},
    {"mkv", "MJPG", 30.f, CAP_GSTREAMER},
    {"mkv", "H264", 30.f, CAP_GSTREAMER},

    {"avi", "MJPG", 30.f, CAP_OPENCV_MJPEG},
};


Size all_sizes[] = {
    Size(640, 480),
    Size(976, 768)
};

TEST_P(videoio_synthetic, write_read_position) { doTest(); }

INSTANTIATE_TEST_CASE_P(videoio, videoio_synthetic,
                        testing::Combine(
                            testing::ValuesIn(all_sizes),
                            testing::ValuesIn(synthetic_params)));

struct Ext_Fourcc_API
{
    const char* ext;
    const char* fourcc;
    VideoCaptureAPIs api;
};

inline static std::ostream &operator<<(std::ostream &out, const Ext_Fourcc_API &p)
{
    out << "(FOURCC(" << p.fourcc << "), \"" << p.ext << "\", " << p.api << ")"; return out;
}


class Videoio_Writer : public Videoio_Test_Base, public testing::TestWithParam<Ext_Fourcc_API>
{
protected:
    Size frame_size;
    int fourcc;
    double fps;
public:
    Videoio_Writer()
    {
        frame_size = Size(640, 480);
        const Ext_Fourcc_API p = GetParam();
        ext = p.ext;
        fourcc = fourccFromString(p.fourcc);
        if (ext.size() == 3)
            video_file = cv::tempfile((fourccToString(fourcc) + "." + ext).c_str());
        else
            video_file = ext;
        fps = 25.;
        apiPref = p.api;
    }
    void SetUp()
    {
    }
    void TearDown()
    {
        if (ext.size() == 3)
            (void)remove(video_file.c_str());
    }
};

TEST_P(Videoio_Writer, write_nothing)
{
    if (!cv::videoio_registry::hasBackend(apiPref))
        throw SkipTestException(cv::String("Backend is not available/disabled: ") + cv::videoio_registry::getBackendName(apiPref));

    VideoWriter writer;
    EXPECT_NO_THROW(writer.open(video_file, apiPref, fourcc, fps, frame_size, true));
    ASSERT_TRUE(writer.isOpened());
#if 0  // no frames
    cv::Mat m(frame_size, CV_8UC3, Scalar::all(127));
    writer << m;
#endif
    EXPECT_NO_THROW(writer.release());
}

static vector<Ext_Fourcc_API> generate_Ext_Fourcc_API()
{
    const size_t N = sizeof(synthetic_params)/sizeof(synthetic_params[0]);
    vector<Ext_Fourcc_API> result; result.reserve(N);
    for (size_t i = 0; i < N; i++)
    {
        const Ext_Fourcc_PSNR& src = synthetic_params[i];
        Ext_Fourcc_API e = { src.ext, src.fourcc, src.api };
        result.push_back(e);
    }

    {
        Ext_Fourcc_API e = { "appsrc ! videoconvert ! video/x-raw, format=(string)NV12 ! filesink location=test.nv12", "\0\0\0\0", CAP_GSTREAMER };
        result.push_back(e);
    }
    {
        Ext_Fourcc_API e = { "appsrc ! videoconvert ! video/x-raw, format=(string)I420 ! matroskamux ! filesink location=test.mkv", "\0\0\0\0", CAP_GSTREAMER };
        result.push_back(e);
    }
    return result;
}

INSTANTIATE_TEST_CASE_P(videoio, Videoio_Writer, testing::ValuesIn(generate_Ext_Fourcc_API()));


TEST(Videoio, exceptions)
{
    VideoCapture cap;

    Mat mat;

    EXPECT_FALSE(cap.grab());
    EXPECT_FALSE(cap.retrieve(mat));
    EXPECT_FALSE(cap.set(CAP_PROP_POS_FRAMES, 1));
    EXPECT_FALSE(cap.open("this_does_not_exist.avi", CAP_OPENCV_MJPEG));

    cap.setExceptionMode(true);

    EXPECT_THROW(cap.grab(), Exception);
    EXPECT_THROW(cap.retrieve(mat), Exception);
    EXPECT_THROW(cap.set(CAP_PROP_POS_FRAMES, 1), Exception);
    EXPECT_THROW(cap.open("this_does_not_exist.avi", CAP_OPENCV_MJPEG), Exception);
}


typedef Videoio_Writer Videoio_Writer_bad_fourcc;

TEST_P(Videoio_Writer_bad_fourcc, nocrash)
{
    if (!isBackendAvailable(apiPref, cv::videoio_registry::getStreamBackends()))
        throw SkipTestException(cv::String("Backend is not available/disabled: ") + cv::videoio_registry::getBackendName(apiPref));

    VideoWriter writer;
    EXPECT_NO_THROW(writer.open(video_file, apiPref, fourcc, fps, frame_size, true));
    ASSERT_FALSE(writer.isOpened());
    EXPECT_NO_THROW(writer.release());
}

static vector<Ext_Fourcc_API> generate_Ext_Fourcc_API_nocrash()
{
    static const Ext_Fourcc_API params[] = {
#ifdef HAVE_MSMF_DISABLED  // MSMF opens writer stream
    {"wmv", "aaaa", CAP_MSMF},
    {"mov", "aaaa", CAP_MSMF},
#endif

#ifdef HAVE_QUICKTIME
    {"mov", "aaaa", CAP_QT},
    {"avi", "aaaa", CAP_QT},
    {"mkv", "aaaa", CAP_QT},
#endif

#ifdef HAVE_AVFOUNDATION
   {"mov", "aaaa", CAP_AVFOUNDATION},
   {"mp4", "aaaa", CAP_AVFOUNDATION},
   {"m4v", "aaaa", CAP_AVFOUNDATION},
#endif

#ifdef HAVE_FFMPEG
    {"avi", "aaaa", CAP_FFMPEG},
    {"mkv", "aaaa", CAP_FFMPEG},
#endif

#ifdef HAVE_GSTREAMER
    {"avi", "aaaa", CAP_GSTREAMER},
    {"mkv", "aaaa", CAP_GSTREAMER},
#endif
    {"avi", "aaaa", CAP_OPENCV_MJPEG},
};

    const size_t N = sizeof(params)/sizeof(params[0]);
    vector<Ext_Fourcc_API> result; result.reserve(N);
    for (size_t i = 0; i < N; i++)
    {
        const Ext_Fourcc_API& src = params[i];
        Ext_Fourcc_API e = { src.ext, src.fourcc, src.api };
        result.push_back(e);
    }
    return result;
}

INSTANTIATE_TEST_CASE_P(videoio, Videoio_Writer_bad_fourcc, testing::ValuesIn(generate_Ext_Fourcc_API_nocrash()));

} // namespace
