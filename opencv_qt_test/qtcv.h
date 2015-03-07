#ifndef QTCV_H
#define QTCV_H

#include <opencv2/core/core.hpp>
#include <QLabel>

class QtCV
{
public:
    static void drawMatOnLabel(cv::Mat const& mat, QLabel *label);
};

#endif // QTCV_H
