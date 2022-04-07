#include "Lane.h"

#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

lane::lane(cv::Mat image) :
    image(image),
    curvature(0),
    is_right(true),
    speed_limit(0),
	warped_image_(480, 1000, CV_8UC3),
	processed_image_(warped_image_.clone())
{
    lane_detection();
}

void lane::warp(){
    cv::Mat perspective_matrix = cv::getPerspectiveTransform(this->src_lane_vertices_, this->dst_lane_vertices_);
    cv::warpPerspective(this->image, this->warped_image_,
        perspective_matrix, this->warped_image_.size(),
        cv::INTER_LINEAR, cv::BORDER_CONSTANT);
}

void lane::unwarp(){
    cv::Mat perspective_matrix = cv::getPerspectiveTransform(this->dst_lane_vertices_, this->src_lane_vertices_);
    cv::warpPerspective(this->warped_image_, this->image,
        perspective_matrix, this->warped_image_.size(),
        cv::INTER_LINEAR, cv::BORDER_CONSTANT);
}

void lane::process_image(){
    cv::Mat img;
    cv::cvtColor(this->warped_image_, img, cv::COLOR_RGB2GRAY);

    cv::Mat mask_yellow, mask_white;
    cv::inRange(img, cv::Scalar(20, 100, 100), cv::Scalar(30, 255, 255), mask_yellow);
    cv::inRange(img, cv::Scalar(150, 150, 150), cv::Scalar(255, 255, 255), mask_white);

    cv::Mat mask;
    cv::bitwise_or(mask_white, mask_yellow, mask);//combine the masks
    cv::bitwise_and(img, mask, this->processed_image_);//apply the mask to the image
}

void lane::clean_image(){
    //clean the image
    cv::GaussianBlur(this->processed_image_, this->processed_image_, cv::Size(9, 9), 0);
    //fill gaps
    cv::Mat kernel = cv::Mat::ones(15, 15, CV_8U);
    cv::dilate(this->processed_image_, this->processed_image_, kernel);
    cv::erode(this->processed_image_, this->processed_image_, kernel);
    cv::morphologyEx(this->processed_image_, this->processed_image_, cv::MORPH_CLOSE, kernel);

    //threshold the image we only want the white
    cv::threshold(this->processed_image_, this->processed_image_, 150, 255, cv::THRESH_BINARY);
}

//TODO: multiple line support
void lane::find_points(){
	std::vector<cv::Point2f> points;
    const cv::Size im_size = this->processed_image_.size();
    bool should_brake = false;
    bool at_end = false;
    bool at_top = false;
    //start window at bottom left
    auto window = cv::Rect(0, 420, 120, 60);

    while (!should_brake) {
        float current_x = window.x + window.width * 0.5f;
        float current_y = window.y + window.height * 0.5f;

        //only use roi in the image
        cv::Mat roi = this->processed_image_(window);

        //get all non-black-pixels (no racism!)
        std::vector < cv::Point2f> locations;
        cv::findNonZero(roi, locations);

        //find the average x value of the white pixels
    	float avg_x = 0.0f;
        for (int i = 0; i < locations.size(); i++) {
            float x = locations[i].x;
            avg_x += window.x + x;
        }

        if (!locations.empty()) {
            avg_x = avg_x / locations.size();
            cv::Point point(avg_x, current_y);

            //add point to the list
            if (point.x < (im_size.width / 2)) {
                this->left_line_.push_back(point);
            }
            else {
                this->right_line_.push_back(point);
            }
        }

        //=== AWESOME MOVING WINDOW SCRIPT ===//
        if (at_end) {
            window.x = 0;
            window.y -= 60;
            at_end = false;
            if (window.y < 0) {
                window.y = 0;
                at_top = true;
            }
        }

        window.x += 120;
        if (window.x + window.width >= im_size.width) {
            window.x = im_size.width - window.width - 1;
            at_end = true;
        }

        if (at_end && at_top) {
            should_brake = true;
        }
    }
}

//TODO: fix overlay
void lane::draw_lane(){
	std::vector<cv::Point> all_pts; //Used for the end polygon at the end. 
    std::vector<cv::Point2f> out_pts;
    cv::Mat perspective_matrix = cv::getPerspectiveTransform(this->dst_lane_vertices_, this->src_lane_vertices_);
    cv::perspectiveTransform(this->left_line_, out_pts, perspective_matrix); //transform to fit on the perspective of the original image

    for (int i = 0; i < out_pts.size() - 1; i++) {
        cv::line(this->image, out_pts[i], out_pts[i + 1], cv::Scalar(255, 0, 0), 3);
        all_pts.push_back(out_pts[i]);
    }
    all_pts.push_back(out_pts[out_pts.size() - 1]);

    //now right line
    cv::perspectiveTransform(this->right_line_, out_pts, perspective_matrix);
    for (int i = 0; i < out_pts.size() - 1; i++) {
        cv::line(this->image, out_pts[i], out_pts[i + 1], cv::Scalar(255, 0, 0), 3);
        all_pts.push_back(out_pts[out_pts.size() - i - 1]);
    }
    all_pts.push_back(out_pts[0]);

    std::vector<std::vector<cv::Point>> arr;
    arr.push_back(all_pts);
    cv::Mat overlay = cv::Mat::zeros(this->image.size(), this->image.type());
    cv::fillPoly(overlay, arr, cv::Scalar(0, 255, 100));
    cv::addWeighted(this->image, 1, overlay, 0.5, 0, this->image);
}

//TODO: Make functions more universal
void lane::lane_detection(){
    //auto start = std::chrono::system_clock::now();
    warp();// O.067
    //auto warp = std::chrono::system_clock::now();
    //std::chrono::duration<double> warp_time = warp - start;
    process_image();// 0.015
    //auto process = std::chrono::system_clock::now();
    //std::chrono::duration<double> process_time = process - warp;
    clean_image(); // 0.125
    //auto clean = std::chrono::system_clock::now();
    //std::chrono::duration<double> clean_time = clean - process;
    find_points(); // 0.006
    //auto points = std::chrono::system_clock::now();
    //std::chrono::duration<double> find_time = points - clean;
    //draw_lane(); // 0.300
    //auto draw = std::chrono::system_clock::now();
    //std::chrono::duration<double> draw_time = draw - points;

    //std::cout << "times: " << warp_time.count() << " " << process_time.count() << " " << clean_time.count() << " " << find_time.count() << " " << draw_time.count() << '\n';

}

void lane::road_detection(){}
