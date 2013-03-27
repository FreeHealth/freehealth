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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Alert::Internal::AlertItemScriptEditor
 * Allow user to edit the Alert::AlertItem scripts (Alert::AlertScript).
 * Modifications are not automatically submitted, you must call submit()
 * before retrieving scripts with scripts().
*/

#include "alertitemscripteditor.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include "ui_alertitemscripteditor.h"

#include <QMenu>
#include <QDebug>

using namespace Alert;
using namespace Internal;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

AlertItemScriptEditor::AlertItemScriptEditor(QWidget *parent) :
    QWidget(parent),
    _previousIndex(-1),
    ui(new Ui::AlertItemScriptEditor)
{
    ui->setupUi(this);
    layout()->setMargin(0);
    ui->add->setIcon(theme()->icon(Core::Constants::ICONADD));
    ui->remove->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    _menu = new QMenu(this);
    for(int i=0; i < 1000; ++i) {
        QString type = AlertScript::typeToString(AlertScript::ScriptType(i));
        if (type.isEmpty())
            break;
        QAction *a = new QAction(_menu);
        a->setText(type);
        a->setData(i);
        _menu->addAction(a);
    }
    ui->add->setMenu(_menu);
    connect(_menu, SIGNAL(triggered(QAction*)), this, SLOT(addAction(QAction*)));
}

AlertItemScriptEditor::~AlertItemScriptEditor()
{
    delete ui;
}

/** Clear the editor */
void AlertItemScriptEditor::clear()
{
    ui->types->clear();
    ui->plainTextEdit->clear();
}

/** Define the Alert::AlertItem to edit */
void AlertItemScriptEditor::setAlertItem(const AlertItem &alert)
{
    _scriptsCache = alert.scripts().toList();
    _scripts = _scriptsCache;
    refreshScriptCombo();
}

void AlertItemScriptEditor::refreshScriptCombo()
{
    ui->types->disconnect(this);
    ui->types->clear();
    qSort(_scripts);

    for(int i=0; i<_scripts.count(); ++i) {
        ui->types->addItem(AlertScript::typeToString(_scripts.at(i).type()));
    }

    foreach(QAction *a, _menu->actions()) {
        a->setEnabled(true);
        for(int i=0; i<_scripts.count(); ++i) {
            if (a->data().toInt() == _scripts.at(i).type()) {
                a->setEnabled(false);
                break;
            }
        }
    }

    connect(ui->types, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypesSelected(int)));
    onTypesSelected(0);
}

/** Return the scripts. If you want to get the user's changes, call submit() before. */
QVector<AlertScript> AlertItemScriptEditor::scripts() const
{
    return _scriptsCache.toVector();
}

/** Submit all user changes to the scripts */
void AlertItemScriptEditor::submit()
{
    // Submit current editing
    if (_scripts.count()) {
        _scripts[ui->types->currentIndex()].setScript(ui->plainTextEdit->toPlainText());
    }
    _scriptsCache.clear();
    _scriptsCache = _scripts;
}

void AlertItemScriptEditor::onTypesSelected(int index)
{
    if (_scripts.count() <= index)
        return;

    if (_previousIndex != -1) {
        // keep changes to the _scripts var
        _scripts[_previousIndex].setScript(ui->plainTextEdit->toPlainText());
    }
    _previousIndex = index;
    // Populate the texteditor
    ui->plainTextEdit->setPlainText(_scripts.at(index).script());
}

void AlertItemScriptEditor::addAction(QAction *a)
{
    // Only one type of script per alert
    AlertScript::ScriptType type = AlertScript::ScriptType(a->data().toInt());
    for(int i=0; i < _scripts.count(); ++i) {
        if (_scripts.at(i).type()==type)
            return;
    }

    // Create a new alert script
    AlertScript script;
    script.setType(type);
    _scripts.append(script);

    // refresh combo
    refreshScriptCombo();

    // Select the newly created
    for(int i=0; i < _scripts.count(); ++i) {
        if (_scripts.at(i).type()==type)
            ui->types->setCurrentIndex(i);
    }
}
