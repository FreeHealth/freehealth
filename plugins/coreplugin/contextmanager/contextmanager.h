#ifndef CONTEXTMANAGER_H
#define CONTEXTMANAGER_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QObject>

/**
 * \file contextmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 25 Apr 2010
 * \brief adaptation of QtCreator MainWindow -> context management
*/

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
    void contextAboutToChange(Core::IContext *context);
    void contextChanged(Core::IContext *context);
};

} // End Core

#endif // TKCONTEXTMANAGER_H
