/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_QMENUITEMVIEW_H
#define UTILS_QMENUITEMVIEW_H

#include "../global_exporter.h"
#include <QMenu>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QAbstractItemModel;
QT_END_NAMESPACE

/**
 * \file qmenuitemview.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 14 May 2013
*/

namespace Utils {
namespace Internal {
class QMenuItemViewPrivate;
} // namespace Internal

class UTILS_EXPORT QMenuItemView : public QMenu
{
    Q_OBJECT
    
public:
    explicit QMenuItemView(QWidget *parent = 0);
    ~QMenuItemView();

    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    void setRootIndex(const QModelIndex & index);
    QModelIndex rootIndex() const;

Q_SIGNALS:
    void hovered(const QString &text) const;
    void triggered(const QModelIndex &index) const;

private Q_SLOTS:
    void aboutToShow();
    void triggered(QAction *action);
    void hovered(QAction *action);

private:
    Internal::QMenuItemViewPrivate *d;
};

} // namespace Utils

#endif  // UTILS_QMENUITEMVIEW_H

