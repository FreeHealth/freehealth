#include "webcamphotoprovider.h"
#include <QtGui/QDialog>
#include <QtGui/QPushButton>
//#include <QtGui/QVBoxLayout>
#include <opencv/highgui.h>
#include <opencv/cv.h>

namespace Webcam {
namespace Internal {

WebcamPhotoProvider::WebcamPhotoProvider():
    m_camera(0),
    m_cvWidget(0)
{
}

QString WebcamPhotoProvider::recievePhotoFile()
{
    QDialog *webCamWindow = new QDialog();
    QVBoxLayout *layout = new QVBoxLayout;
    m_cvWidget = new QOpenCVWidget(webCamWindow);
    layout->addWidget(m_cvWidget);
    webCamWindow->setLayout(layout);
    webCamWindow->resize(500, 400);

    layout->addWidget(new QPushButton(tr("Take Photo")));
    startTimer(100);  // 0.1-second timer

    m_camera = cvCreateCameraCapture(0);
    assert(m_camera);

    webCamWindow->show();
    if (webCamWindow->exec() == QDialog::Accepted) {
        delete webCamWindow;
        return QString("");
    } else {
        delete webCamWindow;
        return QString();
    }
}



WebcamPhotoProvider::~WebcamPhotoProvider()
{
    cvReleaseCapture(&m_camera);
}

void WebcamPhotoProvider::timerEvent(QTimerEvent*) {
    IplImage *image = cvQueryFrame(m_camera);
    m_cvWidget->putImage(image);
}



} // end Internal
} // end Webcam
