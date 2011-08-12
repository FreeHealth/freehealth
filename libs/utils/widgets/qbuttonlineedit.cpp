/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
 \class Utils::QButtonLineEdit
 \brief This widget can present inside a line edit two QToolButtons : one left, the other on the right.
    QButtonLineEdit inherits QLineEdit
    After instanciate it, define the Tools Buttons for the right and the left, or only one.
    The tooltip of the leftButton is shown in gray inside the line edit when the user does not tape in.
    To retreive the text of the QLineEdit use the function : searchText().
    You can delay the textChanged() signal using setDelayedSignals().
 */

#include "qbuttonlineedit.h"

#include <QApplication>
#include <QTextDocument>
#include <QAction>

#include <QDebug>

using namespace Utils;

static inline QString cleanString(const QString &s)
{
    QString ret = s;
    if (Qt::mightBeRichText(ret)) {
        ret.replace(QRegExp("<[^>]*>"), "");
        ret = ret.trimmed();
    }
    return ret;
}


QButtonLineEdit::QButtonLineEdit(QWidget *parent) :
        QLineEdit(parent), m_leftButton(0), m_rightButton(0), m_Delayed(false)
{
    static int handle = 0;
    handle++;
    if (objectName().isNull())
        setObjectName("QButtonLineEdit_" + QString::number(handle));
    m_Timer = new QTimer(this);
    m_Timer->setSingleShot(true);
}

QButtonLineEdit::~QButtonLineEdit()
{
}

void QButtonLineEdit::setDelayedSignals(bool state)
{
    m_Delayed = state;
    m_Timer->stop();
    if (state) {
        blockSignals(true);
        connect(m_Timer, SIGNAL(timeout()), this, SLOT(emitTextChangedSignal()));
    } else {
        blockSignals(false);
        disconnect(m_Timer, SIGNAL(timeout()), this, SLOT(emitTextChangedSignal()));
    }
}

/**
 \brief Define the Left button for the line edit.
 QButtonLineEdit takes ownership of the button. \n
 Text of selected action is show in gray inside the line edit when nothing lies in. \n
 For now pixmaps of QAction must be sized (16x16). \n
 */
void QButtonLineEdit::setLeftButton(QToolButton * button)
{
    button->setParent(this);
    m_leftButton = button;
    m_leftButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");

    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    m_CSS.append(QString("padding-left: %1px;").arg(button->sizeHint().width() + frameWidth));
//    setStyleSheet(QString("QLineEdit { %1 }").arg(m_CSS));

    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), button->sizeHint().height() + frameWidth * 2 + 2),
                   qMax(msz.height(), button->sizeHint().height() + frameWidth * 2 + 2));

    // set text to button toolTip
//    setText(m_leftButton->toolTip());
    m_emptyString = cleanString(m_leftButton->toolTip());
    setSpecificStyleSheet("color:gray;");
    prepareConnections();
    clearFocus();
}

/**
 \brief Define the Left button for the line edit.
 QButtonLineEdit takes ownership of the button. \n
 For now pixmaps of QAction must be sized (16x16). \n
*/
void QButtonLineEdit::setRightButton(QToolButton * button)
{
    button->setParent(this);
    m_rightButton = button;
    m_rightButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    m_CSS.append(QString("padding-right: %1px;").arg(button->sizeHint().width() + frameWidth + 1));
    setSpecificStyleSheet("");
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), button->sizeHint().height() + frameWidth * 2 + 2),
                   qMax(msz.height(), button->sizeHint().height() + frameWidth * 2 + 2));
    prepareConnections();
    clearFocus();
}

void QButtonLineEdit::resizeEvent(QResizeEvent *)
{
    if (m_leftButton) {
        QSize sz = m_leftButton->sizeHint();
        int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
        m_leftButton->move(rect().left() + frameWidth ,
                          (rect().bottom() + 1 - sz.height()) / 2);
    }
    if (m_rightButton) {
        QSize sz = m_rightButton->sizeHint();
        int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
        m_rightButton->move(rect().right() - frameWidth - sz.width(),
                           (rect().bottom() +2 - sz.height()) / 2);
    }
}

void QButtonLineEdit::prepareConnections()
{
    if (m_leftButton)
        connect(m_leftButton, SIGNAL(triggered(QAction*)), this, SLOT(leftTrig(QAction*)));
}

void QButtonLineEdit::emitTextChangedSignal()
{
    blockSignals(false);
    Q_EMIT textChanged(searchText());
//    Q_EMIT cursorPositionChanged(int, int);
//    Q_EMIT editingFinished();
    Q_EMIT returnPressed();
//    Q_EMIT selectionChanged();
//    Q_EMIT textEdited(QString);
    blockSignals(true);
}

void QButtonLineEdit::leftTrig(QAction *action)
{
    m_leftButton->setDefaultAction(action);
    m_emptyString = cleanString(action->text());
    if (text().isEmpty() || (text() == m_emptyString)) {
        setText(emptyTextWithExtraText());
        setSpecificStyleSheet("color:gray;");
    }
//    QString t = ;
//    t.replace(" ", "&nbsp;");
//    t.append("<p style='white-space:pre'>");
//    t.prepend("</p>");
    setToolTip(emptyTextWithExtraText());
    clearFocus();
}

QString QButtonLineEdit::emptyTextWithExtraText() const
{
    return m_emptyString + " " + tr("(press Alt up/down cursor to cycle)");
}

void QButtonLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (m_Delayed)
        m_Timer->stop();

    if (event->modifiers() & Qt::AltModifier) {
        const QList<QAction *> &list = m_leftButton->actions();

        if (list.count()) {
            int actual = list.indexOf(m_leftButton->defaultAction());
            QAction *a = 0;

            if (event->key()==Qt::Key_Down) {
                ++actual;
                if (actual>=list.count())
                    actual = 0;
                a = list.at(actual);
            } else if (event->key()==Qt::Key_Up) {
                --actual;
                if (actual<0)
                    actual = list.count() - 1;
                a = list.at(actual);
            }
            if (a) {
                a->trigger();
                leftTrig(a);
                setFocus();
                QHelpEvent *e = new QHelpEvent(QEvent::ToolTip, pos(), mapToGlobal(pos()));
                QLineEdit::event(e);
                return;
            }
        }
    } else if (m_Delayed) {
        if (event->key()==Qt::Key_Enter) {
            blockSignals(false);
            Q_EMIT returnPressed();
            blockSignals(true);
        } else {
            const QString &t = searchText();
            if (t.isEmpty()) {
                blockSignals(false);
                Q_EMIT returnPressed();
                Q_EMIT textChanged("");
                blockSignals(true);
            } else {
                int delay = 300 - t.length()*t.length()*10;
                if (delay < 0)
                    delay = 0;
                m_Timer->start(delay);
            }
        }
    }
    QLineEdit::keyPressEvent(event);
}

void QButtonLineEdit::focusInEvent(QFocusEvent *event)
{
     if (text()==emptyTextWithExtraText()) {
         clear();
         setSpecificStyleSheet("color:black;");
     }
     QLineEdit::focusInEvent(event);
}

void QButtonLineEdit::focusOutEvent(QFocusEvent *event)
{
    if (text().isEmpty()) {
        bool block = blockSignals(true);
        setText(emptyTextWithExtraText());
        setSpecificStyleSheet("color:gray;");
        blockSignals(block);
    }
    QLineEdit::focusOutEvent(event);
}

/**
\brief Get the text of the line edit. If the text is gray (text of selected action inside the left button), then
an empty QString is returned.
*/
QString QButtonLineEdit::searchText() const
{
    if (text()==emptyTextWithExtraText()) //styleSheet().contains("color:gray"))
        return QString::null;
    return text();
}

/**
  \brief Define rounded border for the QLineEdit
  \todo Buggy
*/
void QButtonLineEdit::setRoundedCorners()
{
    qApp->setStyleSheet(QString("QLineEdit#%1, QFrame#%1 {"
                                  "border-style: groove;"
                                  "border-width: 1px;"
                                  "border-radius: 6px;"
                                  "}").arg(objectName()));
}

void QButtonLineEdit::setSpecificStyleSheet(const QString &css)
{
    setStyleSheet(QString("QLineEdit#%1 { %2; %3 }").arg(objectName(), m_CSS, css));
}

//void QButtonLineEdit::retranslateUi()
//{
//}

void QButtonLineEdit::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        if (styleSheet().contains("color:gray")) {
            if (m_leftButton->defaultAction()) {
                m_emptyString = m_leftButton->defaultAction()->text();
            } else {
                m_emptyString.clear();
            }
            setText(emptyTextWithExtraText());
            setToolTip(emptyTextWithExtraText());
        }
        break;
    default:
        break;
    }
}
