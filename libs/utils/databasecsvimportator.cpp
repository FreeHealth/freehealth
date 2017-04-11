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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Utils::DatabaseCsvImportator
 * Import a CSV content into a database (SQLite/MySQL are both supported).
 * \note Unit-test available (see: tests/auto/auto.pro)
 */

#include "databasecsvimportator.h"

#include <utils/global.h>
#include <utils/database.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_filepathxml.h>
#include <translationutils/trans_database.h>

#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>

#include <QDebug>

using namespace Utils;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Utils {
namespace Internal {
class DatabaseCsvImportatorPrivate
{
public:
    DatabaseCsvImportatorPrivate(DatabaseCsvImportator *parent) :
        _currentPos(-1),
        _currentFieldStartPos(-1),
        _currentLineStartPos(-1),
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~DatabaseCsvImportatorPrivate()
    {
    }

    // Returns true if the current position in the string correspond to a CSV EndOfLine (-> go to next value)
    // Uses the _currentContent & _currentPosition
    bool isCurrentPositionEndOfSqlLine(const ImportationJob &job)
    {
        if (!IN_RANGE_STRICT_MAX(_currentPos, 0, _currentContent.size()))
            return true;

        // Is current pos not a EOL -> false
        if (_currentContent.at(_currentPos) != '\n')
            return false;

        // TODO: manage CR, CRLF, LF
        // field escaped?
        if (isCurrentFieldIsEscaped(job)) {
            // Still escaped?
            if (!isPreviousPositionEscapeChar(job))
                return false;
            // End of escaping?
            // TODO: manage " or ' escaping char with doubling : "" '' -> are not escaping
            // TODO: manage " or ' escaping char with \ : \" \' -> are not escaping
            return (_currentPos-1 != _currentFieldStartPos);
        }
        return true;
    }

    // Returns true if the current position is at the end of the file
    bool isCurrentPositionEndOfFile()
    {
        return (_currentPos >= _currentContent.size());
    }

    // Returns true if the current position
    bool isCurrenPositionFieldSeparator(const ImportationJob &job)
    {
        // Wrong position -> true
        if (!IN_RANGE_STRICT_MAX(_currentPos, 0, _currentContent.size()))
            return true;

        // field escaped?
        if (isCurrentFieldIsEscaped(job)) {
            // Still escaped?
            if (!isPreviousPositionEscapeChar(job))
                return false;
            // End of escaping?
            // TODO: manage " or ' escaping char with doubling : "" '' -> are not escaping
            // TODO: manage " or ' escaping char with \ : \" \' -> are not escaping
            return (_currentPos-1 != _currentFieldStartPos);
        }

        // FieldSeparator -> true
        if (_currentContent.at(_currentPos) == job.fieldSeparator)
            return true;

        // \n -> true
        if (_currentContent.at(_currentPos) == '\n')
            return true;

        return false;
    }

    bool isCurrentPositionEscapeChar(const ImportationJob &job)
    {
        if (!IN_RANGE_STRICT_MAX(_currentPos, 0, _currentContent.size()))
            return false;
        return (_currentContent.at(_currentPos)==job.textEscapingChar);
    }

    bool isPreviousPositionEscapeChar(const ImportationJob &job)
    {
        if (!IN_RANGE_STRICT_MAX(_currentPos-1, 0, _currentContent.size()))
            return false;
        return (_currentContent.at(_currentPos-1)==job.textEscapingChar);
    }

    // Returns true is the current field is escaped,
    // Uses the _currentFieldStartPos must be set
    bool isCurrentFieldIsEscaped(const ImportationJob &job)
    {
        if (!IN_RANGE_STRICT_MAX(_currentFieldStartPos, 0, _currentContent.size()))
            return false;
        return (_currentContent.at(_currentFieldStartPos)==job.textEscapingChar);
    }

    bool parseContent(const ImportationJob &job, Utils::ImportationResult *result)
    {
        Q_UNUSED(result);
        _currentPos = 0;
        _currentFieldStartPos = 0;
        _currentLineStartPos = 0;

        // Omit first line of the CSV file?
        if (job.omitFirstLine) {
            _currentPos = _currentContent.indexOf("\n") + 1;
        }

        // Parse CSV lines
        QString sqllines;
        while (!isCurrentPositionEndOfFile()) {
            // New line
            _currentFieldStartPos = _currentPos;
            _currentLineStartPos = _currentPos;
            QStringList fields;
            while (!isCurrentPositionEndOfSqlLine(job)) {

                // Here we are at the beginning of the field
                while (!isCurrenPositionFieldSeparator(job)) {
                    ++_currentPos;
                }

                // Current Pos at the end of the field
                if (isCurrentFieldIsEscaped(job)) {
                    // Remove the escaping chars
                    int b = _currentFieldStartPos + 1;
                    int e = _currentPos - 1;
                    fields << _currentContent.mid(b, e-b).replace("'", "''");
                } else {
                    fields << _currentContent.mid(_currentFieldStartPos, _currentPos-_currentFieldStartPos).replace("'", "''");
                }

                // If currentChar != '\n' next char is '\n' -> add an empty value
                if (_currentContent.at(_currentPos) == job.fieldSeparator
                        && _currentContent.at(_currentPos+1) == '\n' ) {
                    fields << QString();
                }

                // Go one step further
                if (!isCurrentPositionEndOfSqlLine(job))
                    ++_currentPos;
                _currentFieldStartPos = _currentPos;
            }

            // Create the sql command
            _sqlCommands << QString("INSERT INTO %1 VALUES\n('%2');\n").arg(job.tableName).arg(fields.join("', '"));

            fields.clear();
            // Go one step further (as we are currently at the EOL char)
            ++_currentPos;
        }
        sqllines.chop(2);
        return true;
    }

    Utils::ImportationResult &import(const ImportationJob &job)
    {
        _currentContent.clear();
        _currentPos = 0;
        _sqlCommands.clear();

        Utils::ImportationResult *result = new Utils::ImportationResult;
        // Check database & file
        if (!QFileInfo(job.absFilePath).exists()) {
            result->hasError = true;
            result->errors << tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(job.absFilePath);
            return *result;
        }

        QSqlDatabase db = QSqlDatabase::database(job.databaseConnectionName);
        if (!db.isOpen()) {
            if (!db.open()) {
                result->hasError = true;
                result->errors << tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                  .arg(db.connectionName())
                                  .arg(db.lastError().text());
                return *result;
            }
        }

        // Read file
        _currentContent = Utils::readTextFile(job.absFilePath, Utils::DontWarnUser);
        if (_currentContent.isEmpty()) {
            result->hasError = true;
            result->errors << tkTr(Trans::Constants::FILE_1_ISEMPTY).arg(job.absFilePath);
            return *result;
        }

        // Parse content
        parseContent(job, result);

        // Send to database
        if (!Utils::Database::executeSQL(_sqlCommands, db)) {
            result->hasError = true;
            result->errors << "Unable to send data to database";
        }
        return *result;
    }

public:
    QString _currentContent;
    int _currentPos, _currentFieldStartPos, _currentLineStartPos;
    
    QList<Utils::ImportationJob> _jobs;
    QStringList _sqlCommands;

private:
    DatabaseCsvImportator *q;
};
} // namespace Internal
} // end namespace Utils

/*! Constructor of the Utils::DatabaseCsvImportator class */
DatabaseCsvImportator::DatabaseCsvImportator() :
    d(new DatabaseCsvImportatorPrivate(this))
{
}

/*! Destructor of the Utils::DatabaseCsvImportator class */
DatabaseCsvImportator::~DatabaseCsvImportator()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DatabaseCsvImportator::initialize()
{
    return true;
}

/** Add a new importation job to the object */
void DatabaseCsvImportator::addJob(const ImportationJob &job)
{
    d->_jobs << job;
}

/** Starts all importations and return a reference to the Utils::ImportationResult */
ImportationResult &DatabaseCsvImportator::startJobs()
{
    ImportationResult *result = new ImportationResult;
    foreach(const ImportationJob &job, d->_jobs) {
        ImportationResult jobresult = d->import(job);
        if (jobresult.hasError) {
            result->hasError = true;
            result->errors << jobresult.errors;
        }
        result->messages << jobresult.messages;
    }
    return *result;
}
