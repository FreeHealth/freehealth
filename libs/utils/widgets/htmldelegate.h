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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_HTMLDELEGATE_H
#define UTILS_HTMLDELEGATE_H

#include <utils/global_exporter.h>
#include <QStyledItemDelegate>

/**
 * \file htmldelegate.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.2
 * \date 28 Dec 2011
*/

namespace Utils {
namespace Internal {
class HtmlDelegatePrivate;
} // namespace Internal

class UTILS_EXPORT HtmlDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit HtmlDelegate(QObject *parent = 0);
    ~HtmlDelegate();

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private Q_SLOTS:
    void treeView_indexDataChanged();

private:
    Internal::HtmlDelegatePrivate *d_html;
};

} // namespace Utils

#endif // UTILS_HTMLDELEGATE_H
