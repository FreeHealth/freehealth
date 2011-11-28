/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 ***************************************************************************/
#ifndef DATAPACK_SERVERMANAGER_P_H
#define DATAPACK_SERVERMANAGER_P_H

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "server.h"

namespace DataPack {

class ServerManagerPrivate : public QObject
{
	friend class Server;
	Q_OBJECT
public:
	ServerManagerPrivate();
	~ServerManagerPrivate();
//    QList<Server> servers;
    QNetworkAccessManager *networkAccessManager;
    QString filesCachePath;

    int getServerIndex(const QUrl &url) const;
    bool addServer(const QUrl &url);
    void connectAndUpdate(int index);
	Server *getAt(int index) const;
};

}

#endif // DATAPACK_SERVERMANAGER_P_H
