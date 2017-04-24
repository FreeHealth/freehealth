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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Tools::Internal::PdfTkWrapper
 */

#include "pdftkwrapper.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/imainwindow.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QDesktopServices>
#include <QDir>
#include <QUuid>
#include <QProcess>
#include <QUrl>
#include <QPointer>
#include <QProgressDialog>

#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace {
// For version 1.45
const char * const MAC_MD5     = "9008ff30f6b0319a066a62796de5479c";
const char * const MAC_SHA1    = "04f5b73d0fef8aac91c95b3fa23c1b15ee627040";
const char * const MAC_SHA256  = "b8eea6f0d13de3950f1e979b3b559415775c46edf5da34ef48a376cc261d2ff2";

const char * const XP_MD5      = "8fb7e026f51b8924cbecdf5fa9d1cce3";
const char * const XP_SHA1     = "37d45dfb7ecc00018b141512a88e2c6085cc3072";
const char * const XP_SHA256   = "8826a1ab95375e6960b6b8692f71254dadb75ca580c4769a608337f3cc43a1d1";

} // anonymous namespace

namespace Tools {
namespace Internal {

PdfTkWrapper *PdfTkWrapper::_instance = 0;

class PdfTkWrapperPrivate
{
public:
    PdfTkWrapperPrivate(PdfTkWrapper */*parent*/) :
        _initialized(false),
        _process(0)  // , q(parent)
    {
    }

    ~PdfTkWrapperPrivate()
    {
    }

    QString pdfTkPath()
    {
        // Find the binary according to the current OS
        if (Utils::isRunningOnMac()) {
            // Mac -> check datapacks
            QString path;
            path = settings()->dataPackApplicationInstalledPath() + "/pdfcompletion/mac/bin/pdftk";
            if (QFileInfo(path).exists())
                return path;
            path = settings()->dataPackInstallPath() + "/pdfcompletion/mac/bin/pdftk";
            return path;
        } else if (Utils::isRunningOnWin()) {
            // Win -> check datapacks
            QString path;
            path = settings()->dataPackApplicationInstalledPath() + "/pdfcompletion/win/bin/pdftk.exe";
            if (QFileInfo(path).exists())
                return path;
            path = settings()->dataPackInstallPath() + "/pdfcompletion/win/bin/pdftk.exe";
            return path;
        } else {
            // Other (mainly unices like linux)
            return "/usr/bin/pdftk";
        }
        return QString::null;
    }

    void clearCache(QProcess *process)
    {
        _processOutputFile.remove(process);
        _processTmpFile.remove(process);
        QProgressDialog *dialog = _progressDialog.value(process, 0);
        if (dialog) {
            dialog->close();
            delete dialog;
            dialog = 0;
        }
        _progressDialog.remove(process);
    }

public:
    // Put your data here
    bool _initialized;
    QString _buildedFdf;
    QHash<QString, QString> _fieldValue;
    QPointer<QProcess> _process;
    QHash<QProcess *, QString> _processOutputFile, _processTmpFile;
    QHash<QProcess *, QProgressDialog *> _progressDialog;

private:
    //    PdfTkWrapper *q;
};
} // namespace Internal
} // namespace Tools


/*! Constructor of the Tools::Internal::PdfTkWrapper class */
PdfTkWrapper::PdfTkWrapper(QObject *parent) :
    QObject(parent),
    d(new PdfTkWrapperPrivate(this))
{
    setObjectName("PdfTkWrapper");
    _instance = this;
}

/** Return the unique instance of the object */
PdfTkWrapper *PdfTkWrapper::instance()
{
    Q_ASSERT(_instance);
    return _instance;
}

/*! Destructor of the Tools::Internal::PdfTkWrapper class */
PdfTkWrapper::~PdfTkWrapper()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PdfTkWrapper::initialize()
{
    if (d->_initialized)
        return true;

    // Check if a pdftk binary exists
    if (!QFileInfo(d->pdfTkPath()).exists())
        return false;

    // Security assessment: check bin checksums
    QString pdfTk = d->pdfTkPath();
    if (Utils::isRunningOnMac()) {
        if (Utils::fileMd5(pdfTk) != ::MAC_MD5
                || Utils::fileSha1(pdfTk) != ::MAC_SHA1
                || Utils::fileSha256(pdfTk) != ::MAC_SHA256
                ) {
            LOG_ERROR("Wrong pdftk binary");
            return false;
        }
    } else if (Utils::isRunningOnWin()) {
        // FIXME: may be XP / Win7/ Win8 checksum are different?
        if (Utils::fileMd5(pdfTk) != ::XP_MD5
                || Utils::fileSha1(pdfTk) != ::XP_SHA1
                || Utils::fileSha256(pdfTk) != ::XP_SHA256
                ) {
            LOG_ERROR("Wrong pdftk binary");
            return false;
        }
    } else if (Utils::isRunningOnLinux()) {
        // TODO: check linux binaries
    }

    // Check file permission
    QFile file(pdfTk);
    if (Utils::isRunningOnMac()) {
        // Mac & Win are using the datapack binary, but QuaZip does not correctly handle file permissions
        // So test permissions are reset if needed
        if (!file.permissions().testFlag(QFile::ExeOwner)
                || !file.permissions().testFlag(QFile::ExeGroup)
                || !file.permissions().testFlag(QFile::ExeUser)
                || !file.permissions().testFlag(QFile::ExeOther)) {
            file.setPermissions(QFile::ReadOwner | QFile::ExeOwner | QFile::ReadUser | QFile::ExeUser | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther |  QFile::ExeOther);
        }
    }
    d->_initialized = true;
    return true;
}

/** Check that everything is Ok with this binary wrapper */
bool PdfTkWrapper::isAvailable() const
{
    return d->_initialized;
}

/**
 * Starts a FDF encoding
 * \sa addFdfValue(), endFdfEncoding(), getFdfContent()
*/
void PdfTkWrapper::beginFdfEncoding()
{
    d->_buildedFdf.clear();
    d->_fieldValue.clear();
}

/**
 * Adds a value to the started FDF encoding
 * \sa beginFdfEncoding(), endFdfEncoding(), getFdfContent()
*/
void PdfTkWrapper::addFdfValue(const QString &fieldName, const QString &value, bool toUpper)
{
    QString val;
    if (toUpper)
        val = value.toUpper();
    else
        val = value;
    val = val.simplified();
    val = val.replace("<BR>","", Qt::CaseInsensitive);
    val = val.replace("<BR />","", Qt::CaseInsensitive);
    val = val.replace("<BR/>","", Qt::CaseInsensitive);
    val = val.replace("(","_");
    val = val.replace(")","_");
    val = val.replace("\t","    ", Qt::CaseInsensitive);
    d->_fieldValue.insert(fieldName, val);
}

//void PdfTkWrapper::addFdfValueFromFormItem(const QString &fieldName, const QString &itemUuid, bool toUpper)
//{
//    // get item from scriptengine
//}

/**
 * Stops the started FDF encoding.
 * Appends the pdf file name \e filename to the FDF content.
 * \sa beginFdfEncoding(), addFdfValue(), getFdfContent()
*/
void PdfTkWrapper::endFdfEncoding(const QString &filename)
{
    // Header
    d->_buildedFdf = "%FDF-1.2\n%âãÏÓ\n1 0 obj\n<<\n/FDF\n<<\n/Fields [\n";
    // Fields
    QHashIterator<QString, QString> i(d->_fieldValue);
    while (i.hasNext()) {
        i.next();
        d->_buildedFdf += QString("<< /T (%1) /V (%2) >>\n")
                .arg(i.key()).arg(i.value());
    }
    // Footer
    d->_buildedFdf += "]/F (" + filename + ")\n";
    d->_buildedFdf += "  >>\n>>\nendobj\ntrailer\n<< /Root 1 0 R >>\n%%EOF";
}

/**
 * Returns the encoded FDF content.
 * \sa beginFdfEncoding(), addFdfValue(), endFdfEncoding()
 */
QString PdfTkWrapper::getFdfContent()
{
    return d->_buildedFdf;
}

/**
 * Fill a pdf file \e absPdfFile with a FDF content \e fdfContent with an
 * output file \e absFileNameOut using the string encoding \e isoEncoding for
 * the management of the FDF file.
 */
bool PdfTkWrapper::fillPdfWithFdf(const QString &absPdfFile, const QString &fdfContent, const QString &absFileNameOut, const QString &isoEncoding)
{
    if (!d->_initialized)
        return false;

    // Check PDF in file
    if (absPdfFile.isEmpty() || !QFileInfo(absPdfFile).exists())
        return false;

    // Check PDF out file
    if (absFileNameOut.isEmpty())
        return false;
    if (QFileInfo(absFileNameOut).exists())
        if (!QFile(absFileNameOut).remove())
            return false;

    // Save FDF content into the app tmp path
    QString tmpFdf = settings()->path(Core::ISettings::ApplicationTempPath) + QDir::separator();
    tmpFdf += "fdf_" + QUuid::createUuid().toString().remove("{").remove("}").remove("-") + ".fdf";
    if (!Utils::saveStringToEncodedFile(fdfContent, tmpFdf, isoEncoding, Utils::Overwrite, Utils::DontWarnUser))
        return false;

    // Check private process
    if (d->_process) {
        // ensure process is finished
        d->_process->close();
        delete d->_process;
        d->_process = 0;
    }

    // Starts a QProcess over pdftk
    QStringList args;
    args << absPdfFile
         << "fill_form"
         << tmpFdf
         << "output"
         << absFileNameOut;
    d->_process = new QProcess(this);
    d->_processOutputFile.insert(d->_process, absFileNameOut);
    d->_processTmpFile.insert(d->_process, tmpFdf);

    // Create the progress dialog
    QProgressDialog *dialog = new QProgressDialog(Core::ICore::instance()->mainWindow());
    dialog->setLabelText(tr("Starting PDF completion. Please wait..."));
    dialog->setRange(0, 2);
    dialog->show();
    d->_progressDialog.insert(d->_process, dialog);

    connect(d->_process, SIGNAL(finished(int)), this, SLOT(onProcessFinished(int)));
    connect(d->_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onProcessError(QProcess::ProcessError)));
    d->_process->start(d->pdfTkPath(), args);
    LOG("PDF completion process started.");
    return true;
}

void PdfTkWrapper::onProcessError(QProcess::ProcessError)
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    Q_ASSERT(process);
    if (!process)
        return;

    // Inform user
    Utils::warningMessageBox(tr("PDF Completion error"),
                             tr("The PDF completion process raised the following error:<br />"
                                "<b>%1</b>\n"
                                "%2")
                             .arg(process->errorString())
                             .arg(tkTr(Trans::Constants::CONSTANTS_TRANSLATOR_NAME)),
                             "",
                             tr("PDF Completion error"));

    // Log error
    LOG_ERROR("Unable to complete the PDF completion process. Error: " + process->errorString());

    // Removes tmp file
    if (!QFile(d->_processTmpFile.value(process)).remove())
        LOG_ERROR("Unable to remove tmp file: " + d->_processTmpFile.value(process));

    // Delete the process
    d->clearCache(process);
    delete process;
    process = 0;
}

void PdfTkWrapper::onProcessFinished(int exitCode)
{
    LOG("PDF Completion finished with exitcode: " + QString::number(exitCode));
    QProcess *process = qobject_cast<QProcess*>(sender());
    Q_ASSERT(process);
    if (!process)
        return;
    if (exitCode) {
        Utils::warningMessageBox(tr("pdftk error"),
                                 tr("The pdftk process ends with the exit code: %1\n"
                                    "%2")
                                 .arg(exitCode)
                                 .arg(QString(process->readAllStandardError())));
    }

    // Start the desktop PDF viewer
    if (process) {
        QProgressDialog *dialog = d->_progressDialog.value(process, 0);
        if (dialog) {
            dialog->setLabelText(tr("Opening completed PDF file"));
            dialog->setValue(1);
        }

        if (!QDesktopServices::openUrl(QUrl("file://" + d->_processOutputFile.value(process))))
            LOG_ERROR("Unable to launch the PDF viewer for the following file: " + d->_processOutputFile.value(d->_process));

        // Removes tmp file
        if (!QFile(d->_processTmpFile.value(process)).remove())
            LOG_ERROR("Unable to remove tmp file: " + d->_processTmpFile.value(process));

        // Clean cache
        d->clearCache(process);
    }

    // Destroy the process
    process->kill();
    process->deleteLater();
}


/*

//// Javascript
function franceDeclarationMedTraitant()
{
    // get the pdftkwrapper
    var pdf = freemedforms.pdf;
    if (!pdf.isAvailable) {
        print("pdftk not available");
        return;
    }

    // process data
    var patient = freemedforms.patient;
    if (!patient.isActive) {
        print("not active patient");
        return;
    }

    pdf.beginFdfEncoding();

    freemedforms.forms.namespaceInUse = "Subs::Tools::Identity";
    var isNssOwner = freemedforms.forms.item("FrGroup::IsNSSOwner");
    if (!isNssOwner.isValid) {
        print("isNSSOwner not available");
        return;
    }
    var nss = freemedforms.forms.item("FrGroup::NSS");
    if (!nss.isValid) {
        print("NSS not available");
        return;
    }

    pdf.addFdfValue("nom", patient.usualName + " " + patient.otherNames);
    pdf.addFdfValue("prénom", patient.firstName);
    pdf.addFdfValue("date naissance", freemedforms.tools.dateToString(patient.dateOfBirth, "ddMMyyyy"));
    // TODO: split street into 2 max lines if required

    var street = patient.street;
    if (street.search("\n")) {
        var splitter = street.split("\n");
        pdf.addFdfValue("adresse1", splitter.shift());
        pdf.addFdfValue("adresse2", splitter.join(" ; "));
    } else {
        pdf.addFdfValue("adresse1", patient.street);
    }
    pdf.addFdfValue("adresse3", patient.city + " " + patient.zipcode + " " + patient.state);

    if (!isNssOwner.checked) {
        var owner = freemedforms.forms.item("FrGroup::OwnerName");
        if (!owner.isValid) {
            print("NSS owner not available");
            return;
        }
        pdf.addFdfValue("nom ben", owner.currentText);
//        pdf.addFdfValue("prénom ben", nss.currentText);
    }

    if (nss.currentText.length == 15) {
        pdf.addFdfValue("n° immatriculation", nss.currentText.substring(0, 13));
        pdf.addFdfValue("clé immatric", nss.currentText.substring(13,15));
    }

    var user = freemedforms.user;
    if (user.isValid) {
        print("No valid user");
        //return;
    }

    pdf.addFdfValue("num ident med1", user.identifiers.join("; "));
    pdf.addFdfValue("nom medecin", user.usualName + " " + user.otherNames);
    pdf.addFdfValue("prénom médecin", user.firstName);
    pdf.addFdfValue("identif  medecin", user.identifiers.join("; "));

    pdf.addFdfValue("date déclaration", freemedforms.tools.dateToString(new Date(), "ddMMyyyy"));

    var filename = "~/git/freehealth/global_resources/declarationMT.pdf"
    pdf.endFdfEncoding(filename);

    print(pdf.getFdfContent());
    pdf.fillPdfWithFdf(filename, pdf.getFdfContent(), "/Volumes/HDD/test.pdf", "ISO-8859-1");
}

franceDeclarationMedTraitant();


function franceDeclarationAld()
{
    // get the pdftkwrapper
    var pdf = freemedforms.pdf;
    if (!pdf.isAvailable) {
        freemedforms.log.error("ALD form", "Error in french specific ALD form");
        return;
    }

    // process data
    var patient = freemedforms.patient;
    if (!patient.isActive) {
        freemedforms.log.error("ALD form", "patient is not active");
        return;
    }

    var checked = "Oui";
    var unchecked = "non";

    pdf.beginFdfEncoding();

    // Patient data
    pdf.addFdfValue("nom prénom", patient.usualName + " " + patient.otherNames + " " + patient.firstName);
    pdf.addFdfValue("date naiss", freemedforms.tools.dateToString(patient.dateOfBirth, "ddMMyyyy"));
    // Address
    var street = patient.street;
    var adresse = "";
    if (street.search("\n")) {
        var splitter = street.split("\n");
        adresse = splitter.join(" ; ");
    } else {
        adresse = patient.street;
    }
    adresse += "\n" + patient.city + " " + patient.zipcode + " " + patient.state;
    pdf.addFdfValue("adresse", adresse);
    // Social Number
    var nss = patient.socialNumber1;
    if (patient.socialNumberOwnerName.length) {
        pdf.addFdfValue("nom prénom ass", patient.socialNumberOwnerName);
        if (nss.length == 15) {
            pdf.addFdfValue("immat ass", nss.currentText.substring(0, 13));
            pdf.addFdfValue("clé ass", nss.currentText.substring(13,15));
        }
    } else {
        if (nss.length == 15) {
            pdf.addFdfValue("immat", nss.substring(0, 13));
            pdf.addFdfValue("clé", nss.substring(13,15));
        }
    }

    freemedforms.forms.namespaceInUse = "Subs::Tools::FrenchSpecific::ALD";

    // diagnostic(s) de l'(des) affection(s) de longue durée motivant la demande et sa (leurs) date(s) présumée(s) de début
    item = freemedforms.forms.item("Diagnostic1::Label");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "Diagnostic1::Label not available");
        return;
    }
    pdf.addFdfValue("diag1", item.currentText);

    item = freemedforms.forms.item("Diagnostic2::Label");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "Diagnostic2::Label not available");
        return;
    }
    pdf.addFdfValue("diag2", item.currentText);

    item = freemedforms.forms.item("Diagnostic3::Label");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "Diagnostic3::Label not available");
        return;
    }
    pdf.addFdfValue("diag3", item.currentText);

    item = freemedforms.forms.item("Diagnostic1::Date");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "Diagnostic1::Date not available");
        return;
    }
    pdf.addFdfValue("date diag1", freemedforms.tools.dateToString(new Date(item.currentValue), "ddMMyyyy"));

    item = freemedforms.forms.item("Diagnostic2::Date");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "Diagnostic2::Date not available");
        return;
    }
    pdf.addFdfValue("date diag2", freemedforms.tools.dateToString(new Date(item.currentValue), "ddMMyyyy"));

    item = freemedforms.forms.item("Diagnostic3::Date");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "Diagnostic3::Date not available");
        return;
    }
    pdf.addFdfValue("date diag3", freemedforms.tools.dateToString(new Date(item.currentValue), "ddMMyyyy"));

    item = freemedforms.forms.item("Arguments");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "Arguments not available");
        return;
    }
    pdf.addFdfValue("arguments", item.currentText.replace(/\n/g, "\\n"), false);  // Max 9 Lines

    item = freemedforms.forms.item("Proposition");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "Proposition not available");
        return;
    }
    pdf.addFdfValue("ALD1", unchecked, false); // "ALD non exonérante"
    pdf.addFdfValue("ALD2", unchecked, false); // "ALD 30"
    pdf.addFdfValue("ALD3", unchecked, false); // "ALD hors-liste"
    pdf.addFdfValue("ALD4", unchecked, false); // "Poly-pathologie"
    pdf.addFdfValue("ALD5", unchecked, false); // "ALD autres"
    if (item.currentUuid == "nonExo") {
        pdf.addFdfValue("ALD1", checked, false); // "ALD non exonérante"
    } else if (item.currentUuid == "ald30") {
        pdf.addFdfValue("ALD2", checked, false); // "ALD 30"
    } else if (item.currentUuid == "horsListe") {
        pdf.addFdfValue("ALD3", checked, false); // "ALD hors-liste"
    } else if (item.currentUuid == "polyPatho") {
        pdf.addFdfValue("ALD4", checked, false); // "Poly-pathologie"
    } else if (item.currentUuid == "autres") {
        pdf.addFdfValue("ALD5", checked, false); // "ALD autres"
    }

    // suivi biologique prévu (type d'actes)
    item = freemedforms.forms.item("SuiviBio");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "SuiviBio not available");
        return;
    }
    var list = item.currentText;
    pdf.addFdfValue("suivi bio1", list[0]);
    pdf.addFdfValue("suivi bio2", list[1]);
    pdf.addFdfValue("suivi bio3", list[2]);

    // recours à des professionnels de santé para-médicaux
    item = freemedforms.forms.item("RecoursSpe");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "RecoursSpe not available");
        return;
    }
    list = item.currentText;
    pdf.addFdfValue("rec pro1", list[0]);
    pdf.addFdfValue("rec pro2", list[1]);
    pdf.addFdfValue("rec pro3", list[2]);

    // recours à des spécialistes (préciser la spécialité et le type d'acte spécialisé prévu)
    item = freemedforms.forms.item("RecoursPro");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "RecoursPro not available");
        return;
    }
    list = item.currentText;
    pdf.addFdfValue("rec spé1", list[0]);
    pdf.addFdfValue("rec spé2", list[1]);
    pdf.addFdfValue("rec spé3", list[2]);

    // spécialités pharmaceutiques ou classes thérapeutiques ou dispositifs médicaux
    item = freemedforms.forms.item("Pharma");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "Pharma not available");
        return;
    }
    list = item.currentText;
    pdf.addFdfValue("spé phar1", list[0]);
    pdf.addFdfValue("spé phar2", list[1]);
    pdf.addFdfValue("spé phar3", list[2]);
    pdf.addFdfValue("spé phar4", list[3]);
    pdf.addFdfValue("spé phar5", list[4]);
    pdf.addFdfValue("spé phar6", list[5]);
    pdf.addFdfValue("spé phar7", list[6]);
    pdf.addFdfValue("spé phar8", list[7]);
    pdf.addFdfValue("spé phar9", list[8]);
    pdf.addFdfValue("spé phar10", list[9]);
    pdf.addFdfValue("spé phar11", list[10]);
    pdf.addFdfValue("spé phar12", list[11]);

    // Information(s) concernant la maladie (pour le patient)
    item = freemedforms.forms.item("InfoPatients");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "InfoPatients not available");
        return;
    }
    pdf.addFdfValue("infos mal", item.currentText.replace(/\n/g, "\\n"), false); // Max 8 lines
    pdf.addFdfValue("infos mal2", ""); // Max 3 lines

    // durée prévisible de l'arrêt de travail, s'il y a lieu :
    item = freemedforms.forms.item("DureeAT::Value");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "DureeAT::Value not available");
        return;
    }
    if (item.currentValue > 0) {
        print(item.currentValue);
        var period = freemedforms.forms.item("DureeAT::Period");
        if (!period.isValid) {
            freemedforms.log.error("ALD form", "DureeAT::Period not available");
            return;
        }
        pdf.addFdfValue("durée arrêt", item.currentText + " " + period.currentText);
    }

    // durée prévisible des soins :
    item = freemedforms.forms.item("DureeSoins::Value");
    if (!item.isValid) {
        freemedforms.log.error("ALD form", "DureeSoins::Value not available");
        return;
    }
    if (item.currentValue > 0) {
        print(item.currentValue);
        var period = freemedforms.forms.item("DureeSoins::Period");
        if (!period.isValid) {
            freemedforms.log.error("ALD form", "DureeSoins::Period not available");
            return;
        }
        pdf.addFdfValue("durée soins", item.currentText + " " + period.currentText);
    }


    // "Case à cocher"
    pdf.addFdfValue("Case à cocher1", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher2", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher3", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher4", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher5", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher6", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher7", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher8", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher9", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher10", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher11", unchecked, false); // spé phar 1 check
    pdf.addFdfValue("Case à cocher12", unchecked, false); // spé phar 1 check

    pdf.addFdfValue("Case à cocher13", unchecked, false);   // suivi bio1 check
    pdf.addFdfValue("Case à cocher14", unchecked, false);   // suivi bio2 check
    pdf.addFdfValue("Case à cocher15", unchecked, false);   // suivi bio3 check

    pdf.addFdfValue("Case à cocher16", unchecked, false);   // rec spé1 check
    pdf.addFdfValue("Case à cocher17", unchecked, false);   // rec spé2 check
    pdf.addFdfValue("Case à cocher18", unchecked, false);   // rec spé3 check

    pdf.addFdfValue("Case à cocher19", unchecked, false);   // rec pro1 check
    pdf.addFdfValue("Case à cocher20", unchecked, false);   // rec pro2 check
    pdf.addFdfValue("Case à cocher21", unchecked, false);   // rec pro3 check

    // demande de rémunération
    pdf.addFdfValue("date examen", freemedforms.tools.dateToString(new Date(), "ddMMyyyy")); // date d el'examen

    // Identification du médecin
    var user = freemedforms.user;
    if (user.isValid) {
        freemedforms.log.error("ALD form", "No valid user");
        //return;
    }

    pdf.addFdfValue("IDENTIF MEDECIN", user.fullName.toUpperCase() + "\\n" + user.fullAddress.toUpperCase() + "\\n" + user.identifiers.join("; ").toUpperCase(), false);
    // cachet de l'établissement ou du centre de référence
    // pdf.addFdfValue("IDENTIF ETABLT", "IDENTIF ETABLT");

    // reclassement professionnel envisagé
    pdf.addFdfValue("Recl pro", unchecked, false); // Oui

    // identification du médecin traitant établissant le protocole
    pdf.addFdfValue("ident méd", user.fullName);
    pdf.addFdfValue("ident méd2", user.fullAddress);
    pdf.addFdfValue("num ident méd", user.identifiers.join("; "));

    // identification de l'établissement
    // pdf.addFdfValue("nom étab", "nom étab");
    // pdf.addFdfValue("adress étab", "adress étab");
    // pdf.addFdfValue("nom chef serv", "nom chef serv");
    // pdf.addFdfValue("num finess", "num finess");
    // pdf.addFdfValue("date entrée", "date entrée"); // si le malade est hospitalisé, date d'entrée
    // pdf.addFdfValue("act", checked, false); // activité privée

    ///////////////////////////////////////////////////////
    // Unmatched fields
    // pdf.addFdfValue("att", checked, false);
    // pdf.addFdfValue("exam", checked, false);
    // pdf.addFdfValue("règlem", checked, false);
    // pdf.addFdfValue("REF", checked, false);
    ///////////////////////////////////////////////////////

    // décision du médecin conseil
    // pdf.addFdfValue("acc1", "acc1");
    // pdf.addFdfValue("acc2", "acc2");
    // pdf.addFdfValue("acc3", "acc3");
    // pdf.addFdfValue("mal1", "mal1");
    // pdf.addFdfValue("mal2", "mal2");
    // pdf.addFdfValue("mal3", "mal3");
    // pdf.addFdfValue("date acc1", "date acc1");
    // pdf.addFdfValue("date acc2", "date acc2");
    // pdf.addFdfValue("date acc3", "date acc3");
    // pdf.addFdfValue("date val1", "date val1");
    // pdf.addFdfValue("date val2", "date val2");
    // pdf.addFdfValue("date val3", "date val3");
    // pdf.addFdfValue("motif refus", "motif refus");
    // pdf.addFdfValue("motif refus2", "motif refus2");
    // pdf.addFdfValue("date pro", "date pro"); // décision du médecin conseil: date
    // pdf.addFdfValue("date val pro", "date val pro"); // décision du médecin conseil: protocole valable jusqu'au

    pdf.addFdfValue("date déclaration", freemedforms.tools.dateToString(new Date(), "ddMMyyyy"));

    var filename = freemedforms.forms.extractFormFile("__subForms__/french/protocole_ald", "./pdf/fr/ald.pdf");
    pdf.endFdfEncoding(filename);

    var output = freemedforms.tools.userDocumentPath + "protocole_ALD_" + patient.usualName + " " + patient.otherNames + " " + patient.firstName + ".pdf";
    output = output.replace("  ", " ");
    output = output.replace(" ", "_");

    print(output);

    pdf.fillPdfWithFdf(filename, pdf.getFdfContent(), output, "ISO-8859-1");


}

franceDeclarationAld();


*/
