
/**
 \class Account::AccountContextualWidget
 \brief Simplify the creation of contextualized widget for the Account plugin.
 Contextualized contexts have access to all actions that are added in the particular context of
 Account (Account::Constants::C_ACCOUNT). You can add contexts to the widget by using the addContexts() member.\n
 The Account::AccountContextualWidget are automatically removed from the context manager (no special code needed).
*/

#include "accountcontextualwidget.h"
#include "accountwidgetmanager.h"
#include "constants.h"

#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/contextmanager/icontext.h>
#include <coreplugin/uniqueidmanager.h>

#include <QDebug>

using namespace Account;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }


namespace Account {
namespace Internal {

class AccountContext : public Core::IContext
{
public:
    AccountContext(AccountContextualWidget *w) : Core::IContext(w), wgt(w) { setObjectName("PmhContext"); }
    void setContext(QList<int> c) { ctx = c; }
    void addContext(QList<int> c) { ctx.append(c); }
    QList<int> context() const { return ctx; }
    QWidget *widget() { return wgt; }

private:
    AccountContextualWidget *wgt;
    QList<int> ctx;
};

}  // End namespace Internal
}  // End namespace Account



AccountContextualWidget::AccountContextualWidget(QWidget *parent) :
        QWidget(parent), m_Context(0)
{
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();

    // Create the context object
    m_Context = new Internal::AccountContext(this);
    m_Context->setContext(QList<int>() << uid->uniqueIdentifier(Constants::C_ACCOUNT));

    // Send it to the contextual manager
    contextManager()->addContextObject(m_Context);
}

AccountContextualWidget::~AccountContextualWidget()
{
    // Remove contextual object
    contextManager()->removeContextObject(m_Context);
}

void AccountContextualWidget::addContexts(const QList<int> &contexts)
{
    m_Context->addContext(contexts);
}

QList<int> AccountContextualWidget::contexts() const
{
    return m_Context->context();
}
