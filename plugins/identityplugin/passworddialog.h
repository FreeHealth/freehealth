/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IDENTITY_PASSWORDDIALOG_H
#define IDENTITY_PASSWORDDIALOG_H

#include <QDialog>

/**
 * \file passworddialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.2
 * \date 22 Jan 2013
*/

namespace Identity {
namespace Internal {
namespace Ui {
class  PasswordDialog;
}  // namespace ui

class PasswordDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(PasswordDialog)

public:
    explicit PasswordDialog(QWidget *parent = 0);

    void setOldCryptedPassword(const QString &crypted);

    bool canGetNewPassword() const;
    QString cryptedPassword() const;
    QString uncryptedPassword() const;

//    bool applyChanges(UserModel *model, int userRow) const;

private Q_SLOTS:
    void checkControlPassword(const QString &text);
    void checkNewPassword(const QString &text);

private:
    void done(int result);

private:
    Internal::Ui::PasswordDialog *m_ui;
    bool m_AllIsGood;
    QString m_OldCryptedPass;
    QString m_CryptedNewPass;
    QDialog *m_Parent;
};
}  // namespace Internal
}  // namespace UserPlugin

#endif // IDENTITY_PASSWORDDIALOG_H
