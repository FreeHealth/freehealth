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
/**
  \class Utils::LogData
  Represents a message to log.
  \sa Utils::Log
*/

/**
  \class Utils::Log
  \brief this class holds the logs messages.
    Set messages with addMessage() or addMessages(), set errors with setError() or setErrors().\n
    The strings are stored and you can retrieve messages with messages() and errors with errors().\n
    All message and error is warned into console, you can inhibit this behavior with muteConsoleWarnings(). \n
    You can log QSqlQuery errors using addQueryError().
    The first param of each member is the name of the class that asked to log.

    Return format is for messages and errors :
     - ClassNameAsker : DateOfTheLog - Log
     - ClassNameAsker : DateOftheLog - SQL Error : Driver : xxxxx, Database : xxxx, Query : xxxxxx

     You can directly save the logs via saveLog(), or retrieve a string formatted using toString().
  \sa Utils::LogData
*/

#include "log.h"
#include "global.h"

#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>

#include <QFile>
#include <QDir>
#include <QString>
#include <QVariant>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QHeaderView>
#include <QDateTime>

using namespace Utils;

namespace {
const char * const DEFAULT_LOG_FILENAME = "EHR_log_";
const char * const DEFAULT_DATETIME_FORMAT = "yyyy_MM_dd_HH_mm_ss";
const char * const DEFAULT_LOG_FILENAME_EXTENSION = "txt";
}

QList<LogData> Log::m_Messages;
bool Log::m_HasError = false;
bool Log::m_MuteConsole = false;
bool Log::m_logFileInOut = false;
bool Log::m_debugPlugins = false;
QStringList Log::m_MutedObjects;

/**
 * \fn bool Utils::Log::setDebugFileInOutProcess(bool debug)
 * Set the console debugging of Utils file in/out process (errors are
 * still debugged to console). By default, set to false.
 * \sa Utils::Log::debugFileInOutProcess()
 */

/**
 * \fn bool Utils::Log::setDebugPluginsCreation(bool debug)
 * Set the console debugging of plugins creation process (errors are
 * still debugged to console). By default, set to false.
 * \sa Utils::Log::debugPluginsCreation()
 */

/**
 * Debug whole app/OS configuration to console
 */
void Log::logCompilationConfiguration()
{
    qDebug() << "\n----------";
    if (isDebugWithoutInstallCompilation()) {
        LOG_FOR("Main", "Running debug non-installed version (debug_without_install)");
    } else if (isReleaseCompilation()) {
        LOG_FOR("Main", "Running release version");
    } else {
        LOG_FOR("Main", "Running debug installed version");
    }
    if (isLinuxIntegratedCompilation())
        LOG_FOR("Main", "Linux Integrated");

    LOG_FOR("Main","Libraries in path :\n" + qApp->libraryPaths().join("\n"));
    qDebug() << "----------";
}

QString Log::versionInformation()
{
    return QString("%1 - %2 ; %3 ; Compiled with Qt %6 - Running with Qt %7\n")
            .arg(BINARY_NAME " - " PACKAGE_VERSION)
        #ifdef LINUX_INTEGRATED
        #  ifdef DEBUG
            .arg("Debug (Linux Integrated)")
        #  else
            .arg("Release (Linux Integrated)")
        #  endif
        #else
        #  ifdef DEBUG
            .arg("Debug")
        #  else
            .arg("Release")
        #  endif
        #endif

        #ifdef FULLAPPLICATION_BUILD
            .arg("Full application")
        #else
            .arg("SVN application")
        #endif

            .arg(QT_VERSION_STR)
            .arg(qVersion());
}

void Log::addData(const QString &o, const QString &m, const QDateTime &d, const int t)
{
    m_Messages << LogData(o,m,d,t) ;
    if ((t == LogData::Error) || (t==LogData::CriticalError) || (t==LogData::Warning))
        m_HasError=true;
}

/**
 * Mute all console warning/debug
 */
void Log::muteConsoleWarnings()
{
    m_MuteConsole=true;
}

/**
 * Mute all console output for the specified \e objectName (case insensitive)
 */
void Log::muteObjectConsoleWarnings(const QString &objectName)
{
    if (!m_MutedObjects.contains(objectName, Qt::CaseInsensitive))
        m_MutedObjects << objectName.toLower();
}

/** \sa Utils::Log::muteObjectConsoleWarnings() */
void Log::unmuteObjectConsoleWarnings(const QString &objectName)
{
    m_MutedObjects.removeAll(objectName.toLower());
}

void Log::addMessage(const QString &object, const QString &msg, bool forceWarning)
{
    bool mute = m_MuteConsole || m_MutedObjects.contains(object, Qt::CaseInsensitive);
    if (!mute || forceWarning) {
        QString m = lineWrapString(msg, 90-26);
        m = indentString(m, 26).mid(26);
        qDebug() << QString("%1 %2")
                    .arg(object.leftJustified(25, QChar(' ')))
                    .arg(m);
    }
    addData(object, msg, QDateTime::currentDateTime(), LogData::Message);
}

void Log::addMessages(const QString &o, const QStringList &msg, bool forceWarning)
{
    foreach(const QString &m, msg)
        addMessage(o, m, forceWarning);
}

void Log::addError(const QString &object, const QString &err, const QString &file, const int line, bool forceWarning)
{
    bool mute = m_MuteConsole || m_MutedObjects.contains(object, Qt::CaseInsensitive);
    if (!mute || forceWarning) {
        QString e = QString("** ERROR(%1:%2) ** %3")
                .arg(QFileInfo(file).fileName()).arg(line).arg(err);
        e = lineWrapString(e, 90-26);
        e = indentString(e, 26).mid(26);
        qDebug() << QString("%1 %2")
                    .arg(object.leftJustified(25, QChar(' ')))
                    .arg(e);
    }
    addData(object, err, QDateTime::currentDateTime(), LogData::Error);
}

void Log::addErrors(const QString &o, const QStringList &err, const QString &file, const int line,  bool forceWarning)
{
    foreach(const QString &m, err)
        addError(o, m, file, line, forceWarning);
}

void Log::addQueryError(const QObject *o, const QSqlQuery &q, const QString &file, const int line, bool forceWarning)
{
    addQueryError(o->objectName(), q, file, line, forceWarning);
}

void Log::addQueryError(const QString &o, const QSqlQuery &q, const QString &file, const int line, bool forceWarning)
{
    addError(o, QCoreApplication::translate("Log", "SQL Error: %1 %2 %3 %4")
             .arg(q.lastError().nativeErrorCode())
             .arg(q.lastError().driverText())
             .arg(q.lastError().databaseText())
             .arg(q.lastQuery()), file, line, forceWarning);
}

void Log::addDatabaseLog( const QObject * o, const QSqlDatabase & q, const QString &file, const int line, bool forceWarning)
{
    addDatabaseLog(o->objectName(), q, file, line, forceWarning);
}

void Log::addDatabaseLog( const QString & o, const QSqlDatabase & q, const QString &file, const int line, bool forceWarning)
{
    if (!m_MuteConsole || forceWarning) {
        qDebug() << q << "user" << q.userName() << "pass" << q.password();
    }
    using namespace Trans::Constants;
    using namespace Trans::ConstantTranslations;
    QString log = QString("%1 ; %2 ; %3 ; %4 ; %5 ; %6")
            .arg(tkTr(_1_COLON_2).arg(o, QDateTime::currentDateTime().toString()))
            .arg(tkTr(_1_COLON_2).arg(tkTr(DATABASE), q.driverName()))
            .arg(tkTr(_1_COLON_2).arg(tkTr(HOST), q.hostName()))
            .arg(tkTr(_1_COLON_2).arg(tkTr(PORT)).arg(q.port()))
            .arg(tkTr(_1_COLON_2).arg(tkTr(LOGIN), q.userName()))
            .arg(tkTr(_1_COLON_2).arg(tkTr(PASSWORD), q.password()));
    if(q.lastError().isValid()) {
        log+=(QString("; %1 ; %2 ; %3 ; %4")
              .arg(tkTr(_1_COLON_2).arg(tkTr(ERROR_DRIVER), q.lastError().driverText()))
              .arg(tkTr(_1_COLON_2).arg(tkTr(ERROR_DATABASE), q.lastError().databaseText()))
              .arg(tkTr(_1_COLON_2).arg(tkTr(ERROR_NATIVE), q.lastError().nativeErrorCode()))
              .arg(tkTr(_1_COLON_2).arg(tkTr(ERROR_TYPE), QString::number(q.lastError().type())))
              );
    }
    addError(o, log, file, line, forceWarning);
}


/** \brief Add a message to tkLog containing the elapsed time of \t and restart it. Used for debugging purpose. */
void Log::logTimeElapsed(QTime &t, const QString &object, const QString &forDoingThis)
{
    addMessage("Chrono - " + object, QCoreApplication::translate("Log", "%1 ms: %2")
               .arg(t.elapsed()).arg(forDoingThis));
    t.restart();
}

bool Log::hasError()        { return  m_HasError; }

QStringList Log::messages()
{
    QStringList r;
    foreach(const LogData &v, m_Messages)
        if (v.type == LogData::Message)
            r << v.toString();
    return r;
}

QStringList Log::errors()
{
    QStringList r;
    foreach(const LogData &v, m_Messages)
        if (v.isError())
            r << v.toString();
    return r;
}

void Log::addMessage(const QObject *o, const QString &msg, bool forceWarning)
{
    if (o)
        addMessage(o->objectName(), msg, forceWarning);
    else
        addMessage(Trans::ConstantTranslations::tkTr(Trans::Constants::UNKNOWN), msg, forceWarning);
}

void Log::addMessages(const QObject *o, const QStringList &msg, bool forceWarning)
{
    if (o) {
        foreach(const QString &m, msg)
            addMessage(o, m, forceWarning);
    } else {
        foreach(const QString &m, msg)
            addMessage(Trans::ConstantTranslations::tkTr(Trans::Constants::UNKNOWN), m, forceWarning);
    }
}

void Log::addError(const QObject *o, const QString &err, const QString &file, const int line, bool forceWarning)
{
    if (o)
        addError(o->objectName(), err, file, line, forceWarning);
    else
        addError(Trans::ConstantTranslations::tkTr(Trans::Constants::UNKNOWN), err, file, line,  forceWarning);
}

void Log::addErrors(const QObject *o, const QStringList &err, const QString &file, const int line, bool forceWarning)
{
    if (o) {
        foreach(const QString &m, err)
            addError(o, m, file, line,  forceWarning);
    } else {
        foreach(const QString &m, err)
            addError(Trans::ConstantTranslations::tkTr(Trans::Constants::UNKNOWN), m, file, line, forceWarning);
    }
}


/** \brief Saves the log to the file \e fileName */
QString Log::saveLog(const QString &fileName)
{
    QString f = fileName;
    if (fileName.isEmpty()) {
        f = QString("%1/%2%3.%4")
                .arg(QDir::homePath())
                .arg(DEFAULT_LOG_FILENAME)
                .arg(QDateTime::currentDateTime().toString(DEFAULT_DATETIME_FORMAT))
                .arg(DEFAULT_LOG_FILENAME_EXTENSION);
    }
    QFile file(f);
    if (!file.open(QFile::WriteOnly)) {
        Log::addError("Log", QCoreApplication::translate("Log", "Unable to save %1: Error %2").arg(f , file.errorString()));
        return QString::null;
    }

    QString tmp = toString();
    file.write(tmp.toUtf8());
    file.close();
    return f;
}

/** \brief Transforms log to a readable wiki string. */
QString Log::toString(const QString &settingsLog)
{
    Q_UNUSED(settingsLog);
    // Get all errors
    typedef QPair<QString, QString> pairs;
    QList<pairs> report;
    foreach(const LogData &v, m_Messages) {
        if (v.isError()) {
            report << pairs(v.object, Utils::lineWrapString(v.message, 50));
        }
    }

    // find the max length
    int max = 0;
    foreach(const pairs &pair, report) {
        max = qMax(max, pair.first.length());
    }
    // justify test && create line
    QStringList lines;
    lines << QCoreApplication::translate("Log", "*****  Registered Errors  *****\n");
    foreach(const pairs &pair, report) {
        if (pair.first == "---") {
            lines << "-------------------------";
            continue;
        }
        QString second = " " + pair.second;
        second = second.replace("\n", "\n" + QString().fill(' ', max+5));
        lines << "  " + pair.first.leftJustified(max+2, QLatin1Char('.')) + second;
    }
    return lines.join("\n");
}

/**
  \brief This function is used for debugging purpose.
 Constructs a QTreeWidget with the loggeg messages.\n
  You can specify if you want to have an expanded class tree (by default) or a non expanded chronologic tree.\n
  \sa tkDebugDialog
*/
void Log::messagesToTreeWidget(QTreeWidget *parent, bool expandedByClass)
{
    Q_ASSERT_X(parent, "Log::toTreeWidget", "You must pass a valid QTreeWidget.");
    parent->clear();
    parent->setColumnCount(2);
    QHash<QString, QTreeWidgetItem*> class_item;
    if (expandedByClass) {
        foreach(const LogData &v , m_Messages) {
            if (v.isError())
                continue;
            // create next tree item if class does not already exists
            if (!class_item.keys().contains(v.object))
                class_item.insert(v.object, new QTreeWidgetItem(parent, QStringList() << v.object));
            // add message to the class root item
            new QTreeWidgetItem(class_item.value(v.object), QStringList() << v.date.toString("HH:mm:ss:ms") << v.message);
        }
    } else {
        foreach(const LogData &v , m_Messages) {
            if (v.isError())
                continue;
            new QTreeWidgetItem(parent, QStringList() << v.object << v.message << v.date.toString());
        }
    }
    parent->header()->hide();
    parent->expandAll();
    parent->resizeColumnToContents(0);
    parent->resizeColumnToContents(1);
}

/**
  \brief This function is used for debugging purpose.
 Constructs a QTreeWidget with the loggeg errors.\n
  You can specify if you want to have an expanded class tree (by default) or a non expanded chronologic tree.\n
  \sa tkDebugDialog
*/
void Log::errorsToTreeWidget(QTreeWidget *parent, bool expandedByClass)
{
    Q_ASSERT_X(parent, "Log::toTreeWidget", "You must passed a valid QTreeWidget.");
    parent->clear();
    parent->setColumnCount(2);
    QHash<QString, QTreeWidgetItem*> class_item;
    if (expandedByClass) {
        foreach(const LogData &v , m_Messages) {
            if (!v.isError())
                continue;
            // create next tree item if class does not already exist
            if (!class_item.keys().contains(v.object))
                class_item.insert(v.object, new QTreeWidgetItem(parent, QStringList() << v.object));
            // add message to the class root item
            new QTreeWidgetItem(class_item.value(v.object), QStringList() << QStringList() << v.date.toString("HH:mm:ss:ms") << v.message);
        }
    } else {
        foreach(const LogData &v , m_Messages) {
            if (v.isError())
                continue;
            new QTreeWidgetItem(parent, QStringList() << v.object << v.message << v.date.toString());
        }
    }
    parent->header()->hide();
    parent->expandAll();
    parent->resizeColumnToContents(0);
    parent->resizeColumnToContents(1);
}
