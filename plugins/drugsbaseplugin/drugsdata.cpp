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
  \class DrugsData
  \brief This class owns the getters, setters and viewers of drugs.


  \todo Write documentation
  \todo Actually inside FMF and drugwidgetplugins mfDrugs are carried via a QList of pointers. This way we can not
  use the operators with mfDrugs. May be we must redefine QList of pointers in QList of references ?
 \ingroup freediams drugswidget
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
#include <QDebug>
#include <QLocale>

using namespace DrugsDB::Constants;

using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;

static const char* const FRENCH_RPC_LINK = "http://afssaps-prd.afssaps.fr/php/ecodex/rcp/R%1.htm"; // 2+2+3
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}


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


    QList<DrugComposition *>     m_Compositions;

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
/** \brief Constructor. All created instance are deleted by DrugsDB::Internal::DrugsModel. */
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

/** \brief Add a composition class description to this drug */
void DrugsData::addComposition(DrugComposition *compo)
{
    d->m_Compositions.append(compo);
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

/** \brief Stores a new INN and IamClasses codes for this drug assuming no doublons. Only used by DrugsBase::getDrugByCIS(). */
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
  \sa mfDrugsModel mfDosageDialog
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
  \brief Return the drugs' value referenced by its table and field reference : \e tableref, \e fieldref.
  \sa mfDrugsConstants
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
  \sa mfDrugsConstants::Prescription
*/
bool DrugsData::hasPrescription() const
{
    // TODO this needs improvments +++
    int i =0;
    foreach(const QVariant &q , d->m_PrescriptionValues) {
        if (!q.isNull())
            i++;
    }
    return i > 2;
}

/**
  \brief Return the prescription value referenced by its field \e fieldref.
  \sa mfDrugsConstants::Prescription
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
QString DrugsData::compositionToXml()
{
    QString tmp;
    /** \todo add ATC to drug composition */
    foreach(DrugComposition *compo, d->m_Compositions) {
        tmp += QString("<%1").arg(::XML_COMPOSITION);
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_INN).arg(compo->innName());
//        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_ATC).arg(compo);
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_FORM).arg(compo->form());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_ROUTE).arg(route());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_STRENGTH).arg(compo->innDosage());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_MOLECULAR).arg(compo->moleculeName());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_NATURE).arg(compo->nature());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_NATURE_LK).arg(compo->lkNature());
        tmp += QString("/>\n");
    }
    return tmp;
}

/**
  \brief Return the drug denomination with or without pharmaceutical firms name
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

/** \brief Returns the list of all the molecules' code of the drug composition */
QList<int> DrugsData::listOfCodeMolecules() const
{
    QList<int> tmp;
    foreach(DrugComposition *compo, d->m_Compositions)
        tmp << compo->m_CodeMolecule;
    return tmp;
}

/** \brief Returns the list of all IAM classes' names */
QStringList DrugsData::listOfInnClasses() const
{
    QStringList tmp;
    foreach(int i, this->allInnAndIamClasses())
        if ((200000 < i) && (i < 299999))
            tmp << drugsBase()->getAtcLabel(i);
    tmp.sort();
    return tmp;
}

/** \brief Returns the list of the molecules' name */
QStringList DrugsData::listOfMolecules() const
{
    QStringList toReturn;
    foreach(const DrugComposition *compo, d->m_Compositions)
        toReturn << compo->m_MoleculeName;
    return toReturn;
}

/** \brief Return the list of all unique known Inn and Iam Classes names into a QStringList. */
QStringList DrugsData::listOfInn() const
{
    // return the list of distinct know INN of this drug
    QStringList toReturn;
    QString tmp = "";
    foreach(DrugComposition *compo, d->m_Compositions) {
        if (tmp != compo->innName()) {
            toReturn << compo->innName();
            tmp = compo->innName();
        }
    }
//    qWarning() << toReturn << __FILE__ << __LINE__;
    return toReturn;
}

/** \brief Returns all Inn and IamClasses codes known for this drug. */
QSet<int> DrugsData::allInnAndIamClasses() const
{
    return d->m_IamCodes;
}

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


/** \brief Returns all Inn and IamClasses codes knwon for this drug. */
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

/** \brief Returns the main Inn for this drug. If there is more than one inn or no inn, it returns a null string.*/
QString DrugsData::mainInnName() const
{
    return drugsBase()->getAtcLabel(mainInnCode());
}

/**
  \brief Return the main dosage of the main inn
  \sa mainInnCode(), mainInnName()
*/
QString DrugsData::mainInnDosage() const
{
    QString toReturn;
    int main = mainInnCode();
    if (main != -1) {
        foreach(DrugComposition *compo, d->m_Compositions)
            if ((compo->m_InnCode==main) && (compo->isTheActiveSubstance()))
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

/**
  /~english \brief Return true if drug can be scored.
  /~french  \brief Retourne true si le médicament est sécable.
*/
bool DrugsData::isScoredTablet() const
{
    if (drugsBase()->actualDatabaseInformations()->identifiant==Constants::DB_DEFAULT_IDENTIFIANT)
        return denomination().contains(QRegExp("s.cable", Qt::CaseInsensitive));
    /** \todo manage drug's form */
    return true;
}

QList<QVariant> DrugsData::CIPs() const
{
    QList<QVariant> ret;
    foreach(const DrugsDataPrivate::structCIP &s, d->m_CIPs)
        ret << s.CIP;
    return ret;
}

QStringList DrugsData::CIPsDenominations() const
{
    QStringList ret;
    foreach(const DrugsDataPrivate::structCIP &s, d->m_CIPs)
        ret << s.denomination;
    return ret;
}

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
        atc = "<br>ATC = " + value(Table_DRUGS, DRUGS_ATC).toString();
    } else {
        atc = QApplication::translate("DrugData", "No ATC found");
    }
    msg += QString("<table border=1 cellpadding=2 cellspacing=2 width=100%>\n"
                    " <tr>\n"
                    "   <td colspan=2 rowspan=1 align=center>\n"
                    "       <span style=\"font-weight: bold;\">%1</span>\n"
                    "       <br>%2 = %3 ; "
                    "       %4\n"
                    "   </td>\n"
                    " </tr>\n")
            .arg(denomination())
            .arg(uidName)
            .arg(value(Table_DRUGS, DRUGS_UID).toString())
            .arg(atc);

    QString tmp = "";
    QStringList inns;
    foreach(DrugComposition *compo, d->m_Compositions) {
        if (!compo->innName().isEmpty() && compo->m_InnCode < 200000) {
            if (!inns.contains(compo->innName())) {
                inns << compo->innName();
                tmp += QString("<tr><td>%1</td><td>%2</td></tr>").arg(compo->innName()).arg(compo->dosage());
            }
        } else {
            tmp += QString("<tr><td>%1</td><td>%2</td></tr>").arg(compo->moleculeName()).arg(compo->dosage());
        }
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

QString DrugsData::drugsListToHtml(const QDrugsList & list)
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

/** \brief Fro debugging purpose only. Warn all values of the drug. */
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
                           .arg(DrugsBase::instance()->field(Table_DRUGS, i))
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


