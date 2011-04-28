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
#ifndef PIMENGINE_H
#define PIMENGINE_H

#include <drugsbaseplugin/idrugengine.h>

/**
 * \file pimengine.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 24 Apr 2011
*/

namespace DrugsDB {
namespace Internal {
class PimEnginePrivate;

class PimEngine : public IDrugEngine
{
    Q_OBJECT
public:
//    enum TypeOfIAM
//    {
//        NoIAM            = 0x0000,
//        Unknown          = 0x0001,
//        Information      = 0x0002,
//        InnDuplication   = 0x0004,
//        ClassDuplication = 0x0008,
//        Precaution       = 0x0010,
//        APrendreEnCompte = 0x0020,
//        P450             = 0x0100,
//        GPG              = 0x0200,
//        Deconseille      = 0x1000,
//        ContreIndication = 0x8000
//    };
//    Q_DECLARE_FLAGS(TypesOfIAM, TypeOfIAM);

    explicit PimEngine(QObject *parent = 0);
    ~PimEngine();

    bool init();
    bool canComputeInteractions() const {return true;}

    bool isCalculatingDrugDrugInteractions() const {return false;}
    bool isCalculatingPatientDrugInteractions() const {return true;}

    QString name() const;
    QString shortName() const;
    QString uid() const;
    QIcon icon(const int size = 0) const;
    QString iconFullPath(const int size = 0) const;

    int calculateInteractions(const QVector<IDrug *> &drugs);
    QVector<IDrugInteraction *> getAllInteractionsFound();
    QVector<IDrugInteractionAlert *> getAllAlerts(DrugInteractionResult *addToResult);

    // Private hidden part
    bool checkDrugInteraction(IDrug *drug, const QVector<IDrug *> &drugsList);
    QVector<IDrugInteraction *> getInteractionsFromDatabase(const int & _id1, const int & _id2);

private:
    PimEnginePrivate *d;
};

}  // End namespace Internal
}  // End namespace DrugsDB

//Q_DECLARE_OPERATORS_FOR_FLAGS(DrugsDB::Internal::DrugDrugInteractionEngine::TypesOfIAM)


#endif // PIMENGINE_H
