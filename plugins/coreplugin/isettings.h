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
#ifndef CORESETTINGS_H
#define CORESETTINGS_H

// include toolkit headers
#include <coreplugin/core_exporter.h>

// include Qt headers
#include <QVariant>
QT_BEGIN_NAMESPACE
class QStringList;
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file settings.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.12
 * \date 10 Aug 2009
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
        WebSiteUrl
    };

    virtual ~ISettings() {}

    // QSettings wrappers
    virtual void beginGroup ( const QString & prefix ) = 0;
    virtual QStringList childGroups () const = 0;
    virtual QStringList childKeys () const = 0;
    virtual bool contains ( const QString & key ) const = 0;
    virtual void endGroup () = 0;
    virtual QString fileName () const = 0;
    virtual QString group () const = 0;

    virtual void setValue ( const QString & key, const QVariant & value ) = 0;
    virtual QVariant value ( const QString & key, const QVariant & defaultValue = QVariant() ) const = 0;

    virtual void sync() = 0;

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
