/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_INTERNAL_FORMVIEWDELEGATE_H
#define FORM_INTERNAL_FORMVIEWDELEGATE_H

#include <QStyledItemDelegate>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file formviewdelegate.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 06 Oct 2012
*/

namespace Form {
class FormTreeModel;
namespace Internal {

class FormViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    FormViewDelegate(QObject *parent = 0);
    void setFormTreeModel(FormTreeModel *model);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

public:
    mutable QModelIndex pressedIndex;
    FormTreeModel *_formTreeModel;
};

} // namespace Internal
} // namespace Form

#endif // FORM_INTERNAL_FORMVIEWDELEGATE_H
