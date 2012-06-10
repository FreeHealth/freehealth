/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
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

void AlertItemEditorDialog::setEditableParams(EditableParams params)
{
    ui->editor->setLabelVisible(params & Label);
    ui->editor->setCategoryVisible(params & Category);
    ui->editor->setDescriptionVisible(params & Description);
    ui->editor->setRelationVisible(params & Relation);
    ui->editor->setViewTypeVisible(params & ViewType);
    ui->editor->setContentTypeVisible(params & ContentType);
    ui->editor->setPriorityVisible(params & Priority);
    ui->editor->setOverridingCommentVisible(params & OverrideNeedsComment);
    if (!(params & Timing))
        ui->editor->hideTimingTab();
    if (!(params & CSS))
        ui->editor->hideStyleSheetTab();
    if (!(params & ExtraXml))
        ui->editor->hideExtraXmlTab();
}

void AlertItemEditorDialog::setAlertItem(const AlertItem &item)
{
    ui->editor->setAlertItem(item);
}

void AlertItemEditorDialog::reset()
{
    ui->editor->reset();
}

bool AlertItemEditorDialog::submit(AlertItem &item)
{
    return ui->editor->submit(item);
}
