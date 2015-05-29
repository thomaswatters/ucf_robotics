//
// Created by thomaswatters on 5/23/15.
//

#ifndef PROJECT_LANE_PROCESSOR_H
#define PROJECT_LANE_PROCESSOR_H

#include "image_processor.h"
#include "opencv2/opencv.hpp"

class LaneProcessor : public ImageProcessor
{
public:
    LaneProcessor(std::string subscribe_to, std::string publish_to);\
    virtual void processImage(cv::Mat& img);

private:

    cv::Scalar orange_hsv_low_, orange_hsv_high_;
    cv::Scalar white_hsv_low_, white_hsv_high_;
    cv::Size gaussian_kernel_size_;



};


#endif //PROJECT_LANE_PROCESSOR_H
