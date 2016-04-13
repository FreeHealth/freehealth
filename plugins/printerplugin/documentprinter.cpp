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
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
 /**
   \class Print::Internal::DocumentPrinter
   Wrapper to the Core::IDocumentPrinter. The instance is created by the
   PrinterPlugin class and can be accessed by the plugin manager object pool.\n
   The patient tokens are managed by the Core::IPatient object before the
   print process starts.
*/

#include "documentprinter.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/ipadtools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <printerplugin/printer.h>
#include <printerplugin/constants.h>
#include <printerplugin/printdialog.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QTextDocument>
#include <QString>
#include <QDate>
#include <QLocale>
#include <QApplication>
#include <QDir>

#include <QDebug>

using namespace Print::Internal;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::IPadTools *padTools() {return Core::ICore::instance()->padTools();}

DocumentPrinter::DocumentPrinter(QObject *parent) :
        Core::IDocumentPrinter(parent)
{
}

DocumentPrinter::~DocumentPrinter()
{
}

void DocumentPrinter::clearTokens()
{
    headerTokens.clear();
    footerTokens.clear();
    watermarkTokens.clear();
    globalTokens.clear();
}

void DocumentPrinter::addTokens(const int tokenWhere, const QHash<QString, QVariant> &tokens)
{
    switch (tokenWhere) {
    case Core::IDocumentPrinter::Tokens_Header: headerTokens = tokens; break;
    case Core::IDocumentPrinter::Tokens_Footer: footerTokens = tokens; break;
    case Core::IDocumentPrinter::Tokens_Watermark: watermarkTokens = tokens; break;
    case Core::IDocumentPrinter::Tokens_Global : globalTokens = tokens; break;
    }
}

static void replaceTokens(QString &doc, const QHash<QString, QVariant> &tokens)
{
    QHashIterator<QString, QVariant> it(tokens);
    while (it.hasNext()) {
        it.next();
        Utils::replaceToken(doc, it.key(), it.value().toString());
    }
}

void DocumentPrinter::prepareHeader(Print::Printer *p, const int papers) const
{
    QString header;
    if (user()) {
#ifdef FREEDIAMS
        Q_UNUSED(papers);
        header = user()->value(Core::IUser::PrescriptionHeader).toString();
#else
#ifdef FREEICD
        Q_UNUSED(papers);
        header = user()->value(Core::IUser::GenericHeader).toString();
#else  // ALL OTHERS
        switch (papers) {
        case Core::IDocumentPrinter::Papers_Administrative_User: header = user()->value(Core::IUser::AdministrativeHeader).toString(); break;
        case Core::IDocumentPrinter::Papers_Prescription_User: header = user()->value(Core::IUser::PrescriptionHeader).toString(); break;
        case Core::IDocumentPrinter::Papers_Generic_User: header = user()->value(Core::IUser::GenericHeader).toString(); break;
        }
#endif
#endif
        // Old token style: replace user's tokens
        user()->replaceTokens(header);
    }
    Utils::replaceToken(header, Core::Constants::TOKEN_DATE, QLocale().toString(QDate::currentDate(), QLocale().dateFormat()));
    // Old token style: replace patient's tokens
    if (patient()) {
        patient()->replaceTokens(header);
    }
    // Old token style: replace defined tokens
    replaceTokens(header, globalTokens);
    replaceTokens(header, headerTokens);

#ifdef WITH_PAD
    if (padTools())
        header = padTools()->processPlainText(header);
#endif
    p->setHeader(header);
}

void DocumentPrinter::prepareFooter(Print::Printer *p, const int papers) const
{
    Q_UNUSED(papers);
    QString footer;
    if (user()) {
        // TODO: wrong papers
#ifdef FREEDIAMS
        footer = user()->value(Core::IUser::PrescriptionFooter).toString();
#else
#ifdef FREEICD
        footer = user()->value(Core::IUser::GenericFooter).toString();
#else  // ALL OTHERS
        switch (papers) {
        case Core::IDocumentPrinter::Papers_Administrative_User: footer = user()->value(Core::IUser::AdministrativeFooter).toString(); break;
        case Core::IDocumentPrinter::Papers_Prescription_User: footer = user()->value(Core::IUser::PrescriptionFooter).toString(); break;
        case Core::IDocumentPrinter::Papers_Generic_User: footer = user()->value(Core::IUser::GenericFooter).toString(); break;
        }
#endif
#endif
        // Old token style: replace user's tokens
        user()->replaceTokens(footer);
    }
    // Old token style: replace patient's tokens
    if (patient())
        patient()->replaceTokens(footer);
    // Old token style: replace defined tokens
    replaceTokens(footer, globalTokens);
    replaceTokens(footer, footerTokens);

#ifdef WITH_PAD
    if (padTools())
        footer = padTools()->processPlainText(footer);
#endif

    footer.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(QCoreApplication::translate("Print", "Made with %1.").arg(qApp->applicationName())));
    p->setFooter(footer);
}

void DocumentPrinter::prepareWatermark(Print::Printer *p, const int papers) const
{
    Q_UNUSED(papers);
    int align = Qt::AlignCenter;
    int presence = Printer::DuplicatesOnly;
    QString html;
    if (user()) {
#ifdef FREEDIAMS
        align = user()->value(Core::IUser::PrescriptionWatermarkAlignement).toInt();
        presence = user()->value(Core::IUser::PrescriptionWatermarkPresence).toInt();
        html = user()->value(Core::IUser::PrescriptionWatermark).toString();
#else
#ifdef FREEICD
        align = user()->value(Core::IUser::GenericWatermarkAlignement).toInt();
        presence = user()->value(Core::IUser::GenericWatermarkPresence).toInt();
        html = user()->value(Core::IUser::GenericWatermark).toString();
#else  // ALL OTHERS
        switch (papers) {
        case Core::IDocumentPrinter::Papers_Administrative_User:
            html = user()->value(Core::IUser::AdministrativeWatermark).toString();
            align = user()->value(Core::IUser::AdministrativeWatermarkAlignement).toInt();
            presence = user()->value(Core::IUser::AdministrativeWatermarkPresence).toInt();
            break;
        case Core::IDocumentPrinter::Papers_Prescription_User:
            html = user()->value(Core::IUser::PrescriptionWatermark).toString();
            align = user()->value(Core::IUser::PrescriptionWatermarkAlignement).toInt();
            presence = user()->value(Core::IUser::PrescriptionWatermarkPresence).toInt();
            break;
        case Core::IDocumentPrinter::Papers_Generic_User:
            html = user()->value(Core::IUser::GenericWatermark).toString();
            align = user()->value(Core::IUser::GenericWatermarkAlignement).toInt();
            presence = user()->value(Core::IUser::GenericWatermarkPresence).toInt();
            break;
        }
#endif
#endif
    }

#ifdef WITH_PAD
    if (padTools())
        html = padTools()->processPlainText(html);
#endif

    p->addHtmlWatermark(html,
                       Print::Printer::Presence(presence),
                       Qt::AlignmentFlag(align));
}

void DocumentPrinter::setDocumentName(Print::Printer *p) const
{
    QString tmp;
    if (patient())
        tmp = patient()->data(Core::IPatient::FullName).toString().replace(" ", "_");
    p->printer()->setDocName(QString("%1-%2").arg(qApp->applicationName(), tmp.leftJustified(50,'_')));
}

bool DocumentPrinter::print(const QTextDocument &text, const int papers, bool printDuplicata) const
{
    Print::Printer p;
//    const QString &name = settings()->value(Constants::S_DEFAULT_PRINTER).toString();
    if (!p.getUserPrinter()) {
        QPrinter *print = new QPrinter;
        print->setResolution(QPrinter::HighResolution);
        p.setPrinter(print);
    }

    setDocumentName(&p);
    prepareHeader(&p, papers);
    prepareFooter(&p, papers);
    prepareWatermark(&p, papers);
    // TODO: this can work with PadTools but not with HTML recomposing

//    if (globalTokens.isEmpty()) {
        p.setContent(text);
//    } else {
//        QString html = text.toHtml();
//        Utils::replaceTokens(html, globalTokens);
//        p.setContent(html);
//    }

    p.setPrintWithDuplicata(printDuplicata);

    if (!p.preparePages()) {
        LOG_ERROR("Prepare pages process is wrong");
    }

    QWidget *parent = qApp->activeModalWidget();
    if (!parent)
        parent = Core::ICore::instance()->mainWindow();
    Internal::PrintDialog dlg(parent);
    dlg.setWindowModality(Qt::WindowModal);
    dlg.setPrinter(&p);
    int r = dlg.exec();
    if (r==QDialog::Accepted)
        return true;
    return false;
}

bool DocumentPrinter::print(QTextDocument *text, const int papers, bool printDuplicata) const
{
    return print(*text, papers, printDuplicata);
}

bool DocumentPrinter::print(const QString &html, const int papers, bool printDuplicata) const
{
    QTextDocument doc;
    if (globalTokens.isEmpty()) {
        doc.setHtml(html);
    } else {
        QString _html = html;
        Utils::replaceTokens(_html, globalTokens);
#ifdef WITH_PAD
        if (padTools())
            _html = padTools()->processPlainText(_html);
#endif
        doc.setHtml(_html);
    }
    return print(doc, papers, printDuplicata);
}

bool DocumentPrinter::toPdf(const QString &html, const QString &absOutputFilePath, const int papers, bool printDuplicata) const
{
    // Create a QPrinter pointer (will be deleted by ~Print::Printer)
    Print::Printer p;
    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    printer->setPaperSize(QPrinter::A4);
    printer->setCopyCount(1);
    printer->setPrintRange(QPrinter::AllPages);
    if (!absOutputFilePath.endsWith(".pdf", Qt::CaseInsensitive))
        printer->setOutputFileName(absOutputFilePath + ".pdf");
    else
        printer->setOutputFileName(absOutputFilePath);

    printer->setOutputFormat(QPrinter::PdfFormat);

    printer->setCreator(qApp->applicationName() + " " + qApp->applicationVersion());

    p.setPrinter(printer);
    p.setPaperSize(QPrinter::A4);
    p.setOrientation(QPrinter::Portrait);
    p.setTwoNUp(false);
    p.setPrintWithDuplicata(printDuplicata);

    p.setContent(html);
    prepareHeader(&p, papers);
    prepareFooter(&p, papers);
    prepareWatermark(&p, papers);
    p.preparePages();

    return p.reprint(printer);
}

bool DocumentPrinter::printPreview(const QString &html, const int papers, bool printDuplicata) const
{
    Print::Printer p;
    if (!p.getUserPrinter())
        if (!p.askForPrinter(qApp->activeWindow()))
            return false;

    setDocumentName(&p);
    prepareHeader(&p, papers);
    prepareFooter(&p, papers);
    prepareWatermark(&p, papers);

    if (globalTokens.isEmpty()) {
        p.setContent(html);
    } else {
        QString _html = html;
        Utils::replaceTokens(_html, globalTokens);
#ifdef WITH_PAD
        if (padTools())
            _html = padTools()->processPlainText(_html);
#endif
        p.setContent(_html);
    }
    p.setPrintWithDuplicata(printDuplicata);
    p.previewDialog(qApp->activeWindow());
    // QPrinter is deleted by ~Printer
    return true;
}
