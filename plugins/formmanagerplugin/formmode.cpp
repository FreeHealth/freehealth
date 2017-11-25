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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "formmode.h"

#include <formmanagerplugin/formplaceholder.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <extensionsystem/pluginmanager.h>

#include <QDebug>

using namespace Form;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Form::FormManager *formManager() {return Form::FormManager::instance();}

FormMode::FormMode(FormItemSpec *spec, QObject *parent) :
    Core::BaseMode(parent),
    _placeHolder(0),
    _inPool(false)
{
    Q_ASSERT(spec);
    setObjectName("Form::FormMode::" + _spec->uuid());

    _placeHolder = new Form::FormPlaceHolder;
    _placeHolder->setObjectName("BaseWidget::Mode::FormPlaceHolder" + _spec->uuid());

    setUniqueModeName(_spec->uuid().toUtf8());
    setPatientBarVisibility(true);
    setPriority(Core::Constants::P_MODE_PATIENT_FILE);
    retranslate();

    connect(formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(getPatientForm()));
    qWarning() << "FormMode" << _placeHolder->objectName();
}

FormMode::~FormMode()
{
    if (_inPool)
        pluginManager()->removeObject(this);
}

void FormMode::getPatientForm()
{
//    qWarning() << Q_FUNC_INFO;
    Form::FormMain *root = formManager()->rootForm(_spec->uuid().toUtf8());
    if (!root) {
        if (_inPool)
            pluginManager()->removeObject(this);
        _inPool = false;
    } else {
        if (!_inPool)
            pluginManager()->addObject(this);
        _inPool = true;
    }
    _placeHolder->setRootForm(root);
}

void FormMode::retranslate()
{
    setName(_spec->label());
    QString icon = _spec->iconFileName();
    icon.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::BigPixmapPath));
    setIcon(QIcon(icon));
    // TODO: move the extradata inside the spec to have a fully translated extra-values
    setPriority(_spec->value(Form::FormItemSpec::Spec_Priority, "100").toInt());
}

