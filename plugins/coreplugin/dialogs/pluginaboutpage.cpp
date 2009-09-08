#include "pluginaboutpage.h"

//#include <extensionsystem/pluginspec.h>

#include <coreplugin/constants.h>
#include <utils/global.h>
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
using namespace Trans::ConstantTranslations;

PluginAboutPage::PluginAboutPage(ExtensionSystem::PluginSpec *spec, QObject *parent) :
        Core::IAboutPage(parent), m_Spec(spec)
{
    setObjectName("PluginAboutPage");
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
    return tkTr(Trans::Constants::PLUGINS_CATEGORY);
}

QWidget *PluginAboutPage::widget()
{
    if (!m_Spec)
        return new QWidget();

    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTreeWidget *tree = new QTreeWidget(w);
    tree->header()->hide();
    layout->addWidget(tree);
    QLabel *lbl = new QLabel(w);
    lbl->setText(tkTr(Trans::Constants::DESCRIPTION));
    layout->addWidget(lbl);
    QTextBrowser *tb = new QTextBrowser(w);
    layout->addWidget(tb);

    // popuplate tree
    tree->clear();
    QFont f;
    f.setBold(true);
    QTreeWidgetItem *i = 0;
    i = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::INFORMATIONS));
    i->setFont(0,f);
    new QTreeWidgetItem(i, QStringList() << tkTr(Trans::Constants::VERSION) + " " + m_Spec->version() );
    new QTreeWidgetItem( i, QStringList() << tkTr(Trans::Constants::BUILD_DATE_TIME_1_2).arg(__DATE__, __TIME__));
    if (Utils::isDebugCompilation()) {
        new QTreeWidgetItem( i, QStringList() << tkTr(Trans::Constants::BUILD_DEBUG) );
    } else {
        new QTreeWidgetItem( i, QStringList() << tkTr(Trans::Constants::BUILD_RELEASE) );
    }
    new QTreeWidgetItem(i, QStringList() << m_Spec->license() );
    tree->expandAll();

    // populate textbrowser
    tb->setPlainText(m_Spec->description());

    return w;
}
