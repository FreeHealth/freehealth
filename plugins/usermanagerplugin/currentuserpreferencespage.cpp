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
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "currentuserpreferencespage.h"
#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/constants.h>
#include <usermanagerplugin/widgets/userviewer.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////  CurrentUserPreferencesPage  //////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
CurrentUserPreferencesPage::CurrentUserPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("CurrentUserPreferencesPage"); }

CurrentUserPreferencesPage::~CurrentUserPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString CurrentUserPreferencesPage::id() const { return objectName(); }
QString CurrentUserPreferencesPage::displayName() const { return tkTr(Trans::Constants::USER); }
QString CurrentUserPreferencesPage::category() const { return tkTr(Trans::Constants::GENERAL_PREFERENCES); }
QString CurrentUserPreferencesPage::title() const { return tkTr(Trans::Constants::USER_PREFERENCES); }

int CurrentUserPreferencesPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_MAIN;
}

void CurrentUserPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
    m_Widget->setDataToUi();
}

void CurrentUserPreferencesPage::apply()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void CurrentUserPreferencesPage::finish() { delete m_Widget; }

void CurrentUserPreferencesPage::checkSettingsValidity()
{
}

QWidget *CurrentUserPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new CurrentUserPreferencesWidget(parent);
    return m_Widget;
}

CurrentUserPreferencesWidget::CurrentUserPreferencesWidget(QWidget *parent) :
        QWidget(parent), m_Viewer(0)
{
    setupUi(this);
    m_Viewer = new UserViewer(this);
    userLayout->addWidget(m_Viewer);
    setDataToUi();
    // TODO: connect userchange
}

void CurrentUserPreferencesWidget::setDataToUi()
{
    // TODO: recode this
//    if (userModel()->hasCurrentUser())
//        m_Viewer->setCurrentUser(userModel()->data(Core::IUser::Uuid).toString());
}

void CurrentUserPreferencesWidget::saveToSettings(Core::ISettings *)
{
    if (userModel()->hasCurrentUser()) {
        this->setFocus();
        m_Viewer->submitChangesToModel();
        userModel()->submitRow(userModel()->currentUserIndex().row());
    }
}

void CurrentUserPreferencesWidget::writeDefaultSettings(Core::ISettings *)
{
}

void CurrentUserPreferencesWidget::changeEvent(QEvent *e)
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
