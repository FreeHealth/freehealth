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
#include "accountpreferences.h"

#include <accountplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>

#include <texteditorplugin/texteditor.h>

#include <printerplugin/printer.h>
#include <printerplugin/textdocumentextra.h>

#include <QPixmap>

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  DrugsUserPage  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
AccountUserOptionsPage::AccountUserOptionsPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("AccountUserOptionsPage"); }

AccountUserOptionsPage::~AccountUserOptionsPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString AccountUserOptionsPage::id() const { return objectName(); }
QString AccountUserOptionsPage::name() const { return tkTr(Trans::Constants::USER); }
QString AccountUserOptionsPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void AccountUserOptionsPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDatasToUi();
}

void AccountUserOptionsPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void AccountUserOptionsPage::finish() { delete m_Widget; }

void AccountUserOptionsPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
//    defaultvalues.insert(DrugsDB::Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR, DrugsDB::Constants::S_DEF_AVAILABLEDOSAGESBACKGROUNGCOLOR);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *AccountUserOptionsPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new AccountUserWidget(parent);
    return m_Widget;
}

AccountUserWidget::AccountUserWidget(QWidget *parent) :
        QWidget(parent), header(0), footer(0), wm(0)
{
    header = new Print::TextDocumentExtra;
    footer = new Print::TextDocumentExtra;
    wm = new Print::TextDocumentExtra;
    setupUi(this);
    previewer = Print::Printer::previewer(this);
    userLayout->addWidget(previewer);
    setDatasToUi();
}

void AccountUserWidget::setDatasToUi()
{
    header = Print::TextDocumentExtra::fromXml(settings()->value(Account::Constants::S_USER_HEADER).toString());
    footer = Print::TextDocumentExtra::fromXml(settings()->value(Account::Constants::S_USER_FOOTER).toString());
    wm = Print::TextDocumentExtra::fromXml(settings()->value(Account::Constants::S_USER_WATERMARK).toString());

    previewer->setHeader(header);
    previewer->setFooter(footer);
    previewer->setWatermark(wm);
}

void AccountUserWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    Print::TextDocumentExtra *tmp = new Print::TextDocumentExtra;

    previewer->headerToPointer(tmp);
    s->setValue(Account::Constants::S_USER_HEADER, tmp->toXml());

    previewer->footerToPointer(tmp);
    s->setValue(Account::Constants::S_USER_FOOTER, tmp->toXml());

    previewer->watermarkToPointer(tmp);
    s->setValue(Account::Constants::S_USER_WATERMARK, tmp->toXml());
}

void AccountUserWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("AccountUserWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("AccountUserWidget"));
    s->sync();
}

void AccountUserWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
