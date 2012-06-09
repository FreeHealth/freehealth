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

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace Alert;

AlertItemEditorDialog::AlertItemEditorDialog(QWidget *parent) :
    QDialog(parent),
    _editor(0)
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    setLayout(lay);
    _editor = new AlertItemEditorWidget(this);
    lay->addWidget(_editor);
    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Reset | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    lay->addWidget(box);
    connect(box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(box, SIGNAL(rejected()), this, SLOT(reject()));
    QPushButton *but = box->button(QDialogButtonBox::Reset);
    connect(but, SIGNAL(clicked()), this, SLOT(reset()));
}

void AlertItemEditorDialog::setEditableParams(EditableParams params)
{

}

void AlertItemEditorDialog::setAlertItem(const AlertItem &item)
{
    _editor->setAlertItem(item);
}

void AlertItemEditorDialog::reset()
{
    _editor->reset();
}

bool AlertItemEditorDialog::submit(AlertItem &item)
{
    _editor->submit(item);
}
