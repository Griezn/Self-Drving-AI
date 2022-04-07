#pragma once
#include <opencv2/core.hpp>

//TODO: add more variables to the class
class lane
{
public:
	///======= constructor =======///
	lane(cv::Mat image);

	//pub function
	/// @brief Returns the image stored in the class in half the size.
	///	@return The image half so big
	cv::Mat get_processed() { return this->processed_image_; }

	//======== public variables ========//
	cv::Mat& image;
	float curvature;
	bool is_right;
	int speed_limit;

private:
	///======== transformed images ========///
	cv::Mat warped_image_, processed_image_;

	///======== source points for the line mask ========///
	cv::Point2f src_lane_vertices_[4] = {
		cv::Point(450, 1080),
		cv::Point(800,600),
		cv::Point(1100, 600),
		cv::Point(1450, 1080)
	};

	///======== destination points for the line mask ========///
	cv::Point2f dst_lane_vertices_[4] = {
		cv::Point(0, 480),
		cv::Point(0, 0),
		cv::Point(1000, 0),
		cv::Point(1000, 480)
	};

	///=== Arrays with the points corresponding to the line ===///
	std::vector<cv::Point2f> left_line_;
	std::vector<cv::Point2f> right_line_;

	//======== main functions ==========//
	void lane_detection();
	void road_detection();

	//======== utility functions ========//
	void warp();
	void unwarp();
	void process_image();
	void clean_image();
	void find_points();
	void draw_lane();
};

