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

#include <utils/datevalidator.h>
#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>

#include <QAction>
#include <QToolButton>
#include <QDebug>

using namespace Utils;
using namespace Trans::ConstantTranslations;

/** \brief Default constructor */
BirthDayEdit::BirthDayEdit(QWidget *parent) :
    QButtonLineEdit(parent),
    _rightButton(0), _leftButton(0),
    aShortDisplay(0), aLongDisplay(0), aNumericDisplay(0)
{
    init();
}

/** \brief Additional constructor that initializes the widget with given date */
BirthDayEdit::BirthDayEdit(const QDate &date, QWidget *parent) :
    QButtonLineEdit(parent),
    _rightButton(0), _leftButton(0),
    aShortDisplay(0), aLongDisplay(0), aNumericDisplay(0),
    _validator(0)
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

    _validator = new DateValidator(this);
    setValidator(_validator);

    connect(this, SIGNAL(editingFinished()), this, SLOT(setDisplayedDateString()));
}

void BirthDayEdit::setClearIcon(const QString &fullAbsPath)
{
    if (!_rightButton) {
        _rightButton = new QToolButton(this);
        _rightButton->setFocusPolicy(Qt::ClickFocus);
        setRightButton(_rightButton);
        connect(_rightButton, SIGNAL(clicked()), this, SLOT(clear()));
    }
    _rightButton->setIcon(QIcon(fullAbsPath));
}

void BirthDayEdit::setDateIcon(const QString &fullAbsPath)
{
    if (!_leftButton) {
        _leftButton = new QToolButton(this);
        _leftButton->setFocusPolicy(Qt::ClickFocus);
        _leftButton->setPopupMode(QToolButton::InstantPopup);
        // Create the menu
        aShortDisplay = new QAction(this);
        aLongDisplay = new QAction(this);
        aNumericDisplay = new QAction(this);
        _leftButton->addAction(aLongDisplay);
        _leftButton->addAction(aShortDisplay);
        _leftButton->addAction(aNumericDisplay);
        _leftButton->setDefaultAction(aLongDisplay);
        _leftButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        retranslate();
        _leftButton->resize(20,20);
        setLeftButton(_leftButton);
        connect(_leftButton, SIGNAL(triggered(QAction*)), this, SLOT(formatActionTriggered(QAction*)));
    }
    _leftButton->setIcon(QIcon(fullAbsPath));
    aShortDisplay->setIcon(QIcon(fullAbsPath));
    aLongDisplay->setIcon(QIcon(fullAbsPath));
    aNumericDisplay->setIcon(QIcon(fullAbsPath));
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

    if (_validator->date().isValid()) {
        setExtraStyleSheet("");
    } else {
        //TODO: let color be a global constant, maybe in theme?
        setExtraStyleSheet(QString("background: %1").arg("#f66"));
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

    if (_validator->date().isValid()) {
        // try to convert the QString into a QDate using the built-in formats
        m_date = _validator->date();
        if (m_date != oldDate)
            Q_EMIT dateChanged(m_date);
        updateDisplayText();
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

/** Private slot */
void BirthDayEdit::formatActionTriggered(QAction *a)
{
    if (a==aLongDisplay) {
    } else if (a==aShortDisplay) {
    } else if (a==aNumericDisplay) {
    }
}

/** Clear the place holder. */
void BirthDayEdit::updatePlaceHolder()
{
    setPlaceholderText(tkTr(Trans::Constants::ENTER_DATE_FORMAT_1).arg(_validator->acceptedDateFormat().join(";")));
    setExtraToolTip(tkTr(Trans::Constants::ENTER_DATE_FORMAT_1).arg(_validator->acceptedDateFormat().join(";")));
}

/** Retranslate UI. */
void BirthDayEdit::retranslate()
{
    updatePlaceHolder();
    if (aLongDisplay) {
        aLongDisplay->setText(tkTr(Trans::Constants::SHOW_LONG_FORMAT));
        aLongDisplay->setToolTip(aLongDisplay->text());
        aLongDisplay->setData(QLocale().dateFormat(QLocale::LongFormat));
    }
    if (aShortDisplay) {
        aShortDisplay->setText(tkTr(Trans::Constants::SHOW_SHORT_FORMAT));
        aShortDisplay->setToolTip(aLongDisplay->text());
        aShortDisplay->setData(QLocale().dateFormat(QLocale::ShortFormat));
    }
    if (aNumericDisplay) {
        aNumericDisplay->setText(tkTr(Trans::Constants::SHOW_NUMERIC_FORMAT));
        aNumericDisplay->setToolTip(aLongDisplay->text());
        aNumericDisplay->setData(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    }
}

void BirthDayEdit::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        retranslate();
    }
    QButtonLineEdit::changeEvent(e);
}
