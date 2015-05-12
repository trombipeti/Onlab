#ifndef QPICLABEL_H
#define QPICLABEL_H

#include <QLabel>
#include <QPixmap>
#include <QImage>

#include <ctime>

#include <opencv2/core/core.hpp>

class QPicLabel : public QLabel
{
    Q_OBJECT
public:
    explicit QPicLabel(QWidget *parent = 0);

    void setCVImage(const cv::Mat& image);
signals:

private slots:
    void action_SaveImage();

public slots:
    void showContextMenu(const QPoint& p);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent * event) override;
private:
    cv::Mat cvImg;
    int createTime;
};

#endif // QPICLABEL_H
