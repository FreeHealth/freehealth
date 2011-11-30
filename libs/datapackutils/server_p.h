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
#ifndef DATAPACK_SERVER_P_H
#define DATAPACK_SERVER_P_H

#include <QUrl>
#include <QNetworkReply>
#include <QNetworkAccessManager>

namespace DataPack {
namespace Internal {

class ServerPrivate : public QObject
{
    Q_OBJECT
public:
    ServerPrivate(const QString &url = QString::null);

    bool connected;
    QString url;
    bool m_IsLocal;
    QNetworkAccessManager *networkAccessManager;

    void connectAndUpdate();

public Q_SLOTS:
    void requestReadyRead();
    void requestFinished();
    void requestError(QNetworkReply::NetworkError error);
};

}  // End namespace Internal
}  // End namespace DataPack

#endif // DATAPACK_SERVER_P_H
