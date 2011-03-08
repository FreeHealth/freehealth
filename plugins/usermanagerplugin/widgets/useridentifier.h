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
#ifndef TKUSERIDENTIFIER_H
#define TKUSERIDENTIFIER_H

#include <usermanagerplugin/usermanager_exporter.h>

#include <QDialog>

/**
 * \file useridentifier.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.5
 * \date 17 Sept 2009
*/
namespace UserPlugin {
namespace Internal {
namespace Ui {
class  UserIdentifier;
}  // End Ui

class USER_EXPORT UserIdentifier : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(UserIdentifier);

    enum { MaxNumberOfTries = 4 };

public:
    explicit UserIdentifier( const QStringList & informations = QStringList(), QWidget *parent = 0 );

    QString  login();
    QString  cryptedPassword();

private:
    void done( int result );

private:
    Ui::UserIdentifier *m_ui;
    int m_NumberOfTries;
};

}  // End Internal
}  // End namespace UserPlugin

#endif // TKUSERIDENTIFIER_H
