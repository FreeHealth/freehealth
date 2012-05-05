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

//#include <utils/log.h>
#include <QSqlDatabase>
#include <QObject>
#include <QDebug>

namespace Alert {

class AlertBase:public QObject
{
	Q_OBJECT
	private:
		AlertBase(QObject * parent = 0);
		~AlertBase();
        public:
            static AlertBase * singleton()
            {
                if (m_singleton == NULL)
                {
                	  m_singleton = new AlertBase();
                	  qWarning() << __FILE__ << QString::number(__LINE__) << "AlertBase is created" ;
                    }
                else
                {
                	qWarning() << __FILE__ << QString::number(__LINE__) << "AlertBase is already opened" ;
                    }
                return m_singleton;
            }
            QSqlDatabase m_db;
            
	private:
	    static AlertBase * m_singleton;
	    bool connectToAlertDatabase();
	    bool setTables();
};

}  // Alert

#endif

