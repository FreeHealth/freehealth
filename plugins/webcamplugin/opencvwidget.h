#ifndef OPENCVWIDGET_H
#define OPENCVWIDGET_H

#include <QLabel>

#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace Webcam {
namespace Internal {

class OpenCVWidget : public QLabel
{
public:
    OpenCVWidget(QWidget *parent = 0);
    ~OpenCVWidget();

    void toggleFreezeMode();
    bool isFrozen() const {return frozen;}

    void setImageUpdateFrequency(const int ms);
    int defaultUpdateFrequency() const;

private:
    void timerEvent(QTimerEvent *event);

private:
    QImage _image;
    CvCapture *_camera;
    bool frozen;
    int _timerId, _updateFreq;
};

} // end Internal
} // end Webcam

#endif // OPENCVWIDGET_H
