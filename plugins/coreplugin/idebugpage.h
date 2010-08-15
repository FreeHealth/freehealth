#ifndef IDEBUGPAGE_H
#define IDEBUGPAGE_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>

/**
 * \file idebugpage.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.10
 * \date 09 Aug 2009
 * \class Core::IDebugPage
 * \brief Derive objects from this interface and set it inside the PluginManager oject pool to get the page in the Core::DebugDialog.
*/

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
