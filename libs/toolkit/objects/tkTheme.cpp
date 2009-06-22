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
#include "tkTheme.h"

// include toolkit headers
#include <tkSettings.h>
#include <tkLog.h>

// include Qt headers
#include <QCache>
#include <QString>
#include <QApplication>
#include <QDir>

class tkThemePrivate
{
public:
    QIcon icon( const QString & fileName, tkTheme::IconSize size );
    QString iconFullPath( const QString &fileName, tkTheme::IconSize size );
    QString transformFileName( const QString & fileName, tkTheme::IconSize size );

    QCache<QString, QIcon> m_IconCache;
    QString m_AbsolutePath;
    QString m_SmallIconPath, m_MediumIconPath, m_BigIconPath;
};

QPointer<tkTheme> tkTheme::m_Instance = 0;

tkTheme *tkTheme::instance()
{
    if (!m_Instance)
        m_Instance = new tkTheme(qApp);
    return m_Instance;
}

tkTheme::tkTheme( QObject *parent, const int cacheSize )
    : QObject(parent), d(new tkThemePrivate())
{
    if (!parent)
        setParent(qApp);
    setObjectName( "tkTheme" );
//    tkLog::instance()->addObjectWatcher(this);
    d->m_IconCache.setMaxCost( cacheSize );
}

tkTheme::~tkTheme()
{
    if (d)
        delete d;
    d=0;
}

void tkTheme::setThemeRootPath( const QString &absPath )
{
    // guesses all pixmap size paths
    if ( QDir(absPath).exists() ) {
        d->m_AbsolutePath = QDir::cleanPath(absPath);
        tkLog::addMessage(this, QString("Setting theme path to : %1").arg(d->m_AbsolutePath) );
    }
    else
        tkLog::addError( this, tr("Theme path (%1) does not exist.").arg(absPath) );
}

void tkTheme::setThemeRelativeRootPath( const QString & relPathFromAppBinary )
{
    // guesses all pixmap size paths
    QString path = QDir::cleanPath(qApp->applicationDirPath() + QDir::separator() + relPathFromAppBinary);
    if ( QDir(path).exists() ) {
        d->m_AbsolutePath = path;
        tkLog::addMessage(this, QString("INFO : theme path setted to : %1").arg(path) );
    }
    else
        tkLog::addError( this, tr("Theme path (%1) does not exist.").arg(relPathFromAppBinary) );
}

void tkTheme::refrehCache()
{
}

void tkTheme::setCacheMaxCost( const int max )
{
    d->m_IconCache.setMaxCost(max);
}

void tkTheme::setSmallIconPath( const QString &absPath )
{
    if (QDir(absPath).exists())
        d->m_SmallIconPath = absPath;
    else
        tkLog::addError(this, tr("Theme path (%1) does not exist.").arg("SmallIcon : "+absPath) );
}

void tkTheme::setMediumIconPath( const QString &absPath )
{
    if (QDir(absPath).exists())
        d->m_MediumIconPath = absPath;
    else
        tkLog::addError(this, tr("Theme path (%1) does not exist.").arg("MediumIcon : "+absPath) );
}

void tkTheme::setBigIconPath( const QString &absPath )
{
    if (QDir(absPath).exists())
        d->m_BigIconPath = absPath;
    else
        tkLog::addError(this, tr("Theme path (%1) does not exist.").arg("BigIcon : "+absPath) );
}

QIcon tkTheme::icon( const QString & fileName, IconSize size )
{
    Q_ASSERT_X( m_Instance, "tkTheme::icon", "Class need to be instanciated before" );
    Q_ASSERT_X( !m_Instance->d->m_AbsolutePath.isEmpty(), "tkTheme::icon", "No path setted" );
    return m_Instance->d->icon( fileName, size );
}

QString tkTheme::iconFullPath( const QString &fileName, IconSize size )
{
    return m_Instance->d->iconFullPath(fileName,size);
}

QString tkThemePrivate::iconFullPath( const QString &fileName, tkTheme::IconSize size )
{
    QString path = m_AbsolutePath + QDir::separator() + "pixmap" + QDir::separator();
    if (size == tkTheme::SmallIcon) {
        if (!m_SmallIconPath.isEmpty())
            path = m_SmallIconPath;
        else
            path += "16x16";
    }
    else if (size == tkTheme::MediumIcon) {
        if (!m_MediumIconPath.isEmpty())
            path = m_MediumIconPath;
        else
            path += "32x32";
    }
    else if (size == tkTheme::BigIcon) {
        if (!m_BigIconPath.isEmpty())
            path = m_BigIconPath;
        else
            path += "64x64";
    }
    path = QDir::cleanPath(path) + QDir::separator() + fileName;
    return path;
}

QIcon tkThemePrivate::icon( const QString & fileName, tkTheme::IconSize size )
{
    // retreive transformed FileName (manage size)
    QString transformedFileName = transformFileName( fileName, size );

    // 0. get icon from cache is possible
    if (m_IconCache.contains(transformedFileName))
        return QIcon(*m_IconCache[transformedFileName]);

    // 1. test size by size if a path was setted else use the absolutePath of the theme
    QIcon *i = 0;

    // 2. get icom from file
    QString fullName = iconFullPath(fileName,size);
    if (QFile( fullName ).exists()) {
        i = new QIcon( fullName );
        if (!i->isNull()) {
            m_IconCache.insert( transformedFileName, i );
            return QIcon(*i);
        } else {
            tkLog::addError( "tkTheme", QCoreApplication::translate("tkTheme", "ERROR - Theme : Unable to load icon file %1").arg(fileName) );
        }
    }
    return QIcon();
}

QString tkThemePrivate::transformFileName( const QString & fileName, tkTheme::IconSize size )
{
    switch (size) {
        case tkTheme::SmallIcon : return fileName+"__S__";
        case tkTheme::MediumIcon : return fileName+"__M__";
        case tkTheme::BigIcon : return fileName+"__B__";
    }
    return QString::null;
}

QPixmap tkTheme::splashScreen( const QString &fileName )
{
    if (QFile(m_Instance->d->m_AbsolutePath + "/pixmap/splashscreens/" + fileName).exists() )
        return QPixmap( m_Instance->d->m_AbsolutePath + "/pixmap/splashscreens/" + fileName );
    else
        tkLog::addError( "tkTheme", QString("SplashScreen file does not exists %1").arg(m_Instance->d->m_AbsolutePath + "/pixmap/splashscreens/" + fileName) );
    return QPixmap();
}
