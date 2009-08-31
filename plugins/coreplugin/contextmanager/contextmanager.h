#ifndef TKCONTEXTMANAGER_H
#define TKCONTEXTMANAGER_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QObject>

namespace Core {

class CORE_EXPORT ContextManager : public QObject
{
    Q_OBJECT
public:
    virtual IContext *contextObject(QWidget *widget) = 0;
    virtual void addContextObject(IContext *context) = 0;
    virtual void removeContextObject(IContext *context) = 0;
    virtual void resetContext() = 0;

    virtual IContext *currentContextObject() const = 0;
    virtual void addAdditionalContext(int context) = 0;
    virtual void removeAdditionalContext(int context) = 0;
    virtual bool hasContext(int context) const = 0;

    virtual void updateContext() = 0;

Q_SIGNALS:
    void contextAboutToChange(IContext *context);
    void contextChanged(IContext *context);
};

} // End Core

#endif // TKCONTEXTMANAGER_H
