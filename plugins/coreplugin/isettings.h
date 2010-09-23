/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef CORESETTINGS_H
#define CORESETTINGS_H

// include toolkit headers
#include <coreplugin/core_exporter.h>

// include Qt headers
#include <QVariant>
QT_BEGIN_NAMESPACE
class QStringList;
class QTreeWidget;
class QMainWindow;
class QSettings;
QT_END_NAMESPACE

/**
 * \file settings.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 18 Mar 2010
*/

namespace Core {

class CORE_EXPORT ISettings
{
public:

    enum Paths {
        /** \todo rename ResourcesPath to UserResourcesPath */
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
        ThemeRootPath,
        SmallPixmapPath,
        MediumPixmapPath,
        BigPixmapPath,
        SystemTempPath,
        ApplicationTempPath,
        FormsPath,
        SampleFormsPath,
        DocumentationPath,
        WebSiteUrl,
        UpdateUrl
    };

    virtual ~ISettings() {}

    virtual QSettings *getQSettings() = 0;

    // QSettings wrappers
    virtual void beginGroup( const QString & prefix ) = 0;
    virtual QStringList childGroups() const = 0;
    virtual QStringList childKeys() const = 0;
    virtual bool contains( const QString & key ) const = 0;
    virtual void endGroup() = 0;
    virtual QString fileName() const = 0;
    virtual QString group() const = 0;

    virtual void setValue( const QString & key, const QVariant & value ) = 0;
    virtual QVariant value( const QString & key, const QVariant & defaultValue = QVariant() ) const = 0;

    virtual bool firstTimeRunning() const = 0;
    virtual void noMoreFirstTimeRunning() = 0;

    virtual void sync() = 0;

    // Window's states
    virtual void restoreState( QMainWindow * window, const QString & prefix = QString::null ) = 0;
    virtual void saveState( QMainWindow * window, const QString & prefix = QString::null ) = 0;

    // paths
    virtual void setPath( const int type, const QString & absPath ) = 0;
    virtual QString path( const int type ) const = 0;
    virtual QString resourcesPath() const { return path( ResourcesPath ); }
    virtual QString databasePath() const  { return path( ReadOnlyDatabasesPath ); }

    // values management
    virtual void appendToValue( const QString &key, const QString &value ) = 0;

    // for debugging functions : to treewidget and to string
    virtual QTreeWidget* getTreeWidget(QWidget *parent) const = 0;
    virtual QString toString() const = 0;

};

} // End Core

#endif // CORESETTINGS_H
