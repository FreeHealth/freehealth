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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TEMPLATESEDITDIALOG_H
#define TEMPLATESEDITDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Templates {
namespace Internal {
class TemplatesEditDialogPrivate;
}
class TemplatesModel;

class TemplatesEditDialog : public QDialog {
    Q_OBJECT
public:
    TemplatesEditDialog(QWidget *parent = 0);
    ~TemplatesEditDialog();

    void setModel(Templates::TemplatesModel *model);
    void setModelIndex(const QModelIndex &index);

protected:
    void done(int r);
    void changeEvent(QEvent *e);

private:
    void refreshComboCategory();
private Q_SLOTS:
    void editContent();

private:
    Internal::TemplatesEditDialogPrivate *d;
};

}  // End namespace Templates

#endif // TEMPLATESEDITDIALOG_H
