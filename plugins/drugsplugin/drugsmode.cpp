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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugsmode.h"

#include <formmanagerplugin/formplaceholder.h>
#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/formmanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>
#include <extensionsystem/pluginmanager.h>

#include <QDebug>

using namespace DrugsWidget;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}

DrugsMode::DrugsMode(QObject *parent) :
        Core::IMode(parent),
        inPool(false),
        m_Holder(0)
{
    m_Holder = new Form::FormPlaceHolder;
    m_Holder->setObjectName("DrugsFormPlaceHolder");
    setDisplayName(tkTr(Trans::Constants::DRUGS));
    setIcon(theme()->icon(Core::Constants::ICONDRUGMODE, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_DRUGS);
    setId(Core::Constants::MODE_PATIENT_DRUGS);
    setPatientBarVisibility(true);
    setEnabledOnlyWithCurrentPatient(true);
    setWidget(m_Holder);
    onPatientFormsLoaded();
    connect(&formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(onPatientFormsLoaded()));
}

DrugsMode::~DrugsMode()
{
    if (inPool)
        pluginManager()->removeObject(this);
}

void DrugsMode::onPatientFormsLoaded()
{
    Form::FormTreeModel *model = formManager().formTreeModelForMode(Core::Constants::MODE_PATIENT_DRUGS);
    if (!model) {
        if (inPool)
            pluginManager()->removeObject(this);
        inPool = false;
    } else {
        if (!inPool)
            pluginManager()->addObject(this);
        inPool = true;
    }
    m_Holder->setFormTreeModel(model);
}
