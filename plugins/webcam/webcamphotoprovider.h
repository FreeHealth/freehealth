#ifndef WEBCAMPHOTOPROVIDER_H
#define WEBCAMPHOTOPROVIDER_H

#include <QObject>
#include <patientbaseplugin/iphotoprovider.h>

namespace Webcam {
namespace Internal {

class WebcamPhotoProvider : public Patients::IPhotoProvider
{
    Q_OBJECT

public:
    explicit WebcamPhotoProvider();
    ~WebcamPhotoProvider();
    QString recievePhotoFile();

signals:
    
public slots:
    
};

} // end Internal
} // end Webcam

#endif // WEBCAMPHOTOPROVIDER_H
