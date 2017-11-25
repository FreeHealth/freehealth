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
   \class MainWin::Internal::UserModelWrapper
   Fake wrapper to the Patient::UserModel Just coded for test.
*/

#include "usermodelwrapper.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>

#include <patientbaseplugin/patientmodel.h>

static inline Form::FormManager *formManager() {return Form::FormManager::instance();}
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }

using namespace MainWin::Internal;

UserModelWrapper::UserModelWrapper(QObject *parent) :
        Core::IUser(parent)
{
//    connect(model, SIGNAL(patientChanged(QString)), this, SLOT(onCurrentPatientChanged(QString)));
//    connect(model, SIGNAL(patientCreated(QString)), this, SIGNAL(patientCreated(QString)));
}

UserModelWrapper::~UserModelWrapper()
{
    // as this object was created in Mainwindowplugin class with Core::ICore::instance as parent
    // nothing to do
}

void UserModelWrapper::init()
{
}

QVariant UserModelWrapper::value(int column) const
{
    return column;
}

bool UserModelWrapper::setValue(int ref, const QVariant &value)
{
    return true;
}

