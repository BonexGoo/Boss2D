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

#include "ocr.h"

namespace alpr
{
  
  OCR::OCR(Config* config) : postProcessor(config) {
    this->config = config;
  }


  OCR::~OCR() {
  }

  
  void OCR::performOCR(PipelineData* pipeline_data)
  {
	  BOSS_TRACE("@@@@@ ocr-1", false);
    timespec startTime;
    getTimeMonotonic(&startTime);

	BOSS_TRACE("@@@@@ ocr-2", false);
    segment(pipeline_data);
    
	BOSS_TRACE("@@@@@ ocr-3", false);
    postProcessor.clear();

	BOSS_TRACE("@@@@@ ocr-4", false);
    int absolute_charpos = 0;
	int charlength = 0; // BOSS: 라인개념 대신 라스트인덱스개념으로 변경
    for (unsigned int line_idx = 0; line_idx < pipeline_data->textLines.size(); line_idx++)
    {
		BOSS_TRACE("@@@@@ ocr-5", false);
      std::vector<OcrChar> chars = recognize_line(line_idx, pipeline_data);
      
	  BOSS_TRACE("@@@@@ ocr-6", false);
      for (uint32_t i = 0; i < chars.size(); i++)
	  {
		postProcessor.addLetter(chars[i].letter, line_idx, charlength + chars[i].char_index, chars[i].confidence);
        // BOSS: 윗줄의 원본(postProcessor.addLetter(chars[i].letter, line_idx, chars[i].char_index, chars[i].confidence);)
	  }
	  BOSS_TRACE("@@@@@ ocr-7", false);
	  charlength += pipeline_data->charRegions[line_idx].size(); // BOSS: 라인개념 대신 라스트인덱스개념으로 변경
    }
    
	BOSS_TRACE("@@@@@ ocr-8", false);
    if (config->debugTiming)
    {
      timespec endTime;
      getTimeMonotonic(&endTime);
      std::cout << "OCR Time: " << diffclock(startTime, endTime) << "ms." << std::endl;
    }
	BOSS_TRACE("@@@@@ ocr-9", false);
  }
}