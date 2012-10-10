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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugdruginteractionengine.h"

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/idruginteraction.h>
#include <drugsbaseplugin/druginteractionresult.h>
#include <drugsbaseplugin/idruginteractionalert.h>
#include <drugsbaseplugin/idrug.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>

#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_database.h>

#include <QVector>
#include <QCoreApplication>

enum {
    WarnDDIComputation = false,
    WarnDatabaseExtraction = false,
    WarnFindInteractingDrugInsideDDI = false
};

static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

using namespace DrugInteractions::Internal;
using namespace DrugsDB;
using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;

namespace  {

    const char* const LIST_BASIC_INFO =
            "<tr>"
            "  <td colspan=2><b>%1</b></td>\n"
            "</tr>\n"
            "<tr>"
            "  <td rowspan=2><b>Drugs:</b></td>\n"
            "  <td>%2<br />&nbsp;&nbsp;&nbsp;&nbsp;%3</td>"
            "</tr>"
            "<tr>"
            "  <td>%4<br />&nbsp;&nbsp;&nbsp;&nbsp;%5</td>\n"
            "</tr>\n";
    const char* const LIST_FULL_INFO =
            "<tr>\n"
            "  <td>%1</td>\n"
            "  <td>%2</td>\n"
            "</tr>\n"
            "<tr>\n"
            "  <td>%3</td>\n"
            "  <td>%4</td>\n"
            "</tr>\n";

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

class DrugsInteraction : public IDrugInteraction
{
public:
    enum DataRepresentation {
        DI_Id,
        DI_TypeId,
        DI_TypeName,
        DI_ATC1,
        DI_ATC2,
        DI_ATC1_Label,
        DI_ATC2_Label,
        DI_Risk,
        DI_Management,
        DI_RiskId,
        DI_ManagementId,

        // only used for test
        DI_RiskFr,
        DI_RiskEn,
        DI_ManagementFr,
        DI_ManagementEn,
        DI_ReferencesLink
    };


    DrugsInteraction(IDrugEngine *parentEngine) : IDrugInteraction(parentEngine), m_Engine(parentEngine) {}
    ~DrugsInteraction() {}

    void setValue(const int ref, const QVariant &value)
    {
        if (ref == DI_TypeId) {
            QString t = value.toString();
            DrugDrugInteractionEngine::TypesOfIAM r = DrugDrugInteractionEngine::NoIAM;
            if (t.contains("A"))
                r |= DrugDrugInteractionEngine::DrugDuplication;
            if (t.contains("U"))
                r |= DrugDrugInteractionEngine::InnDuplication;
            if (t.contains("Z"))
                r |= DrugDrugInteractionEngine::ClassDuplication;
            if (t.contains("P"))
                r |= DrugDrugInteractionEngine::Precaution;
            if (t.contains("C"))
                r |= DrugDrugInteractionEngine::ContreIndication;
            if (t.contains("D"))
                r |= DrugDrugInteractionEngine::Deconseille;
            if (t.contains("T"))
                r |= DrugDrugInteractionEngine::APrendreEnCompte;
            if (t.contains("450"))
                r |= DrugDrugInteractionEngine::P450;
            if (t.contains("I"))
                r |= DrugDrugInteractionEngine::Information;
            if (t.contains("Y"))
                r |= DrugDrugInteractionEngine::GPG;
            m_Infos.insert(ref, int(r));
        } else {
            m_Infos.insert(ref, value);
        }
    }

    QVariant value(const int ref) const
    {
        switch (ref) {
        case DI_TypeName: return typeToString(m_Infos.value(DI_TypeId).toInt());
        }
        return m_Infos.value(ref);
    }

    // IDrugInteraction interface
    IDrugEngine *engine() const {return m_Engine;}

    bool isDrugDrugInteraction() const {return true;}
    bool isPotentiallyInappropriate() const {return false;}

    QString type() const {return value(DI_TypeName).toString();}

    QList<IDrug *> drugs() const {return m_InteractingDrugs;}

    QIcon icon(const int levelOfWarning, const int iconsize) const
    {
        DrugDrugInteractionEngine::TypesOfIAM level = DrugDrugInteractionEngine::TypesOfIAM(m_Infos.value(DI_TypeId).toInt());
        Core::ITheme *th = theme();
        Core::ITheme::IconSize size = Core::ITheme::IconSize(iconsize);
        // Minimal alerts
        if (level & DrugDrugInteractionEngine::ContreIndication && (levelOfWarning <= Constants::MinimumLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONCRITICAL, size);
        else if (level & DrugDrugInteractionEngine::Deconseille && (levelOfWarning <= Constants::MinimumLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONDECONSEILLEE, size);
        // Moderate alerts
        else if ((level & DrugDrugInteractionEngine::APrendreEnCompte) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONTAKEINTOACCOUNT, size);
        else if ((level & DrugDrugInteractionEngine::P450) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONP450, size);
        else if ((level & DrugDrugInteractionEngine::GPG) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONGPG, size);
        else if ((level & DrugDrugInteractionEngine::Precaution) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONPRECAUTION, size);
        // Maximum alerts
        else if ((level & DrugDrugInteractionEngine::Information) && (levelOfWarning == Constants::MaximumLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONINFORMATION, size);
        else if (((level & DrugDrugInteractionEngine::InnDuplication) ||
                  (level & DrugDrugInteractionEngine::ClassDuplication) ||
                  (level & DrugDrugInteractionEngine::DrugDuplication)) &&
                 (levelOfWarning == Constants::MaximumLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONINFORMATION, size);
        else if (level & DrugDrugInteractionEngine::NoIAM)
            return th->icon(Constants::INTERACTION_ICONOK, size);
        else
            return th->icon(Constants::INTERACTION_ICONUNKONW, size);
        return QIcon();
    }

    QString header(const QString &separator = QString::null) const
    {
        return QString("%1 %2 %3")
                .arg(drugsBase().getAtcLabel(m_Infos.value(DI_ATC1).toInt()))
                .arg(separator)
                .arg(drugsBase().getAtcLabel(m_Infos.value(DI_ATC2).toInt()));
    }

    QString risk(const QString &lang = QString::null) const
    {
        QString l = lang;
        if (l.isEmpty())
            l = QLocale().name().left(2);
        QString r;
        if (l=="fr")
            r = drugsBase().getLabel(m_Infos.value(DI_RiskId).toInt(), l);
        else
            r = drugsBase().getLabel(m_Infos.value(DI_RiskId).toInt(), "en");
        return r.replace("<br />", "<br>");
    }

    QString management(const QString &lang = QString::null) const
    {
        QString l = lang;
        if (l.isEmpty())
            l = QLocale().name().left(2);
        QString r;
        if (l=="fr")
            r = drugsBase().getLabel(m_Infos.value(DI_ManagementId).toInt(), l);
        else
            r = drugsBase().getLabel(m_Infos.value(DI_ManagementId).toInt(), "en");
        return r.replace("<br />", "<br>");
    }

    QString referencesLink(const QString &lang = QString::null) const
    {
        Q_UNUSED(lang);
        return m_Infos.value(DI_ReferencesLink).toString();
    }

    const IDrug *getInteractingDrug(const IDrug *drug)
    {
        if (WarnFindInteractingDrugInsideDDI) {
            qWarning() << "getInteractingDrug from" << drug->brandName();
            qWarning() << "    drug ATC ids" << drug->allAtcIds();
            qWarning() << "    ATC1" << m_Infos.value(DI_ATC1) << "ATC2" << m_Infos.value(DI_ATC2);
        }

        int testAtcId = m_Infos.value(DI_ATC1).toInt();
        const QVector<int> &ids = drug->allAtcIds();
        if (ids.contains(m_Infos.value(DI_ATC1).toInt()) && ids.contains(m_Infos.value(DI_ATC2).toInt())) {
            if (m_InteractingDrugs.count()==2) {
                for(int i=0; i < m_InteractingDrugs.count(); ++i) {
                    const IDrug *dr = m_InteractingDrugs.at(i);
                    if (dr==drug)
                        continue;
                    return dr;
                }
            }
        }
        if (ids.contains(testAtcId))
            testAtcId = m_Infos.value(DI_ATC2).toInt();

        if (WarnFindInteractingDrugInsideDDI)
            qWarning() << "    searching" << testAtcId;

        for(int i=0; i < m_InteractingDrugs.count(); ++i) {
            const IDrug *dr = m_InteractingDrugs.at(i);
            if (dr==drug)
                continue;

            if (WarnFindInteractingDrugInsideDDI) {
                qWarning() << "        test" << dr->brandName();
                qWarning() << "            " << dr->allAtcIds();
            }

            if (typeId()==DrugDrugInteractionEngine::DrugDuplication)
                return dr;
            if (dr->allAtcIds().contains(testAtcId)) {
                return dr;
            }
        }
        LOG_ERROR_FOR("DDI", "No interactor found?");
        return 0;
    }

    QString toHtml(bool detailled = false) const
    {
        if (m_InteractingDrugs.count() != 2)
            return QString();
        // Link Atc_Ids with interacting drug
        int firstDrugAtcId = 0;
        int secondDrugAtcId = 0;
        if (m_InteractingDrugs.at(0)->atcIdsContains(m_Infos.value(DI_ATC1).toInt())) {
            firstDrugAtcId = m_Infos.value(DI_ATC1).toInt();
            secondDrugAtcId = m_Infos.value(DI_ATC2).toInt();
        } else {
            secondDrugAtcId= m_Infos.value(DI_ATC1).toInt();
            firstDrugAtcId = m_Infos.value(DI_ATC2).toInt();
        }
        // Create the Html
        QString tmp = QString(LIST_BASIC_INFO)
               .arg(type())
               .arg(drugs().at(0)->brandName().replace(" ", "&nbsp;"))
               .arg(drugsBase().getAtcLabel(firstDrugAtcId).replace(" ", "&nbsp;"))
               .arg(drugs().at(1)->brandName().replace(" ", "&nbsp;"))
               .arg(drugsBase().getAtcLabel(secondDrugAtcId).replace(" ", "&nbsp;"));
        if (detailled) {
            QString r = risk();
            if (!r.isEmpty()) {
                tmp += QString("<tr>\n"
                               "  <td><b>%1</b></td>\n"
                               "  <td>%2</td>\n"
                               "</tr>\n"
                               )
                        .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::NATURE_OF_RISK))
                        .arg(r
                             .replace("<br>", " ")
                             .replace("<", "&lt;")
                             .replace(">", "&gt;"));
            }
            r = management();
            if (!r.isEmpty()) {
                tmp += QString("<tr>\n"
                               "  <td><b>%1</b></td>\n"
                               "  <td>%2</td>\n"
                               "</tr>\n"
                               )
                        .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::MANAGEMENT))
                        .arg(r
                             .replace("<br>", "__")
                             .replace("<", "&lt;")
                             .replace(">", "&gt;")
                             .replace("__", "<br>"));
            }
        }
        tmp.prepend("<table width=100% border=1>");
        tmp.append("</table>");
        return tmp;
    }

    int sortIndex() const {return m_Infos.value(DI_TypeId).toInt();}

    // Private interface
    int typeId() const {return m_Infos.value(DI_TypeId).toInt();}
    static QString typeToString(const int t)
    {
         QStringList tmp;
         DrugDrugInteractionEngine::TypesOfIAM r = DrugDrugInteractionEngine::TypesOfIAM(t);
         if (r & DrugDrugInteractionEngine::ContreIndication)
              tmp << tkTr(Trans::Constants::CONTRAINDICATION);
         if (r & DrugDrugInteractionEngine::Deconseille)
              tmp << tkTr(Trans::Constants::DISCOURAGED);
         if (r & DrugDrugInteractionEngine::P450)
              tmp << tkTr(Trans::Constants::P450_IAM);
         if (r & DrugDrugInteractionEngine::GPG)
              tmp << tkTr(Trans::Constants::GPG_IAM);
         if (r & DrugDrugInteractionEngine::APrendreEnCompte)
              tmp << tkTr(Trans::Constants::TAKE_INTO_ACCOUNT);
         if (r & DrugDrugInteractionEngine::Precaution)
              tmp << tkTr(Trans::Constants::PRECAUTION_FOR_USE);
         if (r & DrugDrugInteractionEngine::DrugDuplication)
              tmp << tkTr(Trans::Constants::DRUG_DUPLICATION);
         if (r & DrugDrugInteractionEngine::InnDuplication)
              tmp << tkTr(Trans::Constants::INN_DUPLICATION);
         if (r & DrugDrugInteractionEngine::ClassDuplication)
              tmp << tkTr(Trans::Constants::CLASS_DUPLICATION);
         if (r & DrugDrugInteractionEngine::Information)
              tmp << tkTr(Trans::Constants::INFORMATION);
         return tmp.join(", ");
    }

    void addInteractingDrug(IDrug *drug)
    {
        Q_ASSERT(drug);
        if (!m_InteractingDrugs.contains(drug))
            m_InteractingDrugs << drug;
    }

    void warn() const
    {
        QString n1, n2;
        if (m_InteractingDrugs.count() == 2) {
            n1 = m_InteractingDrugs.at(0)->brandName();
            n2 = m_InteractingDrugs.at(1)->brandName();
        } else if (m_InteractingDrugs.count() == 1) {
            n1 = m_InteractingDrugs.at(0)->brandName();
            n2 = "No drug";
        } else {
            n1 = "No drug";
            n2 = "No drug";
        }
        QString tmp = QString("DrugsInteraction:\n"
                              "    (ID: %1; Type: %2; ID1: %3; ID2: %4)\n"
                              "    (Drug1:%5)\n"
                              "    (Drug2:%6)")
                .arg(m_Infos.value(DI_Id).toString())
                .arg(m_Infos.value(DI_TypeId).toString())
                .arg(m_Infos.value(DI_ATC1).toString())
                .arg(m_Infos.value(DI_ATC2).toString())
                .arg(n1)
                .arg(n2)
                ;
        qWarning() << tmp;
    }

private:
    IDrugEngine *m_Engine;
    QHash<int, QVariant> m_Infos;
    QList<IDrug *> m_InteractingDrugs;
};

class Alert : public IDrugInteractionAlert
{
public:
    Alert(DrugInteractionResult *result, IDrugEngine *engine) :
            IDrugInteractionAlert(engine), m_Overridden(false), m_Result(result)
    {
    }

    ~Alert() {}

    QString engineUid() const {return Constants::DDI_ENGINE_UID;}

    static bool typeInLevel(const DrugDrugInteractionEngine::TypesOfIAM r, const int level)
    {
        bool ret = false;
        switch (level) {
        case Constants::MinimumLevelOfWarning:
            {
                ret = (r & DrugDrugInteractionEngine::ContreIndication ||
                       r & DrugDrugInteractionEngine::Deconseille);
                break;
            }
        case Constants::ModerateLevelOfWarning:
            {
                ret = (r & DrugDrugInteractionEngine::ContreIndication ||
                       r & DrugDrugInteractionEngine::Deconseille ||
                       r & DrugDrugInteractionEngine::GPG ||
                       r & DrugDrugInteractionEngine::P450 ||
                       r & DrugDrugInteractionEngine::APrendreEnCompte ||
                       r & DrugDrugInteractionEngine::Precaution);
                break;
            }
        case Constants::MaximumLevelOfWarning:
            {
                ret = true;
                break;
            }
        }
        return ret;
    }

    // static alert
    QIcon icon(const IDrug *drug, const DrugInteractionInformationQuery &query) const
    {
        if (!m_Result->testedDrugs().contains((IDrug*)drug))
            return QIcon();
        if (!query.engineUid.isEmpty() && query.engineUid!=Constants::DDI_ENGINE_UID) {
            return QIcon();
        }
        int levelOfWarning = query.levelOfWarningStaticAlert;
        DrugDrugInteractionEngine::TypesOfIAM level = getMaximumTypeOfIAM(m_Result->interactions(), drug);
        Core::ITheme *th = theme();
        Core::ITheme::IconSize size = Core::ITheme::IconSize(query.iconSize);
        // Minimal alerts
        if (level & DrugDrugInteractionEngine::ContreIndication && (levelOfWarning <= Constants::MinimumLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONCRITICAL, size);
        else if (level & DrugDrugInteractionEngine::Deconseille && (levelOfWarning <= Constants::MinimumLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONDECONSEILLEE, size);
        // Moderate alerts
        else if ((level & DrugDrugInteractionEngine::APrendreEnCompte) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONTAKEINTOACCOUNT, size);
        else if ((level & DrugDrugInteractionEngine::P450) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONP450, size);
        else if ((level & DrugDrugInteractionEngine::GPG) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONGPG, size);
        else if ((level & DrugDrugInteractionEngine::Precaution) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONPRECAUTION, size);
        // Maximum alerts
        else if ((level & DrugDrugInteractionEngine::Information) && (levelOfWarning == Constants::MaximumLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONINFORMATION, size);
        else if (((level & DrugDrugInteractionEngine::InnDuplication) ||
                  (level & DrugDrugInteractionEngine::ClassDuplication) ||
                  (level & DrugDrugInteractionEngine::DrugDuplication)) &&
                 (levelOfWarning == Constants::MaximumLevelOfWarning))
            return th->icon(Constants::INTERACTION_ICONINFORMATION, size);
        else if (level & DrugDrugInteractionEngine::NoIAM)
            return th->icon(Constants::INTERACTION_ICONOK, size);
        else
            return th->icon(Constants::INTERACTION_ICONUNKONW, size);
        return QIcon();
    }

    DrugDrugInteractionEngine::TypesOfIAM getMaximumTypeOfIAM(const QVector<IDrugInteraction *> &interactions, const IDrug *drug) const
    {
        DrugDrugInteractionEngine::TypesOfIAM r = DrugDrugInteractionEngine::NoIAM;
        for(int i=0; i < interactions.count(); ++i) {
            IDrugInteraction *ddi = interactions.at(i);
            if (ddi->engine()->uid()==Constants::DDI_ENGINE_UID) {
                if (!drug) {
                    r |= DrugDrugInteractionEngine::TypesOfIAM(ddi->sortIndex());
                } else {
                    if (ddi->drugs().contains((IDrug*)drug)) {
                        r |= DrugDrugInteractionEngine::TypesOfIAM(ddi->sortIndex());
                    }
                }
            }
        }
        return r;
    }

    static QString iconPath(const int levelOfDdi, const int iconSize)
    {
        Core::ITheme *th = theme();
        DrugDrugInteractionEngine::TypesOfIAM level = DrugDrugInteractionEngine::TypesOfIAM(levelOfDdi);
        Core::ITheme::IconSize size = Core::ITheme::IconSize(iconSize);
        // Minimal alerts
        if (level & DrugDrugInteractionEngine::ContreIndication)
            return th->iconFullPath(Constants::INTERACTION_ICONCRITICAL, size);
        else if (level & DrugDrugInteractionEngine::Deconseille)
            return th->iconFullPath(Constants::INTERACTION_ICONDECONSEILLEE, size);
        // Moderate alerts
        else if ((level & DrugDrugInteractionEngine::APrendreEnCompte))
            return th->iconFullPath(Constants::INTERACTION_ICONTAKEINTOACCOUNT, size);
        else if ((level & DrugDrugInteractionEngine::P450))
            return th->iconFullPath(Constants::INTERACTION_ICONP450, size);
        else if ((level & DrugDrugInteractionEngine::GPG))
            return th->iconFullPath(Constants::INTERACTION_ICONGPG, size);
        else if ((level & DrugDrugInteractionEngine::Precaution))
            return th->iconFullPath(Constants::INTERACTION_ICONPRECAUTION, size);
        // Maximum alerts
        else if ((level & DrugDrugInteractionEngine::Information))
            return th->iconFullPath(Constants::INTERACTION_ICONINFORMATION, size);
        else if ((level & DrugDrugInteractionEngine::InnDuplication) ||
                  (level & DrugDrugInteractionEngine::ClassDuplication) ||
                  (level & DrugDrugInteractionEngine::DrugDuplication))
            return th->iconFullPath(Constants::INTERACTION_ICONINFORMATION, size);
        else if (level & DrugDrugInteractionEngine::NoIAM)
            return th->iconFullPath(Constants::INTERACTION_ICONOK, size);
        return th->iconFullPath(Constants::INTERACTION_ICONUNKONW, size);
    }

    QString message(const IDrug *drug, const DrugInteractionInformationQuery &query) const
    {
        // TODO: manage in the query level usage : static or dynamic?
        QString toReturn;
        if (!m_Result->testedDrugs().contains((IDrug*)drug))
            return toReturn;

        // get all interactions related to the drug
        QVector<IDrugInteraction *> interactions = m_Result->getInteractions(drug, Constants::DDI_ENGINE_UID);
        QString tmp;
        switch (query.messageType)
        {
        case DrugInteractionInformationQuery::ShortToolTip:
        case DrugInteractionInformationQuery::InformationAlert:
            {
            QMap<int, QString> lines;
            QVector<int> drugInteractionAtcIds;
            QVector<int> drugInteractionIds;
            for(int j=0; j < interactions.count(); ++j) {
                // Get interaction
                IDrugInteraction *di = interactions.at(j);
                DrugsInteraction *ddi = static_cast<DrugsInteraction *>(di);
                Q_ASSERT(ddi);
                if (!ddi)
                    continue;

                // interaction with this interactor already included ?
                int interactorAtcId = -1;
                if (drug->allInnAndInteractingClassesIds().contains(ddi->value(DrugsInteraction::DI_ATC1).toInt())) {
                    interactorAtcId = ddi->value(DrugsInteraction::DI_ATC2).toInt();
                } else {
                    interactorAtcId = ddi->value(DrugsInteraction::DI_ATC1).toInt();
                }
                if (drugInteractionAtcIds.contains(interactorAtcId))
                    continue;
                drugInteractionAtcIds << interactorAtcId;

                // interaction already included ?
                if (drugInteractionIds.contains(ddi->value(DrugsInteraction::DI_Id).toInt()))
                    continue;
                drugInteractionIds << ddi->value(DrugsInteraction::DI_Id).toInt();

                // get the line related to the level of DDI
                int typeId = -1;
                DrugDrugInteractionEngine::TypesOfIAM r = DrugDrugInteractionEngine::TypesOfIAM(ddi->typeId());
                if (!typeInLevel(r, query.levelOfWarningStaticAlert))
                    continue;
                // Minimal alerts
                if (r & DrugDrugInteractionEngine::ContreIndication && (query.levelOfWarningStaticAlert <= Constants::MinimumLevelOfWarning))
                    typeId = DrugDrugInteractionEngine::ContreIndication;
                else if (r & DrugDrugInteractionEngine::Deconseille && (query.levelOfWarningStaticAlert <= Constants::MinimumLevelOfWarning))
                    typeId = DrugDrugInteractionEngine::Deconseille;
                // Moderate alerts
                else if ((r & DrugDrugInteractionEngine::APrendreEnCompte) && (query.levelOfWarningStaticAlert <= Constants::ModerateLevelOfWarning))
                    typeId = DrugDrugInteractionEngine::APrendreEnCompte;
                else if ((r & DrugDrugInteractionEngine::P450) && (query.levelOfWarningStaticAlert <= Constants::ModerateLevelOfWarning))
                    typeId = DrugDrugInteractionEngine::P450;
                else if ((r & DrugDrugInteractionEngine::GPG) && (query.levelOfWarningStaticAlert <= Constants::ModerateLevelOfWarning))
                    typeId = DrugDrugInteractionEngine::GPG;
                else if ((r & DrugDrugInteractionEngine::Precaution) && (query.levelOfWarningStaticAlert <= Constants::ModerateLevelOfWarning))
                    typeId = DrugDrugInteractionEngine::Precaution;
                // Maximum alerts
                else if ((r & DrugDrugInteractionEngine::Information) && (query.levelOfWarningStaticAlert == Constants::MaximumLevelOfWarning))
                    typeId = DrugDrugInteractionEngine::Information;
                else if ((r & DrugDrugInteractionEngine::DrugDuplication) && (query.levelOfWarningStaticAlert == Constants::MaximumLevelOfWarning))
                    typeId = DrugDrugInteractionEngine::DrugDuplication;
                else if ((r & DrugDrugInteractionEngine::InnDuplication) && (query.levelOfWarningStaticAlert == Constants::MaximumLevelOfWarning))
                    typeId = DrugDrugInteractionEngine::InnDuplication;
                else if ((r & DrugDrugInteractionEngine::ClassDuplication) && (query.levelOfWarningStaticAlert == Constants::MaximumLevelOfWarning))
                    typeId = DrugDrugInteractionEngine::ClassDuplication;
                else if (query.levelOfWarningStaticAlert & DrugDrugInteractionEngine::NoIAM)
                    typeId = DrugDrugInteractionEngine::NoIAM;


                // construct line of the alert
                QString line;
                QString drug2;
                if (ddi->typeId() == DrugDrugInteractionEngine::DrugDuplication) {
                    line = QString("<tr>\n"
                                   "  <td>* %1</td>\n"
                                   "</tr>")
                            .arg(ddi->getInteractingDrug(drug)->brandName());
                } else {
                    if (ddi->drugs().at(0)->drugId()==drug->drugId()) {
                        drug2 = ddi->drugs().at(1)->brandName();
                    } else {
                        drug2 = ddi->drugs().at(0)->brandName();
                    }
                    line = QString("<tr>\n"
                                   "  <td>* %1<br>&nbsp;&nbsp;&nbsp;&nbsp;%2</td>\n"
                                   "</tr>")
                            .arg(ddi->getInteractingDrug(drug)->brandName())
                            .arg(di->header("//"));
                }
                QString &ditmp = lines[typeId];

                // no double
                if (!ditmp.contains(line))
                    ditmp += line;
            }

            // construct full message
            if (lines.isEmpty())
                return QString();

            QMap<int, QString>::const_iterator i = lines.constEnd();
            --i;
            while (true) {
                const QString &drugs = i.value();
                if (!drugs.isEmpty()) {
                    tmp += QString("<table border=\"0\" width=\"100%\">\n"
                                   "<tr>\n"
                                   "  <td rowspan=\"%1\" width=\"32px\" align=\"center\" valign=\"top\"><img src=\"%2\"></td>\n"
                                   "  <td><b>%3</b></td>\n"
                                   "</tr>\n"
                                   "%4\n"
                                   "</table>\n<br />"
                                   )
                            .arg(drugs.count("<tr>")  + 1)
                            .arg(iconPath(i.key(), Core::ITheme::MediumIcon))
                            .arg(DrugsInteraction::typeToString(i.key()))
                            .arg(drugs);
                }
                if (i == lines.constBegin())
                    break;
                --i;
            }

            toReturn = tmp;

            break;
        }
        case DrugInteractionInformationQuery::DetailledToolTip:
            {
                QMap<int, QString> lines;
                QVector<int> drugInteractionIds;
                for(int j=0; j < interactions.count(); ++j) {
                    IDrugInteraction *di = interactions.at(j);
                    DrugsInteraction *ddi = static_cast<DrugsInteraction *>(di);
                    Q_ASSERT(ddi);

                    if (drugInteractionIds.contains(ddi->value(DrugsInteraction::DI_Id).toInt()))
                        continue;
                    drugInteractionIds << ddi->value(DrugsInteraction::DI_Id).toInt();

                    int typeId = -1;
                    DrugDrugInteractionEngine::TypesOfIAM level = DrugDrugInteractionEngine::TypesOfIAM(ddi->typeId());
                    // Minimal alerts
                    if (level & DrugDrugInteractionEngine::ContreIndication && (query.levelOfWarningStaticAlert <= Constants::MinimumLevelOfWarning))
                        typeId = DrugDrugInteractionEngine::ContreIndication;
                    else if (level & DrugDrugInteractionEngine::Deconseille && (query.levelOfWarningStaticAlert <= Constants::MinimumLevelOfWarning))
                        typeId = DrugDrugInteractionEngine::Deconseille;
                    // Moderate alerts
                    else if ((level & DrugDrugInteractionEngine::APrendreEnCompte) && (query.levelOfWarningStaticAlert <= Constants::ModerateLevelOfWarning))
                        typeId = DrugDrugInteractionEngine::APrendreEnCompte;
                    else if ((level & DrugDrugInteractionEngine::P450) && (query.levelOfWarningStaticAlert <= Constants::ModerateLevelOfWarning))
                        typeId = DrugDrugInteractionEngine::P450;
                    else if ((level & DrugDrugInteractionEngine::GPG) && (query.levelOfWarningStaticAlert <= Constants::ModerateLevelOfWarning))
                        typeId = DrugDrugInteractionEngine::GPG;
                    else if ((level & DrugDrugInteractionEngine::Precaution) && (query.levelOfWarningStaticAlert <= Constants::ModerateLevelOfWarning))
                        typeId = DrugDrugInteractionEngine::Precaution;
                    // Maximum alerts
                    else if ((level & DrugDrugInteractionEngine::Information) && (query.levelOfWarningStaticAlert == Constants::MaximumLevelOfWarning))
                        typeId = DrugDrugInteractionEngine::Information;
                    else if ((level & DrugDrugInteractionEngine::DrugDuplication) && (query.levelOfWarningStaticAlert == Constants::MaximumLevelOfWarning))
                        typeId = DrugDrugInteractionEngine::DrugDuplication;
                    else if ((level & DrugDrugInteractionEngine::InnDuplication) && (query.levelOfWarningStaticAlert == Constants::MaximumLevelOfWarning))
                        typeId = DrugDrugInteractionEngine::InnDuplication;
                    else if ((level & DrugDrugInteractionEngine::ClassDuplication) && (query.levelOfWarningStaticAlert == Constants::MaximumLevelOfWarning))
                        typeId = DrugDrugInteractionEngine::ClassDuplication;
                    else if (level & DrugDrugInteractionEngine::NoIAM)
                        typeId = DrugDrugInteractionEngine::NoIAM;

                    QString &ditmp = lines[typeId];
                    if (!ditmp.contains(di->header("//"))) {
                        ditmp += QString("<li>%1</li>\n")
                                .arg(di->header("//"));
                    }
                }
                if (lines.isEmpty())
                    return QString();
                QMap<int, QString>::const_iterator i = lines.constEnd();
                --i;
                while (true) {
                    if (!i.value().isEmpty()) {
                        // Manage drug duplication
                        if (i.key() == DrugDrugInteractionEngine::DrugDuplication) {
                            tmp += QString("<ul compact>"
                                           "  <li><b>%1</b></li>\n"
                                           "</ul>\n")
                                    .arg(DrugsInteraction::typeToString(i.key()));
                        } else {
                            // Manage all other interaction type
                            tmp += QString("<ul compact>"
                                           "  <li><b>%1</b></li>\n"
                                           "  <ul>\n"
                                           "%2"
                                           "  </ul>\n"
                                           "</ul>\n"
                                           )
                                    //                                .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::DDI_TEXT))
                                    .arg(DrugsInteraction::typeToString(i.key()))
                                    .arg(i.value());
                        }
                    }
                    if (i==lines.constBegin())
                        break;
                    --i;
                }
                toReturn = QString("<br /><table widht=100% border=1><tr><td align=center width=100%><b>%1</b></td></tr><tr><td>%2</td></tr></table>")
                           .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::DDI_TEXT))
                           .arg(tmp);
                break;
            }
        }

        return toReturn;
    }

    QString message(const DrugInteractionInformationQuery &query) const
    {
        Q_UNUSED(query);
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
            interactions = query.result->getInteractions(query.relatedDrug, Constants::DDI_ENGINE_UID);
        } else {
            interactions = query.result->interactions(Constants::DDI_ENGINE_UID);
        }
        // No interactions ?
        if (interactions.isEmpty())
            return false;
        DrugDrugInteractionEngine::TypesOfIAM r = getMaximumTypeOfIAM(interactions, query.relatedDrug);
        return typeInLevel(r, query.levelOfWarningDynamicAlert);
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

namespace DrugInteractions {
namespace Internal {
class DrugDrugInteractionEnginePrivate
{
public:
    QVector<IDrug *> m_TestedDrugs;
    QVector<IDrugInteraction *> m_FirstPassInteractions, m_Interactions;
    QMap<int, int> m_InteractionsIDs;        /*!<  All possible interactions based on ATC IDs*/
    QMultiMap<int, int> m_DDIFound;               /*!< modified by checkDrugInteraction() */
    QVector<int> m_DoNotWarnAtcDuplicates;
    bool m_LogChrono;
};
}
}

DrugDrugInteractionEngine::DrugDrugInteractionEngine(QObject *parent) :
        IDrugEngine(parent), d(new DrugDrugInteractionEnginePrivate)
{
    setObjectName("DrugDrugInteractionEngine");
    m_IsActive = settings()->value(Constants::S_ACTIVATED_INTERACTION_ENGINES).toStringList().contains(Constants::DDI_ENGINE_UID);
    d->m_LogChrono = false;
    connect(&drugsBase(), SIGNAL(drugsBaseHasChanged()), this, SLOT(drugsBaseChanged()));
}

DrugDrugInteractionEngine::~DrugDrugInteractionEngine()
{
    if (d)
        delete d;
    d = 0;
}

void DrugDrugInteractionEngine::drugsBaseChanged()
{
    init();
}

bool DrugDrugInteractionEngine::init()
{
    // get all interactions ids and mol <-> atc links
    d->m_InteractionsIDs.clear();
    QList<int> fields;
    fields << Constants::INTERACTIONS_ATC_ID1 << Constants::INTERACTIONS_ATC_ID2;
    QString req = drugsBase().select(Constants::Table_INTERACTIONS, fields);
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    db.transaction();
    QSqlQuery query(req , db);
    if (query.isActive()) {
        while (query.next()) {
            d->m_InteractionsIDs.insertMulti(query.value(0).toInt(), query.value(1).toInt());
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();
    LOG(QString("Retrieve %1 DDI").arg(d->m_InteractionsIDs.count()));

    d->m_DoNotWarnAtcDuplicates.clear();
    QHash<int,QString> where;
    // Old drugs base are buggy, boolean field is recorded as a string 'true'/'false'
    req = drugsBase().select(Constants::Table_ATC, Constants::ATC_ID);
    req += " WHERE ";
    where.insert(Constants::ATC_WARNDUPLICATES, "=0");
    req += drugsBase().getWhereClause(Constants::Table_ATC, where);
    where.clear();
    where.insert(Constants::ATC_WARNDUPLICATES, "='false'");
    req += " OR " + drugsBase().getWhereClause(Constants::Table_ATC, where);
    if (query.exec(req)) {
        while (query.next()) {
            d->m_DoNotWarnAtcDuplicates.append(query.value(0).toInt());
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    db.commit();
    return true;
}

bool DrugDrugInteractionEngine::isActive() const
{
    return settings()->value(Constants::S_ACTIVATED_INTERACTION_ENGINES).toStringList().contains(Constants::DDI_ENGINE_UID);
}

QString DrugDrugInteractionEngine::name() const {return QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::DDI_TEXT);}

QString DrugDrugInteractionEngine::shortName() const {return QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::DDI_SHORT_TEXT);}

QString DrugDrugInteractionEngine::tooltip() const { return tr("Detects interaction between drugs"); }

QString DrugDrugInteractionEngine::uid() const {return Constants::DDI_ENGINE_UID;}

QIcon DrugDrugInteractionEngine::icon(const int size) const
{
    return theme()->icon(Constants::I_DRUGDRUGINTERACTIONENGINE, Core::ITheme::IconSize(size));
}

QString DrugDrugInteractionEngine::iconFullPath(const int size) const
{
    return theme()->iconFullPath(Constants::I_DRUGDRUGINTERACTIONENGINE, Core::ITheme::IconSize(size));
}

/** Check the interaction between one drug and a list of drugs. The found interactions are stored in the d->m_Interactions vector. The vector must be cleared before calling this member. */
bool DrugDrugInteractionEngine::checkDrugInteraction(IDrug *drug, const QVector<IDrug *> &drugsList)
{
    QTime t;
    t.start();

    if (WarnDDIComputation)
        qWarning() << "\nDrugDrugInteractionEngine::checkDrugInteraction for drug" << drug->brandName().left(25);

    if (drug->numberOfInn() == 0) {
        if (WarnDDIComputation)
            qWarning() << "  * Drug does not have INN" << drug->brandName().left(25);
        return false;
    }

    if (DrugRoute::maximumSystemicEffect(drug) == DrugRoute::NoSystemicEffect) {
        if (WarnDDIComputation)
            qWarning() << "  * Drug route is not systemic" << drug->brandName().left(25);
        return false;
    }

    const QVector<int> &drug_iams = drug->allInnAndInteractingClassesIds();
    QVector<int> d_iams;
    for(int i = 0; i < drugsList.count(); ++i) {
        IDrug *drug2 = drugsList.at(i);
        if (drug2 == drug)
            continue;

        if (drug2->equals(drug)) {
            // Add a drug duplication
            DrugsInteraction *ddi = new DrugsInteraction(this);
            ddi->setValue(DrugsInteraction::DI_ATC1, -1);
            ddi->setValue(DrugsInteraction::DI_ATC2, -1);
            ddi->addInteractingDrug(drug);
            ddi->addInteractingDrug(drug2);
            d->m_FirstPassInteractions.append(ddi);
            if (WarnDDIComputation)
                qWarning() << "  * Drug duplication" << drug->brandName() << drug2->brandName().left(25);
            continue;
        }

        if (DrugRoute::maximumSystemicEffect(drug2) == DrugRoute::NoSystemicEffect) {
            if (WarnDDIComputation)
                qWarning() << "  * Drug does not have INN" << drug2->brandName().left(25);
            continue;
        }

        // prepare interacting atc from drug2
        for(int j = 0; j < drug2->allInnAndInteractingClassesIds().count(); ++j) {
            int s = drug2->allInnAndInteractingClassesIds().at(j);
            // do not double compute
            if (d_iams.contains(s))
                continue;
            d_iams << s;
//        }

//        for(int i=0; i < d_iams.count(); ++i) {
//            int s = d_iams.at(i);
            for(int j = 0; j < drug_iams.count(); ++j)  {
                int s2 = drug_iams.at(j);
                // foreach iam subst/class test existing interactions with *drug
                if (d->m_InteractionsIDs.keys(s).contains(s2)) {
                    if ((!d->m_DDIFound.contains(s2, s)) && (!d->m_DDIFound.contains(s, s2))) {
                        d->m_DDIFound.insertMulti(s2, s);
                        DrugsInteraction *ddi = new DrugsInteraction(this);
                        ddi->setValue(DrugsInteraction::DI_ATC1, s2);
                        ddi->setValue(DrugsInteraction::DI_ATC2, s);
                        ddi->addInteractingDrug(drug);
                        ddi->addInteractingDrug(drug2);
                        d->m_FirstPassInteractions.append(ddi);
                        if (WarnDDIComputation) {
                            qWarning() << "  * Found DDI" << drug->brandName().left(25) << drug2->brandName().left(25);
                            qWarning() << "             " << drugsBase().getAtcLabel(s) << drugsBase().getAtcLabel(s2);
                        }
                    }
                }

                //             Not necessary because interactions are "mirrored" in the database
                //             if (m_InteractionsIDs.keys(s2).contains(s))
                //                 if ((!m_DDIFound.contains(s2, s)) && (!m_DDIFound.contains(s, s2)))
                //                     m_DDIFound.insertMulti(s, s2);

                // test molecule duplication
                if ((s > 999) && (s2 > 999) && (s == s2)) {
                    if (!d->m_DDIFound.contains(s, -1)) {
                        d->m_DDIFound.insertMulti(s, -1);
                        DrugsInteraction *ddi = new DrugsInteraction(this);
                        ddi->setValue(DrugsInteraction::DI_ATC1, s);
                        ddi->setValue(DrugsInteraction::DI_ATC2, -1);
                        ddi->addInteractingDrug(drug);
                        ddi->addInteractingDrug(drug2);
                        d->m_FirstPassInteractions.append(ddi);
                        if (WarnDDIComputation) {
                            qWarning() << "  * Found Duplication" << drug->brandName().left(25) << drug2->brandName().left(25);
                            qWarning() << "                     " << drugsBase().getAtcLabel(s);
                        }
                    }
                }
            }
        }
    }

    if (d->m_LogChrono)
        Utils::Log::logTimeElapsed(t, "DrugDrugInteractionEngine", QString("checkDrugInteraction: %1 ; %2")
                                   .arg(drug->brandName()).arg(drugsList.count()));

    if (WarnDDIComputation)
        qWarning() << "  * Total DDI" << d->m_DDIFound.count() << "\n\n";

    if (d->m_DDIFound.count() != 0)
        return true;

    return false;
}

int DrugDrugInteractionEngine::calculateInteractions(const QVector<IDrug *> &drugs)
{
    QTime t;
    t.start();
    d->m_DDIFound.clear();
    d->m_TestedDrugs.clear();
    d->m_Interactions.clear();
    qDeleteAll(d->m_FirstPassInteractions);
    d->m_FirstPassInteractions.clear();
    d->m_TestedDrugs = drugs;
    if (WarnDDIComputation)
        qWarning() << "------------------------------------";
    foreach(IDrug *drug, drugs) {
        checkDrugInteraction(drug, drugs);
    }
    if (d->m_LogChrono)
        Utils::Log::logTimeElapsed(t, "DrugDrugInteractionEngine", QString("interactions(): %1 drugs").arg(drugs.count()));
    if (WarnDDIComputation)
        qWarning() << "------------------------------------";
    return d->m_DDIFound.count();
}

QVector<IDrugInteraction *> DrugDrugInteractionEngine::getInteractionsFromDatabase(IDrugInteraction *fromFirstPassInteraction)
{
    if (WarnDatabaseExtraction)
        qWarning() << "DrugDrugInteractionEngine::getInteractionsFromDatabase";
    QVector<IDrugInteraction *> toReturn;
    DrugsInteraction *fromddi = static_cast<DrugsInteraction *>(fromFirstPassInteraction);
    Q_ASSERT(fromddi);
    if (!fromddi)
        return toReturn;
    int _id1 = fromddi->value(DrugsInteraction::DI_ATC1).toInt();
    int id2 = fromddi->value(DrugsInteraction::DI_ATC2).toInt();
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return toReturn;
        }
    }

    // first test : is a duplication interaction ?
    if (id2 == -1) {
        if (WarnDatabaseExtraction)
            qWarning() << "  * Duplication" << drugsBase().getAtcLabel(_id1);

        // Drug duplication ?
        if (_id1==-1) {
            DrugsInteraction *ddi = 0;
            ddi = new ::DrugsInteraction(this);
            ddi->setValue(DrugsInteraction::DI_ATC1, _id1);
            ddi->setValue(DrugsInteraction::DI_ATC2, _id1);
            ddi->setValue(DrugsInteraction::DI_TypeId , "A");
            ddi->setValue(DrugsInteraction::DI_RiskFr, tkTr(Trans::Constants::DRUG_DUPLICATION));
            ddi->setValue(DrugsInteraction::DI_RiskEn, Trans::Constants::DRUG_DUPLICATION);
            ddi->setValue(DrugsInteraction::DI_ReferencesLink, QCoreApplication::translate("DrugsBase", "FreeDiams Interactions Engine"));
            foreach(IDrug *drug, fromddi->drugs()) {
                ddi->addInteractingDrug(drug);
            }
            toReturn << ddi;
            if (WarnDatabaseExtraction)
                qWarning() << "    * Drug Duplication";
            return toReturn;
        } else if (!d->m_DoNotWarnAtcDuplicates.contains(_id1)) {
            DrugsInteraction *ddi = 0;
            ddi = new ::DrugsInteraction(this);
            ddi->setValue(DrugsInteraction::DI_ATC1, _id1);
            ddi->setValue(DrugsInteraction::DI_ATC2, _id1);
            if (_id1 >= 200000) {
                // Inn duplication
                ddi->setValue(DrugsInteraction::DI_TypeId , "Z");
                ddi->setValue(DrugsInteraction::DI_RiskFr, tkTr(Trans::Constants::INN_DUPLICATION));
                ddi->setValue(DrugsInteraction::DI_RiskEn, Trans::Constants::INN_DUPLICATION);
            } else {
                // Inn Class duplication
                ddi->setValue(DrugsInteraction::DI_TypeId , "U");
                ddi->setValue(DrugsInteraction::DI_RiskFr, tkTr(Trans::Constants::CLASS_DUPLICATION));
                ddi->setValue(DrugsInteraction::DI_RiskEn, Trans::Constants::CLASS_DUPLICATION);
            }
            ddi->setValue(DrugsInteraction::DI_ReferencesLink, QCoreApplication::translate("DrugsBase", "FreeDiams Interactions Engine"));
            foreach(IDrug *drug, fromddi->drugs()) {
                ddi->addInteractingDrug(drug);
            }
            id2 = _id1;
            toReturn << ddi;
            if (WarnDatabaseExtraction)
                qWarning() << "    * ATC Duplication" << ddi->value(DrugsInteraction::DI_RiskFr).toString();
            return toReturn;
        } else {
            return toReturn;
        }
    }

    // else retreive INTERACTION from database
    // construct where clause
    QHashWhere where;
    where.insert(Constants::INTERACTIONS_ATC_ID1, QString("=%1").arg(_id1));
    where.insert(Constants::INTERACTIONS_ATC_ID2, QString("=%1").arg(id2));

    Utils::FieldList get;
    Utils::JoinList joins;
    Utils::FieldList conditions;
    get
            << Utils::Field(Constants::Table_IAKNOWLEDGE, Constants::IAKNOWLEDGE_RISK_MASTERLID)
            << Utils::Field(Constants::Table_IAKNOWLEDGE, Constants::IAKNOWLEDGE_MANAGEMENT_MASTERLID)
            << Utils::Field(Constants::Table_IAKNOWLEDGE, Constants::IAKNOWLEDGE_TYPE)
            << Utils::Field(Constants::Table_IAKNOWLEDGE, Constants::IAKNOWLEDGE_WWW)
            << Utils::Field(Constants::Table_INTERACTIONS, Constants::INTERACTIONS_IAID);
    joins
            << Utils::Join(Constants::Table_IA_IAK, Constants::IA_IAK_IAID, Constants::Table_INTERACTIONS, Constants::INTERACTIONS_IAID)
            << Utils::Join(Constants::Table_IAKNOWLEDGE, Constants::IAKNOWLEDGE_IAKID, Constants::Table_IA_IAK, Constants::IA_IAK_IAKID);
    conditions
            << Utils::Field(Constants::Table_INTERACTIONS, Constants::INTERACTIONS_ATC_ID1, QString("=%1").arg(_id1))
            << Utils::Field(Constants::Table_INTERACTIONS, Constants::INTERACTIONS_ATC_ID2, QString("=%1").arg(id2));
    // get interaction links
    QString req = drugsBase().select(get, joins, conditions);
    QSqlQuery query(req, DB);
    if (query.isActive()) {
        while (query.next()) {
            DrugsInteraction *ddi = 0;
            ddi = new DrugsInteraction(this);
            ddi->setValue(DrugsInteraction::DI_ATC1, _id1);
            ddi->setValue(DrugsInteraction::DI_ATC2, id2);
            ddi->setValue(DrugsInteraction::DI_RiskId, query.value(0));
            ddi->setValue(DrugsInteraction::DI_ManagementId, query.value(1));
            ddi->setValue(DrugsInteraction::DI_TypeId, query.value(2));
            ddi->setValue(DrugsInteraction::DI_ReferencesLink, query.value(3));
            ddi->setValue(DrugsInteraction::DI_Id, query.value(4));
            foreach(IDrug *drug, fromddi->drugs()) {
                ddi->addInteractingDrug(drug);
            }
            toReturn << ddi;
            if (WarnDatabaseExtraction) {
                qWarning() << "  * DDI: id" << ddi->value(DrugsInteraction::DI_Id).toString() << "ATC1" << ddi->value(DrugsInteraction::DI_ATC1).toString() << "ATC2" << ddi->value(DrugsInteraction::DI_ATC2).toString();
                qWarning() << "        " << "ATC1" << drugsBase().getAtcLabel(ddi->value(DrugsInteraction::DI_ATC1).toInt()) << "ATC2" << drugsBase().getAtcLabel(ddi->value(DrugsInteraction::DI_ATC2).toInt());
            }
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    // TODO: get interaction bibliography

    return toReturn;
}

QVector<IDrugInteraction *> DrugDrugInteractionEngine::getAllInteractionsFound()
{
    // if no interactions were found : return empty list
    QVector<IDrugInteraction *> toReturn;
    if (d->m_DDIFound.isEmpty() && d->m_FirstPassInteractions.isEmpty())
        return toReturn;

    QTime t;
    t.start();

    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
        }
    }

    // Retrieve from database
    foreach(IDrugInteraction *di, d->m_FirstPassInteractions) {
        toReturn << getInteractionsFromDatabase(di);
    }

//    QMap<int, int>::const_iterator i = d->m_DDIFound.constBegin();
//    while (i != d->m_DDIFound.constEnd()) {
//        toReturn << getInteractionsFromDatabase(i.key(), i.value());
//        ++i;
//    }

    // link DDI with IDrug
//    int id1, id2;
//    foreach(IDrugInteraction* di, toReturn) {
//        DrugsInteraction *ddi = static_cast<DrugsInteraction *>(di);
//        Q_ASSERT(ddi);
//        id1 = ddi->value(DrugsInteraction::DI_ATC1).toInt();
//        id2 = ddi->value(DrugsInteraction::DI_ATC2).toInt();

////        qWarning() << "DDIs" << id1 << id2 << ddi->value(DrugsInteraction::DI_Id).toInt();

//        // test all drugs in the list
//        foreach(IDrug *drg, d->m_TestedDrugs)  {
//            if (drg->allInnAndInteractingClassesIds().contains(id1) || drg->allInnAndInteractingClassesIds().contains(id2)) {
//                ddi->addInteractingDrug(drg);
//            }
//        }
//    }

    qSort(toReturn.begin(), toReturn.end(), IDrugInteraction::greaterThan);
    d->m_Interactions = toReturn;
    if (d->m_LogChrono)
        Utils::Log::logTimeElapsed(t, "DrugDrugInteractionEngine", QString("getAllInteractions: %1 drugs")
                                   .arg(d->m_TestedDrugs.count()));

    return toReturn;
}

QVector<IDrugInteractionAlert *> DrugDrugInteractionEngine::getAllAlerts(DrugInteractionResult *addToResult)
{
    QVector<IDrugInteractionAlert *> alerts;
    alerts << new Alert(addToResult, this);
    return alerts;
}

void DrugDrugInteractionEngine::setActive(bool activate)
{
    if (isActive()==activate)
        return;
    // update settings
    if (activate) {
        settings()->appendToValue(Constants::S_ACTIVATED_INTERACTION_ENGINES, Constants::DDI_ENGINE_UID);
    } else {
        QStringList l = settings()->value(Constants::S_ACTIVATED_INTERACTION_ENGINES).toStringList();
        l.removeAll(Constants::DDI_ENGINE_UID);
        settings()->setValue(Constants::S_ACTIVATED_INTERACTION_ENGINES, l);
    }
}
