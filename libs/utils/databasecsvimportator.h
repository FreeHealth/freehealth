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
 *   Main developers : Eric Maeker                                         *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_DATABASECSVIMPORTATOR_H
#define UTILS_DATABASECSVIMPORTATOR_H

#include <utils/global_exporter.h>
#include <QString>
#include <QStringList>

/**
 * \file databasecsvimportator.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 29 May 2013
*/

namespace Utils {
namespace Internal {
class DatabaseCsvImportatorPrivate;
} // namespace Internal

struct UTILS_EXPORT ImportationJob
{
    ImportationJob() :
        fieldSeparator(','),
        textEscapingChar('"'),
        omitFirstLine(false)
    {}

    QString absFilePath;
    QString databaseConnectionName;
    QString tableName;
    QChar fieldSeparator;
    QChar textEscapingChar;
    bool omitFirstLine;
};

struct UTILS_EXPORT ImportationResult
{
    ImportationResult() : hasError(false) {}

    bool hasError;
    QStringList errors;
    QStringList messages;
};

class UTILS_EXPORT DatabaseCsvImportator
{    
public:
    explicit DatabaseCsvImportator();
    ~DatabaseCsvImportator();
    bool initialize();

    void addJob(const ImportationJob &job);

    ImportationResult &startJobs();
    
private:
    Internal::DatabaseCsvImportatorPrivate *d;
};

} // namespace Utils

#endif  // UTILS_DATABASECSVIMPORTATOR_H

