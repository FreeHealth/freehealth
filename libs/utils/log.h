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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#include <utils/global_exporter.h>

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QMultiMap>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QObject>
#include <QDebug>
QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file log.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.10.0
 * \date 15 Jun 2014
*/

#define LOG_ERROR_FOR(object, message)  Utils::Log::addError(object, message,__FILE__,__LINE__)
#define LOG_ERROR(message)              Utils::Log::addError(this, message,__FILE__,__LINE__)

#define LOG_QUERY_ERROR_FOR(object, qsqlquery)  Utils::Log::addQueryError(object, qsqlquery,__FILE__,__LINE__)
#define LOG_QUERY_ERROR(qsqlquery)              Utils::Log::addQueryError(this, qsqlquery,__FILE__,__LINE__)

#define LOG_FOR(object, message) Utils::Log::addMessage(object, message)
#define LOG(message) Utils::Log::addMessage(this, message)

#define LOG_DATABASE_FOR(object, message) Utils::Log::addDatabaseLog(object, message,__FILE__,__LINE__)
#define LOG_DATABASE(message) Utils::Log::addDatabaseLog(this, message,__FILE__,__LINE__)

#define WARN_FUNC qDebug() << Q_FUNC_INFO

namespace Utils {

class UTILS_EXPORT LogData
{
public:
    enum LogDataType {
        Error = 0,
        CriticalError,
        Warning,
        Message
    };

    LogData( const QString &o, const QString &m, const QDateTime &d, const int t ) :
            object(o), message(m), date(d), type(t) {}

    bool isError() const
    {
        if ((type == Error) || (type==CriticalError) || (type==Warning))
            return true;
        return false;
    }
    QString toString() const
    {
        return QString("%1 - %2: %3").arg(object).arg(date.toString(), message);
    }

    // Data
    QString object, message;
    QDateTime date;
    int type;
};

class UTILS_EXPORT Log
{
    static void addData( const QString &o, const QString &m, const QDateTime &d, const int t );

public:
    static void logCompilationConfiguration();

    static void setDebugPluginsCreation(bool debug) {m_debugPlugins = debug;}
    static bool debugPluginsCreation() {return m_debugPlugins;}

    static void setDebugFileInOutProcess(bool debug) {m_logFileInOut = debug;}
    static bool debugFileInOutProcess() {return m_logFileInOut;}

    static void muteConsoleWarnings();

    static void addMessage( const QObject * o, const QString & msg, bool forceWarning = false );
    static void addMessage( const QString & object, const QString & msg, bool forceWarning = false );
    static void addMessages( const QObject * o, const QStringList & msg, bool forceWarning = false );
    static void addMessages( const QString &o, const QStringList & msg, bool forceWarning = false );

    static void addError( const QObject * o, const QString & err, const QString &file = QString::null, const int line = -1, bool forceWarning = false );
    static void addError( const QString & object, const QString & err, const QString &file = QString::null, const int line = -1, bool forceWarning = false );

    static void addErrors( const QObject * o, const QStringList & err, const QString &file = QString::null, const int line = -1, bool forceWarning = false );
    static void addErrors( const QString &o, const QStringList & err, const QString &file = QString::null, const int line = -1, bool forceWarning = false );

    static void addQueryError( const QObject * o, const QSqlQuery & q, const QString &file = QString::null, const int line = -1, bool forceWarning = false );
    static void addQueryError( const QString & o, const QSqlQuery & q, const QString &file = QString::null, const int line = -1, bool forceWarning = false );

    static void addDatabaseLog( const QObject * o, const QSqlDatabase & q, const QString &file = QString::null, const int line = -1, bool forceWarning = false );
    static void addDatabaseLog( const QString & o, const QSqlDatabase & q, const QString &file = QString::null, const int line = -1, bool forceWarning = false );

    /** \brief Add a message to tkLog containing the elapsed time of \t and restart it. Used for debugging purpose. */
    static void logTimeElapsed( QTime &t, const QString &object, const QString &forDoingThis );

    static bool hasError();

    static QStringList messages();
    static QStringList errors();

    static QString     toString( const QString & settingsLog = QString::null );
    static QString     saveLog( const QString & fileName = QString::null );

    static void messagesToTreeWidget( QTreeWidget *parent, bool expandedByClass = true );
    static void errorsToTreeWidget( QTreeWidget *parent, bool expandedByClass = true );

private:
    static QList<LogData> m_Messages;
    static bool m_HasError;
    static bool m_MuteConsole;
    static bool m_logFileInOut, m_debugPlugins;
};

}  // end Utils

#endif // UTILS_LOG_H
