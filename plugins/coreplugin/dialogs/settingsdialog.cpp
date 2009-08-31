/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
**
**************************************************************************/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <extensionsystem/pluginmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <coreplugin/ioptionspage.h>

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

SettingsDialog::SettingsDialog(QWidget *parent, const QString &categoryId,
                               const QString &pageId)
    : QDialog(parent), m_applied(false)
{
    m_ui->setupUi(this);
    QString initialCategory = categoryId;
    QString initialPage = pageId;
    if (initialCategory.isEmpty() && initialPage.isEmpty()) {
        ISettings *settings = ICore::instance()->settings();
        initialCategory = settings->value("General/LastPreferenceCategory", QVariant(QString())).toString();
        initialPage = settings->value("General/LastPreferencePage", QVariant(QString())).toString();
    }
    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);

    connect(m_ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));

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

    m_ui->splitter->setStretchFactor(m_ui->splitter->indexOf(m_ui->pageTree), 0);
    m_ui->splitter->setStretchFactor(m_ui->splitter->indexOf(m_ui->layoutWidget), 1);
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

bool SettingsDialog::execDialog()
{
    m_applied = false;
    exec();
    return m_applied;
}

void SettingsDialog::done(int val)
{
    ISettings *settings = ICore::instance()->settings();
    settings->setValue("General/LastPreferenceCategory", m_currentCategory);
    settings->setValue("General/LastPreferencePage", m_currentPage);
    QDialog::done(val);
}
