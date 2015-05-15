#include "qpiclabel.h"

#include <iostream>

#include <Qt>
//#include <QImage>
//#include <QResizeEvent>
//#include <QSize>
//#include <QSizePolicy>
#include <QMenu>
//#include <QPoint>
#include <QFileDialog>

#include <sstream>
#include <ctime>
#include <cstdlib>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

QPicLabel::QPicLabel(QWidget *parent) :
    QLabel(parent)
{
    QSizePolicy p = sizePolicy();
    p.setHeightForWidth(true);
    setSizePolicy(p);
    createTime = std::rand();

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));
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
    w = (float)(width()) * 0.95f;
    h = (float)(height()) * 0.95f;


    setPixmap(QPixmapFromCvMat(cvImg).scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//    repaint();
}

void QPicLabel::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(!cvImg.empty())
    {
        std::ostringstream winname;
        winname << "CV image " << createTime;
        cv::namedWindow(winname.str().c_str(), CV_WINDOW_NORMAL);
        cv::imshow(winname.str().c_str(), cvImg);
    }
}

void QPicLabel::setCVImage(const cv::Mat &image)
{
    cvImg = image;
    int w, h;
    w = (float)(width()) * 0.95f;
    h = (float)(height()) * 0.95f;
    setPixmap(QPixmapFromCvMat(cvImg).scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    repaint();
}

void QPicLabel::action_SaveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save image to...", QString(),
                                                    "Image Files (*.png *.jpg *.jpeg *.bmp *.tif)");
    if(fileName.isEmpty())
    {
        return;
    }
    std::string fn = fileName.toStdString();
    if(fn.find_last_of(".") == fn.size())
    {
        fn.append("jpg");
    }

    if(fn.find_last_of(".") == std::string::npos)
    {
        fn.append(".jpg");
    }

    cv::imwrite(fn, cvImg);
}

void QPicLabel::showContextMenu(const QPoint &p)
{
    QPoint posGlobal = mapToGlobal(p);
    QMenu menu;
    menu.addAction("Save picture", this, SLOT(action_SaveImage()));
    menu.exec(posGlobal);
}
