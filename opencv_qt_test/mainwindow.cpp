#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
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
    image1 = cv::imread(fileName.toUtf8().data());
    // Show on left label
    int win_width = 600;
    int win_height = (600.0/(float)image1.cols) * image1.rows;
//    cv::resize(image,image,imgsize);

    cv::cvtColor(image1,image1, CV_RGB2GRAY);

    std::vector<cv::KeyPoint> keypoints;
    cv::GoodFeaturesToTrackDetector gftt(
        500,  // maximum number of corners to be returned
        0.01, // quality level
        10);  // minimum allowed distance between points

    gftt.detect(image1,keypoints);
    cv::drawKeypoints(image1,keypoints,image1,cv::Scalar::all(-1),cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    cv::namedWindow("First", CV_WINDOW_NORMAL);
    cv::resizeWindow("First", win_width, win_height);
    cv::imshow("First", image1);


//    QVector<QRgb> colorTable;
//    for (int i = 0; i < 256; i++) colorTable.push_back(qRgb(i, i, i));

//    QImage img;
//    img = QImage((const unsigned char*)image1.data, image1.rows, image1.cols, image1.step, QImage::Format_RGB888 );
//    img.setColorTable(colorTable);

//    QPixmap pixmap = QPixmap::fromImage(img);
////    ui->origImg->setText("Betöltve");
//    ui->origImg->setPixmap(pixmap);
//    ui->origImg->resize(ui->origImg->pixmap()->size());
}

void MainWindow::on_loadBtn2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), ".",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    if(fileName.isEmpty())
    {
        return;
    }
    image2 = cv::imread(fileName.toUtf8().data());
    // Show on left label
    int win_width = 600;
    int win_height = (600.0/(float)image2.cols) * image2.rows;
//    cv::resize(image,image,imgsize);

    cv::cvtColor(image2,image2, CV_BGR2GRAY);

    std::vector<cv::KeyPoint> keypoints;
    cv::GoodFeaturesToTrackDetector gftt(
        500,  // maximum number of corners to be returned
        0.01, // quality level
        10);  // minimum allowed distance between points

    gftt.detect(image2,keypoints);
    cv::drawKeypoints(image2,keypoints,image2);
//    drawOnImage(image2,keypoints);

    cv::namedWindow("Second", CV_WINDOW_NORMAL);
    cv::resizeWindow("Second", win_width, win_height);
    cv::imshow("Second", image2);
}

