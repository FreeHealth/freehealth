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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DataPackPlugin::Internal::DataPackPage
 * Datapack pack and server creation page.
 * Allow user to create/update datapack server by-hand.
 * \sa DataPackPlugin::DataPackCore
 */

#include "datapackpage.h"
#include "datapackwidget.h"

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
class DataPackPagePrivate
{
public:
    DataPackPagePrivate(DataPackPage *parent) :
        _step(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~DataPackPagePrivate()
    {
    }

public:
    DataPackStep *_step;

private:
    DataPackPage *q;
};
} // namespace Internal
} // namespace DataPackPlugin

/*!
 * Constructor of the DataPackPlugin::Internal::DataPackPage class.
 * This object creates, owns and register the DataPackPlugin::Internal::DataPackStep
 * to the plugin manager object pool.
 */
DataPackPage::DataPackPage(QObject *parent) :
    Core::IToolPage(parent),
    d(new DataPackPagePrivate(this))
{
    d->_step = new DataPackStep(this);
    pluginManager()->addObject(d->_step);
}

/*!
 * Destructor of the DataPackPlugin::Internal::DataPackPage class.
 * Remove the step from the plugin manager.
 */
DataPackPage::~DataPackPage()
{
    pluginManager()->removeObject(d->_step);
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DataPackPage::initialize()
{
    return true;
}

QString DataPackPage::name() const
{
    return tkTr(Trans::Constants::DATAPACK);
}

QString DataPackPage::category() const
{
    return tkTr(Trans::Constants::DATAPACK);
}

QIcon DataPackPage::icon() const
{
    return theme()->icon(Core::Constants::ICON_PACKAGE);
}

QWidget *DataPackPage::createPage(QWidget *parent)
{
    DataPackWidget *w = new DataPackWidget(parent);
    w->initialize();
    return w;
}

/** Datapack pack and server creation step CTor. */
DataPackStep::DataPackStep(QObject *parent) :
    Core::IFullReleaseStep(parent)
{
    setObjectName("DataPackStep");
}

DataPackStep::~DataPackStep()
{
}

bool DataPackStep::createTemporaryStorage()
{
    return true;
}

bool DataPackStep::cleanTemporaryStorage()
{
    return true;
}

bool DataPackStep::startProcessing(ProcessTiming timing, SubProcess subProcess)
{
    _currentTiming = timing;
    _currentSubProcess = subProcess;
    QTimer::singleShot(10, this, SLOT(onSubProcessFinished()));
    return true;
}

void DataPackStep::onSubProcessFinished()
{
    Q_EMIT subProcessFinished(_currentTiming, _currentSubProcess);
}

