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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSDB_IDRUGENGINE_H
#define DRUGSDB_IDRUGENGINE_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QObject>
#include <QIcon>
#include <QStandardItemModel>

/**
 * \file idrugengine.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 17 Jan 2012
*/

namespace DrugsDB {
class IDrugInteraction;
class IDrugInteractionAlert;
class DrugInteractionResult;
class IDrug;

class DRUGSBASE_EXPORT IDrugEngine : public QObject
{
    Q_OBJECT

public:
    /** Construct an active empty drug engine*/
    IDrugEngine(QObject *parent) : QObject(parent), m_IsActive(true) {}
    virtual ~IDrugEngine() {}

    /** Initialize the engine. Return true if the engine is initialized. */
    virtual bool init() = 0;

    /** Return true if the engine is active. Inactive engines will not compute interactions. */
    virtual bool isActive() const {return m_IsActive;}

    /** Return true if the engine should be active by default. This state is used for the preferences. */
    virtual bool isActiveByDefault() const = 0;

    /** Return true if the engine is enable to compute interactions. */
    virtual bool canComputeInteractions() const = 0;

    /** Return true if the engine is computing drug-drug interactions. */
    virtual bool isCalculatingDrugDrugInteractions() const = 0;
    /** Return true if the engine is computing patient-drug interactions. */
    virtual bool isCalculatingPatientDrugInteractions() const = 0;
    /** Return true if the engine is computing drug allergies and intolerances. */
    virtual bool isCalculatingPatientDrugAllergiesAndIntolerances() const = 0;

    /** Unique identifier of the engine. */
    virtual QString uid() const = 0;
    /** Complete name of the engine. */
    virtual QString name() const = 0;
    /** Short name of the engine (eg: DDI, PIM). */
    virtual QString shortName() const = 0;
    /** Tooltip to use when presenting the engine in UIs. */
    virtual QString tooltip() const = 0;
    /** Icon of the engine. */
    virtual QIcon icon(const int size = 0) const = 0;
    /** Path of the icon of the engine. */
    virtual QString iconFullPath(const int size = 0) const = 0;

    /**
       Start the computation of interactions with the specified drugs list \e drugs.\n
       Return the number of found interactions.\n
       The interaction manager will send the drug list once and call the:
       - getAllInteractionsFound()
       - getAllAlerts().
    */
    virtual int calculateInteractions(const QVector<IDrug *> &drugs) = 0;
    /** Return the found interactions during the calculateInteractions() process. */
    virtual QVector<IDrugInteraction *> getAllInteractionsFound() = 0;
    /** Return the alerts created during the calculateInteractions() process. */
    virtual QVector<IDrugInteractionAlert *> getAllAlerts(DrugInteractionResult *addToResult) = 0;

    /** Return a model with all synthesis information to use with a treeview/listview. */
    virtual QAbstractItemModel *precautionModel() const = 0;

public Q_SLOTS:
    /** Define the state of activity of the engine. \sa isActive() */
    virtual void setActive(bool state) {m_IsActive = state;}

protected:
    bool m_IsActive;
};

class DRUGSBASE_EXPORT IDrugAllergyEngine : public IDrugEngine
{
    Q_OBJECT

public:
    enum TypeOfInteraction {
        NoInteraction = 0,
        Intolerance,
        SuspectedIntolerance,
        Allergy,
        SuspectedAllergy
    };

    enum TypeOfSubstrat {
        InnCode = 0,
        ClassInn,
        Drug
    };
    /** Construct an active empty drug engine*/
    IDrugAllergyEngine(QObject *parent) : IDrugEngine(parent) {}
    virtual ~IDrugAllergyEngine() {}

    bool isCalculatingPatientDrugAllergiesAndIntolerances() const {return true;}

    virtual void check(const int typeOfInteraction, const QString &uid, const QString &drugGlobalAtcCode = QString::null, const int drugsDatabaseSourceId = -1) = 0;
    virtual bool has(const int typeOfInteraction, const QString &uid, const int drugsDatabaseSourceId = -1) = 0;

    virtual QStandardItemModel *drugPrecautionModel() const = 0;

Q_SIGNALS:
    void allergiesUpdated();
    void intolerancesUpdated();

};


}  // End namespace DrugsDB


#endif // DRUGSDB_IDRUGENGINE_H
