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
 *   Some part of this code is inspired of the Monkey Studio project       *
 *   http://www.monkeystudio.org/                                          *
 *   Filipe AZEVEDO aka Nox P@sNox <pasnox@gmail.com>                      *
 *                                                                         *
 *   Adaptations to FreeMedForms by : Eric Maeker, MD                      *
 *   eric.maeker@free.fr                                                   *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TKSETTINGS_H
#define TKSETTINGS_H

// include toolkit headers
#include <tkExporter.h>

// include Qt headers
#include <QSettings>
#include <QStringList>
#include <QDir>
#include <QHash>
class QMainWindow;
class QTreeWidget;
class tkSettingsPrivate;

/**
 * \file tkSettings.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.5
 * \date 10 June 2009
*/


class Q_TK_EXPORT tkSettings : public QSettings
{
    Q_OBJECT
public:

    /** */
    enum Paths {
        ResourcesPath = 0,
        ApplicationPath,
        BundleRootPath,
        BundleResourcesPath,
        ReadOnlyDatabasesPath,
        ReadWriteDatabasesPath,
        TranslationsPath,
        QtPlugInsPath,
        QtFrameWorksPath,
        FMFPlugInsPath,
        SmallPixmapPath,
        MediumPixmapPath,
        BigPixmapPath,
        SystemTempPath,
        ApplicationTempPath,
        FormsPath,
        SampleFormsPath,
        WebSiteUrl
    };

    /** */
    static tkSettings* instance();
    /** */
    tkSettings( QObject * parent = 0, const QString & appName = QString::null, const QString & fileName = QString::null );
    /** */
    ~tkSettings();

    // first time running ?
    /** */
    virtual bool firstTimeRunning();
    /** */
    virtual void noMoreFirstTimeRunning();

    // manage mainwindow
    /** */
    virtual void restoreState( QMainWindow * window, const QString & prefix = QString::null );
    /** */
    virtual void saveState( QMainWindow * window, const QString & prefix = QString::null );

    // paths
    /**
        \brief Use the \e Paths enum to set and retrieve paths.
               This function automatically recalculates the path when setting : ApplicationPath, BundleResourcesPath adn ResourcesPath.
               It also inform tkTheme (that is instanciate by tkGlobal::initLib()) of the default theme to use.
    */
    virtual void      setPath( const int type, const QString & absPath );    // Use enum with this member, or
    /** Use the \e Paths enum to set and retrieve paths. */
    virtual QString   path( const int type ) const;                          // redefine your own enum (based on the above)

    /** */
    QString resourcesPath() const { return path( ResourcesPath ); }
    /** */
    QString databasePath() const  { return path( ReadOnlyDatabasesPath ); }

    // for debugging functions : to treewidget and to string
    /** \brief For debugging purpose. */
    virtual QTreeWidget* getTreeWidget( QWidget * parent ) const;
    /** \brief For debugging purpose. */
    virtual QString toString() const;

    // values management
    /** */
    virtual void appendToValue( const QString &key, const QString &value );

private:
    /** */
    QString getIniFile( const QString & name = QString::null, const QString & version = QString::null );

protected:
    tkSettingsPrivate * d;
    static tkSettings* m_Instance;
};

#endif // TKSETTINGS_H
