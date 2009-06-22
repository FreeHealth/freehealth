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
//QStringList tkLog::m_Messages = QStringList();
//QStringList tkLog::m_Errors = QStringList();

//tkLog *tkLog::m_Instance = 0;
//
//class tkLogPrivate {
//public:
//    QList<QObject *> m_WatchedObject;
//};
//
//
//tkLog *tkLog::instance()
//{
//    if (!m_Instance)
//        m_Instance = new tkLog(qApp);
//    return m_Instance;
//}
//
//tkLog::tkLog(QObject *o) :
//        QObject(o), d(0)
//{
//    setObjectName("tkLog");
//    d = new tkLogPrivate();
//}
//
//tkLog::~tkLog()
//{
//    qWarning() << tr("Object destroyed : %1").arg(this->objectName());
//    qWarning() << tr("Object watched list") << d->m_WatchedObject;
//    if (d) delete d;
//    d=0;
//}
//
//void tkLog::addObjectWatcher( QObject *o )
//{
//    if (o) {
//        o->setProperty( TKLOG_MARK, QVariant(TKLOG_MARK) );
//        if (!d->m_WatchedObject.contains(o)) {
//            d->m_WatchedObject<< o;
//        }
//        connect(o, SIGNAL(destroyed(QObject*)), this, SLOT(on_watchedObject_destroyed(QObject*)));
//    }
//}
//
//void tkLog::on_watchedObject_destroyed(QObject *o)
//{
//    if (o->property(TKLOG_MARK).toString() == TKLOG_MARK) {
//        d->m_WatchedObject.removeAll(o);
//        addMessage( TKLOG_WATCHER_NAME, tr("Object destroyed : %1").arg(o->objectName()));
//    }
//}

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
