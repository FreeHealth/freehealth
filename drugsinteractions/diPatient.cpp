/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "diPatient.h"

// include drugswidget headers
#include <mfDrugsConstants.h>

// include toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include "tkConstantTranslations.h"

// include medical toolkit headers
#include <tkMedicalGlobal.h>

// include Qt headers
#include <QHash>
#include <QString>

using namespace mfDrugsConstants;
Q_TK_USING_TRANSLATIONS

const char* const MAIN_PATIENT_TAG  = "PatientDatas";

/**
  \brief Private Part of diPatient
  \internal
*/
class diPatientPrivate
{
public:
    diPatientPrivate()
    {
        m_XmlTags.insert(diPatient::FullName , "Name" );
        m_XmlTags.insert(diPatient::Sex , "Sex" );
        m_XmlTags.insert(diPatient::DateOfBirth , "DateOfbirth");
        m_XmlTags.insert(diPatient::Weight , "Weight");
        m_XmlTags.insert(diPatient::Size , "Size" );
        m_XmlTags.insert(diPatient::CreatinClearance , "CreatinClearance" );
        m_XmlTags.insert(diPatient::ICD10Deceases , "ICD10Deceases");
        m_XmlTags.insert(diPatient::DrugsAllergies , "DrugsAllergies" );
        m_XmlTags.insert(diPatient::Creatinin , "Creatinin");
        m_XmlTags.insert(diPatient::IMC , "IMC");
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

    QHash<int,QVariant> m_Values;
    QHash<int,QString>  m_XmlTags;
};


/** Constructor */
diPatient::diPatient() : d(0)
{
    d = new diPatientPrivate();
    Q_ASSERT(d);
}

/** \brief Destructor */
diPatient::~diPatient()
{
    if (d) delete d;
    d=0;
}

/** \brief Clears all the recorded datas */
void diPatient::clear()
{
    d->m_Values.clear();
}

/** \brief Return trus if patient has got the data referenced by enumerator diPatient::Reference */
bool diPatient::has(const Reference ref) const
{
    return d->m_Values.keys().contains(ref);
}


/** \brief Get the value of the patient according to the enumerator diPatient::Reference. */
QVariant diPatient::value( Reference ref ) const
{
    switch (ref)
    {
        case Age :
        {
            if (d->m_Values.contains(ref))
                return d->m_Values.value(ref);
            if (d->m_Values.contains(DateOfBirth)) {
                QString tmp;
                tmp = tkMedicalGlobal::readableAge(d->m_Values.value(DateOfBirth).toDateTime());
                d->m_Values.insert(Age,tmp);
                return tmp;
            }
        }
        default :
        {
            if (d->m_Values.contains(ref))
                return d->m_Values.value(ref);
        }
    }
    return QVariant();
}

/** \brief Defines a value of the patient according to the enumerator diPatient::Reference. */
void diPatient::setValue( Reference ref, const QVariant &value )
{
    d->m_Values.insert(ref, value);
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
QString diPatient::toXml() const
{
    QHash<QString, QString> datas;
    foreach(int k, d->m_Values.keys()) {
        datas.insert(d->getXmlTag(k), d->m_Values.value(k).toString());
    }
    return tkGlobal::createXml(MAIN_PATIENT_TAG,datas,4,true);
}

/**
  \brief Retreive patient's datas from the XML input (datas are coded).
  \sa toXml()
*/
bool diPatient::fromXml(const QString &xml)
{
    QHash<QString,QString> datas;
    if (!tkGlobal::readXml(xml, MAIN_PATIENT_TAG, datas, true)) {
        tkLog::addError("diPatient", "Error while reading XML patient's datas (diPatient::fromXml())");
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

/**
  \brief Find and replace all tokens that refering to the patient datas.
  Tokens are :
  - [[NAME]]
  - [[FULLNAME]]
  - [[DATEOFBIRTH]]
  - [[AGE]]
  - [[WEIGHT]]
  - [[SIZE]]
  - [[CLCR]]
  \sa tkGlobal::replaceTokens(), mfDrugsConstants
*/
void diPatient::replaceTokens(QString &stringWillBeModified)
{
    tkGlobal::replaceToken(stringWillBeModified, TOKEN_PATIENTNAME,        value(diPatient::FullName).toString() );
    tkGlobal::replaceToken(stringWillBeModified, TOKEN_PATIENTAGE,        value(diPatient::FullName).toString() );
    tkGlobal::replaceToken(stringWillBeModified, TOKEN_WEIGHT,      value(diPatient::Weight).toString() );
    tkGlobal::replaceToken(stringWillBeModified, TOKEN_SIZE,        value(diPatient::Size).toString() );
    tkGlobal::replaceToken(stringWillBeModified, TOKEN_DATEOFBIRTH, value(diPatient::DateOfBirth).toString() );
    tkGlobal::replaceToken(stringWillBeModified, TOKEN_CLCR,        value(diPatient::CreatinClearance).toString() );
}
