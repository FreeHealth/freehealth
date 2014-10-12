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
 *  Main Developers:                                                       *
 *       Eric Maeker <eric.maeker@gmail.com>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class Guard::Internal::GuardLineView
 * \brief Contextualized widget for the Core plugin.
 *
 * This contextualized widget has access to all actions that are added in the particular context of
 * Core (e.g. Core::Constants::C_Core).\n
 * You can add context identificators to the widget by using the addContexts() method.
 * The context object is automatically removed from the context manager in its destructor
 * (no special code needed).
 * \sa Core::IContext, Core::ContextManager, Core::ICore::contextManager()
 */

#include "guardlineview.h"
#include "constants.h"

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/icore.h>

using namespace Guard;
using namespace Internal;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

GuardLineView::GuardLineView(QWidget *parent) :
    QWidget(parent),
    m_Context(0)
{
//    Core::Context context(Constants::C_CORE_PLUGINS);
//    // Create the context object
//    m_Context = new Internal::CoreContext(this);
//    m_Context->setContext(context);

//    // Send it to the contextual manager
//    contextManager()->addContextObject(m_Context);
}

GuardLineView::~GuardLineView()
{
//    // Remove contextual object
//    contextManager()->removeContextObject(m_Context);
}

void GuardLineView::addContexts(const QList<int> &contexts)
{
//    m_Context->addContext(contexts);
}

