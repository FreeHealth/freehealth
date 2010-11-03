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
/***************************************************************************
 *   Main Developper : Eric MAEKER, MD <eric.maeker@free.fr>               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef GLOBALTOOLS_H
#define GLOBALTOOLS_H

#include <QString>
#include <QHash>
#include <QMultiHash>
class QProgressDialog;

/**
 * \file globaltools.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 13 Oct 2010
*/


namespace Core {

namespace Tools
{

const void unzipAllFilesIntoDirs( const QStringList & paths );
const bool unzipFile( const QString & fileName, const QString & pathToUnZippedFiles );

QString noAccent(const QString & s);

QString getBlock(const QString &content, const int posStart, int &posEnd, const QRegExp &delimiter);
QString getBlock(const QString &content, const int posStart, int &posEnd, const QString &delimiter);

bool executeProcess(const QString &proc);
bool executeSqlFile(const QString &connectionName, const QString &fileName, QProgressDialog *dlg = 0);
bool executeSqlQuery(const QString &sql, const QString &dbName, const QString &file = QString::null, int line = -1);

bool connectDatabase(const QString &connection, const QString &fileName);


}  // End namespace Tools

}  // End namespace DrugsDbCreator

#endif // GLOBALTOOLS_H
