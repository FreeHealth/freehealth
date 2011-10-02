/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DRUGALLERGYENGINE_H
#define DRUGALLERGYENGINE_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/idrugengine.h>

#include <QMultiHash>

/**
 * \file allergyengine.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 26 Sept 2011
*/

QT_BEGIN_NAMESPACE
class QModelIndex;
class QStandardItemModel;
QT_END_NAMESPACE

namespace DrugsDB {
namespace Internal {

struct DrugAllergyEngineCache {
    int typeOfInteraction;
    QMultiHash<int, QString> bySubstrat;
};

struct DrugAllergyEngineDoTest {
    int typeOfInteraction;
    int typeOfSubstrat;
};

class DRUGSBASE_EXPORT DrugAllergyEngine : public IDrugEngine
{
    Q_OBJECT
public:
    enum TypeOfInteraction {
        NoInteraction = 0,
        Intolerance,
        SuspectedIntolerance,
        Allergy,
        SuspectedAllergy
    };

    enum TypeOfSubstrat {
        InnCode = 0,
        ClassInn,
        Drug
    };

    explicit DrugAllergyEngine(QObject *parent = 0);
    ~DrugAllergyEngine();

    // IDrugEngine interface
    bool init();
    bool isActive() const;
    bool isActiveByDefault() const {return true;}
    bool canComputeInteractions() const {return true;}

    bool isCalculatingDrugDrugInteractions() const {return false;}
    bool isCalculatingPatientDrugInteractions() const {return true;}

    QString name() const;
    QString shortName() const;
    QString tooltip() const;
    QString uid() const;
    QIcon icon(const int size = 0) const;
    QString iconFullPath(const int size = 0) const;

    int calculateInteractions(const QVector<IDrug *> &drugs);
    QVector<IDrugInteraction *> getAllInteractionsFound();
    QVector<IDrugInteractionAlert *> getAllAlerts(DrugInteractionResult *addToResult);

    // General interface (used by DrugsDB::GlobalDrugsModel)
    void check(const int typeOfInteraction, const QString &uid, const QString &drugGlobalAtcCode = QString::null, const int drugsDatabaseSourceId = -1);
    bool has(const int typeOfInteraction, const QString &uid, const int drugsDatabaseSourceId = -1);

    QStandardItemModel *drugPrecautionModel() const;

public Q_SLOTS:
    void setActive(bool state);

Q_SIGNALS:
    void allergiesUpdated();
    void intolerancesUpdated();

private:
    void clearDrugAllergyCache();
    void clearDrugIntoleranceCache();
    bool needTest(const int typeOfInteraction, const int typeOfSubstrat) const;
    bool test(const int typeOfInteraction, const int typeOfSubstrat, const QString &drugUid, const QStringList &toTest = QStringList());
    void updateDrugsPrecautionsModel() const;

private Q_SLOTS:
    void patientChanged();
    void refreshDrugsPrecautions(const QModelIndex &topleft, const QModelIndex &bottomright);

private:
    QVector<DrugAllergyEngineCache> m_Cache;
    QVector<DrugAllergyEngineDoTest> m_DoTests;
    QMultiHash<QString, int> m_ComputedInteractionCache;

    QVector<QString> m_ProcessedUid;

    QVector<IDrugInteraction *> m_Interactions;

    mutable QStandardItemModel *m_DrugPrecautionModel;
};

}  // End namespace Internal
}  // End namespace DrugsDB


#endif // DRUGALLERGYENGINE_H
