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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SCRIPT_INTERNAL_SCRIPTWRITERDIALOG_H
#define SCRIPT_INTERNAL_SCRIPTWRITERDIALOG_H

#include <QDialog>

/**
 * \file scriptwriterdialog.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 2012-12-30
*/

namespace Script {
namespace Internal {
class ScriptWriterDialogPrivate;

class ScriptWriterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScriptWriterDialog(QWidget *parent = 0);
    ~ScriptWriterDialog();
    bool initialize();

Q_SIGNALS:

public Q_SLOTS:
    void onReadFileTriggered();
    void onSaveFileTriggered();
    void onExecuteScriptTriggered();

private:
    Internal::ScriptWriterDialogPrivate *d;
};

} // namespace Internal
} // namespace Script

#endif // SCRIPT_INTERNAL_SCRIPTWRITERDIALOG_H

