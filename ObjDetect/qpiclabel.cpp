#include "qpiclabel.h"

#include <iostream>

#include <QImage>
#include <QResizeEvent>
#include <QSize>

#include <sstream>

#include <opencv2/core/core.hpp>

QPicLabel::QPicLabel(QWidget *parent) :
    QLabel(parent)
{
}

QPixmap QPixmapFromCvMat(const cv::Mat &image)
{
    QImage QIm;
    if(image.channels() == 3)
    {
        QIm = QImage((const unsigned char*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888)
               .rgbSwapped();
    }
    else if(image.channels() == 1)
    {
        QIm = QImage((const unsigned char*)image.data, image.cols, image.rows, image.step, QImage::Format_Mono);
    }
    else
    {
        std::ostringstream s;
        s << "Unhandled image channel number: " << image.channels();
        throw s.str().c_str();
    }
    return QPixmap::fromImage(QIm);
}

void QPicLabel::resizeEvent(QResizeEvent *event)
{
    const QPixmap* pm = pixmap();
    if(pm == nullptr)
    {
        return;
    }

    int w, h;
    w = width();

//    if(event->oldSize().width() > event->size().width())
//    {
//        w -= 10;
//    }

    h = height();

//    if(event->oldSize().height() > event->size().height())
//    {
//        h -= 10;
//    }

    setPixmap(QPixmapFromCvMat(cvImg).scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    std::cout << "resizeEvent: size " << height() << " " << width() << std::endl;
//    repaint();
}

void QPicLabel::setCVImage(const cv::Mat &image)
{
    cvImg = image;
    std::cout << "setCVImage: size " << height() << " " << width() << std::endl;
    setPixmap(QPixmapFromCvMat(cvImg).scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    repaint();
}
