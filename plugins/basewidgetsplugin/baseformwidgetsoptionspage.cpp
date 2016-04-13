/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "baseformwidgetsoptionspage.h"
#include "ui_baseformwidgetsoptionspage.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_menus.h>

#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

using namespace BaseWidgets;
using namespace Internal;

namespace {
    const char *const SETTINGS_COMPACTVIEW_SPACING = "BaseFormWidgets/CompactView/Spacing";
    const char *const SETTINGS_COMPACTVIEW_MARGIN = "BaseFormWidgets/CompactView/Margin";
}

BaseFormSettingsWidget::BaseFormSettingsWidget(QWidget *parent): QWidget(parent)
{
    m_ui = new Ui::BaseFormWidgetsOptionsPage();
    m_ui->setupUi(this);
    m_ui->marginSpin->setValue( settings()->value(::SETTINGS_COMPACTVIEW_MARGIN, 0).toInt() );
    m_ui->spacingSpin->setValue( settings()->value(::SETTINGS_COMPACTVIEW_SPACING, 2).toInt() );
}

BaseFormSettingsWidget::~BaseFormSettingsWidget()
{
    if (m_ui)
        delete m_ui;
    m_ui=0;
}

void BaseFormSettingsWidget::applyChanges()
{
    settings()->setValue(::SETTINGS_COMPACTVIEW_MARGIN, m_ui->marginSpin->value());
    settings()->value(::SETTINGS_COMPACTVIEW_SPACING, m_ui->spacingSpin->value());
}

void BaseFormSettingsWidget::resetToDefaults()
{
    m_ui->marginSpin->setValue(0);
    m_ui->spacingSpin->setValue(2);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
BaseFormWidgetsOptionsPage::BaseFormWidgetsOptionsPage(QObject *parent) :
    Core::IOptionsPage(parent)
{
    setObjectName("BaseFormWidgetsOptionsPage");
}

BaseFormWidgetsOptionsPage::~BaseFormWidgetsOptionsPage()
{
}

QString BaseFormWidgetsOptionsPage::id() const
{
    return objectName();
}

QString BaseFormWidgetsOptionsPage::displayName() const
{
    return tr("Base Forms Widgets");
}

QString BaseFormWidgetsOptionsPage::category() const
{
    return Trans::ConstantTranslations::tkTr(Trans::Constants::PLUGINS_CATEGORY);
}

int BaseFormWidgetsOptionsPage::sortIndex() const
{
    return Core::Constants::OPTIONINDEX_FORMS;
}

void BaseFormWidgetsOptionsPage::resetToDefaults()
{
    if (m_Widget)
        m_Widget->resetToDefaults();
}

void BaseFormWidgetsOptionsPage::apply()
{
    if (m_Widget)
        m_Widget->applyChanges();
}

void BaseFormWidgetsOptionsPage::checkSettingsValidity()
{
    if (settings()->value(::SETTINGS_COMPACTVIEW_MARGIN,QVariant())==QVariant()) {
        settings()->setValue(::SETTINGS_COMPACTVIEW_MARGIN, 0);
    }

    if (settings()->value(::SETTINGS_COMPACTVIEW_SPACING,QVariant())==QVariant()) {
        settings()->setValue(::SETTINGS_COMPACTVIEW_SPACING, 2);
    }
}

void BaseFormWidgetsOptionsPage::finish()
{
    if (m_Widget)
        delete m_Widget;
}

QWidget *BaseFormWidgetsOptionsPage::createPage(QWidget *parent)
{
    m_Widget = new BaseFormSettingsWidget(parent);
    return m_Widget;
}
