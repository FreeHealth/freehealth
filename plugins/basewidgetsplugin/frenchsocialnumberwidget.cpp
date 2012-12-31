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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class BaseWidgets::Internal::FrenchSocialNumberWidget
 * \brief short description of class
 *
 * Long description of class
 * \sa BaseWidgets::
 */

#include "frenchsocialnumberwidget.h"

#include <QKeyEvent>
#include <QDebug>

#include "ui_frenchsocialnumber.h"

using namespace BaseWidgets;
using namespace Internal;

enum { WarnKeyPressed = false };

namespace {

const QString controlKey1 = "154012A111185";  // clé 82
const QString controlKey2 = "1111111111111";  // clé 20
const QString controlKey3 = "1111111111034";  // clé 97
const QString controlKey4 = "278112b050002";  // clé 86
const QString controlKey5 = "1540250025005";  // clé 22
const QString controlKey6 = "299082B234349";  // clé 29

}

namespace BaseWidgets {
namespace Internal {
class FrenchSocialNumberWidgetPrivate
{
public:
    FrenchSocialNumberWidgetPrivate(FrenchSocialNumberWidget *parent) :
        ui(new Ui::FrenchSocialNumber),
        m_CursorPos(0),
        q(parent)
    {
    }
    
    ~FrenchSocialNumberWidgetPrivate()
    {
        delete ui;
    }
    
    void setupUi()
    {
        ui->setupUi(q);

        // m_Edits is used to manage focus change between line edits
        m_Edits << ui->sex << ui->year << ui->month << ui->departement << ui->commune << ui->birthCode << ui->controlKey;
        m_NbChars << 1     << 2        << 2         << 2               << 3           << 3             << 2;

        // resize lineEdits according to the max length
        for(int i = 0; i < m_Edits.count(); ++i) {
            QLineEdit *l = m_Edits.at(i);
            int frameWidth = l->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
            int top, left, right, bottom;
            l->getTextMargins(&left,&top,&right,&bottom);
            QFontMetrics fm = l->fontMetrics();
            int w = fm.boundingRect(QString().fill('0', m_NbChars.at(i)+1)).width();
            l->setMinimumWidth(w + frameWidth*2 + left + right);
            l->setMaximumWidth(w + frameWidth*2 + left + right);
            l->resize(w + frameWidth*2 + left + right, l->height());
        }
        // add validators to lineEdits
        ui->sex->setValidator(new QRegExpValidator(QRegExp("^\\d{0,1}$"), q));
        ui->year->setValidator(new QRegExpValidator(QRegExp("^\\d{1,2}$"), q));
        ui->month->setValidator(new QRegExpValidator(QRegExp("^\\d{1,2}$"), q));
        ui->departement->setValidator(new QRegExpValidator(QRegExp("^\\d{1,2}$"), q));
        ui->commune->setValidator(new QRegExpValidator(QRegExp("^\\d[0-9AB]?$"), q));
        ui->birthCode->setValidator(new QRegExpValidator(QRegExp("^\\d{1,3}$"), q));
        ui->controlKey->setValidator(new QRegExpValidator(QRegExp("^\\d{1,3}$"), q));

        // add eventFilter to all lineEdits
        for(int i=0;i<m_Edits.count();++i) {
            m_Edits.at(i)->installEventFilter(q);
        }
    }

public:
    Ui::FrenchSocialNumber *ui;
    QList<QLineEdit *> m_Edits;
    QList<int> m_NbChars;
    QString m_FullNumber, _lastPressedKey;
    int m_CursorPos;

private:
    FrenchSocialNumberWidget *q;
};
} // namespace Internal
} // end namespace BaseWidgets


/*! Constructor of the BaseWidgets::Internal::FrenchSocialNumberWidget class */
FrenchSocialNumberWidget::FrenchSocialNumberWidget(QWidget *parent) :
    QWidget(parent),
    d(new FrenchSocialNumberWidgetPrivate(this))
{
    d->setupUi();

//    if (d->m_FullNumber.isEmpty())
//        populateWithPatientData();

//    m_FullNumber = "299082B234349";
//    populateLineEdits();
//    checkControlKey();
}

/*! Destructor of the BaseWidgets::Internal::FrenchSocialNumberWidget class */
FrenchSocialNumberWidget::~FrenchSocialNumberWidget()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FrenchSocialNumberWidget::initialize()
{
    return true;
}

void FrenchSocialNumberWidget::setNumberWithControlKey(const QString &number)
{
    d->m_FullNumber = number;
//    if (d->m_FullNumber.isEmpty())
//        populateWithPatientData();
    populateLineEdits();
    checkControlKey();
}

void FrenchSocialNumberWidget::setNumberWithoutControlKey(const QString &number)
{
    d->m_FullNumber = number;
//    if (d->m_FullNumber.isEmpty())
//        populateWithPatientData();
    populateLineEdits();
    checkControlKey();
}

bool FrenchSocialNumberWidget::isValid() const
{
    return isValid(d->m_FullNumber, d->ui->controlKey->text());
}

bool FrenchSocialNumberWidget::isValid(const QString &number, const QString &key) const
{
    // get control key
    int cKey = controlKey(number);
    if (cKey==-1) {
        return false;
    }
    return key==QString::number(cKey);
}

int FrenchSocialNumberWidget::controlKey(const QString &number) const
{
    if (number.size() != 13)
        return -1;

    QString nString = number;
    // manage departement 2A 2B
    nString.replace("A","0", Qt::CaseInsensitive);
    nString.replace("B","0", Qt::CaseInsensitive);
    QRegExp reg("\\d{13}");
    if (!reg.exactMatch(nString))
        return -1;

    qlonglong n = nString.toLongLong();

    // manage departement 2A 2B
    if (number.contains("A", Qt::CaseInsensitive))
        n -= 1000000;
    else if (number.contains("B", Qt::CaseInsensitive))
        n -= 2000000;
    return (97 - (n%97));
}

QString FrenchSocialNumberWidget::numberWithControlKey() const
{
    return QString(d->m_FullNumber + d->ui->controlKey->text());
}

QString FrenchSocialNumberWidget::numberWithoutControlKey() const
{
    return d->m_FullNumber;
}

QString FrenchSocialNumberWidget::emptyHtmlMask() const
{
    QStringList html;
    for(int i = 0; i < d->m_NbChars.count(); ++i) {
        html << QString().fill('_', d->m_NbChars.at(i));
    }
    return html.join("&nbsp;");
}

QString FrenchSocialNumberWidget::toHtml() const
{
    QStringList html;
    for(int i = 0; i < d->m_Edits.count(); ++i) {
        html << d->m_Edits.at(i)->text();
    }
    return html.join("&nbsp;");
}

/** Calculate the control key and
  - populate the control key line edit or
  - check the control key
*/
void FrenchSocialNumberWidget::checkControlKey()
{
    int cKey = controlKey(d->m_FullNumber);
    if (cKey==-1) {
        return;
    }
    if (d->ui->controlKey->text().isEmpty()) {
        d->ui->controlKey->setText(QString::number(cKey));
    } else if (d->ui->controlKey->text() != QString::number(cKey)) {
        // TODO
    }
}

void FrenchSocialNumberWidget::populateLineEdits(QString number)
{
    if (number.isEmpty()) {
        number = d->m_FullNumber;
    }
    int start = 0;
    int nb = 0;
    for(int i=0; i < d->m_Edits.count(); ++i) {
        nb = d->m_NbChars.at(i);
        d->m_Edits.at(i)->setText(number.mid(start, nb));
        start += nb;
    }
}

void FrenchSocialNumberWidget::addChar(const QString &c, int currentLineEditId, int pos)
{
    qWarning() << "add" << c << currentLineEditId << pos;
    int fullPos = pos;
    for(int i = 0; i < currentLineEditId; ++i) {
        fullPos += d->m_NbChars.at(i);
    }
    if (fullPos <= d->m_FullNumber.size() && d->m_FullNumber.size() < 13) {
        d->m_FullNumber.insert(fullPos, c);
        populateLineEdits();
        setCursorPosition(currentLineEditId, pos+1);
    } else if (fullPos == d->m_FullNumber.size() && d->m_FullNumber.size() < 13) {
        d->m_FullNumber.append(c);
        populateLineEdits();
        setCursorPosition(currentLineEditId, pos+1);
    }
    if (d->m_FullNumber.size()==13) {
        checkControlKey();
    }
}

void FrenchSocialNumberWidget::removeChar(int currentLineEditId, int pos)
{
    qWarning()<<"remove" << currentLineEditId << pos;
    if (currentLineEditId==0 && pos==0)
        return;
    --pos;
    for(int i = 0; i < currentLineEditId; ++i) {
        pos += d->m_NbChars.at(i);
    }
    if (pos < d->m_FullNumber.size()) {
        d->m_FullNumber.remove(pos, 1);
    }
    populateLineEdits();
}


void FrenchSocialNumberWidget::setCursorPosition(int currentLineEditId, int pos)
{
    qWarning() << "setPos" << currentLineEditId << pos;
    if (pos==-1 && currentLineEditId > 0) {
        QLineEdit *l = d->m_Edits.at(currentLineEditId-1);
        l->setFocus();
        l->setCursorPosition(l->text().size());
    } else if (pos > d->m_NbChars.at(currentLineEditId)) {
        if (currentLineEditId+1 < d->m_Edits.count()) {
            QLineEdit *l = d->m_Edits.at(currentLineEditId+1);
            l->setFocus();
            l->setCursorPosition(pos - d->m_NbChars.at(currentLineEditId));
        }
    } else {
        QLineEdit *l = d->m_Edits.at(currentLineEditId);
        l->setFocus();
        l->setCursorPosition(pos);
    }
}

/** Filter the Keyboard events of QLineEdit composing the widget. */
bool FrenchSocialNumberWidget::eventFilter(QObject *o, QEvent *e)
{
    if (e->type()!=QEvent::KeyPress && e->type()!=QEvent::KeyRelease)
        return false;

    QKeyEvent *kevent = static_cast<QKeyEvent*>(e);
    if (!kevent)
        return false;

    QLineEdit *l = static_cast<QLineEdit*>(o);
    if (!l)
        return false;

    int currentId = d->m_Edits.indexOf(l);
    if (currentId==-1)
        return false;
    int nextTab = -1;

    // only key release events
    if (kevent->type()==QKeyEvent::KeyPress) {
        if (WarnKeyPressed)
            qWarning() << "KeyPress"  << kevent;
        switch (kevent->key()) {
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
        case Qt::Key_A:
        case Qt::Key_B:
            d->_lastPressedKey = kevent->text();
            // no management of auto-repeat
            e->ignore();
            return true;
        case Qt::Key_Left:
            // Manage autoRepeat keys
            if (l->cursorPosition()==0)
                setCursorPosition(currentId, -1);
            break;
        case Qt::Key_Right:  // Ok
            if (l->cursorPosition() == d->m_NbChars.at(currentId)) {
                nextTab = currentId+1;
                if (nextTab >= d->m_Edits.count())
                    return true;
                setCursorPosition(nextTab, 0);
                return true;
            }
            break;
        case Qt::Key_Backspace:
        {
            if (kevent->isAutoRepeat()) {
                int pos = l->cursorPosition();
                removeChar(currentId, pos);
                --pos;
                if (pos==0) --pos;
                setCursorPosition(currentId, pos);
            }
            e->ignore();
            return true;
            break;
        }
        case Qt::Key_Delete:
            if (kevent->isAutoRepeat()) {
                int pos = l->cursorPosition();
                ++pos;
                removeChar(currentId, pos);
                setCursorPosition(currentId, pos-1);
            }
            e->ignore();
            return true;
        default: return false;
        }
    } else if (kevent->type()==QKeyEvent::KeyRelease) {
        if (WarnKeyPressed)
            qWarning() << "KeyReleased" << kevent;

        switch (kevent->key()) {
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
        {
            addChar(d->_lastPressedKey, currentId, l->cursorPosition());
            d->_lastPressedKey.clear();
            return true;
            break;
        }
        case Qt::Key_A:
        case Qt::Key_B:
        {
            // only departement 2ndchar
            if (currentId==3 && l->cursorPosition()==1) {
                addChar(kevent->text(), currentId, l->cursorPosition());
                return true;
            }
            break;
        }
        case Qt::Key_Home:
            nextTab = 0;
            break;
        case Qt::Key_End:
            nextTab = d->m_Edits.count()-2;
            break;
        case Qt::Key_Left:
//            if (l->cursorPosition()==0) {
//                setCursorPosition(currentId, -1);
                return true;
//            }
            break;
        case Qt::Key_Right:
            return true;
            break;
        case Qt::Key_Delete:
        {
            // remove char at
            int pos = l->cursorPosition();
            ++pos;
            removeChar(currentId, pos);
            setCursorPosition(currentId, pos-1);
            return true;
        }
        case Qt::Key_Backspace:
        {
            // remove char at
            int pos = l->cursorPosition();
            removeChar(currentId, pos);
            --pos;
            if (pos==0) --pos;
            setCursorPosition(currentId, pos);
            return true;
        }
        default: return false;
        }
    }

    return false;
}

//void FrenchSocialNumberWidget::populateWithPatientData()
//{
//    if (!m_FullNumber.isEmpty())
//        return;

//    m_FullNumber.fill(' ', 13);

//    // Add patient default values
//    if (patient()->data(Core::IPatient::Gender).toString()=="M") {
//        m_FullNumber[0] = '1';
//    } else if (patient()->data(Core::IPatient::Gender).toString()=="F") {
//        m_FullNumber[0] = '2';
//    }

//    // Birth date
//    QModelIndex idx = patient()->index(patient()->currentPatientIndex().row(), Core::IPatient::DateOfBirth);
//    QDate birth = patient()->data(idx, Qt::EditRole).toDate();
//    if (birth.isValid()) {
//        // year
//        m_FullNumber = m_FullNumber.left(1) + QString::number(birth.year()).right(2) + m_FullNumber.mid(3);
//        // month
//        QString month = QString::number(birth.month());
//        if (month.size()==1)
//            month.prepend("0");
//        m_FullNumber = m_FullNumber.left(3) + month + m_FullNumber.mid(5);
//    }

//    m_FullNumber = m_FullNumber.simplified();
////    qWarning() << "AUTO" << m_FullNumber;
//}
