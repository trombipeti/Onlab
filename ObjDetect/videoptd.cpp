#include "videoptd.h"

#include <opencv2/imgproc/imgproc.hpp>

void VideoPTD::start()
{
    if(videoRunning)
    {
        return;
    }
    if(videoFileName == "")
    {
        throw "VideoPTD filename empty, not starting!";
        return;
    }
    cv::namedWindow(videoFileName, CV_WINDOW_NORMAL);
    stopSignal = false;

    vidCap = cv::VideoCapture(videoFileName);

    if(captureThread.joinable())
    {
        captureThread.join();
    }
    captureThread = std::thread(&VideoPTD::videoProcessThread, this);
}

void VideoPTD::videoProcessThread()
{
    videoRunning = true;
    // This is declared outside the while(), otherwise it destroys itself before being processed
    cv::Mat frame;
    while(1)
    {
        bool shouldExit = false;
        vidCap >> frame;
        if(frame.empty())
        {
            break;
        }
        if(frame.cols > 1000 && frame.cols > frame.rows)
        {
            int cols = 1000;
            int rows = frame.rows * (float)(1000.0f / frame.cols);
            cv::resize(frame, frame, cv::Size(cols, rows));
        }
        else if(frame.rows > 1000 && frame.rows > frame.cols)
        {
            int rows = 1000;
            int cols = frame.cols * (float)(1000.0f / frame.rows);
            cv::resize(frame, frame, cv::Size(cols, rows));
        }
        processFrame(frame);

        {
            captureMutex.lock();
            shouldExit = stopSignal;
            captureMutex.unlock();
        }
        if(shouldExit)
        {
            break;
        }
    }
    videoRunning = false;

}

void VideoPTD::processFrame(cv::Mat &frame)
{
    std::vector<cv::Vec4i> shelfLines;
    PriceTagDetector::DetectShelfLines(frame, frame, shelfLines);

//    cv::Canny(frame, frame, 5, 15);
//    PriceTagDetector::DetectBWEdges(frame, frame, 30);
//    cv::cvtColor(frame, frame, CV_GRAY2BGR);

//    for(auto l : shelfLines)
//    {
//        cv::line(frame, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,100,10), 1, CV_AA);
//    }
    cv::imshow(videoFileName, frame);
}

VideoPTD::~VideoPTD()
{
    stop();
    if(captureThread.joinable())
    {
        captureThread.join();
    }
    cv::destroyWindow(videoFileName);
}
