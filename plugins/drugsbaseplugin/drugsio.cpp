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

#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/versionupdater.h>
#include <drugsbaseplugin/dailyschememodel.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>

#include <printerplugin/printer.h>
#include <printerplugin/constants.h>

#include <translationutils/constanttranslations.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/messagesender.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>

#ifdef FREEDIAMS
#  include <fdcoreplugin/patient.h>
#endif

#include <QApplication>
#include <QHash>
#include <QString>
#include <QDir>

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}


namespace DrugsIOConstants {
    const char *const XML_HEADER                          = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    const char *const XML_DRUGS_DATABASE_NAME             = "DrugsDatabaseName";
    const char *const XML_VERSION                         = "version";
    const char *const XML_PRESCRIPTION_MAINTAG            = "Prescription";
    const char *const XML_PRESCRIPTION_ISTEXTUAL          = "IsTextual";
    const char *const XML_PRESCRIPTION_TEXTUALDRUGNAME    = "TextualDrugName";
    const char *const XML_PRESCRIPTION_CIS                = "Drug_UID";
    const char *const XML_PRESCRIPTION_TESTONLY           = "OnlyForTest";
    const char *const XML_PRESCRIPTION_ID                 = "Id";
    const char *const XML_PRESCRIPTION_USEDDOSAGE         = "RefDosage";
    const char *const XML_PRESCRIPTION_CIP                = "Pack_UID";
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
        m_PrescriptionXmlTags.insert(Prescription::IsTextualOnly , XML_PRESCRIPTION_ISTEXTUAL);
        m_PrescriptionXmlTags.insert(Prescription::Pack_UID , XML_PRESCRIPTION_CIP);
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

    ~DrugsIOPrivate()
    {
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
    if (d) {
        delete d;
        d=0;
    }
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
bool DrugsIO::prescriptionFromXml(DrugsDB::DrugsModel *m, const QString &xmlContent, Loader loader)
{
    Q_ASSERT(m);
    QString xml = xmlContent;

    // check prescription encoding version
    bool needUpdate = (!DrugsDB::VersionUpdater::instance()->isXmlIOUpToDate(xml));
    QString version;
    if (needUpdate) {
        version = DrugsDB::VersionUpdater::instance()->xmlVersion(xmlContent);
        Utils::Log::addMessage("DrugsIO::prescriptionFromXml", "Reading old prescription file : version " + version);
        xml = DrugsDB::VersionUpdater::instance()->updateXmlIOContent(xml);
    }

    // Check if the drugs database correspond to the actual one
    QString xmlDbName = DrugsDB::Constants::DEFAULT_DATABASE_IDENTIFIANT;
    QString start = QString("<%1>").arg(XML_DRUGS_DATABASE_NAME);
    QString finish;
    if (xml.contains(start)) {
        int begin = xml.indexOf(start) + start.length();
        finish = QString("</%1").arg(XML_DRUGS_DATABASE_NAME);
        int end = xml.indexOf(finish, begin);
        if (begin==-1 || end==-1) {
            Utils::Log::addError("DrugsIO", tr("Unable to load XML prescription : tag %1 is missing").arg(XML_DRUGS_DATABASE_NAME));
            return false;
        }
        xmlDbName = xml.mid( begin, end - begin);
    }
    if (drugsBase()->actualDatabaseInformations()->identifiant != xmlDbName) {
        Utils::Log::addError("DrugsIO", QString("Try to load a prescription from another drugs database. Actual: %1 ; Xml: %2")
                             .arg(drugsBase()->actualDatabaseInformations()->identifiant, xmlDbName));
        Utils::warningMessageBox(tr("Prescription specifies a different drugs database than the actual one."),
                                 tr("You are trying to load prescription that uses a different drugs database than the "
                                    "actual one. You can not read this prescription unless you change the current "
                                    "database in the Preferences.\n"
                                    "Actual: %1\n"
                                    "Prescription: %2.")
                                 .arg(drugsBase()->actualDatabaseInformations()->identifiant, xmlDbName));
        return false;
    }

    // retreive the prescription (inside the XML_FULLPRESCRIPTION_TAG tags)
    start = QString("<%1").arg(XML_FULLPRESCRIPTION_TAG);
    finish = QString("</%1>").arg(XML_FULLPRESCRIPTION_TAG);
    int begin = xml.indexOf(start) + start.length();
    begin = xml.indexOf("\">", begin) + 2;
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
    Q_ASSERT(m);
    if (loader==ReplacePrescription)
        m->clearDrugsList();

    // build model with serialized prescription
    QHash<QString, QString> hash;
    QList<int> rowsToUpdate;
    int row;
    foreach(const QString &s, drugs) {
        // Some verifications
        if (!Utils::readXml(s+QString("</%1>").arg(XML_PRESCRIPTION_MAINTAG), XML_PRESCRIPTION_MAINTAG,hash,false)) {
            Utils::Log::addError("DrugsIO",tr("Unable to read xml prescription"));
            continue;
        }
        if ((hash.isEmpty()) || (!hash.keys().contains(XML_PRESCRIPTION_CIS)))
            continue;

        // Add infos to the model
        if (hash.value(XML_PRESCRIPTION_ISTEXTUAL).compare("true",Qt::CaseInsensitive) == 0) {
            row = m->addTextualPrescription(hash.value(XML_PRESCRIPTION_TEXTUALDRUGNAME), "");
        } else {
            row = m->addDrug(hash.value(XML_PRESCRIPTION_CIS).toInt(), false);
        }
        hash.remove(XML_PRESCRIPTION_CIS);
        foreach(const QString &k, hash.keys()) {
            m->setData( m->index(row, instance()->d->xmlTagToColumnIndex(k)), hash.value(k) );
        }
        hash.clear();

        // check Model Updaters
        if (needUpdate) {
            rowsToUpdate.append(row);
        }
    }
    if ((needUpdate) && (!version.isEmpty())){
        DrugsDB::VersionUpdater::instance()->updateXmlIOModel(version, m, rowsToUpdate);
    }

    // check interaction, emit final signal from model for views to update
    m->checkInteractions();
    Q_EMIT m->numberOfRowsChanged();

    // small debug information
    Utils::Log::addMessage("DrugsIO", tr("Xml prescription correctly read."));
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
QString DrugsIO::prescriptionToHtml(DrugsDB::DrugsModel *m, int version)
{
    Q_ASSERT(m);
    // clean the model (sort it, hide testing drugs)
    if (m->rowCount() <= 0)
        return QString();

    // keep trace of actual state of the model, then hide testing drugs
    bool testingDrugsVisible = m->testingDrugsAreVisible();
    m->showTestingDrugs(false);
    // sort
    m->sort(0);

    QString ALD, nonALD;
    QString tmp;
    bool lineBreak = settings()->value(S_PRINTLINEBREAKBETWEENDRUGS).toBool();
    // Add drugs
    int i;
    switch (version)
    {
    case MedinTuxVersion :
        {
            for(i=0; i < m->rowCount(); ++i) {
                tmp = "<li>" + m->index(i, Prescription::ToHtml).data().toString();
                if (lineBreak)
                    tmp += "<span style=\"font-size:4pt\"><br /></span>";
                tmp += "</li>";
                if (m->index(i, Prescription::IsALD).data().toBool()) {
                    ALD += tmp;
                } else {
                    nonALD += tmp;
                }
                tmp.clear();
            }
            break;
        }
    case SimpleVersion :
        {
            QHash<QString, QString> tokens_value;
            for(i=0; i < m->rowCount(); ++i) {
                tokens_value.insert("DRUG", m->index(i, Drug::Denomination).data().toString());
                tokens_value.insert("Q_FROM", m->index(i, Prescription::IntakesFrom).data().toString());
                tokens_value.insert("Q_TO", m->index(i, Prescription::IntakesTo).data().toString());
                tokens_value.insert("Q_SCHEME", m->index(i, Prescription::IntakesScheme).data().toString());
                // Manage Daily Scheme See DailySchemeModel::setSerializedContent
                DrugsDB::DailySchemeModel *day = new DrugsDB::DailySchemeModel;
                day->setSerializedContent(m->index(i, Prescription::DailyScheme).data().toString());
                QString d = day->humanReadableDistributedDailyScheme();
                if (d.isEmpty())
                    d = day->humanReadableRepeatedDailyScheme();
                tokens_value.insert("DAILY_SCHEME", d);
                tmp = "<li>[[DRUG]], [[Q_FROM]][ - [Q_TO]][ [Q_SCHEME]][ [DAILY_SCHEME]]";
                Utils::replaceTokens(tmp, tokens_value);
                if (lineBreak)
                    tmp += "<span style=\"font-size:4pt\"><br /></span>";
                tmp += "</li>";
                if (m->index(i, Prescription::IsALD).data().toBool()) {
                    ALD += tmp;
                } else {
                    nonALD += tmp;
                }
                tmp.clear();
            }
            break;

        }
    case DrugsOnlyVersion :
        {
            for(i=0; i < m->rowCount(); ++i) {
                tmp = m->index(i, Drug::Denomination).data().toString();
                tmp = tmp.mid(0, tmp.indexOf(","));
                tmp.prepend("<li>");
                tmp.append("</li>\n");
                if (m->index(i, Prescription::IsALD).data().toBool()) {
                    ALD += tmp;
                } else {
                    nonALD += tmp;
                }
                tmp.clear();
            }
            break;
        }
    }

    if (!ALD.isEmpty()) {
        tmp = settings()->value(S_ALD_PRE_HTML).toString();
        if (version==MedinTuxVersion)
            tmp += QString(ENCODEDHTML_FULLPRESCRIPTION_MEDINTUX).replace("{FULLPRESCRIPTION}", ALD);
        else
            tmp += QString(ENCODEDHTML_FULLPRESCRIPTION_NON_MEDINTUX).replace("{FULLPRESCRIPTION}", ALD);
        tmp += settings()->value(S_ALD_POST_HTML).toString();
    }
    if (!nonALD.isEmpty()) {
        if (version==MedinTuxVersion)
            tmp += QString(ENCODEDHTML_FULLPRESCRIPTION_MEDINTUX).replace("{FULLPRESCRIPTION}", nonALD);
        else
            tmp += QString(ENCODEDHTML_FULLPRESCRIPTION_NON_MEDINTUX).replace("{FULLPRESCRIPTION}", nonALD);
    }

    // show all drugs (including testing to get the testing drugs)
    m->showTestingDrugs(testingDrugsVisible);
    QString toReturn;
    if (version==MedinTuxVersion)
        toReturn = QString(ENCODEDHTML_FULLDOC_MEDINTUX);
    else
        toReturn = QString(ENCODEDHTML_FULLDOC_NON_MEDINTUX);

    toReturn.replace("{GENERATOR}", qApp->applicationName());
    toReturn.replace("{PRESCRIPTION}", tmp );
    toReturn.replace("{ENCODEDPRESCRIPTION}", QString("%1%2")
                     .arg(ENCODEDHTML_FREEDIAMSTAG)
                     .arg(QString(prescriptionToXml(m).toAscii().toBase64())));

//    Utils::saveStringToFile(toReturn,"/Users/eric/Desktop/essai.html");

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
        << Prescription::DailyScheme
        << Prescription::MealTimeSchemeIndex
        << Prescription::IntakesIntervalOfTime
        << Prescription::IntakesIntervalScheme
        << Prescription::Note
        << Prescription::IsINNPrescription
        << Prescription::SpecifyForm
        << Prescription::SpecifyPresentation
        << Prescription::IsALD
        ;
    QHash<QString, QString> forXml;
    int i;
    for(i=0; i<m->rowCount() ; ++i) {
        forXml.insert(XML_PRESCRIPTION_CIS, m->index(i, Drug::UID).data().toString());
        if (m->index(i, Prescription::OnlyForTest).data().toBool()) {
            forXml.insert(instance()->d->xmlTagForPrescriptionRow(Prescription::OnlyForTest), "true");
        } else {
            foreach(int k, keysToSave) {
                forXml.insert( instance()->d->xmlTagForPrescriptionRow(k), m->index(i, k).data().toString() );
            }
        }
        if (m->index(i, Prescription::IsTextualOnly).data().toBool()) {
            forXml.insert(XML_PRESCRIPTION_TEXTUALDRUGNAME,
                          m->index(i, Drug::Denomination).data().toString());
        }
        xmldPrescription += Utils::createXml(XML_PRESCRIPTION_MAINTAG, forXml,4,false);
        forXml.clear();
    }
    // Add XmlVersion XML_HEADER
    xmldPrescription.prepend(QString("<%1 %2=\"%3\">\n")
                             .arg(XML_FULLPRESCRIPTION_TAG)
                             .arg(XML_VERSION).arg(VersionUpdater::instance()->lastXmlIOVersion()));
    // Add drugsBase identifiant
    QString dbName = drugsBase()->actualDatabaseInformations()->identifiant;
    xmldPrescription.prepend(QString("<%1>%2</%1>\n").arg(XML_DRUGS_DATABASE_NAME).arg(dbName));
    // Add the version and the FullPrescription tags
    xmldPrescription.prepend(QString("%1\n").arg(XML_HEADER));
    xmldPrescription.append(QString("</%1>\n").arg(XML_FULLPRESCRIPTION_TAG));
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
    if (toFileName.isEmpty())
        return Utils::saveStringToFile(xmldPrescription,
                                      QDir::homePath() + "/prescription.di",
                                      tr(Core::Constants::FREEDIAMS_FILEFILTER)) ;
    else
        return Utils::saveStringToFile(xmldPrescription, toFileName, Utils::DontWarnUser);
}

static QString prepareHeader()
{
    QString header;
#ifdef FREEDIAMS
    Core::Patient *patient = Core::ICore::instance()->patient();
    header = settings()->value(Constants::S_USERHEADER).toString();
    patient->replaceTokens(header);
    /** \todo Create a prefs for Date format */
    Utils::replaceToken(header, Core::Constants::TOKEN_DATE, QDate::currentDate().toString(QLocale().dateFormat()));
#endif
    return header;
}

static QString prepareFooter(DrugsDB::DrugsModel *model)
{
    QString footer;
#ifdef FREEDIAMS
    footer = settings()->value(Constants::S_USERFOOTER).toString();
    footer.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(QCoreApplication::translate("DrugsIO", "Made with FreeDiams.")));
    Utils::replaceToken(footer, Core::Constants::TOKEN_NUMBEROFDRUGS, QString::number(model->rowCount()));
#else
    footer = "Work in progress";
#endif
    return footer;
}

bool DrugsIO::printPrescription(DrugsDB::DrugsModel *model)
{
    Print::Printer p;
    if (!p.getUserPrinter())
        if (!p.askForPrinter(qApp->activeWindow()))
            return false;
    QString footer;
#ifdef FREEDIAMS
    Core::Patient *patient = Core::ICore::instance()->patient();
    footer = prepareFooter(model);
    p.printer()->setDocName("FreeDiams - " + patient->value(Core::Patient::FullName).toString().leftJustified(50,'_'));
    p.addHtmlWatermark(settings()->value(Constants::S_WATERMARK_HTML).toString(),
                       Print::Printer::Presence(settings()->value(Constants::S_WATERMARKPRESENCE).toInt()),
                       Qt::AlignmentFlag(settings()->value(Constants::S_WATERMARKALIGNEMENT).toInt()));
#else
    /** \todo FreeMedForms prescription printing */
    footer = "<body><p>Work in progress</p></body>";
    footer.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(tr("Made with FreeMedForms.")));
#endif
    p.setHeader(prepareHeader());
    p.setFooter(footer);
    p.previewer(qApp->activeWindow());
    p.printWithDuplicata(settings()->value(Constants::S_PRINTDUPLICATAS).toBool());
    /** \todo Use NormalVersion instead of MedinTuxversion */
    return p.print(DrugsDB::DrugsIO::prescriptionToHtml(model, DrugsIO::MedinTuxVersion));
}

void DrugsIO::prescriptionPreview(DrugsDB::DrugsModel *model)
{
    Print::Printer p;
    QPrinter *printer = new QPrinter;
    printer->setPageSize(QPrinter::A4);
    printer->setColorMode(QPrinter::ColorMode(settings()->value(Print::Constants::S_COLOR_PRINT).toInt()));
    p.setPrinter(printer);
    QString footer;
#ifdef FREEDIAMS
    Core::Patient *patient = Core::ICore::instance()->patient();
    footer = prepareFooter(model);
    p.printer()->setDocName("FreeDiams - " + patient->value(Core::Patient::FullName).toString().leftJustified(50,'_'));
    p.addHtmlWatermark(settings()->value(Constants::S_WATERMARK_HTML).toString(),
                       Print::Printer::Presence(settings()->value(Constants::S_WATERMARKPRESENCE).toInt()),
                       Qt::AlignmentFlag(settings()->value(Constants::S_WATERMARKALIGNEMENT).toInt()));
#else
    /** \todo FreeMedForms prescription printing */
    QString header = "Work in progress";
    footer = "<body><p>Work in progress</p></body>";
    footer.replace("</body>",QString("<br /><span style=\"align:left;font-size:6pt;color:black;\">%1</span></p></body>")
                   .arg(tr("Made with FreeMedForms.")));
#endif
    p.setHeader(prepareHeader());
    p.setFooter(footer);
    /** \todo Use NormalVersion instead of MedinTuxversion */
    p.setContent(DrugsDB::DrugsIO::prescriptionToHtml(model, DrugsIO::MedinTuxVersion));
    p.previewDialog(qApp->activeWindow());
    // QPrinter is deleted by ~Printer
}

/**
  \brief For drag and drop functionnalities, defines the mimeTypes of DrugsIO.
  \sa DrugsDB::DrugsModel::mimeType()
*/
QStringList DrugsIO::prescriptionMimeTypes()
{
    return QStringList() << "application/prescription";
}

