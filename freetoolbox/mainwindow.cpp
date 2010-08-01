/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "itoolpage.h"

#include <utils/log.h>

#include <QTreeWidgetItem>
#include <QMap>
#include <QList>
#include <QDir>


namespace {
    struct PageData {
        int index;
        QString category;
        QString id;
    };
}

Q_DECLARE_METATYPE(::PageData);

MainWindow *MainWindow::m_Instance = 0;

MainWindow *MainWindow::instance()
{
    if (!m_Instance)
        m_Instance = new MainWindow;
    return m_Instance;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_applied(false),
    m_Settings(0)
{
    setObjectName("MainWindow");
#ifdef Q_OS_MAC
    m_Settings = new QSettings(qApp->applicationDirPath()+"/../../../"+qApp->applicationName()+".ini", QSettings::IniFormat, this);
#else
    m_Settings = new QSettings(qApp->applicationDirPath()+QDir::separator()+qApp->applicationName()+".ini", QSettings::IniFormat, this);
#endif
    Utils::Log::addMessage(this, "Using Ini File " + m_Settings->fileName());
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    ui->setupUi(this);
    m_currentCategory = settings()->value("LastPreferenceCategory", QVariant(QString())).toString();
    m_currentPage = settings()->value("LastPreferencePage", QVariant(QString())).toString();

    ui->splitter->setCollapsible(1, false);
    ui->pageTree->header()->setVisible(false);

    connect(ui->pageTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
        this, SLOT(pageSelected()));

}

void MainWindow::preparePages()
{
    QMap<QString, QTreeWidgetItem *> categories;

    int index = 0;
    foreach (IToolPage *page, m_pages) {
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
            treeitem = new QTreeWidgetItem(ui->pageTree);
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

//        m_pages.append(page);
        ui->stackedPages->addWidget(page->createPage(ui->stackedPages));

        if (page->id() == m_currentPage && currentCategory == m_currentCategory) {
            ui->stackedPages->setCurrentIndex(ui->stackedPages->count());
            ui->pageTree->setCurrentItem(item);
        }

        index++;
    }

//    QList<int> sizes;
//    sizes << 150 << 300;
//    ui->splitter->setSizes(sizes);

//    ui->pageTree->sortItems(0, Qt::AscendingOrder);

//    ui->splitter->setStretchFactor(ui->splitter->indexOf(ui->pageTree), 0);
//    ui->splitter->setStretchFactor(ui->splitter->indexOf(ui->layoutWidget), 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QSettings *MainWindow::settings()
{
    return m_Settings;
}

void MainWindow::addPage(IToolPage *page)
{
    if (page)
        m_pages << page;
}

void MainWindow::pageSelected()
{
    QTreeWidgetItem *item = ui->pageTree->currentItem();
    PageData data = item->data(0, Qt::UserRole).value<PageData>();
    int index = data.index;
    m_currentCategory = data.category;
    m_currentPage = data.id;
    ui->stackedPages->setCurrentIndex(index);
}

void MainWindow::showHelp()
{
//    QTreeWidgetItem *item = ui->pageTree->currentItem();
//    const PageData &data = item->data(0, Qt::UserRole).value<PageData>();
//    int index = data.index;
//    Core::HelpDialog::showPage(m_pages.at(index)->helpPage());
}

void MainWindow::saveSettings()
{
    settings()->setValue("LastPreferenceCategory", m_currentCategory);
    settings()->setValue("LastPreferencePage", m_currentPage);
}

void MainWindow::readSettings()
{
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
