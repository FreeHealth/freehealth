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
 *   Main Developer : Christian A. Reiter <christian.a.reiter@gmail.com>   *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PIXMAPBUTTON_H
#define PIXMAPBUTTON_H

#include <QPushButton>

namespace Patients {
/*!
 * \class PixmapButton
 * \brief This class provides a QPushButton with a displayed Pixmap on it.
 *
 * The normal Qt QPushButton only can store a QIcon and has ho pixmap property. This
 * can be unconvenient when using the button as display widget in a MVC pattern as
 * widget - a QDataWidgetMapper doesn't know how to handle a QPushbutton.
 * Here comes the PixmapButton. It still displays a QIcon, but stores a QPixmap underneath.
 * It also provides a \e pixmap property for easy interacting with a QDataWidgetMapper.
 *
 */
class PixmapButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
//    Q_PROPERTY(QAction defaultAction READ defaultAction() WRITE setDefaultAction)
public:
    explicit PixmapButton(QWidget* parent = 0);
    QPixmap pixmap() const;
    void setDefaultAction(QAction *action);
    QAction* defaultAction() const;

Q_SIGNALS:

public Q_SLOTS:
    void setPixmap(const QPixmap& pixmap);
    void clearPixmap();

private:
    QPixmap m_pixmap;
    QAction* m_deletePhotoAction;
    QAction *m_defaultAction;
};

} // end Patients

#endif // PIXMAPBUTTON_H
