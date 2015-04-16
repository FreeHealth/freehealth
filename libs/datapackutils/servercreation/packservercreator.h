/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developers : Eric Maeker                                          *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_PACKSERVERCREATOR_H
#define DATAPACK_PACKSERVERCREATOR_H

#include <datapackutils/datapack_exporter.h>
#include <datapackutils/servercreation/packcreationqueue.h>
#include <QHash>
#include <QString>

/**
 * \file packservercreator.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 11 Jul 2014
*/

namespace DataPack {

class DATAPACK_EXPORT PackServerCreator
{
public:
    explicit PackServerCreator();
    ~PackServerCreator();

    void useDefaultPathForServerDescriptionFiles(const QString &rootPath);
    bool addPackCreationQueue(const PackCreationQueue &queue);
    bool setServerDescriptionFilePath(const QString &serverUid, const QString &descrAbsFileName);

    void setAutoVersionning(const bool enable) {_autoVersionning = enable;}
    bool autoVersionning() const {return _autoVersionning;}

    bool createServer(const QString &serverAbsPath) const;

private:
    PackCreationQueue _queue;
    QHash<QString, QString> _serverUid_DescrFile;
    bool _autoVersionning;
};

} // namespace DataPack

#endif // DATAPACK_PACKSERVERCREATOR_H

