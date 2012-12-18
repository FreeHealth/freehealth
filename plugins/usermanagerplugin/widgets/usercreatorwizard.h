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
#ifndef USERWIZARD_H
#define USERWIZARD_H

#include <usermanagerplugin/usermanager_exporter.h>

#include <QObject>
#include <QWidget>
#include <QWizardPage>
#include <QWizard>
#include <QHash>
#include <QString>

QT_BEGIN_NAMESPACE
class QLabel;
class QEvent;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file usercreatorwizard.h
 * \author Eric MAEKER
 * \version 0.8.2
 * \date 17 Dec 2012
*/

namespace UserPlugin {
namespace Internal {
class UserCreatorWizardPrivate;

namespace Ui {
class UserWizardContactWidget;
}  // End namespace Ui
}  // End namespace Internal

class USER_EXPORT UserCreatorWizard : public QWizard
{
    Q_OBJECT
public:
    enum Pages {
        IdentityAndLoginPage = 0,
        ContactPage,
        ProfilPage,
        RightsPage,
        SpecialiesQualificationsPage,
        ExtraPages,
        LastPage = 10000
    };

    UserCreatorWizard(QWidget *parent = 0);
    ~UserCreatorWizard();

    static bool checkUserRights();

    static void setUserPaper(const int ref, const QString &xml);
    static void setUserRights(const int role, const int value);
    static int userRights(const int role);

protected Q_SLOTS:
    void done(int r);

private:
    void showEvent(QShowEvent *event);

private:
    Internal::UserCreatorWizardPrivate *d;
};

}  // End namespace UserPlugin


#endif // USERWIZARD_H
