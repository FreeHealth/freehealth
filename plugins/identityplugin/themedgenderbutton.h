/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developer:  Christian A. Reiter <christian.a.reiter@gmail.com>   *
 *  Contributors:                                                          *
 *       Eric Maeker                                                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IDENTITY_THEMEDGENDERBUTTON_H
#define IDENTITY_THEMEDGENDERBUTTON_H

#include <QToolButton>

/**
 * \file themedgenderbutton.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 18 Dec 2012
*/

namespace Identity {
namespace Internal {
class ThemedGenderButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap NOTIFY pixmapChanged USER true)

public:
    explicit ThemedGenderButton(QWidget* parent = 0);

    QPixmap pixmap() const;
    void setDefaultAction(QAction *action);
    QAction* defaultAction() const;
    QAction* deletePhotoAction() const;

public Q_SLOTS:
    void setPixmap(const QPixmap &pixmap);
    void clearPixmap();
    void setGenderImage(int genderIndex);

Q_SIGNALS:
    void pixmapChanged(const QPixmap &pix);

private:
    QPixmap m_pixmap;
    QAction* m_deletePhotoAction;
    QAction *m_separator;
    QAction *m_defaultAction;
    bool m_isDefaultGender;
};

} // namespace Internal
} // namespace Identity

#endif // IDENTITY_THEMEDGENDERBUTTON_H
