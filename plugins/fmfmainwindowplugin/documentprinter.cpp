/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "documentprinter.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <printerplugin/printer.h>

#include <utils/global.h>

#include <QTextDocument>
#include <QString>


using namespace Print;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}

static inline void prepareHeader(Printer *p, const int papers)
{
    QString header = user()->value(Core::IUser::PrescriptionHeader).toString();
    user()->replaceTokens(header);
    Utils::replaceToken(header, Core::Constants::TOKEN_DATE, QDate::currentDate().toString(QLocale().dateFormat()));
    Core::IPatient *patient = Core::ICore::instance()->patient();
    patient->replaceTokens(header);
    p->setHeader(header);
}

static inline void prepareFooter(Printer *p, const int papers)
{
    QString footer = user()->value(Core::IUser::PrescriptionFooter).toString();
    user()->replaceTokens(footer);
    Core::IPatient *patient = Core::ICore::instance()->patient();
    patient->replaceTokens(footer);
    footer.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(QCoreApplication::translate("Print", "Made with %1.").arg(qApp->applicationName())));
    p->setFooter(footer);
}

static inline void prepareWatermark(Printer *p, const int papers)
{
    p->addHtmlWatermark(user()->value(Core::IUser::PrescriptionWatermark).toString(),
                       Print::Printer::Presence(user()->value(Core::IUser::PrescriptionWatermarkPresence).toInt()),
                       Qt::AlignmentFlag(user()->value(Core::IUser::PrescriptionWatermarkAlignement).toInt()));
}

DocumentPrinter::DocumentPrinter(QObject *parent) :
        Core::IDocumentPrinter(parent)
{
}

void DocumentPrinter::addTokens(const int tokenWhere, const QHash<QString, QVariant> &tokensAndValues)
{
}

void DocumentPrinter::print(const QTextDocument &text) const
{
    Print::Printer p;
    if (!p.getUserPrinter())
        if (!p.askForPrinter(qApp->activeWindow()))
            return false;
    QString tmp = patient->value(Core::IPatient::BirthName).toString() + "_" + patient->value(Core::IPatient::Surname).toString();
    p.printer()->setDocName(QString("%1-%2").arg(qApp->applicationName(), tmp.leftJustified(50,'_')));

    prepareHeader(&p, m_Papers);
    prepareFooter(&p, m_Papers);
    prepareWatermark(&p, m_Papers);

    p.previewer(qApp->activeWindow());
    p.printWithDuplicata(settings()->value(Constants::S_PRINTDUPLICATAS).toBool());
    // TODO: Use NormalVersion instead of MedinTuxversion */
    return p.print(text);
}

void DocumentPrinter::print(QTextDocument *text) const
{
    print(*text);
}

void DocumentPrinter::print(const QString &html) const
{
    QTextDocument doc;
    doc.setHtml(html);
    print(doc);
}
