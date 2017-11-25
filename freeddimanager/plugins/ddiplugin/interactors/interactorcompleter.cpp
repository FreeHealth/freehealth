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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DrugsDB::Internal::InteractorCompleter
 * QCompleter configured with the interacting molecules.
 * \n FreeToolBox specific class.
 */

#include "interactorcompleter.h"
#include "drugdruginteractioncore.h"
#include "druginteractor.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugDrugInteractionCore *ddiCore() {return DrugsDB::DrugDrugInteractionCore::instance();}

namespace DrugsDB {
namespace Internal {
class MoleculesInteractorCompleterPrivate
{
public:
    MoleculesInteractorCompleterPrivate(MoleculesInteractorCompleter *parent) :
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~MoleculesInteractorCompleterPrivate()
    {
    }

private:
    MoleculesInteractorCompleter *q;
};

class ClassInteractorCompleterPrivate
{
public:
    ClassInteractorCompleterPrivate(ClassInteractorCompleter *parent) :
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~ClassInteractorCompleterPrivate()
    {
    }

private:
    ClassInteractorCompleter *q;
};

} // namespace Internal
} // namespace DrugsDB

/*! Constructor of the DrugsDB::Internal::InteractorCompleter class */
MoleculesInteractorCompleter::MoleculesInteractorCompleter(QObject *parent) :
    QCompleter(parent),
    d(new MoleculesInteractorCompleterPrivate(this))
{
}

/*! Destructor of the DrugsDB::Internal::InteractorCompleter class */
MoleculesInteractorCompleter::~MoleculesInteractorCompleter()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool MoleculesInteractorCompleter::initialize()
{
    setModel(ddiCore()->interactingMoleculesModel());
    return true;
}

/*! Constructor of the DrugsDB::Internal::InteractorCompleter class */
ClassInteractorCompleter::ClassInteractorCompleter(QObject *parent) :
    QCompleter(parent),
    d(new ClassInteractorCompleterPrivate(this))
{
}

/*! Destructor of the DrugsDB::Internal::InteractorCompleter class */
ClassInteractorCompleter::~ClassInteractorCompleter()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ClassInteractorCompleter::initialize()
{
    setModel(ddiCore()->interactingMoleculesModel());
    return true;
}

