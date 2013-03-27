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
 *  Contributors:                                                          *
 *       Eric Maeker <eric.maeker@gmail.com                                *
 ***************************************************************************/
#include "webcamdialog.h"
#include "ui_webcamdialog.h"
#include "webcamconstants.h"
#include "opencvwidget.h"

#include <coreplugin/constants_icons.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <utils/global.h>

//#include <translationutils/constants.h>
//#include <translationutils/trans

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>

#include "ui_webcamdialog.h"

using namespace Webcam;
using namespace Internal;

static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

/*! Default constructor, creates the Dialog and initializes the camera and the OpenCVWidget. */
WebcamDialog::WebcamDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebcamDialog())
{
    ui->setupUi(this);
//    qDebug() << this->objectName();
    setObjectName("WebCamDialog");
    setWindowIcon(theme()->icon(Core::Constants::ICONCAMERAVIDEO));
    setWindowTitle(tr("Take a picture from your webcam"));

    // Freeze button
    m_freezeButton = ui->buttonBox->addButton(tr("Stop"), QDialogButtonBox::ActionRole);
    m_freezeButton->setIcon(theme()->icon(Core::Constants::ICONMEDIAPAUSE));
    m_freezeButton->setCheckable(true);

    QPushButton *button;

    // Cancel button
    button = ui->buttonBox->button(QDialogButtonBox::Cancel);
    button->setIcon(theme()->icon(Core::Constants::ICONQUIT));

    // Ok button
    button = ui->buttonBox->button(QDialogButtonBox::Ok);
    button->setIcon(theme()->icon(Core::Constants::ICONOK));
    button->setDisabled(true);

    m_imageModel = new QStandardItemModel(this);
    ui->listViewPhotos->setModel(m_imageModel);

    connect(m_freezeButton, SIGNAL(clicked(bool)), ui->openCVWidget, SLOT(setFrozen(bool)));
    connect(ui->openCVWidget, SIGNAL(frozen(bool)), this, SLOT(updatefreezeButton(bool)));
    connect(ui->openCVWidget, SIGNAL(clicked()), m_freezeButton, SLOT(click()));
    connect(ui->listViewPhotos, SIGNAL(activated(QModelIndex)), this, SLOT(faceShotActivated(QModelIndex)));

    connect(ui->openCVWidget, SIGNAL(imageReady(bool)), button, SLOT(setEnabled(bool)));
    connect(ui->openCVWidget, SIGNAL(autoFaceShot(QPixmap)), this, SLOT(autoFaceShot(QPixmap)));
}


/*! Default destructor, deletes the UI */
WebcamDialog::~WebcamDialog()
{
    delete ui;
}

/*! Returns the current or choosen pixmap */
QPixmap WebcamDialog::photo() const
{
    if (_pixmap.isNull())
        return ui->openCVWidget->pixmap()->copy(ui->openCVWidget->frame());
    return _pixmap;
}

void WebcamDialog::setDevice(int device)
{
    m_device = device;
}

/*!
 * \brief Updates the "Freeze" or "Stop" button (text, checked state) aaccording to the OpenCVWidget state.
 * \param aFreeze true if widget is frozen, false if not
 */
void WebcamDialog::updatefreezeButton(bool aFreeze)
{
    if (aFreeze) {
        m_freezeButton->setText(tr("Continue"));
        m_freezeButton->setIcon(theme()->icon(Core::Constants::ICONMEDIASTART));
    } else {
        m_freezeButton->setText(tr("Stop"));
        m_freezeButton->setIcon(theme()->icon(Core::Constants::ICONMEDIAPAUSE));
    }
}

/*!
 * \brief Takes a picture as argument and adds it to the picture list from which the user can choose a good one.
 * \param pixmap A QPixmap with the picture to save.
 */
void WebcamDialog::autoFaceShot(const QPixmap &pixmap)
{
    QStandardItem *item = new QStandardItem(QIcon(pixmap), tr("Photo %1").arg(QString::number(m_imageModel->rowCount()+1)));
    m_imageModel->appendRow(item);
}

/*! \brief Catches the activation (mostly a mouse double click) on autoshot pictures.
 *
 * \param index QModelIndex that points to the picture that was activated.
 */
void WebcamDialog::faceShotActivated(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    // set internal pixmap to return later
    QIcon icon = m_imageModel->data(index, Qt::DecorationRole).value<QIcon>();
    _pixmap = icon.pixmap(QSize(150,150));
    accept();
    return;
}

/*! Makes sure that the "Stop"/"Continue" buttons' text is updated after a language change. */
void WebcamDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        updatefreezeButton(ui->openCVWidget->isFrozen());
    }
}
