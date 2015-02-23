#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

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

    void on_loadBtn1_clicked();

    void on_loadBtn2_clicked();

    void on_webcamBtn_clicked();

private:
    Ui::MainWindow *ui;

    cv::Mat image1;
    cv::Mat image2;
    cv::Mat webcamImg;
    cv::VideoCapture capture;
};

#endif // MAINWINDOW_H
