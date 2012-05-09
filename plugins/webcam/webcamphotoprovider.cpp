#include "webcamphotoprovider.h"
#include <QtGui/QDialog>
#include <QtGui/QPushButton>
//#include <QtGui/QVBoxLayout>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <utils/global.h>
#include <QtMultimediaKit/QCamera>

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
    webCamWindow->setLayout(layout);

    m_cvWidget = new QOpenCVWidget(webCamWindow);
    layout->addWidget(m_cvWidget);
//    webCamWindow->resize(500, 400);

    QPushButton *takePhotoButton = new QPushButton(tr("Take Photo"));
    takePhotoButton->setIcon(Core::ICore::instance()->theme()->icon("takescreenshot.png"));
    layout->addWidget(takePhotoButton);

    startTimer(200);  // 0.2-second timer

    // create a OpenCV camera capture on device 0
    // TODO: add settings to change device
    m_camera = cvCreateCameraCapture(0);

    if(!m_camera) {
        Utils::warningMessageBox("text","infotext", "detail", "title");
        cvReleaseCapture(&m_camera);
        return QString();
    }
    webCamWindow->show();

    if (webCamWindow->exec() == QDialog::Accepted) {
        delete webCamWindow;
        return QString("");
    } else {
        cvReleaseCapture(&m_camera);
        delete webCamWindow;
        return QString();
    }
}



WebcamPhotoProvider::~WebcamPhotoProvider()
{
}

void WebcamPhotoProvider::timerEvent(QTimerEvent*) {
    IplImage *image = cvQueryFrame(m_camera);
    m_cvWidget->putImage(image);
}



} // end Internal
} // end Webcam
