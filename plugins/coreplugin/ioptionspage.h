#ifndef IOPTIONSPAGE_H
#define IOPTIONSPAGE_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>

/**
 * \file ioptionspage.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.10
 * \date 09 Aug 2009
 * \class Core::IOptionsPage
 * \brief Represents an application preferences page. Derive objects from this interface and set it inside the PluginManager oject pool to get the page in the Core::SettingsDialog.
*/

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
    virtual void checkSettingsValidity() = 0;
    virtual void applyChanges() = 0;
    virtual void finish() = 0;

    virtual QString helpPage() = 0;

    virtual QWidget *createPage(QWidget *parent = 0) = 0;
};

} // namespace Core

#endif // IDEBUGPAGE_H
