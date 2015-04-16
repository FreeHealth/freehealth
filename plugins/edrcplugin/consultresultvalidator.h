/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef EDRC_PLUGIN_CONSULTRESULTVALIDATOR_H
#define EDRC_PLUGIN_CONSULTRESULTVALIDATOR_H

#include <QList>

/**
 * \file resultconsultvalidator.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 18 June 2013
*/

namespace eDRC {
namespace Internal {
class ConsultResultCriteriaGroup;
class ConsultResultValidatorPrivate;
class ConsultResultCriteria;

struct ConsultResultError {
    enum ErrorType {
        MissingMandatoryItem = 0,
        MissingSignifiantItem
    };

    ConsultResultError() : criteriaId(-1), type(-1) {}
    ConsultResultError(int _criteriaId, int _type) : criteriaId(_criteriaId), type(_type) {}

    int criteriaId;
    int type;
    QString errorShortMessage() const;
};

class ConsultResultValidator
{
public:
    ConsultResultValidator();
    ConsultResultValidator(int crId, const QList<ConsultResultCriteria> &availableCriterias);
    ~ConsultResultValidator();
    static QString version();

    void setCrId(int crId);
    void setAvailableCriterias(const QList<ConsultResultCriteria> &availableCriterias);
    void clearSelectedCriterias();
    void setSelectedCriterias(const QList<int> &selectedId);

    bool check();
    QString errorShortMessage(int criteriaId) const;
    const QList<int> &wrongCriteriaIds() const;

private:
    ConsultResultValidatorPrivate *d;
};

} // namespace eDRC
} // namespace Internal

QDebug operator<<(QDebug dbg, const eDRC::Internal::ConsultResultCriteriaGroup &group);

#endif // EDRC_PLUGIN_CONSULTRESULTVALIDATOR_H
