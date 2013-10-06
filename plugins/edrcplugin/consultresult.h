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
#ifndef EDRC_PLUGIN_CONSULTRESULT_H
#define EDRC_PLUGIN_CONSULTRESULT_H

#include <edrcplugin/edrc_exporter.h>
#include <QList>
#include <QString>
#include <QDateTime>

/**
 * \file resultconsult.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 23 Sept 2013
*/

// TODO: ConsultResult should not be internal if you use it with the editor...

namespace eDRC {
namespace Internal {
class DrcDatabase;

class ConsultResultCriteria
{
public:
    ConsultResultCriteria();
    ~ConsultResultCriteria();

    void setId(int id) {_id = id;}
    void setLabel(const QString &label) {_label = label;}
    void setComment(const QString &comment) {_comment = comment;}
    void setIndentation(int indentation);
    void setWeight(int weight) {_weight = weight;}
    void setSortIndex(int sortIndex) {_sortIndex = sortIndex;}

    int id() const {return _id;}
    QString label(bool useModernLabelling = false) const;
    QString comment() const {return  _comment;}
    QString htmlLabel(bool useModernLabelling = false) const;
    int indentation() const {return _indentation;}
    int weight() const {return _weight;}
    int sortIndex() const {return _sortIndex;}

    bool isLineBreak() const;
    bool isSelectionMandatory() const;
    bool isSelectionSignifiant() const;

private:
    int _id, _weight, _indentation, _sortIndex;
    QString _label, _indent, _comment;
};

class EDRC_EXPORT ConsultResult
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
        D,
        Z
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
    void setDateOfExamination(const QDateTime &dt);
    void setHtmlCommentOnCR(const QString &htmlComment) {_crComment = htmlComment;}
    void setHtmlCommentOnCriterias(const QString &htmlComment)  {_critComment = htmlComment;}

    int consultResultId() const;
    const QList<int> &selectedCriterias() const;
    DiagnosisPosition diagnosisPosition() const;
    MedicalFollowUp medicalFollowUp() const;
    ChronicDiseaseState chronicDiseaseState() const;
    SymptomaticState symptomaticState() const;
    QString htmlCommentOnCR() const {return _crComment;}
    QString htmlCommentOnCriterias() const {return _critComment;}
    QDateTime dateOfExamination() const {return _dateOfExamination;}

    QString toXml(const QString &extraXml = QString::null) const;
    static QString listToXml(const QList<ConsultResult> &list, const QString &extraXml = QString::null);
    static QList<ConsultResult> &fromXml(const QString &xml, QString *extraXmlContent = 0);

    QString toHtml(const QString &globalMask, const QString &selectedCriteriaItemMask, eDRC::Internal::DrcDatabase &edrcBase) const;

    bool operator==(const ConsultResult &other) const;
    bool operator!=(const ConsultResult &other) const {return !operator==(other);}

    static bool lessThanByDate(const ConsultResult &one, const ConsultResult &two);
    static bool greaterThanByDate(const ConsultResult &one, const ConsultResult &two);

    QString diagnosisPositionToHumanReadable() const;
    QString medicalFollowUpToHumanReadable() const;
    QString chronicDiseaseStateToHumanReadable() const;
    QString symptomaticStateToHumanReadable() const;

private:
    int _crId;
    QList<int> _selectedCriteriasIds;
    int _diagnosisPosition, _medicalFollowUp;
    int _symptomatic, _chronicDisease;
    QString _crComment, _critComment;
    QDateTime _dateOfExamination;
};

} // namespace Internal
} // namespace eDRC

QDebug EDRC_EXPORT operator<<(QDebug dbg, const eDRC::Internal::ConsultResult &cr);
QDebug EDRC_EXPORT operator<<(QDebug dbg, const eDRC::Internal::ConsultResultCriteria &crit);

#endif // EDRC_PLUGIN_CONSULTRESULT_H
