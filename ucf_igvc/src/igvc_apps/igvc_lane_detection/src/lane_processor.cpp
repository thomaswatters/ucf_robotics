//
// Created by thomaswatters on 5/23/15.
//

#include "igvc_lane_detection/lane_processor.h"

LaneProcessor::LaneProcessor(std::string subscribe_to, std::string publish_to)
        : ImageProcessor(subscribe_to, publish_to)
{

}

void LaneProcessor::processImage(cv::Mat &img)
{

        int morph_elem = 2;
        int morph_size = 14;
        int morph_operator = 2;

        int operation = morph_operator + 2;
        cv::Mat element = cv::getStructuringElement(morph_elem, cv::Size(2 * morph_size + 1, 2 * morph_size + 1),
                                                    cv::Point(morph_size, morph_size));

        // FIND ORANGE
        cv::Mat hsv_img;
        cvtColor(img, hsv_img, CV_BGR2HSV);
        cv::Mat orange_thresh_bin;
//        inRange(fullImageHSV, Scalar(0, 125, 125), Scalar(80, 255, 255),
//                threshedHSVorange);

        cv::inRange(hsv_img, orange_hsv_low_, orange_hsv_high_, orange_thresh_bin);

        cv::Mat white_thresh_bin;
        cv::Mat blurred_img;
        //blur it
        cv::GaussianBlur(hsv_img, blurred_img, gaussian_kernel_size_, 0, 0);
        cv::inRange(hsv_img, white_hsv_low_, white_hsv_high_, white_thresh_bin);


        cv::Mat dst;
        morphologyEx(orange_thresh_bin, dst, operation, element);

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
