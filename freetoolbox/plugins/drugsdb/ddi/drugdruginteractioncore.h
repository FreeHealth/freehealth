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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGDRUGINTERACTIONCORE_H
#define DRUGDRUGINTERACTIONCORE_H

#include <QObject>
#include <QHash>
#include <QDomNode>
#include <QMultiMap>

namespace DrugsDB {
class DrugsDBCore;
namespace Internal {
class DrugBaseEssentials;
}
}

namespace IAMDb {
class DrugDrugInteraction;
class DrugInteractor;
class DrugInteractorModel;
namespace Internal {
class DrugDrugInteractionCorePrivate;
}

class DrugDrugInteractionCore : public QObject
{
    Q_OBJECT
    friend class DrugsDB::DrugsDBCore;

protected:
    explicit DrugDrugInteractionCore(QObject *parent = 0);
    bool initailize();

public:
    static DrugDrugInteractionCore *instance();
    ~DrugDrugInteractionCore();

    int createInternalUuid() const;

    // Available models
    DrugInteractorModel *interactingMoleculesModel() const;
    DrugInteractorModel *interactingClassesModel() const;

    QList<DrugDrugInteraction *> getDrugDrugInteractions() const;
    QList<DrugInteractor *> getDrugInteractors() const;

    bool populateDrugDatabase(DrugsDB::Internal::DrugBaseEssentials *database);

Q_SIGNALS:
    void interactorCreated(DrugInteractor *di);

public Q_SLOTS:
    void updateXmlFileForDrugDrugInteraction(DrugDrugInteraction *ddi);
    void saveCompleteList(const QList<DrugDrugInteraction *> &ddis);
    void saveCompleteList(const QList<DrugInteractor *> &interactors);

    DrugInteractor *createNewInteractor(const QString &initialLabel, const bool isClass);
    void downloadAllPmids();

private:
    bool saveAtcClassification(DrugsDB::Internal::DrugBaseEssentials *database);
    bool saveClassDrugInteractor(DrugInteractor *interactor, const QList<DrugInteractor *> &completeList, DrugsDB::Internal::DrugBaseEssentials *database, DrugInteractor *parent);
    bool saveDrugDrugInteractions(DrugsDB::Internal::DrugBaseEssentials *database, const QList<DrugInteractor *> &interactors, const QList<DrugDrugInteraction *> &ddis);
    bool saveBibliographicReferences(DrugsDB::Internal::DrugBaseEssentials *database);


private:
    Internal::DrugDrugInteractionCorePrivate *d;
};

}  // End namespace IAMDb

#endif // DRUGDRUGINTERACTIONCORE_H
