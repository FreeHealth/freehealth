/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef MFDRUGSMODEL_H
#define MFDRUGSMODEL_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/dosagemodel.h>

#include <QAbstractTableModel>

/**
 * \file drugsmodel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 04 Mar 2011
*/
namespace DrugsDB {
class DrugsIO;
class InteractionManager;
class DosageModel;
class IDrug;

namespace Internal {
class DrugsModelPrivate;
}  // End Internal
}  // End DrugsDB


namespace DrugsDB {

class DRUGSBASE_EXPORT DrugsModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class Internal::DosageModel;
    friend class DrugsIO;
public:
    enum PrescriptionDeserializer {
        AddPrescription,
        AddWithoutDouble,
        ReplacePrescription
    };

    // INSTANCE CREATION
    DrugsModel(QObject * parent = 0);
    ~DrugsModel();
    static inline void setActiveModel(DrugsModel *model) {m_ActiveModel = model;}
    static inline DrugsModel *activeModel() {return m_ActiveModel;}

    // MODEL FUNCTIONS
    QModelIndex index(int row, int column, const QModelIndex &drugParent = QModelIndex()) const;

    // GETTING DATAS
    int rowCount(const QModelIndex & = QModelIndex()) const;
    int columnCount(const QModelIndex & = QModelIndex()) const { return DrugsDB::Constants::Drug::MaxParam; }
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant drugData(const QVariant &drugId, const int column);
    bool setDrugData(const QVariant &drugId, const int column, const QVariant &value);
    void resetModel();
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool prescriptionHasInteractions();
    bool prescriptionHasAllergies();

    // ADD / REMOVE DRUGS
    void setDrugsList(const QList<IDrug *> &list);
    const QList<IDrug *> &drugsList() const;
    void clearDrugsList();
    int addTextualPrescription(const QString &drugLabel, const QString &drugNote);
    int addDrug(IDrug *drug, bool automaticInteractionChecking = true);
    int addDrug(const QVariant &drugId, bool automaticInteractionChecking = true);
    int removeDrug(const QVariant &drugId);
    int removeLastInsertedDrug();
    bool containsDrug(const QVariant &drugId) const;
    IDrug *getDrug(const QVariant &drugId) const;

    // SORT / ORDER DRUGS INTO PRESCRIPTION
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    bool moveUp(const QModelIndex &item);
    bool moveDown(const QModelIndex &item);

    // FILTERS, MODES AND MODIFICATION STATE
    void showTestingDrugs(bool state);
    bool testingDrugsAreVisible() const;
    void setSelectionOnlyMode(bool b);
    bool isSelectionOnlyMode() const;
    void setModified(bool state);
    bool isModified() const;

    // DRAG AND DROP
    Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
//    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);


    // FOR DOSAGE MANAGEMENT
    Internal::DosageModel *dosageModel(const QVariant &drugUid);
    Internal::DosageModel *dosageModel(const QModelIndex &drugIndex);

    // FOR PRESCRIPTION FORMATTING
    static QString getFullPrescription(const IDrug *drug, bool toHtml = false, const QString &mask = QString::null);

    void warn();

Q_SIGNALS:
    void prescriptionResultChanged(const QString & html);
    void numberOfRowsChanged();

protected:
    void checkInteractions() const;

private Q_SLOTS:
    void dosageDatabaseChanged();

private:
    Internal::DrugsModelPrivate *d;
    static DrugsModel *m_ActiveModel;
};

}  // End DrugsDB

#endif
