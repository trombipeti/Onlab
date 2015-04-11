#include "pricetagdetector.h"

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <algorithm>

#define BW_EDGE_TRESH 30

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
            std::abs(point[0] - prevPoint[0]) > BW_EDGE_TRESH &&
            std::abs(point[2] - prevPoint[2]) > BW_EDGE_TRESH &&
            std::abs(point[2] - prevPoint[2]) > BW_EDGE_TRESH);
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

void PriceTagDetector::detectBWEdges()
{
    std::vector<cv::Mat> channels;
    cv::GaussianBlur(img, img, cv::Size(5,5), 5);
    cv::medianBlur(img, img, 3);
    //    cv::Laplacian(img, img, img.type(), 3);
    cv::split(img, channels);

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

    cv::Mat edgeMapFuzzy(img.size(), CV_8UC1, cv::Scalar(0));

    int lines = img.rows;
    int cols = img.cols;

    for(int i = 1; i < lines - 1; ++i)
    {
        for(int j = 1; j < cols - 1; ++j)
        {
            // Vertical edges
            int vertScore = BWEdgeScore(img.at<cv::Vec3b>(i,j),
                                        img.at<cv::Vec3b>(i, j-1));
            edgeMapFuzzy.at<uchar>(i,j) += vertScore;

            // Horizontal edges
            int horizScore = BWEdgeScore(img.at<cv::Vec3b>(i,j),
                                         img.at<cv::Vec3b>(i-1,j));
            edgeMapFuzzy.at<uchar>(i,j) += horizScore;

            //std::cout << "Horizontal - vertical: " << horizScore << " - " << vertScore << std::endl;

            //            int r, g, b, rprev, gprev, bprev;
            //            b = channels[0].at<uchar>(i,j);
            //            g = channels[1].at<uchar>(i,j);
            //            r = channels[2].at<uchar>(i,j);

            //            bprev = channels[0].at<uchar>(i,j - 1);
            //            gprev = channels[1].at<ucha0>(i,j - 1);
            //            rprev = channels[2].at<uchar>(i,j - 1);

            //            if(isBWEdge(r,g,b,rprev,gprev,bprev))
            //            {
            //                int minGrad = getMinGrad(r,g,b,rprev,gprev,bprev);
            //                std::cout << "Minimum gradient: " << minGrad << std::endl;
            //                edgeMapFuzzy.at<uchar>(i,j) += minGrad;
            //            }

            // Horizontal edges
        }
    }

    cv::namedWindow("Img", CV_WINDOW_NORMAL);
    cv::imshow("Img", img);


    cv::namedWindow("Edge map", CV_WINDOW_NORMAL);
    cv::imshow("Edge map", edgeMapFuzzy);

    cv::Mat cannyEdges(img.size(), img.type());
    cv::Canny(img, cannyEdges, 1, 1);

    cv::namedWindow("Canny edges", CV_WINDOW_NORMAL);
    cv::imshow("Canny edges", cannyEdges);
}

PriceTagDetector::PriceTagDetector(cv::Mat const& _img)
{
    img = _img.clone();
}
