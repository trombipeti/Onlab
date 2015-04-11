#include "pricetagdetector.h"

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#define BW_EDGE_TRESH 50

/**
 * Generic sign function (returns -1 when val < 0, 0 when val = 0, 1 when val > 0)
 */
template <typename T> int sgn(T val) {
    return (T(0) < val) - (T(0) > val);
}

bool PriceTagDetector::isBWEdge(int r1, int g1, int b1, int r2, int g2, int b2)
{
    return (sgn(r1 - r2) == sgn(g1 - g2) &&
            sgn(g1 - g2) == sgn(b1 - b2) &&
            r1 != r2 &&
            std::abs(r1-r2) > BW_EDGE_TRESH &&
            std::abs(g1-g2) > BW_EDGE_TRESH &&
            std::abs(g1-g2) > BW_EDGE_TRESH);
}

void PriceTagDetector::detectBWEdges()
{
    std::vector<cv::Mat> channels;
    cv::GaussianBlur(img, img, cv::Size(3,3), 3);
    cv::Laplacian(img, img, img.type(), 3);
    cv::split(img, channels);

    cv::Mat edgeMapFuzzy(img.size(), CV_8UC1);

    int lines = img.rows;
    int cols = img.cols;

    for(int i = 1; i < lines - 1; ++i)
    {
        for(int j = 1; j < cols - 1; ++j)
        {
            int r, g, b, rprev, gprev, bprev;
            b = channels[0].at<uchar>(i,j);
            g = channels[1].at<uchar>(i,j);
            r = channels[2].at<uchar>(i,j);

            bprev = channels[0].at<uchar>(i,j - 1);
            gprev = channels[1].at<uchar>(i,j - 1);
            rprev = channels[2].at<uchar>(i,j - 1);

            if(isBWEdge(r,g,b,rprev,gprev,bprev))
            {
                edgeMapFuzzy.at<uchar>(i,j) = 255;
            }
            else
            {
                edgeMapFuzzy.at<uchar>(i,j) = 0;
            }
        }
    }
    cv::namedWindow("Img", CV_WINDOW_NORMAL);
    cv::imshow("Img", img);


    cv::namedWindow("Edge map", CV_WINDOW_NORMAL);
    cv::imshow("Edge map", edgeMapFuzzy);
}

PriceTagDetector::PriceTagDetector(cv::Mat const& _img)
{
    img = _img.clone();
}
