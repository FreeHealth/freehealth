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
#ifndef USERPASSWORDDIALOG_H
#define USERPASSWORDDIALOG_H

#include <usermanagerplugin/usermanager_exporter.h>

#include <QDialog>

/**
 * \file userpassworddialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 18 Sept 2009
*/

namespace UserPlugin {
namespace Internal {
namespace Ui {
class  UserPasswordDialog;
}  // End ui
}  // End Internal
class UserModel;

class USER_EXPORT UserPasswordDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(UserPasswordDialog)

public:
    explicit UserPasswordDialog(const QString &actualCryptedPassword, QWidget *parent = 0);

    void changeTitle(const QString &title);
    bool canGetNewPassword() const;
    QString cryptedPassword() const;
    QString clearPassword() const;

    bool applyChanges(UserModel *model, int userRow) const;

private Q_SLOTS:
    void checkControlPassword(const QString &text);
    void checkNewPassword(const QString &text);
    void accept();

private:
    Internal::Ui::UserPasswordDialog *m_ui;
    bool m_AllIsGood;
    QString m_ActualPass;
    QString m_CryptedNewPass;
    QDialog * m_Parent;
};

}  // End UserPlugin

#endif // USERPASSWORDDIALOG_H
