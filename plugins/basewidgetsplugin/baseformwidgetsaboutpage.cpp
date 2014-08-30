#include "baseformwidgetsaboutpage.h"

//#include <extensionsystem/pluginspec.h>

#include <coreplugin/constants.h>
#include <translationutils/constanttranslations.h>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QApplication>

#include <QDebug>

using namespace BaseWidgets;

AboutBaseWidgetsPlugin::AboutBaseWidgetsPlugin(ExtensionSystem::PluginSpec *spec, QObject *parent) :
        Core::IAboutPage(parent), m_Spec(spec)
{
    setObjectName("AboutBaseWidgetsPlugin");
    m_Widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_Widget);
    tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    layout->addWidget(tree);
}

AboutBaseWidgetsPlugin::~AboutBaseWidgetsPlugin()
{
    if (m_Widget)
        delete m_Widget;
}

QString AboutBaseWidgetsPlugin::id() const
{
    return objectName();
}

QString AboutBaseWidgetsPlugin::name() const
{
    return m_Spec->name();
}

QString AboutBaseWidgetsPlugin::category() const
{
    return Trans::ConstantTranslations::tkTr(Trans::Constants::PLUGINS_CATEGORY);
}

void AboutBaseWidgetsPlugin::refreshContents()
{
    if (!m_Spec)
        return;
    tree->clear();
    QFont f;
    f.setBold(true);
    QTreeWidgetItem *i = 0;
    i = new QTreeWidgetItem(tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::INFORMATION));
    i->setFont(0,f);
    new QTreeWidgetItem(i, QStringList() << m_Spec->version() );
    new QTreeWidgetItem(i, QStringList() << m_Spec->compatVersion() );

    i = new QTreeWidgetItem(tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::DESCRIPTION));
    i->setFont(0,f);
    new QTreeWidgetItem(i, QStringList() << m_Spec->description() );
    new QTreeWidgetItem(i, QStringList() << m_Spec->license() );
}

QWidget *AboutBaseWidgetsPlugin::widget()
{
    return m_Widget;
}
