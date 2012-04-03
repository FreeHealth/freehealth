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
#include "constants.h"

#include <utils/log.h>

#include <QTextDocument>
#include <QTextCursor>
#include <QTime>

#include <QDebug>

enum { WarnChrono = false };

namespace PadTools {
namespace Constants {

void setPadFragmentFormat(const QString &tag, int s, int e, QTextDocument *doc, QList<QTextCharFormat> &formats, QTextCharFormat mergeFormat)
{
    QTime c;
    if (WarnChrono)
        c.start();

    QTextCursor cursor(doc);
    int count = e-s;
    for(int i=0; i < count; ++i) {
        cursor.setPosition(s + i);
        cursor.setPosition(s + i + 1, QTextCursor::KeepAnchor);
        // store the original format
        formats << cursor.charFormat();
        // apply anchor && charFormat
        mergeFormat.setAnchor(true);
        mergeFormat.setAnchorNames(QStringList() << tag << QString::number(i));
        cursor.mergeCharFormat(mergeFormat);
    }
    if (WarnChrono)
        Utils::Log::logTimeElapsed(c, "PadTools", "setTokenCharFormat");
}

void removePadFragmentFormat(const QString &tag, QTextDocument *doc, QList<QTextCharFormat> &formats)
{
    QTime c;
    if (WarnChrono)
        c.start();

    QTextCursor cursor(doc);
    // scan the whole document and remove format (using the anchorNames)
    cursor.movePosition(QTextCursor::End);
    int count = cursor.position();
    for(int i=0; i < count; ++i) {
        cursor.setPosition(i);
        cursor.setPosition(i+1, QTextCursor::KeepAnchor);
        const QStringList &an = cursor.charFormat().anchorNames();
        if (an.isEmpty())
            continue;
        if (an.at(0).contains(tag)) {
            // restore format
            QTextCharFormat f = formats.at(an.at(1).toInt());
            // remove anchors
            f.setAnchor(false);
            f.setAnchorNames(QStringList());
            cursor.setCharFormat(f);
        }
    }
    formats.clear();
    if (WarnChrono)
        Utils::Log::logTimeElapsed(c, "PadTools", "removeTokenCharFormat");
}

}  // namespace Constants
}  // namespace PadTools
