#include "pluginaboutpage.h"

//#include <extensionsystem/pluginspec.h>

#include <coreplugin/constants.h>
#include <coreplugin/global.h>
#include <translationutils/constanttranslations.h>

#include <QTreeWidget>
#include <QTextBrowser>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QApplication>
#include <QLabel>

#include <QDebug>

using namespace Core;

PluginAboutPage::PluginAboutPage(ExtensionSystem::PluginSpec *spec, QObject *parent) :
        Core::IAboutPage(parent), m_Spec(spec)
{
    setObjectName("PluginAboutPage");
    m_Widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    layout->addWidget(tree);
    QLabel *lbl = new QLabel(m_Widget);
    lbl->setText(Trans::ConstantTranslations::tkTr(Trans::Constants::DESCRIPTION));
    layout->addWidget(lbl);
    tb = new QTextBrowser(m_Widget);
    layout->addWidget(tb);
}

PluginAboutPage::~PluginAboutPage()
{
    if (m_Widget)
        delete m_Widget;
}

QString PluginAboutPage::id() const
{
    return m_Spec->name();
}

QString PluginAboutPage::name() const
{
    return m_Spec->name();
}

QString PluginAboutPage::category() const
{
    return Trans::ConstantTranslations::tkTr(Trans::Constants::PLUGINS_CATEGORY);
}

void PluginAboutPage::refreshContents()
{
    if (!m_Spec)
        return;
    tree->clear();
    QFont f;
    f.setBold(true);
    QTreeWidgetItem *i = 0;
    i = new QTreeWidgetItem(tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::INFORMATIONS));
    i->setFont(0,f);
    new QTreeWidgetItem(i, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::VERSION) +" "+ m_Spec->version() );
    new QTreeWidgetItem( i, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DATE_TIME_1_2).arg(__DATE__, __TIME__));
    if (tkGlobal::isDebugCompilation()) {
        new QTreeWidgetItem( i, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DEBUG) );
    } else {
        new QTreeWidgetItem( i, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_RELEASE) );
    }
    new QTreeWidgetItem(i, QStringList() << m_Spec->license() );

    tb->setPlainText(m_Spec->description());
}

QWidget *PluginAboutPage::widget()
{
    return m_Widget;
}
