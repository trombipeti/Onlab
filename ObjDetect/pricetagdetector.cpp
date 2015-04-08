#include "pricetagdetector.h"

#include <iostream>

#include <opencv2/core/core.hpp>
#include <vector>

void PriceTagDetector::detectBWEdges()
{
    std::vector<cv::Mat> channels;
    cv::split(img, channels);
}

PriceTagDetector::PriceTagDetector(cv::Mat const& _img)
{
    img = _img.clone();
}
