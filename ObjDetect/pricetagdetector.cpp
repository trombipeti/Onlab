#include "pricetagdetector.h"

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <algorithm>


/**
 * Generic sign function (returns -1 when val < 0, 0 when val = 0, 1 when val > 0)
 */
template <typename T> int sgn(T val) {
    return (T(0) < val) - (T(0) > val);
}

bool PriceTagDetector::isBWEdge(cv::Vec3b point, cv::Vec3b prevPoint) //int r1, int g1, int b1, int r2, int g2, int b2)
{
    return (sgn(point[0] - prevPoint[0]) == sgn(point[1] - prevPoint[1]) &&
            sgn(point[1] - prevPoint[1]) == sgn(point[2] - prevPoint[2]) &&
            point[0] != prevPoint[0] &&
            std::abs(point[0] - prevPoint[0]) > BwEdgeThresh &&
            std::abs(point[2] - prevPoint[2]) > BwEdgeThresh &&
            std::abs(point[2] - prevPoint[2]) > BwEdgeThresh);
}

int PriceTagDetector::BWEdgeScore(cv::Vec3b point, cv::Vec3b prevPoint)
{
    int result = 0;
    if(isBWEdge(point, prevPoint) )
    {
        result += getAvgGrad(point, prevPoint);
    }

    return result;
}

int PriceTagDetector::getMinGrad(cv::Vec3b point, cv::Vec3b prevPoint)
{
    return std::min({std::abs(point[0] - prevPoint[0]),
                     std::abs(point[1] - prevPoint[1]),
                     std::abs(point[2] - prevPoint[2])
                    });
}

int PriceTagDetector::getAvgGrad(cv::Vec3b point, cv::Vec3b prevPoint)
{
    return (
            std::abs(point[0] - prevPoint[0]) +
            std::abs(point[1] - prevPoint[1]) +
            std::abs(point[2] - prevPoint[2])
            ) / 3;
}
int PriceTagDetector::getBwEdgeLimit() const
{
    return BwEdgeLimit;
}

void PriceTagDetector::setBwEdgeLimit(int value)
{
    BwEdgeLimit = value;
}

int PriceTagDetector::getBwEdgeThresh() const
{
    return BwEdgeThresh;
}

void PriceTagDetector::setBwEdgeThresh(int value)
{
    BwEdgeThresh = value;
}


void PriceTagDetector::detectBWEdges()
{
    std::vector<cv::Mat> channels;
    cv::Mat imgToProcess;
    cv::medianBlur(img, imgToProcess, 3);
    cv::split(imgToProcess, channels);

    const char* names[] = {"B", "G", "R"};

    int i = 0;
    for(auto channel : channels)
    {
        //        cv::Canny(channel, channel, 1, 1);
        //        cv::namedWindow(names[i], CV_WINDOW_NORMAL);
        //        cv::imshow(names[i], channel);
        //        i++;
    }

    //    return;

    int lines = imgToProcess.rows;
    int cols = imgToProcess.cols;

    for(int i = 1; i < lines - 1; ++i)
    {
        for(int j = 1; j < cols - 1; ++j)
        {
            int overallScore = 0;
            // Vertical edges
            int vertScore = BWEdgeScore(imgToProcess.at<cv::Vec3b>(i,j),
                                        imgToProcess.at<cv::Vec3b>(i, j-1));
            overallScore += vertScore;

            // Horizontal edges
            int horizScore = BWEdgeScore(imgToProcess.at<cv::Vec3b>(i,j),
                                         imgToProcess.at<cv::Vec3b>(i-1,j));
            overallScore += horizScore;
            edgeMapFuzzy.at<uchar>(i,j) = (overallScore > BwEdgeLimit ? 0 : 255);

        }
    }
}

cv::Mat const& PriceTagDetector::getEdgeMap()
{
    return edgeMapFuzzy;
}

PriceTagDetector::PriceTagDetector(cv::Mat const& _img)
{
    img = _img.clone();

    edgeMapFuzzy = cv::Mat(img.size(), CV_8UC1, cv::Scalar(0));
}
