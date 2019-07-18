#ifndef OPENALPR_ENDTOENDTEST_H
#define OPENALPR_ENDTOENDTEST_H


#include BOSS_OPENCV_U_opencv2__highgui__highgui_hpp //original-code:"opencv2/highgui/highgui.hpp"
#include BOSS_OPENCV_U_opencv2__imgproc__imgproc_hpp //original-code:"opencv2/imgproc/imgproc.hpp"
#include "detection/detector_types.h"
#include BOSS_OPENALPR_U_prewarp_h //original-code:"prewarp.h"
#include "alpr_impl.h"
#include "benchmark_utils.h"

class EndToEndTest
{
  public:
    EndToEndTest(std::string inputDir, std::string outputDir);
    void runTest(std::string country, std::vector<std::string> files);
  
  private:
    
    bool rectMatches(cv::Rect actualPlate, alpr::PlateRegion candidate);
    int totalRectCount(alpr::PlateRegion rootCandidate);
	
    std::string inputDir;
    std::string outputDir;
  
  
};

class EndToEndBenchmarkResult {
  public:
    EndToEndBenchmarkResult()
    {
    this->imageName = "";
    this->detectedPlate = false;
    this->topResultCorrect = false;
    this->top10ResultCorrect = false;
    this->detectionFalsePositives = 0;
    this->resultsFalsePositives = 0;
    }
    
    std::string imageName;
    bool detectedPlate;
    bool topResultCorrect;
    bool top10ResultCorrect;
    int detectionFalsePositives;
    int resultsFalsePositives;
};

#endif	//OPENALPR_ENDTOENDTEST_H