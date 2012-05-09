#ifndef QOPENCVWIDGET_H
#define QOPENCVWIDGET_H

#include <opencv/cv.h>
#include <QPixmap>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QImage>
#include <QDebug>

class QOpenCVWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QOpenCVWidget(QWidget *parent = 0);
    ~QOpenCVWidget();
    void putImage(IplImage *);

private:
    QLabel *imageLabel;
    QVBoxLayout *layout;
    QImage image;
};

#endif // QOPENCVWIDGET_H
