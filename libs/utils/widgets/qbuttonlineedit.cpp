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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 \class Utils::QButtonLineEdit
 \brief This widget can present two QToolButtons inside a QlineEdit: one left, the other on the right.
    QButtonLineEdit inherits QLineEdit
    After instanciating it, define the ToolButtons for the right and the left, or only one.
    The tooltip of the leftButton is shown in gray inside the LineEdit when the user has not typed
    anything in.
    You can delay the textChanged() signal using setDelayedSignals().
    All action pixmaps of the left and right buttons must be 16x16.
 */

#include "qbuttonlineedit.h"

#include <utils/log.h>

#include <QApplication>
#include <QTextDocument>
#include <QAction>
#include <QPainter>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QStyle>
#include <QToolButton>
#include <QTimer>

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

namespace Utils {
namespace Internal {
class QButtonLineEditPrivate
{
public:
    QButtonLineEditPrivate(QButtonLineEdit *parent) :
        _leftButton(0), _rightButton(0), _delayed(false),
        _rightPadding(0), _leftPadding(0),
        q(parent)
    {}

    QString constructStyleSheet(const QString &extraStyleSheet)
    {
        QStringList css;

        // Create padding for buttons
        css << QString("padding-left: %1px").arg(_leftPadding);
        css << QString("padding-right: %1px").arg(_rightPadding);

        // Analyse extraStyleSheet (remove all paddings)
        if (!extraStyleSheet.isEmpty()) {
            foreach(const QString &c, extraStyleSheet.split(";", QString::SkipEmptyParts)) {
                if (!c.startsWith("paddin",Qt::CaseInsensitive))
                    css << c;
            }
        }
        return QString("%1;").arg(css.join(";"));
    }

    void setSpecificStyleSheet()
    {
        q->setStyleSheet(constructStyleSheet(_extraCss));
    }

    // Connects leftbutton
    void prepareConnections()
    {
        if (_leftButton)
            q->connect(_leftButton, SIGNAL(triggered(QAction*)), q, SLOT(leftTrig(QAction*)));
    }

    // Updates placeHolderText of the LineEdit to the text of the current QAction that was last called.
    void updatePlaceholderText()
    {
        QString placeHolder;
        if (!_placeHolder.isEmpty()) {
            placeHolder = _placeHolder;
        } else if (_leftButton && _leftButton->defaultAction()) {
            placeHolder = QString("%1 %2")
                    .arg(cleanString(_leftButton->defaultAction()->text()))
                    .arg(QApplication::translate("Utils::QButtonLineEdit", "(press Alt up/down cursor to cycle)"));
        }
        q->setPlaceholderText(placeHolder);

        QString e;
        if (!_extraToolTipContext.isEmpty() && !_extraToolTipTr.isEmpty()) {
            e = QString("%1<br />").arg(QApplication::translate(_extraToolTipContext.toUtf8(), _extraToolTipTr.toUtf8()));
        }

        if (!_extraToolTipNonTr.isEmpty())
            e += QString("%1<br />").arg(_extraToolTipNonTr);

        placeHolder = placeHolder.replace(" ", "&nbsp;");
        if (!e.contains("<br />"))
            placeHolder = QString("<p>%1%2</p>").arg(e, placeHolder.replace(" ", "&nbsp;"));
        else
            placeHolder.prepend(e);

        q->setToolTip(placeHolder);
    }

public:
    QToolButton *_leftButton;
    QToolButton *_rightButton;
    QString _extraToolTipContext, _extraToolTipTr, _extraToolTipNonTr, _extraCss, _placeHolder;
    QTimer *_timer;
    bool _delayed;
    int _rightPadding, _leftPadding;

private:
    QButtonLineEdit *q;
};
} // Internal
} // Utils

QButtonLineEdit::QButtonLineEdit(QWidget *parent) :
    QLineEdit(parent),
    d_qble(new Internal::QButtonLineEditPrivate(this))
{
    static int handle = 0;
    handle++;
    if (objectName().isNull())
        setObjectName("QButtonLineEdit_" + QString::number(handle));
    d_qble->_timer = new QTimer(this);
    d_qble->_timer->setSingleShot(true);
}

QButtonLineEdit::~QButtonLineEdit()
{
    if (d_qble)
        delete d_qble;
    d_qble = 0;
}

/** Delay textChanged signal emition */
void QButtonLineEdit::setDelayedSignals(bool state)
{
    d_qble->_delayed = state;
    d_qble->_timer->stop();
    if (state) {
        blockSignals(true);
        connect(d_qble->_timer, SIGNAL(timeout()), this, SLOT(emitTextChangedSignal()));
    } else {
        blockSignals(false);
        disconnect(d_qble->_timer, SIGNAL(timeout()), this, SLOT(emitTextChangedSignal()));
    }
}

/**
 \brief Define the left button for the line edit.
 QButtonLineEdit takes ownership of the button. \n
 Text of selected action is shown in gray inside the line edit when it is empty. \n
 For now pixmaps of QAction must be sized (16x16). \n
 */
void QButtonLineEdit::setLeftButton(QToolButton *button)
{
    if (!button)
        return;
    button->setParent(this);
    d_qble->_leftButton = button;
    d_qble->_leftButton->setStyleSheet("border:none;padding: 0 0 0 2px;");
    d_qble->_leftButton->setCursor(Qt::ArrowCursor);

    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    d_qble->_leftPadding = button->sizeHint().width() + frameWidth + 1;

    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), button->sizeHint().height() + frameWidth * 2 + 2),
                   qMax(msz.height(), button->sizeHint().height() + frameWidth * 2 + 2));

    // set text to button toolTip
    d_qble->updatePlaceholderText();
    d_qble->prepareConnections();
    clearFocus();
    d_qble->setSpecificStyleSheet();
}

/**
 \brief Define the right button for the line edit.
 QButtonLineEdit takes ownership of the button. \n
 For now pixmaps of QAction must be sized (16x16). \n
*/
void QButtonLineEdit::setRightButton(QToolButton * button)
{
    if (!button)
        return;

    if (d_qble->_rightButton)
        delete d_qble->_rightButton;

    button->setParent(this);
    d_qble->_rightButton = button;
    d_qble->_rightButton->setStyleSheet("border:none; padding: 0;");
    d_qble->_rightButton->setCursor(Qt::ArrowCursor);

    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    d_qble->_rightPadding = button->sizeHint().width() + frameWidth + 1;
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), button->sizeHint().height() + frameWidth * 2 + 2),
                   qMax(msz.height(), button->sizeHint().height() + frameWidth * 2 + 2));

    d_qble->prepareConnections();
    clearFocus();
    d_qble->setSpecificStyleSheet();
}

/*!
 * \brief Convenience function for setting an icon on the right side.
 *
 * Calls setRightbutton(). Internally creates an empty action with the icon and
 * assigns it to the internal toolbutton. If there is no toolbutton, it creates one.
 */
void QButtonLineEdit::setRightIcon(QIcon icon)
{
    if (icon.isNull())
        return;

    // create a new action with given icon and assign it to the button
    QAction *action = new QAction(icon, "", this);
    QToolButton *button = new QToolButton();
    button->addAction(action);
    button->setDefaultAction(action);
    setRightButton(button);
}

/*!
 * \brief Convenience function for setting an icon on the left side.
 *
 * Calls SetLeftButton(). Internally creates an empty action with the icon and
 * assigns it to the internal toolbutton. If there is no toolbutton, it creates one.
 */
void QButtonLineEdit::setLeftIcon(QIcon icon)
{
    if (icon.isNull())
        return;

    // create a new action with given icon and assign it to the button
    QAction *action = new QAction(icon, "", this);
    QToolButton *button = new QToolButton();
    button->addAction(action);
    button->setDefaultAction(action);
    setLeftButton(button);
}

void QButtonLineEdit::resizeEvent(QResizeEvent *)
{
    if (d_qble->_leftButton) {
        QSize sz = d_qble->_leftButton->sizeHint();
        int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
        d_qble->_leftButton->move(rect().left() + frameWidth, (rect().bottom() + 1 - sz.height()) / 2);
    }
    if (d_qble->_rightButton) {
        QSize sz = d_qble->_rightButton->sizeHint();
        int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
        d_qble->_rightButton->move(rect().right() - frameWidth - sz.width(), (rect().bottom() + 2 - sz.height()) / 2);
    }
}

void QButtonLineEdit::emitTextChangedSignal()
{
    blockSignals(false);
    Q_EMIT textChanged(text());
//    Q_EMIT cursorPositionChanged(int, int);
//    Q_EMIT editingFinished();
    Q_EMIT returnPressed();
//    Q_EMIT selectionChanged();
//    Q_EMIT textEdited(QString);
    blockSignals(true);
}

/** \brief slot for triggered left button */
void QButtonLineEdit::leftTrig(QAction *action)
{
    d_qble->_leftButton->setDefaultAction(action);
    d_qble->updatePlaceholderText();
    clearFocus();
}

void QButtonLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (d_qble->_delayed)
        d_qble->_timer->stop();

    if (event->modifiers() & Qt::AltModifier) {
        if (!d_qble->_leftButton)
            return;
        const QList<QAction *> &list = d_qble->_leftButton->actions();

        if (list.count()) {
            int actual = list.indexOf(d_qble->_leftButton->defaultAction());
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
    } else if (d_qble->_delayed) {
        if (event->key()==Qt::Key_Enter) {
            blockSignals(false);
            Q_EMIT returnPressed();
            blockSignals(true);
        } else {
            const QString &t = text();
            if (t.isEmpty()) {
                blockSignals(false);
                Q_EMIT returnPressed();
                Q_EMIT textChanged("");
                blockSignals(true);
            } else {
                int delay = 300 - t.length()*t.length()*10;
                if (delay < 0)
                    delay = 0;
                d_qble->_timer->start(delay);
            }
        }
    }
    QLineEdit::keyPressEvent(event);
}

/** \brief Define rounded border for the QLineEdit */
void QButtonLineEdit::setRoundedCorners()
{
    // FIXME: Buggy
    setStyleSheet(QString("QLineEdit#%1, QFrame#%1 {"
                          "border-style: groove;"
                          "border-width: 1px;"
                          "border-radius: 6px;"
                          "}").arg(objectName()));
}

/**
 * Set the placeholder to use instead of the calculated placeHolder. By
 * default, the placeholder is computed from the left button defaultaction.
 * @param placeholder
 */
void QButtonLineEdit::setEditorPlaceholderText(const QString &placeholder)
{
    d_qble->_placeHolder = placeholder;
    d_qble->updatePlaceholderText();
}

void QButtonLineEdit::setTranslatableExtraToolTip(const QString &trContext, const QString &translatable)
{
    d_qble->_extraToolTipContext = trContext;
    d_qble->_extraToolTipTr = translatable;
}

void QButtonLineEdit::setExtraToolTip(const QString &nonTranslatable)
{
    d_qble->_extraToolTipNonTr = nonTranslatable;
}

void QButtonLineEdit::setExtraStyleSheet(const QString &extraCss)
{
    d_qble->_extraCss = extraCss;
    if (d_qble->_extraCss.contains("{")) {
        int b = d_qble->_extraCss.indexOf("{") + 1;
        int e = d_qble->_extraCss.indexOf("}", b);
        if (e>0) {
            d_qble->_extraCss = d_qble->_extraCss.mid(b, e-b);
        } else {
            LOG_ERROR("Wrong styleSheet -> {} " + extraCss);
            d_qble->_extraCss.clear();
        }
    }
    d_qble->setSpecificStyleSheet();
}

void QButtonLineEdit::clearExtraStyleSheet()
{
    d_qble->_extraCss.clear();
    d_qble->setSpecificStyleSheet();
}

void QButtonLineEdit::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d_qble->updatePlaceholderText();
        break;
    default:
        break;
    }
}
