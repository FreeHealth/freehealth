/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef QBUTTONLINEEDIT_H
#define QBUTTONLINEEDIT_H

/**
 * \file qbuttonlineedit.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.2
 * \date 30 June 2010
*/

#include <utils/global_exporter.h>

#include <QLineEdit>
#include <QPainter>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QStyle>
#include <QToolButton>
#include <QAction>
#include <QTimer>

namespace Utils {

class UTILS_EXPORT QButtonLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    // Pixmap should be 16x16
    /** \brief constructor */
    QButtonLineEdit( QWidget * parent = 0 );
    /** \brief destructor*/
    ~QButtonLineEdit();

    void setDelayedSignals(bool state);

    void setLeftButton(QToolButton *button);
    void setRightButton(QToolButton *button);

    void setRoundedCorners();

    QString searchText() const;

private:
    QString emptyTextWithExtraText() const;
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent( QResizeEvent * );
    void focusInEvent ( QFocusEvent * event );
    void focusOutEvent ( QFocusEvent * event );
    void setSpecificStyleSheet( const QString & css );
    void prepareConnections();
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void emitTextChangedSignal();

    void leftTrig( QAction * action );

private:
    QToolButton *m_leftButton;
    QToolButton *m_rightButton;
    QString m_CSS;
    QString m_emptyString;
    QTimer *m_Timer;
    bool m_Delayed;
};

}

#endif
