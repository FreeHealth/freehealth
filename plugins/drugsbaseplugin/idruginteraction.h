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
#ifndef DRUGSDB_IDRUGINTERACTION_H
#define DRUGSDB_IDRUGINTERACTION_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/idrug.h>

/**
 * \file idruginteraction.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.5
 * \date 25 Jun 2012
*/

namespace DrugsDB {
class IDrugEngine;
namespace Internal {
class IDrugInteractionPrivate;
}

class DRUGSBASE_EXPORT IDrugInteraction
{
public:
    explicit IDrugInteraction(IDrugEngine *) {}
    virtual ~IDrugInteraction() {}

    virtual IDrugEngine *engine() const = 0;

    virtual bool isDrugDrugInteraction() const = 0;
    virtual bool isPotentiallyInappropriate() const = 0;

    virtual QString type() const = 0;

    virtual QList<IDrug *> drugs() const = 0;

    virtual QIcon icon(const int levelOfWarning = 0, const int iconsize = 0) const = 0;
    virtual QString header(const QString &separator = QString::null) const = 0;
    virtual QString risk(const QString &lang = QString::null) const = 0;
    virtual QString management(const QString &lang = QString::null) const = 0;
    virtual QString referencesLink(const QString &lang = QString::null) const = 0;

    virtual QString toHtml(bool detailled = false) const = 0;

    virtual int sortIndex() const = 0;

    /**
      \brief List sorting helpers.
      This sort helpers uses the sortIndex()
    */
    static bool lessThan(const IDrugInteraction *int1, const IDrugInteraction *int2) {return int1->sortIndex() < int2->sortIndex();}

    /**
      \brief List sorting helpers.
      This sort helpers uses the sortIndex()
    */
    static bool greaterThan(const IDrugInteraction *int1, const IDrugInteraction *int2) {return int1->sortIndex() > int2->sortIndex();}
};

}  // End DrugsDB

DRUGSBASE_EXPORT QDebug operator<<(QDebug dbg, const DrugsDB::IDrugInteraction &c);
DRUGSBASE_EXPORT QDebug operator<<(QDebug dbg, const DrugsDB::IDrugInteraction *c);

#endif  // DRUGSDB_IDRUGINTERACTION_H

