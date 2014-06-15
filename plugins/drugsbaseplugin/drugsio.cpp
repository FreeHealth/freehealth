/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
/**
 * \class DrugsIO
 * \brief Manages the IO operations for the dosages and prescriptions
 * Read/Write prescription from/to XML files.\n
 * Create prints and print preview of prescriptions.\n
 * Manages drugs tokens.
 * \note Unit-test partially available (see: DrugsDB:Internal::DrugsBasePlugin)
*/

#include "drugsio.h"

#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/constants_html.h>
#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/protocolsbase.h>
#include <drugsbaseplugin/idrug.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/versionupdater.h>
#include <drugsbaseplugin/dailyschememodel.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/prescriptiontoken.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/idocumentprinter.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/ipadtools.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/messagesender.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>
#include <translationutils/trans_units.h>

#include <QApplication>
#include <QHash>
#include <QString>
#include <QDir>
#include <QDomDocument>

using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugBaseCore &core() {return DrugsDB::DrugBaseCore::instance();}
static inline DrugsDB::DrugsBase &drugsBase() {return core().drugsBase();}
static inline DrugsDB::ProtocolsBase &protocolsBase() {return core().protocolsBase();}
static inline DrugsDB::VersionUpdater &versionUpdater() {return core().versionUpdater();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPadTools *padTools() {return Core::ICore::instance()->padTools();}

namespace {

    const char *const XML_HEADER                           = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE FreeMedForms>\n";
    const char *const XML_ROOT_TAG                         = "FreeDiams";
    const char *const XML_DRUGS_DATABASE_NAME              = "DrugsDatabaseName";
    const char *const XML_VERSION                          = "version";
    const char *const XML_PRESCRIPTION_MAINTAG             = "Prescription";
    const char *const XML_PRESCRIPTION_ISTEXTUAL           = "IsTextual";
    // FIXME: unused ? const char *const XML_PRESCRIPTION_TEXTUALDRUGNAME     = "TextualDrugName";

    const char *const XML_DRUG_ROOT         = "Drug";
    const char *const XML_DRUG_ATTR_UID1    = "u1";
    const char *const XML_DRUG_ATTR_UID2    = "u2";
    const char *const XML_DRUG_ATTR_UID3    = "u3";
    const char *const XML_DRUG_ATTR_OLDUID  = "old";
    const char *const XML_DRUG_ATTR_DB      = "db";
    const char *const XML_DRUG_DENOMINATION = "DrugName";
    const char *const XML_DRUG_UUID_NAME    = "DrugUidName";
    const char *const XML_DRUG_FORM         = "DrugForm";
    const char *const XML_DRUG_ROUTE        = "DrugRoute";
    const char *const XML_DRUG_STRENGTH     = "DrugStrength";
    // FIXME: unused? const char *const XML_DRUG_COMPOSITION  = "DrugComponents";
    // Unused
    const char *const XML_DRUG_INNS         = "DrugINN";
    const char *const XML_DRUG_INNS_ATC     = "DrugINN_ATC";
    const char *const XML_DRUG_ATC          = "DrugATC";
    // FIXME: unused? const char *const XML_PRESCRIPTION_UID  = "PrescrUuid";

    const char *const XML_COMPOSITION                  = "Composition";
    const char *const XML_COMPOSITION_INN              = "inn";
    const char *const XML_COMPOSITION_ATC              = "atc";
    const char *const XML_COMPOSITION_FORM             = "form";
    const char *const XML_COMPOSITION_ROUTE            = "route";
    const char *const XML_COMPOSITION_STRENGTH         = "strength";
    const char *const XML_COMPOSITION_MOLECULAR        = "molecularName";
    const char *const XML_COMPOSITION_NATURE           = "nature";
    const char *const XML_COMPOSITION_NATURE_LK        = "natureLink";

    const char *const XML_PRESCRIPTION_ROOT                = "Prescription";
    const char *const XML_PRESCRIPTION_DOSAGE              = "Dose";
    const char *const XML_PRESCRIPTION_TESTONLY            = "OnlyForTest";
    const char *const XML_PRESCRIPTION_ID                  = "Id";
    const char *const XML_PRESCRIPTION_USEDDOSAGE          = "RefDosage";
    const char *const XML_PRESCRIPTION_CIP                 = "Pack_UID";
    const char *const XML_PRESCRIPTION_INTAKEFROM          = "IntakeFrom";
    const char *const XML_PRESCRIPTION_INTAKETO            = "IntakeTo";
    const char *const XML_PRESCRIPTION_INTAKESCHEME        = "IntakeScheme";
    const char *const XML_PRESCRIPTION_INTAKEFROMTO        = "IntakeFromTo";
    const char *const XML_PRESCRIPTION_INTAKEINTERVALTIME  = "IntakeIntervalTime";
    const char *const XML_PRESCRIPTION_INTAKEINTERVALSCHEME= "IntakeIntervalScheme";
//    const char *const XML_PRESCRIPTION_INTAKEFULLSTRING    = "IntakeFullString";
    const char *const XML_PRESCRIPTION_DURATIONFROM        = "DurationFrom";
    const char *const XML_PRESCRIPTION_DURATIONTO          = "DurationTo";
    const char *const XML_PRESCRIPTION_DURATIONSCHEME      = "DurationScheme";
    const char *const XML_PRESCRIPTION_DURATIONFROMTO      = "DurationFromTo";
    const char *const XML_PRESCRIPTION_PERIOD              = "Period";
    const char *const XML_PRESCRIPTION_PERIODSCHEME        = "PeriodScheme";
    const char *const XML_PRESCRIPTION_ROUTEID             = "RouteId";
    const char *const XML_PRESCRIPTION_DAILYSCHEME         = "Daily";
    const char *const XML_PRESCRIPTION_MEALSCHEME          = "MealTime";
    const char *const XML_PRESCRIPTION_NOTE                = "Note";
    const char *const XML_PRESCRIPTION_ISINN               = "INN";
    const char *const XML_PRESCRIPTION_SPECIFYFORM         = "SpecifyForm";
    const char *const XML_PRESCRIPTION_SPECIFYPRESCENTATION= "SpecifyPresentation";
    const char *const XML_PRESCRIPTION_ISALD               = "IsAld";
    const char *const XML_PRESCRIPTION_REFILL              = "Refill";
    const char *const XML_PRESCRIPTION_TOHTML              = "Html";

    const char *const XML_EXTRADATA_TAG                    = "ExtraDatas";
    const char *const XML_FULLPRESCRIPTION_TAG             = "FullPrescription";
    const char *const XML_DATEOFGENERATION_TAG             = "DateOfGeneration";
}  // namespace Anonymous

using namespace DrugsDB;
using namespace DrugsDB::Constants;
using namespace Trans::ConstantTranslations;

namespace DrugsDB {
namespace Internal {

class DrugsIOPrivate
{
public:
    DrugsIOPrivate(DrugsIO *parent) :
        _initialized(false),
        q(parent)
    {
    }

    ~DrugsIOPrivate()
    {
        // _tokens are deleted by the padTools()->tokenPool()
    }

    void populateXmlTags()
    {
        if (m_PrescriptionXmlTags.isEmpty()) {
            m_PrescriptionXmlTags.insert(Prescription::Id ,  XML_PRESCRIPTION_ID);
            m_PrescriptionXmlTags.insert(Prescription::UsedDosage , XML_PRESCRIPTION_USEDDOSAGE);
            m_PrescriptionXmlTags.insert(Prescription::IsTextualOnly , XML_PRESCRIPTION_ISTEXTUAL);
            m_PrescriptionXmlTags.insert(Prescription::Pack_UID , XML_PRESCRIPTION_CIP);
            m_PrescriptionXmlTags.insert(Prescription::OnlyForTest, XML_PRESCRIPTION_TESTONLY);
            m_PrescriptionXmlTags.insert(Prescription::IntakesFrom , XML_PRESCRIPTION_INTAKEFROM);
            m_PrescriptionXmlTags.insert(Prescription::IntakesTo, XML_PRESCRIPTION_INTAKETO);
            m_PrescriptionXmlTags.insert(Prescription::IntakesScheme, XML_PRESCRIPTION_INTAKESCHEME);
            m_PrescriptionXmlTags.insert(Prescription::IntakesUsesFromTo, XML_PRESCRIPTION_INTAKEFROMTO);
//            m_PrescriptionXmlTags.insert(Prescription::IntakesFullString, XML_PRESCRIPTION_INTAKEFULLSTRING);
            m_PrescriptionXmlTags.insert(Prescription::IntakesIntervalOfTime, XML_PRESCRIPTION_INTAKEINTERVALTIME);
            m_PrescriptionXmlTags.insert(Prescription::IntakesIntervalSchemeIndex, XML_PRESCRIPTION_INTAKEINTERVALSCHEME);
            m_PrescriptionXmlTags.insert(Prescription::DurationFrom, XML_PRESCRIPTION_DURATIONFROM);
            m_PrescriptionXmlTags.insert(Prescription::DurationTo, XML_PRESCRIPTION_DURATIONTO);
            m_PrescriptionXmlTags.insert(Prescription::DurationScheme, XML_PRESCRIPTION_DURATIONSCHEME);
            m_PrescriptionXmlTags.insert(Prescription::DurationUsesFromTo, XML_PRESCRIPTION_DURATIONFROMTO);
            m_PrescriptionXmlTags.insert(Prescription::Period, XML_PRESCRIPTION_PERIOD);
            m_PrescriptionXmlTags.insert(Prescription::PeriodScheme, XML_PRESCRIPTION_PERIODSCHEME);
            m_PrescriptionXmlTags.insert(Prescription::RouteId, XML_PRESCRIPTION_ROUTEID);
            m_PrescriptionXmlTags.insert(Prescription::SerializedDailyScheme, XML_PRESCRIPTION_DAILYSCHEME);
            m_PrescriptionXmlTags.insert(Prescription::MealTimeSchemeIndex, XML_PRESCRIPTION_MEALSCHEME);
            m_PrescriptionXmlTags.insert(Prescription::Note, XML_PRESCRIPTION_NOTE);
            m_PrescriptionXmlTags.insert(Prescription::IsINNPrescription, XML_PRESCRIPTION_ISINN);
            m_PrescriptionXmlTags.insert(Prescription::SpecifyForm, XML_PRESCRIPTION_SPECIFYFORM);
            m_PrescriptionXmlTags.insert(Prescription::SpecifyPresentation, XML_PRESCRIPTION_SPECIFYPRESCENTATION);
            m_PrescriptionXmlTags.insert(Prescription::IsALD, XML_PRESCRIPTION_ISALD);
            m_PrescriptionXmlTags.insert(Prescription::Refill, XML_PRESCRIPTION_REFILL);
            m_PrescriptionXmlTags.insert(Prescription::ToHtml, XML_PRESCRIPTION_TOHTML);
            m_PrescriptionXmlTags.insert(Drug::Denomination, XML_DRUG_DENOMINATION);
            m_PrescriptionXmlTags.insert(Drug::UidName, XML_DRUG_UUID_NAME);
            m_PrescriptionXmlTags.insert(Drug::Inns, XML_DRUG_INNS);
            m_PrescriptionXmlTags.insert(Drug::InnsATCcodes, XML_DRUG_INNS_ATC);
            m_PrescriptionXmlTags.insert(Drug::ATC, XML_DRUG_ATC);
            m_PrescriptionXmlTags.insert(Drug::Form, XML_DRUG_FORM);
            m_PrescriptionXmlTags.insert(Drug::Route, XML_DRUG_ROUTE);
            m_PrescriptionXmlTags.insert(Drug::GlobalStrength, XML_DRUG_STRENGTH);
        }
    }

    void populateTokens()
    {
#ifdef WITH_PAD
        // Create and register namespaces
        Core::TokenNamespace prescrNs("Prescription");
        prescrNs.setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        prescrNs.setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_PRESCRIPTION);
        prescrNs.setUntranslatedHelpText(DrugsDB::Constants::TOKEN_TOOLTIP_PRESCRIPTION);
        prescrNs.setUntranslatedTooltip(DrugsDB::Constants::TOKEN_TOOLTIP_PRESCRIPTION);

        Core::TokenNamespace prescrDrugNs("Drug");
        prescrDrugNs.setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        prescrDrugNs.setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_DRUG);

        Core::TokenNamespace prescrProtoNs("Protocol");
        prescrProtoNs.setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        prescrProtoNs.setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_PROTOCOL);

        prescrNs.addChild(prescrDrugNs);
        prescrNs.addChild(prescrProtoNs);
        if (padTools() && padTools()->tokenPool()) {
            padTools()->tokenPool()->registerNamespace(prescrNs);
        }

        Core::IToken *t = 0;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_DRUGNAME, Drug::Denomination);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_DRUGNAME);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_Q_FROM, Prescription::IntakesFrom);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_Q_FROM);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_Q_TO, Prescription::IntakesTo);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_Q_TO);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_Q_SCHEME, Prescription::IntakesScheme);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_Q_SCHEME);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_DISTRIB_DAILYSCHEME, -1);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_DISTRIB);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_PERIOD_VALUE, Prescription::Period);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_P_VALUE);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_PERIOD_SCHEME, Prescription::PeriodScheme);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_P_SCHEME);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_D_FROM, Prescription::DurationFrom);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_D_FROM);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_D_TO, Prescription::DurationTo);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_D_TO);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_D_SCHEME, Prescription::DurationScheme);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_D_SCHEME);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_ROUTE, Prescription::Route);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_ROUTE);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_REPEATED_DAILYSCHEME, -1);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_REPEAT);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_MININTERVAL_VALUE, Prescription::IntakesIntervalOfTime);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_MININTERV_VALUE);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_MININTERVAL_SCHEME, Prescription::IntakesIntervalSchemeIndex);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_MININTERV_SCHEME);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_MEAL, Prescription::MealTimeSchemeIndex);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_MEAL);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_REFILL, Prescription::Refill);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_REFILL);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_NOTE, Prescription::Note);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_NOTE);
        _tokens << t;

        // Full tokens
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_Q_FULL, Prescription::IntakesFullString);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_Q_FULL);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_D_FULL, Prescription::DurationFullString);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_D_FULL);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_PERIOD_FULL, Prescription::PeriodFullString);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_P_FULL);
        _tokens << t;
        t = new PrescriptionToken(Core::Constants::TOKEN_PRESC_MININTERVAL_FULL, Prescription::IntakesIntervalFullString);
        t->setTrContext(DrugsDB::Constants::DRUGSBASE_TR_CONTEXT);
        t->setUntranslatedHumanReadableName(DrugsDB::Constants::TOKEN_TEXT_MININTERV_FULL);
        _tokens << t;

        if (padTools() && padTools()->tokenPool()) {
            LOG_FOR(q, "Adding prescription tokens");
            padTools()->tokenPool()->addTokens(_tokens);
        }
#else
        // Clang warning
        Q_UNUSED(q);
#endif
    }

    /** \brief For the Xml transformation of the prescription, returns the xml tag for the mfDrugsConstants::Prescription \e row */
    QString xmlTagForPrescriptionRow(const int row)
    {
        Q_ASSERT(m_PrescriptionXmlTags.contains(row));
        return m_PrescriptionXmlTags.value(row);
    }

    /** \brief For the Xml transformation of the prescription, return the mfDrugsConstants::Prescription for the xml tag \e xmltag */
    int xmlTagToColumnIndex(const QString &xmltag)
    {
        return m_PrescriptionXmlTags.key(xmltag, -1);
    }

    void drugToXml(IDrug *drug, QDomDocument &doc, QDomElement &element)
    {
    //    QString xml;
    //    QDomDocument doc;
        // add drug tag + uids
        QDomElement drugRoot = doc.createElement(::XML_DRUG_ROOT);
        element.appendChild(drugRoot);
        drugRoot.setAttribute(::XML_DRUG_ATTR_UID1, drug->data(IDrug::Uid1).toString());
        drugRoot.setAttribute(::XML_DRUG_ATTR_UID2, drug->data(IDrug::Uid2).toString());
        drugRoot.setAttribute(::XML_DRUG_ATTR_UID3, drug->data(IDrug::Uid3).toString());
        drugRoot.setAttribute(::XML_DRUG_ATTR_DB, drug->data(IDrug::SourceName).toString());
        drugRoot.setAttribute(::XML_DRUG_ATTR_OLDUID, drug->data(IDrug::OldUid).toString());

        QDomElement form = doc.createElement(::XML_DRUG_FORM);
        QDomText text = doc.createTextNode(drug->forms().join(";"));
        form.appendChild(text);
        drugRoot.appendChild(form);

        QDomElement route = doc.createElement(::XML_DRUG_ROUTE);
        text = doc.createTextNode(drug->routes().join(";"));
        route.appendChild(text);
        drugRoot.appendChild(route);

        QDomElement strength = doc.createElement(::XML_DRUG_STRENGTH);
        if (!drug->strength().isEmpty()) {
            text = doc.createTextNode(drug->strength());
            strength.appendChild(text);
        }
        drugRoot.appendChild(strength);

        QDomElement name = doc.createElement(::XML_DRUG_DENOMINATION);
        text = doc.createTextNode(drug->brandName());
        name.appendChild(text);
        drugRoot.appendChild(name);

        QDomElement uidname = doc.createElement(::XML_DRUG_UUID_NAME);
        text = doc.createTextNode(drug->drugUidName());
        uidname.appendChild(text);
        drugRoot.appendChild(uidname);

        foreach(IComponent *compo, drug->components()) {
            QDomElement c = doc.createElement(::XML_COMPOSITION);
            drugRoot.appendChild(c);
            c.setAttribute(::XML_COMPOSITION_INN, compo->innName());
//            c.setAttribute(::XML_COMPOSITION_ATC, compo->data(IComponent::MainAtcCode).toString());
            c.setAttribute(::XML_COMPOSITION_ATC, compo->data(IComponent::AtcId).toString());
            c.setAttribute(::XML_COMPOSITION_FORM, compo->form());
            c.setAttribute(::XML_COMPOSITION_ROUTE, drug->routes().join(";"));
            c.setAttribute(::XML_COMPOSITION_STRENGTH, compo->dosage());
            c.setAttribute(::XML_COMPOSITION_MOLECULAR, compo->moleculeName());
            c.setAttribute(::XML_COMPOSITION_NATURE, compo->nature());
            c.setAttribute(::XML_COMPOSITION_NATURE_LK, compo->lkNature());
        }
    }

    void drugPrescriptionToXml(IDrug *drug, QDomDocument &doc, QDomElement &fullPres)
    {
        QList<int> keysToSave;
        keysToSave
                << Prescription::IsTextualOnly
                << Prescription::UsedDosage
                << Prescription::Pack_UID
                << Prescription::OnlyForTest
                << Prescription::IntakesFrom
                << Prescription::IntakesTo
                << Prescription::IntakesScheme
                << Prescription::IntakesUsesFromTo
                << Prescription::IntakesFullString
                << Prescription::DurationFrom
                << Prescription::DurationTo
                << Prescription::DurationScheme
                << Prescription::DurationUsesFromTo
                << Prescription::Period
                << Prescription::PeriodScheme
                << Prescription::RouteId
                << Prescription::SerializedDailyScheme
                << Prescription::MealTimeSchemeIndex
                << Prescription::IntakesIntervalOfTime
                << Prescription::IntakesIntervalSchemeIndex
                << Prescription::Note
                << Prescription::IsINNPrescription
                << Prescription::SpecifyForm
                << Prescription::SpecifyPresentation
                << Prescription::IsALD
                << Prescription::Refill
                ;

        // Process each prescribed drugs
        QDomElement prescr = doc.createElement(::XML_PRESCRIPTION_ROOT);
        fullPres.appendChild(prescr);

        // add drug XML
        drugToXml(drug, doc, prescr);

        QDomElement item = doc.createElement(::XML_PRESCRIPTION_DOSAGE);
        prescr.appendChild(item);

        // add prescription XML
        for(int i = Prescription::Id; i < Prescription::MaxParam; ++i) {
            const QString &tagName = m_PrescriptionXmlTags.value(i);
            if (tagName.isEmpty()) {
                continue;
            }
            const QVariant &value = drug->prescriptionValue(i);
            if (value.isNull())
                continue;
//            QDomElement element = doc.createElement(tagName);
//            prescr.appendChild(element);
            if (value.type() == QVariant::StringList) {
//                QDomText text = doc.createTextNode(value.toStringList().join(";"));
//                element.appendChild(text);
                item.setAttribute(tagName, value.toStringList().join(";"));
            } else {
//                QDomText text = doc.createTextNode(value.toString());
//                element.appendChild(text);
                item.setAttribute(tagName, value.toString());
            }

    //        if (drugModel->index(i, Prescription::OnlyForTest).data().toBool()) {
    //            forXml.insert(instance()->d->xmlTagForPrescriptionRow(Prescription::OnlyForTest), "true");
    //            forXml.insert(instance()->d->xmlTagForPrescriptionRow(Drug::Denomination), drugModel->index(i, Drug::Denomination).data().toString());
    //            forXml.insert(instance()->d->xmlTagForPrescriptionRow(Drug::Form), drugModel->index(i, Drug::Form).data().toString());
    //            forXml.insert(instance()->d->xmlTagForPrescriptionRow(Drug::Route), drugModel->index(i, Drug::Route).data().toString());
    //            forXml.insert(instance()->d->xmlTagForPrescriptionRow(Drug::GlobalStrength), drugModel->index(i, Drug::GlobalStrength).data().toString());
    //        } else {
    //            foreach(int k, keysToSave) {
    //                if (drugModel->index(i, k).data().type() == QVariant::StringList) {
    //                    forXml.insert(instance()->d->xmlTagForPrescriptionRow(k), drugModel->index(i, k).data().toStringList().join(";"));
    //                } else {
    //                    forXml.insert(instance()->d->xmlTagForPrescriptionRow(k), drugModel->index(i, k).data().toString());
    //                }
    //            }
    //        }
    //        if (drugModel->index(i, Prescription::IsTextualOnly).data().toBool()) {
    //            forXml.insert(XML_PRESCRIPTION_TEXTUALDRUGNAME,
    //                          drugModel->index(i, Drug::Denomination).data().toString());
    //        }
    //        xmldPrescription += Utils::createXml(XML_PRESCRIPTION_MAINTAG, forXml);
    //        forXml.clear();

    //        // Insert composition
    //        drug = drugModel->getDrug(drugModel->index(i, Drug::DrugId).data());
    //        //        Q_ASSERT(drug);
    //        if (drug) {
    //            // Process drugs composition
    //            QString tmp = drug->compositionToXml();
    //            int index = xmldPrescription.lastIndexOf(XML_PRESCRIPTION_MAINTAG) - 2;
    //            xmldPrescription.insert(index, tmp);
    //        }

        }

        // Add full prescription tag and version
    //    xmldPrescription.prepend(QString("<%1 %2=\"%3\">\n")
    //                             .arg(XML_FULLPRESCRIPTION_TAG)
    //                             .arg(XML_VERSION).arg(VersionUpdater::instance()->lastXmlIOVersion()));

        // Close Full prescription
    //    xmldPrescription.append(QString("</%1>\n").arg(XML_FULLPRESCRIPTION_TAG));

        // Add drugsBase information
        QString dbName;
        QString dbInfoAttribs;
        if (drugsBase().actualDatabaseInformation()) {
            dbName = drugsBase().actualDatabaseInformation()->identifier;
            QString t = drugsBase().actualDatabaseInformation()->version;
            dbInfoAttribs += QString("version=\"%1\" ").arg(t.replace("\"","'"));
            t = drugsBase().actualDatabaseInformation()->compatVersion;
            dbInfoAttribs += QString("compatWithFreeDiamsVersion=\"%1\" ").arg(t.replace("\"","'"));
            t = drugsBase().actualDatabaseInformation()->complementaryWebsite;
            dbInfoAttribs += QString("complementaryWebSite=\"%1\" ").arg(t.replace("\"","'"));
            dbInfoAttribs += QString("date=\"%1\" ").arg(drugsBase().actualDatabaseInformation()->date.toString(Qt::ISODate));
            t = drugsBase().actualDatabaseInformation()->provider;
            dbInfoAttribs += QString("provider=\"%1\" ").arg(t.replace("\"","'"));
            t = drugsBase().actualDatabaseInformation()->weblink;
            dbInfoAttribs += QString("webLink=\"%1\" ").arg(t.replace("\"","'"));
            t = drugsBase().actualDatabaseInformation()->packUidName;
            dbInfoAttribs += QString("packUidName=\"%1\" ").arg(t.replace("\"","'"));
            t = drugsBase().actualDatabaseInformation()->drugsUidName;
            dbInfoAttribs += QString("drugUidName=\"%1\" ").arg(t.replace("\"","'"));
        } else {
            dbName = Constants::DB_DEFAULT_IDENTIFIANT;
        }
    //    xmldPrescription.prepend(QString("<%1 %2>%3</%1>\n")
    //                             .arg(XML_DRUGS_DATABASE_NAME)
    //                             .arg(dbInfoAttribs)
    //                             .arg(dbName));

        // Add the date of generation
    //    xmldPrescription.prepend(QString("<%1>%2</%1>\n").arg(XML_DATEOFGENERATION_TAG).arg(QDateTime::currentDateTime().toString(Qt::ISODate)));

        // Add the main root node and extraData
    //    xmldPrescription.prepend(QString("<%1>\n").arg(XML_ROOT_TAG));
    //    xmldPrescription.append(xmlExtraData);
    //    xmldPrescription.append(QString("</%1>\n").arg(XML_ROOT_TAG));

        // Add the version and the FullPrescription tags
    //    xmldPrescription.prepend(QString("%1\n").arg(XML_HEADER));
    }

    IDrug *readDrug(const QDomElement &drugElement)
    {
        IDrug *readingDrug = 0;
        // test uuids
        const QString &uid1 = drugElement.attribute(::XML_DRUG_ATTR_UID1);
        const QString &uid2 = drugElement.attribute(::XML_DRUG_ATTR_UID2);
        const QString &uid3 = drugElement.attribute(::XML_DRUG_ATTR_UID3);
        const QString &olduid = drugElement.attribute(::XML_DRUG_ATTR_OLDUID);
        const QString &db = drugElement.attribute(::XML_DRUG_ATTR_DB);
        if ((uid1.isEmpty() || uid1 == "-1") &&
            (uid2.isEmpty() || uid2 == "-1") &&
            (uid3.isEmpty() || uid3 == "-1") &&
            (olduid.isEmpty() || olduid == "-1")) {
            // is textual
            ITextualDrug *drug = new ITextualDrug;
            drug->setDenomination(drugElement.firstChildElement(::XML_DRUG_DENOMINATION).text());
            drug->setDataFromDb(IDrug::Forms, drugElement.firstChildElement(::XML_DRUG_FORM).text());
            drug->setDataFromDb(IDrug::Strength, drugElement.firstChildElement(::XML_DRUG_STRENGTH).text());
            // add routes and composition
            return drug;
        }

        // Get drug from database
        // --> using oldUid only
        if ((uid1.isEmpty() || uid1 == "-1") &&
            (uid2.isEmpty() || uid2 == "-1") &&
            (uid3.isEmpty() || uid3 == "-1") &&
            (!olduid.isEmpty() || olduid != "-1")) {
            readingDrug = drugsBase().getDrugByOldUid(olduid, db);
            if (readingDrug)
                return readingDrug;
        }
        // --> using all uids
        readingDrug = drugsBase().getDrugByUID(uid1, uid2, uid3, olduid, db);
        if (readingDrug)
            return readingDrug;

        // Error create a virtual drug
        ITextualDrug *td = new ITextualDrug;
        td->setDenomination(drugElement.firstChildElement(::XML_DRUG_DENOMINATION).text());
        td->setDataFromDb(IDrug::Forms, drugElement.firstChildElement(::XML_DRUG_FORM).text());
        td->setDataFromDb(IDrug::Strength, drugElement.firstChildElement(::XML_DRUG_STRENGTH).text());
        td->setDataFromDb(IDrug::Routes, drugElement.firstChildElement(::XML_DRUG_ROUTE).text());
        // add routes, composition, uids
        // TODO: code here = manage error msg
        return td;
    }

    void readDose(IDrug *drug, const QDomElement &doseElement)
    {
        if (!drug) {
            return;
        }
        QDomNamedNodeMap attrib = doseElement.attributes();
        for(int i = 0; i < attrib.count(); ++i) {
            QDomNode at = attrib.item(i);
            int column = xmlTagToColumnIndex(at.nodeName());
            if (column >= Prescription::Id && column < Prescription::MaxParam) {
                drug->setPrescriptionValue(column, at.nodeValue());
            }
        }
        // TODO: code here = manage dose in note if IDrug is pure textual
    }

public:
    bool _initialized;
    Utils::MessageSender m_Sender;  /*!< \brief Message sender instance */
    QHash<QString,QString> m_Data;   /*!< \brief Dosages to transmit : key == uuid, value == xml'd dosage */
    QHash<int,QString> m_PrescriptionXmlTags;
    QVector<Core::IToken *> _tokens;

private:
    DrugsIO *q;
};
}  // namespace Internal
}  // namespace DrugsDB

/** \brief Private constructor */
DrugsIO::DrugsIO(QObject *parent) :
    QObject(parent),
    d(0)
{
    setObjectName("DrugsIO");
    d = new Internal::DrugsIOPrivate(this);
}

/** \brief Destructor */
DrugsIO::~DrugsIO()
{
    if (d) {
        delete d;
        d=0;
    }
}

/**
 * Initialize the object:
 *   - prepare the XML transformation
 *   - create the tokens
 */
bool DrugsIO::initialize()
{
    if (d->_initialized)
        return true;
    d->populateXmlTags();
    d->populateTokens();
    d->_initialized = true;
    return true;
}

/** Returns the initialization state */
bool DrugsIO::isInitialized() const
{
    return d->_initialized;
}

/**
  \brief Start the dosage transmission over the web.
  \todo Manage user's name
*/
bool DrugsIO::startsDosageTransmission()
{
    connect(&d->m_Sender, SIGNAL(sent()), this, SLOT(dosageTransmissionDone()));
    d->m_Data = protocolsBase().getDosageToTransmit();
    if (d->m_Data.count()==0) {
        return false;
    }
    QStringList list = d->m_Data.values();
    d->m_Sender.setMessage(list.join("\n\n"));
    d->m_Sender.setUser(qApp->applicationName() + " - " + qApp->applicationVersion());
    d->m_Sender.setTypeOfMessage(Utils::MessageSender::DosageTransmission);
    d->m_Sender.postMessage();
    return true;
}

/**
  \brief Slot called when the dosage transmission is done.
  Marks all dosage as transmitted into the database.
  \sa DrugsDB::ProtocolsBase::markAllDosageTransmitted()
*/
void DrugsIO::dosageTransmissionDone()
{
    if (d->m_Sender.resultMessage().contains("OK")) {
        LOG(tr("Dosages transmitted."));
        protocolsBase().markAllDosageTransmitted(d->m_Data.keys());
    } else {
        LOG_ERROR(tr("Dosage not successfully transmitted"));
    }
    d->m_Data.clear();
    Q_EMIT transmissionDone();
}

/** \brief Return the dosage sending state */
bool DrugsIO::isSendingDosage()
{
    return d->m_Sender.isSending();
}

/**
 * \brief Populate a DrugsDB::DrugsModel using the XML prescription content \e xmlContent with the options \e loader.
 * You can add to or replace the actual prescription using the enumerator DrugsIO::Loader \e loader.\n
 */
bool DrugsIO::prescriptionFromXml(DrugsDB::DrugsModel *drugModel, const QString &xmlContent, Loader loader)
{
    Q_ASSERT(drugModel);
    if (xmlContent.isEmpty())
        return false;

    QString xml = xmlContent;

    QTime time;
    time.start();

    // check prescription encoding version && update XML content if needed
    bool needUpdate = (!versionUpdater().isXmlIOUpToDate(xmlContent));
    QString version;
    if (needUpdate) {
        version = versionUpdater().xmlVersion(xmlContent);
        LOG("Reading old prescription file: version " + version);
        xml = versionUpdater().updateXmlIOContent(xmlContent);
    }

    // Read the XML file using QDomDocument
    // Prescription file format == Version 0.5.0
    QDomDocument doc;
    QString error;
    int line = -1;
    int column = -1;
    if (!doc.setContent(xml, &error, &line, &column)) {
        LOG_ERROR(tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(line).arg(column).arg(error));
        Utils::warningMessageBox(tr("You are trying to read a corrupted XML file."),
                                 tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(line).arg(column).arg(error));
        return false;
    }

    // TODO: XML: check document type: <!DOCTYPE name>
    QDomElement root = doc.firstChildElement(XML_ROOT_TAG);

    // Check if the drugs database correspond to the actual one
    QString xmlDbName = DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT;
    QDomElement drugsDb = root.firstChildElement(::XML_DRUGS_DATABASE_NAME);
    if (!drugsDb.isNull()) {
        xmlDbName = drugsDb.text();
    }

    // retreive the prescription (inside the XML_FULLPRESCRIPTION_TAG tags)
    QDomElement fullPrescription = root.firstChildElement(XML_FULLPRESCRIPTION_TAG);
    if (fullPrescription.isNull()) {
        LOG_ERROR_FOR("DrugsIO", tr("Unable to load XML prescription: tag %1 is missing").arg(XML_FULLPRESCRIPTION_TAG));
        Utils::warningMessageBox(tr("You are trying to read a corrupted XML file."),
                                 tr("Missing tag: %1").arg(XML_FULLPRESCRIPTION_TAG));
        return false;
    }

    // clear model
    Q_ASSERT(drugModel);
    if (loader==ReplacePrescription)
        drugModel->clearDrugsList();

    // Read prescription itself
    QVector<IDrug *> drugs;
    QList<int> rowsToUpdate;
    QString errorMsg;
    QDomElement prescr = fullPrescription.firstChildElement(XML_PRESCRIPTION_MAINTAG);

    while (!prescr.isNull()) {
        QDomElement item = prescr.firstChildElement(::XML_DRUG_ROOT);
        IDrug *readingDrug = d->readDrug(item);
        item = prescr.firstChildElement(::XML_PRESCRIPTION_DOSAGE);
        d->readDose(readingDrug, item);
        drugs << readingDrug;

        // check Model Updaters
//        if (needUpdate) {
//            rowsToUpdate.append(row);
//        }

        prescr = prescr.nextSiblingElement(XML_PRESCRIPTION_MAINTAG);
    }

    // Feed model with drugs
    drugModel->addDrugs(drugs, false);

    if ((needUpdate) && (!version.isEmpty())){
        versionUpdater().updateXmlIOModel(version, drugModel, rowsToUpdate);
    }

    if (!errorMsg.isEmpty()) {
        errorMsg.prepend(tr("Interaction checking will not take these drugs into account.\n"));
        Utils::warningMessageBox(tr("FreeDiams encountered errors while reading the XML prescription."), errorMsg);
    }

    // check interaction, emit final signal from model for views to update
    drugModel->checkInteractions();
    Q_EMIT drugModel->numberOfRowsChanged();

    // small debug information
    LOG(tr("Xml prescription successfully read."));
    return true;
}

/**
 * \brief Load a prescription file \e fileName to the DrugsDB::DrugsModel \e m.
 * You can add to or replace the actual prescription using the enumerator DrugsIO::Loader \e loader.\n
 * The \e extraData receives the extracted extra data from the loaded prescription file.
 * \sa savePrescription()
*/
bool DrugsIO::loadPrescription(DrugsDB::DrugsModel *drugModel, const QString &fileName, QHash<QString,QString> &extraData, Loader loader )
{
    Q_ASSERT(drugModel);
    QString extras;
    if (!loadPrescription(drugModel, fileName, extras, loader))
        return false;
    Utils::readXml(extras, XML_EXTRADATA_TAG, extraData, false);
    return true;
}

/**
 * \brief Load a prescription file \e fileName to the DrugsDB::DrugsModel \e m.
 * You can add to or replace the actual prescription using the enumerator DrugsIO::Loader \e loader.\n
 * The \e xmlExtraData receives the extracted extra data from the loaded prescription file.
 * \sa savePrescription()
*/
bool DrugsIO::loadPrescription(DrugsDB::DrugsModel *drugModel, const QString &fileName, QString &xmlExtraData, Loader loader )
{
    Q_ASSERT(drugModel);
    if (fileName.isEmpty()) {
        LOG_ERROR_FOR("DrugsIO", tr("No file name passed to load prescription"));
        return false;
    }
    QFileInfo info(fileName);
    if (info.isRelative())
        info.setFile(qApp->applicationDirPath() + QDir::separator() + fileName);

    if (!info.exists()) {
        LOG_ERROR_FOR("DrugsIO", tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(info.absoluteFilePath()));
        return false;
    }
    if (!info.isReadable()) {
        LOG_ERROR_FOR("DrugsIO", tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(info.absoluteFilePath()));
        return false;
    }
    xmlExtraData.clear();
    QString xml = Utils::readTextFile(info.absoluteFilePath());

    // retreive prescription
    prescriptionFromXml(drugModel, xml,loader);

    // get extraData
    QString start = QString("<%1>").arg(XML_EXTRADATA_TAG);
    QString finish = QString("</%1>").arg(XML_EXTRADATA_TAG);
    int begin = xml.indexOf(start) + start.length();
    int end = xml.indexOf(finish, begin);
    if (begin==-1 || end==-1) {
        return true;
    }
    xmlExtraData = xml.mid( begin, end - begin);
    drugModel->resetModel();
    return true;
}

/**
 * \brief Transform a prescription row (one drug) to a readable output.
 * Item is translated to HTML using the token manager and
 * the drug posologic sentence passed as param (\e mask). If the \e mask is
 * empty, a new one is extracted from the user settings (drug posologic sentence plaintext or HTML). \n
 * You can choose to output an HTML string or a plain text string \e toHtml.\n
*/
QString DrugsIO::getDrugPrescription(DrugsDB::DrugsModel *model, const int drugRow, bool toHtml, const QString &mask)
{
    QString tmp;
    if (mask.isEmpty()) {
        if (toHtml) {
            tmp = settings()->value(Constants::S_PRESCRIPTIONFORMATTING_HTML).toString();
        } else {
            tmp = settings()->value(Constants::S_PRESCRIPTIONFORMATTING_PLAIN).toString();
        }
    } else {
        tmp = mask;
    }

#ifdef WITH_PAD
    PrescriptionToken::setPrescriptionModel(model);
    PrescriptionToken::setPrescriptionModelRow(drugRow);
    if (toHtml) {
        return padTools()->processHtml(tmp);
    }
    return padTools()->processPlainText(tmp);
#else
    QHash<QString, QString> tokens_value;
    tokens_value.insert("DRUG", QString());
    tokens_value.insert("NOTE", QString());
    tokens_value.insert("Q_FROM", QString());
    tokens_value.insert("Q_TO", QString());
    tokens_value.insert("Q_SCHEME", QString());
    tokens_value.insert("D_FROM", QString());
    tokens_value.insert("D_TO", QString());
    tokens_value.insert("D_SCHEME", QString());
    tokens_value.insert("PERIOD_SCHEME", QString());
    tokens_value.insert("PERIOD", QString());
    tokens_value.insert("MEAL", QString());
    tokens_value.insert("REPEATED_DAILY_SCHEME", QString());
    tokens_value.insert("DISTRIBUTED_DAILY_SCHEME", QString());
    tokens_value.insert("DAILY_SCHEME", QString());
    tokens_value.insert("MIN_INTERVAL", QString());
    tokens_value.insert("ROUTE", QString());
    tokens_value.insert("REFILL", QString());

    if (!IN_RANGE_STRICT_MAX(drugRow, 0, model->drugsList().count())) {
        LOG_ERROR("row > model list count");
        return QString::null;
    }
    IDrug *drug = model->drugsList().at(drugRow);

    // Manage Textual drugs only
    if (drug->prescriptionValue(Constants::Prescription::IsTextualOnly).toBool()) {
        if (toHtml) {
            tokens_value["DRUG"] = drug->brandName().replace("\n","<br />");
            tokens_value["NOTE"] = drug->prescriptionValue(Constants::Prescription::Note).toString().replace("\n","<br />");
        } else {
            tokens_value["DRUG"] = drug->brandName();
            tokens_value["NOTE"] = drug->prescriptionValue(Constants::Prescription::Note).toString();
        }
        Utils::replaceTokens(tmp, tokens_value);
        return tmp;
    }

    // Manage full prescriptions
    if (drug->prescriptionValue(Constants::Prescription::IsINNPrescription).toBool()) {
        tokens_value["DRUG"] = drug->innComposition() + " [" + tkTr(Trans::Constants::INN) + "]";
    } else {
        tokens_value["DRUG"] =  drug->brandName();
    }

    if (drug->prescriptionValue(Constants::Prescription::IntakesFrom).toDouble()) {
        tokens_value["Q_FROM"] = QString::number(drug->prescriptionValue(Constants::Prescription::IntakesFrom).toDouble());
        if (drug->prescriptionValue(Constants::Prescription::IntakesUsesFromTo).toBool())
            tokens_value["Q_TO"] = QString::number(drug->prescriptionValue(Constants::Prescription::IntakesTo).toDouble());

        tokens_value["Q_SCHEME"] = drug->prescriptionValue(Constants::Prescription::IntakesScheme).toString();
    } else {
        tokens_value["Q_FROM"] = tokens_value["Q_TO"] = tokens_value["Q_SCHEME"] = "";
    }

    // Manage Daily Scheme See DailySchemeModel::setSerializedContent
    DrugsDB::DailySchemeModel *day = new DrugsDB::DailySchemeModel;
    day->setSerializedContent(drug->prescriptionValue(Constants::Prescription::SerializedDailyScheme).toString());
    tokens_value["REPEATED_DAILY_SCHEME"] = day->humanReadableRepeatedDailyScheme();
    tokens_value["DISTRIBUTED_DAILY_SCHEME"] = day->humanReadableDistributedDailyScheme();
    delete day;
    if (tokens_value.value("REPEATED_DAILY_SCHEME").isEmpty()) {
        tokens_value["DAILY_SCHEME"] = tokens_value.value("DISTRIBUTED_DAILY_SCHEME");
    } else {
        tokens_value["DAILY_SCHEME"] = tokens_value.value("REPEATED_DAILY_SCHEME");
    }

    // Duration
    if (drug->prescriptionValue(Constants::Prescription::DurationFrom).toDouble()) {
        tokens_value["D_FROM"] = QString::number(drug->prescriptionValue(Constants::Prescription::DurationFrom).toDouble());
        if (drug->prescriptionValue(Constants::Prescription::DurationUsesFromTo).toBool())
            tokens_value["D_TO"] = QString::number(drug->prescriptionValue(Constants::Prescription::DurationTo).toDouble());

        // Manage plurial form
        tokens_value["PERIOD_SCHEME"] = drug->prescriptionValue(Constants::Prescription::PeriodScheme).toString();
        tokens_value["D_SCHEME"] = drug->prescriptionValue(Constants::Prescription::DurationScheme).toString();
        int max = qMax(drug->prescriptionValue(Constants::Prescription::DurationFrom).toDouble(), drug->prescriptionValue(Constants::Prescription::DurationTo).toDouble());
        if (periods().contains(tokens_value["D_SCHEME"])) {
            tokens_value["D_SCHEME"] = periodPlurialForm(periods().indexOf(tokens_value["D_SCHEME"]), max, tokens_value["D_SCHEME"]);
        }
        max = drug->prescriptionValue(Constants::Prescription::Period).toDouble();
        if (max==1 && QLocale().name().left(2)=="fr")
            ++max;
       if (periods().contains(tokens_value["PERIOD_SCHEME"])) {
            tokens_value["PERIOD_SCHEME"] = periodPlurialForm(periods().indexOf(tokens_value["PERIOD_SCHEME"]), max, tokens_value["PERIOD_SCHEME"]);
        }
    } else {
        tokens_value["PERIOD_SCHEME"] = tokens_value["D_FROM"] = tokens_value["D_TO"] = tokens_value["D_SCHEME"] = "";
    }
    // Meal
    tokens_value["MEAL"] = Trans::ConstantTranslations::mealTime(drug->prescriptionValue(Constants::Prescription::MealTimeSchemeIndex).toInt());
    QString tmp2 = drug->prescriptionValue(Constants::Prescription::Period).toString();

    // Period && note
    // TODO: provide a better management of 'EACH DAY__S__' here .. \sa Translation::periodPlurialForm()
    // Period management of 'EACH DAY__S'
    if (tmp2 == "1") {
        tmp2.clear();
    }
    tokens_value["PERIOD"] = tmp2;

    if (toHtml) {
        tokens_value["NOTE"] = drug->prescriptionValue(Constants::Prescription::Note).toString().replace("\n","<br />");
    } else {
        tokens_value["NOTE"] = drug->prescriptionValue(Constants::Prescription::Note).toString();
    }

    // Min interval
    const QVariant &interval = drug->prescriptionValue(Constants::Prescription::IntakesIntervalOfTime);
    const QVariant &intervalScheme = drug->prescriptionValue(Constants::Prescription::IntakesIntervalSchemeIndex);
    if ((!interval.isNull() && !intervalScheme.isNull()) &&
        interval.toInt() > 0) {
        tokens_value["MIN_INTERVAL"] = interval.toString() + " " + periodPlurialForm(intervalScheme.toInt(), interval.toInt());
    }

    // Route
    tokens_value["ROUTE"] = drug->prescriptionValue(Constants::Prescription::Route).toString();

    // Refill
    // FIXME: correcty manages plurial translations
    int refill = drug->prescriptionValue(Constants::Prescription::Refill).toInt();
    if (refill > 0) {
        if (refill > 1) {
            QString tmp = tkTr(Trans::Constants::REFILL_1_TIMES).arg(refill);
            tmp = tmp.remove("(").remove(")");
            tokens_value["REFILL"] = tmp;
        } else if (refill == 1) {
            QString tmp = tkTr(Trans::Constants::REFILL_1_TIMES).arg(refill);
            int begin = tmp.indexOf("(");
            if (begin > 0) {
                int end = tmp.indexOf(")", begin);
                if (end)
                    tmp = tmp.remove(begin, end-begin);
            }
            tokens_value["REFILL"] = tmp;
        }
    }
    Utils::replaceTokens(tmp, tokens_value);
    if (toHtml)
        tmp = Utils::htmlReplaceAccents(tmp);
    return tmp;
#endif
}

/**
 * \brief Transforms a prescription from a DrugsDB::DrugsModel to and XML prescription content, with or without extra-xml content \e xmlExtraData.
*/
QString DrugsIO::prescriptionToXml(DrugsDB::DrugsModel *drugModel, const QString &xmlExtraData)
{
    Q_ASSERT(drugModel);
    if (!drugModel->testingDrugsAreVisible()) {
        bool yes = Utils::yesNoMessageBox(tr("Save test only drugs too?"),
                                  tr("Drugs added for testing only are actually hidden in this prescription.\n"
                                     "Do you want to add them to the file?"),
                                  tr("Answering 'no' will cause definitive lost of test only drugs when "
                                     "reloading this file."));
        if (yes)
            drugModel->showTestingDrugs(true);
    }

    // Create the XML document
    QDomDocument doc;
    doc.setContent(QString(::XML_HEADER));
    QDomElement root = doc.createElement(::XML_ROOT_TAG);
    doc.appendChild(root);

    // Add date of generation
    QDomElement date = doc.createElement(::XML_DATEOFGENERATION_TAG);
    root.appendChild(date);
    date.setAttribute(::XML_VERSION, QDateTime::currentDateTime().toString(Qt::ISODate));

    // Add full prescription
    QDomElement fullPres = doc.createElement(::XML_FULLPRESCRIPTION_TAG);
    root.appendChild(fullPres);
    fullPres.setAttribute(::XML_VERSION, versionUpdater().lastXmlIOVersion());
    const QList<IDrug *> &drugs = drugModel->drugsList();
    for(int i=0; i < drugs.count(); ++i) {
        IDrug *drug = drugs.at(i);
        d->drugPrescriptionToXml(drug, doc, fullPres);
    }

    // Add extradata
    if (!xmlExtraData.isEmpty()) {
        QString xml = doc.toString(2);
        int index = xml.lastIndexOf(QString("</%1>").arg(XML_ROOT_TAG));
        Q_ASSERT(index != -1);
        xml.insert(index, "\n" + xmlExtraData + "\n");
        // TODO: check the validity of the extra xml content before breaking the whole document
        doc.setContent(xml);
    }

    // Return the xml document
    return doc.toString(2);
}

/**
 * \brief Save the Drugs::DrugsModel's prescription into a XML file.
 * You can add \e extraData to the xml. \e extraData must be xml'd.\n
 * If \e toFileName is empty, user is prompted to select a filename.
*/
bool DrugsIO::savePrescription(DrugsDB::DrugsModel *model, const QHash<QString,QString> &extraData, const QString &toFileName)
{
    Q_ASSERT(model);
    QString extra;
    if (!extraData.isEmpty()) {
        extra = Utils::createXml(XML_EXTRADATA_TAG, extraData);
    }
    QString xmldPrescription = prescriptionToXml(model, extra);
    if (toFileName.isEmpty())
        return Utils::saveStringToFile(xmldPrescription,
                                      QDir::homePath() + "/prescription.di",
                                      tr(Core::Constants::FREEDIAMS_FILEFILTER));
    else
        return Utils::saveStringToFile(xmldPrescription, toFileName, Utils::Overwrite, Utils::DontWarnUser);
}

/**
 * \brief Save the DrugsModel's prescription into a XML file.
 * You can add \e extraXmlData to the xml. \e extraData must be xml'd.\n
 * If \e toFileName is empty, user is prompted to select a filename.
*/
bool DrugsIO::savePrescription(DrugsDB::DrugsModel *model, const QString &extraData, const QString &toFileName)
{
    Q_ASSERT(model);
    QString extra;
    if (!extraData.isEmpty()) {
        extra = QString("\n<%1>\n").arg(XML_EXTRADATA_TAG);
        extra += extraData;
        extra += QString("\n</%1>\n").arg(XML_EXTRADATA_TAG);
    }
    QString xmldPrescription = prescriptionToXml(model, extra);
    if (toFileName.isEmpty()) {
        return Utils::saveStringToFile(xmldPrescription,
                                      QDir::homePath() + "/prescription.di",
                                      tr(Core::Constants::FREEDIAMS_FILEFILTER)) ;
    } else {
        return Utils::saveStringToFile(xmldPrescription, toFileName, Utils::Overwrite, Utils::DontWarnUser);
    }
}

/**
 * \brief For drag and drop functionnalities, defines the mimeTypes of DrugsIO.
 * \sa DrugsDB::DrugsModel::mimeType()
*/
QStringList DrugsIO::prescriptionMimeTypes()
{
    return QStringList() << "application/prescription";
}

