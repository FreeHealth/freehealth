#include "opencvwidget.h"

using namespace Webcam::Internal;

OpenCVWidget::OpenCVWidget(QWidget *parent) :
    QLabel(parent),
    frozen(false),
    _updateFreq(defaultUpdateFrequency())
{
    _camera = cvCreateCameraCapture(CV_CAP_ANY);
    Q_ASSERT(_camera);

    // Log some info about webcam image
    IplImage *image = cvQueryFrame(_camera);
    Q_ASSERT(image);
    printf("Image depth=%i\n", image->depth);
    printf("Image nChannels=%i\n", image->nChannels);

    // Create the QImage
    _image = QImage(100,100,QImage::Format_RGB32);
    for (int x = 0; x < 100; x ++) {
        for (int y =0; y < 100; y++) {
            _image.setPixel(x,y,qRgb(x, y, y));
        }
    }
    setPixmap(QPixmap::fromImage(_image));
    _timerId = startTimer(_updateFreq);
}

OpenCVWidget::~OpenCVWidget()
{
    cvReleaseCapture(&_camera);
}

void OpenCVWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    IplImage *cvimage = cvQueryFrame(_camera);

    int cvIndex, cvLineStart;
    // switch between bit depths
    switch (cvimage->depth) {
    case IPL_DEPTH_8U:
        switch (cvimage->nChannels) {
        case 3:
            if ( (cvimage->width != _image.width()) || (cvimage->height != _image.height()) ) {
                QImage temp(cvimage->width, cvimage->height, QImage::Format_RGB32);
                _image = temp;
            }
            cvIndex = 0; cvLineStart = 0;
            for (int y = 0; y < cvimage->height; y++) {
                unsigned char red,green,blue;
                cvIndex = cvLineStart;
                for (int x = 0; x < cvimage->width; x++) {
                    // DO it
                    red = cvimage->imageData[cvIndex+2];
                    green = cvimage->imageData[cvIndex+1];
                    blue = cvimage->imageData[cvIndex+0];

                    _image.setPixel(x,y,qRgb(red, green, blue));
                    cvIndex += 3;
                }
                cvLineStart += cvimage->widthStep;
            }
            break;
        default:
            printf("This number of channels is not supported\n");
            break;
        }
        break;
    default:
        printf("This type of IplImage is not implemented in QOpenCVWidget\n");
        break;
    }
    setPixmap(QPixmap::fromImage(_image));
}

void OpenCVWidget::toggleFreezeMode()
{
    if (frozen) {
        // Unfreeze
        _timerId = startTimer(_updateFreq);  // 0.1-second timer
    } else {
        // Freeze
        if (_timerId > 0)
            killTimer(_timerId);
    }
    frozen = !frozen;
}

void OpenCVWidget::setImageUpdateFrequency(const int ms)
{
    Q_ASSERT(ms > 0);
    if (_timerId > 0)
        killTimer(_timerId);
    _updateFreq = ms;
    startTimer(_updateFreq);
}

int OpenCVWidget::defaultUpdateFrequency() const
{
    return 50;
}
