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
 *   Main Developer:  Christian A. Reiter <christian.a.reiter@gmail.com>   *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PIXMAPDELEGATE_H
#define PIXMAPDELEGATE_H

#include <QItemDelegate>

namespace Patients {
namespace Internal {


/*!
 * \brief The PixmapDelegate class
 *
 * Responsible for displaying a pixmap onto the IdentityWidget's QPushButton.
 * If there is no pixmap available, it displays a default picture according to the
 * selected patient's gender.
 */
class PixmapDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit PixmapDelegate(QObject *parent = 0);
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

};

} // end Internal
} // end Patients

#endif // PIXMAPDELEGATE_H
