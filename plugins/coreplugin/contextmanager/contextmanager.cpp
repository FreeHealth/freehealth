#include "contextmanager_p.h"

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actionmanager_p.h>

#include <QApplication>
#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QMenuBar>
#include <QDebug>

using namespace Core;
using namespace Core::Internal;

enum { WarnContextUpdates=false };

ContextManagerPrivate::ContextManagerPrivate(QMainWindow *mainWin) :
        ContextManager(),
        m_activeContext(0)
{
    Q_ASSERT(mainWin);
    m_globalContext = QList<int>() << Constants::C_GLOBAL_ID;
    m_additionalContexts << Constants::C_GLOBAL_ID;
    m_mainWindow = mainWin;
    connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*,QWidget*)),
            this, SLOT(updateFocusWidget(QWidget*,QWidget*)));
}

IContext *ContextManagerPrivate::currentContextObject() const
{
    return m_activeContext;
}

IContext *ContextManagerPrivate::contextObject(QWidget *widget)
{
    return m_contextWidgets.value(widget);
}

void ContextManagerPrivate::addContextObject(IContext *context)
{
    if (!context)
        return;
    QWidget *widget = context->widget();
    if (m_contextWidgets.contains(widget))
        return;

    m_contextWidgets.insert(widget, context);
}

void ContextManagerPrivate::removeContextObject(IContext *context)
{
    if (!context)
        return;

    QWidget *widget = context->widget();
    if (!m_contextWidgets.contains(widget))
        return;

    m_contextWidgets.remove(widget);
    if (m_activeContext == context)
        updateContextObject(0);
}

void ContextManagerPrivate::updateFocusWidget(QWidget *old, QWidget *now)
{
    Q_UNUSED(old);

    if (WarnContextUpdates) {
        if (now)
            qWarning() << "ContextManager focusChanged" << now;
        else
            qWarning() << "ContextManager focusChanged to unsetted widget";
    }

    // Prevent changing the context object just because the menu is activated
    if (qobject_cast<QMenuBar*>(now))
        return;

    IContext *newContext = 0;
    QWidget *p = 0;
//    if (m_mainWindow->focusWidget()) {
    if (qApp->focusWidget()) {
        IContext *context = 0;
//        QWidget *p = m_mainWindow->focusWidget();
        p = qApp->focusWidget();
        while (p) {
            context = m_contextWidgets.value(p,0);
            if (context) {
                newContext = context;
                break;
            }
            p = p->parentWidget();
        }
    }
    if (newContext) {
        if (WarnContextUpdates)
            qWarning() << "ContextManager focused" << p << newContext->widget();
        updateContextObject(newContext);
    }
}

void ContextManagerPrivate::resetContext()
{
    updateContextObject(0);
}

void ContextManagerPrivate::addAdditionalContext(int context)
{
    if (context == 0)
        return;

    if (!m_additionalContexts.contains(context))
        m_additionalContexts.prepend(context);
}

void ContextManagerPrivate::removeAdditionalContext(int context)
{
    if (context == 0)
        return;

    int index = m_additionalContexts.indexOf(context);
    if (index != -1)
        m_additionalContexts.removeAt(index);
}

static ActionManagerPrivate *actionManagerPrivate()
{
    ActionManagerPrivate *amp = qobject_cast<ActionManagerPrivate*>(Core::ICore::instance()->actionManager());
    Q_ASSERT(amp);
    return amp;
}

bool ContextManagerPrivate::hasContext(int context) const
{
    return actionManagerPrivate()->hasContext(context);
}

void ContextManagerPrivate::updateContext()
{
    QList<int> contexts;

    if (m_activeContext)
        contexts += m_activeContext->context();

    contexts += m_additionalContexts;

    QList<int> uniquecontexts;
    for (int i = 0; i < contexts.size(); ++i) {
        const int c = contexts.at(i);
        if (!uniquecontexts.contains(c))
            uniquecontexts << c;
    }

    actionManagerPrivate()->setContext(uniquecontexts);
}

void ContextManagerPrivate::updateContextObject(IContext *context)
{
    if (!context)
        return;

    if (context == m_activeContext)
        return;

    IContext *oldContext = m_activeContext;
    m_activeContext = context;

    if (WarnContextUpdates) {
        if (context) {
            QString tmp = context->widget()->objectName();
            if (oldContext) {
                tmp += " " + oldContext->widget()->objectName();
            }
            qWarning() << "updateContextObject" << tmp;
        } else
            qWarning() << "updateContextObject 0 0";
    }


    if (!context || oldContext != m_activeContext) {
        emit contextAboutToChange(context);
        updateContext();
        if (WarnContextUpdates)
            qWarning() << "new context object =" << context << (context ? context->widget() : 0)
            << (context ? context->widget()->metaObject()->className() : 0);
        emit contextChanged(context);
    }
}
