/*
* decolor.cpp
*
* Author:
* Siddharth Kherada <siddharthkherada27[at]gmail[dot]com>
*
* This tutorial demonstrates how to use OpenCV Decolorization Module.
*
* Input:
* Color Image
*
* Output:
* 1) Grayscale image
* 2) Color boost image
*
*/

#include BOSS_OPENCV_U_opencv2__photo_hpp //original-code:"opencv2/photo.hpp"
#include BOSS_OPENCV_U_opencv2__imgproc_hpp //original-code:"opencv2/imgproc.hpp"
#include BOSS_OPENCV_U_opencv2__highgui_hpp //original-code:"opencv2/highgui.hpp"
#include BOSS_OPENCV_U_opencv2__core_hpp //original-code:"opencv2/core.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    CV_Assert(argc == 2);
    Mat I;
    I = imread(argv[1]);

    Mat gray = Mat(I.size(),CV_8UC1);
    Mat color_boost = Mat(I.size(),CV_8UC3);

    decolor(I,gray,color_boost);
    imshow("grayscale",gray);
    imshow("color_boost",color_boost);
    waitKey(0);
}
