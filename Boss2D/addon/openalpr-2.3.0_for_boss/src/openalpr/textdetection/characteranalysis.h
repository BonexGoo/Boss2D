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

#ifndef OPENALPR_CHARACTERANALYSIS_H
#define OPENALPR_CHARACTERANALYSIS_H

#include <algorithm>
#include BOSS_OPENCV_U_opencv2__imgproc__imgproc_hpp //original-code:"opencv2/imgproc/imgproc.hpp"
#include BOSS_OPENALPR_U_utility_h //original-code:"utility.h"
#include BOSS_OPENALPR_U_config_h //original-code:"config.h"
#include BOSS_OPENALPR_U_pipeline_data_h //original-code:"pipeline_data.h"
#include "textcontours.h"
#include "platemask.h"
#include "linefinder.h"

namespace alpr
{

  class CharacterAnalysis
  {

    public:
      CharacterAnalysis(PipelineData* pipeline_data);
      virtual ~CharacterAnalysis();

      cv::Mat bestThreshold;

      TextContours bestContours;


      std::vector<TextContours> allTextContours;

      void analyze();

      cv::Mat getCharacterMask();

    private:
      PipelineData* pipeline_data;
      Config* config;

      bool isPlateInverted();
      void filter(cv::Mat img, TextContours& textContours);

      void filterByBoxSize(TextContours& textContours, int minHeightPx, int maxHeightPx);
      void filterByParentContour( TextContours& textContours );
      void filterContourHoles(TextContours& textContours);
      void filterByOuterMask(TextContours& textContours);

      std::vector<cv::Point> getCharArea(LineSegment topLine, LineSegment bottomLine);
      void filterBetweenLines(cv::Mat img, TextContours& textContours, std::vector<TextLine> textLines );


  };

}

#endif // OPENALPR_CHARACTERANALYSIS_H
