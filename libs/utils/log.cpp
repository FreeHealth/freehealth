/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/**
  \class LogData
  Represents a message to log.
  \ingroup toolkit
  \ingroup object_toolkit
  \sa Log
*/



/**
  \class Log
  \brief this class holds the logs messages.
    Set messages with addMessage() or addMessages(), set errors with setError() or setErrors().\n
    The strings are stored and you can retreive messages with messages() and errors with errors().\n
    All message and error is warned into console, you can inhibate this behavior with muteConsoleWarnings(). \n
    You can log QSqlQuery errors using addQueryError().
    The first param of each members is the name of the class that asked to log.

    Return format is for messages and errors :
     - ClassNameAsker : DateOfTheLog - Log
     - ClassNameAsker : DateOftheLog - SQL Error : Driver : xxxxx, Database : xxxx, Query : xxxxxx

     You can directly save the logs via saveLog(), or retreive a string formatted using toString().
  \ingroup toolkit
  \ingroup object_toolkit
  \sa DataLog
*/

#include "log.h"

#include <translationutils/constanttranslations.h>

#include <QFile>
#include <QDir>
#include <QString>
#include <QVariant>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QHeaderView>

using namespace Utils;

QList<LogData> Log::m_Messages;
bool Log::m_HasError = false;
bool Log::m_MuteConsole = false;

bool Log::warnPluginsCreation() { return true; }


    void Log::addData( const QString &o, const QString &m, const QDateTime &d, const int t )
    {
        m_Messages << LogData(o,m,d,t) ;
        if ((t == LogData::Error) || (t==LogData::CriticalError) || (t==LogData::Warning))
            m_HasError=true;
    }

    void Log::muteConsoleWarnings()
    { m_MuteConsole=true; }

    void Log::addMessage( const QString & object, const QString & msg )
    {
        if (!m_MuteConsole) {
            qWarning() << object << msg;
        }
        addData( object, msg, QDateTime::currentDateTime(), LogData::Message );
    }

    void Log::addMessages( const QString &o, const QStringList & msg )
    {
        foreach( const QString & m, msg )
            addMessage(o, m);
    }

    void Log::addError( const QString & object, const QString & err )
    {
        if (!m_MuteConsole) {
            qWarning() << object << err;
        }
        addData( object, err, QDateTime::currentDateTime(), LogData::Error );
    }

    void Log::addErrors( const QString &o, const QStringList & err )
    {
        foreach( const QString & m, err )
            addError(o, m);
    }


    void Log::addQueryError( const QObject * o, const QSqlQuery & q )
    {
        if (!m_MuteConsole) {
            qWarning() << QCoreApplication::translate( "Log", "SQL Error : Driver : %1, Database : %2, Query : %3" ).arg( q.lastError().driverText(), q.lastError().databaseText(), q.lastQuery() );
        }
        addError(o, QCoreApplication::translate( "Log", "%1 : %2 - SQL Error : Driver : %3, Database : %4, Query : %5" )
                 .arg( o->objectName(), QDateTime::currentDateTime().toString(), q.lastError().driverText(), q.lastError().databaseText(), q.lastQuery() ) );
    }

    void Log::addQueryError( const QString & o, const QSqlQuery & q )
    {
        if (!m_MuteConsole) {
            qWarning() << QCoreApplication::translate( "Log", "SQL Error : Driver : %1, Database : %2, Query : %3" ).arg( q.lastError().driverText(), q.lastError().databaseText(), q.lastQuery() );
        }
        addError(o, QCoreApplication::translate( "Log", "%1 : %2 - SQL Error : Driver : %3, Database : %4, Query : %5" )
                .arg( o, QDateTime::currentDateTime().toString(), q.lastError().driverText(), q.lastError().databaseText(), q.lastQuery() ) );
        qWarning() << q.lastError();
    }

    /** \brief Add a message to tkLog containing the elapsed time of \t and restart it. Used for debugging purpose. */
    void Log::logTimeElapsed( QTime &t, const QString &object, const QString &forDoingThis )
    {
        addMessage( "Chrono - " + object, QCoreApplication::translate( "Log", "%1 ms : %2")
                    .arg(t.elapsed()).arg(forDoingThis));
        t.restart();
    }

    bool Log::hasError()        { return  m_HasError; }

    QStringList Log::messages()
    {
        QStringList r;
        foreach( const LogData &v, m_Messages)
            if (v.type == LogData::Message)
                r << v.toString();
        return r;
    }

    QStringList Log::errors()
    {
        QStringList r;
        foreach( const LogData &v, m_Messages)
            if (v.isError())
                r << v.toString();
        return r;
    }



void Log::addMessage( const QObject * o, const QString & msg )
{
    if (o)
        addMessage(o->objectName(), msg );
    else
        addMessage( Trans::ConstantTranslations::tkTr(Trans::Constants::UNKNOWN), msg );
}

void Log::addMessages( const QObject * o, const QStringList & msg )
{
    if (o) {
        foreach( const QString & m, msg )
            addMessage(o, m);
    } else {
        foreach( const QString & m, msg )
            addMessage(Trans::ConstantTranslations::tkTr(Trans::Constants::UNKNOWN), m);
    }
}

void Log::addError( const QObject * o, const QString & err )
{
    if (o)
        addError(o->objectName(), err );
    else
        addError(Trans::ConstantTranslations::tkTr(Trans::Constants::UNKNOWN), err );
}

void Log::addErrors( const QObject * o, const QStringList & err )
{
    if (o) {
        foreach( const QString & m, err )
            addError(o, m);
    } else {
        foreach( const QString & m, err )
            addError(Trans::ConstantTranslations::tkTr(Trans::Constants::UNKNOWN), m);
    }
}


/** \brief Saves the log to the file \e fileName */
QString Log::saveLog( const QString & fileName )
{
    QString f = fileName;
    if ( fileName.isEmpty() )
        f = QDir::homePath() + "/FMF_LOG.TXT";

    QFile file( f );
    if ( ! file.open( QFile::WriteOnly ) ) {
        Log::addError( "Log", QCoreApplication::translate( "Log", "Unable to save %1 : Error %2" ).arg( f , file.errorString() ) );
        return false;
    }

    QString tmp = toString();
    file.write( tmp.toUtf8() );
    file.close();
    return f;
}

/** \brief Transforms log to a readable string. */
QString Log::toString( const QString & settingsLog )
{
    QString tmp;
    tmp = "********************\n";
    tmp += QCoreApplication::translate( "Log", "Debugging informations" );
    tmp += "\n********************\n\n";

    // add logs
    tmp.append( "********************\n" );
    tmp.append( QCoreApplication::translate( "Log", "********** ERRORS *********\n" ) );
    tmp.append( "********************\n\n" );
    QString prec;
    foreach( const LogData &v, m_Messages) {
        if (v.isError()) {
            if (v.object!=prec) {
                tmp += "\n" + v.object;
                tmp += "\n\t";
                prec = v.object;
            } else {
                tmp += "\t";
            }
            tmp += v.message;
            tmp += "\t";
            tmp += v.date.toString("dd/MM/yyyy hh:mm:ss:ms");
            tmp += "\n";
        }
    }
    tmp.append( "********************\n" );
    tmp.append( QCoreApplication::translate( "Log", "********** MESSAGES *********\n" ) );
    tmp.append( "********************\n\n" );
    prec.clear();
    foreach( const LogData &v, m_Messages) {
        if (!v.isError()) {
            if (v.object!=prec) {
                tmp += "\n" + v.object;
                tmp += "\n\t";
                prec = v.object;
            } else {
                tmp += "\t";
            }
            tmp += v.message.leftJustified(100,' ');;
            tmp += "\t";
            tmp += v.date.toString("dd/MM/yyyy hh:mm:ss:ms");
            tmp += "\n";
        }
    }

    // add settings
    tmp += settingsLog + "\n";

    return tmp;
}

/**
  \brief This function is used for debugging purpose.
 Constructs a QTreeWidget with the loggeg messages.\n
  You can specify if you want to have an expanded class tree (by default) or a non expanded chronologic tree.\n
  \sa tkDebugDialog
*/
void Log::messagesToTreeWidget( QTreeWidget *parent, bool expandedByClass )
{
    Q_ASSERT_X(parent, "Log::toTreeWidget", "You must passed a valid QTreeWidget.");
    parent->clear();
    parent->setColumnCount(2);
    QHash<QString, QTreeWidgetItem*> class_item;
    if (expandedByClass) {
        foreach( const LogData &v , m_Messages ) {
            if (v.isError())
                continue;
            // create nex treeitem if class does not already exists
            if (!class_item.keys().contains(v.object))
                class_item.insert( v.object, new QTreeWidgetItem(parent, QStringList() << v.object ) );
            // add message to the class root item
            new QTreeWidgetItem( class_item.value(v.object), QStringList() << v.date.toString("HH:mm:ss:ms") << v.message );
        }
    } else {
        foreach( const LogData &v , m_Messages ) {
            if (v.isError())
                continue;
            new QTreeWidgetItem( parent, QStringList() << v.object << v.message << v.date.toString() );
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
void Log::errorsToTreeWidget( QTreeWidget *parent, bool expandedByClass )
{
    Q_ASSERT_X(parent, "Log::toTreeWidget", "You must passed a valid QTreeWidget.");
    parent->clear();
    parent->setColumnCount(2);
    QHash<QString, QTreeWidgetItem*> class_item;
    if (expandedByClass) {
        foreach( const LogData &v , m_Messages ) {
            if (!v.isError())
                continue;
            // create nex treeitem if class does not already exists
            if (!class_item.keys().contains(v.object))
                class_item.insert( v.object, new QTreeWidgetItem(parent, QStringList() << v.object ) );
            // add message to the class root item
            new QTreeWidgetItem( class_item.value(v.object), QStringList() << v.message << v.date.toString() );
        }
    } else {
        foreach( const LogData &v , m_Messages ) {
            if (v.isError())
                continue;
            new QTreeWidgetItem( parent, QStringList() << v.object << v.message << v.date.toString() );
        }
    }
}
