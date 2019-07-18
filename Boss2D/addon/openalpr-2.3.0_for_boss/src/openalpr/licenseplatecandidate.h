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

#ifndef OPENALPR_LICENSEPLATECANDIDATE_H
#define OPENALPR_LICENSEPLATECANDIDATE_H

#include <iostream>
#include <stdio.h>
#include <vector>

#include BOSS_OPENCV_U_opencv2__imgproc__imgproc_hpp //original-code:"opencv2/imgproc/imgproc.hpp"
#include BOSS_OPENCV_U_opencv2__core__core_hpp //original-code:"opencv2/core/core.hpp"

#include BOSS_OPENALPR_U_utility_h //original-code:"utility.h"
#include BOSS_OPENALPR_U_constants_h //original-code:"constants.h"
#include "edges/platelines.h"
#include "transformation.h"
#include "textdetection/characteranalysis.h"
#include "edges/platecorners.h"
#include BOSS_OPENALPR_U_config_h //original-code:"config.h"
#include BOSS_OPENALPR_U_pipeline_data_h //original-code:"pipeline_data.h"

namespace alpr
{

  class LicensePlateCandidate
  {

    public:
      LicensePlateCandidate(PipelineData* pipeline_data);
      virtual ~LicensePlateCandidate();


      void recognize();


    private:
      PipelineData* pipeline_data;
      Config* config;


      cv::Mat filterByCharacterHue(std::vector<std::vector<cv::Point> > charRegionContours);
      std::vector<cv::Point> findPlateCorners(cv::Mat inputImage, PlateLines plateLines, CharacterAnalysis textAnalysis);	// top-left, top-right, bottom-right, bottom-left

      cv::Size getCropSize(std::vector<cv::Point2f> areaCorners);

  };
  
}
#endif // OPENALPR_LICENSEPLATECANDIDATE_H
