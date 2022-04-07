#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/highgui.hpp"
#include "Lane.h"

using namespace std;

int main()
{

    cv::VideoCapture cap("switch.mp4");

    //check if it luks to open the video stream
    if (!cap.isOpened()) {
        std::cout << "Could not open capture" << std::endl;
        return -1;
    }

    while (true) {
        cv::Mat frame;
        cap.read(frame);

        // check if there is something on the frame
        if (frame.empty()) {
            std::cout << "Frame is empty" << std::endl;
            break;
        }

        //auto start = std::chrono::system_clock::now();

        lane lane(frame);
        //auto end = std::chrono::system_clock::now();

        auto aids = lane.get_processed();

        //std::chrono::duration<double> elapsed_seconds = end - start;
        //std::cout << "finished in: " << elapsed_seconds.count() << std::endl;

        //TODO: fps counter
        cv::imshow("res", frame);

        // wait for the user to press esc
        const char c = static_cast<char>(cv::waitKey(25));
        if (c==27) {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}