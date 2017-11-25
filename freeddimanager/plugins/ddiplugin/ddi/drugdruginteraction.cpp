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
#include "drugdruginteraction.h"
#include <ddiplugin/constants.h>

#include <coreplugin/constants_icons.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_msgerror.h>

#include <QStringList>

#include <QDebug>

using namespace DDI;
using namespace Trans::ConstantTranslations;

static Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace {
    const int RISK_ID = 100000;
    const int MANAGEMENT_ID = 200000;

    // INTERACTIONS ICONS
    const char * const  INTERACTION_ICONCRITICAL =         "critical.png";      /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONDECONSEILLEE =     "unadvised.png";   /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONTAKEINTOACCOUNT =  "warning.png";       /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONPRECAUTION =       "info.png";          /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONINFORMATION =      "information.png";          /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONUNKONW =           "help.png";          /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONOK =               "ok.png";            /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONP450 =             "p450iam.png";            /*!< \brief Themed icon name for Interactions logo. */
    const char * const  INTERACTION_ICONGPG =              "gpgiam.png";            /*!< \brief Themed icon name for Interactions logo. */
}

static QString levelName(const QString &levelUid)
{
    QStringList tmp;
    if (levelUid.contains("U"))
        tmp << tkTr(Trans::Constants::INN_DUPLICATION);
    if (levelUid.contains("Z"))
        tmp << tkTr(Trans::Constants::CLASS_DUPLICATION);
    if (levelUid.contains("C"))
        tmp << tkTr(Trans::Constants::CONTRAINDICATION);
    if (levelUid.contains("D"))
        tmp << tkTr(Trans::Constants::DISCOURAGED);
    if (levelUid.contains("T"))
        tmp << tkTr(Trans::Constants::TAKE_INTO_ACCOUNT);
    if (levelUid.contains("P"))
        tmp << tkTr(Trans::Constants::PRECAUTION_FOR_USE);
    if (levelUid.contains("I"))
        tmp << tkTr(Trans::Constants::INFORMATION);
    if (levelUid.contains("Y"))
        tmp << tkTr(Trans::Constants::GPG_IAM);
    if (levelUid.contains("450"))
        tmp << tkTr(Trans::Constants::P450_IAM);
    return tmp.join( ", " );
}

static int languageIndex(const QString &lang)
{
    if (lang=="en")
        return 1;
    if (lang=="fr")
        return 2;
    return 2;
}

DrugDrugInteractionDose::DrugDrugInteractionDose(const QVariant &uid):
    m_uid(uid)
{
    m_Data.insert(UsesFrom, false);
    m_Data.insert(UsesTo, false);
}

DrugDrugInteractionDose::~DrugDrugInteractionDose()
{
}

bool DrugDrugInteractionDose::isEmpty() const
{
    return (m_Data.value(FromValue).isNull() &&
            m_Data.value(FromUnits).isNull() &&
            m_Data.value(FromRepartition).isNull() &&
            m_Data.value(ToValue).isNull() &&
            m_Data.value(ToUnits).isNull() &&
            m_Data.value(ToRepartition).isNull());
}

bool DrugDrugInteractionDose::isValid() const
{
    return (!m_uid.isNull() && !m_uid.toString().isEmpty());
}

QVariant DrugDrugInteractionDose::data(const int reference) const
{
    return m_Data.value(reference, QVariant());
}

bool DrugDrugInteractionDose::setData(const int reference, const QVariant &value)
{
    if (reference==Uid) {
        m_uid = value;
    }
    m_Data.insert(reference, value);
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   DrugDrugInteraction   //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/** Construct an empty unduplicated DrugDrugInteraction object. */
DrugDrugInteraction::DrugDrugInteraction()
{
    m_Data.insert(IsDuplicated, false);
    m_Data.insert(IsValid, true);
    m_Data.insert(IsReviewed, false);
    m_Data.insert(DateCreation, QDate::currentDate());
    // Manage doses
    static int doseUid = 0;
    m_FirstDose.setData(DrugDrugInteractionDose::Uid, ++doseUid);
    m_SecondDose.setData(DrugDrugInteractionDose::Uid, ++doseUid);
}

/** Copy ctor */
DrugDrugInteraction::DrugDrugInteraction(const DrugDrugInteraction &copy)
{
    this->m_Data = copy.m_Data;
    this->m_FirstDose = copy.m_FirstDose;
    this->m_Formalized = copy.m_Formalized;
    this->m_SecondDose = copy.m_SecondDose;
}

/** Construct an DrugDrugInteraction object using the XML QDomElement. */
DrugDrugInteraction::DrugDrugInteraction(const QDomElement &element)
{
    if (element.tagName()!="DDI") {
        LOG_ERROR_FOR("DrugDrugInteraction", "Wrong XML Element");
    } else {
        m_Data.insert(FirstInteractorName, Constants::correctedUid(Utils::removeAccents(element.attribute("i1"))));
        m_Data.insert(SecondInteractorName, Constants::correctedUid(Utils::removeAccents(element.attribute("i2"))));
        m_Data.insert(FirstInteractorRouteOfAdministrationIds, element.attribute("i1ra").split(";"));
        m_Data.insert(SecondInteractorRouteOfAdministrationIds, element.attribute("i2ra").split(";"));
        m_Data.insert(LevelCode, element.attribute("l"));
        m_Data.insert(LevelName, ::levelName(element.attribute("l")));
        m_Data.insert(DateCreation, QDate::fromString(element.attribute("a", QDate(2010,01,01).toString(Qt::ISODate)), Qt::ISODate));
        m_Data.insert(DateLastUpdate, QDate::fromString(element.attribute("lu", QDate(2010,01,01).toString(Qt::ISODate)), Qt::ISODate));
        m_Data.insert(IsValid, element.attribute("v", "1").toInt());
        m_Data.insert(IsReviewed, element.attribute("rv", "0").toInt());
        if (!element.attribute("p").isEmpty())
            m_Data.insert(PMIDsStringList, element.attribute("p").split(";"));
        m_Data.insert(IsDuplicated, false);
        // Read Risk
        QDomElement risk = element.firstChildElement("R");
        while (!risk.isNull()) {
            setRisk(risk.attribute("t"), risk.attribute("l"));
            risk = risk.nextSiblingElement("R");
        }
        // Read Management
        QDomElement management = element.firstChildElement("M");
        while (!management.isNull()) {
            setManagement(management.attribute("t"), management.attribute("l"));
            management = management.nextSiblingElement("M");
        }
        // Read Dose related interactions
        QDomElement dose = element.firstChildElement("D");
        while (!dose.isNull()) {
            DrugDrugInteractionDose *ddidose = 0;
            if (dose.attribute("i") == "1") {
                ddidose = &m_FirstDose;
            } else {
                ddidose = &m_SecondDose;
            }
            if (dose.attribute("uf").toInt()==1) {
                ddidose->setData(DrugDrugInteractionDose::UsesFrom, true);
                ddidose->setData(DrugDrugInteractionDose::FromValue , dose.attribute("fv"));
                ddidose->setData(DrugDrugInteractionDose::FromUnits, dose.attribute("fu"));
                ddidose->setData(DrugDrugInteractionDose::FromRepartition , dose.attribute("fr"));
            } else if (dose.attribute("ut").toInt()==1) {
                ddidose->setData(DrugDrugInteractionDose::UsesTo, true);
                ddidose->setData(DrugDrugInteractionDose::ToValue , dose.attribute("tv"));
                ddidose->setData(DrugDrugInteractionDose::ToUnits, dose.attribute("tu"));
                ddidose->setData(DrugDrugInteractionDose::ToRepartition , dose.attribute("tr"));
            }
            dose = dose.nextSiblingElement("D");
        }
        // Read formalized risk
        QDomElement form = element.firstChildElement("F");
        QDomNamedNodeMap attributeMap = form.attributes();
        for(int i=0; i < attributeMap.size(); ++i) {
            addFormalized(attributeMap.item(i).nodeName(), form.attribute(attributeMap.item(i).nodeName()));
        }
    }
}

DrugDrugInteraction::~DrugDrugInteraction()
{
}

/** Return the value of the \e reference. Reference points to the DataRepresentation enum */
QVariant DrugDrugInteraction::data(const int reference) const
{
    return m_Data.value(reference, QVariant());
}

/** Set the value of the \e reference. Reference points to the DataRepresentation enum */
bool DrugDrugInteraction::setData(const int reference, const QVariant &value)
{
    if (reference==LevelCode) {
        m_Data.insert(LevelName, ::levelName(value.toString()));
    }
    m_Data.insert(reference, value);
    return true;
}

/** The validity of the encoded DDI level */
bool DrugDrugInteraction::levelValidity() const
{
    const QString &level = m_Data.value(LevelCode).toString();
    if (level=="450")
        return true;
    return (level.size()==1);
}

static QString firstLevelCode(const QString &levelCode)
{
    // Assuming that levels are one char excepts cytochrome p450 DDI,
    // We just have to return the first char of the \e levelCode or "450"
    if (levelCode.isEmpty())
        return QString();

    if (levelCode.at(0).isDigit())
        return "450";

    return levelCode.at(0);
}

/**
 * Return the level name according to the database level code.
 * If the code corresponds to a multi-level DDI (which is a DDI coding error)
 * all level names are returns separated with a comma.
*/
QString DrugDrugInteraction::levelName(QString levelCode)
{
    QString level = firstLevelCode(levelCode);
    QStringList names;
    while (!level.isEmpty()) {
        if (level == "C")
            names << tkTr(Trans::Constants::CONTRAINDICATION);
        else if (level == "D")
            names << tkTr(Trans::Constants::DISCOURAGED);
        else if (level == "450")
            names << tkTr(Trans::Constants::P450_IAM);
        else if (level == "Y")
            names << tkTr(Trans::Constants::GPG_IAM);
        else if (level == "T")
            names << tkTr(Trans::Constants::TAKE_INTO_ACCOUNT);
        else if (level == "P")
            names << tkTr(Trans::Constants::PRECAUTION_FOR_USE);
        else if (level == "I")
            names << tkTr(Trans::Constants::INFORMATION);

        level = levelCode.remove(0, level.size());
        level = firstLevelCode(level);
    }
    return names.join(", ");
}

/** Set the translated DDI risk */
void DrugDrugInteraction::setRisk(const QString &risk, const QString &lang)
{
    m_Data.insert(RISK_ID + languageIndex(lang), risk);
}

/** Set the translated DDI management comment */
void DrugDrugInteraction::setManagement(const QString &management, const QString &lang)
{
    m_Data.insert(MANAGEMENT_ID + languageIndex(lang), management);
}

/** Return the translated DDI risk */
QString DrugDrugInteraction::risk(const QString &lang) const
{
    return m_Data.value(RISK_ID + languageIndex(lang)).toString();
}

/** Return the translated DDI management comment */
QString DrugDrugInteraction::management(const QString &lang) const
{
    return m_Data.value(MANAGEMENT_ID + languageIndex(lang)).toString();
}

/** Return the icon corresponding to the DDI level according to the current theme */
QIcon DrugDrugInteraction::levelIcon() const
{
    const QString &levelUid = m_Data.value(LevelCode).toString();
    if (levelUid.contains("C"))
        return theme()->icon(INTERACTION_ICONCRITICAL);
    if (levelUid.contains("D"))
        return theme()->icon(INTERACTION_ICONDECONSEILLEE);
    if (levelUid.contains("P"))
        return theme()->icon(INTERACTION_ICONPRECAUTION);
    if (levelUid.contains("450"))
        return theme()->icon(INTERACTION_ICONP450);
    if (levelUid.contains("Y"))
        return theme()->icon(INTERACTION_ICONGPG);
    if (levelUid.contains("T"))
        return theme()->icon(INTERACTION_ICONTAKEINTOACCOUNT);
    if (levelUid.contains("I"))
        return theme()->icon(INTERACTION_ICONINFORMATION);
    return theme()->icon(Core::Constants::ICONHELP);
}

/** When DDI are reviewed, add a reviewer name to the current reviewers */
void DrugDrugInteraction::addReviewer(const QString &name)
{
    QStringList reviewers = m_Data.value(ReviewersStringList).toStringList();
    if (!reviewers.contains(name, Qt::CaseInsensitive)) {
        reviewers << name;
        m_Data.insert(ReviewersStringList, reviewers);
    }
}

/** Add a "DDI formalized" value */
void DrugDrugInteraction::addFormalized(const QString &attr, const QString &value)
{
    m_Formalized.insert(attr,value);
}

/** Transform the DDI to XML */
QString DrugDrugInteraction::toXml() const
{
    QString xml;
    xml = QString("i1=\"%1\" i2=\"%2\" l=\"%3\" a=\"%4\" lu=\"%5\" v=\"%6\" rv=\"%7\"")
            .arg(m_Data.value(FirstInteractorName).toString().replace("<", "&lt;"))
            .arg(m_Data.value(SecondInteractorName).toString().replace("<", "&lt;"))
            .arg(m_Data.value(LevelCode).toString())
            .arg(m_Data.value(DateCreation).toString())
            .arg(m_Data.value(DateLastUpdate).toString())
            .arg(m_Data.value(IsValid).toInt())
            .arg(m_Data.value(IsReviewed).toInt())
            ;

    const QStringList &first = m_Data.value(FirstInteractorRouteOfAdministrationIds).toStringList();
    const QStringList &second = m_Data.value(SecondInteractorRouteOfAdministrationIds).toStringList();
    if (first.count()==1) {
        if (!first.at(0).isEmpty())
            xml += QString(" i1ra=\"%1\"").arg(first.join(";"));
    } else {
        xml += QString(" i1ra=\"%1\"").arg(first.join(";"));
    }
    if (second.count()==1) {
        if (!second.at(0).isEmpty())
            xml += QString(" i2ra=\"%1\"").arg(second.join(";"));
    } else {
        xml += QString(" i2ra=\"%1\"").arg(second.join(";"));
    }
    const QStringList &pmids = m_Data.value(PMIDsStringList).toStringList();
    if (pmids.count() > 0) {
        if (!pmids.at(0).isEmpty()) {
            xml += QString(" p=\"%1\"").arg(pmids.join(";"));
        }
    }

    xml = QString("<DDI %1>").arg(xml);

    // add risk and management
    xml += QString("\n <R l=\"%1\" t=\"%2\"/>")
            .arg("fr")
            .arg(risk("fr").replace("<", "&lt;"));
    xml += QString("\n <R l=\"%1\" t=\"%2\"/>")
            .arg("en")
            .arg(risk("en").replace("<", "&lt;"));
    if (!management("fr").isEmpty()) {
        xml += QString("\n <M l=\"%1\" t=\"%2\"/>")
                .arg("fr")
                .arg(management("fr").replace("<", "&lt;"));
        xml += QString("\n <M l=\"%1\" t=\"%2\"/>")
                .arg("en")
                .arg(management("en").replace("<", "&lt;"));
    }

    // add doses
    QList<const DrugDrugInteractionDose *> doses;
    doses.append(&m_FirstDose);
    doses.append(&m_SecondDose);
    for(int i=0;i<doses.count();++i) {
        const DrugDrugInteractionDose *dose = doses.at(i);
        if (dose->isEmpty())
            continue;
        QString attribs;
        if (dose->data(DrugDrugInteractionDose::UsesFrom).toBool()) {
            attribs = QString("uf=\"1\" fv=\"%1\" fu=\"%2\" fr=\"%3\" ")
                    .arg(dose->data(DrugDrugInteractionDose::FromValue).toString())
                    .arg(dose->data(DrugDrugInteractionDose::FromUnits).toString())
                    .arg(dose->data(DrugDrugInteractionDose::FromRepartition).toString());
        }
        if (dose->data(DrugDrugInteractionDose::UsesTo).toBool()) {
            attribs += QString("ut=\"1\" tv=\"%1\" tu=\"%2\" tr=\"%3\" ")
                    .arg(dose->data(DrugDrugInteractionDose::ToValue).toString())
                    .arg(dose->data(DrugDrugInteractionDose::ToUnits).toString())
                    .arg(dose->data(DrugDrugInteractionDose::ToRepartition).toString());
        }
        if (attribs.isEmpty())
            continue;
        qWarning() << attribs;
        xml += QString("\n  <D i=\"%1\" %2/>").arg(i).arg(attribs);
    }

    // add formalized
    if (m_Formalized.count() > 0) {
        xml += "\n  <F";
        QHashIterator<QString,QString> i(m_Formalized);
         while (i.hasNext()) {
             i.next();
             QString val = i.value();
             val = val.replace("<", "&lt;");
             xml += " " + i.key() + "=\"" + val + "\"";
         }
         xml += "/>";
    }
    return xml + "\n</DDI>\n";
}

bool DrugDrugInteraction::updateDomElement(QDomElement *element, QDomDocument *doc) const
{
    if (element->tagName()!="DDI")
        return false;
    element->setAttribute("i1", m_Data.value(FirstInteractorName).toString());
    element->setAttribute("i2", m_Data.value(SecondInteractorName).toString());
    element->setAttribute("i1ra", m_Data.value(FirstInteractorRouteOfAdministrationIds).toStringList().join(";"));
    element->setAttribute("i2ra", m_Data.value(SecondInteractorRouteOfAdministrationIds).toStringList().join(";"));
    element->setAttribute("l", m_Data.value(LevelCode).toString());
    element->setAttribute("a", m_Data.value(DateCreation).toString());
    element->setAttribute("lu", m_Data.value(DateLastUpdate).toString());
    element->setAttribute("v", m_Data.value(IsValid).toInt());
    element->setAttribute("rv", m_Data.value(IsReviewed).toInt());
    element->setAttribute("p", m_Data.value(PMIDsStringList).toStringList().join(";"));

    // Update risk
    QDomElement riskNode = element->firstChildElement("R");
    if (riskNode.isNull()) {
        // create risk
        QDomElement rFr = doc->createElement("R");
        rFr.setAttribute("l", "fr");
        rFr.setAttribute("t", risk("fr"));
        element->appendChild(rFr);
        QDomElement rEn = doc->createElement("R");
        rEn.setAttribute("l", "en");
        rEn.setAttribute("t", risk("en"));
        element->appendChild(rEn);
    } else {
        if (riskNode.attribute("l").compare("fr",Qt::CaseInsensitive)==0) {
            riskNode.setAttribute("t", risk("fr"));
            QDomElement rEn = riskNode.nextSiblingElement("R");
            if (rEn.isNull()) {
                rEn = doc->createElement("R");
                rEn.setAttribute("l", "en");
                element->appendChild(rEn);
            }
            rEn.setAttribute("t", risk("en"));
        } else if (riskNode.attribute("l").compare("en",Qt::CaseInsensitive)==0) {
            riskNode.setAttribute("t", risk("en"));
            QDomElement rFr = riskNode.nextSiblingElement("R");
            if (rFr.isNull()) {
                rFr = doc->createElement("R");
                rFr.setAttribute("l", "fr");
                element->appendChild(rFr);
            }
            rFr.setAttribute("t", risk("fr"));
        }
    }

    // Update management
    QDomElement manNode = element->firstChildElement("M");
    if (manNode.isNull()) {
        // create management
        QDomElement rFr = doc->createElement("M");
        rFr.setAttribute("l", "fr");
        rFr.setAttribute("t", management("fr"));
        element->appendChild(rFr);
        QDomElement rEn = doc->createElement("M");
        rEn.setAttribute("l", "en");
        rEn.setAttribute("t", management("en"));
        element->appendChild(rEn);
    } else {
        if (manNode.attribute("l").compare("fr",Qt::CaseInsensitive)==0) {
            manNode.setAttribute("t", management("fr"));
            QDomElement rEn = manNode.nextSiblingElement("M");
            if (rEn.isNull()) {
                rEn = doc->createElement("R");
                rEn.setAttribute("l", "en");
                element->appendChild(rEn);
            }
            rEn.setAttribute("t", management("en"));
        } else if (manNode.attribute("l").compare("en",Qt::CaseInsensitive)==0) {
            manNode.setAttribute("t", risk("en"));
            QDomElement rFr = manNode.nextSiblingElement("M");
            if (rFr.isNull()) {
                rFr = doc->createElement("R");
                rFr.setAttribute("l", "fr");
                element->appendChild(rFr);
            }
            rFr.setAttribute("t", management("fr"));
        }
    }

    // Update doses
//    QDomElement dose = element.firstChildElement("D");
//    while (!dose.isNull()) {
//        DrugDrugInteractionDose *ddidose = 0;
//        if (dose.attribute("i") == "1") {
//            ddidose = &m_FirstDose;
//        } else {
//            ddidose = &m_SecondDose;
//        }
//        if (dose.attribute("uf").toInt()==1) {
//            ddidose->setData(DrugDrugInteractionDose::UsesFrom, true);
//            ddidose->setData(DrugDrugInteractionDose::FromValue , dose.attribute("fv"));
//            ddidose->setData(DrugDrugInteractionDose::FromUnits, dose.attribute("fu"));
//            ddidose->setData(DrugDrugInteractionDose::FromRepartition , dose.attribute("fr"));
//        } else if (dose.attribute("ut").toInt()==1) {
//            ddidose->setData(DrugDrugInteractionDose::UsesTo, true);
//            ddidose->setData(DrugDrugInteractionDose::ToValue , dose.attribute("tv"));
//            ddidose->setData(DrugDrugInteractionDose::ToUnits, dose.attribute("tu"));
//            ddidose->setData(DrugDrugInteractionDose::ToRepartition , dose.attribute("tr"));
//        }
//        dose = dose.nextSiblingElement("D");
//    }

    // Update formalized risk
    QDomElement formNode = element->firstChildElement("F");
    if (!formNode.isNull()) {
        QDomNamedNodeMap attributeMap = formNode.attributes();
        for(int i=0; i < attributeMap.size(); ++i) {
            formNode.removeAttribute(attributeMap.item(i).nodeName());
        }
    }
    if (m_Formalized.count() > 0) {
        QHashIterator<QString,QString> i(m_Formalized);
        while (i.hasNext()) {
            i.next();
            formNode.setAttribute(i.key(), i.value());
        }
    }
    return true;
}

QList<DrugDrugInteraction> &DrugDrugInteraction::listFromXml(const QString &xml)
{
    QList<DrugDrugInteraction> *list = new QList<DrugDrugInteraction>;
    QDomDocument doc;
    QString error;
    int line = 0, col = 0;
    if (!doc.setContent(xml)) {
        LOG_ERROR_FOR("DrugDrugInteraction", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(error).arg(line).arg(col));
        return *list;
    }
    QDomElement root = doc.firstChildElement("DrugDrugInteractions");
    QDomElement element = root.firstChildElement("DDI");
    while (!element.isNull()) {
        DrugDrugInteraction di(element);
        list->append(di);
        element = element.nextSiblingElement("DDI");
    }
    return *list;
}

/** Return true if DDI are equal */
bool DrugDrugInteraction::operator==(const DrugDrugInteraction &other) const
{
    // same interactor (whatever is the order)
    if ((other.data(FirstInteractorName).toString()==m_Data.value(FirstInteractorName).toString() &&
         other.data(SecondInteractorName).toString()==m_Data.value(SecondInteractorName).toString()) ||
        (other.data(SecondInteractorName).toString()==m_Data.value(FirstInteractorName).toString() &&
         other.data(FirstInteractorName).toString()==m_Data.value(SecondInteractorName).toString())) {
        if (other.data(LevelCode).toString()==m_Data.value(LevelCode).toString()) {
            if (other.risk("fr") != m_Data.value(RISK_ID+languageIndex("fr")))
                qWarning() << "Same interaction, different FR Risk";
            if (other.risk("en") != m_Data.value(RISK_ID+languageIndex("en")))
                qWarning() << "Same interaction, different EN Risk";
            return true;
        }
    }
    return false;
}

/** Helper for qSort */
bool DrugDrugInteraction::lowerThan(const DrugDrugInteraction &d1, const DrugDrugInteraction &d2)
{
    return d1.data(DrugDrugInteraction::FirstInteractorName).toString() < d2.data(DrugDrugInteraction::FirstInteractorName).toString();
}
