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
 ***************************************************************************/
/**
 \class ICD::IcdContextualWidget
 \brief Simplify the creation of contextualized widget for the ICD plugin.
 Contextualized contexts have access to all actions that are added in the particular context of
 ICD (ICD::Constants::C_ICD). You can add contexts to the widget by using the addContexts() member.\n
 The ICD::IcdContextualWidget are automatically removed from the context manager (no special code needed).
*/

#include "icdcontextualwidget.h"
#include "icdwidgetmanager.h"
#include "constants.h"

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/uniqueidmanager.h>

using namespace ICD;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }


namespace ICD {
namespace Internal {

class IcdContext : public Core::IContext
{
public:
    IcdContext(IcdContextualWidget *w) : Core::IContext(w), wgt(w) { setObjectName("IcdContext"); }
    void setContext(QList<int> c) { ctx = c; }
    void addContext(QList<int> c) { ctx.append(c); }
    QList<int> context() const { return ctx; }
    QWidget *widget() { return wgt; }

private:
    IcdContextualWidget *wgt;
    QList<int> ctx;
};

}  // End namespace Internal
}  // End namespace ICD



IcdContextualWidget::IcdContextualWidget(QWidget *parent) :
    QWidget(parent), m_Context(0)
{
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();

    // Create the context object
    m_Context = new Internal::IcdContext(this);
    m_Context->setContext(QList<int>() << uid->uniqueIdentifier(Constants::C_ICD_PLUGINS));

    // Send it to the contextual manager
    contextManager()->addContextObject(m_Context);
}

IcdContextualWidget::~IcdContextualWidget()
{
    // Remove contextual object
    contextManager()->removeContextObject(m_Context);
}

void IcdContextualWidget::addContexts(const QList<int> &contexts)
{
    m_Context->addContext(contexts);
}
