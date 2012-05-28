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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERTBASE_H
#define ALERTBASE_H

#include <utils/database.h>
#include <QObject>

/**
 * \file alertbase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>, Pierre-Marie Desombre <pm.desombre@gmail.com>
 * \version 0.8.0
 * \date 28 May 2012
*/

namespace Alert {
class AlertCore;
namespace Internal {

class AlertBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class Alert::AlertCore;

protected:
    AlertBase(QObject * parent = 0);
    bool init();

public:
    ~AlertBase();
    bool isInitialized() const {return m_initialized;}

    // For debugging purpose
    void toTreeWidget(QTreeWidget *tree);

private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                          const QString &pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString &login, const QString &pass,
                          const int port,
                          CreationOption createOption
                         );

private Q_SLOTS:
    void onCoreDatabaseServerChanged();

private:
    bool m_initialized;
};

}  // namespace Internal
}  // namespace Alert

#endif  // ALERTBASE_H

