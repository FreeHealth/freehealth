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
#ifndef USERIDENTIFIER_H
#define USERIDENTIFIER_H

#include <usermanagerplugin/usermanager_exporter.h>

#include <QDialog>

/**
 * \file useridentifier.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 04 Apr 2011
*/
namespace UserPlugin {
namespace Internal {
namespace Ui {
class  UserIdentifier;
}  // End Ui

class USER_EXPORT UserIdentifier : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(UserIdentifier)

    enum { MaxNumberOfTries = 4 };

public:
    explicit UserIdentifier(QWidget *parent = 0 );

    QString login() const;
    QString login64crypt() const;

    QString password() const;
    QString cryptedPassword() const;

private:
    void done(int result);

private:
    Ui::UserIdentifier *m_ui;
    int m_NumberOfTries;
};

}  // End Internal
}  // End namespace UserPlugin

#endif // USERIDENTIFIER_H
