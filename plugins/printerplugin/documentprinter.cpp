/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "documentprinter.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <printerplugin/printer.h>
#include <printerplugin/constants.h>

#include <utils/global.h>

#include <QTextDocument>
#include <QString>
#include <QDate>
#include <QLocale>
#include <QApplication>


using namespace Print::Internal;

inline static Core::IUser *user() {return Core::ICore::instance()->user();}
inline static Core::IPatient *patient() {return Core::ICore::instance()->patient();}
inline static Core::ISettings *settings() {return Core::ICore::instance()->settings();}

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
    QString header = user()->value(Core::IUser::PrescriptionHeader).toString();
    // replace user's tokens
    user()->replaceTokens(header);
    Utils::replaceToken(header, Core::Constants::TOKEN_DATE, QDate::currentDate().toString(QLocale().dateFormat()));
    // replace patient's tokens
    patient()->replaceTokens(header);
    // replace defined tokens
    replaceTokens(header, globalTokens);
    replaceTokens(header, headerTokens);
    p->setHeader(header);
}

void DocumentPrinter::prepareFooter(Print::Printer *p, const int papers) const
{
    QString footer = user()->value(Core::IUser::PrescriptionFooter).toString();
    // replace user's tokens
    user()->replaceTokens(footer);
    // replace patient's tokens
    patient()->replaceTokens(footer);
    // replace defined tokens
    replaceTokens(footer, globalTokens);
    replaceTokens(footer, footerTokens);
    footer.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(QCoreApplication::translate("Print", "Made with %1.").arg(qApp->applicationName())));
    p->setFooter(footer);
}

void DocumentPrinter::prepareWatermark(Print::Printer *p, const int papers) const
{
    p->addHtmlWatermark(user()->value(Core::IUser::PrescriptionWatermark).toString(),
                       Print::Printer::Presence(user()->value(Core::IUser::PrescriptionWatermarkPresence).toInt()),
                       Qt::AlignmentFlag(user()->value(Core::IUser::PrescriptionWatermarkAlignement).toInt()));
}

void DocumentPrinter::setDocumentName(Print::Printer *p) const
{
    QString tmp = patient()->value(Core::IPatient::BirthName).toString() + "_" + patient()->value(Core::IPatient::Surname).toString();
    p->printer()->setDocName(QString("%1-%2").arg(qApp->applicationName(), tmp.leftJustified(50,'_')));
}

bool DocumentPrinter::print(const QTextDocument &text, const int papers, bool printDuplicata) const
{
    Print::Printer p;
    if (!p.getUserPrinter())
        if (!p.askForPrinter(qApp->activeWindow()))
            return false;
    /** \todo Ask user for printing configuration */

    setDocumentName(&p);
    prepareHeader(&p, papers);
    prepareFooter(&p, papers);
    prepareWatermark(&p, papers);

    p.previewer(qApp->activeWindow());
    p.printWithDuplicata(printDuplicata);
    return p.print(text);
}

bool DocumentPrinter::print(QTextDocument *text, const int papers, bool printDuplicata) const
{
    return print(*text, papers, printDuplicata);
}

bool DocumentPrinter::print(const QString &html, const int papers, bool printDuplicata) const
{
    QTextDocument doc;
    doc.setHtml(html);
    return print(doc, papers, printDuplicata);
}

bool DocumentPrinter::printPreview(const QString &html, const int papers, bool printDuplicata) const
{
    Print::Printer p;
    QPrinter *printer = new QPrinter;
    printer->setPageSize(QPrinter::A4);
    printer->setColorMode(QPrinter::ColorMode(settings()->value(Print::Constants::S_COLOR_PRINT).toInt()));
    p.setPrinter(printer);

    setDocumentName(&p);
    prepareHeader(&p, papers);
    prepareFooter(&p, papers);
    prepareWatermark(&p, papers);

    p.setContent(html);
    p.printWithDuplicata(printDuplicata);
    p.previewDialog(qApp->activeWindow());
    // QPrinter is deleted by ~Printer
}
