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
