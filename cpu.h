#pragma once
#include <opencv2/opencv.hpp>
#include "cuda.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <array>
#include "common.h"

#define ANGLE 15

struct Point{
    int x;
    int y; 
};

struct pictureParams {
    int max;
    int min;

    int getSize() {return max - min;}
};


Point getRotatedPoint(Point point, int angle);

void restorePictureCPU();

void computePictureSize(const cv::Mat& src);

cv::Mat houghTransformCPU(const cv::Mat& src);
cv::Mat houghTransformCPUZ(const cv::Mat& src);

cv::Mat tresholdTransformCPU(const cv::Mat& src, uchar3 color, uint16_t threshold);

cv::Mat rotatePicture182(const cv::Mat& src);
cv::Mat rotatePicturePikcha(const cv::Mat& src);