#ifndef PRICETAGDETECTOR_H
#define PRICETAGDETECTOR_H

#include <opencv2/core/core.hpp>

class PriceTagDetector
{
private:
    cv::Mat img;
public:
    PriceTagDetector(cv::Mat const& _img);
    void detectBWEdges();
};

#endif // PRICETAGDETECTOR_H
