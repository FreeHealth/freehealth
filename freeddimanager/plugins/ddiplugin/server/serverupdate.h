/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGUN_INTERNAL_SERVERUPDATE_H
#define DDIMANAGER_DDIPLUGUN_INTERNAL_SERVERUPDATE_H

#include <QObject>
#include <QHash>
#include <QString>

/**
 * \file serverupdate.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 2013-11-02
*/

namespace DDI {
namespace Internal {
class ServerUpdatePrivate;

class ServerUpdate
{
public:
    enum UpdateType {
        Update,
        New
    };
    enum UpdateSubType {
        Atc,
        Interactor,
        DrugDrugInteraction
    };

    ServerUpdate();
    ~ServerUpdate();
    ServerUpdate(const ServerUpdate &cp);

    void setType(UpdateType type);
    void setSubType(UpdateSubType subtype);
    void setTokens(const QHash<QString, QString> &tokens);

    UpdateType type() const;
    UpdateSubType subType() const;
    QString wikiContent() const;
    QString hashTag() const;

    bool populateFromServerContent(const QString &hashTag);

    bool operator==(const ServerUpdate &other) const;
    ServerUpdate &operator=(const ServerUpdate &cp);

private:
    ServerUpdatePrivate *d;
};

} // namespace Internal
} // namespace DDI

#endif // DDIMANAGER_DDIPLUGUN_INTERNAL_SERVERUPDATE_H

