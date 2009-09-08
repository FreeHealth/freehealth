/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
/**
  \class DebugDialog
  \brief This class is a QDialog that shows the content of the tkLog class of this library.
  It can send message over the web with the tkSendMessage class (part of freemedforms).
  \ingroup toolkit
  \ingroup widget_toolkit
*/
#include "debugdialog.h"
#include "ui_DebugDialog.h"

#include <extensionsystem/pluginmanager.h>

#include <coreplugin/global.h>
#include <coreplugin/idebugpage.h>

#include <QStackedLayout>

using namespace Core;
using namespace Core::Internal;

DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent), m_ui(new Core::Internal::Ui::DebugDialog)
{
    typedef QMap<QString, QTreeWidgetItem *> CategoryItemMap;
    m_ui->setupUi(this);
    m_slayout = new QStackedLayout(m_ui->forStack);
    m_slayout->setMargin(0);
    m_slayout->setSpacing(0);
    setWindowTitle( qApp->applicationName() );
    setObjectName( "DebugDialog" );

    m_ui->tree->header()->hide();
    connect(m_ui->tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
        this, SLOT(currentItemChanged(QTreeWidgetItem*)));

    QList<IDebugPage*> pages = ExtensionSystem::PluginManager::instance()->getObjects<IDebugPage>();
    setPages(pages);
//    m_ui->splitter->setStretchFactor(0,1);
//    m_ui->splitter->setStretchFactor(1,3);
//    m_ui->splitter->setSizes(QList<int>() << m_ui->tree->size().height() << );
    this->resize(500,500);
}

void DebugDialog::setPages(const QList<IDebugPage*> pages)
{
    typedef QMap<QString, QTreeWidgetItem *> CategoryItemMap;

    CategoryItemMap categories;

    m_ui->tree->clear();
    foreach (IDebugPage *page, pages) {
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
        pageItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        QWidget *w = page->widget();
        if (w)
            m_slayout->addWidget(w);
        m_Widgets.insert(pageItem,w);
    }
}

void DebugDialog::showDialog()
{
    m_ui->tree->expandAll();
    if (QTreeWidgetItem *rootItem = m_ui->tree->topLevelItem(0)) {
        m_ui->tree->scrollToItem(rootItem);
        if (rootItem->childCount())
            m_ui->tree->setCurrentItem(rootItem->child(0));
    }
//    updateOkButton();
//    if (exec() != Accepted)
//        return 0;
//    return currentWizard();
    exec();
}

DebugDialog::~DebugDialog()
{
    // delete all widgets in use
    qDeleteAll(m_Widgets.values());
    delete m_ui;
}

void DebugDialog::currentItemChanged(QTreeWidgetItem *cat)
{
    if (m_Widgets.keys().contains(cat)) {
        m_slayout->setCurrentWidget(m_Widgets.value(cat));
    }
}

void DebugDialog::on_fullScreenButton_clicked()
{
    static bool fullscreen = false;
    fullscreen = !fullscreen;
//    Core::tkGlobal::setFullScreen(this,fullscreen);
//    this->show();
}

