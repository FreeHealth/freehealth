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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef USERMANAGER_COMMANDLINEPARSER_H
#define USERMANAGER_COMMANDLINEPARSER_H

#include <coreplugin/icommandline.h>
#include <QObject>
#include <QHash>

/**
 * \file commandlineparser.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 22 Jan 2013
*/

namespace Core {
namespace Internal {

class CommandLine  : public Core::ICommandLine
{
    Q_OBJECT

public:
    CommandLine(QObject *parent = 0);
    ~CommandLine();

    QVariant value(int param, const QVariant &def = QVariant()) const;
    void setValue(int ref, const QVariant &value);
    QString paramName(int param) const;

private:
    QHash<int,QVariant> params;
    QHash<int, QString> ref;
};

}  // End namespace Internal
}  // End namespace Core

#endif // USERMANAGER_COMMANDLINEPARSER_H
