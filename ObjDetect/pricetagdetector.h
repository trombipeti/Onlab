#ifndef PRICETAGDETECTOR_H
#define PRICETAGDETECTOR_H

#include <opencv2/core/core.hpp>
#include <vector>

class PriceTagDetector
{
private:
    cv::Mat img;

    cv::Mat edgeMapFuzzy;


    /**
     * @brief isBWEdge
     * Determines whether the given RGB pixel values can be part of a black-to-white edge.
     * This means that the sign of difference for each pixel value is the same.
     * @param point Vec3b structure containing RGB values
     * @param prevPoint Vec3b structure containing RGB values of the "previous" point
     * @return True if the pixel is likely a part of a BW edge
     */
    bool isBWEdge(cv::Vec3b point, cv::Vec3b prevPoint);

    int getMinGrad(cv::Vec3b point, cv::Vec3b prevPoint);

    int BWEdgeScore(cv::Vec3b point, cv::Vec3b prevPoint);
    int getAvgGrad(cv::Vec3b point, cv::Vec3b prevPoint);

    int BwEdgeThresh = 30;
    int BwEdgeLimit  = 50;

//    void detectShelfLines();

public:
    PriceTagDetector(cv::Mat const& _img = cv::Mat());

    void detectBWEdges();

    cv::Mat const& getEdgeMap();
    int getBwEdgeThresh() const;
    void setBwEdgeThresh(int value);
    int getBwEdgeLimit() const;
    void setBwEdgeLimit(int value);

    static void DrawHist(std::vector<int> data, const std::string& winname, int colWidth = 5);

    static void DetectShelfLines(const cv::Mat& img, std::vector<cv::Vec4i> &lines);
};

#endif // PRICETAGDETECTOR_H
