/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "printdialog.h"
#include "ui_printdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/idocumentprinter.h>

#include <printerplugin/printer.h>
#include <printerplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>

#include <extensionsystem/pluginmanager.h>

#include <QPainter>
#include <QPicture>
#include <QPushButton>
#include <QDir>
#include <QDateTime>
#include <QFileDialog>
#include <QToolButton>
#include <QAction>
#include <QtPrintSupport/QPrinterInfo>
#include <QtPrintSupport/QPrinter>

#include <QDebug>

using namespace Print::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IDocumentPrinter *docPrinter() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}


PrintDialog::PrintDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Internal::Ui::PrintDialog),
    m_Printer(0),
    aSavePdf(0), aMailPdf(0), aSaveHtml(0), aMailHtml(0)
{
    ui->setupUi(this);
//    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_previewLabel = new QLabel();
    m_previewLabel->setScaledContents(true);
    ui->gridLayout->addWidget(m_previewLabel, 0, 0);
    // temporary fix to respect A4 aspect ratio
    //this->resize(this->parentWidget()->height()*0.7, this->parentWidget()->height()*0.9);
    m_previewLabel->setFixedSize(this->parentWidget()->height()*0.9*0.707070707,
                                 this->parentWidget()->height()*0.9);

    //m_previewLabel->setScaledContents(false);

    ui->labelPresets->hide();
    ui->presetsCombo->hide();

    // Set icons
    ui->nextButton->setIcon(theme()->icon(Core::Constants::ICONONERIGHTARROW));
    ui->prevButton->setIcon(theme()->icon(Core::Constants::ICONONELEFTARROW));
    ui->firstButton->setIcon(theme()->icon(Core::Constants::ICONTWOLEFTARROW));
    ui->lastButton->setIcon(theme()->icon(Core::Constants::ICONTWORIGHTARROW));

    // Fill printer combo
    m_AvailPrinterAtDialogOpens = QPrinterInfo::availablePrinters();
    foreach(const QPrinterInfo &info, m_AvailPrinterAtDialogOpens) {
        ui->printerCombo->blockSignals(true);
        ui->printerCombo->addItem(info.printerName());
        ui->printerCombo->blockSignals(false);
    }

    // Some preparation
    ui->allPages->setChecked(true);

    // Change the buttons of the dialog
    QToolButton *toolButton = new QToolButton(this);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButton->setText(tr("File extra"));
    toolButton->setIcon(theme()->icon(Core::Constants::ICONSAVEAS));

    QAction *a = aSavePdf = new QAction(this);
    a->setText(tr("Save to PDF"));
    toolButton->addAction(a);
//    a = aMailPdf = new QAction(this);
//    a->setText(tr("Send by mail as PDF"));
//    toolButton->addAction(a);
    a = aSaveHtml = new QAction(this);
    a->setText(tr("Save to HTML"));
    toolButton->addAction(a);
//    a = aMailHtml = new QAction(this);
//    a->setText(tr("Send by mail as HTML"));
//    toolButton->addAction(a);
    ui->buttonBox->addButton(toolButton, QDialogButtonBox::ActionRole);
    connect(toolButton, SIGNAL(triggered(QAction*)), this, SLOT(toFile(QAction*)));

    ui->buttonBox->addButton(tkTr(Trans::Constants::FILEPRINT_TEXT).remove("&"), QDialogButtonBox::YesRole);
    this->adjustSize();
}

PrintDialog::~PrintDialog()
{
    delete ui;
}

void PrintDialog::accept()
{
    // Print the doc
    int copies = ui->copies->value();
    m_Printer->printer()->setCopyCount(copies);
    if (ui->allPages->isChecked()) {
        m_Printer->printer()->setPrintRange(QPrinter::AllPages);
    } else {
        m_Printer->printer()->setPrintRange(QPrinter::PageRange);
        m_Printer->printer()->setFromTo(ui->pageFrom->value(), ui->pageTo->value());
    }

    // Print to printer
    if (!m_Printer->reprint(m_Printer->printer())) {
        LOG_FOR("Printer", "Printing error on device " + m_Printer->printer()->printerName());
        return;
    } else {
        LOG_FOR("Printer", "Printing on device successful" + m_Printer->printer()->printerName());
    }

    // Cache to PDF?
    if (settings()->value(Constants::S_KEEP_PDF).toBool()) {
        // Get the cache-PDF path
        QString cachePath = settings()->value(Constants::S_PDF_FOLDER).toString();
        if (cachePath.isEmpty())
            cachePath = QString("%1/%2").arg(settings()->path(Core::ISettings::UserDocumentsPath)).arg("PDF");
        if (QFileInfo(cachePath).isRelative())
            cachePath.append(qApp->applicationDirPath() + QDir::separator());
        if (!QDir(cachePath).exists())
            QDir().mkpath(cachePath);

        // Create the PDF Cache filename
        QString uid = Utils::createUid();
        QString docName = QString("%1_%2.pdf")
                          .arg(qApp->applicationName())
                          .arg(uid);

        // Create the full filename & save
        QString fileName = QDir::cleanPath(cachePath);
        fileName.append(QDir::separator() + docName);
        m_Printer->toPdf(fileName, docName);
        docPrinter()->addPrintedDoc(fileName, docName, QDateTime::currentDateTime(), user()->uuid());
    }
    QDialog::accept();
}

void PrintDialog::setPrinter(Print::Printer *printer)
{
    m_Printer = printer;
    const QString &name = printer->printer()->printerName();
    ui->printerCombo->setCurrentIndex(ui->printerCombo->findText(name, Qt::MatchCaseSensitive));
    // read options
    ui->duplicates->blockSignals(true);
    ui->nup->blockSignals(true);
    ui->duplicates->setChecked(printer->printWithDuplicatas());
    ui->nup->setChecked(printer->isTwoNUp());
    ui->duplicates->blockSignals(false);
    ui->nup->blockSignals(false);
    // read settings
    ui->createPdf->setChecked(settings()->value(Constants::S_KEEP_PDF).toBool());
    ui->createPdf->setEnabled(false);
    // manage pages
    ui->pageFrom->blockSignals(true);
    ui->pageTo->blockSignals(true);
    ui->pageFrom->setValue(1);
    ui->pageFrom->setMaximum(m_Printer->pages().count());
    ui->pageTo->setValue(m_Printer->pages().count());
    ui->pageTo->setMaximum(m_Printer->pages().count());
    ui->pageFrom->blockSignals(false);
    ui->pageTo->blockSignals(false);
    previewPage(0);
}

Print::Printer *PrintDialog::printer() const
{
    return m_Printer;
}

void PrintDialog::setTwoNUp(bool state)
{
    ui->nup->setChecked(state);
    if (m_Printer)
        m_Printer->setTwoNUp(true);
}

bool PrintDialog::isTwoNUp() const
{
    return ui->nup->isChecked();
}

void PrintDialog::setPdfCache(bool state)
{
    ui->createPdf->setChecked(state);
}

bool PrintDialog::isPdfCacheEnabled() const
{
    return ui->createPdf->isChecked();
}

static inline QSize onePageSize() {return QSize(189, 263);}
static inline QSize twoNUpPagesSize() {return QSize(263,189);}

static inline QPixmap onePagePreview(const QSize &paperSize, const QSize &pageSize, const QSize &final, int page, Print::Printer *printer)
{
    Q_UNUSED(pageSize);
    QPixmap pix(paperSize);
    pix.fill();
    QPainter paint;
    paint.begin(&pix);

    printer->pageToPainter(&paint, page, false, true);

    paint.end();
    pix = pix.scaled(final, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    return pix;
}

static inline QPixmap twoNUpPreview(const QSize &paperSize, const QSize &pageSize, const QSize &final, int page, Print::Printer *printer)
{
    Q_UNUSED(pageSize);
    QPixmap pix(paperSize.height(), paperSize.width());
    pix.fill();
    QPainter paint;
    paint.begin(&pix);

    printer->pageToPainter(&paint, page, true, true);

    paint.end();
    pix = pix.scaled(final, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    return pix;
}

void PrintDialog::previewPage(int n)
{
    if (n >= m_Printer->pages().count())
        return;

    if (n < 0)
        return;

    m_PreviewingPage = n;

    if (!ui->nup->isChecked()) {
        m_previewLabel->setMinimumSize(onePageSize());
        //ui->preview->setMaximumSize(onePageSize());
        m_previewLabel->setPixmap(onePagePreview(m_Printer->printer()->paperRect().size(),
                                              m_Printer->printer()->pageRect().size(),
                                              m_previewLabel->size(),
                                              n+1, m_Printer));
        ui->viewPageLabel->setText(QString("%1 of %2").arg(n+1).arg(m_Printer->pages().count()));
    } else {
        m_previewLabel->setMinimumSize(twoNUpPagesSize());
        //m_previewLabel->setMaximumSize(twoNUpPagesSize());
        m_previewLabel->setPixmap(twoNUpPreview(m_Printer->printer()->paperRect().size(),
                                             m_Printer->printer()->pageRect().size(),
                                             m_previewLabel->size(),
                                             n+1, m_Printer));
        ui->viewPageLabel->setText(QString("%1-%2 of %3").arg(n+1).arg(n+2).arg(m_Printer->pages().count()));
    }
}

//#include <QDesktopServices>
//#include <QUrl>
void PrintDialog::toFile(QAction *action)
{
    if (action == aSavePdf) {
        QString f = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                 QDir::homePath(),
                                                 tr("PDF file (*.pdf)"));
        if (f.isEmpty())
            return;
        if (QFileInfo(f).completeSuffix().compare("pdf", Qt::CaseInsensitive) != 0)
            f.append(".pdf");
        if (m_Printer)
            m_Printer->toPdf(f, "DFSDF");
    } else if (action == aMailPdf) {
        // TODO: code this
//        QString fileName = QString("%1/%2.pdf")
//                .arg(settings()->path(Core::ISettings::ApplicationTempPath))
//                .arg(Utils::createUid());
//        QDesktopServices::openUrl(QUrl(QString("mailto:?&subject=--&attachement=%1").arg("/Users/eric/essai.pdf")));
    } else if (action == aSaveHtml) {
        QString f = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                 QDir::homePath(),
                                                 tr("HTML file (*.html *.htm)"));
        if (f.isEmpty())
            return;
        if (QFileInfo(f).completeSuffix().compare("html", Qt::CaseInsensitive) != 0
                && QFileInfo(f).completeSuffix().compare("htm", Qt::CaseInsensitive) != 0)
            f.append(".html");
        if (m_Printer)
            Utils::saveStringToFile(m_Printer->toHtml(), f);
    } else if (action == aMailHtml) {
        // TODO: code this
    }
}

void PrintDialog::on_duplicates_toggled(bool state)
{
    m_Printer->setPrintWithDuplicata(state);
    m_Printer->preparePages();
    // manage pages
    ui->pageFrom->blockSignals(true);
    ui->pageTo->blockSignals(true);
    ui->pageFrom->setValue(1);
    ui->pageFrom->setMaximum(m_Printer->pages().count());
    ui->pageTo->setValue(m_Printer->pages().count());
    ui->pageTo->setMaximum(m_Printer->pages().count());
    ui->pageFrom->blockSignals(false);
    ui->pageTo->blockSignals(false);
    previewPage(0);
}

void PrintDialog::on_nup_toggled(bool state)
{
    if (m_Printer)
        m_Printer->setTwoNUp(state);
    previewPage(0);
}

void PrintDialog::on_nextButton_clicked()
{
    if (ui->nup->isChecked())
        previewPage(m_PreviewingPage+2);
    else
        previewPage(m_PreviewingPage+1);
}

void PrintDialog::on_prevButton_clicked()
{
    if (ui->nup->isChecked())
        previewPage(m_PreviewingPage-2);
    else
        previewPage(m_PreviewingPage-1);
}

void PrintDialog::on_firstButton_clicked()
{
    previewPage(0);
}

void PrintDialog::on_lastButton_clicked()
{
    if (ui->nup->isChecked())
        m_Printer->pages().count()%2==0 ? previewPage(m_Printer->pages().count()-2) : previewPage(m_Printer->pages().count()-1);
    else
        previewPage(m_Printer->pages().count()-1);
}

void PrintDialog::on_pageFrom_valueChanged(int)
{
    ui->pagesRange->setChecked(true);
}

void PrintDialog::on_pageTo_valueChanged(int)
{
    ui->pagesRange->setChecked(true);
}

void PrintDialog::on_printerCombo_currentIndexChanged(int index)
{
    if (!m_Printer)
        return;
    if (index < m_AvailPrinterAtDialogOpens.count() && index >= 0) {
        if (m_Printer->printer()->printerName() == m_AvailPrinterAtDialogOpens.at(index).printerName())
            return;
        QPrinter *printer = new QPrinter(m_AvailPrinterAtDialogOpens.at(index), QPrinter::ScreenResolution);
        m_Printer->setPrinter(printer);
    }
}

void PrintDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
