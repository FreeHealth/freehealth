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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "pad_string.h"
#include "constants.h"
#include "pad_document.h"

#include <QDebug>

using namespace PadTools;

void PadString::debug(int indent) const
{
    QString i(indent, ' ');
    QString str;
    str += i+ QString("[padString:Source(%1;%2);Output(%3;%4)]\n")
            .arg(start()).arg(end())
            .arg(outputStart()).arg(outputEnd());
    str += i+ _string;
    qWarning() << str;
}

/** Paste the content of a string fragment in the output. */
//void PadString::run(QMap<QString,QVariant> &)
//{
////    // TODO: use this output only if HTML is requested */
////    if (start() > 0) {
////        return QString(Constants::TOKEN_AND_POSITION_TAG)
////                .arg(_string).arg(id());
////    }
////    return _string;
//}

///** Paste the content of a string fragment in the output. */
//void PadString::run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *out)
//{
//    Q_UNUSED(tokens);
////    insertFragment(source, out);
//}

void PadString::run(QMap<QString,QVariant> &tokens, PadDocument *document)
{
    Q_UNUSED(tokens);
    // Compute output positions
    setOutputStart(start() + document->positionTranslator().deltaForSourcePosition(start()));
    setOutputEnd(outputStart() + rawLength());
}

void PadString::toOutput(Core::ITokenPool *pool, PadDocument *document)
{
    Q_UNUSED(pool);
    // Compute output positions
    setOutputStart(start() + document->positionTranslator().deltaForSourcePosition(start()));
    setOutputEnd(outputStart() + rawLength());
}

void PadString::toRaw(PadDocument *document)
{
    Q_UNUSED(document);
    // Compute raw positions
    setStart(document->positionTranslator().outputToRaw(outputStart()));
    setEnd(document->positionTranslator().outputToRaw(outputEnd()));
}
