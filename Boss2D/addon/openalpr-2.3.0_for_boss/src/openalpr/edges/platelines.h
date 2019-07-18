/*
 * Copyright (c) 2015 OpenALPR Technology, Inc.
 * Open source Automated License Plate Recognition [http://www.openalpr.com]
 *
 * This file is part of OpenALPR.
 *
 * OpenALPR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License
 * version 3 as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENALPR_PLATELINES_H
#define OPENALPR_PLATELINES_H

#include BOSS_OPENCV_U_opencv2__highgui__highgui_hpp //original-code:"opencv2/highgui/highgui.hpp"
#include BOSS_OPENCV_U_opencv2__imgproc__imgproc_hpp //original-code:"opencv2/imgproc/imgproc.hpp"
#include BOSS_OPENALPR_U_utility_h //original-code:"utility.h"
#include BOSS_OPENALPR_U_binarize_wolf_h //original-code:"binarize_wolf.h"
#include BOSS_OPENALPR_U_config_h //original-code:"config.h"
#include BOSS_OPENALPR_U_pipeline_data_h //original-code:"pipeline_data.h"

namespace alpr
{

  struct PlateLine
  {
    LineSegment line;
    float confidence;
  };

  class PlateLines
  {

    public:
      PlateLines(PipelineData* pipelineData);
      virtual ~PlateLines();

      void processImage(cv::Mat img, std::vector<TextLine> textLines, float sensitivity=1.0);

      std::vector<PlateLine> horizontalLines;
      std::vector<PlateLine> verticalLines;

      std::vector<cv::Point> winningCorners;

    private:

      PipelineData* pipelineData;
      bool debug;

      cv::Mat customGrayscaleConversion(cv::Mat src);
      void findLines(cv::Mat inputImage);
      std::vector<PlateLine> getLines(cv::Mat edges, float sensitivityMultiplier, bool vertical);
  };

}

#endif // OPENALPR_PLATELINES_H
