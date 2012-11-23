/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGALLERGYENGINE_H
#define DRUGALLERGYENGINE_H

#include <druginteractionsplugin/druginteractions_exporter.h>
#include <drugsbaseplugin/idrugengine.h>

#include <QMultiHash>
#include <QVector>

/**
 * \file allergyengine.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.4
 * \date 17 Jan 2012
*/

QT_BEGIN_NAMESPACE
class QModelIndex;
class QStandardItemModel;
QT_END_NAMESPACE

namespace DrugInteractions {
namespace Internal {

struct DrugAllergyEngineCache {
    int typeOfInteraction;
    QMultiHash<int, QString> bySubstrat;
};

struct DrugAllergyEngineDoTest {
    int typeOfInteraction;
    int typeOfSubstrat;
};

class DRUGINTERACTIONS_EXPORT DrugAllergyEngine : public DrugsDB::IDrugAllergyEngine
{
    Q_OBJECT
public:
    explicit DrugAllergyEngine(QObject *parent = 0);
    ~DrugAllergyEngine();

    // IDrugEngine interface
    bool init();
    bool isActive() const;
    bool isActiveByDefault() const {return true;}
    bool canComputeInteractions() const {return true;}

    bool isCalculatingDrugDrugInteractions() const {return false;}
    bool isCalculatingPatientDrugInteractions() const {return true;}
    bool isCalculatingPatientDrugAllergiesAndIntolerances() const {return true;}

    QString name() const;
    QString shortName() const;
    QString tooltip() const;
    QString uid() const;
    QIcon icon(const int size = 0) const;
    QString iconFullPath(const int size = 0) const;

    int calculateInteractions(const QVector<DrugsDB::IDrug *> &drugs);
    QVector<DrugsDB::IDrugInteraction *> getAllInteractionsFound();
    QVector<DrugsDB::IDrugInteractionAlert *> getAllAlerts(DrugsDB::DrugInteractionResult *addToResult);

    // General interface (used by DrugsDB::GlobalDrugsModel)
    void check(const int typeOfInteraction, const QString &uid, const QString &drugGlobalAtcCode = QString::null, const int drugsDatabaseSourceId = -1);
    bool has(const int typeOfInteraction, const QString &uid, const int drugsDatabaseSourceId = -1);

    QStandardItemModel *drugPrecautionModel() const;
    QAbstractItemModel *precautionModel() const {return drugPrecautionModel();}

public Q_SLOTS:
    void setActive(bool state);

private:
    void clearDrugAllergyCache();
    void clearDrugIntoleranceCache();
    bool needTest(const int typeOfInteraction, const int typeOfSubstrat) const;
    bool test(const int typeOfInteraction, const int typeOfSubstrat, const QString &drugUid, const QStringList &toTest = QStringList());
    void updateDrugsPrecautionsModel() const;

private Q_SLOTS:
    void onCurrentPatientChanged();
    void refreshDrugsPrecautions(const QModelIndex &topleft, const QModelIndex &bottomright);

private:
    QVector<DrugAllergyEngineCache> m_Cache;
    QVector<DrugAllergyEngineDoTest> m_DoTests;
    QMultiHash<QString, int> m_ComputedInteractionCache;

    QVector<QString> m_ProcessedUid;

    QVector<DrugsDB::IDrugInteraction *> m_Interactions;

    mutable QStandardItemModel *m_DrugPrecautionModel;
};

}  // End namespace Internal
}  // End namespace DrugInteractions


#endif // DRUGALLERGYENGINE_H
