#include <boss.h>

#if !defined(BOSS_NEED_ADDON_OPENCV) || (BOSS_NEED_ADDON_OPENCV != 0 && BOSS_NEED_ADDON_OPENCV != 1)
    #error BOSS_NEED_ADDON_OPENCV macro is invalid use
#endif
bool __LINK_ADDON_OPENCV__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_OPENCV

#include "boss_addon_opencv.hpp"

#include <boss.hpp>
#include <format/boss_bmp.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(OpenCV, Create, id_opencv, void)
    BOSS_DECLARE_ADDON_FUNCTION(OpenCV, Release, void, id_opencv)
    BOSS_DECLARE_ADDON_FUNCTION(OpenCV, SetMOG2, void, id_opencv, bool, sint32, double, bool)
    BOSS_DECLARE_ADDON_FUNCTION(OpenCV, SetCanny, void, id_opencv, bool, double, double, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(OpenCV, Update, void, id_opencv, id_bitmap_read)
    BOSS_DECLARE_ADDON_FUNCTION(OpenCV, GetUpdatedImage, id_bitmap, id_opencv)
    BOSS_DECLARE_ADDON_FUNCTION(OpenCV, GetFindContours, void, id_opencv, AddOn::OpenCV::FindContoursCB, payload)
    BOSS_DECLARE_ADDON_FUNCTION(OpenCV, GetHoughLines, void, id_opencv, AddOn::OpenCV::HoughLinesCB, payload)
    BOSS_DECLARE_ADDON_FUNCTION(OpenCV, GetHoughCircles, void, id_opencv, AddOn::OpenCV::HoughCirclesCB, payload)

    static autorun Bind_AddOn_OpenCV()
    {
        Core_AddOn_OpenCV_Create() = Customized_AddOn_OpenCV_Create;
        Core_AddOn_OpenCV_Release() = Customized_AddOn_OpenCV_Release;
        Core_AddOn_OpenCV_SetMOG2() = Customized_AddOn_OpenCV_SetMOG2;
        Core_AddOn_OpenCV_SetCanny() = Customized_AddOn_OpenCV_SetCanny;
        Core_AddOn_OpenCV_Update() = Customized_AddOn_OpenCV_Update;
        Core_AddOn_OpenCV_GetUpdatedImage() = Customized_AddOn_OpenCV_GetUpdatedImage;
        Core_AddOn_OpenCV_GetFindContours() = Customized_AddOn_OpenCV_GetFindContours;
        Core_AddOn_OpenCV_GetHoughLines() = Customized_AddOn_OpenCV_GetHoughLines;
        Core_AddOn_OpenCV_GetHoughCircles() = Customized_AddOn_OpenCV_GetHoughCircles;
        return true;
    }
    static autorun _ = Bind_AddOn_OpenCV();
}

// 구현부
namespace BOSS
{
    id_opencv Customized_AddOn_OpenCV_Create(void)
    {
        CVObject* NewCV = new CVObject();
        return (id_opencv) NewCV;
    }

    void Customized_AddOn_OpenCV_Release(id_opencv opencv)
    {
        CVObject* CurCV = (CVObject*) opencv;
        delete CurCV;
    }

    void Customized_AddOn_OpenCV_SetMOG2(id_opencv opencv, bool enable, sint32 history, double threshold, bool shadows)
    {
        if(!opencv) return;
        CVObject* CurCV = (CVObject*) opencv;
        CurCV->mEnableMOG2 = enable;
        if(enable && (
            CurCV->mOldHistory != history ||
            CurCV->mOldThreshold != threshold ||
            CurCV->mOldShadows != shadows))
        {
            CurCV->mOldHistory = history;
            CurCV->mOldThreshold = threshold;
            CurCV->mOldShadows = shadows;
            CurCV->mMOG2 = cv::createBackgroundSubtractorMOG2(history, threshold, shadows);
        }
    }

    void Customized_AddOn_OpenCV_SetCanny(id_opencv opencv, bool enable, double low, double high, sint32 aperture)
    {
        if(!opencv) return;
        CVObject* CurCV = (CVObject*) opencv;
        CurCV->mEnableCanny = enable;
        CurCV->mLow = low;
        CurCV->mHigh = high;
        CurCV->mAperture = aperture;
    }

    void Customized_AddOn_OpenCV_Update(id_opencv opencv, id_bitmap_read bmp)
    {
        if(!opencv) return;
        CVObject* CurCV = (CVObject*) opencv;
        BOSS_ASSERT("본 함수는 32비트 비트맵만 지원합니다", Bmp::GetBitCount(bmp) == 32);

        bytes BmpBits = (bytes) Bmp::GetBits(bmp);
        const sint32 BmpWidth = Bmp::GetWidth(bmp);
        const sint32 BmpHeight = Bmp::GetHeight(bmp);
        cv::Mat OneImage(BmpHeight, BmpWidth, CV_8UC4, (void*) Bmp::GetBits(bmp));

        // 블러와 흑백화
        cv::cvtColor(OneImage, CurCV->mGrayImage, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(CurCV->mGrayImage, CurCV->mGrayImage, cv::Size(9, 9), 2, 2);
        CurCV->mResult = &CurCV->mGrayImage;

        // 배경축출
        if(CurCV->mEnableMOG2)
        {
            CurCV->mMOG2->apply(*CurCV->mResult, CurCV->mMOG2Mask);
            CurCV->mMOG2Image.convertTo(CurCV->mMOG2Image, -1, 1, -100);
            CurCV->mResult->copyTo(CurCV->mMOG2Image, CurCV->mMOG2Mask);
            CurCV->mResult = &CurCV->mMOG2Image;
        }

        // 캐니엣지화
        if(CurCV->mEnableCanny)
        {
            cv::Canny(*CurCV->mResult, CurCV->mCannyImage, CurCV->mLow, CurCV->mHigh, CurCV->mAperture);
            CurCV->mResult = &CurCV->mCannyImage;
        }
    }

    id_bitmap Customized_AddOn_OpenCV_GetUpdatedImage(id_opencv opencv)
    {
        if(!opencv) return nullptr;
        CVObject* CurCV = (CVObject*) opencv;
        const cv::Mat& CurImage = *CurCV->mResult;

        auto NewBitmap = Bmp::Create(4, CurImage.cols, CurImage.rows);
        auto NewBitmapBits = (Bmp::bitmappixel*) Bmp::GetBits(NewBitmap);
        for(sint32 y = 0, yend = CurImage.rows; y < yend; ++y)
        {
            bytes Src = CurImage.data + CurImage.cols * y;
            auto Dst = &NewBitmapBits[CurImage.cols * y];
            for(sint32 x = 0, xend = CurImage.cols; x < xend; ++x)
            {
                Dst->b = *Src;
                Dst->g = *Src;
                Dst->r = *Src;
                Dst->a = 0xFF;
                Dst++; Src++;
            }
        }
        return NewBitmap;
    }

    void Customized_AddOn_OpenCV_GetFindContours(id_opencv opencv, AddOn::OpenCV::FindContoursCB cb, payload data)
    {
        if(!opencv) return;
        CVObject* CurCV = (CVObject*) opencv;

        // 폐쇄도형수집
        std::vector< std::vector<cv::Point> > Contours;
        cv::findContours(*CurCV->mResult, Contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        for(size_t i = 0, iend = Contours.size(); i < iend; ++i)
        {
            const auto& CurContour = Contours[i];
            cb(CurContour.size(), (point64*) &CurContour[0], data);
        }
    }

    void Customized_AddOn_OpenCV_GetHoughLines(id_opencv opencv, AddOn::OpenCV::HoughLinesCB cb, payload data)
    {
        if(!opencv) return;
        CVObject* CurCV = (CVObject*) opencv;

        const sint32 LineMinSize = Math::Min(CurCV->mResult->cols, CurCV->mResult->rows) / 8;
        const sint32 LineMaxGap = LineMinSize / 10;

        // 허프식 선분감지
        std::vector< cv::Vec4f > Lines;
        sint32 Threshold = 100;
        cv::HoughLinesP(*CurCV->mResult, Lines, 1, Math::ToRadian(1), Threshold, LineMinSize, LineMaxGap);

        for(size_t i = 0, iend = Lines.size(); i < iend; ++i)
        {
            const auto& CurLine = Lines[i];
            cb(Point(CurLine[0], CurLine[1]), Point(CurLine[2], CurLine[3]), data);
        }
    }

    void Customized_AddOn_OpenCV_GetHoughCircles(id_opencv opencv, AddOn::OpenCV::HoughCirclesCB cb, payload data)
    {
        if(!opencv) return;
        CVObject* CurCV = (CVObject*) opencv;

        // 허프식 원검출
        std::vector< cv::Vec3f > Circles;
        const double DP = 2; // 해상도의 반감계수(1은 100%, 2는 50%)
        const double MinDist = Math::Min(CurCV->mResult->cols, CurCV->mResult->rows) / 10; // 감지된 원과 원간의 최소간격
        const double Param1 = 200; // CV_HOUGH_GRADIENT의 경우, Canny 엣지 검출기에 전달 된 두 개의 상위 임계값
        const double Param2 = 80; // CV_HOUGH_GRADIENT의 경우, 원 중심에 대한 임계 값, 크기가 작을수록 더 많은 거짓 서클이 감지
        cv::HoughCircles(*CurCV->mResult, Circles, CV_HOUGH_GRADIENT, DP, MinDist, Param1, Param2, 0, 0);

        for(size_t i = 0, iend = Circles.size(); i < iend; ++i)
        {
            const auto& CurCircle = Circles[i];
            cb(Point(CurCircle[0], CurCV->mResult->rows - CurCircle[1]), CurCircle[2], data);
        }
    }
}

CVObject::CVObject()
{
    mEnableMOG2 = false;
    mOldHistory = 0;
    mOldThreshold = 0;
    mOldShadows = false;
    mEnableCanny = false;
    mLow = 0;
    mHigh = 0;
    mAperture = 0;
    mResult = &mGrayImage;
}

CVObject::~CVObject()
{
}

#endif
