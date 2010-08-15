/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
   \class Core::SimpleTextDialog
   \brief Show a dialog that presents a text (HTML or plain text) that can be easily printed.
*/

#include "simpletextdialog.h"
#include "ui_simpletextdialog.h"

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

inline static Core::ITheme *theme() {return Core::ICore::instance()->theme();}
inline static Core::ISettings *settings() {return Core::ICore::instance()->settings();}
inline static Core::IUser *user() {return Core::ICore::instance()->user();}
inline static Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

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

    QPushButton *printButton = new QPushButton(tkTr(Trans::Constants::FILEPRINT_TEXT), this);
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
        Utils::Log::addError(this, "No IDocumentPrinter found");
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

