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
  \class tkDataLog
  Represents a message to log.
  \ingroup toolkit
  \ingroup object_toolkit
  \sa tkLog
*/



/**
  \class tkLog
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
  \sa tkDataLog
*/

#include "tkLog.h"

#include <QFile>
#include <QDir>
#include <QString>
#include <QVariant>
#include <QTreeWidgetItem>
#include <QTreeWidget>

//Q_TK_USING_CONSTANTS

QList<tkDataLog> tkLog::m_Messages;
bool tkLog::m_HasError = false;
bool tkLog::m_MuteConsole = false;

/** \brief Saves the log to the file \e fileName */
QString tkLog::saveLog( const QString & fileName )
{
    QString f = fileName;
    if ( fileName.isEmpty() )
        f = QDir::homePath() + "/FMF_LOG.TXT";

    QFile file( f );
    if ( ! file.open( QFile::WriteOnly ) )
    {
        tkLog::addError( "tkLog", QCoreApplication::translate( "tkLog", "Unable to save %1 : Error %2" ).arg( f , file.errorString() ) );
        return false;
    }

    QString tmp = toString();
    file.write( tmp.toUtf8() );
    file.close();
    return f;
}

/** \brief Transforms log to a readable string. */
QString tkLog::toString( const QString & settingsLog )
{
    QString tmp;
    tmp = "********************\n";
    tmp += QCoreApplication::translate( "tkLog", "Debugging informations" );
    tmp += "\n********************\n\n";

    // add logs
    tmp.append( "********************\n" );
    tmp.append( QCoreApplication::translate( "tkLog", "********** ERRORS *********\n" ) );
    tmp.append( "********************\n\n" );
    tmp.append( errors().join( "\n" ) );
    tmp.append( "********************\n" );
    tmp.append( QCoreApplication::translate( "tkLog", "********** MESSAGES *********\n" ) );
    tmp.append( "********************\n\n" );
    tmp.append( messages().join( "\n" ) );

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
void tkLog::messagesToTreeWidget( QTreeWidget *parent, bool expandedByClass )
{
    Q_ASSERT_X(parent, "tkLog::toTreeWidget", "You must passed a valid QTreeWidget.");
    parent->clear();
    parent->setColumnCount(2);
    QHash<QString, QTreeWidgetItem*> class_item;
    if (expandedByClass) {
        foreach( const tkDataLog &v , m_Messages ) {
            if (v.isError())
                continue;
            // create nex treeitem if class does not already exists
            if (!class_item.keys().contains(v.object))
                class_item.insert( v.object, new QTreeWidgetItem(parent, QStringList() << v.object ) );
            // add message to the class root item
            new QTreeWidgetItem( class_item.value(v.object), QStringList() << v.message << v.date.toString() );
        }
    } else {
        foreach( const tkDataLog &v , m_Messages ) {
            if (v.isError())
                continue;
            new QTreeWidgetItem( parent, QStringList() << v.object << v.message << v.date.toString() );
        }
    }
}

/**
  \brief This function is used for debugging purpose.
 Constructs a QTreeWidget with the loggeg errors.\n
  You can specify if you want to have an expanded class tree (by default) or a non expanded chronologic tree.\n
  \sa tkDebugDialog
*/
void tkLog::errorsToTreeWidget( QTreeWidget *parent, bool expandedByClass )
{
    Q_ASSERT_X(parent, "tkLog::toTreeWidget", "You must passed a valid QTreeWidget.");
    parent->clear();
    parent->setColumnCount(2);
    QHash<QString, QTreeWidgetItem*> class_item;
    if (expandedByClass) {
        foreach( const tkDataLog &v , m_Messages ) {
            if (!v.isError())
                continue;
            // create nex treeitem if class does not already exists
            if (!class_item.keys().contains(v.object))
                class_item.insert( v.object, new QTreeWidgetItem(parent, QStringList() << v.object ) );
            // add message to the class root item
            new QTreeWidgetItem( class_item.value(v.object), QStringList() << v.message << v.date.toString() );
        }
    } else {
        foreach( const tkDataLog &v , m_Messages ) {
            if (v.isError())
                continue;
            new QTreeWidgetItem( parent, QStringList() << v.object << v.message << v.date.toString() );
        }
    }
}
