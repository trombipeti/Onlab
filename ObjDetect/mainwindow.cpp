#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <vector>

#include <QFileDialog>
#include <QDir>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    ui{new Ui::MainWindow}
{
    ui->setupUi(this);
    minHessian = 400;
    ui->hessianSlider->setValue(minHessian);
    detectRan = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawMatOnLabel(const cv::Mat &mat, QLabel *label)
{
    QImage img;
    img = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped();
    QPixmap pixmap = QPixmap::fromImage(img);
    label->setPixmap(pixmap);
    label->resize(label->pixmap()->size());
    std::cout << "Label size after Mat draw:" << std::endl;
    std::cout << label->width() << "x" << label->height() << std::endl;
}

void MainWindow::on_loadRefBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), QDir::homePath(),
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    if(fileName.isEmpty())
    {
        return;
    }
    refImage = cv::imread(fileName.toUtf8().data());

    int w = 300;
    int h = refImage.rows * (300.0f / refImage.cols);
    cv::Size newSize(w, h);
    cv::resize(refImage, refImageQT, newSize);

    drawMatOnLabel(refImageQT, ui->refPicLabel);
    detectRan = false;
}

void MainWindow::on_loadDetectBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), QDir::homePath(),
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    if(fileName.isEmpty())
    {
        return;
    }
    testImage = cv::imread(fileName.toUtf8().data());

    int w = 300;
    int h = testImage.rows * (300.0f / testImage.cols);
    cv::Size newSize(w, h);
    cv::resize(testImage, testImageQT, newSize);

    drawMatOnLabel(testImageQT, ui->testPicLabel);
    detectRan = false;
}

void MainWindow::detectObjects()
{
    if(refImage.empty() || testImage.empty())
    {
        return;
    }
    cv::SurfFeatureDetector detector{minHessian};

    std::vector<cv::KeyPoint> refKP, testKP;

    cv::Mat refImageBW, testImageBW;
    cv::cvtColor(refImage, refImageBW, CV_BGR2GRAY);
    cv::cvtColor(testImage, testImageBW, CV_BGR2GRAY);

    cv::GaussianBlur(refImageBW, refImageBW, cv::Size(3,3), 0.5);
    cv::GaussianBlur(testImageBW, testImageBW , cv::Size(3,3), 0.5);

    detector.detect(refImageBW, refKP);
    detector.detect(testImageBW, testKP);

    cv::Mat refDesc, testDesc;

    cv::SurfDescriptorExtractor extractor;

    extractor.compute(refImageBW, refKP, refDesc);
    extractor.compute(testImageBW, testKP, testDesc);

    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;

    matcher.match(refDesc, testDesc, matches);

    double maxDst = 0;
    double minDst = 100;

    for( int i = 0; i < refDesc.rows; i++ )
    {
        double dist = matches[i].distance;
        if( dist < minDst ) minDst = dist;
        if( dist > maxDst ) maxDst = dist;
    }
    std::cout << "Min - max " << minDst << " - " << maxDst << std::endl;

    std::vector< cv::DMatch > good_matches;

    for( int i = 0; i < refDesc.rows; i++ )
    {
        if( matches[i].distance <= 3*minDst )
        {
            good_matches.push_back( matches[i]);
        }
    }

    cv::Mat matchImg;

    cv::drawMatches(refImageBW, refKP, testImageBW, testKP, matches, matchImg, cv::Scalar::all(-1), cv::Scalar::all(-1),
                    std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);


    std::vector<cv::Point2f> ref;
    std::vector<cv::Point2f> test;

    for( size_t i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        ref.push_back( refKP[ good_matches[i].queryIdx ].pt );
        test.push_back( testKP[ good_matches[i].trainIdx ].pt );
    }

    cv::Mat H = cv::findHomography( ref, test, CV_RANSAC );

    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<cv::Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0);
    obj_corners[1] = cvPoint( refImageBW.cols, 0 );
    obj_corners[2] = cvPoint( refImageBW.cols, refImageBW.rows );
    obj_corners[3] = cvPoint( 0, refImageBW.rows );
    std::vector<cv::Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);

    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
    cv::line( matchImg, scene_corners[0] + cv::Point2f( refImageBW.cols, 0), scene_corners[1] + cv::Point2f( refImageBW.cols, 0), cv::Scalar(0, 255, 0), 4 );
    cv::line( matchImg, scene_corners[1] + cv::Point2f( refImageBW.cols, 0), scene_corners[2] + cv::Point2f( refImageBW.cols, 0), cv::Scalar( 0, 255, 0), 4 );
    cv::line( matchImg, scene_corners[2] + cv::Point2f( refImageBW.cols, 0), scene_corners[3] + cv::Point2f( refImageBW.cols, 0), cv::Scalar( 0, 255, 0), 4 );
    cv::line( matchImg, scene_corners[3] + cv::Point2f( refImageBW.cols, 0), scene_corners[0] + cv::Point2f( refImageBW.cols, 0), cv::Scalar( 0, 255, 0), 4 );


    int w = 800;
    int h = matchImg.rows * (800.0f / matchImg.cols);
    cv::Size newSize(w, h);

    cv::resize(matchImg, matchImg, newSize);

    drawMatOnLabel(matchImg, ui->detectLabel);
    detectRan = true;
}

void MainWindow::on_detectBtn_clicked()
{
    detectObjects();
}

void MainWindow::on_hessianSlider_valueChanged(int value)
{
    minHessian = value;
    if(detectRan)
    {
        detectObjects();
    }
}
