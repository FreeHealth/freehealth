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
 * \class Patients::PatientCore
 * \brief Core object of the patientsbaseplugin (namespace Patients).
 */

#include "patientcore.h"
#include "patientbase.h"
#include "patientmodelwrapper.h"
#include "patientmodel.h"

#include <coreplugin/icore.h>

#include <translationutils/constants.h>

#include <QDebug>

using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Patients {

PatientCore *PatientCore::_instance = 0;

namespace Internal {
/*!
 * \class Patients::Internal::PatientCorePrivate
 * \brief Private implementation of the Patients::PatientCore class.
 *
 * documentation here
 */
class PatientCorePrivate
{
public:
    PatientCorePrivate(PatientCore *parent) :
        _base(0),
        _patientModelWrapper(0),
        _basicSqlPatientModel(0),
        q(parent)
    {
    }
    
    ~PatientCorePrivate()
    {
    }
    
public:
    PatientBase *_base;
    PatientModelWrapper *_patientModelWrapper;
    BasicSqlPatientModel *_basicSqlPatientModel;

private:
    PatientCore *q;
};
}  // namespace Internal
} // end namespace Patients

/*! Constructor of the Patients::PatientCore class */
PatientCore::PatientCore(QObject *parent) :
    QObject(parent),
    d(new PatientCorePrivate(this))
{
    _instance = this;
    setObjectName("PatientCore");
    d->_base = new PatientBase(this);

    // Create IPatient
    d->_patientModelWrapper = new Internal::PatientModelWrapper(this);
    Core::ICore::instance()->setPatient(d->_patientModelWrapper);
}

/*! Destructor of the Patients::PatientCore class */
PatientCore::~PatientCore()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PatientCore::initialize()
{
    if (!d->_base->initialize())
        return false;

    // create singleton model
    PatientModel *model = new PatientModel(this);
    PatientModel::setActiveModel(model);
    d->_patientModelWrapper->initialize(model);

    return true;
}

Internal::PatientBase *PatientCore::patientBase() const
{
    return d->_base;
}

Internal::BasicSqlPatientModel *PatientCore::basicSqlPatientModel() const
{
    return d->_basicSqlPatientModel;
}
