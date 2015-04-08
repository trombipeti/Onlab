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

private:
    Ui::MainWindow *ui;

    cv::Mat refImage;
    cv::Mat testImage;

    void detectObjects();
    void resizeImg(cv::Mat& img, cv::Mat& dest, int width, int height, bool keepPersp = true);
};

#endif // MAINWINDOW_H
