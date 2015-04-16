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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEACCOUNT_COMMANDLINEPARSER_H
#define FREEACCOUNT_COMMANDLINEPARSER_H

#include <coreplugin/icommandline.h>

#include <QString>
#include <QVariant>

/**
 * \file commandlineparser.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 06 Aug 2011
*/

namespace Core {
    class IPatient;
    class Patient;

namespace Internal {
class CommandLinePrivate;

class CommandLine  : public Core::ICommandLine
{
public:
    CommandLine(QObject *parent = 0);
    ~CommandLine();

    QVariant value(int param, const QVariant &def = QVariant()) const;
    QString paramName(int param) const;

private:
    CommandLinePrivate *d;
};

}  // End namespace Internal
}  // End namespace Core

#endif // FREEACCOUNT_COMMANDLINEPARSER_H
