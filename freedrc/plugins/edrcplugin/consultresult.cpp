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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class eDRC::Internal::ConsultResult
 * \brief Data class for user constructed consultation results (RC in french).
 * This object describes the user constructed CR and owns
 * - primkey of the CR extracted from the SFMG database
 * - primkeys of the selected criterias
 * - diagnosis position,
 * - symptomatic state,
 * - position of the medical follow up
 * - chronic disease state ('ALD')
 * - two comments
 *     - one comment about the CR label itself
 *     - one comment about the selected criterias
 * - the ICD10 code associated with the CR
*/

#include "consultresult.h"
#include <edrcplugin/constants.h>
#include <edrcplugin/consultresultvalidator.h>
#include <edrcplugin/database/edrcbase.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_current.h>

#include <QRegExp>
#include <QDomDocument>
#include <QTextDocument>

#include <QDebug>

using namespace eDRC;
using namespace Internal;
using namespace Trans::ConstantTranslations;

/** Construct an empty invalid object */
ConsultResultCriteria::ConsultResultCriteria() :
    _id(-1), _weight(-1), _indentation(-1), _sortIndex(-1)
{}

ConsultResultCriteria::~ConsultResultCriteria()
{}

/** Define the indentation of the criteria */
void ConsultResultCriteria::setIndentation(int indentation)
{
    _indentation = indentation;
    _indent.clear();
    if (_indentation >= 2)
        _indent.fill(' ', _indentation);
}

/**
 * Return the plain text label of the criteria. You can use the \e useModernLabelling to
 * to improve the readability of the label. The indentation is included in the
 * returned string.
 */
QString ConsultResultCriteria::label(bool useModernLabelling) const
{
    if (_label.isEmpty() || !useModernLabelling)
        return QString("%1%2").arg(_indent).arg(_label);

    if (!_label.contains("++"))
        return QString("%1%2").arg(_indent).arg(_label);

    // Compute the modern label
    QString html = _label;
    html.replace("++1|", QChar(10112));
    html.replace("++2|", QChar(10113));
    html.replace("++3|", QChar(10114));
    html.replace("++4|", QChar(10115));
    html.replace("++++", QChar(9745));
    html.replace("+ -" , QChar(8226));
    return QString("%1%2").arg(_indent).arg(html);
}

/**
 * Return the html label of the criteria. You can use the \e useModernLabelling to
 * to improve the readability of the label. The indentation is included in the
 * returned string.
 */
QString ConsultResultCriteria::htmlLabel(bool useModernLabelling) const
{
    if (_label.isEmpty() || !useModernLabelling)
        return QString("%1%2").arg(_indent).arg(_label);

    if (!_label.contains("++"))
        return QString("%1%2").arg(_indent).arg(_label);

    // Compute the modern label
    QString html = _label;
    html.replace("++1|", QString("&#%1;").arg(10112));
    html.replace("++2|", QString("&#%1;").arg(10113));
    html.replace("++3|", QString("&#%1;").arg(10114));
    html.replace("++4|", QString("&#%1;").arg(10115));
    html.replace("++++", QString("&#%1;").arg(9745));
    html.replace("+ -" , QString("&#%1;").arg(8226));
    return QString("%1%2").arg(_indent).arg(Utils::htmlReplaceAccents(html));
}

/** Returns true if the CR criteria is only a line break (empty line) */
bool ConsultResultCriteria::isLineBreak() const
{
    return _label.trimmed().isEmpty();
}

/** Returns true if the criteria is a selection mandatory item */
bool ConsultResultCriteria::isSelectionMandatory() const
{
    return _label.contains("++++");
}

/** Returns true for criteria with a signifiant selection. Coded like '++X|' where X is numeric. */
bool ConsultResultCriteria::isSelectionSignifiant() const
{
    QRegExp reg("\\+\\+\\d\\|");
    return reg.indexIn(_label) != -1;
}

/** Construct an empty invalid object */
ConsultResult::ConsultResult() :
    _crId(-1),
    _diagnosisPosition(DiagnosisPositionUndefined),
    _medicalFollowUp(MedicalFollowUpUndefined),
    _symptomatic(SymptomaticStateUndefined),
    _chronicDisease(ChronicDiseaseStateUndefined)
{
}

ConsultResult::~ConsultResult()
{
}

/** Test emptyness of the object */
bool ConsultResult::isEmpty() const
{
    return (_crId == -1) ||
            (_selectedCriteriasIds.isEmpty() &&
             _diagnosisPosition == DiagnosisPositionUndefined &&
             _medicalFollowUp == MedicalFollowUpUndefined &&
             _symptomatic == SymptomaticStateUndefined &&
            _chronicDisease == ChronicDiseaseStateUndefined);
}

/** Test validity of the CR. A valid CR is a CR that can be identified and which all data are defined. */
bool ConsultResult::isValid() const
{
    return !isEmpty() &&
            !_selectedCriteriasIds.isEmpty() &&
            _diagnosisPosition != DiagnosisPositionUndefined &&
            _medicalFollowUp != MedicalFollowUpUndefined &&
            _symptomatic != SymptomaticStateUndefined &&
            _chronicDisease != ChronicDiseaseStateUndefined;
}

/** Clear the object */
void ConsultResult::clear()
{
    _crId = -1;
    _selectedCriteriasIds.clear();
    _diagnosisPosition = DiagnosisPositionUndefined;
    _medicalFollowUp = MedicalFollowUpUndefined;
    _symptomatic = SymptomaticStateUndefined;
    _chronicDisease = ChronicDiseaseStateUndefined;
    _crComment.clear();
    _critComment.clear();
}

/** Set the CR database \e id */
void ConsultResult::setConsultResult(int id)
{
    _crId = id;
}

/** Define the list of the selected criterias of the CR using their database primkeys */
void ConsultResult::setSelectedCriterias(const QList<int> &criteriaIds)
{
    _selectedCriteriasIds = criteriaIds;
}

/** Define the diagnostic position using the eDRC::Internal::ConsultResult::DiagnosisPosition enum */
void ConsultResult::setDiagnosisPosition(int pos)
{
    _diagnosisPosition = pos;
}

/** Define the medical follow up code using the eDRC::Internal::ConsultResult::MedicalFollowUp enum */
void ConsultResult::setMedicalFollowUp(int pos)
{
    _medicalFollowUp = pos;
}

/** Define if the CR is linked with a chronic disease (ALD in france) */
void ConsultResult::setChronicDiseaseState(int pos)
{
    _chronicDisease = pos;
}

/** Define if the CR is symptomatic or not */
void ConsultResult::setSymptomaticState(int pos)
{
    _symptomatic = pos;
}

int ConsultResult::consultResultId() const
{
    return _crId;
}

const QList<int> &ConsultResult::selectedCriterias() const
{
    return _selectedCriteriasIds;
}

ConsultResult::DiagnosisPosition ConsultResult::diagnosisPosition() const
{
    return ConsultResult::DiagnosisPosition(_diagnosisPosition);
}

ConsultResult::MedicalFollowUp ConsultResult::medicalFollowUp() const
{
    return ConsultResult::MedicalFollowUp(_medicalFollowUp);
}

ConsultResult::ChronicDiseaseState ConsultResult::chronicDiseaseState() const
{
    return ConsultResult::ChronicDiseaseState(_chronicDisease);
}

ConsultResult::SymptomaticState ConsultResult::symptomaticState() const
{
    return ConsultResult::SymptomaticState(_symptomatic);
}

namespace {
    const char * const XML_ROOT_TAG = "eDRC";
    const char * const XML_CR_TAG = "CR";
    const char * const XML_CR_COMMENTS = "Comments";
    const char * const XML_CR_COMMENT = "Comment";
    const char * const XML_ATTRIB_ID = "id";
    const char * const XML_ATTRIB_TYPE = "type";
    const char * const XML_ATTRIB_DIAG_POS = "posdiag";
    const char * const XML_ATTRIB_FOLLOWUP = "followup";
    const char * const XML_ATTRIB_CHRONIC = "chronic";
    const char * const XML_ATTRIB_SYMPTOMATIC = "sympto";
    const char * const XML_ATTRIB_CRITERIAS = "criterias";
    const char * const XML_COMMENT_ONCR = "OnCR";
    const char * const XML_COMMENT_ONCRITERIAS = "OnCrit";
}

/**
 * Transform object to XML. You can add some extra-xml code \e extraXml.
 * The extra content is automatically surrounded with a special tag: eDRC::Constants::XML_EXTRA_TAG.
*/
QString ConsultResult::toXml(const QString &extraXml) const
{
    // TODO: add the eDRC database version & coding system version
    QDomDocument doc("FreeMedForms");
    QDomElement root = doc.createElement(::XML_ROOT_TAG);
    doc.appendChild(root);
    QDomElement element = doc.createElement(::XML_CR_TAG);
    root.appendChild(element);
    element.setAttribute(::XML_ATTRIB_ID, _crId);

    switch (_diagnosisPosition) {
    case A: element.setAttribute(::XML_ATTRIB_DIAG_POS, "A"); break;
    case B: element.setAttribute(::XML_ATTRIB_DIAG_POS, "B"); break;
    case C: element.setAttribute(::XML_ATTRIB_DIAG_POS, "C"); break;
    case D: element.setAttribute(::XML_ATTRIB_DIAG_POS, "D"); break;
    case Z: element.setAttribute(::XML_ATTRIB_DIAG_POS, "Z"); break;
    default: break;
    }
    switch (_medicalFollowUp) {
    case N: element.setAttribute(::XML_ATTRIB_FOLLOWUP, "N"); break;
    case P: element.setAttribute(::XML_ATTRIB_FOLLOWUP, "P"); break;
    case R: element.setAttribute(::XML_ATTRIB_FOLLOWUP, "R"); break;
    default: break;
    }
    switch (_symptomatic) {
    case Symptomatic: element.setAttribute(::XML_ATTRIB_SYMPTOMATIC, "yes"); break;
    case NotSymptomatic: element.setAttribute(::XML_ATTRIB_SYMPTOMATIC, "no"); break;
    default: break;
    }
    switch (_chronicDisease) {
    case ChronicDisease: element.setAttribute(::XML_ATTRIB_CHRONIC, "yes"); break;
    case NotChronicDisease: element.setAttribute(::XML_ATTRIB_CHRONIC, "no"); break;
    default: break;
    }

    // Add criterias
    if (!_selectedCriteriasIds.isEmpty()) {
        QString selectedCriterias;
        foreach(const int id, _selectedCriteriasIds)
            selectedCriterias += QString("%1;").arg(id);
        selectedCriterias.chop(1);
        element.setAttribute(::XML_ATTRIB_CRITERIAS, selectedCriterias);
    }

    // Add comments
    QDomElement comments = doc.createElement(::XML_CR_COMMENTS);
    root.appendChild(comments);
    if (!_crComment.isEmpty()) {
        QDomElement comment = doc.createElement(::XML_CR_COMMENT);
        comment.setAttribute(::XML_ATTRIB_TYPE, ::XML_COMMENT_ONCR);
        QDomText t = doc.createTextNode(_crComment);
        comment.appendChild(t);
        comments.appendChild(comment);
    }
    if (!_critComment.isEmpty()) {
        QDomElement comment = doc.createElement(::XML_CR_COMMENT);
        comment.setAttribute(::XML_ATTRIB_TYPE, ::XML_COMMENT_ONCRITERIAS);
        QDomText t = doc.createTextNode(_critComment);
        comment.appendChild(t);
        comments.appendChild(comment);
    }

    // Add extra xml
    if (!extraXml.isEmpty()) {
        QString xml = doc.toString(2);
        int index = xml.lastIndexOf(QString("</%1>").arg(XML_ROOT_TAG));
        Q_ASSERT(index != -1);
        xml.insert(index, QString("<%1>%2</%1>").arg(Constants::XML_EXTRA_TAG).arg(extraXml));
        // TODO: check the validity of the extra xml content before breaking the whole document
        doc.setContent(xml);
    }
    return QString("<?xml version='1.0' encoding='UTF-8'?>\n%1").arg(doc.toString(2));
}

/**
 * Create a eDRC::Internal::ConsultResult object from XML. If an extra-xml content is available
 * in the \e xml code, you will get it in the \e extraXmlContent. Otherwise, this QString
 * is kept empty. The extra content is surrounded with a special tag: eDRC::Constants::XML_EXTRA_TAG.
 */
ConsultResult &ConsultResult::fromXml(const QString &xml, QString *extraXmlContent)
{
    ConsultResult *cr = new ConsultResult;
    // Prepare XML content
    QDomDocument doc;
    QString error;
    int line, col;
    if (!doc.setContent(xml, &error, &line, &col)) {
        LOG_ERROR_FOR("ConsultResult", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(error).arg(line).arg(col));
        return *cr;
    }
    QDomElement root = doc.firstChildElement(::XML_ROOT_TAG);
    if (root.isNull()) {
        LOG_ERROR_FOR("ConsultResult", tkTr(Trans::Constants::XML_WRONG_NUMBER_OF_TAG_1).arg(::XML_ROOT_TAG));
        return *cr;
    }
    QDomElement element = root.firstChildElement(::XML_CR_TAG);
    if (element.isNull()) {
        LOG_ERROR_FOR("ConsultResult", tkTr(Trans::Constants::XML_WRONG_NUMBER_OF_TAG_1).arg(::XML_CR_TAG));
        return *cr;
    }
    // Read CR data
    cr->setConsultResult(element.attribute(::XML_ATTRIB_ID).toInt());

    // Diagnostic position
    const QString &posDiag = element.attribute(::XML_ATTRIB_DIAG_POS);
    if (posDiag.compare("A", Qt::CaseInsensitive)==0) {
        cr->setDiagnosisPosition(A);
    } else if (posDiag.compare("B", Qt::CaseInsensitive)==0) {
        cr->setDiagnosisPosition(B);
    } else if (posDiag.compare("C", Qt::CaseInsensitive)==0) {
        cr->setDiagnosisPosition(C);
    } else if (posDiag.compare("D", Qt::CaseInsensitive)==0) {
        cr->setDiagnosisPosition(D);
    } else if (posDiag.compare("Z", Qt::CaseInsensitive)==0) {
        cr->setDiagnosisPosition(Z);
    }

    // Follow up
    const QString &f = element.attribute(::XML_ATTRIB_FOLLOWUP);
    if (f.compare("N", Qt::CaseInsensitive)==0) {
        cr->setMedicalFollowUp(N);
    } else if (f.compare("P", Qt::CaseInsensitive)==0) {
        cr->setMedicalFollowUp(P);
    } else if (f.compare("R", Qt::CaseInsensitive)==0) {
        cr->setMedicalFollowUp(R);
    }

    // Symptomatic state
    if (element.attribute(::XML_ATTRIB_SYMPTOMATIC).compare("yes", Qt::CaseInsensitive)==0) {
        cr->setSymptomaticState(Symptomatic);
    } else if (element.attribute(::XML_ATTRIB_SYMPTOMATIC).compare("no", Qt::CaseInsensitive)==0) {
        cr->setSymptomaticState(NotSymptomatic);
    }

    // Chronic disease state
    if (element.attribute(::XML_ATTRIB_CHRONIC).compare("yes", Qt::CaseInsensitive)==0) {
        cr->setChronicDiseaseState(ChronicDisease);
    } else if (element.attribute(::XML_ATTRIB_CHRONIC).compare("no", Qt::CaseInsensitive)==0) {
        cr->setSymptomaticState(NotChronicDisease);
    }

    // Selected criteria ids
    const QString &crit = element.attribute(::XML_ATTRIB_CRITERIAS);
    foreach(const QString &c, crit.split(";", QString::SkipEmptyParts))
        cr->_selectedCriteriasIds.append(c.toInt());

    // Comments
    element = root.firstChildElement(::XML_CR_COMMENTS);
    QDomElement comment = element.firstChildElement(::XML_CR_COMMENT);
    while (!comment.isNull()) {
        if (comment.attribute(::XML_ATTRIB_TYPE).compare(::XML_COMMENT_ONCR, Qt::CaseInsensitive)==0) {
            if (comment.childNodes().count() == 1) {
                cr->setHtmlCommentOnCR(comment.childNodes().at(0).nodeValue());
            }
        } else if (comment.attribute(::XML_ATTRIB_TYPE).compare(::XML_COMMENT_ONCRITERIAS, Qt::CaseInsensitive)==0) {
            if (comment.childNodes().count() == 1) {
                cr->setHtmlCommentOnCriterias(comment.childNodes().at(0).nodeValue());
            }
        }
        comment = comment.nextSiblingElement(::XML_CR_COMMENT);
    }

    // Extra-XML content
    if (extraXmlContent) {
        if (xml.contains(QString("<%1>").arg(Constants::XML_EXTRA_TAG))
                && xml.contains(QString("</%1>").arg(Constants::XML_EXTRA_TAG))) {
            int b = xml.indexOf(QString("<%1>").arg(Constants::XML_EXTRA_TAG));
            int e = xml.indexOf(QString("</%1>").arg(Constants::XML_EXTRA_TAG), b) + QString("<%1>").arg(Constants::XML_EXTRA_TAG).size() + 1;
            *extraXmlContent = xml.mid(b, e-b);
        }
    }
    return *cr;
}

/**
 * Construct an HTML representation of the CR.
 * You can use two mask:
 * - \e globalMask where you can use the following old token style:
 *      - [[CR_DATABASE_VERSION]]
 *      - [[CR_CODING_VALIDITY]]
 *      - [[CR_CODING_VALIDATOR_VERSION]]
 *      - [[CR_ID]]
 *      - [[CR_LABEL]]
 *      - [[CR_CRITERIAS]] (includes the mask for the selected criterias)
 *      - [[CR_DIAGNOSTIC_POSITION_CODE]]
 *      - [[CR_DIAGNOSTIC_POSITION_FULL_LABEL]]
 *      - [[CR_FOLLOWUP_CODE]]
 *      - [[CR_FOLLOWUP_FULL_LABEL]]
 *      - [[CR_CHRONIC_DISEASE]]
 *      - [[CR_SYMPTOMATIC_STATE]]
 *      - [[CR_ICD10_CODES]]
 *      - [[CR_ICD10_LABELS]]
 *      - [[CR_ICD10_CODES_AND_LABELS]]
 * - \e selectedCriteriaItemMask to define the way criterias are presented. You can use the following tokens
 *      - [[CRITERIA_LABEL]]
 *      - [[CRITERIA_MODERN_LABEL]]
 *      - [[CRITERIA_ID]]
 * This member needs to access to database \e edrcBase and does not manage a cache system.
 * Calling it many times can be CPU consuming.
 * \sa eDRC::Constants, eDRC::Constants::TOKEN_CR*
 */
QString ConsultResult::toHtml(const QString &globalMask, const QString &selectedCriteriaItemMask, eDRC::Internal::DrcDatabase &edrcBase) const
{
    QHash<QString, QVariant> tokens;

    // Prepare selected criterias
    QString criteriasString;
    QList<ConsultResultCriteria> criterias = edrcBase.getOrderedCriteriasForCr(_crId);
    foreach(const ConsultResultCriteria &crit, criterias) {
        if (_selectedCriteriasIds.contains(crit.id())) {
            tokens.insert(Constants::TOKEN_CRITERIA_ID, crit.id());
            tokens.insert(Constants::TOKEN_CRITERIA_DEFAULT_LABEL, crit.htmlLabel(false));
            tokens.insert(Constants::TOKEN_CRITERIA_MODERN_LABEL, crit.htmlLabel(true));
            QString tmp = selectedCriteriaItemMask;
            Utils::replaceTokens(tmp, tokens);
            criteriasString += tmp;
        }
    }
    tokens.clear();

    // Add CR tokens
    tokens.insert(Constants::TOKEN_CR_CRITERIAS, criteriasString);
    tokens.insert(Constants::TOKEN_CR_DATABASE_VERSION, edrcBase.version());
    tokens.insert(Constants::TOKEN_CR_CODING_VALIDITY, isValid()?tkTr(Trans::Constants::ISVALID):tkTr(Trans::Constants::ISNOTVALID));
    tokens.insert(Constants::TOKEN_CR_CODING_VALIDATOR_VERSION, ConsultResultValidator::version());
    tokens.insert(Constants::TOKEN_CR_ID, _crId);
    tokens.insert(Constants::TOKEN_CR_LABEL, edrcBase.getCrLabel(_crId));
    QString posDiagCode;
    QString posDiagLabel;
    switch (_diagnosisPosition) {
    case A: posDiagCode="A"; posDiagLabel=QApplication::translate("eDRC::Internal::ConsultResult", "Symptom"); break;
    case B: posDiagCode="B"; posDiagLabel=QApplication::translate("eDRC::Internal::ConsultResult", "Syndrome"); break;
    case C: posDiagCode="C"; posDiagLabel=QApplication::translate("eDRC::Internal::ConsultResult", "Disease definition"); break;
    case D: posDiagCode="D"; posDiagLabel=QApplication::translate("eDRC::Internal::ConsultResult", "Certified diagnostic"); break;
    case Z: posDiagCode="Z"; posDiagLabel=QApplication::translate("eDRC::Internal::ConsultResult", "Not pathological"); break;
    default: posDiagCode=QApplication::translate("eDRC::Internal::ConsultResult", "None"); posDiagLabel=QApplication::translate("eDRC::Internal::ConsultResult", "Diagnostic position is undefined"); break;
    }
    tokens.insert(Constants::TOKEN_CR_DIAGNOSTIC_POSITION_CODE, posDiagCode);
    tokens.insert(Constants::TOKEN_CR_CR_DIAGNOSTIC_POSITION_FULL_LABEL, posDiagLabel);
    QString fuCode;
    QString fuLabel;
    switch (_medicalFollowUp) {
    case N: fuCode="N"; fuLabel=QApplication::translate("eDRC::Internal::ConsultResult", "New"); break;
    case P: fuCode="P"; fuLabel=QApplication::translate("eDRC::Internal::ConsultResult", "Persistant"); break;
    case R: fuCode="R"; fuLabel=QApplication::translate("eDRC::Internal::ConsultResult", "Revised"); break;
    default: fuCode=QApplication::translate("eDRC::Internal::ConsultResult", "None"); fuLabel=QApplication::translate("eDRC::Internal::ConsultResult", "Medical follow up is undefined"); break;
    }
    tokens.insert(Constants::TOKEN_CR_FOLLOWUP_CODE, fuCode);
    tokens.insert(Constants::TOKEN_CR_FOLLOWUP_FULL_LABEL, fuLabel);

    switch (_chronicDisease) {
    case ChronicDisease: tokens.insert(Constants::TOKEN_CR_CHRONIC_DISEASE, QApplication::translate("eDRC::Internal::ConsultResult", "Chronic disease")); break;
    case NotChronicDisease: tokens.insert(Constants::TOKEN_CR_CHRONIC_DISEASE, QApplication::translate("eDRC::Internal::ConsultResult", "Not a chronic disease")); break;
    default: tokens.insert(Constants::TOKEN_CR_CHRONIC_DISEASE, QApplication::translate("eDRC::Internal::ConsultResult", "Chronic disease is undefined")); break;
    }
    switch (_symptomatic) {
    case Symptomatic: tokens.insert(Constants::TOKEN_CR_CR_SYMPTOMATIC_STATE, QApplication::translate("eDRC::Internal::ConsultResult", "Symptomatic")); break;
    case NotSymptomatic: tokens.insert(Constants::TOKEN_CR_CR_SYMPTOMATIC_STATE, QApplication::translate("eDRC::Internal::ConsultResult", "Not symptomatic")); break;
    default: tokens.insert(Constants::TOKEN_CR_CR_SYMPTOMATIC_STATE, QApplication::translate("eDRC::Internal::ConsultResult", "Symptomatic state is undefined")); break;
    }

    // TODO: correctly manage ICD10 labels and codes
    QString icd10 = edrcBase.getCrIcd10RelatedCodes(_crId, true).join("; ");
    tokens.insert(Constants::TOKEN_CR_CR_ICD10_CODES, icd10);
    tokens.insert(Constants::TOKEN_CR_ICD10_LABELS, icd10);
    tokens.insert(Constants::TOKEN_CR_ICD10_CODES_AND_LABELS, icd10);

    // Comments
    QTextDocument doc;
    doc.setHtml(htmlCommentOnCR());
    if (!doc.toPlainText().trimmed().isEmpty())
        tokens.insert(Constants::TOKEN_CR_GLOBAL_HTML_COMMENT, Utils::htmlBodyContent(htmlCommentOnCR(), false));
    else
        tokens.insert(Constants::TOKEN_CR_GLOBAL_HTML_COMMENT, QVariant());
    doc.setHtml(htmlCommentOnCriterias());
    if (!doc.toPlainText().trimmed().isEmpty())
        tokens.insert(Constants::TOKEN_CR_CRITERIAS_HTML_COMMENT, Utils::htmlBodyContent(htmlCommentOnCriterias(), false));
    else
        tokens.insert(Constants::TOKEN_CR_CRITERIAS_HTML_COMMENT, QVariant());

    // Process tokens
    QString tmp = globalMask;
    Utils::replaceTokens(tmp, tokens);
    tmp = Utils::htmlReplaceAccents(tmp);
    return tmp;
}

/** Compare two CR */
bool ConsultResult::operator==(const ConsultResult &other) const
{
    return _crId == other._crId &&
            _selectedCriteriasIds == other._selectedCriteriasIds &&
            _diagnosisPosition == other._diagnosisPosition &&
            _medicalFollowUp == other._medicalFollowUp &&
            _chronicDisease == other._chronicDisease &&
            _symptomatic == other._symptomatic &&
            _crComment == other._crComment &&
            _critComment == other._critComment
            ;
}

QDebug operator<<(QDebug dbg, const eDRC::Internal::ConsultResult &cr)
{
    if (cr.isEmpty()) {
        dbg.nospace() << "ConsultResult(Empty)";
        return dbg.space();
    }

    QString crit;
    foreach(int id, cr.selectedCriterias())
        crit += QString("%1,").arg(id);
    crit.chop(1);

    QString diag;
    switch (cr.diagnosisPosition()) {
    case ConsultResult::A: diag = "A"; break;
    case ConsultResult::B: diag = "B"; break;
    case ConsultResult::C: diag = "C"; break;
    case ConsultResult::D: diag = "D"; break;
    case ConsultResult::Z: diag = "Z"; break;
    default: diag = "Uncoded"; break;
    }

    QString fu;
    switch (cr.medicalFollowUp()) {
    case ConsultResult::N: fu = "N"; break;
    case ConsultResult::P: fu = "P"; break;
    case ConsultResult::R: fu = "R"; break;
    default: fu = "Uncoded"; break;
    }

    dbg.nospace() << QString("ConsultResult(%1%2; PosDiag:%3; FollowUp:%4; Crit:%5; %6; %7)")
                     .arg(cr.consultResultId())
                     .arg(cr.isValid()?" Valid":" Invalid")
                     .arg(diag)
                     .arg(fu)
                     .arg(crit)
                     .arg(cr.chronicDiseaseState()==ConsultResult::ChronicDisease?"ChronicDisease":"NotChronic")
                     .arg(cr.symptomaticState()==ConsultResult::Symptomatic?"Symptomatic":"NotSymptomatic")
                     ;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const eDRC::Internal::ConsultResultCriteria &crit)
{
    // TODO: write this
    return dbg.space();
}

