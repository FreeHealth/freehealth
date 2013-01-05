/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 * \class Tools::Internal::PdfTkWrapper
 */

#include "pdftkwrapper.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QDesktopServices>
#include <QDir>
#include <QUuid>
#include <QProcess>
#include <QUrl>
#include <QPointer>

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
class PdfTkWrapperPrivate
{
public:
    PdfTkWrapperPrivate(PdfTkWrapper *parent) :
        _initialized(false),
        _process(0),
        q(parent)
    {
    }
    
    ~PdfTkWrapperPrivate()
    {
    }

    QString pdkTkPath()
    {
        return "/opt/pdflabs/pdftk/bin/pdftk";
    }
    
public:
    // Put your data here
    bool _initialized;
    QString _buildedFdf;
    QPointer<QProcess> _process;
    QHash<int, QString> _processOutputFile, _processTmpFile;

private:
    PdfTkWrapper *q;
};
} // namespace Internal
} // end namespace Tools


/*! Constructor of the Tools::Internal::PdfTkWrapper class */
PdfTkWrapper::PdfTkWrapper(QObject *parent) :
    QObject(parent),
    d(new PdfTkWrapperPrivate(this))
{
    setObjectName("PdfTkWrapper");
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
    if (!QFileInfo(d->pdkTkPath()).exists())
        return false;

    // Security assessment: check bin checksums
    if (Utils::isRunningOnMac()) {
        if (Utils::md5(d->pdkTkPath()) != ::MAC_MD5
                || Utils::sha1(d->pdkTkPath()) != ::MAC_SHA1
                || Utils::sha256(d->pdkTkPath()) != ::MAC_SHA256) {
            LOG_ERROR("Wrong pdftk binary");
            return false;
        }
    } else if (Utils::isRunningOnWin()) {
        // TODO: may be XP / Win7/ Win8 checksum are different?
        if (Utils::md5(d->pdkTkPath()) != ::XP_MD5
                || Utils::sha1(d->pdkTkPath()) != ::XP_SHA1
                || Utils::sha256(d->pdkTkPath()) != ::XP_SHA256) {
            LOG_ERROR("Wrong pdftk binary");
            return false;
        }
    } else {
        // TODO: check linux/freebsd binaries
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
    d->_buildedFdf = "%FDF-1.2\n%âãÏÓ\n1 0 obj\n<<\n/FDF\n<<\n/Fields [\n";
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
    val = val.replace("<BR>","");
    val = val.replace("<BR />","");
    val = val.replace("(","_");
    val = val.replace(")","_");
    val = val.replace("\t","    ");
    d->_buildedFdf += QString("<< /T (%1) /V (%2) >>\n").arg(fieldName).arg(val);
}

/**
 * Stops the started FDF encoding.
 * Appends the pdf file name \e filename to the FDF content.
 * \sa beginFdfEncoding(), addFdfValue(), getFdfContent()
*/
void PdfTkWrapper::endFdfEncoding(const QString &filename)
{
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
    d->_process->start(d->pdkTkPath(), args);
    d->_processOutputFile.insert(d->_process->pid(), absFileNameOut);
    d->_processTmpFile.insert(d->_process->pid(), tmpFdf);
    connect(d->_process, SIGNAL(finished(int)), this, SLOT(onProcessFinished(int)));

    return true;
}

void PdfTkWrapper::onProcessFinished(int exitCode)
{
    if (exitCode) {
        Utils::warningMessageBox(tr("pdftk error"),
                                 tr("The pdftk process ends with the exit code: %1\n"
                                    "%2")
                                 .arg(exitCode)
                                 .arg(QString(d->_process->readAllStandardError())));
    }

    // Start the desktop PDF viewer
    if (d->_process) {
        qWarning() << "file://" + d->_processOutputFile.value(d->_process->pid());
        QDesktopServices::openUrl(QUrl("file://"+d->_processOutputFile.value(d->_process->pid())));

        // Removes tmp file
        if (!QFile(d->_processTmpFile.value(d->_process->pid())).remove())
            LOG_ERROR("Unable to remove tmp file: " + d->_processTmpFile.value(d->_process->pid()));
        d->_processOutputFile.remove(d->_process->pid());
        d->_processTmpFile.remove(d->_process->pid());
    }

    // Destroy the process
    d->_process->kill();
    d->_process->deleteLater();
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

    pdf.addFdfValue("nom", patient.birthName + " " + patient.secondName);
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

    pdf.addFdfValue("num ident med1", user.identifiants.join("; "));
    pdf.addFdfValue("nom medecin", user.birthName + " " + user.secondName);
    pdf.addFdfValue("prénom médecin", user.firstName);
    pdf.addFdfValue("identif  medecin", user.identifiants.join("; "));

    pdf.addFdfValue("date déclaration", freemedforms.tools.dateToString(new Date(), "ddMMyyyy"));

    var filename = "/Volumes/RamDisk/eric/freemedforms/global_resources/declarationMT.pdf"
    pdf.endFdfEncoding(filename);

    print(pdf.getFdfContent());
    pdf.fillPdfWithFdf(filename, pdf.getFdfContent(), "/Volumes/HDD/test.pdf", "ISO-8859-1");
}

franceDeclarationMedTraitant();

*/
