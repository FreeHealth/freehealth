/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "fsp.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <utils/global.h>

#include <QHash>
#include <QVariant>
#include <QDate>
#include <QDomDocument>

static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }

// FspSet
// Fsp
//   i n="..." v="..."
//   i n="..." v="..."
//   Acts
//     Act
//       i n="..." v="..."
//       i n="..." v="..."
//     /Act
//   /Acts
// /Fsp
// Fsp
//  ...
// /Fsp
// /FspSet

namespace {
const char * const XML_TAG_FSPSET = "FspSet";
const char * const XML_TAG_ROOT = "Fsp";
const char * const XML_TAG_ITEM = "i";
const char * const XML_ATTRIB_NAME = "n";
const char * const XML_ATTRIB_VALUE = "v";
const char * const XML_ATTRIB_LINE = "i";
const char * const XML_TAG_ACTS = "Acts";
const char * const XML_TAG_ACT = "Act";
}

namespace Tools {
namespace Internal {
class FspPrivate
{
public:
    FspPrivate(Fsp */*parent*/)
        // : q(parent)
    {
    }

    ~FspPrivate()
    {
    }

    QString xmlItemName(int index)
    {
        switch (index) {
        // Bill
        case Fsp::Bill_Number : return "b.nb";
        case Fsp::Bill_Date : return "b.dt";
            // Patient
        case Fsp::Patient_FullName : return "p.n";
        case Fsp::Patient_FullAddress : return "p.a";
        case Fsp::Patient_DateOfBirth : return "p.d";
        case Fsp::Patient_Personal_NSS : return "p.nss";
        case Fsp::Patient_Personal_NSSKey : return "p.nssk";
        case Fsp::Patient_Assure_FullName : return "p.afn";
        case Fsp::Patient_Assure_NSS : return "p.anss";
        case Fsp::Patient_Assure_NSSKey : return "p.anssk";
        case Fsp::Patient_Assurance_Number : return "p.anb";
            // Conditions
        case Fsp::Condition_Maladie  : return "c.m";
        case Fsp::Condition_Maladie_ETM : return "c.m.ex";
        case Fsp::Condition_Maladie_ETM_Ald : return "c.m.ex.ald";
        case Fsp::Condition_Maladie_ETM_Autre : return "c.m.ex.autre";
        case Fsp::Condition_Maladie_ETM_L115 : return "c.m.ex.l115";
        case Fsp::Condition_Maladie_ETM_Prevention : return "c.m.ex.p";
        case Fsp::Condition_Maladie_ETM_AccidentParTiers_Oui : return "c.m.ex.at";
        case Fsp::Condition_Maladie_ETM_AccidentParTiers_Date : return "c.m.ex.atdt";
        case Fsp::Condition_Maternite : return "c.mat";
        case Fsp::Condition_Maternite_Date : return "c.mat.dt";
        case Fsp::Condition_ATMP : return "c.at";
        case Fsp::Condition_ATMP_Number : return "c.at.n";
        case Fsp::Condition_ATMP_Date : return "c.at.d";
        case Fsp::Condition_NouveauMedTraitant : return "c.nmt";
        case Fsp::Condition_MedecinEnvoyeur : return "c.me";
        case Fsp::Condition_AccesSpecifique : return "c.as";
        case Fsp::Condition_Urgence : return "c.urg";
        case Fsp::Condition_HorsResidence : return "c.hr";
        case Fsp::Condition_Remplace : return "c.rp";
        case Fsp::Condition_HorsCoordination : return "c.hc";
        case Fsp::Condition_AccordPrealableDate : return "c.prea";
        case Fsp::Unpaid_PartObligatoire : return "u.po";
        case Fsp::Unpaid_PartComplementaire : return "u.pc";
            // Amount
        case Fsp::Amount_Date:  return "am.dt";
        case Fsp::Amount_ActCode:  return "am.code";
        case Fsp::Amount_Activity:  return "am.act";
        case Fsp::Amount_CV:  return "am.cv";
        case Fsp::Amount_OtherAct1:  return "am.o1";
        case Fsp::Amount_OtherAct2:  return "am.o2";
        case Fsp::Amount_Amount:  return "am.am";
        case Fsp::Amount_Depassement:  return "am.dep";
        case Fsp::Amount_Deplacement_IKMD:  return "am.ik";
        case Fsp::Amount_Deplacement_Nb:  return "am.iknb";
        case Fsp::Amount_Deplacement_IKValue:  return "am.ikval";
        case Fsp::TotalAmount: return "total";
            // ModelData
        case Fsp::Label: return "m.lbl";
        }
        return QString::null;
    }

    int indexForXmlItemName(const QString &xmlTag)
    {
        if (xmlTag.startsWith("b.")) {
            if (xmlTag.compare("b.nb", Qt::CaseInsensitive) == 0) {
                return Fsp::Bill_Number;
            } else if (xmlTag.compare("b.nb", Qt::CaseInsensitive) == 0) {
                return Fsp::Bill_Date;
            }
        } else if (xmlTag.startsWith("p.")) {
            if (xmlTag.compare("p.n", Qt::CaseInsensitive) == 0) {
                return Fsp::Patient_FullName;
            } else if (xmlTag.compare("p.a", Qt::CaseInsensitive) == 0) {
                return Fsp::Patient_FullAddress;
            } else if (xmlTag.compare("p.d", Qt::CaseInsensitive) == 0) {
                return Fsp::Patient_DateOfBirth;
            } else if (xmlTag.compare("p.nss", Qt::CaseInsensitive) == 0) {
                return Fsp::Patient_Personal_NSS;
            } else if (xmlTag.compare("p.nssk", Qt::CaseInsensitive) == 0) {
                return Fsp::Patient_Personal_NSSKey;
            } else if (xmlTag.compare("p.afn", Qt::CaseInsensitive) == 0) {
                return Fsp::Patient_Assure_FullName;
            } else if (xmlTag.compare("p.anss", Qt::CaseInsensitive) == 0) {
                return Fsp::Patient_Assure_NSS;
            } else if (xmlTag.compare("p.anssk", Qt::CaseInsensitive) == 0) {
                return Fsp::Patient_Assure_NSSKey;
            } else if (xmlTag.compare("p.anb", Qt::CaseInsensitive) == 0) {
                return Fsp::Patient_Assurance_Number;
            }
        } else if (xmlTag.startsWith("c.")) {
            if (xmlTag.compare("c.m", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Maladie;
            } else if (xmlTag.compare("c.m.ex", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Maladie_ETM;
            } else if (xmlTag.compare("c.m.ex.ald", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Maladie_ETM_Ald;
            } else if (xmlTag.compare("c.m.ex.autre", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Maladie_ETM_Autre;
            } else if (xmlTag.compare("c.m.ex.l115", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Maladie_ETM_L115;
            } else if (xmlTag.compare("c.m.ex.p", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Maladie_ETM_Prevention;
            } else if (xmlTag.compare("c.m.ex.at", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Maladie_ETM_AccidentParTiers_Oui;
            } else if (xmlTag.compare("c.m.ex.atdt", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Maladie_ETM_AccidentParTiers_Date;
            } else if (xmlTag.compare("c.mat", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Maternite;
            } else if (xmlTag.compare("c.mat.dt", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Maternite_Date;
            } else if (xmlTag.compare("c.at", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_ATMP;
            } else if (xmlTag.compare("c.at.n", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_ATMP_Number;
            } else if (xmlTag.compare("c.at.d", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_ATMP_Date;
            } else if (xmlTag.compare("c.nmt", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_NouveauMedTraitant;
            } else if (xmlTag.compare("c.me", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_MedecinEnvoyeur;
            } else if (xmlTag.compare("c.as", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_AccesSpecifique;
            } else if (xmlTag.compare("c.urg", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Urgence;
            } else if (xmlTag.compare("c.hr", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_HorsResidence;
            } else if (xmlTag.compare("c.rp", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_Remplace;
            } else if (xmlTag.compare("c.hc", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_HorsCoordination;
            } else if (xmlTag.compare("c.prea", Qt::CaseInsensitive) == 0) {
                return Fsp::Condition_AccordPrealableDate;
            }
        } else if (xmlTag.startsWith("u.")) {
            if (xmlTag.compare("u.po", Qt::CaseInsensitive) == 0) {
                return Fsp::Unpaid_PartObligatoire;
            } else if (xmlTag.compare("u.pc", Qt::CaseInsensitive) == 0) {
                return Fsp::Unpaid_PartComplementaire;
            }
        } else  if (xmlTag.startsWith("am.")) {
            if (xmlTag.compare("am.dt", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_Date;
            } else if (xmlTag.compare("am.code", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_ActCode;
            } else if (xmlTag.compare("am.act", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_Activity;
            } else if (xmlTag.compare("am.cv", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_CV;
            } else if (xmlTag.compare("am.o1", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_OtherAct1;
            } else if (xmlTag.compare("am.o2", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_OtherAct2;
            } else if (xmlTag.compare("am.am", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_Amount;
            } else if (xmlTag.compare("am.dep", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_Depassement;
            } else if (xmlTag.compare("am.ik", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_Deplacement_IKMD;
            } else if (xmlTag.compare("am.iknb", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_Deplacement_Nb;
            } else if (xmlTag.compare("am.ikval", Qt::CaseInsensitive) == 0) {
                return Fsp::Amount_Deplacement_IKValue;
            }
        } else if (xmlTag.startsWith("m.")) {
            if (xmlTag.compare("m.lbl", Qt::CaseInsensitive) == 0) {
                return Fsp::Label;
            }
        } else if (xmlTag.compare("total", Qt::CaseInsensitive) == 0) {
            return Fsp::TotalAmount;
        }
        return -1;
    }

public:
    QHash<int, QVariant> _data;
    QList< QHash<int, QVariant> > _amountLines;

private:
//    Fsp *q;
};

/*! Constructor of the Tools::Internal::Fsp class */
Fsp::Fsp() :
    d(new FspPrivate(this))
{
    for(int i=0; i< 4; ++i)
        d->_amountLines.append(QHash<int, QVariant>());
}

/** Copy constructor */
Fsp::Fsp(const Fsp &cp) :
    d(new FspPrivate(this))
{
    d->_data = cp.d->_data;
    d->_amountLines = cp.d->_amountLines;
}

/** Copy operator */
Fsp &Fsp::operator=(const Fsp &cp)
{
    if (cp.d == d)
        return *this;
    d->_data = cp.d->_data;
    d->_amountLines = cp.d->_amountLines;
    return *this;
}

/*! Destructor of the Tools::Internal::Fsp class */
Fsp::~Fsp()
{
    if (d)
        delete d;
    d = 0;
}

/** Clear */
void Fsp::clear()
{
    d->_data.clear();
    d->_amountLines.clear();
    for(int i=0; i< 4; ++i)
        d->_amountLines.append(QHash<int, QVariant>());
}

/** Add a bill uuid (max 9chars) */
void Fsp::setBillNumber(const QString &uid)
{
    d->_data.insert(int(Bill_Number), uid);
}

/** Define the billing date */
void Fsp::setBillDate(const QDate &date)
{
    d->_data.insert(int(Bill_Date), date);
}

/** Populate the FSP patient part with the current patient data */
bool Fsp::populateWithCurrentPatientData()
{
    if (patient()->uuid().isEmpty())
        return false;
    d->_data.insert(Patient_FullName, patient()->data(Core::IPatient::FullName));
    d->_data.insert(Patient_FullAddress, patient()->data(Core::IPatient::FullAddress));
    d->_data.insert(Patient_DateOfBirth, patient()->data(Core::IPatient::DateOfBirth));
    QString nss = patient()->data(Core::IPatient::SocialNumber).toString();
    d->_data.insert(Patient_Personal_NSS, nss.left(13));
    if (nss.size() > 13)
        d->_data.insert(Patient_Personal_NSSKey, patient()->data(Core::IPatient::SocialNumber).toString().mid(13));
    d->_data.insert(Patient_Assure_FullName, patient()->data(Core::IPatient::SocialNumberOwnerFullName));
    if (!d->_data.value(Patient_Assure_FullName).toString().isEmpty()) {
        d->_data.insert(Patient_Assure_NSS, nss.left(13));
        if (nss.size() > 13)
            d->_data.insert(Patient_Assure_NSSKey, patient()->data(Core::IPatient::SocialNumber).toString().mid(13));
    }
    //    d->_data.insert(Patient_Assurance_Number, patient()->data(Core::IPatient:));
    return true;
}

/** Define the bill date, all amount date (if not defined) to the current date */
void Fsp::populateAmountsWithCurrentDate()
{
    for(int i=0; i< 4; ++i) {
        QHash<int, QVariant> &values = d->_amountLines[i];
        if (values.value(Amount_Amount).isNull())
            continue;
        if (values.value(Amount_Date).isNull())
            values.insert(Amount_Date, QDate::currentDate());
    }
    if (d->_data.value(Bill_Date).isNull())
        d->_data.insert(Bill_Date, QDate::currentDate());
}

/** Define the patient information */
bool Fsp::setData(int index, const QVariant &value)
{
    d->_data.insert(index, value);
    return true;
}

QVariant Fsp::data(int index) const
{
    return d->_data.value(index);
}

void Fsp::addAmountData(int line, int index, const QVariant &value)
{
    Q_ASSERT(line < 4);
    d->_amountLines[line].insert(index, value);
}

QVariant Fsp::amountLineData(int line, int index) const
{
    Q_ASSERT(line < 4);
    return d->_amountLines[line].value(index);
}

/**
 * Compute the total amount of the FSP by adding each fees amount and
 * set the Fsp::TotalAmount value. \n
 * This does not the IK/ID into account.
 */
void Fsp::computeTotalAmountFromFees()
{
    // TODO: ajouter les IK au montant total
    double total = 0.0;
    for(int i=0; i< 4; ++i) {
        if (!d->_amountLines.value(i).value(Amount_Amount).isNull())
            total += d->_amountLines.value(i).value(Amount_Amount).toDouble();
    }
    d->_data.insert(TotalAmount, total);
}

/** FSP to XML. TotalAmount must be computed first. */
QString Fsp::toXml(XmlDataSet dataSet) const
{
    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE FreeMedForms>\n";
    QDomDocument doc;
    QDomElement root = doc.createElement(::XML_TAG_ROOT);
    doc.appendChild(root);
    QList<int> ids;
    if (dataSet.testFlag(BillData)) {
        ids << Bill_Date << Bill_Number;
    }
    if (dataSet.testFlag(PatientData)) {
        ids <<  Patient_FullName
             << Patient_FullAddress
             << Patient_DateOfBirth
             << Patient_Personal_NSS
             << Patient_Personal_NSSKey
             << Patient_Assure_FullName
             << Patient_Assure_NSS
             << Patient_Assure_NSSKey
             << Patient_Assurance_Number;
    }
    if (dataSet.testFlag(ConditionData)) {
        ids << Condition_Maladie
            << Condition_Maladie_ETM
            << Condition_Maladie_ETM_Ald
            << Condition_Maladie_ETM_Autre
            << Condition_Maladie_ETM_L115
            << Condition_Maladie_ETM_Prevention
            << Condition_Maladie_ETM_AccidentParTiers_Oui
            << Condition_Maladie_ETM_AccidentParTiers_Date
            << Condition_Maternite
            << Condition_Maternite_Date
            << Condition_ATMP
            << Condition_ATMP_Number
            << Condition_ATMP_Date
            << Condition_NouveauMedTraitant
            << Condition_MedecinEnvoyeur
            << Condition_AccesSpecifique
            << Condition_Urgence
            << Condition_HorsResidence
            << Condition_Remplace
            << Condition_HorsCoordination
            << Condition_AccordPrealableDate;
    }
    if (dataSet.testFlag(UnpaidData)) {
        ids << Unpaid_PartObligatoire
            << Unpaid_PartComplementaire;
    }
    if (dataSet.testFlag(ModelData)) {
        for(int i = Label; i < ModelData_ColumnCount; ++i)
            ids << i;
    }

    // Append values
    for(int i=0; i < ids.count(); ++i) {
        int id = ids.at(i);
        if (d->_data.value(id, QVariant()).isNull())
            continue;
        QDomElement item = doc.createElement(::XML_TAG_ITEM);
        item.setAttribute(::XML_ATTRIB_NAME, d->xmlItemName(id));
        item.setAttribute(::XML_ATTRIB_VALUE, d->_data.value(id).toString());
        root.appendChild(item);
    }

    // Append amounts
    if (d->_amountLines.count() && dataSet.testFlag(AmountData)) {
        QDomElement acts = doc.createElement(::XML_TAG_ACTS);
        root.appendChild(acts);
        for(int i=0; i < 4; ++i) {
            QDomElement act = doc.createElement(::XML_TAG_ACT);
            const QHash<int, QVariant> &values = d->_amountLines.at(i);
            for(int j=0; j < Amount_MaxData; ++j) {
                if (values.value(j, QVariant()).isNull())
                    continue;
                QDomElement item = doc.createElement(::XML_TAG_ITEM);
                item.setAttribute(::XML_ATTRIB_NAME, d->xmlItemName(j));
                item.setAttribute(::XML_ATTRIB_LINE, i);
                item.setAttribute(::XML_ATTRIB_VALUE, values.value(j).toString());
                act.appendChild(item);
            }
            acts.appendChild(act);
        }

        QDomElement total = doc.createElement(::XML_TAG_ITEM);
        total.setAttribute(::XML_ATTRIB_NAME, d->xmlItemName(TotalAmount));
        total.setAttribute(::XML_ATTRIB_VALUE, d->_data.value(TotalAmount).toString());
        root.appendChild(total);
    }
    xml += doc.toString(1);
    return xml;
}

QList<Fsp> Fsp::fromXml(const QString &content)
{
    QList<Fsp> fsps;
    QDomDocument doc;
    QString error;
    int col, line;
    if (!doc.setContent(content, &error, &line, &col)) {
        Utils::warningMessageBox("Erreur XML",
                                 QString("La FSP ne peut pas être reconstruite, une erreur XML a été détectée :<br />"
                                         "(%1; %2) %3<br />")
                                 .arg(line).arg(col).arg(error));
        return fsps;
    }
    QDomElement root = doc.firstChildElement(::XML_TAG_FSPSET);
    root = root.firstChildElement(::XML_TAG_ROOT);
    while (!root.isNull()) {
        Fsp fsp;
        // Read patient, condition, unpaid item
        QDomElement item = root.firstChildElement(::XML_TAG_ITEM);
        while (!item.isNull()) {
            fsp.d->_data.insert(d->indexForXmlItemName(item.attribute(::XML_ATTRIB_NAME)), item.attribute(::XML_ATTRIB_VALUE));
            item = item.nextSiblingElement(::XML_TAG_ITEM);
        }
        // Read amount item
        QDomElement acts = root.firstChildElement(::XML_TAG_ACTS);
        QDomElement act = acts.firstChildElement(::XML_TAG_ACT);
        line = 0;
        while (!act.isNull()) {
            QHash<int, QVariant> &values = fsp.d->_amountLines[line];
            item = act.firstChildElement(::XML_TAG_ITEM);
            while (!item.isNull()) {
                values.insert(d->indexForXmlItemName(item.attribute(::XML_ATTRIB_NAME)), item.attribute(::XML_ATTRIB_VALUE));
                item = item.nextSiblingElement(::XML_TAG_ITEM);
            }
            act = act.nextSiblingElement(::XML_TAG_ACT);
            ++line;
        }
        fsps << fsp;
        root = root.nextSiblingElement(::XML_TAG_ROOT);
    }
    return fsps;
}

QList<Fsp> Fsp::fromXmlFile(const QString &absPathFileName)
{
    return fromXml(Utils::readTextFile(absPathFileName));
}

} // namespace Internal
} // namespace Tool
