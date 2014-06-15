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
 *   This code is inspired of the Qt example : Text Edit                   *
 *   Adaptations to FreeMedForms and improvments by : Eric Maeker, MD      *
 *   eric.maeker@gmail.com                                                   *
 ***************************************************************************/

/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "texteditordialog.h"
#include "ui_texteditordialog.h"

#include <QDebug>

using namespace Editor;
using namespace Editor::Internal;

namespace Editor {
namespace Internal {
class TextEditorDialogPrivate
{
public:
    TextEditorDialogPrivate(TextEditorDialog *parent) :
        ui(new Ui::TextEditorDialogWidget),
        _saveOnClose(TextEditorDialog::AskUserToSaveWhenClose),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~TextEditorDialogPrivate()
    {
        delete ui;
    }

public:
    QString _currentFileName;
    Ui::TextEditorDialogWidget *ui;
    int _saveOnClose;

private:
    TextEditorDialog *q;
};
}  // End Internal
}  // End Editor


TextEditorDialog::TextEditorDialog(QWidget *parent) :
    QDialog(parent),
    d(new Internal::TextEditorDialogPrivate(this))
{
    d->ui->setupUi(this);
}

TextEditorDialog::~TextEditorDialog()
{
    if (d)
        delete d;
    d = 0;
}

void TextEditorDialog::done(int r)
{
    if (d->_saveOnClose == AlwaysSaveWhenClose || d->_saveOnClose == AskUserToSaveWhenClose)
        d->ui->textEdit->saveAs();
    QDialog::done(r);
}

void TextEditorDialog::setSaveOnCloseMethod(SaveOnCloseMethod method)
{
    d->_saveOnClose = method;
}

void TextEditorDialog::readFile(const QString &absPath)
{
    d->ui->textEdit->setCurrentFileName(absPath);
}

QString TextEditorDialog::currentEditingFile() const
{
    return d->ui->textEdit->currentFileName();
}
