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

/**
  \class DrugsDB::Internal::DrugComposition
  \brief Represents the moleculare composition of a drug.
*/


/**
  \class DrugsDB::Internal::DrugsData
  \brief Represents a drug.
*/

#include "drugsdata.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/drugsinteraction.h>
#include <drugsbaseplugin/interactionsmanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <QApplication>
#include <QHash>
#include <QMultiHash>
#include <QLocale>
#include <QPair>

#include <QDebug>

using namespace DrugsDB::Constants;

using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;

static const char* const FRENCH_RPC_LINK = "http://afssaps-prd.afssaps.fr/php/ecodex/rcp/R%1.htm"; // 2+2+3
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}


namespace DrugsDB {
namespace Internal {
    class DrugCompositionPrivate
    {
    public:
        DrugCompositionPrivate() : m_InnCode(-1), m_LinkId(-1) {}
        ~DrugCompositionPrivate() {}

    public:
        QString m_MoleculeName;
        QStringList m_IamClass;
        int m_CodeMolecule;
        int m_InnCode;
        QString m_Form;
        QString m_RefDosage;
        QString m_Nature;     // SA / FT
        int m_LinkId;
    };
}  // End namespace Internal
}  // End namespace DrugsDB


/** \brief Create a new molecular component for one specified drug */
DrugComposition::DrugComposition() :
        d(new DrugCompositionPrivate), m_Link(0)
{
}

DrugComposition::~DrugComposition()
{
    if (d)
        delete d;
    d = 0;
    // Don't delete m_Link
}

/** \brief Feed values from the database */
void DrugComposition::setValue(const int fieldref, const QVariant &value)
{
    using namespace DrugsDB::Constants;
    switch (fieldref)
    {
    case COMPO_MOL_FORM : d->m_Form = value.toString(); break;
    case COMPO_MOL_NAME : d->m_MoleculeName = value.toString(); break;
    case COMPO_MOL_CODE : d->m_CodeMolecule = value.toInt(); break;
    case COMPO_DOSAGE : m_Dosage = value.toString(); break;
    case COMPO_REF_DOSAGE : d->m_RefDosage = value.toString(); break;
    case COMPO_NATURE : d->m_Nature = value.toString(); break;
    case COMPO_LK_NATURE: d->m_LinkId = value.toInt(); break;
    case COMPO_IAM_DENOMINATION: m_InnName = value.toString(); break;
    case COMPO_IAM_CLASS_DENOMINATION : d->m_IamClass = value.toStringList(); break;
    default : break;
    }
}

/** \brief Set the linked DrugComposition (this happens when a molecule is transform to another one which is the active one). */
void DrugComposition::setLinkedSubstance(DrugComposition *link)
{
    Q_ASSERT(link);
    m_Link = link;
    if (!link->isLinkedWith(this))
        link->setLinkedSubstance(this);
}

/** \brief Define the ATC id code to \e code. */
void DrugComposition::setInnCode(const int code)
{
    /** \todo One drug component can have multiple ATC IDs */
   d->m_InnCode = code;
}

/** \brief Return the nature link id. */
int DrugComposition::linkId() const
{
    return d->m_LinkId;
}

/** \brief Test link relation with the \e link */
bool DrugComposition::isLinkedWith(DrugComposition *link) const
{
    Q_ASSERT(link);
    return (link==m_Link);
}

/** \brief Test if this component is the active one ? \sa setLinkedSubstance() */
bool DrugComposition::isTheActiveSubstance() const
{
    if (d->m_Nature=="FT")
        return true;
    if (!m_Link)
        return true;
    return false;//(!m_Link->isTheActiveSubstance());  // take care to infinite looping...
}

/** \brief Returns the code of the molecule. */
int DrugComposition::codeMolecule() const
{
    return d->m_CodeMolecule;
}

/** \brief Returns the main ATC id code. */
int DrugComposition::innCode() const
{
    return d->m_InnCode;
}

/** \brief Return the ATC label of the molecule. Check the linked composition for the inn name. */
QString DrugComposition::innName() const
{
    if (this->isTheActiveSubstance())
        return m_InnName;
    else if (m_Link)
        return m_Link->m_InnName; // avoid infinite loop by retreiving value directly not with the function of m_Link
    return QString();
}

/** \brief Returns the interacting classes names. */
QStringList DrugComposition::iamClasses() const
{
    return d->m_IamClass;
}

/** \brief Return the corrected dosage of this component. */
QString DrugComposition::innDosage() const
{
    QString tmp;
    if (this->isTheActiveSubstance())
        tmp = m_Dosage;
    else if (m_Link)
        tmp = m_Link->m_Dosage; // avoid infinite loop by retreiving value directly not with the function of m_Link
    // do some transformations
    if (!tmp.isEmpty()) {
        tmp.replace(",000","");
        tmp.replace(",00","");
    }
    // set the transformed dosage for the next call
    m_Dosage = tmp;
    return tmp;
}

/** \brief Return the dosage of the molecule */
QString DrugComposition::dosage() const
{
    return m_Dosage;
}

/** \brief Return the form of the molecule */
QString DrugComposition::form() const {return d->m_Form;}

/** \brief Return the name of the molecule */
QString DrugComposition::moleculeName() const {return d->m_MoleculeName;}

/** \brief Return the nature of the molecule */
QString DrugComposition::nature() const {return d->m_Nature;}

/** \brief Return the nature link of the molecule */
int DrugComposition::lkNature() const {return d->m_LinkId;}

/** \brief Return the warning text (for debugging purpose) */
QString DrugComposition::warnText() const
{
    QString tmp;
    tmp += "Composition : " + d->m_MoleculeName
           + "\n  Form : " + d->m_Form + "\n  inn : " + m_InnName +  "\n  dosage : " + m_Dosage
           + "\n  refDosagase : " + d->m_RefDosage + "\n  nature : " + d->m_Nature;
    if (m_Link)
        tmp += "\n  Linked";
    tmp += "\n    innName() : " + innName() + "\n    innDosage() : " + innDosage();
    tmp += "\n    iamClasses() : " + iamClasses().join("; ");
    return tmp + "\n";
}

//--------------------------------------------------------------------------------------------------------
//------------------------------- mfDrugPrivate constructor / destructor ---------------------------------
//--------------------------------------------------------------------------------------------------------
namespace DrugsDB {
namespace Internal {
class DrugsDataPrivate
{
public:
    struct structCIP {
        int      CIP;
        QString  denomination;
        QDate    date;
    };

    DrugsDataPrivate()
    {
        m_CISValues.reserve(DRUGS_MaxParam);
        m_PrescriptionValues.reserve(Prescription::MaxParam - Prescription::Id);
    }

    ~DrugsDataPrivate()
    {
        qDeleteAll(m_Compositions);
        m_Compositions.clear();
    }

    QString prescriptionToText(bool toHtml);
    bool usesFromTo(const QString &from, QString &to, const int prescriptionIndex);

public:
    QList<DrugComposition *>     m_Compositions;
    QMultiHash<int, QPair<QString, QString > > m_Routes;  // Pair: x = lang; y = label

    QList<structCIP>             m_CIPs;
    QHash< int, QVariant>        m_CISValues;
    QMultiHash< int, QVariant>   m_COMPOValues;
    QHash< int, QVariant>        m_PrescriptionValues;
    QString                      m_NoLaboDenomination;
    QString                      m_LastHtmlPrescription, m_LastTxtPrescription;   // Cache
    bool                         m_PrescriptionChanges;
    QSet<int>                    m_IamCodes;                 /*!< \brief Stores all Inn and IamClasses codes of this drug. */
};
}
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------- mfDrug constructor / destructor ------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
  \brief Create a new drug.
  All created instance are deleted by DrugsDB::DrugsModel.
*/
DrugsData::DrugsData()
    : d(new DrugsDataPrivate())
{
    d->m_NoLaboDenomination.clear();
    setPrescriptionValue(Prescription::IsTextualOnly, false);
    d->m_PrescriptionChanges = true;
}

DrugsData::~DrugsData()
{
    if (d) delete d;
    d=0;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ Setters -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
  \brief Stores \e value of drugs referenced by its source table index /e tableref, source field \e fieldref.
  \sa DrugsBase::getDrugByCIS()
  \sa mfDrugsConstants
*/
void DrugsData::setValue(const int tableref, const int fieldref, const QVariant & value)
{
    switch(tableref)
    {
    case Table_DRUGS : d->m_CISValues[fieldref] = value; break;
    case Table_COMPO :
        {
            if (fieldref == COMPO_IAM_DENOMINATION)
                d->m_COMPOValues.insertMulti(fieldref, value);
            else if (fieldref == COMPO_IAM_CLASS_DENOMINATION)
                d->m_COMPOValues.insert(fieldref, value);
            else if (!d->m_COMPOValues.values(fieldref).contains(value))
                d->m_COMPOValues.insertMulti(fieldref, value);
            break;
        }
    default: Utils::Log::addError("DrugsData", "Wrong table reference parameter in DrugsData::setValue().",
                                  __FILE__, __LINE__); break;
    }
}

/** \brief Add a composition class to this drug */
void DrugsData::addComposition(DrugComposition *compo)
{
    d->m_Compositions.append(compo);
}

/** \brief Define the routes. */
void DrugsData::addRoute(const int routeId, const QString &lang, const QString &label)
{
    d->m_Routes.insertMulti(routeId, QPair<QString,QString>(lang, label));
}

void DrugsData::addCIP(const int CIP, const QString &denomination, QDate date)
{
    /** \todo one drug can have multi presentations ==> list */
    DrugsDataPrivate::structCIP st;
    st.CIP = CIP;
    st.denomination = denomination;
    st.date = date;
    d->m_CIPs.append(st);
}

/**
  \brief Stores a new ATC ids and interacting classes codes for this drug removing duplicates.
   Only used by DrugDB::DrugsBase::getDrugByCIS().
*/
void DrugsData::addInnAndIamClasses(const QSet<int> &codes)
{
    foreach(const int i, codes) {
        if (d->m_IamCodes.contains(i))
           continue;
        d->m_IamCodes << i;
    }
}

/**
  \brief Stores \e value of drugs' prescription source field \e fieldref.
  \sa DrugsDB::DrugsModel, DrugsDB::DosageDialog
*/
void DrugsData::setPrescriptionValue(const int fieldref, const QVariant & value)
{
    if (d->m_PrescriptionValues.value(fieldref) != value) {
        d->m_PrescriptionChanges = true;
        d->m_PrescriptionValues[fieldref] = value;
    }
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ Getters -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
  \brief Return the drugs' value referenced by its database table and field references : \e tableref, \e fieldref.
  \sa DrugsDB::Constants
*/
QVariant DrugsData::value(const int tableref, const int fieldref) const
{
    switch (tableref)
    {
    case Table_DRUGS :
        {
            if (d->m_CISValues.contains(fieldref))
                return d->m_CISValues.value(fieldref);
            return QVariant();
        }
    case Table_COMPO :
        {
            if (d->m_COMPOValues.contains(fieldref)) {
                if (fieldref == COMPO_IAM_CLASS_DENOMINATION)
                    return d->m_COMPOValues.value(fieldref);
            }
            else {
                return d->m_COMPOValues.values(fieldref);
            }
            return QVariant();
        }
    default: Utils::Log::addError("DrugsData", "Wrong table reference parameter in DrugsData::value().",
                                  __FILE__, __LINE__); break;
    }
    return QVariant();
}

/**
  \brief Return true if drugs has a prescription.
  \sa DrugsDB::Constants::Prescription
*/
bool DrugsData::hasPrescription() const
{
    /** \todo this needs improvements ++ */
    int i = 0;
    foreach(const QVariant &q , d->m_PrescriptionValues) {
        if (!q.isNull())
            i++;
    }
    return i > 2;
}

/**
  \brief Return the prescription value referenced by its field \e fieldref.
  \sa DrugsDB::Constants::Prescription
*/
QVariant DrugsData::prescriptionValue(const int fieldref) const
{
    // manage some particularities
    switch (fieldref)
    {
        case Prescription::IntakesTo :
        {
            if (!d->m_PrescriptionValues.value(Prescription::IntakesUsesFromTo,false).toBool())
                return QVariant();
            break;
        }
        case Prescription::DurationTo :
        {
            if (!d->m_PrescriptionValues.value(Prescription::DurationUsesFromTo,false).toBool())
                return QVariant();
            break;
        }
    case Prescription::Route :
        {
            QString lang = QLocale().name().left(2);
            typedef QPair<QString, QString> pair;
            foreach(const pair &p, d->m_Routes.values(d->m_PrescriptionValues.value(Prescription::Route,-1).toInt())) {
                if (p.first == lang)
                    return p.second;
            }
        }
    }
    return d->m_PrescriptionValues.value(fieldref);
}

namespace {
    const char *const XML_COMPOSITION                  = "Composition";
    const char *const XML_COMPOSITION_INN              = "inn";
    const char *const XML_COMPOSITION_ATC              = "atc";
    const char *const XML_COMPOSITION_FORM             = "form";
    const char *const XML_COMPOSITION_ROUTE            = "route";
    const char *const XML_COMPOSITION_STRENGTH         = "strenght";
    const char *const XML_COMPOSITION_MOLECULAR        = "molecularName";
    const char *const XML_COMPOSITION_NATURE           = "nature";
    const char *const XML_COMPOSITION_NATURE_LK        = "natureLink";
}

/** \brief Return the composition of the drug in XML format. */
QString DrugsData::compositionToXml()
{
    QString tmp;
    /** \todo add ATC to drug composition */
    foreach(DrugComposition *compo, d->m_Compositions) {
        tmp += QString("<%1").arg(::XML_COMPOSITION);
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_INN).arg(compo->innName());
//        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_ATC).arg(compo);
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_FORM).arg(compo->form());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_ROUTE).arg(routes().join(", "));
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_STRENGTH).arg(compo->innDosage());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_MOLECULAR).arg(compo->moleculeName());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_NATURE).arg(compo->nature());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_NATURE_LK).arg(compo->lkNature());
        tmp += QString("/>\n");
    }
    return tmp;
}

/**
  \brief Return the drug denomination with or without pharmaceutical firms name.
  \sa DrugsDB::Constants::S_HIDELABORATORY
*/
QString DrugsData::denomination() const
{
    if (settings()->value(Constants::S_HIDELABORATORY).toBool()) {
        if (d->m_NoLaboDenomination.isEmpty()) {
            d->m_NoLaboDenomination = value(Constants::Table_DRUGS, Constants::DRUGS_NAME).toString();
            foreach(const QString &name, LABOS) {
                if (d->m_NoLaboDenomination.contains(" " + name + " ")) {
                    d->m_NoLaboDenomination.remove(" " + name + " ");
                    return d->m_NoLaboDenomination;
                }
            }
        }
        return d->m_NoLaboDenomination;
    }
    return value(Constants::Table_DRUGS, Constants::DRUGS_NAME).toString();
}

/** \brief Returns the list of all the molecules' code of the drug composition. */
QList<int> DrugsData::listOfCodeMolecules() const
{
    QList<int> tmp;
    foreach(DrugComposition *compo, d->m_Compositions)
        tmp << compo->codeMolecule();
    return tmp;
}

/** \brief Returns the list of all interacting classes' names */
QStringList DrugsData::listOfInnClasses() const
{
    QStringList tmp;
    foreach(int i, this->allInnAndIamClasses())
        if ((200000 < i) && (i < 299999))
            tmp << drugsBase()->getAtcLabel(i);
    tmp.sort();
    return tmp;
}

/** \brief Return all available routes for this drug. */
QStringList DrugsData::routes() const
{
    QString lang = QLocale().name().left(2);
    QStringList toReturn;
    typedef QPair<QString, QString> pair;
    foreach(const pair &p, d->m_Routes.values()) {
        if (p.first == lang)
            toReturn << p.second;
    }
    return toReturn;
}

/** \brief Returns the list of the molecules' name */
QStringList DrugsData::listOfMolecules() const
{
    QStringList toReturn;
    foreach(const DrugComposition *compo, d->m_Compositions)
        toReturn << compo->moleculeName();
    return toReturn;
}

/** \brief Return the list of all unique known ATC labels and interacting Classes names into a QStringList. */
QStringList DrugsData::listOfInn() const
{
    // return the list of distinct know INN of this drug
    QStringList toReturn;
    foreach(DrugComposition *compo, d->m_Compositions) {
        toReturn << compo->innName();
    }
    toReturn.removeDuplicates();
//    qWarning() << toReturn << __FILE__ << __LINE__;
    return toReturn;
}

/** \brief Return the number of known ATC ids and interacting classes ids. */
int DrugsData::numberOfInn() const
{
//    qWarning() << allInnAndIamClasses() << __FILE__ <<__LINE__;
    return allInnAndIamClasses().count();
}

/** \brief Returns all Inn and interacting classes codes known for this drug. */
QSet<int> DrugsData::allInnAndIamClasses() const
{
    return d->m_IamCodes;
}

/** \brief Return all 7char ATC codes ids known. This represent the INN of a molecule. */
QSet<int> DrugsData::allSevenCharsAtcIds() const
{
    QSet<int> list;
    foreach(int i, d->m_IamCodes) {
        if (i < 99999) {
            if (drugsBase()->getAtcCode(i).length()==7) {
                list << i;
            }
        }
    }
    return list;
}

/** \brief Return all 7char ATC codes labels known. This represent the INN of a molecule. */
QStringList DrugsData::allSevenCharsAtcCodes() const
{
    QStringList list;
    foreach(int i, d->m_IamCodes) {
        if (i < 99999) {
            const QString &code = drugsBase()->getAtcCode(i);
            if (code.length()==7) {
                list << code;
            }
        }
    }
    return list;
}

/** \brief Return all ATC codes ids known. */
QSet<int> DrugsData::allAtcIds() const
{
    QSet<int> list;
    foreach(int i, d->m_IamCodes) {
        if (i < 99999) {
            list << i;
        }
    }
    return list;
}

/** \brief Return all ATC codes labels known. */
QStringList DrugsData::allAtcCodes() const
{
    QStringList list;
    foreach(int i, d->m_IamCodes) {
        if (i < 99999) {
            list <<  drugsBase()->getAtcCode(i);
        }
    }
    return list;
}

/** \brief Returns all INN and interacting classes codes knwon for this drug. */
int DrugsData::mainInnCode() const
{
    QSet<int> list;
    foreach(int i, d->m_IamCodes) {
        if (i < 99999)
            list << i;
    }
    if (list.count() == 1)
        return *list.begin();
    return -1;
}

/** \brief Returns the main INN for this drug. If there is more than one INN or no INN, it returns a null string.*/
QString DrugsData::mainInnName() const
{
    return drugsBase()->getAtcLabel(mainInnCode());
}

/**
  \brief Return the main dosage of the main INN
  \sa mainInnCode(), mainInnName()
*/
QString DrugsData::mainInnDosage() const
{
    QString toReturn;
    int main = mainInnCode();
    if (main != -1) {
        foreach(DrugComposition *compo, d->m_Compositions)
            if ((compo->innCode()==main) && (compo->isTheActiveSubstance()))
                return compo->innDosage();
    }
    return QString();
}

//QStringList DrugsData::listOfInnWithPosology() const
//{
//    /** \todo add dosage of molecules */
//    QStringList toReturn;
//    QString tmp = "";
//    const QVariantList &poso = d->m_COMPOValues.values(COMPO_DOSAGE);
//    const QVariantList &refPoso = d->m_COMPOValues.values(COMPO_REF_DOSAGE);
//    int idx = 0;
//    foreach(const QVariant &q, d->m_COMPOValues.values(COMPO_IAM_DENOMINATION)) {
//        if (tmp != q.toString()) {
//            toReturn << q.toString() + " " + poso.at(idx).toString() + " / " + refPoso.at(idx).toString();
//            tmp = q.toString();
//        }
//        ++idx;
//    }
//    return toReturn;
//}

QStringList DrugsData::dosageOfMolecules() const
{
    // return the list of distinct know dosage in composition of the drug
    QStringList toReturn;
    QString tmp = "";
    foreach(DrugComposition *compo, d->m_Compositions)
        toReturn << compo->dosage();
    return toReturn;
}

/** \brief Return true if drug can be scored. */
bool DrugsData::isScoredTablet() const
{
    if (drugsBase()->actualDatabaseInformations()->identifiant==Constants::DB_DEFAULT_IDENTIFIANT)
        return denomination().contains(QRegExp("s.cable", Qt::CaseInsensitive));
    /** \todo manage drug's form */
    return true;
}

/** \brief Unused. */
QList<QVariant> DrugsData::CIPs() const
{
    QList<QVariant> ret;
    foreach(const DrugsDataPrivate::structCIP &s, d->m_CIPs)
        ret << s.CIP;
    return ret;
}

/** \brief Unused. */
QStringList DrugsData::CIPsDenominations() const
{
    QStringList ret;
    foreach(const DrugsDataPrivate::structCIP &s, d->m_CIPs)
        ret << s.denomination;
    return ret;
}

/** \brief Return the web link to the Summary Characteritics of the Product (monographs). */
QString DrugsData::linkToSCP() const
{
    QString toReturn;
    if (!value(Table_DRUGS, DRUGS_LINK_SPC).toString().isEmpty()) {
        if (Internal::DrugsBase::instance()->actualDatabaseInformations()->identifiant == Constants::DB_DEFAULT_IDENTIFIANT)
            toReturn = QString(FRENCH_RPC_LINK).arg(value(Table_DRUGS, DRUGS_LINK_SPC).toString().rightJustified(7,'0'));
        else
            toReturn = value(Table_DRUGS, DRUGS_LINK_SPC).toString();
    }
    return toReturn;
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------------ Viewers -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Return the INN composition of the drug in human readable format. */
QString DrugsData::innComposition() const
{
    QString toReturn;
    QString lastInn;
    foreach(DrugComposition *compo, d->m_Compositions) {
        if (lastInn!=compo->innName())
            toReturn += QString("%1 %2 + ").arg(compo->innName(), compo->innDosage());
        lastInn = compo->innName();
    }
    if (!toReturn.isEmpty()) {
        toReturn.chop(3);
        toReturn = toReturn.toUpper();
        toReturn += " , " + d->m_CISValues.value(DRUGS_FORM).toString();
    }
    return toReturn;
}

/** \brief Return the drug characteristics in human readable format. */
QString DrugsData::toHtml() const
{
    QString msg;

    const QStringList &iams = this->listOfInn();
    const QStringList &iamClass = this->listOfInnClasses();
    QString textIams, textClass;
    if (iams.isEmpty())
        textIams = QCoreApplication::translate("DrugsData", "No INN found.");
    else
        textIams = iams.join("<br>");

    if (iamClass.isEmpty())
        textClass = QCoreApplication::translate("DrugsData", "No interaction class found.");
    else
        textClass = iamClass.join("<br>");

    QString uidName = "UID";
    if (drugsBase()->actualDatabaseInformations()) {
        if (!drugsBase()->actualDatabaseInformations()->drugsUidName.isEmpty())
            uidName = drugsBase()->actualDatabaseInformations()->drugsUidName;
    }
    QString atc;
    if (!value(Table_DRUGS, DRUGS_ATC).toString().isEmpty()) {
        atc = " ; ATC: " + value(Table_DRUGS, DRUGS_ATC).toString();
    }
    QString mark = value(Table_DRUGS, DRUGS_AUTHORIZATION).toString();
    if (!mark.isEmpty()) {
        mark.prepend("<br>");
        mark += " | " + value(Table_DRUGS, DRUGS_TYPE_MP).toString();
    }

    msg += QString("<table border=1 cellpadding=2 cellspacing=2 width=100%>\n"
                    " <tr>\n"
                    "   <td colspan=2 rowspan=1 align=center>\n"
                    "       <span style=\"font-weight: bold;\">%1</span>\n"
                    "       %2"
                    "   </td>\n"
                    " </tr>\n"
                    " <tr>\n"
                    "   <td colspan=2 rowspan=1>\n"
                    "       %3\n"
                    "       <br>%4\n"
                    "       <br>%5 %6: %7 %8\n"
                    "   </td>\n"
                    " </tr>\n")
            .arg(denomination())
            .arg(mark)
            .arg(QCoreApplication::translate("DrugsData", "Form(s): ") + value(Table_DRUGS, DRUGS_FORM).toString())
            .arg(QCoreApplication::translate("DrugsData", "Route(s): ") + value(Table_DRUGS, DRUGS_ROUTE).toString())
            .arg(QCoreApplication::translate("DrugsData", "Identifier(s): "), uidName)
            .arg(value(Table_DRUGS, DRUGS_UID).toString())
            .arg(atc)
            ;

    QString tmp = "";
    QString name;
    foreach(DrugComposition *compo, d->m_Compositions) {
        if (compo->innName().isEmpty()) {
            name = compo->moleculeName();
        } else if (compo->innCode() < 200000) {
            name = compo->innName();
        }
        tmp += QString("<tr><td>%1</td><td>%2</td></tr>").arg(name).arg(compo->dosage());
        name.clear();
    }

    msg += QString(" %1\n"
                   " <tr>\n"
                   "   <td colspan=2 rowspan=1>%2</td>\n"
                   " </tr>\n"
                   "</table>\n\n")
            .arg(tmp)
            .arg(textClass);
    return msg;
}

inline static DrugsDB::InteractionsManager *new_im() {return new DrugsDB::InteractionsManager();}

/** \brief Return the drug list in human readable format. */
QString DrugsData::drugsListToHtml(const QDrugsList &list)
{
    QString msg;

//    // check interactions of the drugs list
//    mfDrugsBase *b = mfDrugsBase::instance();
//    b->interactions(list);
    InteractionsManager *im = new_im();
    im->setDrugsList(list);
    im->checkInteractions();

    // title
    msg = QString("<html>\n"
                   "<head>\n"
                   "<meta http-equiv=\"content-type \" content=\"text/html; charset=UTF-8\">\n"
                   "<title>%1</title>\n"
                   "<meta name=\"author\" content=\"%1\">\n"
                   "<meta name=\"description\" content=\"%1\">\n"
                   "</head>\n"
                   "<body>\n")
            .arg(qApp->applicationName());

    msg += QString("<p align=center><b>%1</b></p>\n").arg(qApp->applicationName());

    foreach(DrugsData* d, list) {
        msg += d->toHtml() + "<br>\n\n";
        if (im->drugHaveInteraction(d)) {
            const QList<DrugsInteraction *> &listDI = im->getInteractions(d);
            msg.append(im->listToHtml(listDI, true));

            msg.append("<br></ul>");
        }
    }
    msg.append("</body>\n</html>\n");

    delete im;

    return msg;
}

/** \brief Used to sort the drugs list. */
bool DrugsData::lessThan(const DrugsData *drug1, const DrugsData *drug2)
{
    bool ald1, ald2;
    ald1 = drug1->prescriptionValue(Prescription::IsALD).toBool();
    ald2 = drug2->prescriptionValue(Prescription::IsALD).toBool();
//    qWarning() << "lessThan" << drug1->denomination() << drug2->denomination() << ald1 << ald2 << (drug1->denomination() < drug2->denomination());
    // ALD First of all
    if ((ald1)  && (!ald2)) {
        return true;
    } else if ((!ald1)  && (ald2)) {
        return false;
    }
    return drug1->denomination() < drug2->denomination();
}

/** \brief For debugging purpose only. Warn all values of the drug. */
void DrugsData::warn() const
{
    if (Utils::isDebugCompilation()) {
        qWarning() << warnText();
    }
}

/** \brief For debugging purpose only. Warn all values of the drug. */
QString DrugsData::warnText() const
{
    if (!Utils::isDebugCompilation())
        return QString();
    QString tmp;
    tmp += d->m_CISValues[Constants::DRUGS_NAME].toString() + "\n";

    foreach(const int i, d->m_CISValues.keys())
        tmp += QString("  %1 = %2\n")
                           .arg(DrugsBase::instance()->fieldName(Table_DRUGS, i))
                           .arg(d->m_CISValues[i].toString());

    foreach(DrugComposition *compo, d->m_Compositions)
        tmp += compo->warnText();

    foreach(const int i, d->m_PrescriptionValues.keys())
        tmp += QString("Prescription : %1 == %2\n")
                           .arg(i)
                           .arg(d->m_PrescriptionValues[i].toString());
    tmp += QString("Distinct INN = %1\n").arg(listOfInn().join("; "));
    tmp += QString("NumberOfINN = %1\n").arg(numberOfInn());
    tmp += QString("Scored Tablet = %1\n").arg(isScoredTablet());
    return tmp;
}

/** \brief For debugging purpose only. Warn all values of the drug. */
void DrugsData::smallDrugWarn() const
{
    if (!Utils::isDebugCompilation())
        return;
    Utils::Log::addMessage("DrugsData", QString("get drug: %1 \t %2 \t %3 \t %4")
                      .arg(this->UID().toString())
                      .arg(this->denomination().leftJustified(60, ' '), this->form(), this->dosageOfMolecules().join(";")));
}


//--------------------------------------------------------------------------------------------------------
//--------------------------------------- TextualDrugsData -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
  \class DrugsDB::Internal::TextualDrugsData
  \brief A TextualDrugsData represent a fully textual drug.
  These kind of drugs are not retrieved from the database.
  Users can set the drug brand name by hand.
*/
TextualDrugsData::TextualDrugsData() :
        DrugsData()
{
    setPrescriptionValue(Prescription::IsTextualOnly, true);
    setValue(Table_DRUGS, DRUGS_UID, -1);
}

void TextualDrugsData::setDenomination(const QString &denomination)
{
    setValue(Table_DRUGS, DRUGS_NAME, denomination);
}


