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
#ifndef EDRC_PLUGIN_CONSULTRESULT_H
#define EDRC_PLUGIN_CONSULTRESULT_H

#include <QList>
#include <QString>

/**
 * \file resultconsult.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 18 June 2013
*/

namespace eDRC {
namespace Internal {

class ConsultResultCriteria
{
public:
    ConsultResultCriteria();
    ~ConsultResultCriteria();

    void setId(int id) {_id = id;}
    void setLabel(const QString &label) {_label = label;}
    void setIndentation(int indentation);
    void setWeight(int weight) {_weight = weight;}
    void setSortIndex(int sortIndex) {_sortIndex = sortIndex;}

    int id() const {return _id;}
    QString label(bool useModernLabelling = false) const;
    QString htmlLabel(bool useModernLabelling = false) const;
    int indentation() const {return _indentation;}
    int weight() const {return _weight;}
    int sortIndex() const {return _sortIndex;}

    bool isLineBreak() const;
    bool isSelectionMandatory() const;
    bool isSelectionSignifiant() const;

private:
    int _id, _weight, _indentation, _sortIndex;
    QString _label, _indent;
};

class ConsultResult
{
public:
    enum SymptomaticState {
        SymptomaticStateUndefined = -1,
        Symptomatic,
        NotSymptomatic
    };

    enum ChronicDiseaseState { // == ALD
        ChronicDiseaseStateUndefined = -1,
        ChronicDisease,
        NotChronicDisease
    };

    enum DiagnosisPosition {
        DiagnosisPositionUndefined = -1,
        A,
        B,
        C,
        D
    };

    enum MedicalFollowUp {
        MedicalFollowUpUndefined = -1,
        N,
        P,
        R
    };

    ConsultResult();
    ~ConsultResult();

    bool isEmpty() const;
    bool isValid() const;

    void clear();

    void setConsultResult(int id);
    void setSelectedCriterias(const QList<int> &criteriaIds);
    void setDiagnosisPosition(int pos);
    void setMedicalFollowUp(int pos);
    void setChronicDiseaseState(int pos);
    void setSymptomaticState(int pos);

    int consultResultId() const;
    const QList<int> &selectedCriterias() const;
    DiagnosisPosition diagnosisPosition() const;
    MedicalFollowUp medicalFollowUp() const;
    ChronicDiseaseState chronicDiseaseState() const;
    SymptomaticState symptomaticState() const;

//    QString toXml() const;
//    bool fromXml(const QDomElement &root);

private:
    int _crId;
    QList<int> _selectedCriteriasIds;
    int _diagnosisPosition, _medicalFollowUp;
    int _symptomatic, _chronicDisease;
};

} // namespace eDRC
} // namespace Internal

#endif // EDRC_PLUGIN_CONSULTRESULT_H
