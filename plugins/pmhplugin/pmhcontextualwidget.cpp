
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

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

namespace PMH {
namespace Internal {

class PmhContext : public Core::IContext
{
public:
    PmhContext(PmhContextualWidget *w) : Core::IContext(w)
    {
        setObjectName("PmhContext");
        setWidget(w);
    }
};

}  // End namespace Internal
}  // End namespace PMH

PmhContextualWidget::PmhContextualWidget(QWidget *parent) :
    QWidget(parent), m_Context(0)
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
