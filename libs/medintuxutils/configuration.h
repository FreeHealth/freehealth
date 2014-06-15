/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MEDINTUXCONFIGURATION_H
#define MEDINTUXCONFIGURATION_H

#include <medintuxutils/medintux_exporter.h>

class QString;
class QVariant;

#include <QObject>
namespace MedinTux {
namespace Internal {
class ConfigurationPrivate;
}

/**
  \ingroup medintuxtoolkit
*/
class MEDINTUX_EXPORT Configuration : public QObject
{
    Q_OBJECT
public:
    enum GlossaryFor {
        ObservationGlossary,
        PrescriptionGlossary,
        DocumentGlossary,
        TerrainGlossary,
        ChampsInsertionGlossary
    };

    enum DefaultList {
        ObservationDefaultList,
        PrescriptionDefaultList,
        DocumentDefaultList,
        ImageDefaultList
    };

    enum PluginsParameters {
        PlugIn_IniFile = 1,
        PlugIn_PathOfMedintuxBinary,
        PlugIn_ExchangeFilePath,
        PlugIn_PatientGUID,
        PlugIn_PrimKey_Obs,
        PlugIn_PrimKey_Terrain,
        PlugIn_PrimKey_Ordo,
        PlugIn_PrimKey_User,
        PlugIn_Unused1,
        PlugIn_Unused2,
        PlugIn_Unused3,
        PlugIn_NumberOfPlugInSpecificArgs,
        PlugIn_NumberOfDrTuxArgs
    };

    static Configuration *instance();
    ~Configuration();

    // Use these functions for medintux plugins
    bool applicationIsAMedintuxPlugins() const;
    QString medintuxPluginIniFileName() const;
    QString medintuxPluginIni(const QString &rubrik, const QString &key ) const;
    QString medintuxPluginInformation( PluginsParameters info ) const;

    // Use these functions if application is not a medintux plugin
    QString findManagerBinaryPath() const;
    QString askUserForManagerBinary() const;
    bool    setManagerBinaryPath(const QString &absPath);

    QString managerIniFileName() const;
    QVariant managerIni( const QString &rubrik, const QString &key ) const;

    QString drtuxBinaryPath() const;
    QString drtuxIniFileName() const;
    QVariant drtuxIni( const QString &rubrik, const QString &key ) const;

    QString glossaryPath() const;
    QString glossaryPath(GlossaryFor rubrik) const;

    QString menuLibraryPath() const;
    QString menuListsPath(DefaultList rubrik) const;
    bool deleteListCache() const;

    QString drtuxUserMenuPath() const;
    QString drtuxResourcesPath() const;

    bool addUserMenu( const QString &shortTitle, const QString &title, const QString &script, const QString &iconAbsPath, const QString &keySequence );


private:
    Configuration( QObject *parent );

    static Configuration *m_Instance;
    Internal::ConfigurationPrivate *d;
};


}

#endif // MEDINTUXCONFIGURATION_H
