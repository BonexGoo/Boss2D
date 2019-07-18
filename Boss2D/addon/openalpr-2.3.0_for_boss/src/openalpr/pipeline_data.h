
#ifndef OPENALPR_PIPELINEDATA_H
#define OPENALPR_PIPELINEDATA_H

#include BOSS_OPENCV_U_opencv2__imgproc__imgproc_hpp //original-code:"opencv2/imgproc/imgproc.hpp"
#include BOSS_OPENALPR_U_utility_h //original-code:"utility.h"
#include BOSS_OPENALPR_U_config_h //original-code:"config.h"
#include "textdetection/textline.h"
#include "edges/scorekeeper.h"
#include BOSS_OPENALPR_U_prewarp_h //original-code:"prewarp.h"

namespace alpr
{

  class PipelineData
  {

    public:
      PipelineData(cv::Mat colorImage, cv::Rect regionOfInterest, Config* config);
      PipelineData(cv::Mat colorImage, cv::Mat grayImage, cv::Rect regionOfInterest, Config* config);
      virtual ~PipelineData();

      void init(cv::Mat colorImage, cv::Mat grayImage, cv::Rect regionOfInterest, Config* config);
      void clearThresholds();

      // Inputs
      Config* config;

      PreWarp* prewarp;

      cv::Mat colorImg;
      cv::Mat grayImg;
      cv::Rect regionOfInterest;

      bool isMultiline;

      cv::Mat crop_gray;

      cv::Mat color_deskewed;

      bool hasPlateBorder;
      cv::Mat plateBorderMask;    
      std::vector<TextLine> textLines;

      std::vector<cv::Mat> thresholds;

      std::vector<cv::Point2f> plate_corners;


      // Outputs
      bool plate_inverted;

      std::string region_code;
      float region_confidence;

      bool disqualified;
      std::string disqualify_reason;
      
      ScoreKeeper confidence_weights;

      // Boxes around characters in cropped image
      // Each row in a multiline plate is an entry in the vector
      std::vector<std::vector<cv::Rect> > charRegions;
      
      // Same data, just not broken down by line
      std::vector<cv::Rect> charRegionsFlat;




      // OCR

  };

}

#endif // OPENALPR_PIPELINEDATA_H