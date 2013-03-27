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
#include "allergyengine.h"

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/idruginteraction.h>
#include <drugsbaseplugin/druginteractionresult.h>
#include <drugsbaseplugin/idruginteractionalert.h>
#include <drugsbaseplugin/idrug.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>

#include <translationutils/constanttranslations.h>

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

using namespace DrugInteractions::Internal;
using namespace DrugsDB;
using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;

namespace {

    const char* const LIST_MASK =
            "<table border=1 cellpadding=2 cellspacing=2 width=100%>\n"
            "<tr>\n"
            "  <td align=center>\n"
            "   <span style=\"font-weight:bold\">%1\n</span>"
            "  </td>\n"
//            "<tr>\n"
//            "  <td colspan=2 align=center>\n"
//            "   <span style=\"font-weight:bold\">%2\n</span>"
//            "</td>\n"
            "</tr>\n"
            "<tr><td>%2</td></tr>\n"
            "</table>\n";

class AllergyInteraction : public IDrugInteraction {
public:
    enum InteractionLevel {
        InteractionLevel_High = 3,
        InteractionLevel_Medium = 2,
        InteractionLevel_Low = 1
    };

    AllergyInteraction(IDrugEngine *parentEngine) : IDrugInteraction(parentEngine), m_Engine(parentEngine) {}
    ~AllergyInteraction() {}

    void setValue(const int ref, const QVariant &value)
    {
        m_Infos.insert(ref, value);
    }

    QVariant value(const int ref) const
    {
        return m_Infos.value(ref);
    }

    // IDrugInteraction interface
    IDrugEngine *engine() const {return m_Engine;}

    bool isDrugDrugInteraction() const {return false;}
    bool isPotentiallyInappropriate() const {return false;}

    QString type() const {return "bla bla";}

    QList<IDrug *> drugs() const {return m_InteractingDrugs;}

    QIcon icon(const int levelOfWarning, const int iconsize) const
    {
        Q_UNUSED(levelOfWarning);
        return theme()->icon(Constants::I_DRUGENGINE, Core::ITheme::IconSize(iconsize));
    }

    QString header(const QString &separator = QString::null) const
    {
        Q_UNUSED(separator);
        return QString("Allergy");
    }

    QString risk(const QString &lang = QString::null) const
    {
        Q_UNUSED(lang);
        return QString();
    }

    QString management(const QString &lang = QString::null) const
    {
        Q_UNUSED(lang);
        return QString();
    }

    QString referencesLink(const QString &lang = QString::null) const
    {
        Q_UNUSED(lang);
//        return m_Infos.value(PIM_ReferencesLink).toString();
        return QString();
    }

    QString toHtml(bool detailled = false) const
    {
        Q_UNUSED(detailled);
        return QString();
    }

    int sortIndex() const {return 1;} //return m_Infos.value(Level).toInt();}

    void addInteractingDrug(IDrug *drug) {m_InteractingDrugs << drug;}

    QString levelToString(const int level)
    {
        switch (level) {
        case 3: return tkTr(Trans::Constants::HIGH);
        case 2: return tkTr(Trans::Constants::MEDIUM);
        case 1: return tkTr(Trans::Constants::LOW);
        }
        return QString();
    }

private:
    IDrugEngine *m_Engine;
    QHash<int, QVariant> m_Infos;
    QList<IDrug *> m_InteractingDrugs;
};


class AllergyAlert : public IDrugInteractionAlert
{
public:
    AllergyAlert(DrugInteractionResult *result, IDrugEngine *engine) :
            IDrugInteractionAlert(engine), m_Overridden(false), m_Result(result)
    {
    }

    ~AllergyAlert() {}

    QString engineUid() const {return Constants::ALLERGY_ENGINE_UID;}

    // static alert
    QIcon icon(const IDrug *drug, const DrugInteractionInformationQuery &query) const
    {
        Q_UNUSED(drug);
//        if (!m_Result->testedDrugs().contains((IDrug*)drug))
//            return QIcon();
        if (!query.engineUid.isEmpty() && query.engineUid!=Constants::ALLERGY_ENGINE_UID) {
            return QIcon();
        }
////        int level = getMaximumTypeOfIAM(m_Result->interactions(), drug);
//        Core::ITheme *th = theme();
        Core::ITheme::IconSize size = Core::ITheme::IconSize(query.iconSize);
        return theme()->icon(DrugsDB::Constants::I_ALLERGYENGINE, size);
    }

    QString message(const IDrug *drug, const DrugInteractionInformationQuery &query) const
    {
        Q_UNUSED(query);
        Q_UNUSED(drug);
//        QString toReturn;
//        if (!m_Result->testedDrugs().contains((IDrug*)drug))
//            return toReturn;

//        // get all interactions related to the drug
//        QVector<IDrugInteraction *> interactions = m_Result->getInteractions(drug, Constants::ALLERGY_ENGINE_UID);

//        QString tmp;
//        switch (query.messageType)
//        {
//        case DrugInteractionInformationQuery::DetailledToolTip:
//        {
//            qWarning() << "****** DetailledTooltip";

//            // get all different types
//            QStringList types;
//            for(int j=0; j < interactions.count(); ++j) {
//                IDrugInteraction *di = interactions.at(j);
//                PimInteraction *pim = static_cast<PimInteraction *>(di);
//                Q_ASSERT(pim);
//                QString t = pim->value(PimInteraction::PIM_TypeName).toString();
//                if (!types.contains(t))
//                    types << t;
//            }

//            // foreach types construct the html output
//            foreach(const QString &type, types) {
//                QMap<int, QString> lines; // K=level - V=html
//                QVector<int> pimIds;
//                for(int j=0; j < interactions.count(); ++j) {
//                    IDrugInteraction *di = interactions.at(j);
//                    PimInteraction *pim = static_cast<PimInteraction *>(di);
//                    Q_ASSERT(pim);
//                    if (pim->value(PimInteraction::PIM_TypeName).toString()!=type)
//                        continue;

//                    if (pimIds.contains(pim->value(PimInteraction::PIM_ID).toInt()))
//                        continue;
//                    pimIds << pim->value(PimInteraction::PIM_ID).toInt();

//                    int level = pim->value(PimInteraction::PIM_Level).toInt();
//                    int typeId = -1;

//                    // Minimal alerts (only High levels)
//                    if (level==PimInteraction::InteractionLevel_High && (query.levelOfWarningStaticAlert <= Constants::MinimumLevelOfWarning))
//                        typeId = PimInteraction::InteractionLevel_High;
//                    // Moderate alerts (medium alerts)
//                    else if (level==PimInteraction::InteractionLevel_Medium && (query.levelOfWarningStaticAlert <= Constants::ModerateLevelOfWarning))
//                        typeId = PimInteraction::InteractionLevel_Medium;
//                    // Maximum alerts (all alerts)
//                    else if (level==PimInteraction::InteractionLevel_Low && (query.levelOfWarningStaticAlert <= Constants::MaximumLevelOfWarning))
//                        typeId = PimInteraction::InteractionLevel_Low;

//                    if (typeId==-1)
//                        continue;

////                    pim->warn();

//                    QString &ditmp = lines[typeId];
//                    if (!ditmp.contains(pim->header())) {
//                        ditmp += QString("    <li>%1</li>\n")
//                                .arg(pim->header());
//                    }
//                }

//                qWarning() << type << lines;

//                if (lines.isEmpty())
//                    continue;

//                QString html;
//                QMap<int, QString>::const_iterator i = lines.constEnd();
//                --i;
//                while (true) {
//                    if (!i.value().isEmpty()) {
//                        html += QString("<ul compact>"
//                                       "  <li><b>%1</b></li>\n"
//                                       "  <ul>\n"
//                                       "%2"
//                                       "  </ul>\n"
//                                       "</ul>\n"
//                                       )
//                                .arg(PimInteraction::levelToString(i.key()))
//                                .arg(i.value());
//                    }
//                    if (i==lines.constBegin())
//                        break;
//                    --i;
//                }
//                tmp += QString("<tr><td align=center width=100%><b>%1</b></td></tr><tr><td>%2</td></tr>")
//                        .arg(type)
//                        .arg(html);
//            }
//            if (tmp.isEmpty())
//                return QString();

//            tmp = QString("<br /><table widht=100% border=1>%1</table>")
//                    .arg(tmp);

//            break;
//        }
//        case DrugInteractionInformationQuery::ShortToolTip:
//        {
//            qWarning() << "****** ShortToolTip";
//            for(int j=0; j < interactions.count(); ++j) {
//                IDrugInteraction *di = interactions.at(j);
//                PimInteraction *pim = static_cast<PimInteraction *>(di);
//                if (!pim)
//                    continue;
//                tmp += "-&nbsp;" + pim->interactingAtcLabels().join(";") + "<br />";
//            }
//            if (!tmp.isEmpty()) {
//                tmp.chop(6);
//                toReturn.append(QString(LIST_MASK)
//                                .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::DrugAllergyEngine_TEXT))
//                                .arg(tmp));
//            }
//            break;
//        }
//        case DrugInteractionInformationQuery::InformationAlert:
//        {
//            qWarning() << "****** InformationAlert";
//            QHash<int, QString> riskLines; //k=riskMasterLid, line
//            QMultiHash<int, int> levelLines; //k=level, v=riskMasterLid
//            QVector<int> interactionAtcIds;
//            QVector<int> interactionIds;
//            for(int j=0; j < interactions.count(); ++j) {
//                // Get interaction
//                IDrugInteraction *di = interactions.at(j);
//                PimInteraction *pim = static_cast<PimInteraction *>(di);
//                Q_ASSERT(pim);
//                if (!pim)
//                    continue;

//                // interaction with this interactor already included ?
//                const QVector<int> &pimRelAtc = pim->allAtcRelated();
//                bool includeIt = false;
//                for(int aa = 0; aa < pimRelAtc.count(); ++aa) {
//                    int testAtc = pimRelAtc.at(aa);
//                    int interactorAtcId = -1;
//                    if (drug->allInnAndInteractingClassesIds().contains(testAtc)) {
//                        interactorAtcId = testAtc;
//                    }
//                    if (interactionAtcIds.contains(interactorAtcId))
//                        continue;
//                    else
//                        includeIt = true;
//                    interactionAtcIds << interactorAtcId;
//                }
//                if (!includeIt)
//                    continue;

//                // interaction already included ?
//                if (interactionIds.contains(pim->value(PimInteraction::PIM_ID).toInt()))
//                    continue;
//                interactionIds << pim->value(PimInteraction::PIM_ID).toInt();

//                pim->warn();

//                // get the line related to the level of pim
//                //                    int typeId = pim->value(PimInteraction::PIM_Level).toInt();

//                // TODO: manage levelOfWarning

//                int riskId = pim->value(PimInteraction::PIM_RiskMasterLid).toInt();
//                int level =  pim->value(PimInteraction::PIM_Level).toInt();
//                // construct line of risk content
//                QString riskLine;
//                foreach(IDrug *drug, pim->drugs()) {
//                    riskLine = QString("&nbsp;&nbsp;*&nbsp;%1: %2<br />\n")
//                            .arg(drug->brandName())
//                            .arg(pim->interactingAtcLabels().join(";"));

//                    QString &ditmp = riskLines[riskId];

//                    // no double
//                    if (!riskLines.values().contains(riskLine))
//                        ditmp += riskLine;
//                }

//                // add riskMasterLid to level
//                levelLines.insertMulti(level, riskId);
//            }

//            // construct full message
//            // for each level
//            QString levelLine;
//            QString riskLine;
//            //                int rows = 0;
//            foreach(const int level, levelLines.keys()) {
//                riskLine.clear();
//                levelLine = QString("<tr>\n"
//                                    "  <td colspan=2><b>%1</b></td>\n"
//                                    "</tr>\n")
//                        .arg(PimInteraction::levelToString(level).toUpper());
//                foreach(const int riskId, levelLines.values(level)) {
//                    if (riskLines.value(riskId).isEmpty())
//                        continue;
//                    riskLine = QString(
//                                "<tr>\n"
//                                "  <td width=\"100%\">%2</td>\n"
//                                "</tr>")
//                            .arg(riskLines.value(riskId));
//                }
//                if (riskLine.isEmpty())
//                    continue;
//                tmp += QString("<table border=\"0\" width=\"100%\">\n"
//                               "%1\n"
//                               "%2\n"
//                               "</table>\n<br />"
//                               )
//                        .arg(levelLine)
//                        .arg(riskLine);
//            }
//            break;
//        }
//        }
//        toReturn = tmp;
//        return toReturn;
//        return QString("MESSAGE");
        return QString();
    }

    QString message(const DrugInteractionInformationQuery &query) const
    {
        Q_UNUSED(query);
        qWarning() << Q_FUNC_INFO;
        if (!m_Result->testedDrugs().isEmpty())
            return QString();
        return QString();
    }

    // dynamic alert
    bool hasDynamicAlertWidget(const DrugInteractionInformationQuery &query) const
    {
        Q_UNUSED(query);
        // No Result ?
//        if (!query.result)
//            return false;
//        QVector<IDrugInteraction *> interactions;
//        if (query.relatedDrug) {
//            interactions = query.result->getInteractions(query.relatedDrug, Constants::ALLERGY_ENGINE_UID);
//        } else {
//            interactions = query.result->interactions(Constants::ALLERGY_ENGINE_UID);
//        }
//        // No interactions ?
//        if (interactions.isEmpty())
//            return false;
//        return true;
        return false;
    }

    QWidget *dynamicAlertWidget(const DrugInteractionInformationQuery &query, QWidget *parent = 0)
    {
        Q_UNUSED(query);
        Q_UNUSED(parent);
        return 0;
    }

    void setOverridden(bool overridden) {m_Overridden=overridden;}

    bool wasOverridden() const {return m_Overridden;}

private:
    bool m_Overridden;
    DrugInteractionResult *m_Result;
};

}

struct PimSource {
    int sourceId;
    QMultiHash<int, int> m_AtcIdsByPimId;
    QMultiHash<int, int> m_AtcIdsDoseRelatedByPimId;
    QMultiHash<int, int> m_AtcIdsIcdRelatedByPimId;
};

namespace DrugsDB {
namespace Internal {

class DrugAllergyEnginePrivate
{
public:
    QVector<IDrug *> m_TestedDrugs;
    QHash<int, PimSource> m_SourcesById;
//    QVector<IDrugInteraction *> m_Interactions;
//    QMap<int, int> m_InteractionsIDs;        /*!<  All possible interactions based on ATC IDs*/
//    QMultiMap<int, int> m_DDIFound;               /*!< modified by checkDrugInteraction() */
    QMultiHash<int, int> m_FoundPimIdsBySources;
    bool m_LogChrono;
};
}
}

DrugAllergyEngine::DrugAllergyEngine(QObject *parent) :
    IDrugAllergyEngine(parent),
    m_DrugPrecautionModel(0)
{
    setObjectName("DrugAllergyEngine");
}

DrugAllergyEngine::~DrugAllergyEngine()
{
}

bool DrugAllergyEngine::init()
{
    m_IsActive = settings()->value(Constants::S_ACTIVATED_INTERACTION_ENGINES).toStringList().contains(Constants::ALLERGY_ENGINE_UID);
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
    connect(patient(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(refreshDrugsPrecautions(QModelIndex,QModelIndex)));
    return true;
}

bool DrugAllergyEngine::isActive() const
{
    return settings()->value(Constants::S_ACTIVATED_INTERACTION_ENGINES).toStringList().contains(Constants::ALLERGY_ENGINE_UID);
}

QString DrugAllergyEngine::name() const
{
    return QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::ALLERGYENGINE_TEXT);
}

QString DrugAllergyEngine::shortName() const
{
    return QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::ALLERGY_TEXT);
}

QString DrugAllergyEngine::tooltip() const
{
    return tr("Detects allergies and intolerances to medications");
}

QString DrugAllergyEngine::uid() const
{
    return Constants::ALLERGY_ENGINE_UID;
}

QIcon DrugAllergyEngine::icon(const int size) const
{
    return theme()->icon(DrugsDB::Constants::I_ALLERGYENGINE, Core::ITheme::IconSize(size));
}

QString DrugAllergyEngine::iconFullPath(const int size) const
{
    return theme()->iconFullPath(DrugsDB::Constants::I_ALLERGYENGINE, Core::ITheme::IconSize(size));
}

int DrugAllergyEngine::calculateInteractions(const QVector<IDrug *> &drugs)
{
    Q_UNUSED(drugs);
    m_Interactions.clear();
//    for(int i = 0; i < drugs.count(); ++i) {
//        IDrug *drug = drugs.at(i);
//        const QString &uid = drug->drugId().toString();
//        if (m_ComputedInteractionCache.uniqueKeys().contains(uid)) {
//            QList<int> type = m_ComputedInteractionCache.values(uid);
//            if (type.contains(NoInteraction))
//                continue;
//            // create an AllergyInteraction
//            AllergyInteraction *allergy = new AllergyInteraction(this);
//            allergy->addInteractingDrug(drug);
//            m_Interactions << allergy;
//        } else {
//            check(Allergy, uid);
//            if (has(Allergy, uid)) {
//                QList<int> type = m_ComputedInteractionCache.values(uid);
//                if (type.contains(NoInteraction))
//                    continue;
//                AllergyInteraction *allergy = new AllergyInteraction(this);
//                allergy->addInteractingDrug(drug);
//                m_Interactions << allergy;
//            }
//        }
//    }
    return m_Interactions.count();
}

QVector<IDrugInteraction *> DrugAllergyEngine::getAllInteractionsFound()
{
    QVector<IDrugInteraction *> toReturn;

    return toReturn;
}

QVector<IDrugInteractionAlert *> DrugAllergyEngine::getAllAlerts(DrugInteractionResult *addToResult)
{
    QVector<IDrugInteractionAlert *> alerts;
    alerts << new AllergyAlert(addToResult, this);
    return alerts;
}

void DrugAllergyEngine::setActive(bool activate)
{
    if (isActive()==activate)
        return;
    // update settings
    if (activate) {
        settings()->appendToValue(Constants::S_ACTIVATED_INTERACTION_ENGINES, Constants::ALLERGY_ENGINE_UID);
    } else {
        QStringList l = settings()->value(Constants::S_ACTIVATED_INTERACTION_ENGINES).toStringList();
        l.removeAll(Constants::ALLERGY_ENGINE_UID);
        settings()->setValue(Constants::S_ACTIVATED_INTERACTION_ENGINES, l);
    }
}

void DrugAllergyEngine::clearDrugAllergyCache()
{
    m_ComputedInteractionCache.clear();
//    QHashIterator<QString, int> i(m_ComputedInteractionCache);
//    while (i.hasNext()) {
//        i.next();
//        if (i.value()==Allergy || i.value()==SuspectedAllergy) {
//            m_ComputedInteractionCache.remove(i.key());
//        }
//    }
    Q_EMIT allergiesUpdated();
}

void DrugAllergyEngine::clearDrugIntoleranceCache()
{
    m_ComputedInteractionCache.clear();
//    QHashIterator<QString, int> i(m_ComputedInteractionCache);
//    while (i.hasNext()) {
//        i.next();
//        if (i.value()==Intolerance || i.value()==SuspectedIntolerance)
//            m_ComputedInteractionCache.remove(i.key());
//    }
    Q_EMIT intolerancesUpdated();
}

/** Return true if the engine must compute interaction according to the \e typeOfInteraction and \e typeOfSubstrat. */
bool DrugAllergyEngine::needTest(const int typeOfInteraction, const int typeOfSubstrat) const
{
    for(int i=0; i<m_DoTests.count();++i) {
        const DrugAllergyEngineDoTest &t = m_DoTests.at(i);
        if (t.typeOfInteraction==typeOfInteraction && t.typeOfSubstrat==typeOfSubstrat)
            return true;
    }
    return false;
}

/** Return true if the engine found an interaction according to the params. */
bool DrugAllergyEngine::test(const int typeOfInteraction, const int typeOfSubstrat, const QString &drugUid, const QStringList &toTest)
{
    if (typeOfSubstrat!=Drug && toTest.isEmpty()) {
        return false;
    }

    for(int i = 0; i < m_Cache.count(); ++i) {
        // get the known interactions from the cache
        const DrugAllergyEngineCache &c = m_Cache.at(i);
        if (c.typeOfInteraction!=typeOfInteraction)
            continue;

        // get the interacting substrat list
        const QStringList &l = c.bySubstrat.values(typeOfSubstrat);
        if (l.isEmpty())
            continue;

        // test all strings (toTest)
        if (typeOfSubstrat==ClassInn) {
            foreach(const QString &test, toTest) {
                // check ATC Class
                foreach(const QString &s, l) {
                    if (test.startsWith(s, Qt::CaseInsensitive)) {
                        m_ComputedInteractionCache.insertMulti(drugUid, typeOfInteraction);
                        return true;
                    }
                }
            }
        } else if (typeOfSubstrat==InnCode) {
            foreach(const QString &test, toTest) {
                // exact match
                if (l.contains(test, Qt::CaseInsensitive)) {
                    m_ComputedInteractionCache.insertMulti(drugUid, typeOfInteraction);
                    return true;
                }
            }
        } else if (typeOfSubstrat==Drug) {
            if (l.contains(drugUid, Qt::CaseInsensitive)) {
                m_ComputedInteractionCache.insertMulti(drugUid, typeOfInteraction);
                return true;
            }
        }
    }
    return false;
}

/** Return the state of interaction according to the params. You MUST call check() before. */
bool DrugAllergyEngine::has(const int typeOfInteraction, const QString &uid, const int drugSource)
{
    Q_UNUSED(drugSource);
    if (m_ComputedInteractionCache.contains(uid)) {
        // return the most important value
        QList<int> types = m_ComputedInteractionCache.values(uid);
        if (types.contains(typeOfInteraction))
            return true;
    }

    return false;
}

/** Check the drug interaction status according to the params. */
void DrugAllergyEngine::check(const int typeOfInteraction, const QString &uid, const QString &drugGlobalAtcCode, const int drugSource)
{
    Q_UNUSED(drugSource);
    if (m_ComputedInteractionCache.contains(uid)) {
        return;
    }

    bool found = false;

    if (needTest(typeOfInteraction, InnCode)) {
        // get all drugs inns
        QStringList inns = drugsBase().getDrugCompositionAtcCodes(uid);
        inns << drugGlobalAtcCode;
        if (test(typeOfInteraction, InnCode, uid, inns))
            found = true;
    }

    if (needTest(typeOfInteraction, ClassInn)) {
        // get all molecules ATC codes
        QStringList atcs;
        atcs << drugsBase().getDrugCompositionAtcCodes(uid);
        atcs << drugGlobalAtcCode;
        atcs.removeAll("");
        if (test(typeOfInteraction, ClassInn, uid, atcs))
            found = true;
    }

    if (needTest(typeOfInteraction, Drug)) {
        if (test(typeOfInteraction, Drug, uid))
            found = true;
    }

    if (!found)
        m_ComputedInteractionCache.insert(uid, NoInteraction);

    if (m_ComputedInteractionCache.size() > 10000) {
        m_ComputedInteractionCache.remove(m_ComputedInteractionCache.begin().key());
    }
}

void DrugAllergyEngine::onCurrentPatientChanged()
{
    m_Interactions.clear();
    m_DoTests.clear();
    m_Cache.clear();
    m_ComputedInteractionCache.clear();
    m_ProcessedUid.clear();
    int currentRow = patient()->currentPatientIndex().row();
    refreshDrugsPrecautions(patient()->index(currentRow, 0), patient()->index(currentRow, patient()->columnCount()));
}


/** When patient physiology changes or when the current patient changed, this slots refreshes the patient physiology (allergies, intolerances) */
void DrugAllergyEngine::refreshDrugsPrecautions(const QModelIndex &topleft, const QModelIndex &bottomright)
{
//    qWarning() << Q_FUNC_INFO << Core::IPatient::DrugsAllergiesWithoutPrecision;
//    bool refreshModel = false;
//    int ref = topleft.column();
    if (Utils::inRange(topleft.column(), bottomright.column(), Core::IPatient::DrugsUidAllergies)) {
//        qWarning() << "DrugAllergyEngine::refreshDrugsPrecautions  Core::IPatient::DrugsUidAllergies";
//        d->uidAllergies = patient()->data(Core::IPatient::DrugsUidAllergies).toStringList();
//        d->uidAllergies.removeAll("");
//        d->m_testUidAllergies = !d->uidAllergies.isEmpty();
//        d->clearm_ComputedInteractionCache();
//        refreshModel = true;

        // clear old testing values and emit allergiesUpdated
        clearDrugAllergyCache();
    }

    if (Utils::inRange(topleft.column(), bottomright.column(), Core::IPatient::DrugsInnAllergies)) {
//    if (ref == Core::IPatient::DrugsInnAllergies) {
//        qWarning() << "DrugAllergyEngine::refreshDrugsPrecautions  Core::IPatient::DrugsInnAllergies";
//        qWarning() << patient()->data(Core::IPatient::DrugsInnAllergies).toStringList();
//        d->innAllergies = patient()->data(Core::IPatient::DrugsInnAllergies).toStringList();
//        d->innAllergies.removeAll("");
//        d->m_testInnAllergies = !d->innAllergies.isEmpty();
//        d->clearm_ComputedInteractionCache();
////        qWarning() << "  -----> DrugsInnAllergies" << d->innAllergies;
//        refreshModel = true;

        // clear old testing values and emit allergiesUpdated
        clearDrugAllergyCache();
    }

    if (Utils::inRange(topleft.column(), bottomright.column(), Core::IPatient::DrugsAtcAllergies)) {
        qWarning() << "DrugAllergyEngine::refreshDrugsPrecautions  Core::IPatient::DrugsAtcAllergies";
        qWarning() << patient()->data(Core::IPatient::DrugsAtcAllergies).toStringList();

        // add the test state
        if (!needTest(Allergy, InnCode)) {
            DrugAllergyEngineDoTest test;
            test.typeOfInteraction = Allergy;
            test.typeOfSubstrat = InnCode;
            m_DoTests.append(test);
        }

        // add the testing values
        bool updated = false;
        for(int i = 0; i < m_Cache.count(); ++i) {
            DrugAllergyEngineCache &c = m_Cache[i];
            if (c.typeOfInteraction==Allergy) {
                c.bySubstrat.clear();
                foreach(const QString &s, patient()->data(Core::IPatient::DrugsAtcAllergies).toStringList()) {
                    c.bySubstrat.insertMulti(InnCode, s);
                }
                updated = true;
                break;
            }
        }

        // if needed -> create the Cache
        if (!updated) {
            DrugAllergyEngineCache c;
            c.typeOfInteraction = Allergy;
            foreach(const QString &s, patient()->data(Core::IPatient::DrugsAtcAllergies).toStringList()) {
                c.bySubstrat.insertMulti(InnCode, s);
            }
            m_Cache.append(c);
        }

//            if (atc.length()==7)
//                d->fullAtcAllergies.append(atc);
//            else
//                d->classAtcAllergies.append(atc);

        // clear old testing values and emit allergiesUpdated
        clearDrugAllergyCache();
    }

    //    if (ref == Core::IPatient::DrugsAtcIntolerances) {
//        qWarning() << "DrugAllergyEngine::refreshDrugsPrecautions  Core::IPatient::DrugsAtcIntolerances";
//        d->fullAtcIntolerances.clear();
//        d->classAtcIntolerances.clear();
//        foreach(const QString &atc, patient()->data(Core::IPatient::DrugsAtcIntolerances).toStringList()) {
//            if (atc.length()==7)
//                d->fullAtcIntolerances.append(atc);
//            else
//                d->classAtcIntolerances.append(atc);
//        }
//        d->fullAtcIntolerances.removeAll("");
//        d->classAtcIntolerances.removeAll("");
//        d->m_testAtcIntolerances = !d->fullAtcIntolerances.isEmpty() || !d->classAtcIntolerances.isEmpty();
//        d->clearDrugIntoleranceCache();
//        refreshModel = true;
//    }
//    if (ref == Core::IPatient::DrugsInnIntolerances) {
//        d->innIntolerances = patient()->data(Core::IPatient::DrugsInnIntolerances).toStringList();
//        d->innIntolerances.removeAll("");
//        d->m_testInnIntolerances = !d->innIntolerances.isEmpty();
//        d->clearDrugIntoleranceCache();
//        refreshModel = true;
//    }
//    if (ref == Core::IPatient::DrugsUidIntolerances) {
//        d->uidIntolerances = patient()->data(Core::IPatient::DrugsUidIntolerances).toStringList();
//        d->uidIntolerances.removeAll("");
//        d->m_testUidIntolerances = !d->uidIntolerances.isEmpty();
//        d->clearDrugIntoleranceCache();
//        refreshModel = true;
//    }
//    d->m_ProcessedUid.clear();
//    if (refreshModel || !d->m_DrugsPrecautionsModel)
//        d->updateDrugsPrecautionsModel();
//    reset();
}

static void addBranch(QStandardItem *rootAllergies, QStandardItem *rootIntolerances, const QString &name,
                      const QStringList &allergies, const QStringList &intolerances, bool atc, bool uids,
                      const QBrush &allergiesBrush, const QBrush &intolerancesBrush)
{
    QStandardItem *allergiesItem = new QStandardItem(name);
    QStandardItem *intolerancesItem = new QStandardItem(name);
    allergiesItem->setForeground(allergiesBrush);
    intolerancesItem->setForeground(intolerancesBrush);
    rootAllergies->appendRow(allergiesItem);
    rootIntolerances->appendRow(intolerancesItem);
    //    allergies
    QStringList sorted = allergies;
    qSort(sorted);
    foreach(const QString &item, sorted) {
        QString lbl;
        if (atc)
             lbl = drugsBase().getAtcLabel(item);
        else if (uids)
             lbl = drugsBase().getDrugName(item);
        else
            lbl = item;
        if (!lbl.isEmpty()) {
            QStandardItem *i = new QStandardItem(lbl);
            i->setForeground(allergiesBrush);
            i->setToolTip(tkTr(Trans::Constants::ALLERGY_TO_1).arg(lbl));
            allergiesItem->appendRow(i);
        }
    }
    //    intolerances
    sorted.clear();
    sorted = intolerances;
    qSort(sorted);
    foreach(const QString &item, sorted) {
        QString lbl;
        if (atc)
             lbl = drugsBase().getAtcLabel(item);
        else if (uids)
             lbl = drugsBase().getDrugName(item);
        else
            lbl = item;
        if (!lbl.isEmpty()) {
            QStandardItem *i = new QStandardItem(lbl);
            i->setToolTip(tkTr(Trans::Constants::INTOLERANCE_TO_1).arg(lbl));
            i->setForeground(intolerancesBrush);
            intolerancesItem->appendRow(i);
        }
    }
}

void DrugAllergyEngine::updateDrugsPrecautionsModel() const
{
    if (!m_DrugPrecautionModel)
        m_DrugPrecautionModel = new QStandardItemModel((DrugAllergyEngine*)this);
    m_DrugPrecautionModel->clear();
    QFont bold;
    bold.setBold(true);
    QStandardItem *rootItem = m_DrugPrecautionModel->invisibleRootItem();

    if (patient()->data(Core::IPatient::DrugsAllergiesWithoutPrecision).isNull() &&
            patient()->data(Core::IPatient::DrugsUidAllergies).isNull() &&
            patient()->data(Core::IPatient::DrugsAtcAllergies).isNull() &&
            patient()->data(Core::IPatient::DrugsInnAllergies).isNull() &&
            patient()->data(Core::IPatient::DrugsIntolerancesWithoutPrecision).isNull() &&
            patient()->data(Core::IPatient::DrugsUidIntolerances).isNull() &&
            patient()->data(Core::IPatient::DrugsAtcIntolerances).isNull() &&
            patient()->data(Core::IPatient::DrugsInnIntolerances).isNull() &&
            patient()->data(Core::IPatient::DrugsInnAtcIntolerances).isNull()) {
        QStandardItem *uniqueItem = new QStandardItem(tkTr(Trans::Constants::NO_ALLERGIES_INTOLERANCES));
        uniqueItem->setFont(bold);
        rootItem->appendRow(uniqueItem);
    } else {
        QStandardItem *allergiesItem = new QStandardItem(tkTr(Trans::Constants::KNOWN_ALLERGIES));
        QStandardItem *intolerancesItem = new QStandardItem(tkTr(Trans::Constants::KNOWN_INTOLERANCES));
        allergiesItem->setFont(bold);
        intolerancesItem->setFont(bold);
        QBrush allergiesBrush = QBrush(QColor(settings()->value(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR).toString()).darker(300));
        QBrush intolerancesBrush = QBrush(QColor(settings()->value(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR).toString()).darker(300));
        allergiesItem->setForeground(allergiesBrush);
        intolerancesItem->setForeground(intolerancesBrush);

        addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::ATC), patient()->data(Core::IPatient::DrugsAtcAllergies).toStringList(), patient()->data(Core::IPatient::DrugsAtcIntolerances).toStringList(), true, false, allergiesBrush, intolerancesBrush);
        addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::DRUGS), patient()->data(Core::IPatient::DrugsUidAllergies).toStringList(), patient()->data(Core::IPatient::DrugsUidIntolerances).toStringList(), false, true, allergiesBrush, intolerancesBrush);
        addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::INN), patient()->data(Core::IPatient::DrugsInnAllergies).toStringList(), patient()->data(Core::IPatient::DrugsInnIntolerances).toStringList(), false, false, allergiesBrush, intolerancesBrush);

        rootItem->appendRow(allergiesItem);
        rootItem->appendRow(intolerancesItem);
    }
}

QStandardItemModel *DrugAllergyEngine::drugPrecautionModel() const
{
    updateDrugsPrecautionsModel();
    return m_DrugPrecautionModel;
}


