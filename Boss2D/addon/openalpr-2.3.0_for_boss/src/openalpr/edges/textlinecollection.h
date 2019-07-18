/* 
 * File:   textlinecollection.h
 * Author: mhill
 *
 * Created on October 25, 2014, 4:06 PM
 */

#ifndef OPENALPR_TEXTLINECOLLECTION_H
#define	OPENALPR_TEXTLINECOLLECTION_H

#include BOSS_OPENALPR_U_utility_h //original-code:"utility.h"

#include BOSS_OPENCV_U_opencv2__imgproc__imgproc_hpp //original-code:"opencv2/imgproc/imgproc.hpp"
#include BOSS_OPENALPR_U_textdetection__textline_h //original-code:"textdetection/textline.h"

namespace alpr
{

  class TextLineCollection
  {
  public:
    TextLineCollection(std::vector<TextLine> textLines);

    int isLeftOfText(LineSegment line);
    int isAboveText(LineSegment line);

    LineSegment centerHorizontalLine;
    LineSegment centerVerticalLine;

    LineSegment longerSegment;
    LineSegment shorterSegment;

    float charHeight;
    float charAngle;

    cv::Mat getDebugImage(cv::Size imageSize);

  private:

    LineSegment topCharArea;
    LineSegment bottomCharArea;


    cv::Mat textMask;

    void findCenterHorizontal();
    void findCenterVertical();
  };

}

#endif	/* OPENALPR_TEXTLINECOLLECTION_H */

