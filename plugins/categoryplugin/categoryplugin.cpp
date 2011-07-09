/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \namespace Category
  \brief Contains category manager for all applications of the FreeMedForms project.
  The Category plugin owns:
  - interface classes for implementation of category in any plugin
  - its database
  - some models for your views
  - dialogs for edition/creation of categories
*/

#include "categoryplugin.h"
#include "categorycore.h"

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Category;

CategoryPlugin::CategoryPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating CategoryPlugin";
}

CategoryPlugin::~CategoryPlugin()
{
}

bool CategoryPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "CategoryPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    return true;
}

void CategoryPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "CategoryPlugin::extensionsInitialized";
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Create the core instance
    CategoryCore::instance(this);
}


Q_EXPORT_PLUGIN(CategoryPlugin)
