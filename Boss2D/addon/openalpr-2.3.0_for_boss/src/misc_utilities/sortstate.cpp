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

#include BOSS_OPENCV_U_opencv2__highgui__highgui_hpp //original-code:"opencv2/highgui/highgui.hpp"
#include BOSS_OPENCV_U_opencv2__imgproc__imgproc_hpp //original-code:"opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

#include "licenseplatecandidate.h"
#include "../statedetection/state_detector.h"
#include BOSS_OPENALPR_U_utility_h //original-code:"utility.h"
#include BOSS_OPENALPR_U_support__filesystem_h //original-code:"support/filesystem.h"

using namespace std;
using namespace cv;
using namespace alpr;

// Given a directory full of pre-cropped images, identify the state that each image belongs to.
// This is used to sort our own positive image database as a first step before grabbing characters to use to train the OCR.

bool detectPlate( StateDetector* identifier, Mat frame);

int main( int argc, const char** argv )
{
  #ifndef SKIP_STATE_DETECTION

  string inDir;
  string outDir;
  Mat frame;

  //Check if user specify image to process
  if(argc == 3 )
  {
    inDir = argv[1];
    outDir =  argv[2];
    outDir = outDir + "/";
  }
  else
  {
    printf("Use:\n\t%s directory \n",argv[0]);
    printf("Ex: \n\t%s ./pics/   \n",argv[0]);
    return 0;
  }

  Config config("us");
  StateDetector identifier(config.country, config.config_file_path, config.runtimeBaseDir);

  if (DirectoryExists(outDir.c_str()) == false)
  {
    printf("Output dir does not exist\n");
    return 0;
  }

  if (DirectoryExists(inDir.c_str()))
  {
    vector<string> files = getFilesInDir(inDir.c_str());

    for (int i = 0; i< files.size(); i++)
    {
      if (hasEnding(files[i], ".png"))
      {
        string fullpath = inDir + "/" + files[i];
        cout << fullpath << endl;
        frame = imread( fullpath.c_str() );


        vector<StateCandidate> candidates = identifier.detect(frame.data, frame.elemSize(), frame.cols, frame.rows);

        if (candidates.size() > 0)
        {
          cout << candidates[0].confidence << " : " << candidates[0].state_code;

          ostringstream convert;   // stream used for the conversion
          convert << i;      // insert the textual representation of 'Number' in the characters in the stream

          string copyCommand = "cp \"" + fullpath + "\" " + outDir + candidates[0].state_code + convert.str() + ".png";
          system( copyCommand.c_str() );
          waitKey(50);
          //while ((char) waitKey(50) != 'c') { }
        }
      }
    }
  }
  #endif

  return 1;
}

bool detectPlate( StateDetector* identifier, Mat frame);
