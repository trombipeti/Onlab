#include "videoptd.h"

void VideoPTD::start()
{
    cv::namedWindow(videoFileName, CV_WINDOW_NORMAL);
    stopSignal = false;

    vidCap = cv::VideoCapture(videoFileName);

    std::thread(&VideoPTD::videoProcessThread, this);
}

void VideoPTD::videoProcessThread()
{
    while(1)
    {
        bool shouldExit = false;
        cv::Mat frame;
        vidCap >> frame;
        processFrame(frame);

        {
            std::lock_guard<std::mutex> lock(captureMutex);
            shouldExit = stopSignal;
        }
        if(shouldExit)
        {
            break;
        }
    }

}

void VideoPTD::processFrame(cv::Mat &frame)
{
    std::vector<cv::Vec4i> shelfLines;
    PriceTagDetector::DetectShelfLines(frame, shelfLines);

    for(auto l : shelfLines)
    {
        cv::line(frame, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,100,10), 2);
    }
    cv::imshow(videoFileName, frame);
}
