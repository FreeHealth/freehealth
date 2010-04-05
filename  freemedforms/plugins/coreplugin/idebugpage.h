#ifndef IDEBUGPAGE_H
#define IDEBUGPAGE_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>

namespace Core {

class CORE_EXPORT IDebugPage : public QObject
{
    Q_OBJECT
public:
    IDebugPage(QObject *parent = 0) : QObject(parent) {}
    virtual ~IDebugPage() {}

    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString category() const = 0;

    virtual QWidget *widget() = 0;
};

} // namespace Core

#endif // IDEBUGPAGE_H
