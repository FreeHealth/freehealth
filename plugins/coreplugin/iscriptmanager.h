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
    virtual QScriptValue addScriptObject(const QObject *object) = 0;

};
}

#endif // ISCRIPTMANAGER_H
