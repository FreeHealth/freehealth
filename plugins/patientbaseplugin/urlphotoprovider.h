#ifndef PATIENTS_URLPHOTOPROVIDER_H
#define PATIENTS_URLPHOTOPROVIDER_H

#include <coreplugin/iphotoprovider.h>
#include <QString>

namespace Patients {

class UrlPhotoProvider : public Core::IPhotoProvider
{
    Q_OBJECT
public:
    explicit UrlPhotoProvider(QObject *parent = 0);
    ~UrlPhotoProvider();

    QString id() const;
    QString name() const;
    QString displayText() const;
    bool isActive() const;
    bool isEnabled() const;
    int priority() const;

public Q_SLOTS:
    void startReceivingPhoto();
};

} // namespace Patients

#endif // PATIENTS_URLPHOTOPROVIDER_H
