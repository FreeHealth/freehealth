/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
   \class Core::SimpleTextDialog
   \brief Show a dialog that presents a text (HTML or plain text) that can be easily printed.
*/

#include "simpletextdialog.h"
#include "ui_simpletextdialog.h"

#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>

#include <coreplugin/ipatient.h>
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/dialogs/helpdialog.h>
#include <coreplugin/idocumentprinter.h>

#include <utils/log.h>
#include <utils/global.h>

#include <extensionsystem/pluginmanager.h>

#include <QPushButton>
#include <QDialogButtonBox>

using namespace Core;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

SimpleTextDialog::SimpleTextDialog(const QString &title, const QString &zoomSettingKey, QWidget *parent) :
    QDialog(parent),
    ui(new Internal::Ui::SimpleTextDialog),
    m_Key(zoomSettingKey),
    m_Papers(Core::IDocumentPrinter::Papers_Generic_User),
    m_Duplicata(false)
{
    ui->setupUi(this);
    setObjectName("SimpleTextDialog");
//    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_GroupLeader);
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);

    setWindowTitle(title);
    ui->label->setText(title);

    QPushButton *printButton = new QPushButton(tkTr(Trans::Constants::FILEPRINT_TEXT).remove("&"), this);
    printButton->setIcon(theme()->icon(Core::Constants::ICONPRINT));
    ui->buttonBox->addButton(printButton, QDialogButtonBox::ActionRole);

    ui->zoomIn->setIcon(theme()->icon(Core::Constants::ICONFONTBIGGER));
    ui->zoomOut->setIcon(theme()->icon(Core::Constants::ICONFONTSMALLER));

    connect(printButton, SIGNAL(clicked()), this, SLOT(print()));
    connect(ui->buttonBox->button(QDialogButtonBox::Help), SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(ui->zoomIn, SIGNAL(clicked()), this, SLOT(zoomIn()));
    connect(ui->zoomOut, SIGNAL(clicked()), this, SLOT(zoomOut()));

    if (!m_Key.isEmpty()) {
        m_Zoom = settings()->value(m_Key, 1).toInt();
        ui->textBrowser->zoomIn(m_Zoom);
    }

    if (parent)
        Utils::resizeAndCenter(this, parent);
    else
        Utils::resizeAndCenter(this, Core::ICore::instance()->mainWindow());
}

SimpleTextDialog::~SimpleTextDialog()
{
    if (!m_Key.isEmpty()) {
        settings()->setValue(m_Key, m_Zoom);
        settings()->sync();
    }
    delete ui;
}

void SimpleTextDialog::setHtml(const QString &html)
{
    ui->textBrowser->setHtml(html);
}

void SimpleTextDialog::setPlainText(const QString &text)
{
    ui->textBrowser->setPlainText(text);
}

void SimpleTextDialog::showHelp()
{
    if (!m_HelpUrl.isEmpty())
        Core::HelpDialog::showPage(m_HelpUrl);
}

void SimpleTextDialog::print()
{
    Core::IDocumentPrinter *p = printer();
    if (!p) {
        LOG_ERROR("No IDocumentPrinter found");
        return;
    }

    p->clearTokens();
    QHash<QString, QVariant> tokens;
    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, this->windowTitle());
    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);

    p->print(ui->textBrowser->toHtml(), m_Papers, m_Duplicata);
}

void SimpleTextDialog::zoomIn()
{
    ui->textBrowser->zoomIn(2);
    m_Zoom += 2;
}

void SimpleTextDialog::zoomOut()
{
    ui->textBrowser->zoomOut(2);
    m_Zoom -= 2;
}

void SimpleTextDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

