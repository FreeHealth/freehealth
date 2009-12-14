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

/**
  \class ThemePrivate
  \brief this is the theme manager. Actually manages only the icons of the app.
  You first need to instanciate it, inform it of path to use. Then ask the needed QIcon.\n
  There is a cache of asked icons. Limit of the cache can be dynamycally setted using setCacheMaxCost().

  \ingroup toolkit
  \ingroup object_toolkit
  \sa constants_theme
  \todo write full documentation
*/

#include "theme.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

// include Qt headers
#include <QCache>
#include <QString>
#include <QApplication>
#include <QDir>

using namespace Core;
using namespace Core::Internal;

class ThemePrivatePrivate
{
public:
    QIcon icon( const QString & fileName, ThemePrivate::IconSize size );
    QString iconFullPath( const QString &fileName, ThemePrivate::IconSize size );
    QString transformFileName( const QString & fileName, ThemePrivate::IconSize size );

};

static QString transformFileName( const QString & fileName, ThemePrivate::IconSize size )
{
    switch (size) {
        case ThemePrivate::SmallIcon : return fileName+"__S__";
        case ThemePrivate::MediumIcon : return fileName+"__M__";
        case ThemePrivate::BigIcon : return fileName+"__B__";
    }
    return QString::null;
}


ThemePrivate::ThemePrivate( QObject *parent, const int cacheSize )
    : ITheme(parent)
{
    if (!parent)
        setParent(qApp);
    setObjectName( "ThemePrivate" );
//    Log::instance()->addObjectWatcher(this);
    m_IconCache.setMaxCost( cacheSize );
}

ThemePrivate::~ThemePrivate()
{
}

void ThemePrivate::setThemeRootPath( const QString &absPath )
{
    // guesses all pixmap size paths
    if ( QDir(absPath).exists() ) {
        m_AbsolutePath = QDir::cleanPath(absPath);
        Utils::Log::addMessage(this, QString("Setting theme path to : %1").arg(m_AbsolutePath) );
    }
    else
        Utils::Log::addError( this, Trans::ConstantTranslations::tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(absPath) );
}

void ThemePrivate::setThemeRelativeRootPath( const QString & relPathFromAppBinary )
{
    // guesses all pixmap size paths
    QString path = QDir::cleanPath(qApp->applicationDirPath() + QDir::separator() + relPathFromAppBinary);
    if ( QDir(path).exists() ) {
        m_AbsolutePath = path;
        Utils::Log::addMessage(this, QString("INFO : theme path setted to : %1").arg(path) );
    }
    else
        Utils::Log::addError( this, Trans::ConstantTranslations::tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(relPathFromAppBinary) );
}

void ThemePrivate::refreshCache()
{
    /** \todo */
}

/** \brief Defines the max number of icons in the cache */
void ThemePrivate::setCacheMaxCost( const int max )
{
    m_IconCache.setMaxCost(max);
}

/** \brief Set the specific small icon path - should not be used */
void ThemePrivate::setSmallIconPath( const QString &absPath )
{
    if (QDir(absPath).exists())
        m_SmallIconPath = absPath;
    else
        Utils::Log::addError(this, Trans::ConstantTranslations::tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg("SmallIcon : "+absPath) );
}

/** \brief Set the specific medium icon path - should not be used */
void ThemePrivate::setMediumIconPath( const QString &absPath )
{
    if (QDir(absPath).exists())
        m_MediumIconPath = absPath;
    else
        Utils::Log::addError(this, Trans::ConstantTranslations::tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg("MediumIcon : "+absPath) );
}

/** \brief Set the specific big icon path - should not be used */
void ThemePrivate::setBigIconPath( const QString &absPath )
{
    if (QDir(absPath).exists())
        m_BigIconPath = absPath;
    else
        Utils::Log::addError(this, Trans::ConstantTranslations::tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg("BigIcon : "+absPath) );
}

/** \brief Returns the icon corresponding to the themed file name \e fileName and the size \e size */
QIcon ThemePrivate::icon( const QString & fileName, IconSize size )
{
    Q_ASSERT_X( !m_AbsolutePath.isEmpty(), "ThemePrivate::icon", "No path setted" );
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
            Utils::Log::addError( "ThemePrivate", QCoreApplication::translate("ThemePrivate", "ERROR - Theme : Unable to load icon file %1").arg(fileName) );
        }
    }
    return QIcon();
}

/** \brief Returns the full icon's file name absolute path corresponding to the themed file name \e fileName and the size \e size */
QString ThemePrivate::iconFullPath( const QString &fileName, IconSize size )
{
    QString path = m_AbsolutePath + QDir::separator() + "pixmap" + QDir::separator();
    if (size == ThemePrivate::SmallIcon) {
        if (!m_SmallIconPath.isEmpty())
            path = m_SmallIconPath;
        else
            path += "16x16";
    }
    else if (size == ThemePrivate::MediumIcon) {
        if (!m_MediumIconPath.isEmpty())
            path = m_MediumIconPath;
        else
            path += "32x32";
    }
    else if (size == ThemePrivate::BigIcon) {
        if (!m_BigIconPath.isEmpty())
            path = m_BigIconPath;
        else
            path += "64x64";
    }
    path = QDir::cleanPath(path) + QDir::separator() + fileName;
    return path;
}

/** \brief Returns the themed splashscreen \e fileName. */
QPixmap ThemePrivate::splashScreen( const QString &fileName )
{
    QString file = fileName;
    QString extra;
    switch (QDate::currentDate().month())
    {
        case 7:
        case 8:
        {
            extra = "summer";
            break;
        }
        case 10 :
        {
            extra = "halloween";
            break;
        }
        case 12 :
        {
            extra = "christmas";
            break;
        }
    }
    // does extra theme exists ?
    if (!extra.isEmpty()) {
        QFileInfo fi(file);
        file = fi.baseName() + "-" + extra + "." + fi.completeSuffix();
    }
    if (!QFile(m_AbsolutePath + "/pixmap/splashscreens/" + file).exists())
        file = fileName;

    // return themed splashscreen
    if (QFile(m_AbsolutePath + "/pixmap/splashscreens/" + file).exists() )
        return QPixmap(m_AbsolutePath + "/pixmap/splashscreens/" + file );
    else
        Utils::Log::addError( "ThemePrivate", QString("SplashScreen file does not exists %1").arg(m_AbsolutePath + "/pixmap/splashscreens/" + fileName) );
    return QPixmap();
}
