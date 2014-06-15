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
 *       NAME <MAIL@ADDRESS.COM>                                           *
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

#include <QDebug>

using namespace Core;
using namespace Core::Internal;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

SettingsDialog::SettingsDialog(QWidget *parent, const QString &categoryId, const QString &pageId) :
        QDialog(parent), m_applied(false)
{
    Q_UNUSED(categoryId);
    Q_UNUSED(pageId);
    // TODO: allow users to open SettingsDialog with a specific page
    m_ui = new Ui::SettingsDialog();
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    m_ui->setupUi(this);
    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    m_ui->buttonBox->button(QDialogButtonBox::Apply)->setToolTip(tr("Apply all changes (all pages)"));
    m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setToolTip(tr("Reset the current page to defaults"));

    connect(m_ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
    connect(m_ui->buttonBox->button(QDialogButtonBox::Help), SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));

    m_pages = ExtensionSystem::PluginManager::instance()->getObjects<IOptionsPage>();

    m_ui->widget->setPages<IOptionsPage>(m_pages);
    m_ui->widget->setSettingKey("Dialogs/Settings");
    m_ui->widget->setupUi();

    // resize and center window
    Utils::resizeAndCenter(this);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::apply()
{
    foreach (IOptionsPage *page, m_pages)
        page->apply();
    settings()->sync();
    m_applied = true;
}

void SettingsDialog::restoreDefaults()
{
    IOptionsPage *page = qobject_cast<IOptionsPage*>(m_ui->widget->currentPage());
    if (page) {
        page->resetToDefaults();
        settings()->sync();
    }
}

void SettingsDialog::showHelp()
{
    IOptionsPage *page = qobject_cast<IOptionsPage*>(m_ui->widget->currentPage());
    if (page)
        page->resetToDefaults();
    HelpDialog::showPage(page->helpPage());
//    QTreeWidgetItem *item = m_ui->pageTree->currentItem();
//    const PageData &data = item->data(0, Qt::UserRole).value<PageData>();
//    int index = data.index;
//    HelpDialog::showPage(m_pages.at(index)->helpPage());
}

void SettingsDialog::done(int r)
{
    m_ui->widget->saveState();
    m_ui->buttonBox->setFocus();
    if (r==QDialog::Accepted) {
        m_applied = true;
        foreach (IOptionsPage *page, m_pages) {
            page->apply();
            page->finish();
        }
    } else { // QDialog::Rejected
        foreach (IOptionsPage *page, m_pages)
            page->finish();
    }
    QDialog::done(r);
}
