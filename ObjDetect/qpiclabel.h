#ifndef QPICLABEL_H
#define QPICLABEL_H

#include <QLabel>
#include <QPixmap>
#include <QImage>

#include <opencv2/core/core.hpp>

class QPicLabel : public QLabel
{
    Q_OBJECT
public:
    explicit QPicLabel(QWidget *parent = 0);

    void setCVImage(const cv::Mat& image);
signals:

public slots:

protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    cv::Mat cvImg;
};

#endif // QPICLABEL_H
