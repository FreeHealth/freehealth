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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DrugsDB::Internal::PrescriptionPrinterJob
 */

/*!
 * \class DrugsDB::Internal::PrescriptionPrinter
 */

#include "prescriptionprinter.h"
#include "drugsmodel.h"
#include "constants_html.h"
#include "constants.h"
#include "drugbasecore.h"
#include "drugsio.h"

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/idocumentprinter.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_units.h>
#include <translationutils/trans_patient.h>
#include <extensionsystem/pluginmanager.h>

#include <QApplication>
#include <QDebug>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}
static inline DrugsDB::DrugsIO &drugsIo() {return DrugsDB::DrugBaseCore::instance().drugsIo();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace DrugsDB {
namespace Internal {
class PrescriptionPrinterJobPrivate
{
public:
    PrescriptionPrinterJobPrivate(PrescriptionPrinterJob *parent) :
        _uid(0),
        _drugsModel(0),
        _lineBreakBetweenDrugs(true),
        _sortOnPrinting(true),
        _addPatientBiometrics(true),
        _printDuplicates(true),
        _outputType(PrescriptionPrinterJob::FullHtml),
        q(parent)
    {
        Q_UNUSED(q);
        static int handle = 1;
        _uid = handle;
        ++handle;
    }

    ~PrescriptionPrinterJobPrivate()
    {
    }

public:
    int _uid;
    DrugsModel *_drugsModel;
    bool _lineBreakBetweenDrugs, _sortOnPrinting, _addPatientBiometrics, _printDuplicates;
    PrescriptionPrinterJob::OutputType _outputType;
    QString _xmlExtraData;

private:
    PrescriptionPrinterJob *q;
};


class PrescriptionPrinterPrivate
{
public:
    PrescriptionPrinterPrivate(PrescriptionPrinter *parent) :
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~PrescriptionPrinterPrivate()
    {
    }
    
    QString getDrugPosologicSentence(const PrescriptionPrinterJob &job, int drugIndex)
    {
        switch (job.outputType()) {
        case PrescriptionPrinterJob::FullHtml:
        {
            QString tmp;
            // Get full prescription sentence in HTML
            tmp = job.drugsModel()->data(job.drugsModel()->index(drugIndex, Constants::Prescription::ToHtml)).toString();
            // Add a line break
            if (job.addLineBreakBetweenEachDrugs())
                tmp += "<span style=\"font-size:4pt\"><br /></span>";
             tmp.prepend("<li>");
             tmp.append("</li>");
            tmp += "\n\n";
            return tmp;
        }
        case PrescriptionPrinterJob::DrugsNameOnly :
        {
            QString tmp;
            tmp = job.drugsModel()->index(drugIndex, Constants::Drug::Denomination).data().toString();
            tmp = tmp.mid(0, tmp.indexOf(","));
             tmp.prepend("<li>");
             tmp.append("</li>");
            tmp += "\n\n";
            return tmp;
        }
        default: return QString();
        } // switch
        return QString();
    }

    // Return ALD pre html document from the user settings
    QString getPreAldHtml()
    {
        // Add ALD Pre text from user settings
        QString aldPre = settings()->value(Constants::S_ALD_PRE_HTML).toString();

        // Keep only body content
        QString css = Utils::htmlTakeAllCssContent(aldPre);
        aldPre = Utils::htmlBodyContent(aldPre);
        aldPre.prepend(css);
        aldPre.prepend("\n\n<!-- PRE ALD --> \n");
        aldPre.append("\n<!-- PRE ALD END --> \n\n");
        return aldPre;
    }

    // Return ALD post html document from the user settings
    QString getPostAldHtml()
    {
        // Add ALD Post text from user settings
        QString aldPost = settings()->value(Constants::S_ALD_POST_HTML).toString();

        // Keep only body content
        QString css = Utils::htmlTakeAllCssContent(aldPost);
        aldPost = Utils::htmlBodyContent(aldPost);
        aldPost.prepend(css);
        aldPost.prepend("\n\n<!-- POST ALD --> \n");
        aldPost.append("\n<!-- POST ALD END --> \n\n");
        return aldPost;
    }

    // Return a block of prescription lines. Each drug is representing a line.
    QString getPrescriptionBlock(const QStringList &lines)
    {
        return QString(
                    "<table border=0>\n"
                    " <tr>\n"
                    "   <td>\n"
                    "<ol>\n"
                    "%1\n"
                    "</ol>\n"
                    "   </td>\n"
                    " </tr>\n"
                    "</table>\n"
                    ).arg(lines.join(""));
    }

    // Return the Html header for the prescription file
    QString getHtmlHeader(const QString &xmlBase64)
    {
        return QString(
                    "<head>\n"
                    "  <meta name=\"qrichtext\" content=\"1\" />\n"
                    "  <meta name=\"charset\" content=\"UTF-8\" />\n"
                    "  <meta name=\"generator\" content=\"%1-%2\" />\n"
                    "  <meta name=\"prescription\" content=\"%3\" />\n"
                    "  <title>%4</title>\n"
                    "  <style type=\"text/css\">\n"
                    "    A:link {text-decoration: none}\n"
                    "    A:visited {text-decoration: none}\n"
                    "    A:active {text-decoration: none}\n"
                    "    A:hover {text-decoration: none;}\n"
                    "  </style>\n"
                    "</head>\n"
                    )
                .arg(qApp->applicationName())
                .arg(qApp->applicationVersion())
                .arg(xmlBase64)
                .arg(QApplication::translate("PrescriptionPrinter", "Pharmaceutical Drugs Prescription"))
                ;
    }

    // Returns the prescription transformed to Html
    QString prescriptionToHtml(const PrescriptionPrinterJob &job)
    {
        Q_ASSERT(job.drugsModel());

        // No drugs?
        if (job.drugsModel()->rowCount() <= 0)
            return QString();

        // Cache testing drugs visibility
        bool testingDrugsVisible = job.drugsModel()->testingDrugsAreVisible();
        job.drugsModel()->showTestingDrugs(false);

        // Sort drugsModel if required
        if (job.sortBeforePrinting())
            job.drugsModel()->sort(0);

        QStringList aldLines, nonAldLines;
        QString tmp;

        // Extract drugs
        QString css;
        for(int i=0; i < job.drugsModel()->rowCount(); ++i) {
            // Get drugs HTML posologic sentence
            tmp = getDrugPosologicSentence(job, i);

            // Extract CSS
            if (css.isEmpty())
                css = Utils::htmlTakeAllCssContent(tmp);
            else
                Utils::htmlTakeAllCssContent(tmp);

            // Split drugs according to their IsALD value
            if (job.drugsModel()->index(i, Constants::Prescription::IsALD).data().toBool()) {
                aldLines << tmp;
            } else {
                nonAldLines << tmp;
            }
        }

        // Create the XML'd prescription content
        job.drugsModel()->showTestingDrugs(testingDrugsVisible);
        QString xmlBase64 = drugsIo().prescriptionToXml(job.drugsModel(), job.xmlExtraData()).toUtf8().toBase64();

        // Start the HTML content
        QString body;
        body = "<html>\n";
        body += getHtmlHeader(xmlBase64);
        body += "<body>\n";
        body += css;
        body += QString("\n\n<a href=%1%2 style=\"color:black\">\n\n").arg(Constants::ENCODEDHTML_FREEDIAMSTAG).arg(xmlBase64);
        body += job.patientBiometricsToHtml();

        if (!aldLines.isEmpty()) {
            // Add Pre Ald Html code
            body += getPreAldHtml();

            // Add drugs to the prescription
            body += Utils::htmlReplaceAccents(getPrescriptionBlock(aldLines));

            // Add Post Ald Html code
            body += getPostAldHtml();
        }

        if (!nonAldLines.isEmpty()) {
            // Add drugs to the prescription
            body += Utils::htmlReplaceAccents(getPrescriptionBlock(nonAldLines));
        }

        body += "\n\n</a>\n";
        body += "</body>\n";
        body += "</html>\n";

        // Utils::saveStringToFile(body, "/Users/eric/Desktop/essai.html");

        // Reset the state of the model
        job.drugsModel()->showTestingDrugs(testingDrugsVisible);

        // Return body
        return body;
    }
    
private:
    PrescriptionPrinter *q;
};
} // namespace Internal
} // end namespace DrugsDB

PrescriptionPrinterJob::PrescriptionPrinterJob() :
    d(new PrescriptionPrinterJobPrivate(this))
{
}

PrescriptionPrinterJob::~PrescriptionPrinterJob()
{
    if (d)
        delete d;
    d = 0;
}

/**
 * Read user settings:
 * - get default print version (todo)
 * - get all boolean of this class
 */
void PrescriptionPrinterJob::readSettings()
{
    // TODO: add a settings to set the default print PrescriptionPrinterJob::OutputType
    d->_lineBreakBetweenDrugs = settings()->value(DrugsDB::Constants::S_PRINTLINEBREAKBETWEENDRUGS).toBool();
    d->_sortOnPrinting = settings()->value(DrugsDB::Constants::S_AUTOSORT_ON_PRINTING).toBool();
    d->_addPatientBiometrics = settings()->value(DrugsDB::Constants::S_ADD_BIOMETRICS_ON_PRINTS).toBool();
    d->_printDuplicates = settings()->value(Constants::S_PRINTDUPLICATAS).toBool();
}

/** Defines the type of printing according to the \e type */
void PrescriptionPrinterJob::setVersion(OutputType type)
{
    d->_outputType = type;
}

/** Returns the type of printing */
PrescriptionPrinterJob::OutputType PrescriptionPrinterJob::outputType() const
{
    return d->_outputType;
}

/** Returns true if the drugsmodel must be sorted before the printing */
bool PrescriptionPrinterJob::sortBeforePrinting() const
{
    return d->_sortOnPrinting;
}

bool PrescriptionPrinterJob::addLineBreakBetweenEachDrugs() const
{
    return d->_lineBreakBetweenDrugs;
}

bool PrescriptionPrinterJob::printDuplicates() const
{
   return d->_printDuplicates;
}

/** Add patient biometrics to the prescription output? true/false */
bool PrescriptionPrinterJob::addPatientBiometrics() const
{
    return d->_addPatientBiometrics;
}

/** Prepare patient biometrics html output */
QString PrescriptionPrinterJob::patientBiometricsToHtml() const
{
    if (!d->_addPatientBiometrics)
        return QString();
    QStringList bio;

    // Include Weight
    // FIXME: use tkTr(Trans::Constants::_1_COLON_2) "%1: %2" translation
    if (!patient()->data(Core::IPatient::WeightInGrams).toString().isEmpty()
            && patient()->data(Core::IPatient::WeightInGrams).toDouble() > 0.) {
        bio << QString("%1: %2 %3")
               .arg(tkTr(Trans::Constants::WEIGHT))
               .arg(QString::number(patient()->data(Core::IPatient::WeightInGrams).toDouble() / 100, 'f', 2))
               .arg(tkTr(Trans::Constants::KILOGRAM_S));
    }

    // Include Height
    if (!patient()->data(Core::IPatient::HeightInCentimeters).toString().isEmpty()
            && patient()->data(Core::IPatient::HeightInCentimeters).toDouble() > 0.) {
        bio << QString("%1: %2 %3")
               .arg(tkTr(Trans::Constants::HEIGHT))
               .arg(QString::number(patient()->data(Core::IPatient::HeightInCentimeters).toDouble(), 'f', 2))
               .arg(tkTr(Trans::Constants::CENTIMETER_S));
    }

    // Include CreatinClearance
    if (!patient()->data(Core::IPatient::CreatinClearance).toString().isEmpty()
            && !patient()->data(Core::IPatient::CreatinClearanceUnit).toString().isEmpty()) {
        bio << tkTr(Trans::Constants::CREATININ_CLEARANCE) + ": " + patient()->data(Core::IPatient::CreatinClearance).toString() + " " + patient()->data(Core::IPatient::CreatinClearanceUnit).toString();
    }

    return QString("<div style=\"prescriptionPatientBiometrics\">%1</div>").arg(bio.join("<br />"));
}

/**
 * Add extra XML data to the html ouput . The html output of a prescription contains all the
 * all the prescription in an XML way (so that it can be recomputed later), you can add
 * some more xml data setting the \e xmlExtraData.
 */
void PrescriptionPrinterJob::addXmlExtraData(const QString &xmlExtraData)
{
    d->_xmlExtraData = xmlExtraData;
}

/** Returns the XML extra data to add to the XML'd prescription */
QString PrescriptionPrinterJob::xmlExtraData() const
{
    return d->_xmlExtraData;
}

/** Defines the DrugsDB::Internal::DrugsModel to use. Avoid zero pointer. */
void PrescriptionPrinterJob::setDrugsModel(DrugsModel *drugsModel)
{
    Q_ASSERT(drugsModel);
    d->_drugsModel = drugsModel;
}

/** Returns the DrugsDB::Internal::DrugsModel to use */
DrugsModel *PrescriptionPrinterJob::drugsModel() const
{
    return d->_drugsModel;
}


/*! Constructor of the DrugsDB::Internal::PrescriptionPrinter class */
PrescriptionPrinter::PrescriptionPrinter(QObject *parent) :
    QObject(parent),
    d(new PrescriptionPrinterPrivate(this))
{
}

/*! Destructor of the DrugsDB::Internal::PrescriptionPrinter class */
PrescriptionPrinter::~PrescriptionPrinter()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PrescriptionPrinter::initialize()
{
    return true;
}

/** Return the prescription transformed to Html using the default formatting. */
QString PrescriptionPrinter::prescriptionToHtml(DrugsModel *model)
{
    PrescriptionPrinterJob job;
    job.readSettings();
    job.setDrugsModel(model);
    return d->prescriptionToHtml(job);
}

/** Return the prescription transformed to Html using a defined formatting throught \e job. */
QString PrescriptionPrinter::prescriptionToHtml(const PrescriptionPrinterJob &job)
{
    return d->prescriptionToHtml(job);
}

/**
 * \brief Transform prescription to a readable HTML output.
 * All items of the prescription are translated to HTML using the token manager and
 * the HTML drug posologic sentence from the user settings.\n
 * The XML encoded prescription is added inside the HTML code.\n
*/
bool PrescriptionPrinter::print(DrugsModel *model)
{
    PrescriptionPrinterJob job;
    job.readSettings();
    job.setDrugsModel(model);
    return print(job);
}

/**
 * \brief Transform prescription to a readable HTML output.
 * All items of the prescription are translated to HTML using the token manager and
 * the HTML drug posologic sentence from the user settings.\n
 * The XML encoded prescription is added inside the HTML code.\n
*/
bool PrescriptionPrinter::print(const PrescriptionPrinterJob &job)
{
    // Create a printing job
    Core::IDocumentPrinter *p = printer();
    p->clearTokens();
    QHash<QString, QVariant> tokens;
    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, tr("Drugs Prescription"));
    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);
    return p->print(d->prescriptionToHtml(job),
                    Core::IDocumentPrinter::Papers_Prescription_User,
                    job.printDuplicates());
}

/**
 * Creates a print preview dialog with the prescription
 * from the DrugsDB::DrugsModel \e model
*/
void PrescriptionPrinter::printPreview(DrugsDB::DrugsModel *model)
{
    PrescriptionPrinterJob job;
    job.readSettings();
    job.setDrugsModel(model);

    Core::IDocumentPrinter *p = printer();
    p->clearTokens();
    QHash<QString, QVariant> tokens;
    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, tr("Drugs Prescription"));

    // TODO: add EPISODE_DATE token for FMF
    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);

    // TODO: add more options for the user: select papers, print duplicatas...
    QString html = d->prescriptionToHtml(job);
    QString css = Utils::htmlTakeAllCssContent(html);
    html = Utils::htmlBodyContent(html);
    html = Utils::htmlRemoveLinkTags(html);
    html.prepend(css);
    p->printPreview(html,
             Core::IDocumentPrinter::Papers_Prescription_User,
             settings()->value(Constants::S_PRINTDUPLICATAS).toBool());
}
