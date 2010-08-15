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
/**
  \class Core::DebugDialog
  \brief This class is a QDialog that shows the content of the tkLog class of this library.
  It can send message over the web with the tkSendMessage class (part of freemedforms).
*/
#include "debugdialog.h"
#include "ui_debugdialog.h"

#include <utils/log.h>
#include <utils/global.h>
#include <extensionsystem/pluginmanager.h>

#include <coreplugin/idebugpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>

#include <QStackedLayout>
#include <QHeaderView>
#include <QMainWindow>

using namespace Core;
using namespace Core::Internal;
using namespace Trans::ConstantTranslations;

DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent), m_ui(new Core::Internal::Ui::DebugDialog)
{
    m_ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

    m_slayout = new QStackedLayout(m_ui->forStack);
    m_slayout->setMargin(0);
    m_slayout->setSpacing(0);
    setWindowTitle( qApp->applicationName() );
    setObjectName( "DebugDialog" );

    m_ui->tree->header()->hide();

    // connect tree navigation
    connect(m_ui->tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
        this, SLOT(currentItemChanged(QTreeWidgetItem*)));

    m_sending = false;
    if (!Utils::isDebugCompilation())
        m_ui->butSend->setEnabled( Utils::Log::hasError() );

    QList<IDebugPage*> pages = ExtensionSystem::PluginManager::instance()->getObjects<IDebugPage>();
    setPages(pages);

    // resize and center windows
    Utils::resizeAndCenter(this);
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
//    Utils::setFullScreen(this,fullscreen);
//    this->show();
}

void DebugDialog::on_butSend_clicked()
{
    if (!Utils::isDebugCompilation()) {
        if (!Utils::Log::hasError())  // this should never be in this member
            return;
    }
    Utils::Log::addMessage(this, tkTr(Trans::Constants::START_MESSAGE_SENDING) );

    QString msg = Utils::askUser( tkTr(Trans::Constants::START_MESSAGE_SENDING), tkTr(Trans::Constants::PLEASE_DESCRIBE_PROBLEM) );
    // get full log including settings and logs
    msg += "\n\n" + Utils::Log::toString( Core::ICore::instance()->settings()->toString() );

    // send informations
    connect( &m_sender, SIGNAL(sent()), this, SLOT(onSendMessage_done()));
    m_sender.setTypeOfMessage( Utils::MessageSender::InformationToDevelopper );
    m_sender.setMessage( msg );
    if (m_sender.postMessage()) {
        m_sending = true;
        // showing a messagebox
        m_infoMessageBox = new QMessageBox(this);
        m_infoMessageBox->setText( tr( "Sending debugging informations to dev team") );
        m_infoMessageBox->setInformativeText( tr("Trying to send informations to dev team.\n"
                                                 "Using Url : %1 \n"
                                                 "Please wait..." ).arg( m_sender.usedUrl() ) );
        m_infoMessageBox->setWindowTitle( qApp->applicationName() );
        m_infoMessageBox->setStandardButtons( QMessageBox::Ok );
        m_infoMessageBox->show();
    } else {
        m_sending = false;
    }
}

bool DebugDialog::on_butSave_clicked()
{
    Core::ISettings *s = Core::ICore::instance()->settings();
    QString fileName = s->path( Core::ISettings::ResourcesPath ) + "/logs.txt" ;
    Utils::Log::addMessage( this, tkTr(Trans::Constants::SAVING_FILE_1).arg(fileName));
    return Utils::saveStringToFile( Utils::Log::toString(), fileName );
}

void DebugDialog::onSendMessage_done()
{
    Utils::Log::addMessage( this, tr( "Debugging informations correctly sent." ) );
    if ( m_infoMessageBox )
    {
        m_infoMessageBox->setInformativeText( tr("Debugging informations correctly send to dev team.\n"
                                             "Using Url : %1 \n"
                                             "%2" ).arg( m_sender.usedUrl(), m_sender.resultMessage() ) );
        m_infoMessageBox->exec();
        delete m_infoMessageBox;
        m_infoMessageBox=0;
    }
    m_sending = false;
}
