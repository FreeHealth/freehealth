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
 * Inherits Utils::QButtonLineEdit and accepts an input format that can be freely defined for
 * each translation. It parses the input and tries to make a date out of it, using
 * the available masks (user provided, system QLocale()->dateFormat(QLocale::ShortFormat),
 * FMF provided). When the focus is lost, it displays the date in the standard way
 * if possible.
 */

#include "birthdayedit.h"

#include <utils/datevalidator.h>
#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>

#include <QAction>
#include <QToolButton>
#include <QDebug>

using namespace Utils;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Utils {
namespace Internal {
class BirthDayEditPrivate
{
public:
    BirthDayEditPrivate(BirthDayEdit *parent):
        _rightButton(0),
        _leftButton(0),
        aShortDisplay(0),
        aLongDisplay(0),
        aNumericDisplay(0),
        _validator(0),
        q(parent)
    {}

    ~BirthDayEditPrivate()
    {}

    void createRightButton(const QString &fullAbsPath)
    {
        if (!_rightButton) {
            _rightButton = new QToolButton(q);
            _rightButton->setFocusPolicy(Qt::ClickFocus);
            _rightButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            q->setRightButton(_rightButton);
            QObject::connect(_rightButton, SIGNAL(clicked()), q, SLOT(clear()));
        }
        _rightButton->setIcon(QIcon(fullAbsPath));
    }

    void createLeftButton(const QString &fullAbsPath)
    {
        if (!_leftButton) {
            _leftButton = new QToolButton(q);
            _leftButton->setFocusPolicy(Qt::ClickFocus);
            _leftButton->setPopupMode(QToolButton::InstantPopup);
            _leftButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
            // Create the menu
            aShortDisplay = new QAction(q);
            aLongDisplay = new QAction(q);
            aNumericDisplay = new QAction(q);
            _leftButton->addAction(aLongDisplay);
            _leftButton->addAction(aShortDisplay);
            _leftButton->addAction(aNumericDisplay);
            q->setLeftButton(_leftButton);
            _leftButton->setDefaultAction(aLongDisplay);
            QObject::connect(_leftButton, SIGNAL(triggered(QAction*)), q, SLOT(formatActionTriggered(QAction*)));
        }
        _leftButton->setIcon(QIcon(fullAbsPath));
        aShortDisplay->setIcon(QIcon(fullAbsPath));
        aLongDisplay->setIcon(QIcon(fullAbsPath));
        aNumericDisplay->setIcon(QIcon(fullAbsPath));
    }

public:
    QDate m_date;
    QDate m_maximumDate;
    QDate m_minimumDate;
    QToolButton *_rightButton, *_leftButton;
    QAction *aShortDisplay, *aLongDisplay, *aNumericDisplay;
    DateValidator *_validator;
    QString _defaultEditingFormat;

private:
    BirthDayEdit *q;
};
} // namespace Internal
} // namespace Utils

/** \brief Default constructor */
BirthDayEdit::BirthDayEdit(QWidget *parent) :
    QButtonLineEdit(parent),
    d_de(new Internal::BirthDayEditPrivate(this))
{
    init();
}

/** \brief Additional constructor that initializes the widget with given date */
BirthDayEdit::BirthDayEdit(const QDate &date, QWidget *parent) :
    QButtonLineEdit(parent),
    d_de(new Internal::BirthDayEditPrivate(this))
{
    init(date);
}

BirthDayEdit::~BirthDayEdit()
{
    if (d_de) {
        delete d_de;
    }
    d_de = 0;
}

/** \brief init function called from constructors to init all internal values */
void BirthDayEdit::init(const QDate& date, const QDate& maximumDate, const QDate& minimumDate)
{
    d_de->m_date = date;
    d_de->m_minimumDate = minimumDate;
    d_de->m_maximumDate = maximumDate;

    d_de->_validator = new DateValidator(this);
    setValidator(d_de->_validator);

    d_de->_defaultEditingFormat = tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR);
}

/** Define the absolute path \e fullAbsPath to the 'clear' icon */
void BirthDayEdit::setClearIcon(const QString &fullAbsPath)
{
    d_de->createRightButton(fullAbsPath);
    retranslate();
}

/** Define the absolute path \e fullAbsPath to the 'date' icon */
void BirthDayEdit::setDateIcon(const QString &fullAbsPath)
{
    d_de->createLeftButton(fullAbsPath);
    retranslate();
}

/** Sets the internal date of the widget to date */
void BirthDayEdit::setDate(const QDate &date)
{
    QDate oldDate = d_de->m_date;
    d_de->m_date = date;
    d_de->_validator->setDate(date);
    if (oldDate != date)
        Q_EMIT dateChanged(d_de->m_date);
    updateDisplayText();
}

/** Returns the current editing date */
QDate BirthDayEdit::date() const
{
    return d_de->m_date;
}

/** \brief sets the internal date of the widget to NULL
 *
 *  If internal date is not already NULL, the widget emits the dateChanged(QDate &) signal. */
void BirthDayEdit::clear()
{
    if(!d_de->m_date.isNull()) {
        d_de->m_date = QDate();
        Q_EMIT dateChanged(d_de->m_date);
    }
    d_de->_validator->setDate(d_de->m_date);
    setText("");
    updatePlaceHolder();
}

/** \brief overrides the default focusOutEvent and sets a custom css.
 *
 *  When widget focus is lost with a date string that can't be interpreted by the
 *  validator as a valid date, the css is changed, e.g. red background, to indicate
 *  that there is something wrong. */
void BirthDayEdit::focusOutEvent(QFocusEvent *event)
{
    // switching to displayMode
    setValidator(0);
    // check validity of the current edition (if use does not it enter)
    QString val = text();
    int pos = 0;
    if (d_de->_validator->validate(val, pos) == QValidator::Intermediate)
        d_de->_validator->fixup(val);
    setText(val);
    d_de->m_date = d_de->_validator->date();
    if (d_de->m_date.isValid()) {
        clearExtraStyleSheet();
    } else {
        if (text().isEmpty()) {
            clearExtraStyleSheet();
        } else {
            //TODO: let color be a global constant, maybe in theme?
            setExtraStyleSheet(QString("background: %1").arg("#f66"));
        }
    }
    updateDisplayText();
    QButtonLineEdit::focusOutEvent(event);
}

/** \brief overrides the default focusInEvent and sets \sa DateValidator. */
void BirthDayEdit::focusInEvent(QFocusEvent *event)
{
    // switching to editMode
    if (d_de->m_date.isValid()) {
        setText(d_de->m_date.toString(d_de->_defaultEditingFormat));
    } else {
//        setText("");
    }
    setValidator(d_de->_validator);
    d_de->_validator->setDate(d_de->m_date);
    QButtonLineEdit::focusInEvent(event);
}

/** \brief sets the internal date of the widget to the given string
 *
 * Tries to parse the string using QDate::fromString. If it is an invalid date string,
 * the date field is set to NULL. This method is called when a valid date string was
 * entered and the user presses Enter or the widget looses focus.
 */
void BirthDayEdit::setDateString(QString dateString)
{
    // inform validator
    int pos = 0;
    d_de->_validator->validate(dateString, pos);
    QDate previousDate = d_de->m_date;
    d_de->m_date = d_de->_validator->date();
    if (d_de->m_date.isValid()) {
        if (d_de->m_date != previousDate)
            Q_EMIT dateChanged(d_de->m_date);
    }
    updateDisplayText();
}

/** \brief updates the displayText with the internal date using the default FMF date format */
void BirthDayEdit::updateDisplayText()
{
//    qWarning() << "updateDisplayText(), focus:" << hasFocus() << "valid date:" << m_date.isValid();
    // Edit mode -> do nothing
    if (hasFocus()) {
        return;
    }

    // Display mode
    if (d_de->m_date.isValid()) {
        if (d_de->_leftButton) {
            setText(d_de->m_date.toString(d_de->_leftButton->defaultAction()->data().toString()));
        } else {
            setText(d_de->m_date.toString(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR)));
        }
    } else {
        // no valid date saved, maybe NULL
//        clear();
    }
}

/** Private slot */
void BirthDayEdit::formatActionTriggered(QAction *a)
{
    Q_UNUSED(a);
    updateDisplayText();
}

/** Clear the place holder. */
void BirthDayEdit::updatePlaceHolder()
{
//    setPlaceholderText(tkTr(Trans::Constants::ENTER_DATE));
//    setExtraToolTip(tkTr(Trans::Constants::ENTER_DATE_FORMAT_1).arg(_validator->acceptedDateFormats().join("<br/>")));
}

/** Retranslate UI. */
void BirthDayEdit::retranslate()
{
    updatePlaceHolder();
    if (d_de->aLongDisplay) {
        d_de->aLongDisplay->setText(tkTr(Trans::Constants::SHOW_LONG_FORMAT));
        d_de->aLongDisplay->setToolTip(d_de->aLongDisplay->text());
        d_de->aLongDisplay->setData(QLocale().dateFormat(QLocale::LongFormat));
    }
    if (d_de->aShortDisplay) {
        d_de->aShortDisplay->setText(tkTr(Trans::Constants::SHOW_SHORT_FORMAT));
        d_de->aShortDisplay->setToolTip(d_de->aShortDisplay->text());
        d_de->aShortDisplay->setData(QLocale().dateFormat(QLocale::ShortFormat));
    }
    if (d_de->aNumericDisplay) {
        d_de->aNumericDisplay->setText(tkTr(Trans::Constants::SHOW_NUMERIC_FORMAT));
        d_de->aNumericDisplay->setToolTip(d_de->aNumericDisplay->text());
        d_de->aNumericDisplay->setData(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    }
    d_de->_validator->translateFormats();
}

void BirthDayEdit::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        retranslate();
    }
    QButtonLineEdit::changeEvent(e);
}
