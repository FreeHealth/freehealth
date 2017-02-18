#ifndef RECORDIMPORTDIALOG_P_H
#define RECORDIMPORTDIALOG_P_H

#include <QFileDialog>
#include "recordimportconstants.h"
#include "recordimportdialog.h"
#include "recordimportplugin.h"
#include "ui_recordimportdialog.h"
#include "recordimportdialog_p.h"

#include <formmanagerplugin/formmanager_exporter.h>

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

namespace RecordImport {
namespace Internal {

class RecordImportDialogPrivate : public QObject
{
    Q_OBJECT
public:
    RecordImportDialogPrivate(RecordImportDialog *q) : q_ptr(q)
    {}

//    ~RecordImportDialogPrivate()
//    {
//        if (ui)
//            delete ui;
//        ui = 0;
//    }

    void createUi()
    {
        ui = new Ui::RecordImportDialog;
        ui->setupUi(q_ptr);
        //_filePushButton = new QPushButton(q);
        ui->filePushButton->setText(QObject::tr("File"));
        ui->fileLineEdit->clear();
        ui->fieldSeparatorLineEdit->setMaxLength(1);
        ui->recordSeparatorLineEdit->setMaxLength(1);
    }

    void connectUi()
    {
        QObject::connect(ui->filePushButton, SIGNAL(clicked()),
                         this, SLOT(getFileName()));
        QObject::connect(ui->readPushButton, SIGNAL(clicked()),
                         q_ptr, SLOT(parse()));
        QObject::connect(ui->startPushButton, SIGNAL(clicked()),
                         q_ptr, SLOT(import()));
        QObject::connect(ui->softwareComboBox, SIGNAL(currentTextChanged()),
                         this, SLOT(setStartButtonState()));

    }

    QString fileName()
    {
        return m_filename;
    }
    QChar fieldSeparator()
    {
        return m_fieldSeparator;
    }
    QChar recordSeparator()
    {
        return m_recordSeparator;
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
    Ui::RecordImportDialog *ui;

    //QPushButton *_sendButton;
    //QPushButton *_filePushButton;

private:
    RecordImportDialog *q_ptr;
    QString m_filename;
    QChar m_fieldSeparator;
    QChar m_recordSeparator;


};

} // namespace Internal
} // namespace RecordImport

#endif // RECORDIMPORTDIALOG_P_H
