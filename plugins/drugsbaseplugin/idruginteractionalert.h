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
#ifndef IDRUGINTERACTIONALERT_H
#define IDRUGINTERACTIONALERT_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/druginteractioninformationquery.h>
#include <drugsbaseplugin/idrugengine.h>

class QIcon;
class QWidget;

namespace DrugsDB {
class DrugInteractionResult;
class IDrug;

class DRUGSBASE_EXPORT IDrugInteractionAlert
{
public:
    IDrugInteractionAlert(IDrugEngine *engine) : m_Engine(engine){}
    virtual ~IDrugInteractionAlert() {}

    virtual QString engineUid() const {if (m_Engine) return m_Engine->uid(); return QString();}
    virtual IDrugEngine *engine() const {return m_Engine;}

    // static alert
    virtual QIcon icon(const IDrug *drug, const DrugInteractionInformationQuery &query) const = 0;
    virtual QString message(const IDrug *drug, const DrugInteractionInformationQuery &query) const = 0;
    virtual QString message(const DrugInteractionInformationQuery &query) const = 0;

    // dynamic alert
    virtual bool hasDynamicAlertWidget(const DrugInteractionInformationQuery &query) const = 0;
    virtual QWidget *dynamicAlertWidget(const DrugInteractionInformationQuery &query, QWidget *parent = 0) = 0;
    virtual void setOverridden(bool overridden) = 0;
    virtual bool wasOverridden() const = 0;

private:
    IDrugEngine *m_Engine;
};

}  // End namespace DrugsDB

#endif // IDRUGINTERACTIONALERT_H
