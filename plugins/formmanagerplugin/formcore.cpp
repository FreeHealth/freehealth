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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Form::FormCore
 * \brief This object is the single Core of the Form plugin.
 * You can access to any unique object of the Form plugin here:
 * - the Form::FormManager: manages forms
 * - the Form::EpisodeManager: manages episodes (content of a form)
 * And some useful functions like activateCentralPatientFileMode().
 *
 * \sa Form::FormManager, Form::EpisodeManager
 */

#include "formcore.h"
#include "formmanager.h"
#include "formexporter.h"
#include "episodemanager.h"
#include "patientformitemdatawrapper.h"
#include "formcontextualwidgetmanager.h"
#include "episodebase.h"

#include <coreplugin/icore.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/constants_menus.h>

#include <translationutils/constants.h>
#include <extensionsystem/pluginmanager.h>

#include <QDebug>

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

namespace Form {
namespace Internal {
class FormCorePrivate
{
public:
    FormCorePrivate(FormCore *parent) :
        _initialized(false),
        _formManager(0),
        _episodeManager(0),
        _widgetManager(0),
        _patientFormItemDataWrapper(0),
        _formExporter(0),
        _episodeBase(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~FormCorePrivate()
    {
    }

public:
    bool _initialized;
    FormManager *_formManager;
    EpisodeManager *_episodeManager;
    FormContextualWidgetManager *_widgetManager;
    PatientFormItemDataWrapper *_patientFormItemDataWrapper;
    FormExporter *_formExporter, *_identitytFormExporter;
    EpisodeBase *_episodeBase;

private:
    FormCore *q;
};
}  // namespace Internal
} // end namespace Form

Form::FormCore *Form::FormCore::_instance = 0;

Form::FormCore &Form::FormCore::instance() // static
{
    Q_ASSERT(_instance);
    return *_instance;
}

/*! Constructor of the FormCore class */
FormCore::FormCore(QObject *parent) :
    QObject(parent),
    d(new FormCorePrivate(this))
{
    _instance = this;
    d->_formManager = new FormManager(this);
    d->_episodeManager = new EpisodeManager(this);
    d->_patientFormItemDataWrapper = new PatientFormItemDataWrapper(this);
    d->_formExporter = new FormExporter(false, this);
    d->_identitytFormExporter = new FormExporter(true, this);
    d->_episodeBase = new EpisodeBase(this);
}

/*! Destructor of the FormCore class */
FormCore::~FormCore()
{
    pluginManager()->removeObject(d->_formExporter);
    pluginManager()->removeObject(d->_identitytFormExporter);
    _instance = 0;
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FormCore::initialize()
{
    if (d->_initialized)
        return true;
    d->_formManager->initialize();
    d->_episodeManager->initialize();
    d->_widgetManager = new Internal::FormContextualWidgetManager(this);
    d->_patientFormItemDataWrapper->initialize();
    d->_formExporter->initialize();
    d->_identitytFormExporter->initialize();
    pluginManager()->addObject(d->_formExporter);
    pluginManager()->addObject(d->_identitytFormExporter);
    d->_initialized = true;
    return true;
}

/** Returns \e true if the core is initialized (with or without error) */
bool FormCore::isInitialized() const
{
    return d->_initialized;
}

/** Return the unique instance of the Form::FormManager */
Form::FormManager &FormCore::formManager() const
{
    return *d->_formManager;
}

/** Return the unique instance of the Form::EpisodeManager */
Form::EpisodeManager &FormCore::episodeManager() const
{
    return *d->_episodeManager;
}

/** Return the unique instance of the Form::PatientFormItemDataWrapper */
Form::PatientFormItemDataWrapper &FormCore::patientFormItemDataWrapper() const
{
    return *d->_patientFormItemDataWrapper;
}

void FormCore::activatePatientFileCentralMode()
{
    modeManager()->activateMode(Core::Constants::MODE_PATIENT_FILE);
}
