#ifndef PATIENTIMPORTDIALOG_P_H
#define PATIENTIMPORTDIALOG_P_H

#include <QFileDialog>
#include "patientimportconstants.h"
#include "patientimportdialog.h"
#include "patientimportplugin.h"
#include "ui_patientimportdialog.h"
#include "processpatientimport.h"
#include "patientimportdialog_p.h"

#include <utils/log.h>
#include <utils/global.h>

#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_icons.h>

#include <QDebug>

#include <QLineEdit>
#include <QProgressBar>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QPlainTextEdit>
#include <QFileDialog>

namespace PatientImport {
namespace Internal {

class PatientImportDialogPrivate : public QObject
{
    Q_OBJECT
public:
    PatientImportDialogPrivate(PatientImportDialog *q) : q_ptr(q)
    {}

//    ~PatientImportDialogPrivate()
//    {
//        if (ui)
//            delete ui;
//        ui = 0;
//    }

    void createUi()
    {
        ui = new Ui::PatientImportDialog;
        ui->setupUi(q_ptr);
        //_filePushButton = new QPushButton(q);
        ui->filePushButton->setText(QObject::tr("File"));
        ui->fileLineEdit->clear();
    }

    void connectUi()
    {
        QObject::connect(ui->filePushButton, SIGNAL(clicked()),
                         this, SLOT(getFileName()));
        QObject::connect(ui->startPushButton, SIGNAL(clicked()),
                         q_ptr, SLOT(startImport()));
        QObject::connect(ui->softwareComboBox, SIGNAL(currentTextChanged()),
                         this, SLOT(setStartButtonState()));

    }

    QString fileName()
    {
        return m_filename;
    }

public Q_SLOTS:
    void setStartButtonState()
    {
        qWarning() << !fileName().isEmpty();
        if ((!fileName().isEmpty()) && (!(ui->softwareComboBox->currentIndex()==(-1)))) {
            ui->startPushButton->setEnabled(true);
        }
    }

    void getFileName()
    {
        qWarning() << "getFileName()";
        QString fileName;
        fileName = QFileDialog::getOpenFileName(ui->filePushButton, QObject::tr("Open File"));
        ui->fileLineEdit->setText(fileName);
        m_filename = fileName;
        setStartButtonState();

    }

public:
    Ui::PatientImportDialog *ui;

    //QPushButton *_sendButton;
    //QPushButton *_filePushButton;

private:
    PatientImportDialog *q_ptr;
    QString m_filename;


};

} // namespace Internal
} // namespace PatientImport

#endif // PATIENTIMPORTDIALOG_P_H
