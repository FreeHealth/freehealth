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
/***************************************************************************
 *   Adaptations to FreeMedForms and improvments by : Eric Maeker, MD      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/

/**
  \class Core::SettingsDialog
  Dialogs that presents all the IOptionsPage stored in the plugin manager.
*/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <utils/global.h>

#include <extensionsystem/pluginmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ioptionspage.h>
#include <coreplugin/dialogs/helpdialog.h>

#include <QHeaderView>
#include <QPushButton>

namespace {
    struct PageData {
        int index;
        QString category;
        QString id;
    };
}

Q_DECLARE_METATYPE(::PageData);

using namespace Core;
using namespace Core::Internal;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

SettingsDialog::SettingsDialog(QWidget *parent, const QString &categoryId, const QString &pageId) :
        QDialog(parent), m_applied(false)
{
    m_ui = new Ui::SettingsDialog();
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    m_ui->setupUi(this);
    QString initialCategory = categoryId;
    QString initialPage = pageId;
    if (initialCategory.isEmpty() && initialPage.isEmpty()) {
        initialCategory = settings()->value("Dialogs/Settings/LastPreferenceCategory", QVariant(QString())).toString();
        initialPage = settings()->value("Dialogs/Settings/LastPreferencePage", QVariant(QString())).toString();
    }
    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);

    connect(m_ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
    connect(m_ui->buttonBox->button(QDialogButtonBox::Help), SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));

    m_ui->splitter->setCollapsible(1, false);
    m_ui->pageTree->header()->setVisible(false);

    connect(m_ui->pageTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
        this, SLOT(pageSelected()));

    QMap<QString, QTreeWidgetItem *> categories;

    QList<IOptionsPage*> pages =
        ExtensionSystem::PluginManager::instance()->getObjects<IOptionsPage>();

    int index = 0;
    foreach (IOptionsPage *page, pages) {
        PageData pageData;
        pageData.index = index;
        pageData.category = page->category();
        pageData.id = page->id();

        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, page->name());
        item->setData(0, Qt::UserRole, qVariantFromValue(pageData));

        QStringList categoriesId = page->category().split(QLatin1Char('|'));
        QStringList trCategories = page->category().split(QLatin1Char('|'));
        QString currentCategory = categoriesId.at(0);

        QTreeWidgetItem *treeitem;
        if (!categories.contains(currentCategory)) {
            treeitem = new QTreeWidgetItem(m_ui->pageTree);
            treeitem->setText(0, trCategories.at(0));
            treeitem->setData(0, Qt::UserRole, qVariantFromValue(pageData));
            categories.insert(currentCategory, treeitem);
        }

        int catCount = 1;
        while (catCount < categoriesId.count()) {
            if (!categories.contains(currentCategory + QLatin1Char('|') + categoriesId.at(catCount))) {
                treeitem = new QTreeWidgetItem(categories.value(currentCategory));
                currentCategory +=  QLatin1Char('|') + categoriesId.at(catCount);
                treeitem->setText(0, trCategories.at(catCount));
                treeitem->setData(0, Qt::UserRole, qVariantFromValue(pageData));
                categories.insert(currentCategory, treeitem);
            } else {
                currentCategory +=  QLatin1Char('|') + categoriesId.at(catCount);
            }
            ++catCount;
        }

        categories.value(currentCategory)->addChild(item);

        m_pages.append(page);
        m_ui->stackedPages->addWidget(page->createPage(m_ui->stackedPages));

        if (page->id() == initialPage && currentCategory == initialCategory) {
            m_ui->stackedPages->setCurrentIndex(m_ui->stackedPages->count());
            m_ui->pageTree->setCurrentItem(item);
        }

        index++;
    }

    QList<int> sizes;
    sizes << 150 << 300;
    m_ui->splitter->setSizes(sizes);

    m_ui->pageTree->sortItems(0, Qt::AscendingOrder);

    m_ui->splitter->setStretchFactor(m_ui->splitter->indexOf(m_ui->pageTree), 0);
    m_ui->splitter->setStretchFactor(m_ui->splitter->indexOf(m_ui->layoutWidget), 1);

    // resize and center window
    Utils::resizeAndCenter(this);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::pageSelected()
{
    QTreeWidgetItem *item = m_ui->pageTree->currentItem();
    PageData data = item->data(0, Qt::UserRole).value<PageData>();
    int index = data.index;
    m_currentCategory = data.category;
    m_currentPage = data.id;
    m_ui->stackedPages->setCurrentIndex(index);
}

void SettingsDialog::accept()
{
    m_applied = true;
    foreach (IOptionsPage *page, m_pages) {
        page->applyChanges();
        page->finish();
    }
    done(QDialog::Accepted);
}

void SettingsDialog::reject()
{
    foreach (IOptionsPage *page, m_pages)
        page->finish();
    done(QDialog::Rejected);
}

void SettingsDialog::apply()
{
    foreach (IOptionsPage *page, m_pages)
        page->applyChanges();
    m_applied = true;
}

void SettingsDialog::restoreDefaults()
{
    QTreeWidgetItem *item = m_ui->pageTree->currentItem();
    const PageData &data = item->data(0, Qt::UserRole).value<PageData>();
    int index = data.index;
    m_pages.at(index)->resetToDefaults();
}

void SettingsDialog::showHelp()
{
    QTreeWidgetItem *item = m_ui->pageTree->currentItem();
    const PageData &data = item->data(0, Qt::UserRole).value<PageData>();
    int index = data.index;
    HelpDialog::showPage(m_pages.at(index)->helpPage());
}

bool SettingsDialog::execDialog()
{
    m_applied = false;
    exec();
    return m_applied;
}

void SettingsDialog::done(int val)
{
    settings()->setValue("Dialogs/Settings/LastPreferenceCategory", m_currentCategory);
    settings()->setValue("Dialogs/Settings/LastPreferencePage", m_currentPage);
    QDialog::done(val);
}
