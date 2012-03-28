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
#include "pad_core.h"
#include "constants.h"

#include <QDebug>

using namespace PadTools;

void PadCore::print(int indent) const
{
	QString str(indent, ' ');
    str += QString("%1%2%1").arg(Constants::TOKEN_CORE_DELIMITER).arg(_name);
    qWarning() << str;
}

QString PadCore::run(QMap<QString,QVariant> &tokens) const
{
    /** \todo use this output only if HTML is requested */
    const QString &value = tokens[_name].toString();
    if (start() > 0 && !value.isEmpty()) {
        return QString(Constants::TOKEN_AND_POSITION_TAG)
                .arg(value).arg(id());
    }
    return value;
}

void PadCore::run(QMap<QString,QVariant> &tokens, QTextDocument *source, QTextDocument *output) const
{
    const QString &value = tokens[_name].toString();
    qWarning() << "PadCore; name" << _name << "value" << value;
    if (!value.isEmpty())
        insertText(output, value);
}
