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

#include <QRegExp>

using namespace eDRC;
using namespace Internal;

/** Construct an empty invalid object */
ConsultResultCriteria::ConsultResultCriteria() :
    _id(-1), _weight(-1), _indentation(-1), _sortIndex(-1)
{}

ConsultResultCriteria::~ConsultResultCriteria()
{}

void ConsultResultCriteria::setIndentation(int indentation)
{
    _indentation = indentation;
    _indent.clear();
    if (_indentation >= 2)
        _indent.fill(' ', _indentation);
}

QString ConsultResultCriteria::label(bool useModernLabelling) const
{
    if (_label.isEmpty() || !useModernLabelling)
        return QString("%1%2").arg(_indent).arg(_label);

    if (!_label.contains("++"))
        return QString("%1%2").arg(_indent).arg(_label);

    QString html = _label;
    html.replace("++1|", QChar(10112));
    html.replace("++2|", QChar(10113));
    html.replace("++3|", QChar(10114));
    html.replace("++4|", QChar(10115));
    html.replace("++++", QChar(9745));
    html.replace("+ -" , QChar(8226));
    return QString("%1%2").arg(_indent).arg(html);
}

QString ConsultResultCriteria::htmlLabel(bool useModernLabelling) const
{
    if (_label.isEmpty() || !useModernLabelling)
        return QString("%1%2").arg(_indent).arg(_label);

    if (!_label.contains("++"))
        return QString("%1%2").arg(_indent).arg(_label);

    QString html = _label;
    html.replace("++1|", QString("&#%1;").arg(10112));
    html.replace("++2|", QString("&#%1;").arg(10113));
    html.replace("++3|", QString("&#%1;").arg(10114));
    html.replace("++4|", QString("&#%1;").arg(10115));
    html.replace("++++", QString("&#%1;").arg(9745));
    html.replace("+ -" , QString("&#%1;").arg(8226));
    return QString("%1%2").arg(_indent).arg(html);
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

#include <QDebug>
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
    return (_crId == -1) || _selectedCriteriasIds.isEmpty();
}

/** Test validity of the CR. A valid CR is a CR that can be identified and which all data are defined. */
bool ConsultResult::isValid() const
{
    return !isEmpty() &&
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
}

void ConsultResult::setConsultResult(int id)
{
    _crId = id;
}

/** Define the list of the selected criterias of the CR using their database primkeys */
void ConsultResult::setSelectedCriterias(const QList<int> &criteriaIds)
{
    _selectedCriteriasIds = criteriaIds;
}

void ConsultResult::setDiagnosisPosition(int pos)
{
    _diagnosisPosition = pos;
}

void ConsultResult::setMedicalFollowUp(int pos)
{
    _medicalFollowUp = pos;
}

void ConsultResult::setChronicDiseaseState(int pos)
{
    _chronicDisease = pos;
}

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
