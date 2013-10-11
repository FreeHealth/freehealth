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
#ifndef DDIMANAGER_DDIPLUGIN_DRUGINTERACTORTABLEMODEL_H
#define DDIMANAGER_DDIPLUGIN_DRUGINTERACTORTABLEMODEL_H

#include <QAbstractTableModel>

/**
 * \file druginteractortablemodel.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 11 Oct 2013
*/

namespace DDI {
class DrugInteractor;

namespace Internal {
class DrugInteractorTableModelPrivate;
} // namespace Internal

class DrugInteractorTableModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class DDI::Internal::DrugInteractorTableModelPrivate;

public:
    enum DataRepresentation {
        Id = 0,
        Uuid,
        IsInteractingClass,
        IsReviewed,
        IsAutoFound,
        DoNotWarnDuplicated,
        TranslatedLabel,
        EnLabel,
        FrLabel,
        DeLabel,
        ClassInformationFr,
        ClassInformationEn,
        ClassInformationDe,
        ATCCodeStringList,
        DateOfCreation,
        DateLastUpdate,
        PMIDStringList,
        ChildrenUuid,
        Reference,
        Comment,
        ColumnCount
    };

    enum ShowData {
        InteractingClasses,
        InteractingMolecules,
        All
    };

    DrugInteractorTableModel(ShowData show = All, QObject *parent = 0);
    ~DrugInteractorTableModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const;

    DrugInteractor *createInteractingClass(const QString &initialLabel);
    DrugInteractor *createInteractor(const QString &initialLabel);

    int numberOfUnreviewed() const;
    int numberOfUnlinked() const;

public Q_SLOTS:
    bool submit();

Q_SIGNALS:
    void unreviewedCountChanged();
    void unlinkedCountChanged();

private:
    Internal::DrugInteractorTableModelPrivate *d;
};

}  // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_DRUGINTERACTORTABLEMODEL_H
