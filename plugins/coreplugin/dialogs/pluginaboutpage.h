#ifndef PLUGINABOUTPAGE_H
#define PLUGINABOUTPAGE_H

#include <coreplugin/core_exporter.h>
#include <extensionsystem/pluginspec.h>

#include <coreplugin/iaboutpage.h>
#include <QObject>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTextBrowser;
QT_END_NAMESPACE

namespace Core {

class CORE_EXPORT PluginAboutPage : public Core::IAboutPage
{
    Q_OBJECT
public:
    PluginAboutPage(ExtensionSystem::PluginSpec *spec, QObject *parent);
    ~PluginAboutPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void refreshContents();

    QWidget *widget();

private:
    QPointer<QWidget> m_Widget;
    ExtensionSystem::PluginSpec *m_Spec;
    QTreeWidget *tree;
    QTextBrowser *tb;
};

} // End Core

#endif // BASEFORMWIDGETABOUTPAGE_H
