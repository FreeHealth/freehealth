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
#ifndef IDENTITY_INTERNAL_PASSWORDWIDGET_H
#define IDENTITY_INTERNAL_PASSWORDWIDGET_H

#include <utils/widgets/detailswidget.h>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

/**
 * \file passwordwidget.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 22 Jan 2013
*/

namespace Identity {
namespace Internal {
class PasswordWidgetPrivate;

class PasswordWidget : public Utils::DetailsWidget
{
    Q_OBJECT
    Q_PROPERTY(QString uncryptedPassword READ uncryptedPassword WRITE setUncryptedPassword RESET resetUncryptedPassword NOTIFY uncryptedPasswordChanged)
    Q_PROPERTY(QString cryptedPassword READ cryptedPassword WRITE setCryptedPassword RESET resetCryptedPassword NOTIFY cryptedPasswordChanged)

public:
    explicit PasswordWidget(QWidget *parent = 0);
    ~PasswordWidget();
    bool initialize();
    bool isPasswordValidAndConfirmed();
    void setReadOnly(bool readonly);
    QLineEdit *loginEditor() const;

Q_SIGNALS:
    void uncryptedPasswordChanged(const QString &password);
    void cryptedPasswordChanged(const QString &password);

public Q_SLOTS:
    void clear();

    void resetUncryptedPassword();
    QString uncryptedPassword() const;
    void setUncryptedPassword(const QString &password);

    void resetCryptedPassword();
    QString cryptedPassword() const;
    void setCryptedPassword(const QString &password);

private Q_SLOTS:
    void onChangeOrSetPasswordClicked();
    void onLoginChanged(const QString &login);

private:
    Internal::PasswordWidgetPrivate *d;
};

} // namespace Internal
} // namespace Identity

#endif // IDENTITY_INTERNAL_PASSWORDWIDGET_H

