#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    scene = nullptr;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    if(capture.isOpened())
    {
        capture.release();
    }
    delete scene;
    delete ui;
}

void MainWindow::on_loadBtn1_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), ".",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    if(fileName.isEmpty())
    {
        return;
    }
    refImage = cv::imread(fileName.toUtf8().data());
    // Show on left label
    int win_width = 600;
    int win_height = (600.0/(float)refImage.cols) * refImage.rows;
//    cv::resize(image,image,imgsize);

    cv::Mat bwImage;
    cv::cvtColor(refImage, bwImage, CV_RGB2GRAY);

    std::vector<cv::KeyPoint> keypoints;
    cv::GoodFeaturesToTrackDetector gftt(
        500,  // maximum number of corners to be returned
        0.01, // quality level
        10);  // minimum allowed distance between points

    gftt.detect(bwImage,keypoints);
    cv::drawKeypoints(refImage,keypoints,refImage,cv::Scalar::all(-1),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    cv::namedWindow("First", CV_WINDOW_NORMAL);
    cv::resizeWindow("First", win_width, win_height);
    cv::imshow("First", refImage);


    QVector<QRgb> colorTable;
    for (int i = 0; i < 256; i++) colorTable.push_back(qRgb(i, i, i));

////    img.setColorTable(colorTable);

    QImage img;
//    cv::cvtColor(refImage,refImage,CV_BGR2RGB);
    std::cout << refImage.channels() << std::endl;
    img = QImage((const unsigned char*)(refImage.data), refImage.cols, refImage.rows, refImage.step, QImage::Format_RGB888 ).rgbSwapped();

    QPixmap pixmap = QPixmap::fromImage(img);
    //    ui->refLabel->setText("Betöltve");
    ui->refLabel->setPixmap(pixmap);
    ui->refLabel->resize(ui->refLabel->pixmap()->size());
}

void MainWindow::on_loadBtn2_clicked()
{
    if(capture.isOpened())
    {
        capture.release();
    }
    capture = cv::VideoCapture(0);
    if(! capture.isOpened())
    {
        QMessageBox mb;
        mb.setText("Cannot open webcam");
        mb.exec();
        return;
    }
//    while(1)
//    {
        cv::Mat webcam, gray;
        capture >> webcam;

        cv::resize(webcam, webcam, cv::Size(200,150));

    //    img.setColorTable(colorTable);

        cv::namedWindow("Valami");
        cv::imshow("Valami", webcam);

        QImage img;
//        cv::cvtColor(webcam, gray, CV_BGR2GRAY);
        img = QImage((const unsigned char*)gray.data, gray.cols, gray.rows, gray.step, QImage::Format_RGB888);
        QPixmap pixmap = QPixmap::fromImage(img);
        ui->refLabel->setText("Betöltve");
        ui->refLabel->setPixmap(pixmap);
        ui->refLabel->resize(ui->refLabel->pixmap()->size());

//    }
    capture.release();
}


void MainWindow::on_webcamBtn_clicked()
{
    static volatile bool webcam_on = false;
    if(capture.isOpened())
    {
        ui->webcamBtn->setText(tr("Open webcam"));
        capture.release();
        webcam_on = false;
        return;
    }
    capture = cv::VideoCapture(0);
    if(! capture.isOpened())
    {
        QMessageBox mb;
        mb.setText("Cannot open webcam");
        mb.exec();
        return;
    }
    webcam_on = true;
    ui->webcamBtn->setText(tr("Take picture"));
    while(webcam_on)
    {
        cv::Mat grayImg;
        capture >> webcamImg;

        cv::cvtColor(webcamImg, grayImg, CV_BGR2GRAY);

        cv::GaussianBlur(grayImg, grayImg, cv::Size(3,3), 0);

        std::vector<cv::KeyPoint> keypoints;
//        cv::GoodFeaturesToTrackDetector gftt(
//                    500,  // maximum number of corners to be returned
//                    0.01, // quality level
//                    10);  // minimum allowed distance between points

//        gftt.detect(grayImg,keypoints);

        cv::SiftFeatureDetector sift;
        sift.detect(grayImg,keypoints);
        cv::drawKeypoints(webcamImg,keypoints,webcamImg, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


        QImage img;
        img = QImage((const unsigned char*)webcamImg.data, webcamImg.cols, webcamImg.rows, webcamImg.step, QImage::Format_RGB888).rgbSwapped();
        QPixmap pixmap = QPixmap::fromImage(img);
        ui->refLabel->setPixmap(pixmap);
        ui->refLabel->resize(ui->refLabel->pixmap()->size());

        if((cv::waitKey(10)) >= 0)
        {
            break;
        }
    }
    capture.release();
}
