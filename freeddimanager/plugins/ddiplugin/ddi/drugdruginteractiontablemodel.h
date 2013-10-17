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
#ifndef DDIMANAGER_DDIPLUGIN_DRUGDRUGINTERACTIONTABLEMODEL_H
#define DDIMANAGER_DDIPLUGIN_DRUGDRUGINTERACTIONTABLEMODEL_H

#include <QAbstractTableModel>
QT_BEGIN_NAMESPACE
class QSqlRecord;
QT_END_NAMESPACE

/**
 * \file drugdruginteractiontablemodel.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 17 Oct 2013
*/

namespace DDI {
class DrugInteractor;

namespace Internal {
class DrugDrugInteractionTableModelPrivate;
} // namespace Internal

class DrugDrugInteractionTableModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class DDI::Internal::DrugDrugInteractionTableModelPrivate;

public:
    enum DataRepresentation {
        Id = 0,
        Uid,
        FirstInteractorUid,
        SecondInteractorUid,
        IsReviewed,
        IsValid,
        LevelCode,
        LevelName,
        LevelComboIndex,
        DateCreation,
        DateLastUpdate,
        RiskFr,
        RiskEn,
        ManagementFr,
        ManagementEn,
        ReviewersStringList,
        Source,
        Comment,
        FirstInteractorRouteOfAdministrationIds,
        SecondInteractorRouteOfAdministrationIds,

        // Doses
        FirstDoseUseFrom,
        FirstDoseUsesTo,
        FirstDoseFromValue,
        FirstDoseFromUnits,          // Return the Id in the DrugDrugInteractionModel::units() stringList
        FirstDoseFromRepartition,    // Return the Id in the DrugDrugInteractionModel::repartition() stringList
        FirstDoseToValue,
        FirstDoseToUnits,            // Return the Id in the DrugDrugInteractionModel::units() stringList
        FirstDoseToRepartition,      // Return the Id in the DrugDrugInteractionModel::repartition() stringList
        SecondDoseUseFrom,
        SecondDoseUsesTo,
        SecondDoseFromValue,
        SecondDoseFromUnits,         // Return the Id in the DrugDrugInteractionModel::units() stringList
        SecondDoseFromRepartition,   // Return the Id in the DrugDrugInteractionModel::repartition() stringList
        SecondDoseToValue,
        SecondDoseToUnits,           // Return the Id in the DrugDrugInteractionModel::units() stringList
        SecondDoseToRepartition,     // Return the Id in the DrugDrugInteractionModel::repartition() stringList

        PMIDStringList,
        ColumnCount
    };

    DrugDrugInteractionTableModel(QObject *parent = 0);
    ~DrugDrugInteractionTableModel();
    virtual bool initialize();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual void fetchMore(const QModelIndex &parent);
    virtual bool canFetchMore(const QModelIndex &parent) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

//    virtual QVariant headerData(int section, Qt::Orientation orientation,
//                                int role = Qt::DisplayRole) const;

    static QStringList units();
    static QStringList repartitions();
    static QString unit(int index);
    static QString repartition(int index);

    int numberOfUnreviewed() const;

public Q_SLOTS:
    virtual bool submitAll();

private Q_SLOTS:
    void populateNewRowWithDefault(int row, QSqlRecord &record);

Q_SIGNALS:
    void unreviewedCountChanged();
    void unlinkedCountChanged();

private:
    Internal::DrugDrugInteractionTableModelPrivate *d;
};

}  // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_DRUGDRUGINTERACTIONTABLEMODEL_H
