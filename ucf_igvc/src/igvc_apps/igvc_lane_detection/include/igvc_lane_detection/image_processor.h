//
// Created by thomaswatters on 5/23/15.
//

#ifndef PROJECT_SOURCE_FILE_H
#define PROJECT_SOURCE_FILE_H


#include <ros/ros.h>

#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>

#include <opencv2/highgui/highgui.hpp>

class ImageProcessor
{
public:
    ros::NodeHandle nh_;
    image_transport::ImageTransport it_;
    image_transport::Subscriber sub_;
    image_transport::Publisher pub_;
    ImageProcessor(std::string subscribeTo,  std::string publishTo);
    ~ImageProcessor();
    virtual void processImage(cv::Mat& imgPointer) = 0;
    void imageCb(const sensor_msgs::ImageConstPtr& msg);
};



#endif //PROJECT_SOURCE_FILE_H
