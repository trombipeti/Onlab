#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

#include <opencv2/core/core.hpp>


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

    void on_hessianSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    cv::Mat refImage, refImageQT;
    cv::Mat testImage, testImageQT;

    int minHessian;
    bool detectRan;

    void detectObjects();
    void drawMatOnLabel(const cv::Mat &mat, QLabel *label);
};

#endif // MAINWINDOW_H
