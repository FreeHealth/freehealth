/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef UNIQUEIDMANAGER_H
#define UNIQUEIDMANAGER_H

#include "core_exporter.h"

#include <QtCore/QString>
#include <QtCore/QHash>

namespace Core {

class CORE_EXPORT UniqueIDManager
{
public:
    UniqueIDManager();
    ~UniqueIDManager();

    static UniqueIDManager *instance() { return m_instance; }

    bool hasUniqueIdentifier(const QString &id) const;
    int uniqueIdentifier(const QString &id);
    QString stringForUniqueIdentifier(int uid);

private:
    QHash<QString, int> m_uniqueIdentifiers;
    static UniqueIDManager *m_instance;
};

} // namespace Core

#endif // UNIQUEIDMANAGER_H
