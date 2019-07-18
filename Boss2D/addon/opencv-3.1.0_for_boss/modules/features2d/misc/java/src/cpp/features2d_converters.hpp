#ifndef __FEATURES2D_CONVERTERS_HPP__
#define __FEATURES2D_CONVERTERS_HPP__

#include BOSS_OPENCV_U_opencv2__opencv_modules_hpp //original-code:"opencv2/opencv_modules.hpp"
#include BOSS_OPENCV_U_opencv2__core_hpp //original-code:"opencv2/core.hpp"

#include "features2d_manual.hpp"

void Mat_to_vector_KeyPoint(cv::Mat& mat, std::vector<cv::KeyPoint>& v_kp);
void vector_KeyPoint_to_Mat(std::vector<cv::KeyPoint>& v_kp, cv::Mat& mat);

void Mat_to_vector_DMatch(cv::Mat& mat, std::vector<cv::DMatch>& v_dm);
void vector_DMatch_to_Mat(std::vector<cv::DMatch>& v_dm, cv::Mat& mat);

void Mat_to_vector_vector_KeyPoint(cv::Mat& mat, std::vector< std::vector< cv::KeyPoint > >& vv_kp);
void vector_vector_KeyPoint_to_Mat(std::vector< std::vector< cv::KeyPoint > >& vv_kp, cv::Mat& mat);

void Mat_to_vector_vector_DMatch(cv::Mat& mat, std::vector< std::vector< cv::DMatch > >& vv_dm);
void vector_vector_DMatch_to_Mat(std::vector< std::vector< cv::DMatch > >& vv_dm, cv::Mat& mat);


#endif
