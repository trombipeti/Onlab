#include "qtcv.h"

#include <opencv2/core/core.hpp>
#include <QLabel>
#include <QPixmap>
#include <QImage>

void QtCV::drawMatOnLabel(const cv::Mat &mat, QLabel *label)
{
    QImage img;
    img = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped();
    QPixmap pixmap = QPixmap::fromImage(img);
    label->setPixmap(pixmap);
    label->resize(label->pixmap()->size());
}
