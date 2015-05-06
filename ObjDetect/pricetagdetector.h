#ifndef PRICETAGDETECTOR_H
#define PRICETAGDETECTOR_H

#include <opencv2/core/core.hpp>
#include <vector>

class PriceTagDetector
{
private:

//    static int BwEdgeThresh;
//    static int BwEdgeLimit;

//    void detectShelfLines();
    static int getTheta(cv::Vec4i line, int &score);

public:
    PriceTagDetector(cv::Mat const& _img = cv::Mat());

    static int GetBwEdgeLimit();
    static void SetBwEdgeLimit(int value);

    /**
     * @brief isBWEdge
     * Determines whether the given RGB pixel values can be part of a black-to-white edge.
     * This means that the sign of difference for each pixel value is the same.
     * @param point Vec3b structure containing RGB values
     * @param prevPoint Vec3b structure containing RGB values of the "previous" point
     * @return True if the pixel is likely a part of a BW edge
     */
    static bool isBWEdge(cv::Vec3b point, cv::Vec3b prevPoint);

    static int getMinGrad(cv::Vec3b point, cv::Vec3b prevPoint);

    static int BWEdgeScore(cv::Vec3b point, cv::Vec3b prevPoint);
    static int getAvgGrad(cv::Vec3b point, cv::Vec3b prevPoint);



    static void DetectBWEdges(const cv::Mat& img, cv::Mat& output, int minGrad);

    static void DrawHist(std::vector<int> data, const std::string& winname, int colWidth = 5);

    static void DetectShelfLines(const cv::Mat& img, std::vector<cv::Vec4i> &lines);
};

#endif // PRICETAGDETECTOR_H
