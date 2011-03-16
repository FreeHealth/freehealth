/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "drugsmode.h"

#include <formmanagerplugin/formplaceholder.h>
#include <formmanagerplugin/iformio.h>

#include <coreplugin/icore.h>
//#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

#include <extensionsystem/pluginmanager.h>

#include <QDebug>

using namespace DrugsWidget;
using namespace Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

DrugsMode::DrugsMode(QObject *parent) :
        Core::BaseMode(parent),
        m_Holder(0)
{
    m_Holder = new Form::FormPlaceHolder;
    setName(tr("Drugs"));
    setIcon(theme()->icon(Core::Constants::ICONDRUGMODE, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_DRUGS);
    setUniqueModeName(Core::Constants::MODE_PATIENT_DRUGS);
//    const QList<int> &context;
//    setContext();
    setWidget(m_Holder);

    connect(Core::ICore::instance(), SIGNAL(loadPatientForms(QString)), this, SLOT(loadPatientFile(QString)));
}

DrugsMode::~DrugsMode()
{
}

void DrugsMode::loadPatientFile(const QString &absDirPath)
{
    qWarning() << Q_FUNC_INFO << absDirPath;
    if (absDirPath.isEmpty())
        return;

    // get all form readers (IFormIO)
    Form::IFormIO *reader = 0;
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();

    // try to read form
    Form::FormMain *root = 0;
    foreach(Form::IFormIO *io, list) {
        if (io->setFileName(absDirPath + "/drugs.xml") && io->canReadFile()) {
            root = io->loadForm();
            if (root)
                break;
        }
    }

    // If nothing was read -> remove or inactivate the mode

    // Tell the PlaceHolder of the FormMain to use as root item
    // FormPlaceHolder will manage deletion of the item
    m_Holder->setRootForm(root);
}
