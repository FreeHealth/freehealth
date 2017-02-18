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
 *  Authors:                                                               *
 *  Eric MAEKER <eric.maeker@gmail.com>                                    *
 *  Jerome PINGUET <jerome@jerome.cc>                                      *
 ***************************************************************************/
/**
 \class PMH::PmhContextualWidget
 \brief Simplify the creation of contextualized widget for the PMH plugin.
 Contextualized contexts have access to all actions that are added in the particular context of
 PMH (PMH::Constants::C_PMH). You can add contexts to the widget by using the addContexts() member.\n
 The PMH::PmhContextualWidget are automatically removed from the context manager (no special code needed).
*/

#include "pmhcontextualwidget.h"
#include "constants.h"

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QDebug>

using namespace PMH;
using namespace Internal;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

PmhContextualWidget::PmhContextualWidget(QWidget *parent) :
    QWidget(parent),
    m_Context(0)
{
    m_Context = new Internal::PmhContext(this);
    m_Context->setContext(Core::Context(Constants::C_PMH_PLUGINS));

    // Send it to the contextual manager
    contextManager()->addContextObject(m_Context);
}

PmhContextualWidget::~PmhContextualWidget()
{
    // Remove contextual object
    contextManager()->removeContextObject(m_Context);
}

Core::IContext *PmhContextualWidget::context() const
{
    return m_Context;
}
