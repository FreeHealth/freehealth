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
#ifndef TKMEDINTUXCONFIGURATION_H
#define TKMEDINTUXCONFIGURATION_H

#include <tkMedintuxExporter.h>

class QString;
class QVariant;
class tkMedintuxConfigurationPrivate;

#include <QObject>

/**
  \ingroup medintuxtoolkit
*/
class Q_TKMT_EXPORT tkMedintuxConfiguration : public QObject
{
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

    static tkMedintuxConfiguration *instance();
    ~tkMedintuxConfiguration();

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
    QString defaultListsPath(DefaultList rubrik) const;
    bool deleteListCache() const;

    QString drtuxUserMenuPath() const;
    QString drtuxResourcesPath() const;

    bool addUserMenu( const QString &shortTitle, const QString &title, const QString &script, const QString &iconAbsPath, const QString &keySequence );


private:
    tkMedintuxConfiguration( QObject *parent );

    static tkMedintuxConfiguration *m_Instance;
    tkMedintuxConfigurationPrivate *d;
};

#endif // TKMEDINTUXCONFIGURATION_H
