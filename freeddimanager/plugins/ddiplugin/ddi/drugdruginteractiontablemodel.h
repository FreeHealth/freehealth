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
 * \date 01 Jan 2014
*/

namespace DDI {
class DrugInteractor;
class DDICore;

namespace Internal {
class DrugDrugInteractionTableModelPrivate;
} // namespace Internal

class DrugDrugInteractionTableModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class DDI::Internal::DrugDrugInteractionTableModelPrivate;
    friend class DDI::DDICore;

protected:
    DrugDrugInteractionTableModel(QObject *parent = 0);
    virtual bool initialize();
    bool onDdiDatabaseChanged();

public:
    enum DataRepresentation {
        Id = 0,
        Uid,
        FirstInteractorUid,
        SecondInteractorUid,
        FirstInteractorLabel,
        SecondInteractorLabel,
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
        FirstDoseFromUnitsId,          // Return the Id in the DrugDrugInteractionModel::units() stringList
        FirstDoseFromRepartitionId,    // Return the Id in the DrugDrugInteractionModel::repartition() stringList
        FirstDoseToValue,
        FirstDoseToUnitsId,            // Return the Id in the DrugDrugInteractionModel::units() stringList
        FirstDoseToRepartitionId,      // Return the Id in the DrugDrugInteractionModel::repartition() stringList
        SecondDoseUseFrom,
        SecondDoseUsesTo,
        SecondDoseFromValue,
        SecondDoseFromUnitsId,         // Return the Id in the DrugDrugInteractionModel::units() stringList
        SecondDoseFromRepartitionId,   // Return the Id in the DrugDrugInteractionModel::repartition() stringList
        SecondDoseToValue,
        SecondDoseToUnitsId,           // Return the Id in the DrugDrugInteractionModel::units() stringList
        SecondDoseToRepartitionId,     // Return the Id in the DrugDrugInteractionModel::repartition() stringList

        PMIDStringList,
        ColumnCount
    };

    enum DrugDrugInteractionError {
        ErrorsNotChecked                = 0x0000,
        NoError                         = 0x0001,
        NoRiskDefined                   = 0x0002,
        NotReviewed                     = 0x0004,
        IsMultilevel                    = 0x0008,
        TranslationMissing              = 0x0010,
        NoBibliographicReferences       = 0x0020,
        FirstInteractorDoesNotExists    = 0x0040,
        SecondInteractorDoesNotExists   = 0x0080
    };
    Q_DECLARE_FLAGS(DrugDrugInteractionErrors, DrugDrugInteractionError)

    ~DrugDrugInteractionTableModel();
    bool checkInteractionErrors();

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

    bool hasError(int row, DrugDrugInteractionError error);

    bool isMultiLevel(int row) const;
    bool splitMultiLevel(int row);
    bool isUntranslated(int row) const;
    QString humanReadableDrugDrugInteractionOverView(int row) const;

    int numberOfUnreviewed() const;

public Q_SLOTS:
    virtual bool submitAll();

private Q_SLOTS:
    void populateNewRowWithDefault(int row, QSqlRecord &record);

protected:
    void setSqlFilter(const QString &filter);

Q_SIGNALS:
    void unreviewedCountChanged();
    void unlinkedCountChanged();

private:
    Internal::DrugDrugInteractionTableModelPrivate *d;
};

class DrugDrugInteractionFilteredTableModel  : public DrugDrugInteractionTableModel
{
    Q_OBJECT
public:
    explicit DrugDrugInteractionFilteredTableModel(QObject *parent = 0);
    ~DrugDrugInteractionFilteredTableModel();
    bool initialize();

    void filterLastUpdated(const QDate &since);
    void filterNewItems(const QDate &since);
    void filterLastUpdatedAndNewItems(const QDate &since);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

public Q_SLOTS:
    bool submit();
    bool submitAll();
};

}  // namespace DDI

Q_DECLARE_OPERATORS_FOR_FLAGS(DDI::DrugDrugInteractionTableModel::DrugDrugInteractionErrors)

#endif // DDIMANAGER_DDIPLUGIN_DRUGDRUGINTERACTIONTABLEMODEL_H
