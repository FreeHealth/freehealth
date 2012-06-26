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

/**
 * \class Utils::BirthDayEdit
 * \brief Replacement class for QDateEdit to better handle birthday entering
 *
 * Inherits QLineEdit and accepts an input format that can be freely defined for
 * each translation. It parses the input and tries to make a date out of it, using
 * the available masks (user provided, system QLocale()->dateFormat(QLocale::ShortFormat),
 * FMF provided). When the focus is lost, it displays the date in the standard way.
 */

#include "birthdayedit.h"
#include <QDebug>

#include <translationutils/constants.h>
#include <utils/datevalidator.h>
//#include "coreplugin/icore.h"
//#include "coreplugin/itheme.h"
//#include "coreplugin/constants_icons.h"


//static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

using namespace Utils;
using namespace Trans::ConstantTranslations;

/** \brief Default constructor */
BirthDayEdit::BirthDayEdit(QWidget *parent) :
    QButtonLineEdit(parent)
{
    init();
}

/** \brief Additional constructor that initializes the widget with given date */
BirthDayEdit::BirthDayEdit(const QDate &date, QWidget *parent) :
    QButtonLineEdit(parent)
{
    init(date);
}

BirthDayEdit::~BirthDayEdit()
{
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
}

/** \brief sets the internal date of the widget to date */
void BirthDayEdit::setDate(const QDate& date)
{
    QDate oldDate = m_date;
    m_date = date;
    if (oldDate != date)
        Q_EMIT dateChanged(m_date);
    updateDisplayText();
}

/** \brief getter function for the internal date
 *  \return a QDate() */
QDate BirthDayEdit::date() const
{
    return QDate(m_date);
}

/** \brief sets the internal date of the widget to NULL
 *
 *  If internal date is not already NULL, the widget emits the dateChanged(QDate &) signal. */
void BirthDayEdit::clear()
{
    if(!m_date.isNull()) {
        m_date = QDate();
        Q_EMIT dateChanged(m_date);
    }
    setText("");
    updatePlaceHolder();
}

/** \brief overrides the default focusOutEvent and sets a custom css.
 *
 *  When widget focus is lost with a date string that can't be interpreted by the
 *  validator as a valid date, the css is changed, e.g. red background */
void BirthDayEdit::focusOutEvent(QFocusEvent *event) {
    int pos = 0;
    QString tmpText = text();

    if (validator()->validate(tmpText, pos) == QValidator::Acceptable) {
        setStyleSheet("");

    } else { // QValidator::Intermediate, QValidator::Invalid

        //TODO: let color be a global constant, maybe in theme?
        setStyleSheet(QString("background: %1;").arg("#f66"));
    }
    QButtonLineEdit::focusOutEvent(event);
}

/** \brief Convenience slot that sets the internal date to the interpreted
 *  value taken from the displayed string in the widget.
 *
 *  It just calls \sa setDateString(). */
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
    // pointer to the DateValidator
    const DateValidator * pValidator = qobject_cast<const DateValidator*>(validator());
    Q_ASSERT(pValidator);

    if (pValidator->validate(tmpDateString, pos) == QValidator::Acceptable) {

        // try to convert the QString into a QDate using the built-in formats
        tmpDate = QDate::fromString(tmpDateString, pValidator->matchedFormat(tmpDateString));
        m_date = tmpDate;
        if (m_date != oldDate)
            Q_EMIT dateChanged(m_date);
        updateDisplayText();
        //FIXME: buggy toolbutton
        m_toolButton->show();
    }
}

/** \brief updates the displayText with the internal date using the default FMF date format */
void BirthDayEdit::updateDisplayText()
{
    // was there a valid date saved?
    if (m_date.isValid()) {
        setText(m_date.toString(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR)));
    } else {
        // no valid date saved, maybe NULL
        clear();
    }
}


void BirthDayEdit::updatePlaceHolder()
{
    // TODO: add date format string
    setPlaceholderText(tr("Enter a date (format: %1)").arg("ADD FORMAT"));
    setToolTip(tr("Enter a date (format: %1)").arg("ADD FORMAT"));
}
