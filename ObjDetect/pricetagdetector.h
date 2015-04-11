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
     * @param r1 Red component of the first pixel.
     * @param g1 Green component of the first pixel.
     * @param b1 Blue component of the first pixel.
     * @param r2 Red component of the second pixel.
     * @param g2 Green component of the second pixel.
     * @param b2 Blue component of the second pixel.
     * @return True if the pixel is likely a part of a BW edge
     */
    bool isBWEdge(int r1, int g1, int b1, int r2, int g2, int b2);

public:
    PriceTagDetector(cv::Mat const& _img);
    void detectBWEdges();
};

#endif // PRICETAGDETECTOR_H
