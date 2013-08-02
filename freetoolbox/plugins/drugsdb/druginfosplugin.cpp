/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "druginfosplugin.h"
#include "biamextractor.h"
#include "pimintegrator.h"
#include "pimdatabasecreator.h"
#include "pregnancyclassification.h"

#include <coreplugin/dialogs/pluginaboutpage.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace DrugInfos;
using namespace Internal;

DrugInfosPlugin::DrugInfosPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "Creating DrugInfosPlugin";
}

DrugInfosPlugin::~DrugInfosPlugin()
{
    qWarning() << "DrugInfosPlugin::~DrugInfosPlugin()";
}

bool DrugInfosPlugin::initialize(const QStringList &/*arguments*/, QString */*errorMessage*/)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugInfosPlugin::initialize";

    addAutoReleasedObject(new BiamExtractorPage(this));
    addAutoReleasedObject(new PimsTreePage(this));
    addAutoReleasedObject(new PimDatabasePage(this));
    addAutoReleasedObject(new PregnancyClassificationPage(this));

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    return true;
}

void DrugInfosPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugInfosPlugin::extensionsInitialized";
}

Q_EXPORT_PLUGIN(DrugInfosPlugin)
