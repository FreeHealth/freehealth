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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pimengine.h"

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

struct PimAtcRelated {
    PimAtcRelated(const int atc_id, const double dose = 0, const QString &doseUnit = QString::null) :
            atcId(atc_id), maxDailyDose(dose), maxDailyDoseUnit(doseUnit) {}

    PimAtcRelated() : atcId(-1), maxDailyDose(-1), maxDailyDoseUnit(QString::null) {}

    int atcId;
    double maxDailyDose;
    QString maxDailyDoseUnit;
};

class PimInteraction : public IDrugInteraction {
public:
    enum DataRepresentation {
        PIM_ID,
        PIM_SID,
        PIM_TID,
        PIM_SourceName,
        PIM_TypeMasterLid,
        PIM_TypeName,
        PIM_Label,
        PIM_Level,
        PIM_RiskMasterLid,
        PIM_ReferencesLink,
        PIM_CountryRelated,
        PIM_PubMed_PMID
    };

    enum InteractionLevel {
        InteractionLevel_High = 3,
        InteractionLevel_Medium = 2,
        InteractionLevel_Low = 1
    };

    PimInteraction(IDrugEngine *parentEngine) : IDrugInteraction(parentEngine), m_Engine(parentEngine) {}
    ~PimInteraction() {}

    void setValue(const int ref, const QVariant &value)
    {
        m_Infos.insert(ref, value);
    }

    QVariant value(const int ref) const
    {
        if (ref==PIM_TypeName) {
            QString l = QLocale().name().left(2).toLower();
            if (l=="fr")
                return drugsBase().getLabel(m_Infos.value(PIM_TypeMasterLid).toInt(), l);
            else
                return drugsBase().getLabel(m_Infos.value(PIM_TypeMasterLid).toInt(), "en");
        }
        return m_Infos.value(ref);
    }

    // IDrugInteraction interface
    IDrugEngine *engine() const {return m_Engine;}

    bool isDrugDrugInteraction() const {return false;}
    bool isPotentiallyInappropriate() const {return true;}

    QString type() const {return levelToString(this);}

    QList<IDrug *> drugs() const {return m_InteractingDrugs;}

    QIcon icon(const int levelOfWarning, const int iconsize) const
    {
        Q_UNUSED(levelOfWarning);
        return theme()->icon(Constants::I_PIMENGINE, Core::ITheme::IconSize(iconsize));
    }

    QString header(const QString &separator = QString::null) const
    {
        // for non ICD10 related
        return QString("%1 %2")
                .arg(interactingAtcLabels().join(";"))
                .arg(separator);
    }

    QString risk(const QString &lang = QString::null) const
    {
        QString l = lang;
        if (l.isEmpty())
            l = QLocale().name().left(2);
        QString r;
//        if (l=="fr")
//            r = drugsBase().getLabel(m_Infos.value(PIM_RiskMasterLid).toInt(), l);
//        else
            r = drugsBase().getLabel(m_Infos.value(PIM_RiskMasterLid).toInt(), "en");
        return r.replace("<br />", "<br>");
    }

    QString management(const QString &lang = QString::null) const
    {
        Q_UNUSED(lang);
//        QString l = lang;
//        if (l.isEmpty())
//            l = QLocale().name().left(2);
//        QString r;
//        if (l=="fr")
//            r = drugsBase().getLabel(m_Infos.value(DI_ManagementId).toInt(), l);
//        else
//            r = drugsBase().getLabel(m_Infos.value(DI_ManagementId).toInt(), "en");
//        return r.replace("<br />", "<br>");
        return QString();
    }

    QString referencesLink(const QString &lang = QString::null) const
    {
        Q_UNUSED(lang);
        return m_Infos.value(PIM_ReferencesLink).toString();
    }

    QString toHtml(bool detailled = false) const
    {
        Q_UNUSED(detailled);
//        if (m_InteractingDrugs.count() != 2)
            return QString();
//        // Link Atc_Ids with interacting drug
//        int firstDrugAtcId = 0;
//        int secondDrugAtcId = 0;
//        if (m_InteractingDrugs.at(0)->atcIdsContains(m_Infos.value(DI_ATC1).toInt())) {
//            firstDrugAtcId = m_Infos.value(DI_ATC1).toInt();
//            secondDrugAtcId = m_Infos.value(DI_ATC2).toInt();
//        } else {
//            secondDrugAtcId= m_Infos.value(DI_ATC1).toInt();
//            firstDrugAtcId = m_Infos.value(DI_ATC2).toInt();
//        }
//        // Create the Html
//        QString tmp = QString(LIST_BASIC_INFO)
//                      .arg(type())
//                      .arg(drugs().at(0)->brandName().replace(" ", "&nbsp;"))
//                      .arg(drugsBase().getAtcLabel(firstDrugAtcId).replace(" ", "&nbsp;"))
//                      .arg(drugs().at(1)->brandName().replace(" ", "&nbsp;"))
//                      .arg(drugsBase().getAtcLabel(secondDrugAtcId).replace(" ", "&nbsp;"));
//        if (detailled) {
//            QString r = risk();
//            if (!r.isEmpty()) {
//                tmp += QString("<tr>\n"
//                               "  <td><b>%1</b></td>\n"
//                               "  <td>%2</td>\n"
//                               "</tr>\n"
//                               )
//                        .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::NATURE_OF_RISK))
//                        .arg(r
//                             .replace("<br>", " ")
//                             .replace("<", "&lt;")
//                             .replace(">", "&gt;"));
//            }
//            r = management();
//            if (!r.isEmpty()) {
//                tmp += QString("<tr>\n"
//                               "  <td><b>%1</b></td>\n"
//                               "  <td>%2</td>\n"
//                               "</tr>\n"
//                               )
//                        .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::MANAGEMENT))
//                        .arg(r
//                             .replace("<br>", "__")
//                             .replace("<", "&lt;")
//                             .replace(">", "&gt;")
//                             .replace("__", "<br>"));
//            }
//        }
//        tmp.prepend("<table width=100% border=1>");
//        tmp.append("</table>");
//        return tmp;
    }

    int sortIndex() const {return m_Infos.value(PIM_Level).toInt();}

    void addInteractingDrug(IDrug *drug) {m_InteractingDrugs << drug;}

    void addIcdSid(const QList<QVariant> &icdSids)
    {
        m_IcdSids << icdSids;
    }

    void addAtcRelated(const QVector<PimAtcRelated> &atcRelated)
    {
        m_AtcRelated << atcRelated;
    }

    void addIcdSid(const QVariant &icdSid)
    {
        m_IcdSids << icdSid;
    }

    void addAtcRelated(const PimAtcRelated &atcRelated)
    {
        m_AtcRelated << atcRelated;
    }

    QVector<int> allAtcRelated() const
    {
        QVector<int> ids;
        foreach(const PimAtcRelated &atc, m_AtcRelated)
            ids << atc.atcId;
        return ids;
    }

    QVector<int> interactingAtcIds() const
    {
        QVector<int> toReturn;
        for(int i = 0; i < m_AtcRelated.count(); ++i) {
            int atcId = m_AtcRelated.at(i).atcId;
            foreach(IDrug *drug, m_InteractingDrugs) {
                if (drug->allInnAndInteractingClassesIds().contains(atcId)) {
                    toReturn << atcId;
                }
            }
        }
        return toReturn;
    }

    QStringList interactingAtcLabels() const
    {
        const QVector<int> &ids = interactingAtcIds();
        QStringList t;
        for(int i = 0; i < ids.count(); ++i) {
            t << drugsBase().getAtcLabel(ids.at(i));
        }
        return t;
    }

    void warn() const
    {
        QString tmp = QString("PimInteraction:\n"
                              "    (ID: %1; SID: %2; TID: %3)\n"
                              "    (SourceName: %4; TypeName: %5; TypeMastrLid: %10)\n"
                              "    (Level:%6; Country: %7; PMID: %8; WWW: %9)"
                              )
                .arg(m_Infos.value(PIM_ID).toString())
                .arg(m_Infos.value(PIM_SID).toString())
                .arg(m_Infos.value(PIM_TID).toString())
                .arg(m_Infos.value(PIM_SourceName).toString())
                .arg(value(PIM_TypeName).toString())
                .arg(m_Infos.value(PIM_Level).toString())
                .arg(m_Infos.value(PIM_CountryRelated).toString())
                .arg(m_Infos.value(PIM_PubMed_PMID).toString())
                .arg(m_Infos.value(PIM_ReferencesLink).toString())
                .arg(m_Infos.value(PIM_TypeMasterLid).toInt())
                ;
        for(int i = 0; i < m_InteractingDrugs.count(); ++i) {
            tmp += "\n    (Drug:" + m_InteractingDrugs.at(i)->brandName() + ")";
        }
        for(int i = 0; i < m_AtcRelated.count(); ++i) {
            int atcId = m_AtcRelated.at(i).atcId;
            if (m_AtcRelated.at(i).maxDailyDose > 0) {
                tmp += QString("\n    (AtcId:%1(%2); %3%4")
                       .arg(drugsBase().getAtcLabel(atcId))
                       .arg(atcId)
                       .arg(m_AtcRelated.at(i).maxDailyDose)
                       .arg(m_AtcRelated.at(i).maxDailyDoseUnit);
            } else {
                tmp += QString("\n    (AtcId:%1(%2); All dosages)")
                       .arg(drugsBase().getAtcLabel(atcId)).arg(atcId);
            }
        }
        for(int i = 0; i < m_IcdSids.count(); ++i) {
            tmp += "\n    (ICD:" + m_IcdSids.at(i).toString() + ")";
        }
        qWarning() << tmp;
    }

    static QString levelToString(const int level)
    {
        switch (level) {
        case InteractionLevel_High: return tkTr(Trans::Constants::HIGH);
        case InteractionLevel_Medium: return tkTr(Trans::Constants::MEDIUM);
        case InteractionLevel_Low: return tkTr(Trans::Constants::LOW);
        }
        return QString();
    }

    static QString levelToString(const PimInteraction *pim)
    {
        return levelToString(pim->value(PimInteraction::PIM_Level).toInt());
    }


private:
    IDrugEngine *m_Engine;
    QHash<int, QVariant> m_Infos;
    QList<IDrug *> m_InteractingDrugs;
    QList<QVariant> m_IcdSids;
    QVector<PimAtcRelated> m_AtcRelated;
//    QVector<PimIcdRelated> m_IcdRelated;
};


class Alert : public IDrugInteractionAlert
{
public:
    Alert(DrugInteractionResult *result, IDrugEngine *engine) :
            IDrugInteractionAlert(engine), m_Overridden(false), m_Result(result)
    {
    }

    ~Alert() {}

    QString engineUid() const {return Constants::PIM_ENGINE_UID;}

    // static alert
    QIcon icon(const IDrug *drug, const DrugInteractionInformationQuery &query) const
    {
        if (!m_Result->testedDrugs().contains((IDrug*)drug))
            return QIcon();
        if (!query.engineUid.isEmpty() && query.engineUid!=Constants::PIM_ENGINE_UID) {
            return QIcon();
        }
//        int level = getMaximumTypeOfIAM(m_Result->interactions(), drug);
        Core::ITheme *th = theme();
        Core::ITheme::IconSize size = Core::ITheme::IconSize(query.iconSize);
        return th->icon(Constants::I_PIMENGINE, size);
    }

    QString message(const IDrug *drug, const DrugInteractionInformationQuery &query) const
    {
        QString toReturn;
        if (!m_Result->testedDrugs().contains((IDrug*)drug))
            return toReturn;

        // get all interactions related to the drug
        QVector<IDrugInteraction *> interactions = m_Result->getInteractions(drug, Constants::PIM_ENGINE_UID);

        QString tmp;
        switch (query.messageType)
        {
        case DrugInteractionInformationQuery::DetailledToolTip:
        {
            qWarning() << "****** DetailledTooltip";

            // get all different types
            QStringList types;
            for(int j=0; j < interactions.count(); ++j) {
                IDrugInteraction *di = interactions.at(j);
                PimInteraction *pim = static_cast<PimInteraction *>(di);
                Q_ASSERT(pim);
                QString t = pim->value(PimInteraction::PIM_TypeName).toString();
                if (!types.contains(t))
                    types << t;
            }

            // foreach types construct the html output
            foreach(const QString &type, types) {
                QMap<int, QString> lines; // K=level - V=html
                QVector<int> pimIds;
                for(int j=0; j < interactions.count(); ++j) {
                    IDrugInteraction *di = interactions.at(j);
                    PimInteraction *pim = static_cast<PimInteraction *>(di);
                    Q_ASSERT(pim);
                    if (pim->value(PimInteraction::PIM_TypeName).toString()!=type)
                        continue;

                    if (pimIds.contains(pim->value(PimInteraction::PIM_ID).toInt()))
                        continue;
                    pimIds << pim->value(PimInteraction::PIM_ID).toInt();

                    int level = pim->value(PimInteraction::PIM_Level).toInt();
                    int typeId = -1;

                    // Minimal alerts (only High levels)
                    if (level==PimInteraction::InteractionLevel_High && (query.levelOfWarningStaticAlert <= Constants::MinimumLevelOfWarning))
                        typeId = PimInteraction::InteractionLevel_High;
                    // Moderate alerts (medium alerts)
                    else if (level==PimInteraction::InteractionLevel_Medium && (query.levelOfWarningStaticAlert <= Constants::ModerateLevelOfWarning))
                        typeId = PimInteraction::InteractionLevel_Medium;
                    // Maximum alerts (all alerts)
                    else if (level==PimInteraction::InteractionLevel_Low && (query.levelOfWarningStaticAlert <= Constants::MaximumLevelOfWarning))
                        typeId = PimInteraction::InteractionLevel_Low;

                    if (typeId==-1)
                        continue;

//                    pim->warn();

                    QString &ditmp = lines[typeId];
                    if (!ditmp.contains(pim->header())) {
                        ditmp += QString("    <li>%1</li>\n")
                                .arg(pim->header());
                    }
                }

                qWarning() << type << lines;

                if (lines.isEmpty())
                    continue;

                QString html;
                QMap<int, QString>::const_iterator i = lines.constEnd();
                --i;
                while (true) {
                    if (!i.value().isEmpty()) {
                        html += QString("<ul compact>"
                                       "  <li><b>%1</b></li>\n"
                                       "  <ul>\n"
                                       "%2"
                                       "  </ul>\n"
                                       "</ul>\n"
                                       )
                                .arg(PimInteraction::levelToString(i.key()))
                                .arg(i.value());
                    }
                    if (i==lines.constBegin())
                        break;
                    --i;
                }
                tmp += QString("<tr><td align=center width=100%><b>%1</b></td></tr><tr><td>%2</td></tr>")
                        .arg(type)
                        .arg(html);
            }
            if (tmp.isEmpty())
                return QString();

            tmp = QString("<br /><table widht=100% border=1>%1</table>")
                    .arg(tmp);

            break;
        }
        case DrugInteractionInformationQuery::ShortToolTip:
        {
            qWarning() << "****** ShortToolTip";
            for(int j=0; j < interactions.count(); ++j) {
                IDrugInteraction *di = interactions.at(j);
                PimInteraction *pim = static_cast<PimInteraction *>(di);
                if (!pim)
                    continue;
                tmp += "-&nbsp;" + pim->interactingAtcLabels().join(";") + "<br />";
            }
            if (!tmp.isEmpty()) {
                tmp.chop(6);
                toReturn.append(QString(LIST_MASK)
                                .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::PIMENGINE_TEXT))
                                .arg(tmp));
            }
            break;
        }
        case DrugInteractionInformationQuery::InformationAlert:
        {
            qWarning() << "****** InformationAlert";
            QHash<int, QString> riskLines; //k=riskMasterLid, line
            QMultiHash<int, int> levelLines; //k=level, v=riskMasterLid
            QVector<int> interactionAtcIds;
            QVector<int> interactionIds;
            for(int j=0; j < interactions.count(); ++j) {
                // Get interaction
                IDrugInteraction *di = interactions.at(j);
                PimInteraction *pim = static_cast<PimInteraction *>(di);
                Q_ASSERT(pim);
                if (!pim)
                    continue;

                // interaction with this interactor already included ?
                const QVector<int> &pimRelAtc = pim->allAtcRelated();
                bool includeIt = false;
                for(int aa = 0; aa < pimRelAtc.count(); ++aa) {
                    int testAtc = pimRelAtc.at(aa);
                    int interactorAtcId = -1;
                    if (drug->allInnAndInteractingClassesIds().contains(testAtc)) {
                        interactorAtcId = testAtc;
                    }
                    if (interactionAtcIds.contains(interactorAtcId))
                        continue;
                    else
                        includeIt = true;
                    interactionAtcIds << interactorAtcId;
                }
                if (!includeIt)
                    continue;

                // interaction already included ?
                if (interactionIds.contains(pim->value(PimInteraction::PIM_ID).toInt()))
                    continue;
                interactionIds << pim->value(PimInteraction::PIM_ID).toInt();

                pim->warn();

                // get the line related to the level of pim
                //                    int typeId = pim->value(PimInteraction::PIM_Level).toInt();

                // TODO: manage levelOfWarning

                int riskId = pim->value(PimInteraction::PIM_RiskMasterLid).toInt();
                int level =  pim->value(PimInteraction::PIM_Level).toInt();
                // construct line of risk content
                QString riskLine;
                foreach(IDrug *drug, pim->drugs()) {
                    // FIXME: use tkTr(Trans::Constants::_1_COLON_2) "%1: %2" translation
                    riskLine = QString("&nbsp;&nbsp;*&nbsp;%1: %2<br />\n")
                            .arg(drug->brandName())
                            .arg(pim->interactingAtcLabels().join(";"));

                    QString &ditmp = riskLines[riskId];

                    // no double
                    if (!riskLines.values().contains(riskLine))
                        ditmp += riskLine;
                }

                // add riskMasterLid to level
                levelLines.insertMulti(level, riskId);
            }

            // construct full message
            // for each level
            QString levelLine;
            QString riskLine;
            //                int rows = 0;
            foreach(const int level, levelLines.keys()) {
                riskLine.clear();
                levelLine = QString("<tr>\n"
                                    "  <td colspan=2><b>%1</b></td>\n"
                                    "</tr>\n")
                        .arg(PimInteraction::levelToString(level).toUpper());
                foreach(const int riskId, levelLines.values(level)) {
                    if (riskLines.value(riskId).isEmpty())
                        continue;
                    riskLine = QString(
                                "<tr>\n"
                                "  <td width=\"100%\">%2</td>\n"
                                "</tr>")
                            .arg(riskLines.value(riskId));
                }
                if (riskLine.isEmpty())
                    continue;
                tmp += QString("<table border=\"0\" width=\"100%\">\n"
                               "%1\n"
                               "%2\n"
                               "</table>\n<br />"
                               )
                        .arg(levelLine)
                        .arg(riskLine);
            }
            break;
        }
        }
        toReturn = tmp;
        return toReturn;
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
        // No Result ?
        if (!query.result)
            return false;
        QVector<IDrugInteraction *> interactions;
        if (query.relatedDrug) {
            interactions = query.result->getInteractions(query.relatedDrug, Constants::PIM_ENGINE_UID);
        } else {
            interactions = query.result->interactions(Constants::PIM_ENGINE_UID);
        }
        // No interactions ?
        if (interactions.isEmpty())
            return false;
        return true;
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

namespace DrugInteractions {
namespace Internal {

class PimEnginePrivate
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

PimEngine::PimEngine(QObject *parent) :
        IDrugEngine(parent),
        d(new Internal::PimEnginePrivate)
{
    setObjectName("PimEngine");
    m_IsActive = settings()->value(Constants::S_ACTIVATED_INTERACTION_ENGINES).toStringList().contains(Constants::PIM_ENGINE_UID);
    connect(&drugsBase(), SIGNAL(drugsBaseHasChanged()), SLOT(drugsBaseChanged()));
}

PimEngine::~PimEngine()
{
    if (d)
        delete d;
    d = 0;
}

void PimEngine::drugsBaseChanged()
{
    init();
}

bool PimEngine::init()
{
    // cache all sources atcIds
    d->m_SourcesById.clear();
    QString req;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR("Unable to open drugs database");
            return false;
        }
    }

    // query all sources ID
    req = drugsBase().select(Constants::Table_PIM_SOURCES, Constants::PIM_SOURCES_SID);
    QSqlQuery source(DB);
    if (source.exec(req)) {
        while (source.next()) {
            PimSource pimSource;
            pimSource.sourceId = source.value(0).toInt();
            // get all related ATC_ID
            Utils::FieldList get;
            Utils::JoinList join;
            Utils::FieldList conds;
            get << Utils::Field(Constants::Table_PIMS_RELATED_ATC, Constants::PIMS_RELATC_PIM_ID);
            get << Utils::Field(Constants::Table_PIMS_RELATED_ATC, Constants::PIMS_RELATC_ATC_ID);
            get << Utils::Field(Constants::Table_PIMS_RELATED_ATC, Constants::PIMS_RELATC_MAXDAYDOSE);
            join << Utils::Join(Constants::Table_PIMS, Constants::PIMS_ID, Constants::Table_PIMS_RELATED_ATC, Constants::PIMS_RELATC_PIM_ID);
            conds << Utils::Field(Constants::Table_PIMS, Constants::PIMS_SID, QString("=%1").arg(pimSource.sourceId));
            req = drugsBase().select(get, join, conds);
            QSqlQuery atc(DB);
            if (atc.exec(req)) {
                while (atc.next()) {
                    if (atc.value(2).toString().isEmpty()) {
                        pimSource.m_AtcIdsByPimId.insert(atc.value(0).toInt(), atc.value(1).toInt());
                    } else {
                        pimSource.m_AtcIdsDoseRelatedByPimId.insert(atc.value(0).toInt(), atc.value(1).toInt());
                    }
                }
            } else {
                LOG_QUERY_ERROR(atc);
            }

            // get all related ICD_SID
            get.clear();
            join.clear();
            get << Utils::Field(Constants::Table_PIMS_RELATED_ICD, Constants::PIMS_RELICD_PIM_ID);
            get << Utils::Field(Constants::Table_PIMS_RELATED_ICD, Constants::PIMS_RELICD_ICD_SID);
            join << Utils::Join(Constants::Table_PIMS, Constants::PIMS_ID, Constants::Table_PIMS_RELATED_ICD, Constants::PIMS_RELICD_PIM_ID);
            req = drugsBase().select(get, join, conds);
            if (atc.exec(req)) {
                while (atc.next()) {
                    pimSource.m_AtcIdsIcdRelatedByPimId.insert(atc.value(0).toInt(), atc.value(1).toInt());
                }
            } else {
                LOG_QUERY_ERROR(atc);
            }

            d->m_SourcesById.insert(pimSource.sourceId, pimSource);
        }
    }

//    qWarning() << "non dose related" << d->m_AtcIdsPimBySources;
//    qWarning() << "dose related" << d->m_AtcIdsPimDoseRelatedBySources;

    return true;
}

bool PimEngine::isActive() const
{
    return settings()->value(Constants::S_ACTIVATED_INTERACTION_ENGINES).toStringList().contains(Constants::PIM_ENGINE_UID);
}

bool PimEngine::canComputeInteractions() const
{
    return (patient()->data(Core::IPatient::YearsOld).toInt() >= 75);
}

QString PimEngine::name() const
{
    return QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::PIMENGINE_TEXT) + " " + tr("(Experimental)");
}

QString PimEngine::shortName() const
{
    return QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::PIMENGINE_SHORT_TEXT);
}

QString PimEngine::tooltip() const
{ // TODO: this is not a real good English sentence - not translatable neither...
    return tr("Detects interaction between drugs and patient in elderly");
}

QString PimEngine::uid() const
{
    return Constants::PIM_ENGINE_UID;
}

QIcon PimEngine::icon(const int size) const
{
    return theme()->icon(Constants::I_PIMENGINE, Core::ITheme::IconSize(size));
}

QString PimEngine::iconFullPath(const int size) const
{
    return theme()->iconFullPath(Constants::I_PIMENGINE, Core::ITheme::IconSize(size));
}

int PimEngine::calculateInteractions(const QVector<IDrug *> &drugs)
{
    d->m_FoundPimIdsBySources.clear();
    if (!canComputeInteractions()) {
        return 0;
    }
    int nbPimNonDoseRelated = 0;
    int nbPimDoseRelated = 0;
    d->m_TestedDrugs = drugs;

    // For all drugs, make ATC_ID / ICD_SID tests
    for(int i = 0; i < drugs.count(); ++i) {
        IDrug *drug = drugs.at(i);
        // foreach pim sources
        foreach(const PimSource &source, d->m_SourcesById.values()) {

            // test all pims atcId non dose dependant
            const QList<int> &atcIds = source.m_AtcIdsByPimId.values();
            for(int j = 0; j < atcIds.count(); ++j) {
                int atcId = atcIds.at(j);
                if (drug->allInnAndInteractingClassesIds().contains(atcId)) {
                    foreach(const int pimId, source.m_AtcIdsByPimId.keys(atcId)) {
                        ++nbPimNonDoseRelated;
                        d->m_FoundPimIdsBySources.insertMulti(source.sourceId, pimId);
                    }
                }
            }

            // test all pims atcId dose dependant
            const QList<int> &atcIdsDoseDep = source.m_AtcIdsDoseRelatedByPimId.values();
            for(int j = 0; j < atcIdsDoseDep.count(); ++j) {
                int atcId = atcIdsDoseDep.at(j);
                if (drug->allInnAndInteractingClassesIds().contains(atcId)) {
                    foreach(const int pimId, source.m_AtcIdsDoseRelatedByPimId.keys(atcId)) {
                        ++nbPimDoseRelated;
                        d->m_FoundPimIdsBySources.insertMulti(source.sourceId, pimId);
                    }
                }
            }
        }

//        foreach(const int source, d->m_AtcIdsPimDoseRelatedBySources.uniqueKeys()) {
//            const QList<int> &atcIds = d->m_AtcIdsPimDoseRelatedBySources.values(source);
//            for(int j = 0; j < atcIds.count(); ++j) {
//                if (drug->allInnAndInteractingClassesIds().contains(atcIds.at(j))) {
//                    ++nbPimDoseRelated;
//                    d->m_FoundPimAtcIdsBySources.insertMulti(source, atcIds.at(j));
//                }
//            }
//        }
    }

//    qWarning() << "nbPims doseRelated" << nbPimDoseRelated << "nonDoseRelated" << nbPimNonDoseRelated << "Found AtcIds" << d->m_FoundPimIdsBySources;
    return nbPimNonDoseRelated + nbPimDoseRelated;
}

static PimInteraction *getPimInteractionFromDb(const int sourceId, const int pimId, IDrugEngine *engine, bool getIcdRelatedPims = true)
{
    if (!getIcdRelatedPims) {
        QHash<int, QString> where;
        where.insert(Constants::PIMS_RELICD_PIM_ID, QString("=%1").arg(pimId));
        int nbicd = drugsBase().count(Constants::Table_PIMS_RELATED_ICD, Constants::PIMS_RELICD_PIM_ID, drugsBase().getWhereClause(Constants::Table_PIMS_RELATED_ICD, where));
        if (nbicd > 0)
            return 0;
    }
    PimInteraction *pim = new PimInteraction(engine);
    // Get PIMS , Source && Type infos
    // Prepare the SQL query
    Utils::FieldList get;
    get << drugsBase().fields(Constants::Table_PIMS);
    get << drugsBase().fields(Constants::Table_PIM_SOURCES);
    get << drugsBase().fields(Constants::Table_PIM_TYPES);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_PIM_SOURCES, Constants::PIM_SOURCES_SID, Constants::Table_PIMS, Constants::PIMS_SID);
    joins << Utils::Join(Constants::Table_PIM_TYPES, Constants::PIM_TYPES_TID, Constants::Table_PIMS, Constants::PIMS_TID);

    Utils::FieldList where;
    where << Utils::Field(Constants::Table_PIM_SOURCES, Constants::PIM_SOURCES_SID, QString("=%1").arg(sourceId));
    where << Utils::Field(Constants::Table_PIMS, Constants::PIMS_ID, QString("=%1").arg(pimId));

//        select PIMS.*, PIM_SOURCES.*, PIM_TYPES.* from  PIMS
//        JOIN PIM_SOURCES ON PIM_SOURCES.PIM_SID=PIMS.PIM_SID
//        JOIN PIM_TYPES ON PIM_TYPES.PIM_TID=PIMS.PIM_TID
//        where PIMS.PIM_ID=28 AND PIMS.PIM_SID=1;

    QString req = drugsBase().select(get, joins, where);

//    qWarning() << req;

    QSqlQuery query(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
    if (query.exec(req)) {
        if (query.next()) {
            // get << drugsBase().fields(Constants::Table_PIMS);
            // get << drugsBase().fields(Constants::Table_PIM_SOURCES);
            // get << drugsBase().fields(Constants::Table_PIM_TYPES);
            // get PIMS table values
            pim->setValue(PimInteraction::PIM_ID, query.value(Constants::PIMS_ID));
            pim->setValue(PimInteraction::PIM_SID, query.value(Constants::PIMS_SID));
            pim->setValue(PimInteraction::PIM_TID, query.value(Constants::PIMS_TID));
            pim->setValue(PimInteraction::PIM_Level, query.value(Constants::PIMS_LEVEL));
            pim->setValue(PimInteraction::PIM_RiskMasterLid, query.value(Constants::PIMS_RISK_MASTER_LID));
            int c = Constants::PIMS_MaxParam;
            // get PIM_SOURCES table values
            pim->setValue(PimInteraction::PIM_SourceName, query.value(c + Constants::PIM_SOURCES_NAME));
            pim->setValue(PimInteraction::PIM_ReferencesLink, query.value(c + Constants::PIM_SOURCES_WWW));
            pim->setValue(PimInteraction::PIM_PubMed_PMID, query.value(c + Constants::PIM_SOURCES_PMID));
            pim->setValue(PimInteraction::PIM_CountryRelated, query.value(c + Constants::PIM_SOURCES_COUNTRY));
            c += Constants::PIM_SOURCES_MaxParam;
            // get PIM_TYPES table values
            pim->setValue(PimInteraction::PIM_TypeMasterLid, query.value(c + Constants::PIM_TYPES_MASTER_LID));
        } else {
            LOG_ERROR_FOR("PimInteraction", QString("No PIM found. PIM_ID:%1; PIM_SID:%2").arg(pimId).arg(sourceId));
        }
    } else {
        LOG_QUERY_ERROR_FOR("PimInteraction", query);
    }
    query.finish();

    // get all ATC_ID
    joins.clear();
    joins << Utils::Join(Constants::Table_PIMS, Constants::PIMS_ID, Constants::Table_PIMS_RELATED_ATC, Constants::PIMS_RELATC_PIM_ID);
    req = drugsBase().select(Constants::Table_PIMS_RELATED_ATC, joins, where);
    if (query.exec(req)) {
        while (query.next()) {
            PimAtcRelated atc(query.value(Constants::PIMS_RELATC_ATC_ID).toInt(),
                              query.value(Constants::PIMS_RELATC_MAXDAYDOSE).toDouble(),
                              query.value(Constants::PIMS_RELATC_MAXDAYDOSEUNIT).toString());
            pim->addAtcRelated(atc);
        }
    } else {
        LOG_QUERY_ERROR_FOR("PimInteraction", query);
    }
    query.finish();

    // get all ICD_SID
    joins.clear();
    joins << Utils::Join(Constants::Table_PIMS, Constants::PIMS_ID, Constants::Table_PIMS_RELATED_ICD, Constants::PIMS_RELICD_PIM_ID);
    req = drugsBase().select(Constants::Table_PIMS_RELATED_ICD, joins, where);
    if (query.exec(req)) {
        while (query.next()) {
            pim->addIcdSid(query.value(Constants::PIMS_RELICD_ICD_SID));
        }
    } else {
        LOG_QUERY_ERROR_FOR("PimInteraction", query);
    }
    query.finish();

    pim->warn();

    return pim;
}

QVector<IDrugInteraction *> PimEngine::getAllInteractionsFound()
{
    QVector<IDrugInteraction *> toReturn;

    // cycle into d->m_FoundPimIdsBySources to get the pims
    // foreach PIM sources
    foreach(const int sourceId, d->m_FoundPimIdsBySources.uniqueKeys()) {
        foreach(const int pimId, d->m_FoundPimIdsBySources.values(sourceId)) {
            PimInteraction *pim = getPimInteractionFromDb(sourceId, pimId, this, false);
            if (!pim)
                continue;

            // associate drugs to interaction
            const QVector<int> &pimAtcs = pim->allAtcRelated();
            for(int i = 0; i < d->m_TestedDrugs.count(); ++i) {
                IDrug *drug = d->m_TestedDrugs.at(i);
                const QVector<int> &drugAtcs = drug->allInnAndInteractingClassesIds();
                for (int j = 0; j < pimAtcs.count(); ++j) {
                    if (drugAtcs.contains(pimAtcs.at(j))) {
                        pim->addInteractingDrug(drug);
                    }
                }
            }
            toReturn << pim;
        }  // end for each detected pimId
    }  // end for each sources
    return toReturn;
}

QVector<IDrugInteractionAlert *> PimEngine::getAllAlerts(DrugInteractionResult *addToResult)
{
    QVector<IDrugInteractionAlert *> alerts;
    alerts << new Alert(addToResult, this);
    return alerts;
}

void PimEngine::setActive(bool activate)
{
    if (isActive()==activate)
        return;
    // update settings
    if (activate) {
        settings()->appendToValue(Constants::S_ACTIVATED_INTERACTION_ENGINES, Constants::PIM_ENGINE_UID);
    } else {
        QStringList l = settings()->value(Constants::S_ACTIVATED_INTERACTION_ENGINES).toStringList();
        l.removeAll(Constants::PIM_ENGINE_UID);
        settings()->setValue(Constants::S_ACTIVATED_INTERACTION_ENGINES, l);
    }
}


