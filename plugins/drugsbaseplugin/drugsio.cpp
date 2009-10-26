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

/**
  \class DrugsIO
  \brief Manages the IO operations for the dosages and prescriptions
  \ingroup freediams drugswidget
*/

#include "drugsio.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/messagesender.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>

#include <QApplication>
#include <QHash>
#include <QString>
#include <QDir>

namespace DrugsIOConstants {
    const char *const XML_VERSION                         = "<?xml version=\"0.0.8\" encoding=\"UTF-8\"?>\n";
    const char *const XML_PRESCRIPTION_MAINTAG            = "Prescription";
    const char *const XML_PRESCRIPTION_CIS                = "CIS";
    const char *const XML_PRESCRIPTION_TESTONLY           = "OnlyForTest";
    const char *const XML_PRESCRIPTION_ID                 = "Id";
    const char *const XML_PRESCRIPTION_USEDDOSAGE         = "RefDosage";
    const char *const XML_PRESCRIPTION_CIP                = "CIP";
    const char *const XML_PRESCRIPTION_INTAKEFROM         = "IntakeFrom";
    const char *const XML_PRESCRIPTION_INTAKETO           = "IntakeTo";
    const char *const XML_PRESCRIPTION_INTAKESCHEME       = "IntakeScheme";
    const char *const XML_PRESCRIPTION_INTAKEFROMTO       = "IntakeFromTo";
    const char *const XML_PRESCRIPTION_INTAKEINTERVALTIME = "IntakeIntervalTime";
    const char *const XML_PRESCRIPTION_INTAKEINTERVALSCHEME = "IntakeIntervalScheme";
    const char *const XML_PRESCRIPTION_INTAKEFULLSTRING   = "IntakeFullString";
    const char *const XML_PRESCRIPTION_DURATIONFROM       = "DurationFrom";
    const char *const XML_PRESCRIPTION_DURATIONTO         = "DurationTo";
    const char *const XML_PRESCRIPTION_DURATIONSCHEME     = "DurationScheme";
    const char *const XML_PRESCRIPTION_DURATIONFROMTO     = "DurationFromTo";
    const char *const XML_PRESCRIPTION_PERIOD             = "Period";
    const char *const XML_PRESCRIPTION_PERIODSCHEME       = "PeriodScheme";
    const char *const XML_PRESCRIPTION_DAILYSCHEME        = "Daily";
    const char *const XML_PRESCRIPTION_MEALSCHEME         = "MealTime";
    const char *const XML_PRESCRIPTION_NOTE               = "Note";
    const char *const XML_PRESCRIPTION_ISINN              = "INN";
    const char *const XML_PRESCRIPTION_SPECIFYFORM        = "SpecifyForm";
    const char *const XML_PRESCRIPTION_SPECIFYPRESCENTATION = "SpecifyPresentation";
    const char *const XML_PRESCRIPTION_ISALD              = "IsAld";
    const char *const XML_PRESCRIPTION_TOHTML             = "Html";
    const char *const XML_EXTRADATAS_TAG                  = "ExtraDatas";
    const char *const XML_FULLPRESCRIPTION_TAG            = "FullPrescription";
}

using namespace DrugsIOConstants;
using namespace DrugsDB;
using namespace DrugsDB::Constants;
using namespace Trans::ConstantTranslations;

namespace DrugsDB {
namespace Internal {
/** \brief Private part of DrugsIO \internal */
class DrugsIOPrivate
{
public:
    DrugsIOPrivate()
    {
        m_PrescriptionXmlTags.insert(Prescription::Id ,  XML_PRESCRIPTION_ID);
        m_PrescriptionXmlTags.insert(Prescription::UsedDosage , XML_PRESCRIPTION_USEDDOSAGE);
        m_PrescriptionXmlTags.insert(Prescription::CIP , XML_PRESCRIPTION_CIP);
        m_PrescriptionXmlTags.insert(Prescription::OnlyForTest, XML_PRESCRIPTION_TESTONLY);
        m_PrescriptionXmlTags.insert(Prescription::IntakesFrom , XML_PRESCRIPTION_INTAKEFROM);
        m_PrescriptionXmlTags.insert(Prescription::IntakesTo, XML_PRESCRIPTION_INTAKETO);
        m_PrescriptionXmlTags.insert(Prescription::IntakesScheme, XML_PRESCRIPTION_INTAKESCHEME);
        m_PrescriptionXmlTags.insert(Prescription::IntakesUsesFromTo, XML_PRESCRIPTION_INTAKEFROMTO);
        m_PrescriptionXmlTags.insert(Prescription::IntakesFullString, XML_PRESCRIPTION_INTAKEFULLSTRING);
        m_PrescriptionXmlTags.insert(Prescription::IntakesIntervalOfTime, XML_PRESCRIPTION_INTAKEINTERVALTIME);
        m_PrescriptionXmlTags.insert(Prescription::IntakesIntervalScheme, XML_PRESCRIPTION_INTAKEINTERVALSCHEME);
        m_PrescriptionXmlTags.insert(Prescription::DurationFrom, XML_PRESCRIPTION_DURATIONFROM);
        m_PrescriptionXmlTags.insert(Prescription::DurationTo, XML_PRESCRIPTION_DURATIONTO);
        m_PrescriptionXmlTags.insert(Prescription::DurationScheme, XML_PRESCRIPTION_DURATIONSCHEME);
        m_PrescriptionXmlTags.insert(Prescription::DurationUsesFromTo, XML_PRESCRIPTION_DURATIONFROMTO);
        m_PrescriptionXmlTags.insert(Prescription::Period, XML_PRESCRIPTION_PERIOD);
        m_PrescriptionXmlTags.insert(Prescription::PeriodScheme, XML_PRESCRIPTION_PERIODSCHEME);
        m_PrescriptionXmlTags.insert(Prescription::DailyScheme, XML_PRESCRIPTION_DAILYSCHEME);
        m_PrescriptionXmlTags.insert(Prescription::MealTimeSchemeIndex, XML_PRESCRIPTION_MEALSCHEME);
        m_PrescriptionXmlTags.insert(Prescription::Note, XML_PRESCRIPTION_NOTE);
        m_PrescriptionXmlTags.insert(Prescription::IsINNPrescription, XML_PRESCRIPTION_ISINN);
        m_PrescriptionXmlTags.insert(Prescription::SpecifyForm, XML_PRESCRIPTION_SPECIFYFORM);
        m_PrescriptionXmlTags.insert(Prescription::SpecifyPresentation, XML_PRESCRIPTION_SPECIFYPRESCENTATION);
        m_PrescriptionXmlTags.insert(Prescription::IsALD, XML_PRESCRIPTION_ISALD);
        m_PrescriptionXmlTags.insert(Prescription::ToHtml, XML_PRESCRIPTION_TOHTML);
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
        if (!m_PrescriptionXmlTags.values().contains(xmltag))
            return -1;
        return m_PrescriptionXmlTags.key(xmltag);
    }


public:
    Utils::MessageSender   m_Sender;  /*!< \brief Message sender instance */
    QHash<QString,QString> m_Datas;   /*!< \brief Dosages to transmit : key == uuid, value == xml'd dosage */
    QHash<int,QString>     m_PrescriptionXmlTags;
};
}
}

DrugsIO *DrugsIO::m_Instance = 0;

/** \brief Returns the unique instance of DrugsIO */
DrugsIO *DrugsIO::instance(QObject *parent)
{
    if (!m_Instance) {
        if (parent)
            m_Instance = new DrugsIO(parent);
        else
            m_Instance = new DrugsIO(qApp);
    }
    return m_Instance;
}

/** \brief Private constructor */
DrugsIO::DrugsIO(QObject *parent) : QObject(parent), d(0)
{
    setObjectName("DrugsIO");
    d = new Internal::DrugsIOPrivate();
}

/** \brief Destructor */
DrugsIO::~DrugsIO()
{
    if (d) delete d; d=0;
}

/**
  \brief Start the dosage transmission over the web.
  \todo Manage user's name
*/
bool DrugsIO::startsDosageTransmission()
{
    connect(&d->m_Sender, SIGNAL(sent()), this, SLOT(dosageTransmissionDone()));
    d->m_Datas = Internal::DrugsBase::instance()->getDosageToTransmit();
    if (d->m_Datas.count()==0) {
        return false;
    }
    QStringList list = d->m_Datas.values();
    d->m_Sender.setMessage(list.join("\n\n"));
    d->m_Sender.setUser(qApp->applicationName() + " - " + qApp->applicationVersion());
    d->m_Sender.setTypeOfMessage(Utils::MessageSender::DosageTransmission);
    d->m_Sender.postMessage();
    return true;
}

/**
  \brief Slot called when the dosage transmission is done.
  Marks all dosage as transmitted into the database.
  \sa mfDrugsBase::markAllDosageTransmitted()
*/
void DrugsIO::dosageTransmissionDone()
{
    if (d->m_Sender.resultMessage().contains("OK")) {
        Utils::Log::addMessage(this, tr("Dosages transmitted."));
	Internal::DrugsBase::instance()->markAllDosageTransmitted(d->m_Datas.keys());
    } else
        Utils::Log::addError(this, tr("Dosage not correctly transmitted"));
    d->m_Datas.clear();
    Q_EMIT transmissionDone();
}

/** \brief Return the dosage sending state */
bool DrugsIO::isSendingDosage()
{
    return instance()->d->m_Sender.isSending();
}

/**
  \brief Transfert a XML'd prescription to the model
 */
bool DrugsIO::prescriptionFromXml(DrugsDB::DrugsModel *m, const QString &xml, Loader loader)
{
    Q_ASSERT(m);
    // retreive the prescription (inside the XML_FULLPRESCRIPTION_TAG tags)
    QString start = QString("<%1>").arg(XML_FULLPRESCRIPTION_TAG);
    QString finish = QString("</%1>").arg(XML_FULLPRESCRIPTION_TAG);
    int begin = xml.indexOf(start) + start.length();
    int end = xml.indexOf(finish, begin);
    if (begin==-1 || end==-1) {
        Utils::Log::addError("DrugsIO", tr("Unable to load XML prescription : tag %1 is missing").arg(XML_FULLPRESCRIPTION_TAG));
        return false;
    }
    QString x = xml.mid( begin, end - begin);

    // split full prescription drug by drug
    QString splitter = QString("</%1>").arg(XML_PRESCRIPTION_MAINTAG);
    QStringList drugs = x.split(splitter, QString::SkipEmptyParts );

    // clear model
//    DrugsModel *m = DRUGMODEL;
    Q_ASSERT(m);
    if (loader==ReplacePrescription)
        m->clearDrugsList();

    // rebuild model with serialized prescription
    QHash<QString, QString> hash;
    int row;
    foreach( const QString &s, drugs) {
        if (!Utils::readXml(s+QString("</%1>").arg(XML_PRESCRIPTION_MAINTAG),XML_PRESCRIPTION_MAINTAG,hash,false)) { //tkSerializer::threeCharKeyHashToHash(s);
            Utils::Log::addError("DrugsIO",tr("Unable to read xml prescription"));
            continue;
        }
        if ((hash.isEmpty()) || (!hash.keys().contains(XML_PRESCRIPTION_CIS)))
            continue;
        row = m->addDrug(hash.value(XML_PRESCRIPTION_CIS).toInt(), false);
        hash.remove(XML_PRESCRIPTION_CIS);
        foreach(const QString &k, hash.keys()) {
            m->setData( m->index(row, instance()->d->xmlTagToColumnIndex(k)), hash.value(k) );
        }
        hash.clear();
    }
    m->checkInteractions();
    Utils::Log::addMessage("DrugsIO",tr("Xml prescription correctly read."));
    return true;
}

/**
  \brief Load a Prescription file and assumed the transmission to the DrugsModel
  You can add to or replace the actual prescription using the enumerator DrugsIO::Loader \e loader.\n
  The \e extraDatas receives the extracted extra datas from the loaded prescription file.
  \sa savePrescription()
  \todo manage xml document version
*/
bool DrugsIO::loadPrescription(DrugsDB::DrugsModel *m, const QString &fileName, QHash<QString,QString> &extraDatas, Loader loader )
{
    Q_ASSERT(m);
    QString extras;
    if (!loadPrescription(m, fileName, extras, loader))
        return false;
    Utils::readXml(extras, XML_EXTRADATAS_TAG, extraDatas, false);
    return true;
}


/**
  \brief Load a Prescription file and assumed the transmission to the DrugsModel.
  You can add to or replace the actual prescription using the enumerator DrugsIO::Loader \e loader.\n
  The \e xmlExtraDatas receives the extracted extra datas from the loaded prescription file.
  \sa savePrescription()
  \todo manage xml document version
*/
bool DrugsIO::loadPrescription(DrugsDB::DrugsModel *m, const QString &fileName, QString &xmlExtraDatas, Loader loader )
{
    Q_ASSERT(m);
    if (fileName.isEmpty()) {
        Utils::Log::addError("DrugsIO", tr("No file name passed to load prescription"));
        return false;
    }
    if (!QFile(fileName).exists()) {
        Utils::Log::addError("DrugsIO", tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(fileName));
        return false;
    }
    if (!QFileInfo(fileName).isReadable()) {
        Utils::Log::addError("DrugsIO", tkTr(Trans::Constants::FILE_1_ISNOT_READABLE).arg(fileName));
        return false;
    }
//    DrugsModel *m = DRUGMODEL;
    xmlExtraDatas.clear();
    QString xml = Utils::readTextFile(fileName);

    // retreive prescription
    prescriptionFromXml(m, xml,loader);

    // get extradatas
    QString start = QString("<%1>").arg(XML_EXTRADATAS_TAG);
    QString finish = QString("</%1>").arg(XML_EXTRADATAS_TAG);
    int begin = xml.indexOf(start) + start.length();
    int end = xml.indexOf(finish, begin);
    if (begin==-1 || end==-1) {
        return true;
    }
    xmlExtraDatas = xml.mid( begin, end - begin);
    m->reset();
    return true;
}

/**
  \brief Transform prescription to readable Html.
  Prescription is automatically sorted.\n
  The XML encoded prescription is added inside the HTML code.\n
*/
QString DrugsIO::prescriptionToHtml(DrugsDB::DrugsModel *m)
{
    Q_ASSERT(m);
    // clean the model (sort it, hide testing drugs)
//    DrugsModel *m = DRUGMODEL;
    if (m->rowCount() <= 0)
        return QString();

    // keep trace of actual state of the model, then hide testing drugs
    bool testingDrugsVisible = m->testingDrugsAreVisible();
    m->showTestingDrugs(false);
    // sort
    m->sort(0);

    Core::ISettings *s = Core::ICore::instance()->settings();

    // Prepare font format
//    QFont drugsFont;
//    drugsFont.fromString(s->value(MFDRUGS_SETTING_DRUGFONT).toString());
//    QFont prescrFont;
//    prescrFont.fromString(s->value(MFDRUGS_SETTING_PRESCRIPTIONFONT).toString());

    QString ALD, nonALD;
    QString tmp;
    // Add drugs
    int i;
//    QString drugStyle, prescrStyle;
//    drugStyle = Utils::fontToHtml(drugsFont, "black");
//    prescrStyle = Utils::fontToHtml(prescrFont, "black");// + "margin-left:20px;";
    for(i=0; i < m->rowCount(); ++i) {
        tmp = "<li>" + m->index(i, Prescription::ToHtml).data().toString();
        if (s->value(S_PRINTLINEBREAKBETWEENDRUGS).toBool())
            tmp += "<span style=\"font-size:4pt\"><br /></span>";
        tmp += "</li>";
//        tmp = QString(ENCODEDHTML_DRUG);
//        tmp.replace( "{NUMBER}", QString::number(i+1));
//        tmp.replace( "{DRUGSTYLE}", drugStyle);
//        if (m->index(i,Prescription::IsINNPrescription).data().toBool()) {
//            tmp.replace( "{DRUG}", m->index(i,Drug::InnCompositionString).data().toString() + " - " + tr("[INN]"));
//        } else {
//            tmp.replace( "{DRUG}", m->index( i, Drug::Denomination ).data().toString());
//        }
//        tmp.replace( "{PRESCRIPTIONSTYLE}", prescrStyle );
//        tmp.replace( "{PRESCRIPTION}", m->index( i, Prescription::ToHtml ).data().toString());
        //        tmp.replace( "{NOTE}", index( i, Prescription::Note).data().toString());

        if (m->index( i, Prescription::IsALD ).data().toBool()) {
            ALD += tmp;
        } else {
            nonALD += tmp;
        }
        tmp.clear();
    }
    if (!ALD.isEmpty()) {
        tmp = s->value(S_ALD_PRE_HTML).toString();
        tmp += QString(ENCODEDHTML_FULLPRESCRIPTION).replace( "{FULLPRESCRIPTION}", ALD );
        tmp += s->value(S_ALD_POST_HTML).toString();
    }
    if (!nonALD.isEmpty()) {
        tmp += QString(ENCODEDHTML_FULLPRESCRIPTION).replace( "{FULLPRESCRIPTION}", nonALD );
    }

    // show all drugs (including testing to get the testing drugs)
    m->showTestingDrugs(true);
    QString toReturn;
    toReturn = QString(ENCODEDHTML_FULLDOC);
    toReturn.replace("{GENERATOR}", qApp->applicationName());
    toReturn.replace("{PRESCRIPTION}", tmp );
    toReturn.replace("{ENCODEDPRESCRIPTION}", QString("%1%2")
                     .arg(ENCODEDHTML_FREEDIAMSTAG)
                     .arg(QString(prescriptionToXml(m).toAscii().toBase64())));

    // return to the state of the model
    m->showTestingDrugs(testingDrugsVisible);
    return toReturn;
}


/**
  \brief Transforms the DrugsModel's prescription into a XML encoded string.
*/
QString DrugsIO::prescriptionToXml(DrugsDB::DrugsModel *m)
{
    Q_ASSERT(m);
//    DrugsModel *m = DRUGMODEL;
    if (!m->testingDrugsAreVisible()) {
        bool yes = Utils::yesNoMessageBox(tr("Save test only drugs too ?"),
                                  tr("Drugs added for testing only are actually hidden in this prescription.\n"
                                     "Do you want to add them to the file ?"),
                                  tr("Answering 'no' will cause definitive lost of test only drugs when "
                                     "reloading this file."));
        if (yes)
            m->showTestingDrugs(true);
    }
    QString xmldPrescription;
    QList<int> keysToSave;
    keysToSave << Prescription::IntakesFrom << Prescription::IntakesTo
            << Prescription::IntakesScheme << Prescription::IntakesUsesFromTo << Prescription::DurationFrom
            << Prescription::DurationTo << Prescription::DurationScheme << Prescription::DurationUsesFromTo
            << Prescription::Period << Prescription::PeriodScheme <<  Prescription::MealTimeSchemeIndex
            << Prescription::Note << Prescription::IsINNPrescription << Prescription::SpecifyForm
            << Prescription::SpecifyPresentation << Prescription::IsALD << Prescription::CIP
            << Prescription::IntakesIntervalOfTime << Prescription::IntakesIntervalScheme
            << Prescription::UsedDosage;
    QHash<QString, QString> forXml;
    int i;
    for(i=0; i<m->rowCount() ; ++i) {
        forXml.insert(XML_PRESCRIPTION_CIS, m->index(i, Drug::CIS).data().toString());
        if (m->index(i, Prescription::OnlyForTest).data().toBool()) {
            forXml.insert(instance()->d->xmlTagForPrescriptionRow(Prescription::OnlyForTest), "true");
        } else {
            foreach(int k, keysToSave) {
                forXml.insert( instance()->d->xmlTagForPrescriptionRow(k), m->index(i, k).data().toString() );
            }
        }
        xmldPrescription += Utils::createXml(XML_PRESCRIPTION_MAINTAG, forXml,4,false); // += "[Drug]" + tkSerializer::threeCharKeyHashToString(serializeIt);
        forXml.clear();
    }
    xmldPrescription.prepend( QString("<%1>\n").arg(XML_FULLPRESCRIPTION_TAG));
    xmldPrescription.append( QString("</%1>\n").arg(XML_FULLPRESCRIPTION_TAG));
    return xmldPrescription;
}

/**
  \brief Save the DrugsModel's prescription into a XML file.
  You can add \e extraDatas to the xml. \e extraDatas must be xml'd.\n
  If \e toFileName is empty, user is prompted to select a filename.
*/
bool DrugsIO::savePrescription(DrugsDB::DrugsModel *model, const QHash<QString,QString> &extraDatas, const QString &toFileName)
{
    Q_ASSERT(model);
    QString xmldPrescription = prescriptionToXml(model);
    // add extraDatas
    if (!extraDatas.isEmpty()) {
        QString tmp = Utils::createXml(XML_EXTRADATAS_TAG,extraDatas,4,false);
        xmldPrescription.append(tmp);
    }
    // add xml doc version to the beginning of the doc
    xmldPrescription.prepend(XML_VERSION);
    if (toFileName.isEmpty())
        return Utils::saveStringToFile(xmldPrescription,
                                      QDir::homePath() + "/prescription.di",
                                      tr(Core::Constants::FREEDIAMS_FILEFILTER)) ;
    else
        return Utils::saveStringToFile(xmldPrescription, toFileName, Utils::DontWarnUser);
}

/**
  \brief Save the DrugsModel's prescription into a XML file.
  You can add \e extraXmlDatas to the xml. \e extraDatas must be xml'd.\n
  If \e toFileName is empty, user is prompted to select a filename.
*/
bool DrugsIO::savePrescription(DrugsDB::DrugsModel *model, const QString &extraXmlDatas, const QString &toFileName)
{
    Q_ASSERT(model);
    QString xmldPrescription = prescriptionToXml(model);
    // add extraDatas
    if (!extraXmlDatas.isEmpty()) {
        xmldPrescription.append(QString("\n<%1>\n").arg(XML_EXTRADATAS_TAG));
        xmldPrescription.append(extraXmlDatas);
        xmldPrescription.append(QString("\n</%1>\n").arg(XML_EXTRADATAS_TAG));
    }
    // add xml doc version to the beginning of the doc
    xmldPrescription.prepend(XML_VERSION);
    if (toFileName.isEmpty())
        return Utils::saveStringToFile(xmldPrescription,
                                      QDir::homePath() + "/prescription.di",
                                      tr(Core::Constants::FREEDIAMS_FILEFILTER)) ;
    else
        return Utils::saveStringToFile(xmldPrescription, toFileName, Utils::DontWarnUser);
}

