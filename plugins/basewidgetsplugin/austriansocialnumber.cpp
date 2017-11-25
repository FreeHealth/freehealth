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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, MD <eric.maeker@gmail.com>              *
 *  Contributors:                                                          *
 *       Christian A. Reiter <christian.a.reiter@gmail.com>                *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "austriansocialnumber.h"
#include "ui_austriansocialnumber.h"

#include <formmanagerplugin/iformitem.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <datapackutils/datapackcore.h>

#include <utils/log.h>

#include <QEvent>
#include <QKeyEvent>
#include <QRegExpValidator>
#include <QIcon>

#include <QDebug>

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

using namespace BaseWidgets;
using namespace Internal;

static inline QLabel *findLabel(Form::FormItem *item)
{
    QLabel *l = 0;
    // Find label
    const QString &lbl = item->spec()->value(Form::FormItemSpec::Spec_UiLabel).toString();
    if (!lbl.isEmpty()) {
        l = item->parentFormMain()->formWidget()->findChild<QLabel*>(lbl);
        if (l) {
            l->setText(item->spec()->label());
        } else {
            l = new QLabel(item->formWidget());
            l->setText(item->spec()->label());
        }
    }
    return l;
}

AustrianSocialNumberEdit::AustrianSocialNumberEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AustrianSocialNumberEdit),
    m_CursorPos(0)
{
    ui->setupUi(this);

    // m_Edits is used to manage focus change between line edits
    m_Edits   << ui->leSVNR << ui->leBirthDate;
    m_NbChars << 4          << 6;

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
    ui->leSVNR->setValidator(new QRegExpValidator(QRegExp("^\\d{4}$"), this)); // 4 digits
    ui->leBirthDate->setValidator(new QRegExpValidator(QRegExp("^\\d{6}$"), this)); //6 digits

    // add eventFilter to all lineEdits
    for(int i=0;i<m_Edits.count();++i) {
        m_Edits.at(i)->installEventFilter(this);
    }

    /** \bug ? how can one be sure that m_FullNumber is empty here? */
    if (m_FullNumber.isEmpty())
        populateWithPatientData();
    connect(ui->leSVNR, SIGNAL(textChanged(QString)), this, SLOT(updateStatus()));
    connect(ui->leSVNR, SIGNAL(textChanged(QString)), this, SLOT(checkIfEditFull(QString&)));
    connect(ui->leBirthDate, SIGNAL(textChanged(QString)), this, SLOT(updateStatus()));
    connect(ui->leBirthDate, SIGNAL(textChanged(QString)), this, SLOT(checkIfEditFull(QString&)));
}

AustrianSocialNumberEdit::~AustrianSocialNumberEdit()
{
    delete ui;
}

void AustrianSocialNumberEdit::setNumber(const QString &number)
{
    m_FullNumber = isValid(number)? number : QString();
    if (m_FullNumber.isEmpty())
        populateWithPatientData();
    populateLineEdits();
}

bool AustrianSocialNumberEdit::isValid() const
{
    return isValid(m_FullNumber);
}

/** checks if the given full SVNR is valid. Therefore the checksum is calculated
 and compared with the 4th position */
bool AustrianSocialNumberEdit::isValid(const QString &number) const
{
    const QString nString = number.trimmed().simplified().replace(" ","");

    // check if all positions are there (not needed, included in following regexp
//    if (number.size() != 10)
//        return false;

    // check if it is a real number, no chars
    QRegExp reg("^\\d{10}$");
    if (!reg.exactMatch(nString))
        return false;

    // manage SVNR
    QString svnr = nString.left(4);
    int sum =
            nString.at(0).digitValue() * 3 +
            nString.at(1).digitValue() * 7 +
            nString.at(2).digitValue() * 9 +
            // -----   4th char is the control char!
            nString.at(4).digitValue() * 5 +
            nString.at(5).digitValue() * 8 +
            nString.at(6).digitValue() * 4 +
            nString.at(7).digitValue() * 2 +
            nString.at(8).digitValue() * 1 +
            nString.at(9).digitValue() * 6;
    int controlSum = sum % 11;

    if (controlSum >= 10) {
        return false;
    }

    return (nString.at(3).digitValue() == controlSum);
}

QString AustrianSocialNumberEdit::number() const
{
    return m_FullNumber;
}

QString AustrianSocialNumberEdit::emptyHtmlMask() const
{
    QStringList html;
    for(int i = 0; i < m_NbChars.count(); ++i) {
        html << QString().fill('_', m_NbChars.at(i));
    }
    return html.join("&nbsp;");
}

QString AustrianSocialNumberEdit::toHtml() const
{
    QStringList html;
    for(int i = 0; i < m_Edits.count(); ++i) {
        html << m_Edits.at(i)->text();
    }
    return html.join("&nbsp;");
}

void AustrianSocialNumberEdit::populateLineEdits(QString number)
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

void AustrianSocialNumberEdit::addChar(const QString &c, int currentLineEditId, int pos)
{
    qWarning() << "add" << c << currentLineEditId << pos;
    int fullPos = pos;
    for(int i = 0; i < currentLineEditId; ++i) {
        fullPos += m_NbChars.at(i);
    }
    if (fullPos <= m_FullNumber.size() && m_FullNumber.size() < 10) {
        m_FullNumber.insert(fullPos, c);
        populateLineEdits();
        setCursorPosition(currentLineEditId, pos+1);
    } else if (fullPos == m_FullNumber.size() && m_FullNumber.size() < 10) {
        m_FullNumber.append(c);
        populateLineEdits();
        setCursorPosition(currentLineEditId, pos+1);
    }
    if (m_FullNumber.size()==10) {
        // checkControlKey();
    }
}

void AustrianSocialNumberEdit::removeChar(int currentLineEditId, int pos)
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


void AustrianSocialNumberEdit::setCursorPosition(int currentLineEditId, int pos)
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

void AustrianSocialNumberEdit::checkIfEditFull(QString &str) {
    QLineEdit *edit = qobject_cast<QLineEdit*>(sender());
    if (!edit) {
        return;
    }

    if (str.length() == edit->maxLength() && edit != m_Edits.last()) {
        focusNextChild();
    } else if (str.isEmpty() && edit != m_Edits.first()) {
        focusPreviousChild();
    }
}

/** Filter the Keyboard events of QLineEdit composing the widget. */
bool AustrianSocialNumberEdit::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type()!=QEvent::KeyPress && e->type()!=QEvent::KeyRelease)
        return false;

    QKeyEvent *kevent = static_cast<QKeyEvent*>(e);
    if (!kevent)
        return false;

    QLineEdit *lineEdit = static_cast<QLineEdit*>(obj);
    if (!lineEdit)
        return false;

    int currentId = m_Edits.indexOf(lineEdit);
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
        case Qt::Key_9: return false;
            if(lineEdit->cursorPosition() == lineEdit->maxLength()-1) {
                focusNextChild();
                return false;
            }
        case Qt::Key_Right:  // Ok
            if (lineEdit->cursorPosition()==lineEdit->maxLength()  // if we are at the end of the edit
                    && currentId != m_Edits.count()-1) {            // and it's not the last edit in the Widget
                focusNextChild();
            }
            return true;
        case Qt::Key_Left:
            // Manage autoRepeat keys
            if (lineEdit->cursorPosition()==0  // if we are at the beginning of the edit
                    && currentId != 0) {    // and it's not the first edit in the widget
                focusPreviousChild(); // -> go one edit left
            }
            return true;
        case Qt::Key_Backspace:
        {
            if (lineEdit->cursorPosition() == 0 // if we are at the beginning of the edit
                    && currentId != 0) {         // and it's not the first edit in the widget
                focusPreviousChild();
                return false;   // process Backspace as usual
//            if (kevent->isAutoRepeat()) {
//                int pos = lineEdit->cursorPosition();
//                removeChar(currentId, pos);
//                --pos;
//                if (pos==0) --pos;
//                setCursorPosition(currentId, pos);
            }
//            e->ignore();
            return false;
//            break;
        }
        case Qt::Key_Delete:
            if (kevent->isAutoRepeat()) {
                int pos = lineEdit->cursorPosition();
                ++pos;
                removeChar(currentId, pos);
                setCursorPosition(currentId, pos-1);
            }
            e->ignore();
            return true;
        default: return false;
        }
    }
//    else if (kevent->type()==QKeyEvent::KeyRelease) {
//        //        qWarning() << "KeyReleased"  << kevent->text() << kevent->key();

//        switch (kevent->key()) {
//        case Qt::Key_0:
//        case Qt::Key_1:
//        case Qt::Key_2:
//        case Qt::Key_3:
//        case Qt::Key_4:
//        case Qt::Key_5:
//        case Qt::Key_6:
//        case Qt::Key_7:
//        case Qt::Key_8:
//        case Qt::Key_9:
//        {
//            addChar(kevent->text(), currentId, lineEdit->cursorPosition());
//            return true;
//            break;
//        }
//        case Qt::Key_Home:
//            nextTab = 0;
//            break;
//        case Qt::Key_End:
//            nextTab = m_Edits.count()-2;
//            break;
//        case Qt::Key_Left:
////            if (l->cursorPosition()==0) {
////                setCursorPosition(currentId, -1);
//                return true;
////            }
//            break;
//        case Qt::Key_Right:
//            return true;
//            break;
//        case Qt::Key_Delete:
//        {
//            // remove char at
//            int pos = lineEdit->cursorPosition();
//            ++pos;
//            removeChar(currentId, pos);
//            setCursorPosition(currentId, pos-1);
//            return true;
//        }
//        case Qt::Key_Backspace:
//        {
//            // remove char at
//            int pos = lineEdit->cursorPosition();
//            removeChar(currentId, pos);
//            --pos;
//            if (pos==0) --pos;
//            setCursorPosition(currentId, pos);
//            return true;
//        }
//        default: return false;
//        }
//    }

    return false;
}

void AustrianSocialNumberEdit::populateWithPatientData()
{
    // already data here? then exit.
    if (!m_FullNumber.isEmpty())
        return;

    m_FullNumber.fill(' ', 10);

    // Add patient default values: birth date
    const QDate birth = patient()->data(Core::IPatient::DateOfBirth).toDate();
    if (birth.isValid()) {
        m_FullNumber = m_FullNumber.left(4) + birth.toString("ddMMyy");
    }
    /** \bug there should be no whitespace in m_Fullnumber */
}

void AustrianSocialNumberEdit::updateStatus()
{
    const QPixmap pixmap= this->isValid()? QPixmap(DataPack::DataPackCore::instance().icon("warning.png", DataPack::DataPackCore::SmallPixmaps)) : 0;
    ui->labelNumberOk->setPixmap(pixmap);
}

////////////////////////////////////////// FormItem /////////////////////////////////////////////
AustrianSocialNumberFormWidget::AustrianSocialNumberFormWidget(Form::FormItem *formItem, QWidget *parent) :
    Form::IFormWidget(formItem,parent), m_SVNR(0)
{
    // Create SVNR widget
    m_SVNR = new AustrianSocialNumberEdit(this);
    m_SVNR->setObjectName("AustrianSocialNumberEdit_" + m_FormItem->uuid());

    // QtUi Loaded?
    const QString &layout = formItem->spec()->value(Form::FormItemSpec::Spec_UiInsertIntoLayout).toString();
    if (!layout.isEmpty()) {
        // Find layout
        QLayout *lay = qFindChild<QLayout*>(formItem->parentFormMain()->formWidget(), layout);
        if (lay) {
            lay->addWidget(m_SVNR);
        } else {
            LOG_ERROR("Using the QtUiLinkage, layout was not found in the ui: " + formItem->uuid());
        }
        m_Label = findLabel(formItem);
    } else {
        QHBoxLayout *hb = new QHBoxLayout(this);
        // Add QLabel
        createLabel(m_FormItem->spec()->label(), Qt::AlignJustify);
        hb->addWidget(m_Label);

        hb->addWidget(m_SVNR);
    }
    retranslate();

    connect(patient(), SIGNAL(currentPatientChanged()), m_SVNR, SLOT(populateWithPatientData()));
    m_SVNR->populateWithPatientData();

    // create itemdata
    m_ItemData = new AustrianSocialNumberFormData(formItem);
    m_ItemData->setWidget(m_SVNR);
    formItem->setItemDatas(m_ItemData);
}

AustrianSocialNumberFormWidget::~AustrianSocialNumberFormWidget()
{
}

QString AustrianSocialNumberFormWidget::printableHtml(bool withValues) const
{
    if (m_FormItem->getOptions().contains("notprintable"))
        return QString();

    QString content;
    if (!withValues) {
        content += QString("%1:&nbsp;%2")
                .arg(m_FormItem->spec()->label())
                .arg(m_SVNR->emptyHtmlMask());
    } else {
        content += QString("%1:&nbsp;%2")
                .arg(m_FormItem->spec()->label())
                .arg(m_SVNR->toHtml());
    }
    return content;
}

void AustrianSocialNumberFormWidget::retranslate()
{
    if (m_Label)
        m_Label->setText(m_FormItem->spec()->label());
}


////////////////////////////////////////// ItemData /////////////////////////////////////////////
AustrianSocialNumberFormData::AustrianSocialNumberFormData(Form::FormItem *item) :
        m_FormItem(item), m_Widget(0)
{}

AustrianSocialNumberFormData::~AustrianSocialNumberFormData()
{}

void AustrianSocialNumberFormData::clear()
{
    const QString &s = m_FormItem->valueReferences()->defaultValue().toString();

    if (s.isEmpty())
        return;

    m_Widget->setNumber(s);
}

bool AustrianSocialNumberFormData::isModified() const
{
    return m_OriginalValue != m_Widget->number();
}

void AustrianSocialNumberFormData::setModified(bool modified)
{
    if (!modified)
        m_OriginalValue == m_Widget->number();
}

bool AustrianSocialNumberFormData::setData(const int ref, const QVariant &data, const int role)
{
    Q_UNUSED(ref);
    qWarning() << "AustrianSocialNumberFormData::setData" << data << role;
//    if (role==Qt::EditRole || role==Qt::DisplayRole) {
//        if (data.canConvert(QVariant::Int))  { // Tristate
//            m_Check->setCheckState(Qt::CheckState(data.toInt()));
//        }
//    }
    return true;
}

QVariant AustrianSocialNumberFormData::data(const int ref, const int role) const
{
    Q_UNUSED(ref);
    Q_UNUSED(role);
    return m_Widget->number();
}

void AustrianSocialNumberFormData::setStorableData(const QVariant &data)
{
    if (!data.isValid())
        return;
    m_OriginalValue = data.toString();
    m_Widget->setNumber(m_OriginalValue);
}

QVariant AustrianSocialNumberFormData::storableData() const
{
    return m_Widget->number();
}
