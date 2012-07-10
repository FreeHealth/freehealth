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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "webcamdialog.h"
#include "ui_webcamdialog.h"

#include "webcamdialog.h"
#include "trackcontroller.h"
#include "renderwidget.h"
#include "processingthread.h"
#include <QtCore/QTimer>
#include <QtGui/QPixmap>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

using namespace Webcam;

WebcamDialog::WebcamDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebcamDialog),
    m_snapshot(QPixmap())
{
    ui->setupUi(this);
    trackController = new TrackController();
    trackController->setRootFilter(ui->renderWidget);

    ui->takePhotoButton->setDefaultAction(ui->actionTakePhoto);
    ui->takePhotoButton->setIcon(Core::ICore::instance()->theme()->icon("takescreenshot.png"));

    ui->actionTakePhoto->setEnabled(false);
    connect(ui->actionTakePhoto, SIGNAL(triggered()), this, SLOT(takePhoto()));

    //start a timer to update some statistics
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateStats()));
    updateTimer->start(1000);
    startTracking();
}

QPixmap WebcamDialog::photo() const
{
    return m_snapshot;
}

WebcamDialog::~WebcamDialog()
{
    delete ui;
}

void WebcamDialog::closeEvent(QCloseEvent*) {
    if(trackController->isTracking()) {
        trackController->stopTracking();
    }
}

// flip the image vertically
void WebcamDialog::onFlipVerticalChanged(bool flip) {
    trackController->getProcessingThread()->setFlipVertical(flip);
}


void WebcamDialog::takePhoto() {
    if (trackController->isTracking()) {
        stopTracking();

    } else {
        startTracking();
    }
}

void WebcamDialog::startTracking() {
    trackController->setFrameSize(CaptureThread::Size640);
    trackController->startTracking();
    ui->actionTakePhoto->setEnabled(true);
    //	ui->actionStart->setEnabled(false);
    //	ui->actionStop->setEnabled(true);
}

void WebcamDialog::stopTracking() {
    trackController->stopTracking();
    //	ui->actionStart->setEnabled(true);
    //	ui->actionStop->setEnabled(false);
}


void WebcamDialog::updateStats() {
    //	statusBar()->showMessage(QString("FPS: ")+QString::number(trackController->getFPS(), 'f', 1));
}
