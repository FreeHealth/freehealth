/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#include "virtualdatabasecreator.h"
#include "ui_virtualdatabasecreator.h"

#include <accountbaseplugin/accountbase.h>
#include <accountplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

using namespace Account;
using namespace Account::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }



VirtualDatabaseCreatorPage::VirtualDatabaseCreatorPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0) { setObjectName("VirtualDatabaseCreatorPage"); }

VirtualDatabaseCreatorPage::~VirtualDatabaseCreatorPage()
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = 0;
}

QString VirtualDatabaseCreatorPage::id() const { return objectName(); }
QString VirtualDatabaseCreatorPage::name() const { return tr("Virtual database creator"); }
QString VirtualDatabaseCreatorPage::category() const { return tkTr(Trans::Constants::ACCOUNTANCY); }

void VirtualDatabaseCreatorPage::resetToDefaults()
{
}

void VirtualDatabaseCreatorPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
}

void VirtualDatabaseCreatorPage::finish() { delete m_Widget; }

void VirtualDatabaseCreatorPage::checkSettingsValidity()
{
}

QWidget *VirtualDatabaseCreatorPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new VirtualDatabaseCreator(parent);
    return m_Widget;
}





VirtualDatabaseCreator::VirtualDatabaseCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::VirtualDatabaseCreator)
{
    ui->setupUi(this);
}

VirtualDatabaseCreator::~VirtualDatabaseCreator()
{
    delete ui;
}

void VirtualDatabaseCreator::setDatasToUi()
{
}

void VirtualDatabaseCreator::saveToSettings(Core::ISettings *sets)
{
    Q_UNUSED(sets);
}

void VirtualDatabaseCreator::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("AccountUserWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("VirtualDatabaseCreator"));
    s->sync();
}

void VirtualDatabaseCreator::on_populate_clicked()
{
}

void VirtualDatabaseCreator::changeEvent(QEvent *e)
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




