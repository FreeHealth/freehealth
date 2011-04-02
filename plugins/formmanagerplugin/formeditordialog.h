/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef FORMEDITORDIALOG_H
#define FORMEDITORDIALOG_H

#include <QDialog>
QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
QT_END_NAMESPACE

namespace Form {
class EpisodeModel;
namespace Ui {
    class FormEditorDialog;
}

class FormEditorDialog : public QDialog
{
    Q_OBJECT

public:
    enum EditionMode {
        ViewOnly      = 0x000,
        AllowAddition = 0x001,
        AllowEdition  = 0x002,
        AllowRemoval  = 0x004,
        DefaultMode   = ViewOnly
    };
    Q_DECLARE_FLAGS(EditionModes, EditionMode)

    explicit FormEditorDialog(EpisodeModel *model, EditionModes mode = DefaultMode, QWidget *parent = 0);
    ~FormEditorDialog();

protected:
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void on_addForm_clicked();

private:
    Ui::FormEditorDialog *ui;
    QSortFilterProxyModel *proxyModel;
};

}  // End namespace Form
Q_DECLARE_OPERATORS_FOR_FLAGS(Form::FormEditorDialog::EditionModes);

#endif // FORMEDITORDIALOG_H
