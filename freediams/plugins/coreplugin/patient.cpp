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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "patient.h"
#include <patientbaseplugin/constants_settings.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <medicalutils/global.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
//#include <coreplugin/constants_tokensandsettings.h>

#include <QHash>
#include <QString>

using namespace Trans::ConstantTranslations;
using namespace Core;
using namespace Core::Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static const char* const MAIN_PATIENT_TAG  = "PatientDatas";

namespace Core {
namespace Internal {

/**
  \brief Private Part of Core::Patient
  \internal
*/
class PatientPrivate
{
public:
    PatientPrivate()
    {
        m_XmlTags.insert(IPatient::Uid ,        "UID");
        m_XmlTags.insert(IPatient::UsualName ,  "Lastnames" );
        m_XmlTags.insert(IPatient::OtherNames ,  "OtherNames" );
        m_XmlTags.insert(IPatient::Firstname ,  "Firstnames" );
        m_XmlTags.insert(IPatient::Gender ,     "Gender" );
        m_XmlTags.insert(IPatient::DateOfBirth ,"DateOfbirth");
        m_XmlTags.insert(IPatient::WeightInGrams ,        "WeightInGrams");
        m_XmlTags.insert(IPatient::HeightInCentimeters ,  "HeightInCentimeters" );
        m_XmlTags.insert(IPatient::Creatinine,  "Creatinine" );
        m_XmlTags.insert(IPatient::CreatinineUnit,  "CreatinineUnit" );
        m_XmlTags.insert(IPatient::BMI ,        "BMI");
        m_XmlTags.insert(IPatient::CreatinClearance , "CreatinClearance" );
        m_XmlTags.insert(IPatient::ICD10Diseases ,    "ICD10Diseases");
        m_XmlTags.insert(IPatient::DrugsAtcAllergies ,   "DrugsAtcAllergies" );
        m_XmlTags.insert(IPatient::DrugsUidAllergies ,   "DrugsUidAllergies" );
        m_XmlTags.insert(IPatient::DrugsInnAllergies ,   "DrugsInnAllergies" );
        m_XmlTags.insert(IPatient::DrugsAtcIntolerances ,   "DrugsAtcIntolerances" );
        m_XmlTags.insert(IPatient::DrugsUidIntolerances ,   "DrugsUidIntolerances" );
        m_XmlTags.insert(IPatient::DrugsInnIntolerances ,   "DrugsInnIntolerances" );
    }

    QString getXmlTag(const int key) const
    {
        return m_XmlTags.value(key);
    }

    int getReferenceFromTag(const QString &tag) const
    {
        if (m_XmlTags.values().indexOf(tag)==-1)
            return -1;
        return m_XmlTags.key(tag);
    }

    QHash<int, QVariant> m_Values;
    QHash<int, QString>  m_XmlTags;
};

}  // End Internal
}  // End Core


/** Constructor */
Patient::Patient(QObject *parent) :
    IPatient(parent), d(new PatientPrivate)
{
    Q_ASSERT(d);
    setObjectName("Core::Patient");
}

/** \brief Destructor */
Patient::~Patient()
{
    if (d) delete d;
    d=0;
}

/** \brief Clears all the recorded datas */
void Patient::clear()
{
    d->m_Values.clear();
}

/** \brief Return trus if patient has got the data referenced by enumerator Patient::Reference */
bool Patient::has(const int ref) const
{
    return d->m_Values.keys().contains(ref);
}

/** \brief Get the value of the patient according to the enumerator Patient::Reference. */
QVariant Patient::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        int ref = index.column();
        switch (ref)
        {
        case GenderIndex:
            {
                const QString &g = d->m_Values.value(Core::IPatient::Gender).toString();
//                qWarning() << g;
                if (g=="M")
                    return 0;
                if (g=="F")
                    return 1;
                if (g=="H")
                    return 2;
                return -1;
            }
        case YearsOld :
            {
                if (has(DateOfBirth)) {
                    return MedicalUtils::ageYears(d->m_Values.value(DateOfBirth).toDate());
                }
                break;
            }
        case Age :
            {
                if (d->m_Values.contains(ref))
                    return d->m_Values.value(ref);
                if (has(DateOfBirth)) {
                    QString tmp;
                    tmp = MedicalUtils::readableAge(d->m_Values.value(IPatient::DateOfBirth).toDate(), settings()->value(Patients::Constants::S_PEDIATRICSMONTHSYEARSLIMIT, 36).toInt());
                    d->m_Values.insert(Age,tmp);
                    return tmp;
                }
                break;
            }
        case CreatinClearance :
            {
                if (has(ref) && (!d->m_Values.value(ref).isNull()))
                    return d->m_Values.value(ref);
                // If we can not retrieve it from command line --> calculate it
                if (data(YearsOld).toInt()>0) {
                    if (has(Creatinine) && has(Gender) && has(WeightInGrams)) {
                        bool isMale = d->m_Values.value(Gender).toString().startsWith("M");
                        return MedicalUtils::clearanceCreatinin(data(YearsOld).toInt(),
                                                                d->m_Values.value(WeightInGrams).toDouble()/100.,
                                                                d->m_Values.value(Creatinine).toDouble(),
                                                                isMale);
                    }
                }
                break;
            }
        case FullName:
            {
                QString r;
                if (has(OtherNames))
                    r = QString("%1 - %2 %3")
                    .arg(d->m_Values.value(UsualName).toString())
                    .arg(d->m_Values.value(OtherNames).toString())
                    .arg(d->m_Values.value(Firstname).toString());
                else
                    r = QString("%1 %3")
                        .arg(d->m_Values.value(UsualName).toString())
                        .arg(d->m_Values.value(Firstname).toString());
                return r;
            }
        case DrugsAllergiesWithoutPrecision:
            {
                QString allergies;
                const QStringList &drug = d->m_Values.value(Core::IPatient::DrugsUidAllergies).toStringList();
                if (!drug.isEmpty())
                    allergies += tr("Drugs(%1), ").arg(drug.join(";"));
                const QStringList &inns = d->m_Values.value(Core::IPatient::DrugsInnAllergies).toStringList();
                if (!inns.isEmpty())
                    allergies += tr("INN(%1), ").arg(inns.join(";"));
                const QStringList &atc = d->m_Values.value(Core::IPatient::DrugsAtcAllergies).toStringList();
                if (!atc.isEmpty())
                    allergies += tr("ATC(%1), ").arg(atc.join(";"));
                allergies.chop(2);
                return allergies;
            }
        case DrugsIntolerancesWithoutPrecision:
            {
                QString intolerances;
                const QStringList &drug = d->m_Values.value(Core::IPatient::DrugsUidIntolerances).toStringList();
                if (!drug.isEmpty())
                    intolerances += tr("Drugs(%1), ").arg(drug.join(";"));
                const QStringList &inns = d->m_Values.value(Core::IPatient::DrugsInnIntolerances).toStringList();
                if (!inns.isEmpty())
                    intolerances += tr("INN(%1), ").arg(inns.join(";"));
                const QStringList &atc = d->m_Values.value(Core::IPatient::DrugsAtcIntolerances).toStringList();
                if (!atc.isEmpty())
                    intolerances += tr("ATC(%1), ").arg(atc.join(";"));
                intolerances.chop(2);
                return intolerances;
            }
        default:
            {
                return d->m_Values.value(ref, QVariant());
            }
        }
    } else if (role == Qt::DecorationRole) {
        if (index.column()==Gender || index.column()==GenderIndex) {
            const QString &g = d->m_Values.value(Core::IPatient::Gender).toString();
            if (g=="M") {
                return theme()->icon(Core::Constants::ICONMALE);
            } else if (g=="F") {
                return theme()->icon(Core::Constants::ICONFEMALE);
            } else if (g=="H") {
                return theme()->icon(Core::Constants::ICONHERMAPHRODISM);
            }
            return QIcon();
        }
    }
    return QVariant();
}

QVariant Patient::data(int column) const
{
    QModelIndex idx = index(0, column);
    return data(idx);
}

bool Patient::setValue(int ref, const QVariant &value)
{
    QModelIndex idx = index(0, ref);
    return setData(idx, value);
}

/** \brief Defines a value of the patient according to the enumerator Patient::Reference. */
bool Patient::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role != Qt::EditRole)
        return false;

    // Read only values
    switch (index.column()) {
    case DrugsAllergiesWithoutPrecision:
    case DrugsIntolerancesWithoutPrecision:
        return false;
    }

    d->m_Values.insert(index.column(), value);
    return true;
}

/**
  \brief Creates the XML output of the whole patient datas (datas are coded).
  The XML looks like :
  \verbatim
  <PatientDatas>
     <FullName>"encoded"</FullName>
     [...]
  </PatientDatas>
  \endverbatim
  \sa fromXml()
*/
QString Patient::toXml() const
{
    QHash<QString, QString> datas;
    foreach(int k, d->m_Values.keys()) {
        datas.insert(d->getXmlTag(k), d->m_Values.value(k).toString());
    }
    return Utils::createXml(MAIN_PATIENT_TAG,datas,4,true);
}

/**
  \brief Retreive patient's datas from the XML input (datas are coded).
  \sa toXml()
*/
bool Patient::fromXml(const QString &xml)
{
    QHash<QString,QString> datas;
    if (!Utils::readXml(xml, MAIN_PATIENT_TAG, datas, true)) {
        LOG_ERROR("Error while reading XML patient's datas (Patient::fromXml())");
        return false;
    }
    foreach(const QString &k, datas.keys()) {
        int id = d->getReferenceFromTag(k);
        if (id!=-1) {
            d->m_Values.insert(id, datas.value(k));
        }
    }
    return true;
}
