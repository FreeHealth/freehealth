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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DataPackPlugin::Internal::ServerCreationPage
 * Datapack pack and server creation page.
 * Allow user to create/update datapack server by-hand.
 * \sa DataPackPlugin::DataPackCore
 */

#include "servercreationpage.h"
#include "servercreationwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <extensionsystem/pluginmanager.h>

#include <QTimer>

#include <QDebug>

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

namespace DataPackPlugin {
namespace Internal {
class ServerCreationPagePrivate
{
public:
    ServerCreationPagePrivate(ServerCreationPage *parent) :
        _step(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~ServerCreationPagePrivate()
    {
    }

public:
    ServerCreationStep *_step;

private:
    ServerCreationPage *q;
};
} // namespace Internal
} // namespace DataPackPlugin

/*!
 * Constructor of the DataPackPlugin::Internal::ServerCreationPage class.
 * This object creates, owns and register the DataPackPlugin::Internal::ServerCreationStep
 * to the plugin manager object pool.
 */
ServerCreationPage::ServerCreationPage(QObject *parent) :
    Core::IToolPage(parent),
    d(new ServerCreationPagePrivate(this))
{
    d->_step = new ServerCreationStep(this);
    pluginManager()->addObject(d->_step);
}

/*!
 * Destructor of the DataPackPlugin::Internal::ServerCreationPage class.
 * Remove the step from the plugin manager.
 */
ServerCreationPage::~ServerCreationPage()
{
    pluginManager()->removeObject(d->_step);
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ServerCreationPage::initialize()
{
    return true;
}

QString ServerCreationPage::name() const
{
    return tkTr(Trans::Constants::SERVER);
}

QString ServerCreationPage::category() const
{
    return tkTr(Trans::Constants::DATAPACK);
}

QIcon ServerCreationPage::icon() const
{
    return theme()->icon(Core::Constants::ICON_PACKAGE);
}

QWidget *ServerCreationPage::createPage(QWidget *parent)
{
    DataPackWidget *w = new DataPackWidget(parent);
    w->initialize();
    return w;
}

/** Datapack pack and server creation step CTor. */
ServerCreationStep::ServerCreationStep(QObject *parent) :
    Core::IFullReleaseStep(parent)
{
    setObjectName("ServerCreationStep");
}

ServerCreationStep::~ServerCreationStep()
{
}

bool ServerCreationStep::createTemporaryStorage()
{
    return true;
}

bool ServerCreationStep::cleanTemporaryStorage()
{
    return true;
}

bool ServerCreationStep::startProcessing(ProcessTiming timing, SubProcess subProcess)
{
    _currentTiming = timing;
    _currentSubProcess = subProcess;
    QTimer::singleShot(10, this, SLOT(onSubProcessFinished()));
    return true;
}

void ServerCreationStep::onSubProcessFinished()
{
    Q_EMIT subProcessFinished(_currentTiming, _currentSubProcess);
}

