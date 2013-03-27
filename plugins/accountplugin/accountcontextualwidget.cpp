/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/

/**
 * \class Account::AccountContextualWidget
 * \brief Contextualized widget for the Account plugin.
 *
 * This contextualized widget has access to all actions that are added in the particular context of
 * Account (Account::Constants::C_ACCOUNT). You can add contexts to the widget by using the addContexts() member.\n
 * The Account::AccountContextualWidget are automatically removed from the context manager (no special code needed).
 */

#include "accountcontextualwidget.h"
#include "accountwidgetmanager.h"
#include "constants.h"

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QDebug>

using namespace Account;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }


namespace Account {
namespace Internal {

class AccountContext : public Core::IContext
{
public:
    AccountContext(AccountContextualWidget *w) :
        Core::IContext(w)
    {
        setObjectName("PmhContext");
        setWidget(w);
    }
};

}  // End namespace Internal
}  // End namespace Account

AccountContextualWidget::AccountContextualWidget(QWidget *parent) :
        QWidget(parent), m_Context(0)
{
    // Create the context object
    m_Context = new Internal::AccountContext(this);
    m_Context->setContext(Core::Context(Constants::C_ACCOUNT));

    // Send it to the contextual manager
    contextManager()->addContextObject(m_Context);
}

AccountContextualWidget::~AccountContextualWidget()
{
    // Remove contextual object
    qWarning() << "AccountContextualWidget::~AccountContextualWidget()" ;
    contextManager()->removeContextObject(m_Context);
}
