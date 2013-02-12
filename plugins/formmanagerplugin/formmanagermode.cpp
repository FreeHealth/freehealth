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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "formmanagermode.h"
#include "formcore.h"
#include "formplaceholder.h"
#include "iformitem.h"
#include "iformio.h"
#include "formmanager.h"
#include "constants_db.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <extensionsystem/pluginmanager.h>

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ModeManager *modeManager()  { return Core::ICore::instance()->modeManager(); }
static inline Core::ActionManager *actionManager()  { return Core::ICore::instance()->actionManager(); }
static inline Core::IMainWindow *mainWindow()  { return Core::ICore::instance()->mainWindow(); }

/**
 * \class Form::Internal::FormManagerMode
 * Mode for the central form files (called "Patient files").
*/
FormManagerMode::FormManagerMode(QObject *parent) :
    Core::IMode(parent),
    m_inPluginManager(false),
    m_actionInBar(false)
{
    setDisplayName(tkTr(Trans::Constants::PATIENT));
    setIcon(theme()->icon(Core::Constants::ICONPATIENTFILES, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_FILE);
    setId(Core::Constants::MODE_PATIENT_FILE);
    setPatientBarVisibility(true);
    setEnabledOnlyWithCurrentPatient(true);

    m_Holder = new FormPlaceHolder;
    m_Holder->setObjectName("EpisodesFormPlaceHolder");
//    m_Holder->installEventFilter(this);

//    const QList<int> &context;
//    setContext();
    setWidget(m_Holder);
    onPatientFormsLoaded();
    connect(&formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(onPatientFormsLoaded()));
}

FormManagerMode::~FormManagerMode()
{
    if (m_inPluginManager) {
        pluginManager()->removeObject(this);
    }
    // m_Holder is deleted by Core::IMode
}

QString FormManagerMode::name() const
{
    return tkTr(Trans::Constants::PATIENT);
}

/**
  \brief Get the patient form from the episode database, send the load signal with the form absPath and load it.
  \sa Core::ICore::loadPatientForms()
*/
bool FormManagerMode::onPatientFormsLoaded()
{
    if (!m_inPluginManager) {
        pluginManager()->addObject(this);
        m_inPluginManager = true;
    }
    Form::FormTreeModel *model = formManager().formTreeModelForMode(Core::Constants::MODE_PATIENT_FILE);
    m_Holder->setFormTreeModel(model);
    return (model);
}
