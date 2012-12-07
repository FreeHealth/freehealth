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
 *   Main developers : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "frenchsocialnumber.h"
#include "ui_frenchsocialnumber.h"

#include <formmanagerplugin/iformitem.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <utils/log.h>

#include <QEvent>
#include <QKeyEvent>
#include <QRegExpValidator>

#include <QDebug>

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

namespace {

const QString controlKey1 = "154012A111185";  // clé 82
const QString controlKey2 = "1111111111111";  // clé 20
const QString controlKey3 = "1111111111034";  // clé 97
const QString controlKey4 = "278112b050002";  // clé 86
const QString controlKey5 = "1540250025005";  // clé 22
const QString controlKey6 = "299082B234349";  // clé 29

}

using namespace BaseWidgets;
using namespace Internal;

static inline QLabel *findLabel(Form::FormItem *item)
{
    QLabel *l = 0;
    // Find label
    const QString &lbl = item->spec()->value(Form::FormItemSpec::Spec_UiLabel).toString();
    if (!lbl.isEmpty()) {
        l = qFindChild<QLabel*>(item->parentFormMain()->formWidget(), lbl);
        if (l) {
            l->setText(item->spec()->label());
        } else {
            l = new QLabel(item->formWidget());
            l->setText(item->spec()->label());
        }
    }
    return l;
}

FrenchSocialNumber::FrenchSocialNumber(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrenchSocialNumber),
    m_CursorPos(0)
{
    ui->setupUi(this);

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
    ui->sex->setValidator(new QRegExpValidator(QRegExp("^\\d{0,1}$"), this));
    ui->year->setValidator(new QRegExpValidator(QRegExp("^\\d{1,2}$"), this));
    ui->month->setValidator(new QRegExpValidator(QRegExp("^\\d{1,2}$"), this));
    ui->departement->setValidator(new QRegExpValidator(QRegExp("^\\d{1,2}$"), this));
    ui->commune->setValidator(new QRegExpValidator(QRegExp("^\\d[0-9AB]?$"), this));
    ui->birthCode->setValidator(new QRegExpValidator(QRegExp("^\\d{1,3}$"), this));
    ui->controlKey->setValidator(new QRegExpValidator(QRegExp("^\\d{1,3}$"), this));

    // add eventFilter to all lineEdits
    for(int i=0;i<m_Edits.count();++i) {
        m_Edits.at(i)->installEventFilter(this);
    }

    if (m_FullNumber.isEmpty())
        populateWithPatientData();

//    m_FullNumber = "299082B234349";
//    populateLineEdits();
//    checkControlKey();
}

FrenchSocialNumber::~FrenchSocialNumber()
{
    delete ui;
}

void FrenchSocialNumber::setNumberWithControlKey(const QString &number)
{
    m_FullNumber = number;
    if (m_FullNumber.isEmpty())
        populateWithPatientData();
    populateLineEdits();
    checkControlKey();
}

void FrenchSocialNumber::setNumberWithoutControlKey(const QString &number)
{
    m_FullNumber = number;
    if (m_FullNumber.isEmpty())
        populateWithPatientData();
    populateLineEdits();
    checkControlKey();
}

bool FrenchSocialNumber::isValid() const
{
    return isValid(m_FullNumber, ui->controlKey->text());
}

bool FrenchSocialNumber::isValid(const QString &number, const QString &key) const
{
    // get control key
    int cKey = controlKey(number);
    if (cKey==-1) {
        return false;
    }
    return key==QString::number(cKey);
}

int FrenchSocialNumber::controlKey(const QString &number) const
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

QString FrenchSocialNumber::numberWithControlKey() const
{
    return m_FullNumber + ui->controlKey->text();
}

QString FrenchSocialNumber::numberWithoutControlKey() const
{
    return m_FullNumber;
}

QString FrenchSocialNumber::emptyHtmlMask() const
{
    QStringList html;
    for(int i = 0; i < m_NbChars.count(); ++i) {
        html << QString().fill('_', m_NbChars.at(i));
    }
    return html.join("&nbsp;");
}

QString FrenchSocialNumber::toHtml() const
{
    QStringList html;
    for(int i = 0; i < m_Edits.count(); ++i) {
        html << m_Edits.at(i)->text();
    }
    return html.join("&nbsp;");
}

/** Calculate the control key and
  - populate the control key line edit or
  - check the control key
*/
void FrenchSocialNumber::checkControlKey()
{
    int cKey = controlKey(m_FullNumber);
    if (cKey==-1) {
        return;
    }
    if (ui->controlKey->text().isEmpty()) {
        ui->controlKey->setText(QString::number(cKey));
    } else if (ui->controlKey->text() != QString::number(cKey)) {
        // TODO
    }
}

void FrenchSocialNumber::populateLineEdits(QString number)
{
    if (number.isEmpty()) {
        number = m_FullNumber;
    }
    int start = 0;
    int nb = 0;
    for(int i=0; i < m_Edits.count(); ++i) {
        nb = m_NbChars.at(i);
        m_Edits.at(i)->setText(number.mid(start, nb));
        start += nb;
    }
}

void FrenchSocialNumber::addChar(const QString &c, int currentLineEditId, int pos)
{
    qWarning() << "add" << c << currentLineEditId << pos;
    int fullPos = pos;
    for(int i = 0; i < currentLineEditId; ++i) {
        fullPos += m_NbChars.at(i);
    }
    if (fullPos <= m_FullNumber.size() && m_FullNumber.size() < 13) {
        m_FullNumber.insert(fullPos, c);
        populateLineEdits();
        setCursorPosition(currentLineEditId, pos+1);
    } else if (fullPos == m_FullNumber.size() && m_FullNumber.size() < 13) {
        m_FullNumber.append(c);
        populateLineEdits();
        setCursorPosition(currentLineEditId, pos+1);
    }
    if (m_FullNumber.size()==13) {
        checkControlKey();
    }
}

void FrenchSocialNumber::removeChar(int currentLineEditId, int pos)
{
    qWarning()<<"remove" << currentLineEditId << pos;
    if (currentLineEditId==0 && pos==0)
        return;
    --pos;
    for(int i = 0; i < currentLineEditId; ++i) {
        pos += m_NbChars.at(i);
    }
    if (pos < m_FullNumber.size()) {
        m_FullNumber.remove(pos, 1);
    }
    populateLineEdits();
}


void FrenchSocialNumber::setCursorPosition(int currentLineEditId, int pos)
{
    qWarning() << "setPos" << currentLineEditId << pos;
    if (pos==-1 && currentLineEditId > 0) {
        QLineEdit *l = m_Edits.at(currentLineEditId-1);
        l->setFocus();
        l->setCursorPosition(l->text().size());
    } else if (pos > m_NbChars.at(currentLineEditId)) {
        if (currentLineEditId+1 < m_Edits.count()) {
            QLineEdit *l = m_Edits.at(currentLineEditId+1);
            l->setFocus();
            l->setCursorPosition(pos - m_NbChars.at(currentLineEditId));
        }
    } else {
        QLineEdit *l = m_Edits.at(currentLineEditId);
        l->setFocus();
        l->setCursorPosition(pos);
    }
}

/** Filter the Keyboard events of QLineEdit composing the widget. */
bool FrenchSocialNumber::eventFilter(QObject *o, QEvent *e)
{
    if (e->type()!=QEvent::KeyPress && e->type()!=QEvent::KeyRelease)
        return false;

    QKeyEvent *kevent = static_cast<QKeyEvent*>(e);
    if (!kevent)
        return false;

    QLineEdit *l = static_cast<QLineEdit*>(o);
    if (!l)
        return false;

    int currentId = m_Edits.indexOf(l);
    if (currentId==-1)
        return false;
    int nextTab = -1;

    // only key release events
    if (kevent->type()==QKeyEvent::KeyPress) {
//        qWarning() << "KeyPress"  << kevent;//;->text() << kevent->key();
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
            // no management of auto-repeat
            e->ignore();
            return true;
        case Qt::Key_Left:
            // Manage autoRepeat keys
            if (l->cursorPosition()==0)
                setCursorPosition(currentId, -1);
            break;
        case Qt::Key_Right:  // Ok
            if (l->cursorPosition()==m_NbChars.at(currentId)) {
                nextTab = currentId+1;
                if (nextTab >= m_Edits.count())
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
        //        qWarning() << "KeyReleased"  << kevent->text() << kevent->key();

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
            addChar(kevent->text(), currentId, l->cursorPosition());
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
            nextTab = m_Edits.count()-2;
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

void FrenchSocialNumber::populateWithPatientData()
{
    if (!m_FullNumber.isEmpty())
        return;

    m_FullNumber.fill(' ', 13);

    // Add patient default values
    if (patient()->data(Core::IPatient::Gender).toString()=="M") {
        m_FullNumber[0] = '1';
    } else if (patient()->data(Core::IPatient::Gender).toString()=="F") {
        m_FullNumber[0] = '2';
    }

    // Birth date
    QModelIndex idx = patient()->index(patient()->currentPatientIndex().row(), Core::IPatient::DateOfBirth);
    QDate birth = patient()->data(idx, Qt::EditRole).toDate();
    if (birth.isValid()) {
        // year
        m_FullNumber = m_FullNumber.left(1) + QString::number(birth.year()).right(2) + m_FullNumber.mid(3);
        // month
        QString month = QString::number(birth.month());
        if (month.size()==1)
            month.prepend("0");
        m_FullNumber = m_FullNumber.left(3) + month + m_FullNumber.mid(5);
    }

    m_FullNumber = m_FullNumber.simplified();
//    qWarning() << "AUTO" << m_FullNumber;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////   FrenchSocialNumberFormWidget   //////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FrenchSocialNumberFormWidget::FrenchSocialNumberFormWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_NSS(0)
{
    setObjectName("FrenchSocialNumber_" + m_FormItem->uuid());
    // Create NSS widget
    m_NSS = new FrenchSocialNumber(this);
    m_NSS->setObjectName("FrenchSocialNumber_" + m_FormItem->uuid());

    // QtUi Loaded ?
    const QString &layout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!layout.isEmpty()) {
        // Find layout
        QLayout *lay = qFindChild<QLayout*>(formItem->parentFormMain()->formWidget(), layout);
        if (lay) {
            lay->addWidget(m_NSS);
        } else {
            LOG_ERROR("Using the QtUiLinkage, layout not found in the ui: " + formItem->uuid());
        }
        m_Label = findLabel(formItem);
    } else {
        QHBoxLayout *hb = new QHBoxLayout(this);
        // Add QLabel
        createLabel(m_FormItem->spec()->label(), Qt::AlignJustify);
        hb->addWidget(m_Label);

        hb->addWidget(m_NSS);
    }
    retranslate();

    connect(patient(), SIGNAL(currentPatientChanged()), m_NSS, SLOT(populateWithPatientData()));
    m_NSS->populateWithPatientData();

    // create itemdata
    m_ItemData = new FrenchSocialNumberFormData(formItem);
    m_ItemData->setWidget(m_NSS);
    formItem->setItemData(m_ItemData);
}

FrenchSocialNumberFormWidget::~FrenchSocialNumberFormWidget()
{
}

QString FrenchSocialNumberFormWidget::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains("notprintable"))
        return QString();

    QString content;
    if (!withValues) {
        content += QString("%1:&nbsp;%2")
                .arg(m_FormItem->spec()->label())
                .arg(m_NSS->emptyHtmlMask());
    } else {
        content += QString("%1:&nbsp;%2")
                .arg(m_FormItem->spec()->label())
                .arg(m_NSS->toHtml());
    }
    return content;
}

void FrenchSocialNumberFormWidget::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////   FrenchSocialNumberFormData   ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
FrenchSocialNumberFormData::FrenchSocialNumberFormData(Form::FormItem *item) :
        m_FormItem(item), m_Widget(0)
{}

FrenchSocialNumberFormData::~FrenchSocialNumberFormData()
{}

void FrenchSocialNumberFormData::clear()
{
    const QString &s = m_FormItem->valueReferences()->defaultValue().toString();
    m_Widget->setNumberWithControlKey(s);
}

bool FrenchSocialNumberFormData::isModified() const
{
    return m_OriginalValue != m_Widget->numberWithControlKey();
}

void FrenchSocialNumberFormData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue = m_Widget->numberWithControlKey();
}

bool FrenchSocialNumberFormData::setData(const int ref, const QVariant &data, const int role)
{
    qWarning() << "FrenchSocialNumberFormData::setData" << data << role << ref;
//    if (role==Qt::EditRole || role==Qt::DisplayRole) {
//        if (data.canConvert(QVariant::Int))  { // Tristate
//            m_Check->setCheckState(Qt::CheckState(data.toInt()));
//        }
//    }
    return true;
}

QVariant FrenchSocialNumberFormData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
    return m_Widget->numberWithControlKey();
}

void FrenchSocialNumberFormData::setStorableData(const QVariant &data)
{
    if (!data.isValid())
        return;
    m_OriginalValue = data.toString();
    m_Widget->setNumberWithControlKey(m_OriginalValue);
}

QVariant FrenchSocialNumberFormData::storableData() const
{
    return m_Widget->numberWithControlKey();
}
