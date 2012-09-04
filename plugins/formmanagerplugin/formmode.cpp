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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "modewidget.h"

#include <formmanagerplugin/formplaceholder.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemspec.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/modemanager/basemode.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <extensionsystem/pluginmanager.h>

#include <QDebug>

using namespace BaseWidgets;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Form::FormManager *formManager() {return Form::FormManager::instance();}

ModeWidget::ModeWidget(Form::FormItem *item, QWidget *parent) :
    Form::IFormWidget(item, parent),
    _formItem(item),
    _mode(0),
    _placeHolder(0),
    _inPool(false)
{
    Q_ASSERT(item);
    setObjectName("BaseWidget::Mode::" + _formItem->uuid());

    _placeHolder = new Form::FormPlaceHolder;
    _placeHolder->setObjectName("BaseWidget::Mode::FormPlaceHolder" + _formItem->uuid());

    _mode = new Core::BaseMode(this);
    _mode->setUniqueModeName(_formItem->uuid().toUtf8());
    _mode->setPatientBarVisibility(true);

    qWarning() << "ModeWidget" << _placeHolder->objectName();

    retranslate();

    connect(formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(getPatientForm()));
}

ModeWidget::~ModeWidget()
{
    if (_inPool)
        pluginManager()->removeObject(_mode);
}

void ModeWidget::addWidgetToContainer(Form::IFormWidget *widget)
{
    Q_UNUSED(widget);
}

bool ModeWidget::isContainer() const
{
    return false;
}

QString ModeWidget::printableHtml(bool withValues) const
{
    Q_UNUSED(withValues);
    return QString::null;
}

void ModeWidget::getPatientForm()
{
//    qWarning() << Q_FUNC_INFO;
    Form::FormMain *root = formManager()->rootForm(_formItem->uuid().toUtf8());
    if (!root) {
        if (_inPool)
            pluginManager()->removeObject(_mode);
        _inPool = false;
    } else {
        if (!_inPool)
            pluginManager()->addObject(_mode);
        _inPool = true;
    }
    _placeHolder->setRootForm(root);
}

void ModeWidget::retranslate()
{
    _mode->setName(_formItem->spec()->label());
    QString icon = _formItem->spec()->iconFileName();
    icon.replace(Core::Constants::TAG_APPLICATION_THEME_PATH, settings()->path(Core::ISettings::BigPixmapPath));
    _mode->setIcon(QIcon(icon));
    // TODO: move the extradata inside the spec to have a fully translated extra-values
    _mode->setPriority(_formItem->extraData().value("priority", "100").toInt());
}

