#include "qopencvwidget.h"


QOpenCVWidget::QOpenCVWidget(QWidget *parent) :
    QWidget(parent)
{
    layout = new QVBoxLayout();
    imageLabel = new QLabel();
    QImage image = QImage(100, 100, QImage::Format_RGB32);
    layout->addWidget(imageLabel);
    for (int x = 0; x < 100; x ++) {
        for (int y =0; y < 100; y++) {
            image.setPixel(x,y,qRgb(x, y, y));
        }
    }
    imageLabel->setPixmap(QPixmap::fromImage(image));
    setLayout(layout);
}

QOpenCVWidget::~QOpenCVWidget() {
}


void QOpenCVWidget::putImage(IplImage *cvimage) {

    int cvIndex, cvLineStart;

    // switch between bit depths
    switch (cvimage->depth) {
    case IPL_DEPTH_8U:
        switch (cvimage->nChannels) {
        case 3:
            if ( (cvimage->width != image.width()) || (cvimage->height != image.height()) ) {
                QImage temp(cvimage->width, cvimage->height, QImage::Format_RGB32);
                image = temp;
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

                    image.setPixel(x,y,qRgb(red, green, blue));
                    cvIndex += 3;
                }
                cvLineStart += cvimage->widthStep;
            }
            break;
        default:
            qWarning() << "This number of channels is not supported.";
            break;
        }
        break;
    default:
        qWarning() << "This type of IplImage is not implemented in QOpenCVWidget.";
        break;
    }
    imageLabel->setPixmap(QPixmap::fromImage(image));
}
