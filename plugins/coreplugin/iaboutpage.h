#ifndef IABOUTPAGE_H
#define IABOUTPAGE_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>

namespace Core {

class CORE_EXPORT IAboutPage : public QObject
{
    Q_OBJECT
public:
    IAboutPage(QObject *parent = 0) : QObject(parent) {}
    virtual ~IAboutPage() {}

    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString category() const = 0;

    virtual void refreshContents() = 0;

    virtual QWidget *widget() = 0;
};

} // namespace Core

#endif // IABOUTPAGE_H
