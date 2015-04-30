#ifndef VIDEOPTD_H
#define VIDEOPTD_H

#include "pricetagdetector.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <thread>
#include <mutex>

class VideoPTD
{
private:
    volatile bool stopSignal;
    cv::VideoCapture vidCap;

    void processFrame(cv::Mat& frame);
    void videoProcessThread();

    std::string videoFileName;

    std::mutex captureMutex;

public:
    VideoPTD(std::string& videoFile) : stopSignal(false), videoFileName(videoFile) {}

    void start();

    void stop()
    {
        std::lock_guard<std::mutex> lock(captureMutex);
        stopSignal = true;
    }
};

#endif // VIDEOPTD_H
