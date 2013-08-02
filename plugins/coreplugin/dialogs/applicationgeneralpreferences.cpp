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
#include "applicationgeneralpreferences.h"

#include "ui_applicationgeneralpreferences.h"

#include <utils/log.h>
#include <utils/updatechecker.h>
#include <translationutils/constants.h>
#include <translationutils/trans_spashandupdate.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/theme.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>

using namespace Trans::ConstantTranslations;
using namespace Core;
using namespace Internal;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

ApplicationGeneralPreferencesWidget::ApplicationGeneralPreferencesWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ApplicationGeneralPreferencesWidget)
{
    setObjectName("ApplicationGeneralPreferencesWidget");
    ui->setupUi(this);
    ui->updateCheckingCombo->addItems(Trans::ConstantTranslations::checkUpdateLabels());
    setDataToUi();
}

void ApplicationGeneralPreferencesWidget::setDataToUi()
{
    ui->autoSave->setChecked(settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING).toBool());
    ui->updateCheckingCombo->setCurrentIndex(settings()->value(Utils::Constants::S_CHECKUPDATE).toInt());
    ui->virtualDataCheck->setChecked(settings()->value(Core::Constants::S_ALLOW_VIRTUAL_DATA).toBool());
}

void ApplicationGeneralPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, ui->autoSave->isChecked());
    s->setValue(Utils::Constants::S_CHECKUPDATE, ui->updateCheckingCombo->currentIndex());
    s->setValue(Core::Constants::S_ALLOW_VIRTUAL_DATA, ui->virtualDataCheck->isChecked());
}

void ApplicationGeneralPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    //    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("ApplicationGeneralPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("FreeDiamsMainWindow"));
    s->setValue(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, true);
    s->setValue(Utils::Constants::S_CHECKUPDATE, Trans::Constants::CheckUpdate_AtStartup);
    s->setValue(Core::Constants::S_ALLOW_VIRTUAL_DATA, true);

    s->sync();
}

void ApplicationGeneralPreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

ApplicationGeneralPreferencesPage::ApplicationGeneralPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("ApplicationGeneralPreferencesPage"); }

ApplicationGeneralPreferencesPage::~ApplicationGeneralPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString ApplicationGeneralPreferencesPage::id() const { return objectName(); }
QString ApplicationGeneralPreferencesPage::displayName() const
{
    return tkTr(Trans::Constants::GENERAL).remove("&");
}

QString ApplicationGeneralPreferencesPage::title() const
{
    return tkTr(Trans::Constants::GENERAL_PREFERENCES).remove("&");
}

QString ApplicationGeneralPreferencesPage::category() const
{
    return tkTr(Trans::Constants::GENERAL_PREFERENCES).remove("&");
}

int ApplicationGeneralPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_MAIN;
}

void ApplicationGeneralPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void ApplicationGeneralPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void ApplicationGeneralPreferencesPage::finish() { delete m_Widget; }

void ApplicationGeneralPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, true);
    defaultvalues.insert(Utils::Constants::S_CHECKUPDATE, Trans::Constants::CheckUpdate_AtStartup);
    defaultvalues.insert(Core::Constants::S_ALLOW_VIRTUAL_DATA, true);
    defaultvalues.insert("Dialogs/Settings/LastPage", id());
    defaultvalues.insert("Dialogs/Settings/LastCategory", category());

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
}

QWidget *ApplicationGeneralPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new Internal::ApplicationGeneralPreferencesWidget(parent);
    return m_Widget;
}

