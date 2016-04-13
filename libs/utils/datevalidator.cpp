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
 *   Main developers : Christian A. Reiter, <christian.a.reiter@gmail.com> *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "datevalidator.h"

#include <translationutils/constants.h>
#include "translationutils/constanttranslations.h"

#include <QDate>
#include <QDebug>

using namespace Utils;
using namespace Trans::ConstantTranslations;

namespace {
const char * const SEPARATORS = "-./,;: ";
}

DateValidator::DateValidator(QObject *parent) :
    QValidator(parent)
{
    // split localized dateFormat string and put the parts in a separated QStringList
    //: this is a input mask for the local date format for ModernDateEditor that can be used by QDate::fromString()
    m_dateFormatList << tr("ddMMyy");
    m_dateFormatList << tr("ddMMyyyy");
    m_lastValidFormat = QString();

    // add the systems default date ShortFormat
    addDateFormat(QLocale().dateFormat(QLocale::ShortFormat));

    // and then the FMF editor default format without separators
    QRegExp withSep = QRegExp(QString("[%1]*").arg(::SEPARATORS));
    addDateFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR).remove(withSep));

    // let the user also enter the date exactly like the dateformat is
    addDateFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
}

/** \brief Validates the input string with custom date formats
 *
 * The function checks if the input string matches a string
 * in the format list. This list that is set up with FMF
 * defaults, system locale defaults and user defined settings.
 */
QValidator::State DateValidator::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);

    // input contains only valid chars
    if (!QRegExp(QString("[0-9%1]*").arg(::SEPARATORS)).exactMatch(input)) {
//        qDebug() << "NON VALID CHAR" << input;
        return QValidator::Invalid;
    }

    // check if input string can be converted into a date
    // using a dateFormat of the list
    foreach(const QString &format, m_dateFormatList) {
        _currentDate = QDate::fromString(input, format);
        if (_currentDate.isValid()) {
//            qDebug() << "Date conversion succeded:" << input;

            // interpret entered 2digit year before normal lifespan to be in current century
            if (_currentDate.year() < (QDate::currentDate().year() - 80) && !format.contains("yyyy")) {
                _currentDate = _currentDate.addYears(100);
            }
            return QValidator::Acceptable;
        }
    }

    // no match by now
    // check if the user enters digits or a SEPARATOR
    // everything else is discouraged
    if(QRegExp(QString("[0-9%1]*").arg(::SEPARATORS)).exactMatch(input)) {
//        qDebug() << "probable date format with separators:" << input;
        return QValidator::Intermediate;
    }

    // if nothing of the above was possible, the input string is mess.
//    qDebug() << "invalid date format:" << input;
    return QValidator::Invalid;
}

/*! \brief Sets the internal date
 *
 * The date is internally saved as QDate.
 */
void DateValidator::setDate(const QDate &date)
{
    _currentDate = date;
}

/** Returns the current editing string converted in a QDate. If input is not valid, the returned QDate will be invalid too. */
QDate DateValidator::date() const
{
    return _currentDate;
}

/*! \brief Adds a date format string to the internal list */
void DateValidator::addDateFormat(const QString &format)
{
    if (!m_dateFormatList.contains(format, Qt::CaseSensitive))
        m_dateFormatList.append(format);
}

/*! \brief Translates date formats on the fly
 *
 * This function is called e.g. when changing language of the program.
 */
void DateValidator::translateFormats()
{
    m_dateFormatList.takeFirst();
    m_dateFormatList.takeFirst();
    m_dateFormatList.prepend(tr("ddMMyy"));
    m_dateFormatList.prepend(tr("ddMMyyyy"));
}

void DateValidator::fixup(QString &input) const
{
    QRegExp withSep = QRegExp(QString("[%1]*").arg(::SEPARATORS));
    if (input.contains(withSep)) {
        input = input.remove(withSep);
        foreach(const QString &format, m_dateFormatList) {
            _currentDate = QDate::fromString(input, format);
            if (_currentDate.isValid()) {
                break;
            }
        }
    }
}
