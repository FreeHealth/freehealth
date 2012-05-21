#include "webcamphotoprovider.h"
#include <QtGui/QDialog>
#include <QtGui/QPushButton>

#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <utils/global.h>

#include <webcamdialog.h>

using namespace cv;

namespace Webcam {
namespace Internal {

WebcamPhotoProvider::WebcamPhotoProvider()
{
}

/*!
  \brief returns Photo that is captured by the selected webcam.

  All of the code is in this function, there is no async calling, because
  the dialog should be modal and no other functions should be possible.
*/
QString WebcamPhotoProvider::recievePhotoFile()
{
    WebcamDialog dialog(0);

    // Escape button pressed, etc.
    if(dialog.exec() != QDialog::Accepted) {
        return QString();
    }
    // ok, here we go:
    return QString();
    // return &dialog.photo();
}


WebcamPhotoProvider::~WebcamPhotoProvider()
{
}

} // end Internal
} // end Webcam
