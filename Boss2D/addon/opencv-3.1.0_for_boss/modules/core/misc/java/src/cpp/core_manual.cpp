#define LOG_TAG "org.opencv.core.Core"
#include "common.h"
#include "core_manual.hpp"
#include BOSS_OPENCV_U_opencv2__core__utility_hpp //original-code:"opencv2/core/utility.hpp"

static int quietCallback( int, const char*, const char*, const char*, int, void* )
{
    return 0;
}

namespace cv {

void setErrorVerbosity(bool verbose)
{
    if(verbose)
        cv::redirectError(0);
    else
        cv::redirectError((cv::ErrorCallback)quietCallback);
}

}
