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
#include <utils/datevalidator.h>
//#include "coreplugin/icore.h"
//#include "coreplugin/itheme.h"
//#include "coreplugin/constants_icons.h"


//static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

using namespace Utils;

BirthDayEdit::BirthDayEdit(QWidget *parent) :
    QButtonLineEdit(parent)
{
    init();
}

BirthDayEdit::BirthDayEdit(const QDate &date, QWidget *parent) :
    QButtonLineEdit(parent)
{
    init(date);
}

BirthDayEdit::~BirthDayEdit()
{
}

// "date" property:

/** \brief sets the internal date of the widget to date */
void BirthDayEdit::setDate(const QDate& date)
{
    m_date = date;
    emit dateChanged(m_date);
}

QDate BirthDayEdit::date() const
{
    return QDate(m_date);
}

/// \brief sets the internal date of the widget to NULL
void BirthDayEdit::clear()
{
    bool emitSignal = true;
    if(m_date.isNull())
        emitSignal = true;
    m_date = QDate();
    if (emitSignal)
        emit dateChanged(m_date);
}

void BirthDayEdit::setDisplayedDateString()
{
    setDateString(text());
}

/** \brief sets the internal date of the widget to the given string
 *
 * Tries to parse the string using QDate::fromString. If it is an invalid date string,
 * the date field is set to NULL. This method is called when a valid date string was
 * entered and the user presses Enter or the widget looses focus.
 */
void BirthDayEdit::setDateString(const QString& dateString)
{
    QDate tmpDate;
    QDate oldDate = m_date;
    QString tmpDateString = dateString;

    int pos = 0;
    //pointer to the DateValidator
    const DateValidator * pValidator = qobject_cast<const DateValidator*>(validator());

    switch (validator()->validate(tmpDateString, pos)) {
    case QValidator::Acceptable: {

        // try to convert the QString into a QDate using the built-in formats
        tmpDate = QDate::fromString(tmpDateString, pValidator->matchedFormat(tmpDateString));
        m_date = tmpDate;
        if (m_date != oldDate)
            emit dateChanged(m_date);
        updateDisplayText();
        //FIXME: buggy toolbutton
        m_toolButton->show();
        break;
    }
        /* FIXME: due to QLineEdit not firing editingFinished() when the QValidator returns
         * Intermediate/Invalid the following code will never be called... */
    case QValidator::Intermediate:
    case QValidator::Invalid: {
        // no valid date found
        // -> set the internal date to NULL
        m_date = QDate();
        emit dateChanged(m_date);
        updateDisplayText();
        break;
    }
    default: break;
    }

}

/** \brief updates the displayText with the internal date using the FMF date format */
void BirthDayEdit::updateDisplayText()
{
    // was there valid date saved?
    if (m_date.isValid()) {
        setText(m_date.toString(Trans::Constants::DATEFORMAT_FOR_EDITOR));
        qDebug() << "DATEFORMAT_FOR_EDITOR:" << Trans::Constants::DATEFORMAT_FOR_EDITOR;
    } else {
        // no valid date saved, maybe NULL
        clear();
    }
}


/** \brief init function called from constructors to init all internal values */
void BirthDayEdit::init(const QDate& date, const QDate& maximumDate, const QDate& minimumDate)
{
    m_date = date;
    m_minimumDate = minimumDate;
    m_maximumDate = maximumDate;

    setValidator(new DateValidator(this));

    m_toolButton = new QToolButton(this);
    m_toolButton->setFocusPolicy(Qt::ClickFocus);
    setRightButton(m_toolButton);
    m_toolButton->hide();
//    m_button->setIcon(Core::ICore::instance()->theme()->icon(Core::Constants::ICONOK));

    connect(this, SIGNAL(editingFinished()), this, SLOT(setDisplayedDateString()));
//    connect(this, SIGNAL(editingFinished()), this, SLOT(updateDisplayText()));
}
