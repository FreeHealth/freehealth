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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
   \class MainWin::Internal::PatientModelWrapper
   Fake wrapper to the Patient::PatientModel Just coded for test.
*/

#include "patientmodelwrapper.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>

#include <patientbaseplugin/patientmodel.h>

static inline Form::FormManager *formManager() {return Form::FormManager::instance();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }

using namespace MainWin::Internal;

PatientModelWrapper::PatientModelWrapper(QObject *parent) :
        Core::IPatient(parent)
{
//    connect(model, SIGNAL(patientChanged(QString)), this, SLOT(onCurrentPatientChanged(QString)));
//    connect(model, SIGNAL(patientCreated(QString)), this, SIGNAL(patientCreated(QString)));
}

PatientModelWrapper::~PatientModelWrapper()
{
    // as this object was created in Mainwindowplugin class with Core::ICore::instance as parent
    // nothing to do
}

void PatientModelWrapper::onCurrentPatientChanged(const QString &)
{
    Q_EMIT currentPatientChanged();
}

void PatientModelWrapper::init()
{
}

QModelIndex PatientModelWrapper::currentPatientIndex() const
{
    return index(0,0);
}

QVariant PatientModelWrapper::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    return index.row();
}

QVariant PatientModelWrapper::data(int column) const
{
    QModelIndex idx = index(currentPatientIndex().row(), column);
    return this->data(idx);
}

bool PatientModelWrapper::setValue(int ref, const QVariant &value)
{
    QModelIndex idx = index(currentPatientIndex().row(), ref);
    return setData(idx, value);
}

bool PatientModelWrapper::setData(const QModelIndex &item, const QVariant &value, int role)
{
    return true;
}

Core::IPatientBar * PatientModelWrapper::patientBar() const
{
    return 0;
}

void PatientModelWrapper::hidePatientBar()
{
}

void PatientModelWrapper::showPatientBar()
{
}

bool PatientModelWrapper::isPatientBarVisible() const
{
    return true;
}

void PatientModelWrapper::patientDataChanged(const QModelIndex &index)
{
}
