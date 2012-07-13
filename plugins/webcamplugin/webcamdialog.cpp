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

using namespace Webcam;
using namespace Internal;

static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

WebCamDialog::WebCamDialog(QWidget *parent) :
    QDialog(parent),
    m_openCVWidget(new Internal::OpenCVWidget(this))
{
    setObjectName("WebCamDialog");
    setWindowIcon(theme()->icon(Core::Constants::ICONCAMERAVIDEO));
    setWindowTitle(tr("Take a picture from your webcam"));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_openCVWidget);
    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal, this);
    m_freezeButton = box->addButton(tr("Freeze"), QDialogButtonBox::ActionRole);
    m_freezeButton->setIcon(theme()->icon(Core::Constants::ICONTAKESCREENSHOT));

    QPushButton *cancelButton;
    cancelButton = box->button(QDialogButtonBox::Cancel);
    cancelButton->setIcon(theme()->icon(Core::Constants::ICONQUIT));

    cancelButton = box->button(QDialogButtonBox::Ok);
    cancelButton->setIcon(theme()->icon(Core::Constants::ICONOK));

    layout->addWidget(box);
    setLayout(layout);
    resize(500, 400);

    connect(m_freezeButton, SIGNAL(clicked()), this, SLOT(toggleFreezeMode()));
    connect(box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(box, SIGNAL(rejected()), this, SLOT(reject()));
}

QPixmap WebCamDialog::photo() const
{
    return QPixmap(*m_openCVWidget->pixmap());
}

void WebCamDialog::toggleFreezeMode()
{
    m_openCVWidget->toggleFreezeMode();
    if (m_openCVWidget->isFrozen()) {
        m_freezeButton->setText(tr("Unfreeze"));
        m_freezeButton->setIcon(theme()->icon(Core::Constants::ICONCAMERAVIDEO));
    } else {
        m_freezeButton->setText(tr("Freeze"));
        m_freezeButton->setIcon(theme()->icon(Core::Constants::ICONTAKESCREENSHOT));
    }
}

void WebCamDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        if (m_openCVWidget->isFrozen())
            m_freezeButton->setText(tr("Unfreeze"));
        else
            m_freezeButton->setText(tr("Freeze"));
    }
}
