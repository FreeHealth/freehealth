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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Identity::Internal::PasswordWidget
 */

#include "passwordwidget.h"
#include "passworddialog.h"
#include "identityconstants.h"
#include "ui_passwordwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>
#include <utils/passwordandlogin.h>
#include <utils/widgets/qbuttonlineedit.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>

#include <QFormLayout>

#include <QDebug>

using namespace Identity;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

namespace Identity {
namespace Internal {
class PasswordWidgetPrivate
{
public:
    PasswordWidgetPrivate(PasswordWidget *parent) :
        ui(0),
        q(parent)
    {
    }

    ~PasswordWidgetPrivate()
    {
        delete ui;
    }

    // Create the content widget
    QWidget *createWidget()
    {
        Q_ASSERT(!ui);
        QWidget *w = new QWidget(q);
        ui = new Ui::PasswordWidget;
        ui->setupUi(w);
        ui->login->setIcon(theme()->icon(Core::Constants::ICONEYES));
        return w;
    }

    void retranslatePasswordButton()
    {
        if (_cachedCryptedPassword.isEmpty() && _cachedUncryptedPassword.isEmpty())
            ui->changePassword->setText(tkTr(Trans::Constants::SET_PASSWORD));
        else
            ui->changePassword->setText(tkTr(Trans::Constants::CHANGE_PASSWORD));
    }

public:
    Ui::PasswordWidget *ui;
    QString _cachedUncryptedPassword, _cachedCryptedPassword;
    Utils::PasswordCrypter crypter;

private:
    PasswordWidget *q;
};
} // namespace Internal
} // namespace Identity


/*! Constructor of the Identity::Internal::PasswordWidget class */
PasswordWidget::PasswordWidget(QWidget *parent) :
    Utils::DetailsWidget(parent),
    d(new PasswordWidgetPrivate(this))
{
    setSummaryText(tkTr(Constants::PASSWORD_SUMMARY_NOLOGIN_NOPASSWORD));
    setWidget(d->createWidget());
    setState(Expanded);
    connect(d->ui->changePassword, SIGNAL(clicked()), this, SLOT(onChangeOrSetPasswordClicked()));
    connect(d->ui->login, SIGNAL(textChanged(QString)), this, SLOT(onLoginChanged(QString)));
}

/*! Destructor of the Identity::Internal::PasswordWidget class */
PasswordWidget::~PasswordWidget()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PasswordWidget::initialize()
{
    return true;
}

bool PasswordWidget::isPasswordValidAndConfirmed()
{
    return (!d->_cachedCryptedPassword.isEmpty() || !d->_cachedUncryptedPassword.isEmpty());
}

void PasswordWidget::setReadOnly(bool readonly)
{
    d->ui->login->setReadOnly(readonly);
    d->ui->changePassword->setEnabled(!readonly);
}

// used in mappers only
QLineEdit *PasswordWidget::loginEditor() const
{
    return d->ui->login;
}

void PasswordWidget::clear()
{
    d->ui->login->clear();
    d->_cachedCryptedPassword.clear();
    d->_cachedUncryptedPassword.clear();
    d->retranslatePasswordButton();
}

void PasswordWidget::resetUncryptedPassword()
{
    d->ui->passwordStatus->clear();
    d->_cachedUncryptedPassword.clear();
    d->retranslatePasswordButton();
}

QString PasswordWidget::uncryptedPassword() const
{
    return d->_cachedUncryptedPassword;
}

void PasswordWidget::setUncryptedPassword(const QString &password)
{
    d->ui->passwordStatus->setText(tkTr(Constants::UNCRYPTED_PASSWORD_AVAILABLE));
    d->_cachedUncryptedPassword = password;
    d->retranslatePasswordButton();
}

void PasswordWidget::resetCryptedPassword()
{
    d->ui->passwordStatus->clear();
    d->_cachedCryptedPassword.clear();
    d->retranslatePasswordButton();
}

QString PasswordWidget::cryptedPassword() const
{
    return d->_cachedCryptedPassword;
}

void PasswordWidget::setCryptedPassword(const QString &password)
{
    d->ui->passwordStatus->setText(tkTr(Constants::CRYPTED_PASSWORD_AVAILABLE));
    d->_cachedCryptedPassword = password;
    d->retranslatePasswordButton();
}

void PasswordWidget::onChangeOrSetPasswordClicked()
{
    // Open a dialog
    PasswordDialog dlg(this);
    // Feed old password
    if (!d->_cachedCryptedPassword.isEmpty()) {
        dlg.setOldCryptedPassword(d->_cachedCryptedPassword);
    } else if (!d->_cachedUncryptedPassword.isEmpty()) {
        dlg.setOldCryptedPassword(d->crypter.cryptPassword(d->_cachedUncryptedPassword));
    }
    // Execute dialog
    if (dlg.exec() == QDialog::Accepted) {
        // Emit password changed
        d->_cachedCryptedPassword = dlg.cryptedPassword();
        d->_cachedUncryptedPassword = dlg.uncryptedPassword();
        Q_EMIT cryptedPasswordChanged(dlg.cryptedPassword());
        Q_EMIT uncryptedPasswordChanged(dlg.uncryptedPassword());
    }
}

/**
 * When login content is updated, check its validity and
 * warn user of the validity of the login
 */
void PasswordWidget::onLoginChanged(const QString &login)
{
    QStringList msg;

    // Avoid some chars
    QStringList avoid;
    avoid.append("'");
    avoid.append(",");
    avoid.append("/");
    avoid.append("\\");
    foreach(const QString &a, avoid) {
        if (login.contains(a))
            msg << tr("Login must not contain the following char: %1").arg(a);
    }

    // Length
    if (login.size() < 6)
        msg << tr("Login is too short (6 chars minimum)");

    if (msg.isEmpty()) {
        d->ui->info->setText(tr("Login is valid"));
        d->ui->info->setStyleSheet("color: darkgreen");
    } else {
        d->ui->info->setText(tr("Login is invalid") + "\n" + msg.join("\n"));
        d->ui->info->setStyleSheet("color: darkred");
    }
}
