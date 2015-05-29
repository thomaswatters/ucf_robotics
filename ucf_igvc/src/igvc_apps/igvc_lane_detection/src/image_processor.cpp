//
// Created by thomaswatters on 5/23/15.
//

#include "igvc_lane_detection/image_processor.h"




ImageProcessor::ImageProcessor(std::string subscribe_to, std::string publish_to)
        : it_(nh_)
{
    sub_ = it_.subscribe(
            subscribe_to,1, &ImageProcessor::imageCb, this);
    pub_ = it_.advertise(publish_to, 1);   //the paramater 1 here may have to be changed in the future
}

ImageProcessor::~ImageProcessor()
{

}

void ImageProcessor::imageCb(const sensor_msgs::ImageConstPtr& msg)
{

    cv_bridge::CvImagePtr cv_ptr;
    try
    {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception:  %s:", e.what());
        return;
    }

    //function needs to be overloaded in the child class.
    processImage(cv_ptr->image);

    if (cv_ptr->image.channels() > 2)
    {
        cv_ptr->encoding = "bgr8";
    }
    else
    {
        cv_ptr->encoding = "mono8";
    }


    //broadcast image
    pub_.publish(cv_ptr->toImageMsg());
}