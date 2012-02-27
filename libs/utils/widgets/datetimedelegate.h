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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DATETIMEDELEGATE_H
#define DATETIMEDELEGATE_H

#include <utils/global_exporter.h>

#include <QItemDelegate>
#include <QWidget>
#include <QObject>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QSize>
#include <QDateTime>
#include <QDate>

namespace Utils {

class UTILS_EXPORT DateTimeDelegate : public QItemDelegate
 {
     Q_OBJECT
 public:
     DateTimeDelegate(QObject *parent = 0, bool dateOnly = false);

     void setDateRange(const QDateTime &min, const QDateTime &max);
     void setDateRange(const QDate &min, const QDate &max);
     void setDateOnly(bool state);

     QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     void setEditorData(QWidget *editor, const QModelIndex &index) const;
     void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

     void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;

 private:
     bool m_IsDateOnly;
     QTime m_MaxTime, m_MinTime;
     QDate m_MaxDate, m_MinDate;
 };


} // end namespace Utils


#endif // DATETIMEDELEGATE_H
