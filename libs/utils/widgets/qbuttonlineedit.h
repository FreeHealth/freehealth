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
 *   Main Developer : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef QBUTTONLINEEDIT_H
#define QBUTTONLINEEDIT_H

/**
 * \file qbuttonlineedit.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 10 Jan 2012
*/

#include <utils/global_exporter.h>

#include <QString>
#include <QLineEdit>
QT_BEGIN_NAMESPACE
class QToolButton;
class QTimer;
QT_END_NAMESPACE

namespace Utils {

class UTILS_EXPORT QButtonLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    QButtonLineEdit( QWidget * parent = 0 );
    ~QButtonLineEdit();

    void setDelayedSignals(bool state);

    void setLeftButton(QToolButton *button);
    void setRightButton(QToolButton *button);

    void setRoundedCorners();

protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *);

private:
    void updatePlaceholderText();
    void setSpecificStyleSheet(const QString &css = QString::null);
    void prepareConnections();
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void emitTextChangedSignal();
    void leftTrig(QAction *action);

private:
    QToolButton *m_leftButton;
    QToolButton *m_rightButton;
    QString m_CSS;
    QString m_emptyString;
    QTimer *m_Timer;
    bool m_Delayed;
};

}  // End namespace Utils

#endif
