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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CORE_ICOMMANDLINE_H
#define CORE_ICOMMANDLINE_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QVariant>

/**
 * \file icommandline.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \author Jerome Pinguet
 * \version 0.6.0
 * \date 09 Aug 2011
 * \class Core::ICommandLine
 * \brief Command line parser interface.
*/

namespace Core {

class CORE_EXPORT ICommandLine : public QObject
{
    Q_OBJECT

public:
    enum Params {
        Chrono = 1000,
        ConfigFile,
        ClearUserDatabases,
        CreateVirtuals,
        ResetUserPreferences,
        UserClearLogin,
        UserClearPassword,
        CheckFormUpdates,
        AlwaysUpdateForms,
        NoCheckUpdate,
        MaxParam
    };

    ICommandLine(QObject *parent = 0) : QObject(parent) {}
    virtual ~ICommandLine() {}

    virtual QVariant value(int param, const QVariant &def = QVariant()) const = 0;
    virtual QString paramName(int param) const = 0;
};

} // end Core

#endif // CORE_ICOMMANDLINE_H
