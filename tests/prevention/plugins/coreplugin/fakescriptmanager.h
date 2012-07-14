#ifndef CORE_FAKESCRIPTMANAGER_H
#define CORE_FAKESCRIPTMANAGER_H

#include <coreplugin/iscriptmanager.h>

#include <QScriptEngine>
#include <QScriptValue>

namespace Core {

class FakeScriptManager : public Core::IScriptManager
{
    Q_OBJECT
public:
    FakeScriptManager(QObject *parent) : Core::IScriptManager(parent) {}

    QScriptValue evaluate(const QString &script) {return _engine.evaluate(script);}
    QScriptValue addScriptObject(QObject *object) {Q_UNUSED(object); return QScriptValue();}

private:
    QScriptEngine _engine;
};

} // namespace Core

#endif // CORE_FAKESCRIPTMANAGER_H
