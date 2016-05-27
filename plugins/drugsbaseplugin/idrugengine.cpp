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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

#include "idrugengine.h"

/**
 * \class DrugsDB::IDrugEngine
 * This engine is responsible of drug interaction computations. Its interface is accessed
 * using a QVector of drugs. Objects must be stored into the pluginmanager object pool.
 */

/**
 * \fn DrugsDB::IDrugEngine::IDrugEngine(QObject *parent)
 * Construct an active empty drug engine
 */

/** \fn virtual bool DrugsDB::IDrugEngine::init() = 0;
 * Initialize the engine. Return true if the engine is initialized.
 */

/** \fn virtual bool DrugsDB::IDrugEngine::isActive() const
 * Return true if the engine is active. Inactive engines will not compute interactions.
 */

/** \fn virtual bool DrugsDB::IDrugEngine::isActiveByDefault() const = 0;
 * Return true if the engine should be active by default. This state is used for the preferences.
 */

/** \fn virtual bool DrugsDB::IDrugEngine::canComputeInteractions() const = 0;
 * Return true if the engine is enable to compute interactions.
 */

/** \fn virtual bool DrugsDB::IDrugEngine::isCalculatingDrugDrugInteractions() const = 0;
 * Return true if the engine is computing drug-drug interactions.
 */

/** \fn virtual bool DrugsDB::IDrugEngine::isCalculatingPatientDrugInteractions() const = 0;
 * Return true if the engine is computing patient-drug interactions.
 */

/** \fn virtual bool DrugsDB::IDrugEngine::isCalculatingPatientDrugAllergiesAndIntolerances() const = 0;
 * Return true if the engine is computing drug allergies and intolerances.
 */

/** \fn virtual QString DrugsDB::IDrugEngine::uid() const = 0;
 * Unique identifier of the engine.
 */

/** \fn virtual QString DrugsDB::IDrugEngine::name() const = 0;
 * Complete name of the engine.
 */

/** \fn virtual QString DrugsDB::IDrugEngine::shortName() const = 0;
 * Short name of the engine (eg: DDI, PIM).
 */

/** \fn virtual QString DrugsDB::IDrugEngine::tooltip() const = 0;
 * Tooltip to use when presenting the engine in UIs.
 */

/** \fn virtual QIcon DrugsDB::IDrugEngine::icon(const int size = 0) const = 0;
 * Icon of the engine.
 */

/** \fn virtual QString DrugsDB::IDrugEngine::iconFullPath(const int size = 0) const = 0;
 * Path of the icon of the engine.
 */

/** \fn virtual int DrugsDB::IDrugEngine::calculateInteractions(const QVector<DrugsDB::IDrug *> &drugs) = 0;
   Start the computation of interactions with the specified drugs list \e drugs.\n
   Return the number of found interactions.\n
   The interaction manager will send the drug list once and call the:
   - getAllInteractionsFound()
   - getAllAlerts().
*/

/** \fn virtual QVector<DrugsDB::IDrugInteraction *> DrugsDB::IDrugEngine::getAllInteractionsFound() = 0;
 * Return the found interactions during the calculateInteractions() process.
 */

/** \fn virtual QVector<DrugsDB::IDrugInteractionAlert *> DrugsDB::IDrugEngine::getAllAlerts(DrugInteractionResult *addToResult) = 0;
 * Return the alerts created during the calculateInteractions() process.
 */

/** \fn virtual QAbstractItemModel *DrugsDB::IDrugEngine::precautionModel() const = 0;
 * Return a model with all synthesis information to use with a treeview/listview.
 */

/** \fn virtual QString DrugsDB::IDrugEngine::engineDataReport() const {return QString();}
 * Return a report on the engine data (for eg the number of available interactions...)
 */

/** \fn virtual void DrugsDB::IDrugEngine::setActive(bool state) {m_IsActive = state;}
 * Define the state of activity of the engine. \sa isActive()
 */

