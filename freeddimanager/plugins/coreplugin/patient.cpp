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

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <medicalutils/global.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <QHash>
#include <QString>

using namespace Trans::ConstantTranslations;
using namespace Core;
using namespace Core::Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

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
        m_XmlTags.insert(IPatient::Firstname ,  "Firstnames" );
        m_XmlTags.insert(IPatient::Gender ,     "Gender" );
        m_XmlTags.insert(IPatient::DateOfBirth ,"DateOfbirth");
        m_XmlTags.insert(IPatient::WeightInGrams, "WeightInGrams");
        m_XmlTags.insert(IPatient::HeightInCentimeters, "HeightInCentimeters" );
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
        return m_XmlTags.key(tag, -1);
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

/** \brief Return true if patient has got the data referenced by enumerator Patient::Reference */
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
        // Use only if we need a PatientModel in FreeDDIManager?
        return "FakeValue";
    }
    return QVariant();
}

QVariant Patient::data(int column) const
{
    return data(index(0,column));
}

bool Patient::setValue(int ref, const QVariant &value)
{
    return setData(index(0,ref), value);
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
        Utils::Log::addError("Patient", "Error while reading XML patient's datas (Patient::fromXml())",
                             __FILE__, __LINE__);
        return false;
    }
    foreach(const QString &k, datas.keys()) {
        int id = d->getReferenceFromTag(k);
        if (id!=-1) {
            d->m_Values.insert(id, datas.value(k));
        }
    }
//    qWarning() << d->m_Values;
    return true;
}
