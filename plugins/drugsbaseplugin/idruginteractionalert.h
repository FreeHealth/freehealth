/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef IDRUGINTERACTIONALERT_H
#define IDRUGINTERACTIONALERT_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/druginteractioninformationquery.h>

class QIcon;
class QWidget;

namespace DrugsDB {
class DrugInteractionResult;
class IDrug;

class DRUGSBASE_EXPORT IDrugInteractionAlert //: public QObject
{
//    Q_OBJECT
public:
    IDrugInteractionAlert() {}
    virtual ~IDrugInteractionAlert() {}

    virtual QString engineUid() const = 0;

    // static alert
    virtual QIcon icon(const IDrug *drug, const DrugInteractionInformationQuery &query) const = 0;
    virtual QString message(const IDrug *drug, const DrugInteractionInformationQuery &query) const = 0;
    virtual QString message(const DrugInteractionInformationQuery &query) const = 0;

    // dynamic alert
    virtual bool hasDynamicAlertWidget(const DrugInteractionInformationQuery &query) const = 0;
    virtual QWidget *dynamicAlertWidget(const DrugInteractionInformationQuery &query, QWidget *parent = 0) = 0;
    virtual void setOverridden(bool overridden) = 0;
    virtual bool wasOverridden() const = 0;
};

}  // End namespace DrugsDB

#endif // IDRUGINTERACTIONALERT_H
