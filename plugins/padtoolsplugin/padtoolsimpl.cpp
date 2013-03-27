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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
  \class PadTools::PadToolsImpl
  Implementation of the virtual Core::IPadTools, start of everything.
 */

#include "padtoolsimpl.h"
#include "tokenpool.h"
#include "pad_analyzer.h"
#include "pad_highlighter.h"
#include "padwriter.h"

#include <utils/log.h>

#include <QTime>
#include <QCryptographicHash>

#include <QDebug>

using namespace PadTools;
using namespace Internal;

PadToolsImpl::PadToolsImpl(QObject *parent) :
    Core::IPadTools(parent)
{
    _pool = new TokenPool(this);
}

PadToolsImpl::~PadToolsImpl()
{}

Core::ITokenPool *PadToolsImpl::tokenPool() const
{
    return _pool;
}

/**
 * Process a \e plainText tokened document, analyze it,
 * replace tokens by their values and return the resulting plaintext string.
 */
QString PadToolsImpl::processPlainText(const QString &plainText)
{
    QTime chr;
    chr.start();

    PadAnalyzer analyzer;
//    QString t = templ;
//    if (t.contains("&lt;")) {
//        t = t.replace("&lt;","<").replace("&gt;",">");
//    }
    PadDocument *pad = analyzer.analyze(plainText);
//    errors = analyzer.lastErrors();
    Utils::Log::logTimeElapsed(chr, "PadTools", "Analyze text source");

    pad->toOutput(_pool);
    const QString &text = pad->outputDocument()->toPlainText();
    Utils::Log::logTimeElapsed(chr, "PadTools", "Process text");
    return text;
}

/** Process a \e html pad document and return a html string. */
QString PadToolsImpl::processHtml(const QString &html)
{
    QTime chr;
    chr.start();
//    qWarning() << QCryptographicHash::hash(html.toUtf8(), QCryptographicHash::Md5);
//    Utils::Log::logTimeElapsed(chr, "PadAnalyzer", "MD5");

    PadAnalyzer analyzer;
//    QString t = html;
//    if (t.contains("&lt;")) {
//        t = t.replace("&lt;","<").replace("&gt;",">");
//    }
    QTextDocument *doc = new QTextDocument(this);
    doc->setHtml(html);
    PadDocument *pad = analyzer.analyze(doc, 0);
//    errors = analyzer.lastErrors();
    Utils::Log::logTimeElapsed(chr, "PadTools", "Analyze HTML source");

    pad->toOutput(_pool);
    const QString &out = pad->outputDocument()->toHtml();
    Utils::Log::logTimeElapsed(chr, "PadTools", "Process HTML");
    return out;
}

Core::IPadWriter *PadToolsImpl::createWriter(QWidget *parent)
{
    return new Internal::PadWriter(parent);
}

