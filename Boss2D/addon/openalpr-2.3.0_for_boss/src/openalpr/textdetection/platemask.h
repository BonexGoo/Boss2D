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

#ifndef OPENALPR_PLATEMASK_H
#define	OPENALPR_PLATEMASK_H

#include BOSS_OPENCV_U_opencv2__imgproc__imgproc_hpp //original-code:"opencv2/imgproc/imgproc.hpp"
#include BOSS_OPENALPR_U_pipeline_data_h //original-code:"pipeline_data.h"
#include "textcontours.h"

namespace alpr
{

  class PlateMask {
  public:
    PlateMask(PipelineData* pipeline_data);
    virtual ~PlateMask();

    bool hasPlateMask;

    cv::Mat getMask();

    void findOuterBoxMask(std::vector<TextContours > contours);

  private:

    PipelineData* pipeline_data;
    cv::Mat plateMask;


  };

}
#endif	/* OPENALPR_PLATEMASK_H */

