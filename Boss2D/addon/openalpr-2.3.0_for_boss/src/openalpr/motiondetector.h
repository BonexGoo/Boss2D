
#ifndef OPENALPR_MOTIONDETECTOR_H
#define OPENALPR_MOTIONDETECTOR_H

#include BOSS_OPENCV_U_opencv2__opencv_hpp //original-code:"opencv2/opencv.hpp"
#include BOSS_OPENALPR_U_utility_h //original-code:"utility.h"

namespace alpr
{
  class MotionDetector
  {
      private: cv::Ptr<cv::BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
      private: cv::Mat fgMaskMOG2;
      public:
          MotionDetector();
          virtual ~MotionDetector();

          void ResetMotionDetection(cv::Mat* frame);
          cv::Rect MotionDetect(cv::Mat* frame);
  };
}

#endif // OPENALPR_MOTIONDETECTOR_H