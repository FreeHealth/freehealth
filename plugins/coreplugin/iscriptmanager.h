#ifndef ISCRIPTMANAGER_H
#define ISCRIPTMANAGER_H

#include <coreplugin/core_exporter.h>
#include <QObject>
#include <QScriptValue>

namespace Core {

class CORE_EXPORT IScriptManager : public QObject
{
    Q_OBJECT
public:
    explicit IScriptManager(QObject *parent = 0) : QObject(parent) {}

    virtual QScriptValue evaluate(const QString &script) = 0;
    virtual QScriptValue addScriptObject(QObject *object) = 0;
//    virtual QScriptValue addScriptObject(QObject *object, const QString &objectNamespace, const QString &objectScriptName) = 0;

    // TODO: add exceptions catchers
};
}

#endif // ISCRIPTMANAGER_H
