#include "tkContextManager.h"

#include <tkConstants.h>
#include <tkUniqueIdentifier.h>
#include <tkActionManager.h>

#include <QApplication>
#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QMenuBar>
#include <QDebug>

class tkContextManagerPrivate
{
public:
    tkContextManagerPrivate() : m_activeContext(0), m_mainWindow(0)
    {
        m_additionalContexts << tkConstants::C_GLOBAL_ID;
    }

public:
    QList<int> m_globalContext;
    QList<int> m_additionalContexts;
    tkContext *m_activeContext;
    QMap<QWidget *, tkContext *> m_contextWidgets;
    QMainWindow *m_mainWindow;
};

tkContextManager *tkContextManager::m_Instance = 0;

tkContextManager *tkContextManager::instance(QMainWindow *mainWin)
{
    if (!m_Instance) {
        m_Instance = new tkContextManager(mainWin);
    }
    Q_ASSERT(m_Instance);
    return m_Instance;
}


tkContextManager::tkContextManager(QMainWindow *mainWin) : d(0)
{
    Q_ASSERT(mainWin);
    d = new tkContextManagerPrivate();
    d->m_mainWindow = mainWin;
    connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*,QWidget*)),
            this, SLOT(updateFocusWidget(QWidget*,QWidget*)));
}

tkContext *tkContextManager::currentContextObject() const
{
    return d->m_activeContext;
}

tkContext *tkContextManager::contextObject(QWidget *widget)
{
    return d->m_contextWidgets.value(widget);
}

void tkContextManager::addContextObject(tkContext *context)
{
    if (!context)
        return;
    QWidget *widget = context->widget();
    if (d->m_contextWidgets.contains(widget))
        return;

    d->m_contextWidgets.insert(widget, context);
}

void tkContextManager::removeContextObject(tkContext *context)
{
    if (!context)
        return;

    QWidget *widget = context->widget();
    if (!d->m_contextWidgets.contains(widget))
        return;

    d->m_contextWidgets.remove(widget);
    if (d->m_activeContext == context)
        updateContextObject(0);
}

void tkContextManager::updateFocusWidget(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)

//    if (now)
//        qWarning() << "tkContextManager focusChanged" << now;

    // Prevent changing the context object just because the menu is activated
    if (qobject_cast<QMenuBar*>(now))
        return;

    tkContext *newContext = 0;
//    if (d->m_mainWindow->focusWidget()) {
    if (qApp->focusWidget()) {
        tkContext *context = 0;
//        QWidget *p = d->m_mainWindow->focusWidget();
        QWidget *p = qApp->focusWidget();
        while (p) {
            context = d->m_contextWidgets.value(p,0);
            if (context) {
                newContext = context;
                break;
            }
            p = p->parentWidget();
        }
    }
    if (newContext) {
        updateContextObject(newContext);
    }
}

void tkContextManager::resetContext()
{
    updateContextObject(0);
}

void tkContextManager::addAdditionalContext(int context)
{
    if (context == 0)
        return;

    if (!d->m_additionalContexts.contains(context))
        d->m_additionalContexts.prepend(context);
}

void tkContextManager::removeAdditionalContext(int context)
{
    if (context == 0)
        return;

    int index = d->m_additionalContexts.indexOf(context);
    if (index != -1)
        d->m_additionalContexts.removeAt(index);
}

bool tkContextManager::hasContext(int context) const
{
    return tkActionManager::instance()->hasContext(context);
}

void tkContextManager::updateContext()
{
    QList<int> contexts;

    if (d->m_activeContext)
        contexts += d->m_activeContext->context();

    contexts += d->m_additionalContexts;

    QList<int> uniquecontexts;
    for (int i = 0; i < contexts.size(); ++i) {
        const int c = contexts.at(i);
        if (!uniquecontexts.contains(c))
            uniquecontexts << c;
    }

    tkActionManager::instance()->setContext(uniquecontexts);
}

void tkContextManager::updateContextObject(tkContext *context)
{
    if (context == d->m_activeContext)
        return;
    tkContext *oldContext = d->m_activeContext;
    d->m_activeContext = context;
    if (!context || oldContext != d->m_activeContext) {
        emit contextAboutToChange(context);
        updateContext();
//        if (debugtkContextManager)
//            qDebug() << "new context object =" << context << (context ? context->widget() : 0)
//            << (context ? context->widget()->metaObject()->className() : 0);
        emit contextChanged(context);
    }
}
