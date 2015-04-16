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
    IDrugEngine(QObject *parent) : QObject(parent), m_IsActive(true) {}
    virtual ~IDrugEngine() {}

    virtual bool init() = 0;
    virtual bool isActive() const {return m_IsActive;}
    virtual bool isActiveByDefault() const = 0;
    virtual bool canComputeInteractions() const = 0;

    virtual bool isCalculatingDrugDrugInteractions() const = 0;
    virtual bool isCalculatingPatientDrugInteractions() const = 0;
    virtual bool isCalculatingPatientDrugAllergiesAndIntolerances() const = 0;

    virtual QString uid() const = 0;
    virtual QString name() const = 0;
    virtual QString shortName() const = 0;
    virtual QString tooltip() const = 0;
    virtual QIcon icon(const int size = 0) const = 0;
    virtual QString iconFullPath(const int size = 0) const = 0;

    virtual int calculateInteractions(const QVector<IDrug *> &drugs) = 0;
    virtual QVector<IDrugInteraction *> getAllInteractionsFound() = 0;
    virtual QVector<IDrugInteractionAlert *> getAllAlerts(DrugInteractionResult *addToResult) = 0;

    virtual QAbstractItemModel *precautionModel() const = 0;

    virtual QString engineDataReport() const {return QString();}

public Q_SLOTS:
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
