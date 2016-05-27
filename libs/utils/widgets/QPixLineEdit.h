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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef QPIXLINEEDIT_H
#define QPIXLINEEDIT_H

#include <utils/global_exporter.h>

#include <QLineEdit>
#include <QPainter>
#include <QPaintEvent>
#include <QStyle>

/**
 * \file QPixLineEdit.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 29 May 2011
*/

namespace Utils {

class UTILS_EXPORT QPixLineEdit :  public QLineEdit
{
    Q_OBJECT
public:

    // Pixmap should be 16x16
    /** */
    QPixLineEdit( QWidget * parent = 0 ) : QLineEdit( parent ){}

    /** */
    ~QPixLineEdit() {}

    /** */
    void setInsidePixmap( const QPixmap & pix )
    {
        p = pix; // pix is supposed to be a square
        int marg = ( this->height() - p.height() ) / 2;
        setStyleSheet( QString( "QLineEdit { padding-left: %1px; } " ).arg( p.width() + marg ) );
    }


private:
    void paintEvent( QPaintEvent * event )
    {
        QLineEdit::paintEvent( event );
        QPainter pix( this );
        int marg = ( this->height() - p.height() ) / 2;
        pix.drawPixmap( QPoint( 5, marg ) , p, QRect( 0, 0, this->width() , this->height() ) );
    }

    QPixmap p;
};

}

#endif
