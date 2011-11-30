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
#include "server_p.h"
#include "core.h"

#include <utils/log.h>
#include <utils/global.h>

#include <QDir>

using namespace DataPack;
using namespace Internal;


ServerPrivate::ServerPrivate(const QUrl &url) :
    QObject(), m_IsLocal(false)
{
    setObjectName("DataPack::Server");
    this->url = url;
    networkAccessManager = DataPack::Core::instance()->networkManager();
}

void ServerPrivate::connectAndUpdate()
{
    // Forge the server config filename
    QUrl requestUrl = url;
    QDir dir(requestUrl.path());
    requestUrl.setPath(dir.filePath("server.conf.xml"));

    QNetworkRequest request(requestUrl); // TODO ADD THE FILES TO THE URL
    request.setRawHeader("User-Agent", "FreeMedForms"); // TODO specify the version?
    QNetworkReply *reply = networkAccessManager->get(request);
    connect(reply, SIGNAL(readyRead()), this, SLOT(requestReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(requestError(QNetworkReply::NetworkError)));
}


void ServerPrivate::requestReadyRead()
{
    WARN_FUNC;
    Q_ASSERT(networkAccessManager);
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        LOG_ERROR("BUGGGG");
        return;
    }
//    QString content = reply->readAll();
}

void ServerPrivate::requestFinished()
{
    WARN_FUNC;
}

void ServerPrivate::requestError(QNetworkReply::NetworkError error)
{
    WARN_FUNC;
    // Save the config file to the m_BackUpPath
    qDebug("error: %d", error);
}
