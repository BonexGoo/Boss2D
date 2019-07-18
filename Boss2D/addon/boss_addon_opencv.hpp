#pragma once
#include <boss.h>

#if !BOSS_WINDOWS
    #define _CTYPE_U 0x01
    #define _CTYPE_L 0x02
    #define _CTYPE_D 0x04
    #define _CTYPE_S 0x08
    #define _CTYPE_P 0x10
    #define _CTYPE_C 0x20
    #define _CTYPE_X 0x40
    #define _CTYPE_B 0x80
    #define _CTYPE_R (_CTYPE_P|_CTYPE_U|_CTYPE_L|_CTYPE_D|_CTYPE_B)
    #define _CTYPE_A (_CTYPE_L|_CTYPE_U)
    #define _CTYPE_N _CTYPE_D
    #define _U _CTYPE_U
    #define _L _CTYPE_L
    #define _N _CTYPE_N
    #define _S _CTYPE_S
    #define _P _CTYPE_P
    #define _C _CTYPE_C
    #define _X _CTYPE_X
    #define _B _CTYPE_B
#endif

#include "boss_integration_opencv-3.1.0.h"
#include <addon/opencv-3.1.0_for_boss/include/opencv2/opencv.hpp>
extern "C"
{
    #include <addon/opencv-3.1.0_for_boss/include/opencv/cv.h>
    #include <addon/opencv-3.1.0_for_boss/include/opencv/highgui.h>
}

#include <boss.hpp>
#include <element/boss_point.hpp>
#include <element/boss_rect.hpp>

class CVObject
{
public:
    CVObject();
    ~CVObject();

public:
    // MOG2
    bool mEnableMOG2;
    sint32 mOldHistory;
    double mOldThreshold;
    bool mOldShadows;
    cv::Mat mMOG2Mask;
    cv::Mat mMOG2Image;
    cv::Ptr<cv::BackgroundSubtractor> mMOG2;
    // Canny
    bool mEnableCanny;
    double mLow;
    double mHigh;
    sint32 mAperture;
    cv::Mat mCannyImage;

    // Result
    cv::Mat mGrayImage;
    cv::Mat* mResult;
};
