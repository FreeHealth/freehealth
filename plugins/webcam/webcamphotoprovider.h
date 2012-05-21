#ifndef WEBCAMPHOTOPROVIDER_H
#define WEBCAMPHOTOPROVIDER_H

#include <QObject>
#include <patientbaseplugin/iphotoprovider.h>
#include <webcam_exporter.h>
#include <webcamdialog.h>

namespace Webcam {
namespace Internal {

/*! \class Webcam::Internal::WebcamPhotoProvider
 * \brief Implements the Patients::IPhotoProvider interface
 *
 *
 */
class WEBCAMSHARED_EXPORT WebcamPhotoProvider : public Patients::IPhotoProvider
{
    Q_OBJECT
public:
    WebcamPhotoProvider();
    ~WebcamPhotoProvider();
    QString recievePhotoFile();
};

} // end Internal
} // end Webcam

#endif // WEBCAMPHOTOPROVIDER_H
