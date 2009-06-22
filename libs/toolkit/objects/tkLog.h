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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TKLOG_H
#define TKLOG_H

// include toolkit headers
#include <tkExporter.h>
class tkLogPrivate;

// include Qt headers
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QMultiMap>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QObject>
#include <QDebug>
class QTreeWidget;


class tkDataLog
{
public:
    enum tkLogDataType {
        Error = 0,
        CriticalError,
        Warning,
        Message
    };

    tkDataLog( const QString &o, const QString &m, const QDateTime &d, const int t ) :
            object(o), message(m), date(d), type(t) {}
    QString object, message;
    QDateTime date;
    int type;

    bool isError() const
    {
        if ((type == Error) || (type==CriticalError) || (type==Warning))
            return true;
        return false;
    }

    QString toString() const
    {
        return QString("%1 - %2 : %3").arg(object).arg(date.toString(), message);
    }

};


/**
 * \file tkLog.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.5
 * \date 16 June 2009
*/

class Q_TK_EXPORT tkLog
{

    static void addData( const QString &o, const QString &m, const QDateTime &d, const int t )
    {
        m_Messages << tkDataLog(o,m,d,t) ;
        if ((t == tkDataLog::Error) || (t==tkDataLog::CriticalError) || (t==tkDataLog::Warning))
            m_HasError=true;
    }

public:

    static void addMessage( const QObject * o, const QString & msg )
    {
        if (o)
            addMessage(o->objectName(), msg );
        else
            addMessage(o->objectName(), QApplication::translate("tkLog","Unknwown") );
    }

    static void addMessage( const QString & object, const QString & msg )
    {
        qWarning() << msg;
        addData( object, msg, QDateTime::currentDateTime(), tkDataLog::Message );
    }

    static void addMessages( const QObject * o, const QStringList & msg )
    {
        if (o) {
            foreach( const QString & m, msg )
                addMessage(o, m);
        } else {
            foreach( const QString & m, msg )
                addMessage(QApplication::translate("tkLog","Unknwown"), m);
        }
    }

    static void addMessages( const QString &o, const QStringList & msg )
    {
        foreach( const QString & m, msg )
            addMessage(o, m);
    }

    static void addError( const QObject * o, const QString & err )
    {
        if (o)
            addError(o->objectName(), err );
        else
            addError(o->objectName(), QApplication::translate("tkLog","Unknwown") );
    }

    static void addError( const QString & object, const QString & err )
    {
        qWarning() << err;
        addData( object, err, QDateTime::currentDateTime(), tkDataLog::Error );
    }

    static void addErrors( const QObject * o, const QStringList & err )
    {
        if (o) {
            foreach( const QString & m, err )
                addError(o, m);
        } else {
            foreach( const QString & m, err )
                addError(QApplication::translate("tkLog","Unknwown"), m);
        }
    }

    static void addErrors( const QString &o, const QStringList & err )
    {
        foreach( const QString & m, err )
            addError(o, m);
    }


    static void addQueryError( const QObject * o, const QSqlQuery & q )
    {
        qWarning() << QCoreApplication::translate( "tkLog", "SQL Error : Driver : %1, Database : %2, Query : %3" ).arg( q.lastError().driverText(), q.lastError().databaseText(), q.lastQuery() );
        addError(o, QCoreApplication::translate( "tkLog", "%1 : %2 - SQL Error : Driver : %3, Database : %4, Query : %5" )
                 .arg( o->objectName(), QDateTime::currentDateTime().toString(), q.lastError().driverText(), q.lastError().databaseText(), q.lastQuery() ) );
    }

    static void addQueryError( const QString & o, const QSqlQuery & q )
    {
        qWarning() << QCoreApplication::translate( "tkLog", "SQL Error : Driver : %1, Database : %2, Query : %3" ).arg( q.lastError().driverText(), q.lastError().databaseText(), q.lastQuery() );
        addError(o, QCoreApplication::translate( "tkLog", "%1 : %2 - SQL Error : Driver : %3, Database : %4, Query : %5" )
                .arg( o, QDateTime::currentDateTime().toString(), q.lastError().driverText(), q.lastError().databaseText(), q.lastQuery() ) );
        qWarning() << q.lastError();
    }

    /** \brief Add a message to tkLog containing the elapsed time of \t and restart it. Used for debugging purpose. */
    static void logTimeElapsed( QTime &t, const QString &object, const QString &forDoingThis )
    {
        addMessage( "Chrono - " + object, QCoreApplication::translate( "tkLog", "%1 ms : %2")
                    .arg(t.elapsed()).arg(forDoingThis));
        t.restart();
    }

    static bool hasError()        { return  m_HasError; }

    static QStringList messages()
    {
        QStringList r;
        foreach( const tkDataLog &v, m_Messages)
            if (v.type == tkDataLog::Message)
                r << v.toString();
        return r;
    }

    static QStringList errors()
    {
        QStringList r;
        foreach( const tkDataLog &v, m_Messages)
            if (v.isError())
                r << v.toString();
        return r;
    }

    static QString     toString( const QString & settingsLog = QString::null );
    static QString     saveLog( const QString & fileName = QString::null );

    static void messagesToTreeWidget( QTreeWidget *parent, bool expandedByClass = true );
    static void errorsToTreeWidget( QTreeWidget *parent, bool expandedByClass = true );

private:
//    static QStringList m_Messages;
//    static QStringList m_Errors;
    static QList<tkDataLog> m_Messages;
    static bool m_HasError;
};

#endif // TKLOG_H
