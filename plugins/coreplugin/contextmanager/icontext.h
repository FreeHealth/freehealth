#ifndef ICONTEXT_H
#define ICONTEXT_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QWidget>

class CORE_EXPORT IContext : public QObject
{
public:
    IContext(QObject *parent = 0) : QObject(parent) {}
    virtual ~IContext() {}

    virtual QList<int> context() const = 0;
    virtual QWidget *widget() = 0;
    virtual QString contextHelpId() const { return QString(); }
};


#endif // ICONTEXT_H
