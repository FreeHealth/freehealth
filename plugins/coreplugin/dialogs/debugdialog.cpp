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
/**
  \class Core::DebugDialog
  \brief This class is a QDialog that shows the content of the tkLog class of this library.
  It can send message over the web with the tkSendMessage class (part of freemedforms).
*/
#include "debugdialog.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_spashandupdate.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_filepathxml.h>
#include <extensionsystem/pluginmanager.h>

#include <coreplugin/idebugpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>

#include <QStackedLayout>
#include <QHeaderView>
#include <QMainWindow>

#include "ui_debugdialog.h"

using namespace Core;
using namespace Core::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}

DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent), m_ui(new Core::Internal::Ui::DebugDialog)
{
    m_ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);

    setWindowTitle(qApp->applicationName());
    setObjectName("DebugDialog");

    QList<IDebugPage*> pages =
        ExtensionSystem::PluginManager::instance()->getObjects<IDebugPage>();

    m_ui->widget->setPages<IDebugPage>(pages);
    m_ui->widget->setSettingKey("Dialogs/Debug");
    m_ui->widget->setupUi();
    m_ui->widget->expandAllCategories();

    // Now we have a special bug report dialog (Utils::BugReportDialog)
    // So we don't want to use redondant code --> disable the send button
    // FIXME: remove the send button
    m_ui->butSend->setEnabled(false);

    // resize and center windows
    Utils::resizeAndCenter(this);
}

DebugDialog::~DebugDialog()
{
    m_ui->widget->saveState();
    delete m_ui;
}

void DebugDialog::on_butSend_clicked()
{
    LOG(tkTr(Trans::Constants::START_MESSAGE_SENDING));

    QString msg;

    msg += "{{tag>bugreport}}\n\n";
    msg += "====== BUG REPORT ======\n\n";
    msg += "  * Date of report: $<%DATE%>$\n";
    msg += "  * User: " + user()->value(Core::IUser::FullName).toString() + "\n";
    msg += "  * User Uid: " + user()->uuid() + "\n";

    msg += "===== USER OUTPUT =====\n\n";
    msg += Utils::askUser(tkTr(Trans::Constants::START_MESSAGE_SENDING), tkTr(Trans::Constants::PLEASE_DESCRIBE_PROBLEM));
    msg += "\n";
    msg += Utils::askUser(tkTr(Trans::Constants::START_MESSAGE_SENDING), tkTr(Trans::Constants::YOUR_MAIL_ADDRESS));

    // get full log including settings and logs
    msg += "\n\n" + Utils::Log::toString( Core::ICore::instance()->settings()->toString() );

    // send information
    connect(&m_sender, SIGNAL(sent()), this, SLOT(onSendMessage_done()));
    m_sender.setTypeOfMessage( Utils::MessageSender::InformationToDeveloper );
    m_sender.setMessage(msg);
    if (m_sender.postMessage()) {
        m_sending = true;
        // showing a messagebox
        m_infoMessageBox = new QMessageBox(this);
        m_infoMessageBox->setText( tr( "Sending debugging information to the development team") );
        m_infoMessageBox->setInformativeText( tr("Trying to send information to the development team.\n"
                                                 "Using URL: %1\n"
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
    QString fileName = s->path( Core::ISettings::UserResourcesPath ) + "/logs.txt" ;
    Utils::Log::addMessage( this, tkTr(Trans::Constants::SAVING_FILE_1).arg(fileName));
    return Utils::saveStringToFile( Utils::Log::toString(), fileName );
}

void DebugDialog::onSendMessage_done()
{
    LOG(tr("Debugging information successfully sent."));
    if (m_infoMessageBox) {
        m_infoMessageBox->setInformativeText( tr("Debugging information successfully sent to the development team.\n"
                                             "Using Url: %1\n"
                                             "%2" ).arg( m_sender.usedUrl(), m_sender.resultMessage() ) );
        m_infoMessageBox->exec();
        delete m_infoMessageBox;
        m_infoMessageBox=0;
    }
    m_sending = false;
}
