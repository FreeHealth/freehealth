#include "webcamphotoprovider.h"

namespace Webcam {
namespace Internal {

QString WebcamPhotoProvider::recievePhotoFile()
{
    return "/tmp/photo.png";
}

WebcamPhotoProvider::WebcamPhotoProvider()
{
}

} // end Internal
} // end Webcam
