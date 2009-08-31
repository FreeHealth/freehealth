#ifndef IOPTIONSPAGE_H
#define IOPTIONSPAGE_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>

namespace Core {

class CORE_EXPORT IOptionsPage : public QObject
{
    Q_OBJECT
public:
    IOptionsPage(QObject *parent = 0) : QObject(parent) {}
    virtual ~IOptionsPage() {}

    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString category() const = 0;

    virtual void resetToDefaults() = 0;
    virtual void applyChanges() = 0;
    virtual void finish() = 0;

    virtual QWidget *createPage(QWidget *parent = 0) = 0;
};

} // namespace Core

#endif // IDEBUGPAGE_H
