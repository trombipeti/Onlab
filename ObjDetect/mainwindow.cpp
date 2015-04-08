#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <vector>

#include <QFileDialog>
#include <QDir>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "imagematcher.h"
#include "pricetagdetector.h"

//#include <opencv2/nonfree/nonfree.hpp>
//#include <opencv2/calib3d/calib3d.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    ui{new Ui::MainWindow}
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeImg(cv::Mat& img, cv::Mat& dest, int width, int height, bool keepPersp)
{
    if(keepPersp)
    {
        cv::Size newSize;
        bool wBigger = (width > height);
        if(wBigger)
        {
            newSize.width = width;
            newSize.height = img.rows * ((float)newSize.width / img.cols);
        }
        else
        {
            newSize.height = height;
            newSize.width = img.cols * ((float)newSize.height / img.rows);
        }
        cv::resize(img, dest, newSize);
    }
    else
    {
        cv::resize(img, dest, cv::Size(width, height));
    }
}

void MainWindow::on_loadRefBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image Of Object"), "/home/trombipeti/Képek/ObjDetect",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    if(fileName.isEmpty())
    {
        return;
    }
    refImage = cv::imread(fileName.toUtf8().data());

    resizeImg(refImage, refImage, 1000, 1000);

    PriceTagDetector pt(refImage);

    pt.detectBWEdges();

    ui->refPicLabel->setCVImage(refImage);

}

void MainWindow::on_loadDetectBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image Of Scene"), "/home/trombipeti/Képek/ObjDetect",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    if(fileName.isEmpty())
    {
        return;
    }
    testImage = cv::imread(fileName.toUtf8().data());

    resizeImg(testImage, testImage, 1000, 1000);

    ui->testPicLabel->setCVImage(testImage);
}

void MainWindow::detectObjects()
{
    if(refImage.empty() || testImage.empty())
    {
        return;
    }

    cv::Mat matchImg(300, 300, CV_8UC3, cv::Scalar(0,0,0));
//    std::vector<cv::Point2f> refCorners(4);
//    refCorners[0] = cvPoint(0,0);
//    refCorners[1] = cvPoint( refImageBW.cols, 0 );
//    refCorners[2] = cvPoint( refImageBW.cols, refImageBW.rows );
//    refCorners[3] = cvPoint( 0, refImageBW.rows );
//    std::vector<cv::Point2f> foundObjCorners(4);


////    cv::perspectiveTransform( refCorners, foundObjCorners, H);

////    cv::Mat detectImg = testImage.clone();

//    cv::line( matchImg, foundObjCorners[0] + cv::Point2f( refImageBW.cols, 0), foundObjCorners[1] + cv::Point2f( refImageBW.cols, 0), cv::Scalar(0, 255, 0), 4 );
//    cv::line( matchImg, foundObjCorners[1] + cv::Point2f( refImageBW.cols, 0), foundObjCorners[2] + cv::Point2f( refImageBW.cols, 0), cv::Scalar( 0, 255, 0), 4 );
//    cv::line( matchImg, foundObjCorners[2] + cv::Point2f( refImageBW.cols, 0), foundObjCorners[3] + cv::Point2f( refImageBW.cols, 0), cv::Scalar( 0, 255, 0), 4 );
//    cv::line( matchImg, foundObjCorners[3] + cv::Point2f( refImageBW.cols, 0), foundObjCorners[0] + cv::Point2f( refImageBW.cols, 0), cv::Scalar( 0, 255, 0), 4 );

    ui->detectLabel->setCVImage(matchImg);
}

void MainWindow::on_detectBtn_clicked()
{
//    detectObjects();
    ImageMatcher matcher(refImage, testImage);
    if(matcher.classify(true))
    {
        ui->detectLabel->setText("Egyezik");
    }
    else
    {
        ui->detectLabel->setText("Nem egyezik");
    }
}
