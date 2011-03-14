/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
/////////////////////////////////  AccountUserOptionsPage  //////////////////////////////////////////
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
