#ifndef CONTEXTMANAGERPRIVATE_H
#define CONTEXTMANAGERPRIVATE_H
#include "contextmanager.h"
#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QMap>

#include <coreplugin/contextmanager/icontext.h>


namespace Core {
namespace Internal {

class ContextManagerPrivate : public ContextManager
{
    Q_OBJECT
public:
    ContextManagerPrivate(QMainWindow *mainWin);
    ContextManagerPrivate() {}

    IContext *contextObject(QWidget *widget);
    void addContextObject(IContext *context);
    void removeContextObject(IContext *context);
    void resetContext();

    IContext *currentContextObject() const;
    void addAdditionalContext(int context);
    void removeAdditionalContext(int context);
    bool hasContext(int context) const;

    void updateContext();

private Q_SLOTS:
    void updateFocusWidget(QWidget *old, QWidget *now);

private:
    void updateContextObject(IContext *context);

private:
    QList<int> m_globalContext;
    QList<int> m_additionalContexts;
    IContext *m_activeContext;
    QMap<QWidget *, IContext *> m_contextWidgets;
    QMainWindow *m_mainWindow;
};


} // End Internal
} // End Core

#endif // CONTEXTMANAGERPRIVATE_H
