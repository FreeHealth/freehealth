/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class Calendar::BasicItemEditionDialog
  Basic editing dialog of Calendar::CalendarItem. This dialog can be extended with the
  usage of Calendar::ICalendarItemDataWidget objects. \n
  There are two ways to add these king of objects to the dialog:
    - using the plugin manager object pool,
  \code
  // User adding an object to the plugin manager pool
  ExtensionSystem::PluginManager::instance()->addObject(yourItemDataWidgetEditorPointer);
  // Then each time the Calendar::BasicItemEditionDialog is called your extended widget will be included in the dialog
  \endcode

    - add objects before setting up the Calendar::CalendarItem.
  \code
  // User adding an object
  Calendar::BasicItemEditionDialog dlg(yourAbstractCalendarModelPointer);
  dlg.addCalendarDataWidget(yourItemDataWidgetEditorPointer); // optionnal
  dlg.init(yourCalendarItem);
  if (dlg.exec()==QDialog::Accepted) {
      // retrieve updated item
      yourCalendarItem = dlg.item();
  }
  \endcode

  You must inform the Calendar::AbstractCalendarModel to use. The Calendar::CalendarItem must
  be a newly created item or an extracted item from the Calendar::AbstractCalendarModel.
*/

#include "basic_item_edition_dialog.h"
#include "calendar_item.h"
#include "icalendaritemdatawidget.h"
#include "abstract_calendar_model.h"

#include <extensionsystem/pluginmanager.h>
#include <translationutils/constanttranslations.h>
#include <utils/global.h>

#include "ui_basic_item_edition_dialog.h"

#include <QDebug>

using namespace Calendar;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }

BasicItemEditionDialog::BasicItemEditionDialog(AbstractCalendarModel *model, QWidget *parent) :
        QDialog(parent),
        m_Model(model),
        ui(new Internal::Ui::BasicItemEditionDialog)
{
    Q_ASSERT(model);
    ui->setupUi(this);
    m_moreInfo = ui->buttonBox->addButton(tkTr(Trans::Constants::MORE_INFORMATIONS), QDialogButtonBox::HelpRole);
    connect(m_moreInfo, SIGNAL(clicked()), this, SLOT(showMoreTriggered()));

    ui->viewer->setModel(model);

    // Ask plugin manager for extended widgets
    QList<ICalendarItemDataWidget*> extended = pluginManager()->getObjects<ICalendarItemDataWidget>();
    for(int i = 0; i < extended.count(); ++i)
        addCalendarDataWidget(extended.at(i));

    adjustSize();
    Utils::centerWidget(this);
}

BasicItemEditionDialog::~BasicItemEditionDialog()
{
    delete ui;
}

void BasicItemEditionDialog::done(int r)
{
    qWarning() << "DONE";
    if (r==QDialog::Accepted) {
        ui->viewer->submit();
        m_Model->submit(m_Item);
    }
    QDialog::done(r);
}

/**
  Add specific widgets to the editor using the Calendar::ICalendarItemDataWidget interface.
  You must set the Calendar::CalendarItem AFTER you have added ALL your Calendar::ICalendarItemDataWidget.
*/
void BasicItemEditionDialog::addCalendarDataWidget(Calendar::ICalendarItemDataWidget *dataWidget)
{
    ui->viewer->addCalendarDataWidget(dataWidget);
    ui->viewer->adjustSize();
}

/** Initialize the dialog with the specified Calendar::CalendarItem \e item. */
void BasicItemEditionDialog::init(const CalendarItem &item)
{
    m_Item = item;
    ui->viewer->setCalendarEvent(item);
}

void BasicItemEditionDialog::showMoreTriggered()
{
    ui->viewer->toogleExtraInformations();
    adjustSize();
    Utils::centerWidget(this);
}
