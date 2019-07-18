#include <iostream>
#include <string>

#include BOSS_OPENCV_U_opencv2__core__core_hpp //original-code:"opencv2/core/core.hpp"
#include BOSS_OPENCV_U_opencv2__core__utility_hpp //original-code:"opencv2/core/utility.hpp"
#include BOSS_OPENCV_U_opencv2__imgproc__imgproc_hpp //original-code:"opencv2/imgproc/imgproc.hpp"
#include BOSS_OPENCV_U_opencv2__imgcodecs_hpp //original-code:"opencv2/imgcodecs.hpp"
#include BOSS_OPENCV_U_opencv2__highgui__highgui_hpp //original-code:"opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    std::string in;
    cv::CommandLineParser parser(argc, argv, "{@input|../data/building.jpg|input image}{help h||show help message}");
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    in = parser.get<string>("@input");

    Mat image = imread(in, IMREAD_GRAYSCALE);

#if 0
    Canny(image, image, 50, 200, 3); // Apply canny edge
#endif

    // Create and LSD detector with standard or no refinement.
#if 1
    Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD);
#else
    Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_NONE);
#endif

    double start = double(getTickCount());
    vector<Vec4f> lines_std;

    // Detect the lines
    ls->detect(image, lines_std);

    double duration_ms = (double(getTickCount()) - start) * 1000 / getTickFrequency();
    std::cout << "It took " << duration_ms << " ms." << std::endl;

    // Show found lines
    Mat drawnLines(image);
    ls->drawSegments(drawnLines, lines_std);
    imshow("Standard refinement", drawnLines);

    waitKey();
    return 0;
}
