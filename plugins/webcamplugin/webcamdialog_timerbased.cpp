/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>    *
 *   Contributors:                                                         *
 *       Eric Maeker <eric.maeker@gmail.com                                *
 ***************************************************************************/
#include "webcamdialog_timerbased.h"
#include "webcamconstants.h"

#include <coreplugin/constants_icons.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

//#include <translationutils/constants.h>
//#include <translationutils/trans

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QEvent>

using namespace Webcam;
using namespace Internal;

static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

WebCamDialog_TimerBased::WebCamDialog_TimerBased(QWidget *parent) :
    QDialog(parent),
    _cvWidget(new Internal::CvWidget(this))
{
    setObjectName("WebCamDialog_TimerBased");
    setWindowIcon(theme()->icon(Core::Constants::ICONCAMERAVIDEO));
    setWindowTitle(tr("Take a picture from your webcam"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_cvWidget);
    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Close | QDialogButtonBox::Ok, Qt::Horizontal, this);
    _freeze = box->addButton("Freeze", QDialogButtonBox::ActionRole);
    _freeze->setIcon(theme()->icon(Core::Constants::ICONTAKESCREENSHOT));

    QPushButton *but;
    but = box->button(QDialogButtonBox::Close);
    but->setIcon(theme()->icon(Core::Constants::ICONQUIT));

    but = box->button(QDialogButtonBox::Ok);
    but->setIcon(theme()->icon(Core::Constants::ICONOK));

    layout->addWidget(box);
    setLayout(layout);
    resize(500, 400);

    connect(_freeze, SIGNAL(clicked()), this, SLOT(toggleFreezeMode()));
    connect(box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(box, SIGNAL(rejected()), this, SLOT(reject()));
}

QPixmap WebCamDialog_TimerBased::photo() const
{
    return QPixmap(*_cvWidget->pixmap());
}

void WebCamDialog_TimerBased::toggleFreezeMode()
{
    _cvWidget->toggleFreezeMode();
    if (_cvWidget->isFrozen()) {
        _freeze->setText(tr("Unfreeze"));
        _freeze->setIcon(theme()->icon(Core::Constants::ICONCAMERAVIDEO));
    } else {
        _freeze->setText(tr("Freeze"));
        _freeze->setIcon(theme()->icon(Core::Constants::ICONTAKESCREENSHOT));
    }
}

void WebCamDialog_TimerBased::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        if (_cvWidget->isFrozen())
            _freeze->setText(tr("Unfreeze"));
        else
            _freeze->setText(tr("Freeze"));
    }
}



CvWidget::CvWidget(QWidget *parent) :
    QLabel(parent),
    frozen(false),
    _updateFreq(defaultUpdateFrequency())
{
    _camera = cvCreateCameraCapture(CV_CAP_ANY);
    Q_ASSERT(_camera);

    // Log some info about webcam image
    IplImage *image = cvQueryFrame(_camera);
    Q_ASSERT(image);
    printf("Image depth=%i\n", image->depth);
    printf("Image nChannels=%i\n", image->nChannels);

    // Create the QImage
    _image = QImage(100,100,QImage::Format_RGB32);
    for (int x = 0; x < 100; x ++) {
        for (int y =0; y < 100; y++) {
            _image.setPixel(x,y,qRgb(x, y, y));
        }
    }
    setPixmap(QPixmap::fromImage(_image));
    _timerId = startTimer(_updateFreq);
}

CvWidget::~CvWidget()
{
    cvReleaseCapture(&_camera);
}

void CvWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    IplImage *cvimage = cvQueryFrame(_camera);

    int cvIndex, cvLineStart;
    // switch between bit depths
    switch (cvimage->depth) {
    case IPL_DEPTH_8U:
        switch (cvimage->nChannels) {
        case 3:
            if ( (cvimage->width != _image.width()) || (cvimage->height != _image.height()) ) {
                QImage temp(cvimage->width, cvimage->height, QImage::Format_RGB32);
                _image = temp;
            }
            cvIndex = 0; cvLineStart = 0;
            for (int y = 0; y < cvimage->height; y++) {
                unsigned char red,green,blue;
                cvIndex = cvLineStart;
                for (int x = 0; x < cvimage->width; x++) {
                    // DO it
                    red = cvimage->imageData[cvIndex+2];
                    green = cvimage->imageData[cvIndex+1];
                    blue = cvimage->imageData[cvIndex+0];

                    _image.setPixel(x,y,qRgb(red, green, blue));
                    cvIndex += 3;
                }
                cvLineStart += cvimage->widthStep;
            }
            break;
        default:
            printf("This number of channels is not supported\n");
            break;
        }
        break;
    default:
        printf("This type of IplImage is not implemented in QOpenCVWidget\n");
        break;
    }
    setPixmap(QPixmap::fromImage(_image));
}

void CvWidget::toggleFreezeMode()
{
    if (frozen) {
        // Unfreeze
        _timerId = startTimer(_updateFreq);  // 0.1-second timer
    } else {
        // Freeze
        if (_timerId > 0)
            killTimer(_timerId);
    }
    frozen = !frozen;
}

void CvWidget::setImageUpdateFrequency(const int ms)
{
    Q_ASSERT(ms > 0);
    if (_timerId > 0)
        killTimer(_timerId);
    _updateFreq = ms;
    startTimer(_updateFreq);
}

int CvWidget::defaultUpdateFrequency() const
{
    return 50;
}
