#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

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

    void on_loadRefBtn_clicked();

    void on_takePicBtn_clicked();

    void on_webcamBtn_clicked();

private:
    volatile bool webcam_on;

    Ui::MainWindow *ui;

    QGraphicsScene *scene;

    cv::Mat refImage;
    cv::Mat webcamImg;
    cv::VideoCapture capture;
};

#endif // MAINWINDOW_H
