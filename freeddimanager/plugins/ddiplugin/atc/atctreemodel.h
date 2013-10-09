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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEDDIMANAGER_ATCTREEMODEL_H
#define FREEDDIMANAGER_ATCTREEMODEL_H

#include <QStandardItemModel>

/**
 * \file atctreemodel.h
 * \author Eric MAEKER
 * \version 0.10.0
 * \date 09 Oct 2013
*/

namespace DDI {
class DDICore;
namespace Internal {
class AtcTreeModelPrivate;
}

class AtcTreeModel : public QStandardItemModel
{
    Q_OBJECT
    friend class DDI::Internal::AtcTreeModelPrivate;
    friend class DDI::DrugsDBCore;

protected:
    AtcTreeModel(QObject * parent = 0);
    bool initialize();

public:
    enum DataRepresentation {
        ATC_Code = 0,
        ATC_EnglishLabel,
        ATC_FrenchLabel,
        ATC_DeutschLabel,
        ATC_CodeAndLabel,
        NumberOfColumn
    };
    ~AtcTreeModel();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QStringList getAtcLabel(const QStringList &codes);

    static bool insertAtcCodeToDatabase(const QString &connectionName);

private:
    static AtcTreeModel *_instance;
    Internal::AtcTreeModelPrivate *d;
};

}  //  End namespace DDI

#endif // FREEDDIMANAGER_ATCTREEMODEL_H
