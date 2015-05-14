#include "pricetagdetector.h"

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <map>
#include <algorithm>

//int PriceTagDetector::BwEdgeThresh = 30;
//int PriceTagDetector::BwEdgeLimit  = 50;

/**
 * Generic sign function (returns -1 when val < 0, 0 when val = 0, 1 when val > 0)
 */
template <typename T> int sgn(T val) {
    return (T(0) < val) - (T(0) > val);
}

bool PriceTagDetector::isBWEdge(cv::Vec3b point, cv::Vec3b prevPoint)
{
    return (sgn(point[0] - prevPoint[0]) == sgn(point[1] - prevPoint[1]) &&
            sgn(point[1] - prevPoint[1]) == sgn(point[2] - prevPoint[2]) &&
            point[0] != prevPoint[0]);
}

int PriceTagDetector::BWEdgeScore(cv::Vec3b point, cv::Vec3b prevPoint)
{
    int result = 0;
    if(isBWEdge(point, prevPoint) )
    {
        result += getAvgGrad(point, prevPoint);
//        std::cout << "-------------------------------------------" << std::endl;
//        std::cout << "Average gradient: " << result << std::endl;
//        std::cout << (int)point[0] << "-" << (int)prevPoint[0] << std::endl;
//        std::cout << (int)point[1] << "-" << (int)prevPoint[1] << std::endl;
//        std::cout << (int)point[2] << "-" << (int)prevPoint[2] << std::endl;
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

int PriceTagDetector::getTheta(cv::Vec4i line/*, int& score*/)
{
    float x1, y1, x2, y2;
    x1 = line[0];
    y1 = line[1];
    x2 = line[2];
    y2 = line[3];
    float xdiff = /*std::abs*/(x1 - x2);
    float ydiff = /*std::abs*/(y1 - y2);
    float theta = std::atan(ydiff / xdiff);
    theta *= 180.0f/CV_PI;
    theta = std::fmod(theta, 180.0f);
    int theta_int = (int)(theta + 0.5f);
//    score = sqrt(sqrt(xdiff*xdiff + ydiff*ydiff));
    return theta_int;
}


void PriceTagDetector::DetectShelfLines(const cv::Mat& img, cv::Mat& result, std::vector<cv::Vec4i> &lines)
{
    cv::Mat cannyEdges, imgToProcess;
//    cv::cvtColor(edgeMapFuzzy, cannyEdges, CV_GRAY2BGR);

    float resizeRatio = 1.0f;
    if(img.cols > 1000 && img.cols > img.rows)
    {
        int cols = 1000;
        resizeRatio = (float)(1000.0f / img.cols);
        int rows = img.rows * resizeRatio;
        cv::resize(img, imgToProcess, cv::Size(cols, rows));
    }
    else if(img.rows > 1000 && img.rows > img.cols)
    {
        int rows = 1000;
        resizeRatio = (float)(1000.0f / img.rows);
        int cols = img.cols * resizeRatio;
        cv::resize(img, imgToProcess, cv::Size(cols, rows));
    }
    else
    {
        imgToProcess = img.clone();
    }
    cv::GaussianBlur(imgToProcess, imgToProcess, cv::Size(5,5), 0);
    cv::Canny(imgToProcess, cannyEdges, 5, 15);
//    cv::Sobel(imgToProcess, cannyEdges, -1, 1, 1);
//    cv::cvtColor(cannyEdges, cannyEdges, CV_BGR2GRAY);
//    cv::imshow("Canny", cannyEdges);
//    PriceTagDetector::DetectBWEdges(imgToProcess, cannyEdges, 0);
//    cv::medianBlur(cannyEdges, cannyEdges, 3);

    std::vector<cv::Vec4i> houghLines;
    cv::HoughLinesP(cannyEdges, houghLines, 1, CV_PI/180.0, 50, 150, 10);

    std::vector<cv::Vec2f> longLines;
    cv::HoughLines(cannyEdges, longLines, 1, CV_PI/180.0, 150);
//    lines.clear();
//    for( size_t i = 0; i < longLines.size(); i++ )
//    {
//        float rho = longLines[i][0], theta = longLines[i][1];
//        cv::Point pt1, pt2;
//        double a = cos(theta), b = sin(theta);
//        double x0 = a*rho, y0 = b*rho;
//        pt1.x = cvRound(x0 + 100*(-b));
//        pt1.y = cvRound(y0 + 100*(a));
//        pt2.x = cvRound(x0 - 100*(-b));
//        pt2.y = cvRound(y0 - 100*(a));
//        lines.push_back(cv::Vec4i(pt1.x, pt1.y, pt2.x, pt2.y));
//    }


    lines = houghLines;

    std::map<int, int> thetas;
    std::map<int, int> rhos;
    for(size_t i = 0;i<180;i++)
    {
        thetas[i] = 1;
//        rhos[i] = 1;
    }
    int maxval_theta = 0;
    int maxval_rho   = 0;

    for(size_t i = 0;i<longLines.size(); ++i)
    {
        float rho = longLines[i][0], theta = longLines[i][1];
        int theta_int = (int)(theta + 0.5f);
        int rho_int = (int)(rho + 0.5f);
        rho_int = rho_int - (rho_int % 10);
//        thetas[theta_int] += 1;
        rhos[rho_int] += 1;
        if(rhos[rho_int] > maxval_rho)
        {
            maxval_rho = rhos[rho_int];
        }
    }

    for( size_t i = 0; i < houghLines.size(); i++ )
    {
        cv::Vec4i l = houghLines[i];
//        int score = 0;
        int theta_int = getTheta(l/*, score*/);
        thetas[theta_int] += 1;
        if(thetas[theta_int] > maxval_theta)
        {
            maxval_theta = thetas[theta_int];
        }
    }

    for(auto line : houghLines)
    {
        int line_theta = getTheta(line);
        for(int i = 0;i<4;++i)
        {
            line[i] /= resizeRatio;
        }
        if(thetas[line_theta] >= (float)(maxval_theta*0.7f))
        {
            cv::line(result, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(10,255,255), 5/resizeRatio, CV_AA);
        }
        else
        {
            cv::line(result, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(255,100,10), 1.5/resizeRatio, CV_AA);
        }
    }

    std::vector<int> thetavals;
    for(auto mapit : thetas)
    {
        int val = mapit.second;
        if(maxval_theta != 0)
        {
            val /= (maxval_theta/200.0f);
        }
        thetavals.push_back(val);
    }


    std::vector<int> rhovals;
    for(auto mapit : rhos)
    {
        int val = mapit.second;
        if(maxval_rho != 0)
        {
            val /= (maxval_rho/200.0f);
        }
        rhovals.push_back(val);
    }


    PriceTagDetector::DrawHist(thetavals, "Theta histogram");
    PriceTagDetector::DrawHist(rhovals, "Rho histogram");
}


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
            int coltop = (histrows - data[i/colWidth] + minval);
            if(coltop < 0)
            {
                coltop = 0;
            }
            for(int j = histrows - 1;j > coltop;--j)
            {
                hist.at<uchar>(j,i) = 150 + 105 * ((i/colWidth)%2);
            }
        }
        cv::namedWindow(winname, CV_WINDOW_NORMAL);
        cv::imshow(winname, hist);
    }
}

void PriceTagDetector::DetectBWEdges(const cv::Mat& img, cv::Mat& output, int minGrad)
{
    cv::Mat imgToProcess = img.clone();

    cv::Mat edgeMapFuzzy(img.size(), CV_8UC1, cv::Scalar(0));

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
            int vertScore = PriceTagDetector::BWEdgeScore(imgToProcess.at<cv::Vec3b>(i,j),
                                        imgToProcess.at<cv::Vec3b>(i, j-1));
//            overallScore += vertScore;

            // Horizontal edges
            int horizScore = PriceTagDetector::BWEdgeScore(imgToProcess.at<cv::Vec3b>(i,j),
                                         imgToProcess.at<cv::Vec3b>(i-1,j));
//            overallScore += horizScore;
            overallScore = std::max(vertScore, horizScore);
            edgeMapFuzzy.at<uchar>(i,j) = (overallScore > minGrad ? 0 : 255);

        }
    }

    output = edgeMapFuzzy;

//    cv::namedWindow("Processed image", CV_WINDOW_NORMAL);
//    cv::imshow("Processed image", imgToProcess);
}

PriceTagDetector::PriceTagDetector(cv::Mat const& _img)
{

}
