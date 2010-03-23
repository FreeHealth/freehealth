/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#ifndef CORESETTINGSPRIVATE_H
#define CORESETTINGSPRIVATE_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/isettings.h>

#include <QSettings>
#include <QStringList>
#include <QDir>
#include <QHash>

QT_BEGIN_NAMESPACE
class QMainWindow;
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file settings_p.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 23 Mar 2010
*/

namespace Core {
namespace Internal {

class CORE_EXPORT SettingsPrivate : public ISettings, public QSettings
{
public:
    SettingsPrivate(QObject *parent = 0, const QString &appName = QString::null, const QString &fileName = QString::null );
    ~SettingsPrivate();

    QSettings *getQSettings();

    // QSettings wrappers
    inline void beginGroup ( const QString & prefix ) { QSettings::beginGroup(prefix); }
    inline QStringList childGroups () const { return QSettings::childGroups(); }
    inline QStringList childKeys () const { return QSettings::childKeys(); }
    inline bool contains ( const QString & key ) const { return QSettings::contains(key); }
    inline void endGroup () { QSettings::endGroup(); }
    inline QString fileName () const { return QSettings::fileName(); }
    inline QString group () const { return QSettings::group();}

    inline void setValue( const QString & key, const QVariant & value ) { QSettings::setValue(key,value); }
    inline QVariant value( const QString & key, const QVariant & defaultValue = QVariant() ) const { return QSettings::value(key,defaultValue); }

    inline void sync() {QSettings::sync();}

    // first time running ?
    bool firstTimeRunning() const;
    void noMoreFirstTimeRunning();
    QString licenseApprovedApplicationNumber() const;
    void setLicenseApprovedApplicationNumber(const QString &version);

    // manage mainwindow
    void restoreState( QMainWindow * window, const QString & prefix = QString::null );
    void saveState( QMainWindow * window, const QString & prefix = QString::null );

    // paths
    void setPath( const int type, const QString & absPath );
    QString path( const int type ) const;

    // for debugging functions : to treewidget and to string
    QTreeWidget* getTreeWidget( QWidget * parent ) const;
    QString toString() const;

    // values management
    void appendToValue( const QString &key, const QString &value );    

protected:
    QString getIniFile(const QString &name = QString::null, const QString &version = QString::null);

private:
    QHash< int, QString >   m_Enum_Path;
    bool                    m_FirstTime;
};

} // End Internal
} // End Core

#endif // CORESETTINGSPRIVATE_H
