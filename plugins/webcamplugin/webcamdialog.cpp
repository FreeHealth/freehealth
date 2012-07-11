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
#include <trackcontroller.h>
#include <renderwidget.h>
#include <processingthread.h>
#include <QtCore/QTimer>
#include <QtGui/QPixmap>
#include <QtGui/QToolBar>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <QToolButton>

#include <QDebug>

using namespace Webcam;

WebcamDialog::WebcamDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WebcamDialog),
    m_snapshot(QPixmap())
{
    ui->setupUi(this);
    trackController = new TrackController();
    trackController->setRootFilter(ui->renderWidget);

    QToolBar *toolbar = new QToolBar(this);
    toolbar->setFloatable(false);
    toolbar->setOrientation(Qt::Vertical);

    ui->verticalLayout->addWidget(toolbar);

    toolbar->addAction(ui->actionFreeze);
    ui->actionFreeze->setIcon(Core::ICore::instance()->theme()->icon("takescreenshot.png", Core::ITheme::MediumIcon));
    ui->actionFreeze->setVisible(true);
    connect(ui->actionFreeze, SIGNAL(triggered()), this, SLOT(freeze()));

    toolbar->addAction(ui->actionRetry);
    ui->actionRetry->setIcon(Core::ICore::instance()->theme()->icon("retry.png", Core::ITheme::MediumIcon));
    ui->actionRetry->setVisible(false);
    connect(ui->actionRetry, SIGNAL(triggered()), this, SLOT(unFreeze()));

    toolbar->addAction(ui->actionUse);
    ui->actionUse->setIcon(Core::ICore::instance()->theme()->icon("ok.png", Core::ITheme::MediumIcon));
    ui->actionUse->setVisible(false);
    connect(ui->actionUse, SIGNAL(triggered()), this, SLOT(usePhoto()));


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
    delete trackController;
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


void WebcamDialog::freeze() {
    if (trackController->isTracking()) {
        stopTracking();
        ui->actionFreeze->setVisible(false);
        ui->actionRetry->setVisible(true);
        ui->actionUse->setVisible(true);

        m_snapshot = ui->renderWidget->currentPixmap();
    }
}

void WebcamDialog::unFreeze()
{
    if (!trackController->isTracking()) {
        ui->actionFreeze->setVisible(true);
        ui->actionRetry->setVisible(false);
        ui->actionUse->setVisible(false);
        startTracking();
    }

}


void WebcamDialog::startTracking() {
    trackController->setFrameSize(CaptureThread::Size640);
    trackController->startTracking();
}

void WebcamDialog::stopTracking() {
    trackController->stopTracking();
}


void WebcamDialog::updateStats() {
    //	statusBar()->showMessage(QString("FPS: ")+QString::number(trackController->getFPS(), 'f', 1));
}

void WebcamDialog::usePhoto()
{
//    qDebug() << "m_snapshot size:" << m_snapshot.size();
    stopTracking();
    QDialog::accept();
}
