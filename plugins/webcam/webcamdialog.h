#ifndef WEBCAM_DIALOG_H
#define WEBCAM_DIALOG_H

#include "ui_webcamdialog.h"
#include "capturethread.h"
#include <QDialog>

class TrackController;
class RenderWidget;
class QToolBar;
class QDockWidget;
class SettingsWidget;

namespace ui {
class WebcamDialog;
}

namespace Webcam {
namespace Internal {

class WebcamDialog : public QDialog {
    Q_OBJECT
public:
    WebcamDialog(QWidget *parent);
    ~WebcamDialog();

    QPixmap photo() const;


public slots:
    void onFlipVerticalChanged(bool flip);
    void startTracking();
    void stopTracking();
    void takePhoto();
    void updateStats();
protected:
    void closeEvent(QCloseEvent*);
private:
    Ui::WebcamDialog *ui;
    TrackController* trackController;
    QTimer* updateTimer;
    QPixmap m_snapshot;
};

} // end Internal
} // end Webcam
#endif
