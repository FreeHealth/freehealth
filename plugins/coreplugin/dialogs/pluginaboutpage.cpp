/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pluginaboutpage.h"

//#include <extensionsystem/pluginspec.h>

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
    setId(objectName());
}

PluginAboutPage::~PluginAboutPage()
{
    if (m_Widget)
        delete m_Widget;
}

QString PluginAboutPage::displayName() const
{
    return m_Spec->name();
}

QString PluginAboutPage::category() const
{
    return tkTr(Trans::Constants::PLUGINS_CATEGORY);
}

QWidget *PluginAboutPage::createPage(QWidget *parent)
{
    if (!m_Spec)
        return new QWidget(parent);

    QWidget *w = new QWidget(parent);
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
    i = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::INFORMATION));
    i->setFont(0,f);
    new QTreeWidgetItem(i, QStringList() << tkTr(Trans::Constants::VERSION) + " " + m_Spec->version() );
    new QTreeWidgetItem( i, QStringList() << tkTr(Trans::Constants::BUILD_DATE_TIME_1_2).arg(__DATE__, __TIME__));
    if (Utils::isDebugWithoutInstallCompilation()) {
        new QTreeWidgetItem( i, QStringList() << tkTr(Trans::Constants::BUILD_DEBUG) + " - no install");
    } else {
        new QTreeWidgetItem( i, QStringList() << tkTr(Trans::Constants::BUILD_RELEASE) );
    }
    new QTreeWidgetItem(i, QStringList() << "License: " + m_Spec->license());
    tree->expandAll();

    // populate textbrowser
    tb->setPlainText(m_Spec->description());

    return w;
}
