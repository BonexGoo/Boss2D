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

#ifndef OPENALPR_TESSERACTOCR_H
#define OPENALPR_TESSERACTOCR_H

#include <iostream>
#include <stdio.h>

#include BOSS_OPENALPR_U_utility_h //original-code:"utility.h"
#include BOSS_OPENALPR_U_config_h //original-code:"config.h"
#include BOSS_OPENALPR_U_pipeline_data_h //original-code:"pipeline_data.h"

#include BOSS_OPENALPR_U_constants_h //original-code:"constants.h"
#include BOSS_OPENCV_U_opencv2__imgproc__imgproc_hpp //original-code:"opencv2/imgproc/imgproc.hpp"
#include BOSS_OPENALPR_U_support__filesystem_h //original-code:"support/filesystem.h"
#include BOSS_OPENALPR_U_support__version_h //original-code:"support/version.h"

#include "ocr.h"
#include BOSS_OPENALPR_U_tesseract__baseapi_h //original-code:"tesseract/baseapi.h"

namespace alpr
{

  class TesseractOcr : public OCR 
  {

    public:
      TesseractOcr(Config* config);
      virtual ~TesseractOcr();



    private:

      std::vector<OcrChar> recognize_line(int line_index, PipelineData* pipeline_data);
      void segment(PipelineData* pipeline_data);
    
      tesseract::TessBaseAPI tesseract;

  };

}

#endif // OPENALPR_TESSERACTOCR_H
