#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <vector>

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "imagematcher.h"
#include "pricetagdetector.h"
#include "videoptd.h"

//#include <opencv2/nonfree/nonfree.hpp>
//#include <opencv2/calib3d/calib3d.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    ui{new Ui::MainWindow}
{
    ui->setupUi(this);

    ui->goodMatchSlider->setMinimum(7);
    ui->goodMatchSlider->setMaximum(100);
    ui->goodMatchSlider->setSingleStep(1);
    ui->goodMatchSlider->setSingleStep(10);
    ui->goodMatchSlider->setValue(15);

    ui->validMatchSlider->setMinimum(7);
    ui->validMatchSlider->setMaximum(100);
    ui->validMatchSlider->setSingleStep(1);
    ui->validMatchSlider->setSingleStep(10);
    ui->validMatchSlider->setValue(10);

    ui->secondRatioSlider->setMinimum(0);
    ui->secondRatioSlider->setMaximum(100);
    ui->secondRatioSlider->setValue(92);

    ui->minFeatDistSlider->setMinimum(0);
    ui->minFeatDistSlider->setMaximum(200);
    ui->minFeatDistSlider->setValue(25);

    ui->detectBtn->setEnabled(false);
    ui->detectEdgesBtn->setEnabled(false);

    ui->bwEdgeLimitSlider->setMaximum(255);
    ui->bwEdgeLimitSlider->setEnabled(false);

    ui->bwEdgeThreshSlider->setMaximum(255);
    ui->bwEdgeThreshSlider->setEnabled(false);
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
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp *.tif)"));
    if(fileName.isEmpty())
    {
        return;
    }
    sift_QueryImg = cv::imread(fileName.toUtf8().data());

    resizeImg(sift_QueryImg, sift_QueryImg, 1000, 1000);

    ui->refPicLabel->setCVImage(sift_QueryImg);

    if( ! sift_testImage.empty())
    {
        ui->detectBtn->setEnabled(true);
    }

}


void MainWindow::on_loadBwBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), "/home/trombipeti/Képek/ObjDetect",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp *.tif)"));
    if(fileName.isEmpty())
    {
        return;
    }
    bw_QueryImg = cv::imread(fileName.toUtf8().data());

    resizeImg(bw_QueryImg, bw_QueryImg, 1000, 1000);

    ui->bwRefLabel->setCVImage(bw_QueryImg);


    ui->bwEdgeLimitSlider->setEnabled(true);
    ui->bwEdgeLimitSlider->setValue(PriceTagDetector::GetBwEdgeLimit());
    ui->bwEdgeLimitLabel->setText(QString::number(PriceTagDetector::GetBwEdgeLimit()));

    ui->bwEdgeThreshSlider->setEnabled(true);
    ui->bwEdgeThreshSlider->setValue(PriceTagDetector::GetBwEdgeThresh());
    ui->bwEdgeThreshLabel->setText(QString::number(PriceTagDetector::GetBwEdgeThresh()));

    ui->detectEdgesBtn->setEnabled(true);
}

void MainWindow::on_loadDetectBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image Of Scene"), "/home/trombipeti/Képek/ObjDetect",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp *.tif)"));
    if(fileName.isEmpty())
    {
        return;
    }
    sift_testImage = cv::imread(fileName.toUtf8().data());

    resizeImg(sift_testImage, sift_testImage, 1000, 1000);

    ui->testPicLabel->setCVImage(sift_testImage);

    if( ! sift_QueryImg.empty())
    {
        ui->detectBtn->setEnabled(true);
    }
}


void MainWindow::on_detectBtn_clicked()
{
    matcher.setRefImg(sift_QueryImg);
    matcher.setTestImg(sift_testImage);
    cv::Mat matched;
    if(matcher.classify(matched))
    {
        try {
        ui->detectLabel->setCVImage(matched);
        } catch(const char* e) {
            std::cout << "Exception: " << e << std::endl;
        }
    }
    else
    {
        ui->detectLabel->setText("Nem egyezik");
    }
}

void MainWindow::on_goodMatchSlider_valueChanged(int value)
{
    matcher.setMinGoodMatchSize(value);
    ui->goodMatchLabel->setText(QString::number(value));

    if(matcher.getMinValidMatchSize() > (size_t)value)
    {
        ui->validMatchSlider->setValue(value);
    }

    if(ui->validMatchSlider->maximum() > value)
    {
        ui->validMatchSlider->setMaximum(value);
    }
    else if(value < ui->validMatchSlider->minimum())
    {
        ui->validMatchSlider->setMinimum(value - 1);
    }
    else
    {
        ui->validMatchSlider->setMaximum(100);
    }
}

void MainWindow::on_validMatchSlider_valueChanged(int value)
{
    matcher.setMinValidMatchSize(value);
    ui->validMatchLabel->setText(QString::number(value));
}

void MainWindow::on_minFeatDistSlider_valueChanged(int value)
{
    float dist = (float)(value)/100.0f;
    matcher.setMinFeatureDist(dist);
    ui->minFeatDistLabel->setText(QString::number(dist, 'f', 2));
}

void MainWindow::on_secondRatioSlider_valueChanged(int value)
{
    float ratio = (float)(value)/100.0f;
    matcher.setMinSecondTestRatio(ratio);
    ui->secondRatioLabel->setText(QString::number(ratio, 'f', 2));
}

void MainWindow::on_detectEdgesBtn_clicked()
{
    cv::Mat edges;
    PriceTagDetector::DetectBWEdges(bw_QueryImg, edges);
    cv::cvtColor(edges, edges, CV_GRAY2BGR);
    ui->bwEdgesLabel->setCVImage(edges);
}

void MainWindow::on_bwEdgeLimitSlider_valueChanged(int value)
{
    ui->bwEdgeLimitLabel->setText(QString::number(value));
    PriceTagDetector::SetBwEdgeLimit(value);
}

void MainWindow::on_bwEdgeThreshSlider_valueChanged(int value)
{
    ui->bwEdgeThreshLabel->setText(QString::number(value));
    PriceTagDetector::SetBwEdgeThresh(value);
}

void MainWindow::on_actionOpen_video_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open video"), QString(),
                                                    tr("Video Files (*.avi *.mp4)"));
    if(fileName.isEmpty())
    {
        return;
    }
    vptd_p = std::unique_ptr<VideoPTD>( new VideoPTD(fileName.toStdString().c_str()));
    try
    {
        vptd_p.get()->start();
        ui->actionStart_video->setEnabled(true);
    }
    catch(const char* e)
    {
        QMessageBox msgbox;
        msgbox.setText(e);
        msgbox.exec();
    }
}

void MainWindow::on_actionStop_video_triggered()
{
    ui->actionStart_video->setEnabled(true);
    vptd_p.get()->stop();
}

void MainWindow::on_actionStart_video_triggered()
{
    ui->actionStop_video->setEnabled(true);
    vptd_p.get()->start();
}

