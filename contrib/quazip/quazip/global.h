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
#ifndef QUAZIP_GLOBAL_FREEMEDFORMS_H
#define QUAZIP_GLOBAL_FREEMEDFORMS_H

#include <quazip/quazip_global.h>
#include <QString>

QT_BEGIN_NAMESPACE
class QStringList;
class QProgressBar;
QT_END_NAMESPACE

namespace QuaZipTools {

    QUAZIP_EXPORT bool unzipFile(const QString &fileName, const QString &pathToUnZippedFiles = QString(), QProgressBar *bar = 0);

    QUAZIP_EXPORT bool unzipAllFilesIntoDirs(const QStringList &paths);

    QUAZIP_EXPORT QStringList zipFileNameContent(const QString &absFileName);

}  //  End namespace QuaZipTools

#endif // QUAZIP_GLOBAL_FREEMEDFORMS_H
