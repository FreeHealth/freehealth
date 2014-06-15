/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
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
}

LogErrorDebugPage::~LogErrorDebugPage()
{
}

QString LogErrorDebugPage::displayName() const     { return Trans::ConstantTranslations::tkTr(Trans::Constants::LOGGED_ERRORS); }
QString LogErrorDebugPage::category() const { return Trans::ConstantTranslations::tkTr(Trans::Constants::ERRORS); }

QWidget *LogErrorDebugPage::createPage(QWidget *parent)
{
    QWidget *m_Widget = new QWidget(parent);
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTreeWidget *tree = new QTreeWidget(m_Widget);
//    tree->header()->hide();
    Utils::Log::errorsToTreeWidget(tree, true);
    layout->addWidget(tree);
    return m_Widget;
}

LogMessageDebugPage::LogMessageDebugPage(QObject *parent) :
        IDebugPage(parent)
{
    setObjectName("LogMessageDebugPage");
}

LogMessageDebugPage::~LogMessageDebugPage()
{
}

QString LogMessageDebugPage::displayName() const
{ return Trans::ConstantTranslations::tkTr(Trans::Constants::LOGGED_MESSAGES); }

QString LogMessageDebugPage::category() const
{ return Trans::ConstantTranslations::tkTr(Trans::Constants::MESSAGES); }

QWidget *LogMessageDebugPage::createPage(QWidget *parent)
{
    QWidget *m_Widget = new QWidget(parent);
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTreeWidget *tree = new QTreeWidget(m_Widget);
    layout->addWidget(tree);
    Utils::Log::messagesToTreeWidget(tree, true);
    return m_Widget;
}



///// SETTINGS

SettingDebugPage::SettingDebugPage(QObject *parent) :
        IDebugPage(parent)
{
    setObjectName("SettingDebugPage");
}

SettingDebugPage::~SettingDebugPage()
{
}

QWidget *SettingDebugPage::createPage(QWidget *parent)
{
    QWidget *m_Widget = new QWidget(parent);
    QGridLayout *layout = new QGridLayout(m_Widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    QTreeWidget *tree = new QTreeWidget(m_Widget);
//    tree->header()->hide();
    layout->addWidget(tree);
    Core::ICore::instance()->settings()->getTreeWidget(tree);
    return m_Widget;
}
