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
/**
 * \class eDRC::Internal::ConsultResultValidator
 * Check the validity of a user coded CR and provide error message (if required).
 * The validator only checks the CR criterias selection congruence with the
 * coding rules of the CR. \n
 * The validator as coding rules version. Any update of the internal code will
 * lead to a new version of the validator.
*/

#include "consultresultvalidator.h"
#include "consultresult.h"
#include "constants.h"

#include <utils/global.h>

#include <QList>
#include <QApplication>

#include <QDebug>

enum { WarnGroupCreation = false, WarnSelectionChecking = false };

using namespace eDRC;
using namespace Internal;

namespace eDRC {
namespace Internal {
// A group is a list of criterias that have the same parent and the same indentation. Groups
// are separated by a blank line with a weight of 7 or 8.
// Eg:
// ++++ TUMÉFACTION LOCALISÉE
// ++++ DOULOUREUSE souvent lancinante
//
// ++1| SUPERFICIELLE
//    ++++ rougeur et inflammation dans la localisation sous cutanée
//    ++++ augmentation de la chaleur locale
//    + – mise en évidence de pus
// ++1| INTRAMUSCULAIRE
//    ++++ mise en évidence de pus
//
// Group 1:
// ++++ TUMÉFACTION LOCALISÉE
// ++++ DOULOUREUSE souvent lancinante
// Group 2:
// ++1| SUPERFICIELLE
// ++1| INTRAMUSCULAIRE
// Group 3: (parent = ++1| SUPERFICIELLE)
//    ++++ rougeur et inflammation dans la localisation sous cutanée
//    ++++ augmentation de la chaleur locale
//    + – mise en évidence de pus
// Group 4: (parent = ++1| INTRAMUSCULAIRE)
//    ++++ mise en évidence de pus
//
// A group can contain
// - mandatory elements (starting with '++++')
// - required elements  (starting with '++X|' where X is a numeric value < 10)
// - optional elements  (starting with '+ -')
// - descriptive element ('asymptomatique')
class ConsultResultCriteriaGroup
{
public:
    ConsultResultCriteriaGroup() :
        _parentId(-1)
    {}

    // Add a criteria to the group
    void addCriteria(const ConsultResultCriteria &crit)
    {
        _criterias << crit;
    }

    // Returns the list of criterias of the group
    const QList<ConsultResultCriteria> &criterias() const
    {
        return _criterias;
    }

    // Returns the parent index in the list
    int parentIndex() const
    {
        return _parentId;
    }

    // Define the parent criteriaId of the group
    void setCriteriaParentId(int criteriaId)
    {
        _parentId = criteriaId;
    }

    // Join groups into one
    static ConsultResultCriteriaGroup &joinGroups(const QList<ConsultResultCriteriaGroup> &groups)
    {
        ConsultResultCriteriaGroup *gr = new ConsultResultCriteriaGroup;
        foreach(const ConsultResultCriteriaGroup &group, groups) {
            gr->_criterias << group._criterias;
            gr->_parentId = group.parentIndex();
        }
        return *gr;
    }

    int minimumItemToBeSelected() const
    {
        if (_criterias.isEmpty())
            return 0;
        // Find first label starting with '\\s*++'

        // Get the min using the first criteria label
        const QString &label = _criterias.at(0).label();
        if (label.contains("++1|"))
            return 1;
        else if (label.contains("++2|"))
            return 2;
        else if (label.contains("++3|"))
            return 3;
        else if (label.contains("++4|"))
            return 4;
        else if (label.contains("++5|"))
            return 5;
        else if (label.contains("++6|"))
            return 6;
        return 0;
    }

//    int numberOfSelectedItems(const QList<int> &selectedId)
//    {
//        if (selectedId.isEmpty())
//            return 0;
//    }

    QList<ConsultResultError> isSelectionValid(const QList<int> &selectedCriteriaIds) const
    {
        if (WarnSelectionChecking)
            qWarning() << "IS SELECTION VALID?";
        QList<ConsultResultError> errors;
        // Has parent && parent not selected -> nothing to do
        if (_parentId != -1 && !selectedCriteriaIds.contains(_parentId)) {
            if (WarnSelectionChecking)
                qWarning() << "    HAS PARENT AND PARENT NOT SELECTED";
            return errors;
        }

        int nbSelected = 0;
        int signifiance = 0;
        foreach(const ConsultResultCriteria &crit, _criterias) {
            if (WarnSelectionChecking) {
                QString sel;
                selectedCriteriaIds.contains(crit.id())? sel="Selected":sel="NotSelected";
                qWarning() << "    " <<  sel << crit.label();
            }

            if (crit.isSelectionMandatory()) {
                // All mandatory criterias are selected
                if (!selectedCriteriaIds.contains(crit.id())) {
                    errors << ConsultResultError(crit.id(), ConsultResultError::MissingMandatoryItem);
                }
            } else if (crit.isSelectionSignifiant()) {
                // Count all signifiants
                if (selectedCriteriaIds.contains(crit.id())) {
                    ++nbSelected;
                }
                // Get signifiance (eq: ++X| in labels)
                signifiance = crit.weight() - 1;
            }
        }
        if (WarnSelectionChecking)
            qWarning() << "    " << signifiance << nbSelected;

        // Missing signifiant item?
        if (signifiance > 0 && (nbSelected < signifiance)) {
            foreach(const ConsultResultCriteria &crit, _criterias) {
                if (crit.isSelectionSignifiant() && !selectedCriteriaIds.contains(crit.id())) {
                    errors << ConsultResultError(crit.id(), ConsultResultError::MissingSignifiantItem);
                }
            }
        }

        if (WarnSelectionChecking)
            qWarning() << "END IS SELECTION VALID =" << errors.isEmpty();

        return errors;
    }

private:
    QList<ConsultResultCriteria> _criterias;
    int _parentId;
};

class ConsultResultValidatorPrivate
{
public:
    ConsultResultValidatorPrivate(ConsultResultValidator *parent) :
        crId(-1),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~ConsultResultValidatorPrivate()
    {}

    // Create a criteria group starting from \e firstCriteria with the parent \e parentCriteria
    // Returns the last criteria id used (or -1 in case of error)
    int createGroup(int firstCriteria, int parentCriteria = -1)
    {

        if (!IN_RANGE_STRICT_MAX(firstCriteria, 0, _criterias.count())) {
            if (WarnGroupCreation)
                qWarning() << "ERROR";
            return -1;
        }

        // Find end of the group
        int currentIndentation = _criterias.at(firstCriteria).indentation();
        // If we are in a child branch but the parent is not known -> find it
        if (currentIndentation > 1 && parentCriteria == -1) {
            int i = firstCriteria;
            while (i >= 0) {
                const ConsultResultCriteria &crit = _criterias.at(i);
                if (crit.indentation() == (currentIndentation-1) &&
                        !crit.label().trimmed().isEmpty()) {
                    parentCriteria = i;
                    break;
                }
                --i;
            }
        }

        if (WarnGroupCreation)
            qWarning() << QString("%1CREATE").arg(QString().fill(' ', (currentIndentation-1)*2)) << firstCriteria << parentCriteria << _criterias.count();

        int i = firstCriteria;
        ConsultResultCriteriaGroup group;
        // Get parent criteriaId
        if (parentCriteria != -1) {
            if (!IN_RANGE_STRICT_MAX(parentCriteria, 0, _criterias.count()))
                return -1;
            const ConsultResultCriteria &crit = _criterias.at(parentCriteria);
            group.setCriteriaParentId(crit.id());
        }

        // Cycle throught available criterias
        while (i < _criterias.count()) {
            const ConsultResultCriteria &crit = _criterias.at(i);

            // New group (children)
            if (crit.indentation() > currentIndentation) {
                // Create the new group & continue the screening of criterias
                i = createGroup(i, i-1);
                if (i==-1) {
                    if (WarnGroupCreation)
                        qWarning() << QString("%1END").arg(QString().fill(' ', (currentIndentation-1)*2)) << i;
                    return -1;
                }
                continue;
            }

            // End of group
            if ((crit.weight() >= 7 && crit.isLineBreak())
                    || crit.indentation() < currentIndentation) {
                // If was a child group -> return
                if (parentCriteria != -1) {
                    _groups.prepend(group);
                    if (WarnGroupCreation)
                        qWarning() << QString("%1END (child)").arg(QString().fill(' ', (currentIndentation-1)*2)) << i;
                    return i;
                }
                // Create new group
                i = createGroup(i+1);
                continue;
            }

            // Same group
            if (crit.indentation() == currentIndentation) {
                if (WarnGroupCreation)
                    qWarning() << QString("%1  ADD").arg(QString().fill(' ', (currentIndentation-1)*2)) << crit.label();
                group.addCriteria(crit);
            }

            // Next criteria
            ++i;
        }
        _groups.prepend(group);
        if (WarnGroupCreation)
            qWarning() << QString("%1END (branch)").arg(QString().fill(' ', (currentIndentation-1)*2)) << i;
        return i;
    }

    // Check groups after their creation
    void joinGroups()
    {
        // Join all root groups that have only one criteria
        QList<ConsultResultCriteriaGroup> groups;
        for(int i = _groups.count() - 1; i >= 0; --i) {
            const ConsultResultCriteriaGroup &crit = _groups.at(i);
            if (crit.criterias().count() == 1 &&
                    crit.criterias().at(0).label().contains("++") &&
                    crit.parentIndex() == -1) {
                groups.prepend(_groups.takeAt(i));
            }
        }
        if (!groups.isEmpty()) {
            _groups.prepend(ConsultResultCriteriaGroup::joinGroups(groups));
        }
    }

public:
    int crId;
    QList<ConsultResultCriteria> _criterias;
    QList<ConsultResultCriteriaGroup> _groups;
    QList<int> _selectedCriterias;
    QList<ConsultResultError> _errors;
    QList<int> _wrongCriteriaIds;

private:
    ConsultResultValidator *q;
};
}
}

/** Returns a human readable error message */
QString ConsultResultError::errorShortMessage() const
{
    switch (type) {
    case MissingMandatoryItem: return QApplication::translate("ConsultResultError", "Mandatory item is not selected");
    case MissingSignifiantItem: return QApplication::translate("ConsultResultError", "Missing signifiant item in selection");
    }
    return QString();
}

/** Construct an empty invalid object */
ConsultResultValidator::ConsultResultValidator() :
    d(new ConsultResultValidatorPrivate(this))
{
}

/** Construct a valid object */
ConsultResultValidator::ConsultResultValidator(int crId, const QList<ConsultResultCriteria> &availableCriterias) :
    d(new ConsultResultValidatorPrivate(this))
{
    d->crId = crId;
    setAvailableCriterias(availableCriterias);
}

ConsultResultValidator::~ConsultResultValidator()
{
    if (d)
        delete d;
    d = 0;
}

/** Returns the version of the validator */
QString ConsultResultValidator::version()
{
    return Constants::VALIDATOR_VERSION;
}

/** Define the unique identifiant of the CR in the database \e crId */
void ConsultResultValidator::setCrId(int crId)
{
    d->crId = crId;
}

/**
 * Set the ordered list of available criterias for the coding of the CR.
 * The list must be correctly sorted.
 */
void ConsultResultValidator::setAvailableCriterias(const QList<ConsultResultCriteria> &availableCriterias)
{
    d->_criterias = availableCriterias;
    d->_groups.clear();
    d->createGroup(0);
    d->joinGroups();
    if (WarnGroupCreation) {
        foreach(const ConsultResultCriteriaGroup &group, d->_groups)
            qWarning() << group;
    }
}

/** Remove the criteria selection */
void ConsultResultValidator::clearSelectedCriterias()
{
    d->_selectedCriterias.clear();
}

/**
 * Define the selected criteria. The list contains an ordered or unordered list
 * of criteria database unique identifiers
 */
void ConsultResultValidator::setSelectedCriterias(const QList<int> &selectedId)
{
    d->_selectedCriterias = selectedId;
}

/**
 * Check the selection. You must call this member before extracting errors from the validator.
 * \code
    ConsultResultValidator validator;
    validator.setCrId(1);
    QList<int> selectedIds;
    validator.clearSelectedCriterias();
    validator.setSelectedCriterias(selectedIds);
    validator.check();

    // Check reported wrong ids
    QList<int> wrongCriteriaIdentifiers = validator.wrongCriteriaIds();
    ...
 * \endcode
 */
bool ConsultResultValidator::check()
{
    d->_errors.clear();
    d->_wrongCriteriaIds.clear();

    // Check all groups
    foreach(const ConsultResultCriteriaGroup &group, d->_groups)
        d->_errors << group.isSelectionValid(d->_selectedCriterias);

    // Construct wrong criteria id list
    foreach(const ConsultResultError &error, d->_errors)
        d->_wrongCriteriaIds << error.criteriaId;

    qWarning() << "VALID = " << d->_errors.isEmpty();
    return d->_errors.isEmpty();
}

/** Return the error message corresponding to the criteria with the database uid \e criteriaId or an empty string */
QString ConsultResultValidator::errorShortMessage(int criteriaId) const
{
    foreach(const ConsultResultError &error, d->_errors) {
        if (error.criteriaId == criteriaId)
            return error.errorShortMessage();
    }
    return QString();
}

/** Return the list of wrong criterias (the one that should/shouldn't be selected) or an empty list */
const QList<int> &ConsultResultValidator::wrongCriteriaIds() const
{
    return d->_wrongCriteriaIds;
}

/** Console debugging */
QDebug operator<<(QDebug dbg, const eDRC::Internal::ConsultResultCriteriaGroup &group)
{
    QString out;
    foreach(const eDRC::Internal::ConsultResultCriteria &crit, group.criterias()) {
        out += QString("    %1 (W:%2; I%3)\n")
               .arg(crit.label())
               .arg(crit.weight())
               .arg(crit.indentation());
    }
    if (out.isEmpty())
        dbg.nospace() << "CriteriaGroup(Empty)";
    else
        dbg.nospace() << QString("CriteriaGroup(\n%1)").arg(out);
    return dbg.space();
}


