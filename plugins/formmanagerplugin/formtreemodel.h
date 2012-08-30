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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FORM_FORMTREEMODEL_H
#define FORM_FORMTREEMODEL_H

#include <formmanagerplugin/formmanager_exporter.h>

#include <QStandardItemModel>

/**
 * \file formtreemodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 26 Aug 2012
*/

namespace Form {
class FormMain;
namespace Internal {
class FormTreeModelPrivate;
}

class FORM_EXPORT FormTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Label = 0,
        Uuid,
        EmptyColumn1,
        EmptyColumn2,
        MaxData
    };

    explicit FormTreeModel(Form::FormMain *emptyRootForm, QObject *parent = 0);
    ~FormTreeModel();

    void initialize();
    void refreshFormTree();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
//    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool isUniqueEpisode(const QModelIndex &index) const;
    bool isNoEpisode(const QModelIndex &index);
    bool isMultiEpisode(const QModelIndex &index) const {return !isUniqueEpisode(index);}
    Form::FormMain *formForIndex(const QModelIndex &index) const;

private:
    Internal::FormTreeModelPrivate *d;
};

} // namespace Form

#endif // FORM_FORMTREEMODEL_H
