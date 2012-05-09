#ifndef WEBCAMPHOTOPROVIDER_H
#define WEBCAMPHOTOPROVIDER_H

#include <QObject>
#include <patientbaseplugin/iphotoprovider.h>
#include <webcam_exporter.h>
#include <qopencvwidget.h>
//#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace Webcam {
namespace Internal {

class WEBCAMSHARED_EXPORT WebcamPhotoProvider : public Patients::IPhotoProvider
{
    Q_OBJECT

public:
    WebcamPhotoProvider();
    ~WebcamPhotoProvider();
    QString recievePhotoFile();

private:
    QOpenCVWidget *m_cvWidget;
    CvCapture *m_camera;

protected:
    void timerEvent(QTimerEvent*);

};

} // end Internal
} // end Webcam

#endif // WEBCAMPHOTOPROVIDER_H
