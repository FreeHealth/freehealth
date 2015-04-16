/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include <QFileDialog>

#include "ui_debugdialog.h"

using namespace Core;
using namespace Core::Internal;
using namespace Trans::ConstantTranslations;

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

    connect(m_ui->butSave, SIGNAL(clicked()), this, SLOT(saveLogToFile()));

    // resize and center windows
    Utils::resizeAndCenter(this);
}

DebugDialog::~DebugDialog()
{
    m_ui->widget->saveState();
    delete m_ui;
}

/** Save the current log to a log file in the Core::ISettings::UserResourcesPath */
bool DebugDialog::saveLogToFile()
{
    Core::ISettings *s = Core::ICore::instance()->settings();
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save log to..."),
                                                    s->path(Core::ISettings::UserDocumentsPath),
                                                    "*.txt");
    if (fileName.isEmpty())
        return false;
    if (QFileInfo(fileName).completeSuffix().isEmpty())
        fileName += ".txt";
    LOG(tkTr(Trans::Constants::SAVING_FILE_1).arg(fileName));
    if (Utils::saveStringToFile(Utils::Log::toString(), fileName)) {
        Utils::informativeMessageBox(tr("Log correctly saved"),
                                     tr("The log was correctly saved into<br />"
                                        "%1").arg(fileName),
                                     "",
                                     tr("Saving log"));
        return true;
    }
    return false;
}
