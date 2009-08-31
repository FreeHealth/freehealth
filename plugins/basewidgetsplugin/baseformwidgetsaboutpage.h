#ifndef BASEFORMWIDGETABOUTPAGE_H
#define BASEFORMWIDGETABOUTPAGE_H

//#include "basewigets_exporter.h"
#include <extensionsystem/pluginspec.h>

#include <coreplugin/iaboutpage.h>
#include <QObject>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

namespace BaseWidgets {

class AboutBaseWidgetsPlugin : public Core::IAboutPage
{
    Q_OBJECT
public:
    AboutBaseWidgetsPlugin(ExtensionSystem::PluginSpec *spec, QObject *parent);
    ~AboutBaseWidgetsPlugin();

    QString id() const;
    QString name() const;
    QString category() const;

    void refreshContents();

    QWidget *widget();

private:
    QPointer<QWidget> m_Widget;
    ExtensionSystem::PluginSpec *m_Spec;
    QTreeWidget *tree;
};

} // End Core

#endif // BASEFORMWIDGETABOUTPAGE_H
