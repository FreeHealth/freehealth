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
 *   Main Developers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <utils/widgets/uppercasevalidator.h>

#include <QString>
#include <QDebug>

using namespace Utils;

UpperCaseValidator::UpperCaseValidator(QObject *parent) :
    QValidator(parent)
{}  // nothing to do in constructor

UpperCaseValidator::~UpperCaseValidator()
{}

QValidator::State UpperCaseValidator::validate(QString &text, int &pos) const  // PS: no UpperCase for the first letter of variables, only for class names
{
    Q_UNUSED(pos); // pos is not needed
    text = text.toUpper();  // Uppercase the text
    return QValidator::Acceptable;  // return "ok text is like we want it to be"
}

CapitalizationValidator::CapitalizationValidator(QObject *parent) :
    QValidator(parent)
{}  // nothing to do in constructor

CapitalizationValidator::~CapitalizationValidator()
{}

QValidator::State CapitalizationValidator::validate(QString &text, int &pos) const  // PS: no UpperCase for the first letter of variables, only for class names
{
    int previous = pos-2;
    if (text.isEmpty())
        return QValidator::Acceptable;

    if (previous < 0) {
        text = text.replace(0, 1, text.at(pos-1).toUpper());
    } else {
        QChar prev = text.at(previous);
        if (prev==' ' || prev=='-' || prev==',' || prev=='.' || prev==';') {
            text = text.replace(previous+1, 1, text.at(pos-1).toUpper());
        }
    }
    return QValidator::Acceptable;  // return "ok text is like we want it to be"
}

