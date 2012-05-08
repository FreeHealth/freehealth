#ifndef IPHOTOPROVIDER_H
#define IPHOTOPROVIDER_H

#include <patientbaseplugin/patientbase_exporter.h>
#include <QObject>


namespace Patients {
/*!
  \class Patients::IPhotoProvider
  \brief Provides a hook for plugins that return a photo path.

  Implement this interface if you want to provide another way of
  fetching a foto for the user instead of the default (searching for it
  in the files system).
  Possibilities are webcams, scanners, other databases, LDAP, etc.

  The recievePhotoFile() method should return a filename of an \e existing photo.
 */

class PATIENT_EXPORT IPhotoProvider : public QObject
{
    Q_OBJECT
protected:
    explicit IPhotoProvider(QObject *parent = 0) : QObject(parent) {}
public:
    virtual ~IPhotoProvider() {}

    virtual QString recievePhotoFile() = 0;
};

} // end namespace Patients

#endif // IPHOTOPROVIDER_H
