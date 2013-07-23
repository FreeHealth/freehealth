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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugsbaseplugin.h"

#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/constants_html.h>
#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbaseinfo.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/protocolsbase.h>
#include <drugsbaseplugin/idruginteraction.h>
#include <drugsbaseplugin/druginteractionquery.h>
#include <drugsbaseplugin/druginteractionresult.h>
#include <drugsbaseplugin/idrug.h>
#include <drugsbaseplugin/drugsio.h>
#include <drugsbaseplugin/interactionmanager.h>
#include <drugsbaseplugin/dailyschememodel.h>
#include <drugsbaseplugin/globaldrugsmodel.h>
#include <drugsbaseplugin/druginteractioninformationquery.h>
#include <drugsbaseplugin/idrugengine.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/log.h>
#include <utils/randomizer.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_datetime.h>

#include <QTest>

using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;

// static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
// static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
// static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::DrugBaseCore &drugsCore() {return DrugsDB::DrugBaseCore::instance();}
static inline DrugsDB::InteractionManager &interactionManager() {return DrugsDB::DrugBaseCore::instance().interactionManager();}
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static inline DrugsDB::ProtocolsBase &protocolsBase() {return DrugsDB::DrugBaseCore::instance().protocolsBase();}
static inline DrugsDB::DrugsIO &drugsIo() {return DrugsDB::DrugBaseCore::instance().drugsIo();}

/** Plugin Unit-Test: test DrugBaseCore and DrugsBase initialization */
void DrugsBasePlugin::test_drugsbase_init()
{
    // Check drugscore initialization
    QVERIFY(drugsCore().isInitialized() == true);
    QVERIFY(drugsBase().isInitialized() == true);
    QVERIFY(protocolsBase().isInitialized() == true);
    QVERIFY(drugsIo().isInitialized() == true);

    // Check drugsbase initialization
    QVERIFY(drugsBase().actualDatabaseInformation() != 0);
    QVERIFY(drugsBase().actualDatabaseInformation()->identifier.isEmpty() == false);
    QVERIFY(drugsBase().actualDatabaseInformation()->fileName.isEmpty() == false);
    QVERIFY(drugsBase().actualDatabaseInformation()->version.isEmpty() == false);
#if DRUGS_DATABASE_VERSION == 0x000804
    QVERIFY(drugsBase().actualDatabaseInformation()->version == "0.8.4");
#elif DRUGS_DATABASE_VERSION == 0x000604
    QVERIFY(drugsBase().actualDatabaseInformation()->version == "0.6.4");
#endif
}

static inline DrugsDB::IDrug *getDrug(int row)
{
    // Create a virtual drug and prescription
    using namespace DrugsDB::Constants;
    DrugsDB::IDrug *drug = drugsBase().getDrugByUID("-1", QString::number(row+1));
    if (!drug) {
        LOG_ERROR_FOR("DrugPosologicSentencePreferences", "Unable to retreive a drug from the database");
    } else {
        Utils::Randomizer r;
        // r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
        double intakeFrom = r.randomDouble(1, 10);
        double intakeTo = r.randomDouble(intakeFrom, intakeFrom + 10.);
        bool intakeFromTo = r.randomBool();
        int period = r.randomInt(1, 10);
        int periodScheme = r.randomInt(Trans::Constants::Time::Seconds, Trans::Constants::Time::Decade);
        int interval = r.randomInt(0, 10);
        int intervalScheme = r.randomInt(Trans::Constants::Time::Seconds, Trans::Constants::Time::Decade);
        double durationFrom = r.randomDouble(1, 10);
        double durationTo = r.randomDouble(durationFrom, durationFrom + 10.);
        bool durationFromTo = r.randomBool();
        int durationScheme = r.randomInt(Trans::Constants::Time::Seconds, Trans::Constants::Time::Decade);
        int meal = r.randomInt(Trans::Constants::Time::Undefined, Trans::Constants::Time::WithOrWithoutFood);

        drug->setPrescriptionValue(Prescription::IntakesFrom, intakeFrom);
        drug->setPrescriptionValue(Prescription::IntakesTo, intakeTo);
        drug->setPrescriptionValue(Prescription::IntakesScheme, Trans::Constants::INTAKES);
        drug->setPrescriptionValue(Prescription::IntakesUsesFromTo, intakeFromTo);
        drug->setPrescriptionValue(Prescription::Period, period);
        drug->setPrescriptionValue(Prescription::PeriodScheme, Trans::ConstantTranslations::period(periodScheme));
        drug->setPrescriptionValue(Prescription::IntakesIntervalOfTime, interval);
        drug->setPrescriptionValue(Prescription::IntakesIntervalSchemeIndex, intervalScheme);
        drug->setPrescriptionValue(Prescription::DurationFrom, durationFrom);
        drug->setPrescriptionValue(Prescription::DurationTo, durationTo);
        drug->setPrescriptionValue(Prescription::DurationScheme, Trans::ConstantTranslations::period(durationScheme));
        drug->setPrescriptionValue(Prescription::DurationUsesFromTo, durationFromTo);
        drug->setPrescriptionValue(Prescription::MealTimeSchemeIndex, meal);
        drug->setPrescriptionValue(Prescription::Note, "This a note to take into account<br />written in two lines...");
        const QStringList &tags = Trans::ConstantTranslations::dailySchemeXmlTagList();
        QString daily = "<" + tags.at(1) + "=1>";
        daily += "<" + tags.at(3) + "=1>";
        daily += "<" + tags.at(6) + "=1>";
        drug->setPrescriptionValue(Prescription::SerializedDailyScheme, daily);
    }
    return drug;
}

void DrugsBasePlugin::test_drugsmodel_add_drug_pointer()
{
    // Test description:
    // Insert three drugs with prescription data in the model
    // Then check each index model data
    // Our testing drug is inserted at row 1
    int testingRow = 1;
    DrugsModel *model = new DrugsModel(this);
    model->addDrug(getDrug(0), false);
    IDrug *drug = getDrug(1);
    model->addDrug(drug, false);
    model->addDrug(getDrug(2), false);
    QVERIFY(model->rowCount() == 3);

    // Check indexes data from the model
    using namespace DrugsDB::Constants;
    QList<int> columns;
    columns
            << Prescription::IntakesFrom
            << Prescription::IntakesTo
            << Prescription::IntakesScheme
            << Prescription::IntakesUsesFromTo
            << Prescription::Period
            << Prescription::PeriodScheme
            << Prescription::IntakesIntervalOfTime
            << Prescription::IntakesIntervalSchemeIndex
            << Prescription::DurationFrom
            << Prescription::DurationTo
            << Prescription::DurationScheme
            << Prescription::DurationUsesFromTo
            << Prescription::MealTimeSchemeIndex
            << Prescription::Note
               ;
    foreach(int col, columns) {
        QVERIFY(drug->prescriptionValue(col) == model->data(model->index(testingRow, col)));
    }
}

void DrugsBasePlugin::test_drugsio_xmloutput()
{
    // XML output contains extra-data
    DrugsModel *model = new DrugsModel(this);
    model->addDrug(getDrug(0), false);
    QVERIFY(drugsIo().prescriptionToXml(model, "<extra>data</extra>").contains("<extra>data</extra>", Qt::CaseSensitive) == true);

    // Model -> XML -> Model without data lose/transformation
    model->addDrug(getDrug(1), false);
    model->addDrug(getDrug(2), false);
    QVERIFY(model->rowCount() == 3);
    QString xml = drugsIo().prescriptionToXml(model);
    QVERIFY(xml.isEmpty() == false);
    DrugsModel *model2 = new DrugsModel(this);
    drugsIo().prescriptionFromXml(model2, xml);
    QVERIFY(model->rowCount() == model2->rowCount());
    for(int i = 0; i < model->rowCount(); ++i) {
        for (int j = 0; j < model->columnCount(); ++j) {
            QModelIndex index1 = model->index(i, j);
            QModelIndex index2 = model2->index(i, j);
            QVERIFY(model->data(index1) == model2->data(index2));
        }
    }
    QVERIFY(drugsIo().prescriptionToXml(model2).isEmpty() == false);
    QVERIFY(drugsIo().prescriptionToXml(model) == drugsIo().prescriptionToXml(model2));
    delete model;
    delete model2;
}

// Tests:
// IDrug / IComponent interfaces
// DrugsModel (interactions handling)
// DrugsIO (toHtml)
// Interactions (create a DrugsDB::IDrugEngine and register it before)
