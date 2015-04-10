#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

#include <opencv2/core/core.hpp>

#include "imagematcher.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_loadRefBtn_clicked();

    void on_loadDetectBtn_clicked();

    void on_detectBtn_clicked();

    void on_goodMatchSlider_valueChanged(int value);

    void on_validMatchSlider_valueChanged(int value);

    void on_minFeatDistSlider_valueChanged(int value);

    void on_secondRatioSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    cv::Mat refImage;
    cv::Mat testImage;

    ImageMatcher matcher;

    void resizeImg(cv::Mat& img, cv::Mat& dest, int width, int height, bool keepPersp = true);
};

#endif // MAINWINDOW_H
