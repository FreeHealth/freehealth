#include "commondebugpages.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <extensionsystem/pluginmanager.h>

#include <QTreeWidget>
#include <QGridLayout>
#include <QHeaderView>

using namespace Core;
using namespace Core::Internal;


LogErrorDebugPage::LogErrorDebugPage(QObject *parent) :
        IDebugPage(parent)
{
    setObjectName("LogErrorDebugPage");
    m_Widget = new QWidget();
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    Utils::Log::errorsToTreeWidget(tree, true);
    layout->addWidget(tree);
}

LogErrorDebugPage::~LogErrorDebugPage()
{
    if (m_Widget)
        delete m_Widget;
}

QString LogErrorDebugPage::name() const     { return Trans::ConstantTranslations::tkTr(Trans::Constants::LOGGED_ERRORS); }
QString LogErrorDebugPage::category() const { return Trans::ConstantTranslations::tkTr(Trans::Constants::ERRORS); }

void LogErrorDebugPage::refreshContents()
{
    tree->clear();
    Utils::Log::errorsToTreeWidget(tree, true);
}

QWidget *LogErrorDebugPage::widget()
{
    return m_Widget;
}

LogMessageDebugPage::LogMessageDebugPage(QObject *parent) :
        IDebugPage(parent)
{
    setObjectName("LogMessageDebugPage");
    m_Widget = new QWidget();
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    tree = new QTreeWidget(m_Widget);
    layout->addWidget(tree);
}

LogMessageDebugPage::~LogMessageDebugPage()
{
    if (m_Widget)
        delete m_Widget;
}

QString LogMessageDebugPage::name() const
{ return Trans::ConstantTranslations::tkTr(Trans::Constants::LOGGED_MESSAGES); }

QString LogMessageDebugPage::category() const
{ return Trans::ConstantTranslations::tkTr(Trans::Constants::MESSAGES); }

void LogMessageDebugPage::refreshContents()
{
    tree->clear();
    Utils::Log::messagesToTreeWidget(tree, true);
}

QWidget *LogMessageDebugPage::widget()
{
    return m_Widget;
}



///// SETTINGS

SettingDebugPage::SettingDebugPage(QObject *parent) :
        IDebugPage(parent)
{
    setObjectName("SettingDebugPage");
    m_Widget = new QWidget();
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    tree = new QTreeWidget(m_Widget);
    tree->header()->hide();
    layout->addWidget(tree);
}

SettingDebugPage::~SettingDebugPage()
{
    if (m_Widget)
        delete m_Widget;
}

void SettingDebugPage::refreshContents()
{
    tree->clear();
    Core::ICore::instance()->settings()->getTreeWidget(tree);
//    tree->expandAll();
//    tree->resizeColumnToContents(0);
//    tree->resizeCoflumnToContents(1);
}

QWidget *SettingDebugPage::widget()
{
    return m_Widget;
}
