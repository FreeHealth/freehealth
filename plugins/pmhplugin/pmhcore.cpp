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
 *       Jerome Pinguet <jerome@jerome.cc                                  *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pmhcore.h"
#include "pmhcategorymodel.h"
#include "pmhwidgetmanager.h"
#include "pmhcontentexporter.h"
#include "pmhtokens.h"

#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/formmanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/ipadtools.h>

#include <utils/log.h>
#include <extensionsystem/pluginmanager.h>

#include <QApplication>

using namespace PMH;
using namespace Internal;

static inline Core::IPadTools *padTools() {return Core::ICore::instance()->padTools();}
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

class ModelTest;

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
#ifdef WITH_PAD
        foreach(PmhTokens *tok, m_Tokens)
            padTools()->tokenPool()->removeToken(tok);
        qDeleteAll(m_Tokens);
        m_Tokens.clear();
#endif
    }

public:
    PmhCategoryModel *m_PmhCategoryModel;
    PmhWidgetManager *m_PmhWidgetManager;
    PmhContentExporter *m_Exporter;
    QList<PmhTokens *> m_Tokens;
};

} // namespace Internal
} // namespace PMH

PmhCore::PmhCore(QObject *parent) :
    QObject(parent), d(new PmhCorePrivate)
{
    setObjectName("PmhCore");
    d->m_PmhCategoryModel = new PmhCategoryModel(this);
    d->m_PmhWidgetManager = new PmhWidgetManager(this);
    d->m_Exporter = new PmhContentExporter(this);
    d->m_Exporter->initialize();
    pluginManager()->addObject(d->m_Exporter);
    connect(&formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(onPatientFormsLoaded()));

#ifdef WITH_PAD
    LOG("Creating PMHx tokens");
    PmhTokens *tok = new PmhTokens;
    tok->setOutputType(PmhTokens::HtmlOutput);
    tok->initialize(d->m_PmhCategoryModel);
    d->m_Tokens << tok;
    padTools()->tokenPool()->addToken(tok);

    tok = new PmhTokens;
    tok->setOutputType(PmhTokens::PlainTextOutput);
    tok->initialize(d->m_PmhCategoryModel);
    d->m_Tokens << tok;
    padTools()->tokenPool()->addToken(tok);
#endif
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
