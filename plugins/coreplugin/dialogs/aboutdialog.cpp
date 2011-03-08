/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
/**
  \class Core::AboutDialog
  \brief This dialog shows a complete About dialog with a tabwidget containing : a short about text, a complete team list, a full licence text.
   It also shows the compilation date and time and the actual version of the application. \n
   The name of the application is picked into qApp->applicationName() and the version into qApp->applicationVersion(). \n
   You can inform the team using the addTeamList().\n
   You can get the default licence terms using the static getLicenceTerms() function. This function manages english and french language by looking the default QLocale()::name().\n
*/

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <extensionsystem/pluginmanager.h>

#include <coreplugin/iaboutpage.h>

#include <utils/global.h>

#include <QStackedLayout>
#include <QHeaderView>

using namespace Core;
using namespace Core::Internal;

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent), m_ui(new Core::Internal::Ui::AboutDialog)
{
    typedef QMap<QString, QTreeWidgetItem *> CategoryItemMap;
    m_ui->setupUi(this);
    m_ui->applicationNameLabel->setText(qApp->applicationName());
    m_slayout = new QStackedLayout(m_ui->forStack);
    m_ui->forStack->setLayout(m_slayout);
    setWindowTitle(qApp->applicationName());
    setObjectName("AboutDialog");

    m_ui->tree->header()->hide();
    connect(m_ui->tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
        this, SLOT(currentItemChanged(QTreeWidgetItem*)));

    QList<IAboutPage*> pages = ExtensionSystem::PluginManager::instance()->getObjects<IAboutPage>();
    setPages(pages);
//    m_ui->splitter->setStretchFactor(0,1);
//    m_ui->splitter->setStretchFactor(1,3);

    // resize and center window
    Utils::resizeAndCenter(this);
}

void AboutDialog::setPages(const QList<IAboutPage*> pages)
{
    typedef QMap<QString, QTreeWidgetItem *> CategoryItemMap;

    CategoryItemMap categories;

    m_ui->tree->clear();
    foreach (IAboutPage *page, pages) {
        // ensure category root
        const QString categoryName = page->category();
        CategoryItemMap::iterator cit = categories.find(categoryName);
        if (cit == categories.end()) {
            QTreeWidgetItem *categoryItem = new QTreeWidgetItem(m_ui->tree);
            categoryItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
            categoryItem->setText(0, page->category());
            cit = categories.insert(categoryName, categoryItem);
        }
        // add item
        QTreeWidgetItem *pageItem = new QTreeWidgetItem(cit.value(), QStringList(page->name()));
        QWidget *w = page->widget();
        pageItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        m_Widgets.insert(pageItem,w);
        m_slayout->addWidget(w);
    }
    m_ui->tree->sortItems(0,Qt::AscendingOrder);
}

void AboutDialog::showDialog()
{
    m_ui->tree->expandAll();
    if (QTreeWidgetItem *rootItem = m_ui->tree->topLevelItem(0)) {
        m_ui->tree->scrollToItem(rootItem);
        if (rootItem->childCount())
            m_ui->tree->setCurrentItem(rootItem->child(0));
    }
    exec();
}

AboutDialog::~AboutDialog()
{
    // delete all widgets in use
    qDeleteAll(m_Widgets.values());
    delete m_ui;
}

void AboutDialog::currentItemChanged(QTreeWidgetItem *cat)
{
    if (m_Widgets.keys().contains(cat)) {
        m_slayout->setCurrentWidget(m_Widgets.value(cat));
    }
}


