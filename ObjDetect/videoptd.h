#ifndef VIDEOPTD_H
#define VIDEOPTD_H

#include "pricetagdetector.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <thread>
#include <mutex>
#include <atomic>

class VideoPTD
{
private:
    volatile std::atomic_bool stopSignal;

    std::string videoFileName;

    volatile std::atomic_bool videoRunning;

    cv::VideoCapture vidCap;

    void processFrame(cv::Mat &frame);

    std::mutex captureMutex;
    std::thread captureThread;

public:

    explicit VideoPTD(const char* videoFile = "")
    {
        stopSignal = false;
        videoFileName = videoFile;
        videoRunning = false;
    }

    VideoPTD(VideoPTD&& rhs) = delete;
    VideoPTD& operator=(VideoPTD&& rhs) = delete;

    virtual ~VideoPTD();

    void videoProcessThread();

    bool isStared() const
    {
        return (bool)videoRunning;
    }

    void start();

    void stop()
    {
        captureMutex.lock();
        stopSignal = true;
        captureMutex.unlock();
    }
};

#endif // VIDEOPTD_H
