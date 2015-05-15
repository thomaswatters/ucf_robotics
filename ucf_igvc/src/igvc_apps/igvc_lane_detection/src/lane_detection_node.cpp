//
// Created by thomaswatters on 5/14/15.
//

#include "ros/ros.h"
#include "image_transport/image_transport.h"
#include "cv_bridge/cv_bridge.h"
#include "opencv2/opencv.hpp"


/// Global variables
cv::Mat src, dst;

void processImage();

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        src = cv_bridge::toCvShare(msg, "bgr8")->image;
        cv::imshow("orig", src);
        processImage();
        cv::imshow("processed", dst);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}


int morph_elem = 2;
int morph_size = 14;
int morph_operator = 2;

void processImage()
{
    using namespace cv;
    // Since MORPH_X : 2,3,4,5 and 6
    int operation = morph_operator + 2;
    Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1,
                                                         2 * morph_size + 1),
                                        Point(morph_size, morph_size));

    // FIND ORANGE
    Mat fullImageHSV;
    cvtColor(src, fullImageHSV, CV_BGR2HSV);
    Mat threshedHSVorange;
    inRange(fullImageHSV, Scalar(0, 125, 125), Scalar(80, 255, 255),
            threshedHSVorange);
    imshow("threshHSV", threshedHSVorange);

    Mat threshedHSVwhite;
    Mat srcBlur;
    //blur it
    GaussianBlur(fullImageHSV, srcBlur, Size(5, 5), 0, 0);
    inRange(fullImageHSV, Scalar(0, 0, 35), Scalar(255, 70, 255),
            threshedHSVwhite);


    imshow("WHITE", threshedHSVwhite);


    // TODO- ELONGATE MORE VERTIVALLY RATHER THAN HORIZONTALLY
    // TODO- Problem with slanted saw horse, nothing will catch bottom
    // TODO- Noise reduction algo
    morphologyEx(threshedHSVorange, dst, operation, element);

    bitwise_not(dst, dst);
    bitwise_and(threshedHSVwhite, dst, dst);


    int blocksCols = 12;
    int blocksRows = 12;
    int blockWidth = dst.size().width / blocksCols;
    int blockHeight = dst.size().height / blocksRows;

    for (int i = 0; i < blocksCols; i++)
    {
        for (int j = 0; j < blocksRows; j++)
        {
            vector<vector<Point> > contours;
            Vec4f lines;

            Rect grid(blockWidth * i, blockHeight * j, blockWidth, blockHeight);
            cv::Mat roi = dst(grid);
            cv::findContours(roi, contours, cv::RETR_LIST,
                             cv::CHAIN_APPROX_SIMPLE);

            std::vector<Point> points;
            for (std::vector<std::vector<Point> >::const_iterator iter = contours.begin();
                 iter != contours.end(); iter++)
            {
                if (cv::contourArea((*iter)) < 100)
                    continue;

                for (std::vector<Point>::const_iterator iter2 = (*iter).begin();
                     iter2 != (*iter).end(); iter2++)
                {
                    points.push_back(*iter2);
                }
            }
            if (points.size() > 0)
            {
                cv::fitLine(points, lines, CV_DIST_HUBER, 0, 0.01, 0.01);
            }
            else
                continue;


            Point2f low_left(lines[2] - lines[0] * 1000,
                             lines[3] - lines[1] * 1000);
            Point2f up_right(lines[2] + lines[0] * 1000,
                             lines[3] + lines[1] * 1000);


            cv::line(roi, low_left, up_right, Scalar(255, 0, 0), 10);

        }

    }
}



int main(int argc, char** argv)
{
    ros::init(argc, argv, "image_listener");
    ros::NodeHandle nh;
    cv::namedWindow("orig");
    cv::namedWindow("processed");
    cv::namedWindow("WHITE");
    cv::namedWindow("threshHSV");

    cv::startWindowThread();
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("/front_camera/image_raw", 1, imageCallback);

    ros::Rate sleep_rate(15);
    while(ros::ok())
    {
        ros::spinOnce();
        sleep_rate.sleep();
    }

    ros::spin();
    cv::destroyWindow("view");

    return 0;
}
