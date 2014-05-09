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
 *  Main developers : Eric Maeker                                          *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_PACKCREATIONQUEUE_H
#define DATAPACK_PACKCREATIONQUEUE_H

#include <datapackutils/datapack_exporter.h>
#include <QString>
#include <QMultiHash>
#include <QList>

/**
 * \file packcreationqueue.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 08 May 2014
*/

namespace DataPack {
struct DATAPACK_EXPORT RequestedPackCreation {
    enum ContentType {
        ZippedFile = 0,
        UnzippedFile,
        DirContent
    };

    QString serverUid, descriptionFilePath;
    QMultiHash<int, QString> content;
};

class DATAPACK_EXPORT PackCreationQueue
{
public:
    explicit PackCreationQueue();
    ~PackCreationQueue();

    bool checkValidity(const RequestedPackCreation &request) const;
    bool addToQueue(const RequestedPackCreation &request);
    const QList<RequestedPackCreation> &queue() const {return _queue;}

    bool createZippedContent(const RequestedPackCreation &request, const QString &absZipFileName);

    bool fromXmlFile(const QString &absFile);
    bool saveToXmlFile(const QString &absFile, bool useRelativePath = false) const;

private:
    QList<RequestedPackCreation> _queue;
};

} // namespace DataPack

#endif // DATAPACK_PACKCREATIONQUEUE_H

