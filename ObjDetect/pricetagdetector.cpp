#include "pricetagdetector.h"

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <map>
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

void PriceTagDetector::DetectShelfLines(const cv::Mat& img, std::vector<cv::Vec4i> &lines)
{
    cv::Mat cannyEdges, imgToProcess;
//    cv::cvtColor(edgeMapFuzzy, cannyEdges, CV_GRAY2BGR);

    if(img.cols > 1000 || img.rows > 1000)
    {
        cv::resize(img, imgToProcess, cv::Size(1000,1000));
    }
    else
    {
        imgToProcess = img.clone();
    }
    cv::GaussianBlur(imgToProcess, imgToProcess, cv::Size(5,5), 0);
    cv::Canny(imgToProcess, cannyEdges, 5, 15);

    std::vector<cv::Vec4i> houghLines;
    cv::HoughLinesP(cannyEdges, houghLines, 1, CV_PI/180.0, 50, 50, 10);

    std::map<int, int> thetas;
    int maxval = 0;
    for( size_t i = 0; i < houghLines.size(); i++ )
    {
        cv::Vec4i l = houghLines[i];
        float x1, y1, x2, y2;
        x1 = l[0];
        y1 = l[1];
        x2 = l[2];
        y2 = l[3];
        float xdiff = std::abs(x1 - x2);
        float ydiff = std::abs(y1 - y2);
        float theta = std::atan(ydiff / xdiff);
        theta *= 180.0f/CV_PI;
        theta = std::fmod(theta, 180.0f);
        int theta_int = (int)(theta + 0.5f);
        thetas[theta_int] += sqrt(xdiff*xdiff + ydiff*ydiff);
        if(thetas[theta_int] > maxval)
        {
            maxval = thetas[theta_int];
        }
    }


    std::vector<int> thetavals;
    for(auto mapit : thetas)
    {
        int val = mapit.second;
        if(maxval != 0)
        {
            val /= (maxval/200.0f);
        }
        thetavals.push_back(val);
    }

    PriceTagDetector::DrawHist(thetavals, "Theta histogram");
}

//void PriceTagDetector::detectShelfLines()
//{
//    cv::Mat cannyEdges, imgToProcess;
////    cv::cvtColor(edgeMapFuzzy, cannyEdges, CV_GRAY2BGR);

//    if(img.cols > 1000 || img.rows > 1000)
//    {
//        cv::resize(img, imgToProcess, cv::Size(1000,1000));
//    }
//    else
//    {
//        imgToProcess = img.clone();
//    }
//    cv::GaussianBlur(imgToProcess, imgToProcess, cv::Size(5,5), 0);
////    cv::erode(img, imgToProcess, cv::Mat());
////    cv::dilate(imgToProcess, imgToProcess, cv::Mat());
//    cv::Canny(imgToProcess, cannyEdges, 5, 15);
////    cv::Sobel(imgToProcess, cannyEdges, -1, 1, 1);
////    cv::cvtColor(cannyEdges, cannyEdges, CV_BGR2GRAY);


//    cv::HoughLinesP(cannyEdges, houghLines, 1, CV_PI/180.0, 50, 50, 10);

//    std::map<int, int> thetas;
//    int maxval = 0;
//    for( size_t i = 0; i < houghLines.size(); i++ )
//    {
//        cv::Vec4i l = houghLines[i];
//        float x1, y1, x2, y2;
//        x1 = l[0];
//        y1 = l[1];
//        x2 = l[2];
//        y2 = l[3];
//        float xdiff = std::abs(x1 - x2);
//        float ydiff = std::abs(y1 - y2);
//        float theta = std::atan(ydiff / xdiff);
//        theta *= 180.0f/CV_PI;
//        theta = std::fmod(theta, 180.0f);
//        int theta_int = (int)(theta + 0.5f);
//        thetas[theta_int] += sqrt(xdiff*xdiff + ydiff*ydiff);
//        if(thetas[theta_int] > maxval)
//        {
//            maxval = thetas[theta_int];
//        }
//    }


//    std::vector<int> thetavals;
//    for(auto mapit : thetas)
//    {
//        int val = mapit.second;
//        if(maxval != 0)
//        {
//            val /= (maxval/200.0f);
//        }
//        thetavals.push_back(val);
//    }

//    DrawHist(thetavals, "Theta histogram");

////    cv::line( imgToProcess, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, CV_AA);
////    cv::namedWindow("Hough lines", CV_WINDOW_NORMAL);
////    cv::imshow("Hough lines", imgToProcess);

//}

void PriceTagDetector::DrawHist(std::vector<int> data, const std::string &winname, int colWidth)
{
    if(data.size() > 0)
    {
        int maxval = *(std::max_element(data.begin(), data.end()));
        int minval = *(std::min_element(data.begin(), data.end()));
        int histcols = data.size() * colWidth;
        int histrows = maxval - minval;
        if(histcols <= 0 || histrows <= 0)
        {
            // TODO Error handling
            return;
        }
        cv::Mat hist(cv::Size(histcols, histrows), CV_8UC1, cv::Scalar(0));
        for(int i = 0;i<histcols;++i)
        {
//            for(int it = i; it<i+10;++it)
//            {
                for(int j = histrows - 1;j >= (data[i/colWidth] - minval);--j)
                {
                    hist.at<uchar>(j,i) = 100 + 155 * ((i/colWidth)%2);
                }
//            }
        }
        cv::namedWindow(winname, CV_WINDOW_NORMAL);
        cv::imshow(winname, hist);
    }
}

void PriceTagDetector::detectBWEdges()
{
    cv::Mat imgToProcess = img.clone();

    std::vector<cv::Mat> channels;
//    cv::medianBlur(img, imgToProcess, 3);
//    cv::GaussianBlur(img, imgToProcess, cv::Size(3,3), 0);
//    cv::Laplacian(imgToProcess, imgToProcess, imgToProcess.type());

//     detectShelfLines();

//    cv::split(imgToProcess, channels);

//    cv::Mat grads[3];
//    const char* names[] = {"grad B", "grad G", "grad R"};

//    cv::Sobel(channels[0], grads[0], -1, 1, 1, 3, 1, 0 );
//    cv::Sobel(channels[1], grads[1], -1, 1, 1, 3, 1, 0 );
//    cv::Sobel(channels[2], grads[2], -1, 1, 1, 3, 1, 0 );

//    for (int i = 0;i<3;i++)
//    {
//        cv::namedWindow(names[i], CV_WINDOW_NORMAL);
//        cv::imshow(names[i], grads[i]);
//    }

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
//            overallScore += vertScore;

            // Horizontal edges
            int horizScore = BWEdgeScore(imgToProcess.at<cv::Vec3b>(i,j),
                                         imgToProcess.at<cv::Vec3b>(i-1,j));
//            overallScore += horizScore;
            overallScore = std::max(vertScore, horizScore);
            edgeMapFuzzy.at<uchar>(i,j) = (overallScore > BwEdgeLimit ? 255 : 0);

        }
    }


//    cv::namedWindow("Processed image", CV_WINDOW_NORMAL);
//    cv::imshow("Processed image", imgToProcess);
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
