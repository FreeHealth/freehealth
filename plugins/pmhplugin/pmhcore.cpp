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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pmhcore.h"
#include "pmhcategorymodel.h"
#include "pmhwidgetmanager.h"
#include "pmhcontentexporter.h"

#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/formmanager.h>

#include <extensionsystem/pluginmanager.h>

#include <QApplication>

using namespace PMH;
using namespace Internal;

static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

PmhCore *PmhCore::m_Instance = 0;

PmhCore *PmhCore::instance(QObject *parent)
{
    if (!m_Instance) {
        if (!parent)
            m_Instance = new PmhCore(qApp);
        else
            m_Instance = new PmhCore(parent);
    }
    return m_Instance;
}

namespace PMH {
namespace Internal {
class PmhCorePrivate
{
public:
    PmhCorePrivate() :
        m_PmhCategoryModel(0),
        m_PmhWidgetManager(0),
        m_Exporter(0)
    {
    }

    ~PmhCorePrivate()
    {
        if (m_PmhCategoryModel)
            delete m_PmhCategoryModel;
        m_PmhCategoryModel = 0;
    }

public:
    PmhCategoryModel *m_PmhCategoryModel;
    PmhWidgetManager *m_PmhWidgetManager;
    PmhContentExporter *m_Exporter;
};

} // namespace Internal
} // namespace PMH

PmhCore::PmhCore(QObject *parent) :
    QObject(parent), d(new PmhCorePrivate)
{
    d->m_PmhCategoryModel = new PmhCategoryModel(this);
    d->m_PmhWidgetManager = new PmhWidgetManager(this);
    d->m_Exporter = new PmhContentExporter(this);
    d->m_Exporter->initialize();
    pluginManager()->addObject(d->m_Exporter);
    connect(&formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(onPatientFormsLoaded()));
}

PmhCore::~PmhCore()
{
    pluginManager()->removeObject(d->m_Exporter);
    if (d)
        delete d;
    d = 0;
}

PmhCategoryModel *PmhCore::pmhCategoryModel() const
{
    return d->m_PmhCategoryModel;
}

void PmhCore::onPatientFormsLoaded()
{
    d->m_PmhCategoryModel->setRootFormUid(formManager().centralFormUid());
    d->m_PmhCategoryModel->refreshFromDatabase();
}
