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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Patients::PatientsCore
 * \brief Core object of the patientsbaseplugin (namespace Patients).
 */

#include "patientscore.h"
#include "patientbase.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Patients {

PatientsCore *PatientsCore::_instance = 0;

namespace Internal {
/*!
 * \class Patients::Internal::PatientsCorePrivate
 * \brief Private implementation of the Patients::PatientsCore class.
 *
 * documentation here
 */
class PatientsCorePrivate
{
public:
    PatientsCorePrivate(PatientsCore *parent) :
        _base(0),
        q(parent)
    {
    }
    
    ~PatientsCorePrivate()
    {
    }
    
public:
    PatientBase *_base;

private:
    PatientsCore *q;
};
}  // namespace Internal
} // end namespace Patients

/*! Constructor of the Patients::PatientsCore class */
PatientsCore::PatientsCore(QObject *parent) :
    QObject(parent),
    d(new PatientsCorePrivate(this))
{
    _instance = this;
    setObjectName("PatientsCore");
    d->_base = new PatientBase(this);
}

/*! Destructor of the Patients::PatientsCore class */
PatientsCore::~PatientsCore()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PatientsCore::initialize()
{
    if (!d->_base->initialize())
        return false;
    return true;
}

Internal::PatientBase *PatientsCore::patientBase() const
{
}

PatientModel *PatientsCore::patientSearchModel() const
{
}
