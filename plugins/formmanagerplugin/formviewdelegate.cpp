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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "formviewdelegate.h"
#include "formtreemodel.h"
#include "iformitem.h"
#include "iformitemspec.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <QApplication>
#include <QPainter>
#include <QModelIndex>

#include <QDebug>

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

namespace Form {
namespace Internal {

FormViewDelegate::FormViewDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    _formTreeModel(0)
{
}

void FormViewDelegate::setFormTreeModel(FormTreeModel *model)
{
    _formTreeModel = model;
}

QSize FormViewDelegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    Q_ASSERT(_formTreeModel);
    const bool topLevel = !index.parent().isValid();
    if (topLevel) {
        // For top level item, user can define in the Form::FormMain extraData the height of the item
        // If the item has a defined height, use it
        // Else check the empty root form height
        // Else use the default (10px)
        QSize itemSize(10, 10);
        Form::FormMain *form = _formTreeModel->formForIndex(index);
        if (form) {
            if (form->extraData().contains("rootitemextraheight")) {
                itemSize = QSize(10, form->extraData().value("rootitemextraheight").toInt());
            } else {
                if (form->rootFormParent()->extraData().contains("rootitemextraheight")) {
                    itemSize = QSize(10, form->rootFormParent()->extraData().value("rootitemextraheight").toInt());
                }
            }
        }
        return QStyledItemDelegate::sizeHint(option, index) + itemSize;
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

void FormViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    // Add the fancy button
    if (option.state & QStyle::State_MouseOver) {
        if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
            pressedIndex = QModelIndex();
        QBrush brush = option.palette.alternateBase();
        if (index == pressedIndex)
            brush = option.palette.dark();
        painter->fillRect(option.rect, brush);
    }

    QStyledItemDelegate::paint(painter, option, index);

    // Draw fancy button
    if (index.column()==FormTreeModel::EmptyColumn1 &&
            (option.state & QStyle::State_MouseOver)) {
        QIcon icon;
        if (option.state & QStyle::State_Selected) {
            if (_formTreeModel->isNoEpisode(index))
                return;
            icon = theme()->icon(Core::Constants::ICONADDLIGHT);
        } else {
            if (_formTreeModel->isNoEpisode(index))
                return;
            icon = theme()->icon(Core::Constants::ICONADDDARK);
        }

        QRect iconRect(option.rect.right() - option.rect.height(),
                       option.rect.top(),
                       option.rect.height(),
                       option.rect.height());

        icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
    }
}

} // namespace Internal
} // namespace Form
