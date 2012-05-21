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

namespace Webcam {
namespace Internal {

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

} // end Internal
} // end Webcam
