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
#include "webcamdialog.h"
#include "ui_webcamdialog.h"
#include "webcamconstants.h"
#include "opencvwidget.h"

#include <coreplugin/constants_icons.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

//#include <translationutils/constants.h>
//#include <translationutils/trans

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QEvent>
#include <QDebug>

using namespace Webcam;
using namespace Internal;

static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

WebcamDialog::WebcamDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebcamDialog())
{
    ui->setupUi(this);
//    qDebug() << this->objectName();
    setObjectName("WebCamDialog");
    setWindowIcon(theme()->icon(Core::Constants::ICONCAMERAVIDEO));
    setWindowTitle(tr("Take a picture from your webcam"));
    
//    ui->openCVWidget = new Internal::OpenCVWidget(this);
    
    ui->openCVWidget->setToolTip(tr("<ul>"
                                  "<li>Click on 'Freeze'</li>"
                                  "<li>draw a frame with the mouse</li>"
                                  "<li>use the mouse wheel to set the size of the frame</li></ul>"));
    
    m_freezeButton = ui->buttonBox->addButton(tr("Freeze"), QDialogButtonBox::ActionRole);
    m_freezeButton->setIcon(theme()->icon(Core::Constants::ICONTAKESCREENSHOT));
    
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setIcon(theme()->icon(Core::Constants::ICONQUIT));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setIcon(theme()->icon(Core::Constants::ICONOK));
    
    connect(m_freezeButton, SIGNAL(clicked()), this, SLOT(toggleFreezeMode()));
}

WebcamDialog::~WebcamDialog()
{
    delete ui;
}

QPixmap WebcamDialog::photo() const
{
    return QPixmap(*ui->openCVWidget->pixmap());
}

void WebcamDialog::toggleFreezeMode()
{
    ui->openCVWidget->toggleFreezeMode();
    if (ui->openCVWidget->isFrozen()) {
        m_freezeButton->setText(tr("Unfreeze"));
        m_freezeButton->setIcon(theme()->icon(Core::Constants::ICONCAMERAVIDEO));
    } else {
        m_freezeButton->setText(tr("Freeze"));
        m_freezeButton->setIcon(theme()->icon(Core::Constants::ICONTAKESCREENSHOT));
    }
}

void WebcamDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        if (ui->openCVWidget->isFrozen())
            m_freezeButton->setText(tr("Unfreeze"));
        else
            m_freezeButton->setText(tr("Freeze"));
    }
}
