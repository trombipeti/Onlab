#ifndef PRICETAGDETECTOR_H
#define PRICETAGDETECTOR_H

#include <opencv2/core/core.hpp>

class PriceTagDetector
{
private:
    cv::Mat img;

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
public:
    PriceTagDetector(cv::Mat const& _img);
    void detectBWEdges();
};

#endif // PRICETAGDETECTOR_H
