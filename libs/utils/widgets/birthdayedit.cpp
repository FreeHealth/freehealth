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
 *   Main developers : Christian A. Reiter, <christian.a.reiter@gmail.com> *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "birthdayedit.h"
#include <QDebug>

#include <translationutils/constants.h>

using namespace Utils;

BirthDayEdit::BirthDayEdit(QWidget *parent) :
    QLineEdit(parent)
{
    init();
}

BirthDayEdit::BirthDayEdit(const QDate &date, QWidget *parent) :
    QLineEdit(parent)
{
    init(date);
    //* QLinEdit input mask for BirthDayEdit
    setInputMask(tr("xxxxxxxxxx")); // e.g. 15.11.1978
}

BirthDayEdit::~BirthDayEdit()
{
}


QDate BirthDayEdit::date() const
{
    return QDate(m_date);
}

/**
 * \brief BirthDayEdit::setDateFormats
 * \param formats QString that should provide a comma separated list of
 * input formats like "ddMMyy,ddMMyyyy". This string is expanded into a QStringList
 * and used to determine a date out of the user-endered text of the widget.
 */
void BirthDayEdit::setDateFormats(QString formats)
{
    //TODO: check string for errors
    m_dateFormatList.clear();

    // cut string and put it the parts a separated QStringList
    m_dateFormatList = formats.split(",", QString::SkipEmptyParts);

    // always use the default formats
    m_dateFormatList.append(QLocale().dateFormat(QLocale::ShortFormat));
    m_dateFormatList.append(QLocale().dateFormat(QLocale::NarrowFormat));

    // and then the FMF editor default format
    m_dateFormatList.append(Trans::Constants::DATEFORMAT_FOR_EDITOR);
}

/// \brief sets the internal dateof the widget to NULL
void BirthDayEdit::clear()
{
    m_date = QDate();
    emit dateChanged(m_date);
}

/** \brief sets the internal date of the widget to date */
void BirthDayEdit::setDate(const QDate& date)
{
    m_date = date;
    emit dateChanged(m_date);
}
/** \brief sets the internal date of the widget to the string
 *
 * tries to parse the string. If it is an invalid date string, the date field
 * is set to NULL.
 */
void BirthDayEdit::setDateString(const QString& dateString)
{
    QDate tmpDate;
    // cycle through all translatable date formats that are held in a list
    foreach(QString format, m_dateFormatList) {

        // try to convert the QString into a QDate using this format
        tmpDate = QDate::fromString(dateString, format);
        if (tmpDate.isValid()) {
            m_date = tmpDate;
            emit dateChanged(m_date);
            qDebug() << "Date conversion succeded:" << dateString << "using format" << format;
            // we found a parseable valid date, so we can return safely
            return;
        }
        qDebug() << "Date conversion failed:" << dateString << "using format" << format;
    }

    // end without finding a valid date
    // -> set the internal date to invalid
    if (m_date.isValid()) {
        m_date = QDate();
        emit dateChanged(m_date);
        updateDisplayText();
    } else {
        m_date = QDate();
    }
}

void BirthDayEdit::updateDisplayText()
{
    // was there valid date saved?
    if (m_date.isValid()) {
        setText(m_date.toString(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    } else {
        // no valid date saved, maybe NULL
        setText("");
        clear();
    }
}


/** \brief init function called from constructor to init all internal values */
void BirthDayEdit::init(const QDate& date, const QDate& maximumDate, const QDate& minimumDate)
{
    m_date = date;
    m_minimumDate = minimumDate;
    m_maximumDate = maximumDate;
    // get translated input filters - each country has own methods to
    // write dates in various (sometimes more than one) ways
    //TODO: maybe later make formats configurable in the UI?
    QString dateFormats = tr("ddMMyy,ddMMyyyy").trimmed();

    setDateFormats(dateFormats);

    connect(this,SIGNAL(textChanged(QString)), this, SLOT(setDateString(QString)));
    connect(this, SIGNAL(editingFinished()), this, SLOT(updateDisplayText()));
}
