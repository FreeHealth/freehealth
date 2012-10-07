#include "urlphotoprovider.h"

#include <coreplugin/iphotoprovider.h>
#include "urlphotodialog.h"

namespace Patients {

UrlPhotoProvider::UrlPhotoProvider(QObject *parent) :
    Core::IPhotoProvider(parent)
{
}

/*! Deletes this Provider from the pool */
UrlPhotoProvider::~UrlPhotoProvider()
{
}

QString UrlPhotoProvider::id() const
{
    return "url";
}

/*! \brief Returns a translated name for this provider. */
QString UrlPhotoProvider::name() const
{
    return "Url";
}

/*! Return the translateable text of the object. */
QString UrlPhotoProvider::displayText() const
{
    return QString("Download from a URL...");
}

/*! \brief Returns the active state of the provider. */
bool UrlPhotoProvider::isActive() const
{
    //TODO: check if there is a network connection!
    return true;
}

/*! \brief Returns the enabled state of the provider. This can be changed in user settings. */
bool UrlPhotoProvider::isEnabled() const
{
    return isActive();
}

/*! \brief Returns a middle-low priority. */
int UrlPhotoProvider::priority() const
{
    return 30;
}

void UrlPhotoProvider::startReceivingPhoto()
{
    UrlPhotoDialog dialog;
    QPixmap photo;
    if (dialog.exec() == QDialog::Accepted)
        photo = dialog.photo();
    Q_EMIT photoReady(photo);
}



} // namespace Patients
