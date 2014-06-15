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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Alert::AlertItemEditorDialog
  Editor for any Alert::AlertItem. You can define what values are editable
  in the dialog using the Alert::AlertItemEditorDialog::EditableParam and
  setEditableParams().\n
  When the dialog is accepted/rejected, nothing is submitted in the editing
  item. You must call the submit() procedure.\n
  \code
    // Example of code
    AlertItemEditorDialog dlg(this);
    dlg.setAlertItem(item);
    if (dlg.exec() == QDialog::Accepted)
        dlg.submit(item); // edited values are commited in the item
  \endcode

*/

#include "alertitemeditordialog.h"
#include "alertitemeditorwidget.h"
#include "ui_alertitemeditordialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>

using namespace Alert;

AlertItemEditorDialog::AlertItemEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlertItemEditorDialog)
{
    ui->setupUi(this);
    setWindowTitle(ui->title->text());
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QPushButton *but = ui->buttonBox->button(QDialogButtonBox::Reset);
    connect(but, SIGNAL(clicked()), this, SLOT(reset()));
}

AlertItemEditorDialog::~AlertItemEditorDialog()
{
    delete ui;
}

/**
 * Defines the user editable params of the alert.
 */
void AlertItemEditorDialog::setEditableParams(EditableParams params)
{
    ui->editor->setLabelVisible(params.testFlag(Label));
    ui->editor->setCategoryVisible(params.testFlag(Category));
    ui->editor->setDescriptionVisible(params.testFlag(Description));
    ui->editor->setIconVisible(params.testFlag(Icon));
    ui->editor->setRelationVisible(params.testFlag(Relation));
    ui->editor->setViewTypeVisible(params.testFlag(ViewType));
    ui->editor->setContentTypeVisible(params.testFlag(ContentType));
    ui->editor->setPriorityVisible(params.testFlag(Priority));
    ui->editor->setOverridingCommentVisible(params.testFlag(OverrideNeedsComment));
    if (!(params.testFlag(Timing)))
        ui->editor->hideTimingTab();
    if (!(params.testFlag(CSS)))
        ui->editor->hideStyleSheetTab();
    if (!(params.testFlag(ExtraXml)))
        ui->editor->hideExtraXmlTab();
    if (!(params.testFlag(Scripts)))
        ui->editor->hideScriptsTab();
}

/** Define the initial alert to use in the editor */
void AlertItemEditorDialog::setAlertItem(const AlertItem &item)
{
    ui->editor->setAlertItem(item);
}

/**
 * Reset the editor to the initial alert
 * \sa setAlertItem()
*/
void AlertItemEditorDialog::reset()
{
    ui->editor->reset();
}

/**
 * Submit the content of the editing widget to the item.
 */
bool AlertItemEditorDialog::submit(AlertItem &item)
{
    return ui->editor->submit(item);
}
