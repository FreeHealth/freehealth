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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGDRUGINTERACTIONCORE_H
#define DRUGDRUGINTERACTIONCORE_H

#include <QObject>
#include <QHash>
#include <QDomNode>
#include <QMultiMap>

/**
 * \file drugdruginteractioncore.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 09 Dec 2012
*/

namespace DrugsDB {
class DrugsDBCore;
namespace Internal {
class DrugBaseEssentials;
}
}

namespace DrugsDB {
namespace Internal {
class DrugInteractor;
class DrugDrugInteraction;
class DrugInteractorModel;
class DrugDrugInteractionCorePrivate;
}  // namespace Internal

class DrugDrugInteractionCore : public QObject
{
    Q_OBJECT
    friend class DrugsDB::DrugsDBCore;

protected:
    explicit DrugDrugInteractionCore(QObject *parent = 0);
    bool initialize();

public:
    static DrugDrugInteractionCore *instance();
    ~DrugDrugInteractionCore();

    int createInternalUuid() const;

    // Available models
    Internal::DrugInteractorModel *interactingMoleculesModel() const;
    Internal::DrugInteractorModel *interactingClassesModel() const;

    QList<Internal::DrugDrugInteraction *> getDrugDrugInteractions() const;
    QList<Internal::DrugInteractor *> getDrugInteractors() const;

    // Feeders / Checkers
    bool canAddAtc() const;
    bool canAddDrugDrugInteractions() const;
    bool canAddPims() const;
    bool canAddPregnancyChecking() const;

    bool isAtcInstalledInDatabase(DrugsDB::Internal::DrugBaseEssentials *database);
    bool addAtcDataToDatabase(DrugsDB::Internal::DrugBaseEssentials *database);
    bool addDrugDrugInteractionsToDatabase(DrugsDB::Internal::DrugBaseEssentials *database);
    bool addPimsToDatabase(DrugsDB::Internal::DrugBaseEssentials *database);
    bool addPregnancyCheckingDataToDatabase(DrugsDB::Internal::DrugBaseEssentials *database);
    bool populateDrugDatabase(DrugsDB::Internal::DrugBaseEssentials *database);

public Q_SLOTS:
    void updateXmlFileForDrugDrugInteraction(Internal::DrugDrugInteraction *ddi);
    void saveCompleteList(const QList<Internal::DrugDrugInteraction *> &ddis);
    void saveCompleteList(const QList<Internal::DrugInteractor *> &interactors);

    Internal::DrugInteractor *createNewInteractor(const QString &initialLabel, const bool isClass);
    void downloadAllPmids();

private:
    Internal::DrugDrugInteractionCorePrivate *d;
};

}  // End namespace DrugsDB

#endif // DRUGDRUGINTERACTIONCORE_H
