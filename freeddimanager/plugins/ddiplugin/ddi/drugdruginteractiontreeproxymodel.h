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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREETOOLBOX_DRUGDRUGINTERACTIONMODEL_H
#define FREETOOLBOX_DRUGDRUGINTERACTIONMODEL_H

#include <QAbstractItemModel>

/**
 * \file drugdruginteractionmodel.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 14 Oct 2013
*/

namespace DDI {
namespace Internal {
class DrugDrugInteraction;
class DrugDrugInteractionModelPrivate;
}  // namespace Internal

class DrugDrugInteractionModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        GeneralLabel = 0, // use it as a READ-ONLY data for the tree view
        HumanReadableSynthesis,     // same as the tooltip
        FirstInteractorName,
        SecondInteractorName,
        LevelCode,
        LevelName,
        LevelComboIndex,
        DateCreation,
        DateLastUpdate,
        ListOfUpdates,
        RiskFr,
        RiskEn,
        ManagementFr,
        ManagementEn,
        IsReviewed,
        IsReviewedCheckState,
        ReviewersStringList,
        Comment,
        InternalUid,
        InternalUidWithInteractors,
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

    explicit DrugDrugInteractionModel(QObject *parent = 0);
    ~DrugDrugInteractionModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool splitMultipleLevelInteraction(const QModelIndex &index, bool splitMirrored = true);

    static QStringList units();
    static QStringList repartitions();
    static QString unit(int index);
    static QString repartition(int index);

//    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
//    void fetchMore(const QModelIndex &parent = QModelIndex());

    QHash<QString, int> getLevelStatistics() const;

public Q_SLOTS:
    bool addDrugDrugInteraction(const Internal::DrugDrugInteraction &ddi);
    void filterInteractionsForInteractor(const QString &interactorName);
    void setActualReviewer(const QString &name);
    bool saveModel();

private:
    Internal::DrugDrugInteractionModelPrivate *d;
};

}  // End namespace DDI

#endif // FREETOOLBOX_DRUGDRUGINTERACTIONMODEL_H
