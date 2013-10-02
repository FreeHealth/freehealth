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
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Utils::HprimHeader
 * \note Unit-test available (see: tests/auto/auto.pro)
 */

/**
 * \class Utils::HprimRawContent
 * \note Unit-test available (see: tests/auto/auto.pro)
 */

/**
 * \class Utils::Hprim2Content
 * \note Unit-test available (see: tests/auto/auto.pro)
 */

/**
 * \class Utils::HprimMessage
 * \note Unit-test available (see: tests/auto/auto.pro)
 */

#include "hprimparser.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_patient.h>

#include <QTextStream>
#include <QStringList>

#include <QDebug>

using namespace Trans::ConstantTranslations;

namespace {
const char *const EOF_TAG1 = "****FIN****";
const char *const EOF_TAG2 = "****FINFICHIER****";
const char *const LAB_TAG = "****LAB****";
const char *const ENCODED_LINE_SEPARATOR = "|";
}

using namespace Utils;
using namespace HPRIM;

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////          HprimHeader          ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/** Construct an empty invalid HPRIMv1 header */
HprimHeader::HprimHeader()
{}

HprimHeader::~HprimHeader()
{}

/**
 * Returns \e true if the HPRIM header is valid. A valid HPRIM allow user to
 * correctly identify the patient.
 */
bool HprimHeader::isValid() const
{
    // Valid message rules:
    // - At least patient name, firstname & dob are filled
    // - Contains a message
    return !_data.value(PatientName).isEmpty()
            && !_data.value(PatientFirstName).isEmpty()
            && !_data.value(PatientDateOfBirth).isEmpty();
}

/** Return \e true if the HPRIM header is empty. */
bool HprimHeader::isNull() const
{
    return _data.isEmpty() && _fullContent.isEmpty();
}

/** Set data of the HPRIM header */
bool HprimHeader::setData(const int ref, const QString &value)
{
    if (ref==PatientDateOfBirth) {
        // Check the format with a QDate generation (yyyy and yy)
        QDate date = QDate::fromString(value, "dd/MM/yyyy");
        if (!date.isValid()) {
            date = QDate::fromString(value, "dd/MM/yy");
            if (!date.isValid())
                return false;
        }
        // Or with a regexp matching "[0-3][0-9]/[0-1][0-09]/[1-2][0-9][0-9][0-9]"
    }
    _data.insert(ref, value);
    return true;
}

/** Returns the data of the HPRIM header */
QString HprimHeader::data(const int ref) const
{
    return _data.value(ref).trimmed();
}

/**
 * Set the full content of the HPRIM header. \n
 * Full content must contain 12 lines.
 */
void HprimHeader::setRawSource(const QString &fullContent)
{
    _fullContent = fullContent;
}

/**
 * Returns the full content of the HPRIM header.\n
 * If the header was created using setRawSource(), returns the exact same content.\n
 * If the header was populated using setData(), creates and returns a valid HPRIM v1 output
 * (ending with an empty line: SPACE+LF).
 */
QString HprimHeader::rawSource() const
{
    // If a full content was defined return it
    if (!_fullContent.isEmpty())
        return _fullContent;

    // Create the message
    _fullContent += _data.value(PatientId) + "\n";
    _fullContent += _data.value(PatientName) + "\n";
    _fullContent += _data.value(PatientFirstName) + "\n";
    if (!_data.value(PatientAddressFirstLine).isEmpty())
        _fullContent += _data.value(PatientAddressFirstLine).leftJustified(30, ' ') + "\n";
    else
        _fullContent += " \n";
    if (!_data.value(PatientAddressSecondLine).isEmpty())
        _fullContent += _data.value(PatientAddressSecondLine).leftJustified(30, ' ') + "\n";
    else
        _fullContent += " \n";
    _fullContent += _data.value(PatientAddressZipCode) + " " + _data.value(PatientAddressCity) + "\n";
    _fullContent += _data.value(PatientDateOfBirth) + "\n";
    _fullContent += _data.value(PatientSocialNumber) + "\n";
    _fullContent += _data.value(ExtraCode) + "\n";
    _fullContent += _data.value(DateOfExamination) + "\n";
    _fullContent += _data.value(SenderIdentity) + "\n";
    _fullContent += _data.value(ReceiverIdentity) + "\n";
    _fullContent += " \n";

    return _fullContent;
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////          HprimRawContent          //////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
HprimRawContent::HprimRawContent()
{}

HprimRawContent::~HprimRawContent()
{}

/** Return \e true if the content is defined and valid (correctly ending with the two EOF tags) */
bool HprimRawContent::isValid() const
{
    // Not empty
    if (_fullContent.isEmpty())
        return false;

    // Contains the two EOF tags in the correct order
    int beginFirst = _fullContent.indexOf(::EOF_TAG1);
    if (beginFirst == -1)
        return false;
    beginFirst += QString(::EOF_TAG1).size();

    int beginSecond = _fullContent.indexOf(::EOF_TAG2, beginFirst);
    if (beginSecond == -1)
        return false;

    // No extra-informations between the two EOF tags
    if (!_fullContent.mid(beginFirst, beginSecond-beginFirst).simplified().isEmpty())
        return false;
    beginSecond += QString(::EOF_TAG2).size();

    // No extra-informations after the last EOF tag
    return _fullContent.mid(beginSecond).simplified().isEmpty();
}

bool HprimRawContent::isNull() const
{
    return _fullContent.isEmpty();
}

void HprimRawContent::setRawSource(const QString &fullContent)
{
    _fullContent = fullContent;
}

QString HprimRawContent::rawSource() const
{
    return _fullContent;
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////          HprimMessage          ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * A valid HPRIM message always contains:
 * - valid HPRIM v1 header
 * - a raw message that can be processed by other objects (like Utils::HPRIM::Hprim2Content)
 * Construct an invalid message.
 */
HprimMessage::HprimMessage()
{}

HprimMessage::~HprimMessage()
{}

/** Returns \e true if the header and the rawContent of the message are both valid */
bool HprimMessage::isValid() const
{
    return _header.isValid() && _rawContent.isValid();
}

/** Returns \e true if the header only is null (no HPRIM data) */
bool HprimMessage::isNull() const
{
    return _header.isNull();
}

/** Set the header of the hprimmessage */
void HprimMessage::setHeader(const HprimHeader &header)
{
    _header = header;
}

/** Returns the header of the hprimmessage */
const HprimHeader &HprimMessage::header() const
{
    return _header;
}

/** Set the rawContent of the hprimmessage */
void HprimMessage::setRawContent(const HprimRawContent &rawContent)
{
    _rawContent = rawContent;
}

/** Returns the rawContent of the hprimmessage */
const HprimRawContent &HprimMessage::rawContent() const
{
    return _rawContent;
}

/** Transform the header & raw message to a valid plain text HPRIM message */
QString HprimMessage::toRawSource() const
{
    return QString("%1%2").arg(_header.rawSource()).arg(_rawContent.rawSource());
}

/** Returns a better readable HPRIM content using basic html (bold only for the header) */
QString HprimMessage::toBasicHtml() const
{
    int justify = 30;
    QString html;
    html += "<span style=\"font-weight:600; color: darkblue\"><pre>";
    html += QString("%1\n*%2*\n%1\n\n")
            .arg(QString().fill('*', 90))
            .arg(Utils::centerString(tkTr(Trans::Constants::MESSAGE_HEADER), ' ', 88));
    html += QString("%1: %2 %3\n")
            .arg(tkTr(Trans::Constants::PATIENT).rightJustified(justify, ' '))
            .arg(header().patientName())
            .arg(header().patientFirstName());
    html += QString("%1: %2\n")
            .arg(tkTr(Trans::Constants::DATE_OF_BIRTH).rightJustified(justify, ' '))
            .arg(QLocale().toString(header().patientDateOfBirth(), QLocale::LongFormat));
    html += QString("%1: %2\n")
            .arg(tkTr(Trans::Constants::SOCIAL_NUMBER).rightJustified(justify, ' '))
            .arg(header().data(Utils::HPRIM::HprimHeader::PatientSocialNumber));
    html += QString("%1: %2 %3 (%4 %5)\n")
            .arg(tkTr(Trans::Constants::FULLADDRESS).rightJustified(justify, ' '))
            .arg(header().data((Utils::HPRIM::HprimHeader::PatientAddressFirstLine)))
            .arg(header().data((Utils::HPRIM::HprimHeader::PatientAddressSecondLine)))
            .arg(header().data((Utils::HPRIM::HprimHeader::PatientAddressZipCode)))
            .arg(header().data((Utils::HPRIM::HprimHeader::PatientAddressCity)));
    html += "\n";
    html += QString("%1: %2\n")
            .arg(tkTr(Trans::Constants::FROM).rightJustified(justify, ' '))
            .arg(header().data(Utils::HPRIM::HprimHeader::SenderIdentity));
    html += QString("%1: %2\n")
            .arg(tkTr(Trans::Constants::TO).rightJustified(justify, ' '))
            .arg(header().data(Utils::HPRIM::HprimHeader::ReceiverIdentity));
    html += QString("%1: %2\n")
            .arg(tkTr(Trans::Constants::DATE).rightJustified(justify, ' '))
            .arg(QLocale().toString(header().dateOfExamination(), QLocale::LongFormat));
    html += "\n";
    html += "</span></pre>\n";
    html += "<pre>";
    html += QString("%1\n*%2*\n%1\n\n")
            .arg(QString().fill('*', 90))
            .arg(Utils::centerString(tkTr(Trans::Constants::MESSAGE_CONTENT), ' ', 88));
    html += rawContent().rawSource().replace("<", "&lt;");
    html += "</pre>";
    return html;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////          Hprim2Content          ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
Hprim2Content::Hprim2Content() :
    _parseError(false)
{}

Hprim2Content::Hprim2Content(const HprimRawContent &rawContent) :
    _parseError(false)
{
    // Parse content
    parseContent(rawContent);
}

Hprim2Content::~Hprim2Content()
{}

bool Hprim2Content::parseContent(const HprimRawContent &rawContent)
{
    _parseError = false;
    QString source = rawContent.rawSource();

    // No labtag -> exit without parsing error
    if (!source.contains(::LAB_TAG))
        return !_parseError;

    // Read one line at time
    QTextStream flux(&source, QIODevice::ReadOnly);
    if (!flux.seek(source.indexOf(::LAB_TAG))) {
        LOG_ERROR_FOR("Hprim2Content", "Unable to seek position");
        _parseError = true;
        return !_parseError;
    }

    int lineNb = -1;
    while (!flux.atEnd()) {
        QString line = flux.readLine();

        // Process only encoded results
        if (!line.startsWith("RES|"))
            continue;

        // Split the encoded line & check
        QStringList values = line.split(::ENCODED_LINE_SEPARATOR);

        // Store the encoded values
        ++lineNb;
        _lines.insert(lineNb, values);
    }

    // Get only encoded lines
    return !_parseError;
}

/**
 * Returns \e true if the object contains encoded values and if its rawContent was correctly parsed. \n
 * If the object only contains textual data, it will return \e false as the content is not HRPIM 2.
 */
bool Hprim2Content::isValid() const
{
    return !isNull() && !_parseError;
}

/** Returns \e true if the object contains encoded values */
bool Hprim2Content::isNull() const
{
    return _lines.isEmpty();
}

/** Return the number of decoded available lines */
int Hprim2Content::numberOfLines() const
{
    return _lines.count();
}

/** Returns the raw data of the decoded line \e lineNumber at \e rawDataIndex */
QString Hprim2Content::rawData(int lineNumber, int rawDataIndex) const
{
    // Outside the available lines?
    if (!IN_RANGE_STRICT_MAX(lineNumber, 0, _lines.count()))
        return QString::null;

    // Outside the available rawData of the line?
    const QStringList &rawData = _lines.value(lineNumber);
    if (!IN_RANGE_STRICT_MAX(rawDataIndex, 0, rawData.count()))
        return QString::null;

    // Return the rawdata value of the line
    return rawData.value(rawDataIndex);
}


namespace Utils {
namespace HPRIM {

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////          PARSER          //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/**
 * Parse an HPRIM v1 message and return a reference to the Utils::HPRIM::HprimMessage object. \n
 * You can parse the Utils::HPRIM::HprimRawContent for an HPRIM v2 data
 * using the Utils::HPRIM::Hprim2Content object.
 */
HprimMessage &parseHprimRawSource(const QString &fullMessage)
{
    HprimMessage *msg = new HprimMessage;
    HprimHeader hdr;
    HprimRawContent rawContent;

    // Parse header
    int line = 0;
    QString full = fullMessage;

    // CR only
    if (full.contains("\r") && !full.contains("\n"))
        full = full.replace("\r", "\n");

    // Correcting wrong encoding
    full = Utils::correctTextAccentEncoding(full);

    QTextStream flux(&full, QIODevice::ReadOnly);
    QStringList lines;
    while (!flux.atEnd() && line<12) {
        lines << flux.readLine();
        ++line;
    }
    if (line != 12) {
        msg->setHeader(hdr);
        msg->setRawContent(rawContent);
        return *msg;
    }
    hdr.setRawSource(full.left(flux.pos()));

    // Populate the HprimMessage
    int i = -1;
    hdr.setData(HprimHeader::PatientId, lines.at(++i));
    hdr.setData(HprimHeader::PatientName, lines.at(++i));
    hdr.setData(HprimHeader::PatientFirstName, lines.at(++i));
    hdr.setData(HprimHeader::PatientAddressFirstLine, lines.at(++i));
    hdr.setData(HprimHeader::PatientAddressSecondLine, lines.at(++i));
    // parse zip / city
    ++i;
    const QString &zipCity = lines.at(i);
    int begin = zipCity.indexOf(" ");
    hdr.setData(HprimHeader::PatientAddressZipCode, zipCity.left(begin));
    hdr.setData(HprimHeader::PatientAddressCity, zipCity.mid(begin));

    hdr.setData(HprimHeader::PatientDateOfBirth, lines.at(++i));
    hdr.setData(HprimHeader::PatientSocialNumber, lines.at(++i));
    hdr.setData(HprimHeader::ExtraCode, lines.at(++i));
    hdr.setData(HprimHeader::DateOfExamination, lines.at(++i));
    hdr.setData(HprimHeader::SenderIdentity, lines.at(++i));
    hdr.setData(HprimHeader::ReceiverIdentity, lines.at(++i));

    // Create RawContentMessage
    rawContent.setRawSource(full.mid(flux.pos()));

    // Create the message to return
    msg->setHeader(hdr);
    msg->setRawContent(rawContent);

    // Return the message
    return *msg;
}

/**
 * Transform a plain text message to a valid HPRIM rawContent and returns
 * a reference to the object. You do not have to manage EOF HPRIM tags.
 * All CR EOL are removed or replaced with LF.
 */
HprimRawContent &createMessageRawContent(const QString &plainTextMessage)
{
    HprimRawContent *content = new HprimRawContent;
    QString corrected = plainTextMessage;

    // Manage EOF tags
    if (corrected.contains(::EOF_TAG1, Qt::CaseInsensitive)
            || corrected.contains(::EOF_TAG2, Qt::CaseInsensitive)) {
        corrected = corrected.remove(::EOF_TAG1, Qt::CaseInsensitive);
        corrected = corrected.remove(::EOF_TAG2, Qt::CaseInsensitive);
    }

    // Manage EOL
    if (corrected.contains("\r")) {
        corrected = corrected.replace("\r", "<#@cr@#>");
        corrected = corrected.replace("<#@cr@#>\n", "\n");
        corrected = corrected.replace("<#@cr@#>", "\n");
    }

    // Add LF at the end of the text if needed
    if (!corrected.endsWith("\n"))
        corrected.append("\n");
    content->setRawSource(QString("%1%2\n%3\n")
                          .arg(corrected)
                          .arg(::EOF_TAG1)
                          .arg(::EOF_TAG2));
    return *content;
}

} // namespace HPRIM
} // namespace Utils


// HPRIM 2 (lab results)
// ****LAB****
// Séparateur=|
// RES-01 (type de segment) TEX RES
// RES-02 Libellé de l'analyse en texte clair
// RES-03 Code de l'analyse défini par le médecin ou selon classif hprim
// RES-04 Type de résultat A=Numérique N=Numérique C=Code
// RES-05 Résultat1 (en numérique sans espace ni virgule, séparateur décimal=., +ou- devant le chiffre)
// RES-06 Unit1 Les unités sont exprimées en système MKSA
// RES-07 Valeur normale inférieure 1
// RES-08 Valeur normale supérieure 1␣
// RES-09 Indicateur d'anormalité
//          L inférieur à la normale
//          H supérieur à la normale
//          LL inférieur à la valeur panique basse
//          HH supérieur à la valeur panique haute
//          N normal
// RES-10 Statut du résultat
//          F validé
//          R non validé
//          C modifié corrigé
// RES-11 Résultat 2. Ce champ et les 3 suivants permettent de communiquer le résultat dans une deuxième unité
// RES-12 Unité 2
// RES-13 Valeur normale inférieure 2
// RES-14 Valeur normale supérieur 2
