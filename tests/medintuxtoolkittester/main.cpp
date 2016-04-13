/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2016 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
#include <QApplication>

#include <QtGui>

#include <tkMedintuxConfiguration.h>
#include <tkGlobal.h>
#include <tkDebugDialog.h>
#include <tkSettings.h>
#include <tkLog.h>


/**
  \brief This project is only a tester of the FreeMedForms' medintuxtoolkit lib
  \author Eric MAEKER, MD <eric.maeker@gmail.com>
*/


void warnPathAndValues(tkMedintuxConfiguration *conf)
{
    // if manager bin is not defined, automatically searches for it.
    tkLog::addMessage( "main", conf->managerIniFileName());
    tkLog::addMessage( "main", conf->drtuxIniFileName());
    tkLog::addMessage( "main", conf->glossaryPath());
    tkLog::addMessage( "main", conf->glossaryPath(tkMedintuxConfiguration::ChampsInsertionGlossary));
    tkLog::addMessage( "main", conf->glossaryPath(tkMedintuxConfiguration::ObservationGlossary));
    tkLog::addMessage( "main", conf->glossaryPath(tkMedintuxConfiguration::PrescriptionGlossary));
    tkLog::addMessage( "main", conf->glossaryPath(tkMedintuxConfiguration::TerrainGlossary));
    tkLog::addMessage( "main", conf->glossaryPath(tkMedintuxConfiguration::DocumentGlossary));
    qWarning();

    tkLog::addMessage( "main", conf->defaultListsPath(tkMedintuxConfiguration::DocumentDefaultList));
    tkLog::addMessage( "main", conf->defaultListsPath(tkMedintuxConfiguration::PrescriptionDefaultList));
    tkLog::addMessage( "main", conf->defaultListsPath(tkMedintuxConfiguration::ObservationDefaultList));
    tkLog::addMessage( "main", conf->defaultListsPath(tkMedintuxConfiguration::ImageDefaultList));

    tkLog::addMessage( "main", conf->drtuxUserMenuPath());
    tkLog::addMessage( "main", conf->drtuxResourcesPath());
}


void warnIniValues( const QString &filename )
{
    QSettings s(filename, QSettings::IniFormat );
    QString n = QFileInfo(filename).fileName();
    foreach( const QString &keys, s.allKeys() )
        tkLog::addMessage(n, QString("%1\t%2").arg(keys,s.value(keys).toString()));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName( "tkMedintuxTester" );
    app.setApplicationVersion( "0.0.1" );

    tkSettings s;

    tkMedintuxConfiguration *conf = tkMedintuxConfiguration::instance();
    if (conf->applicationIsAMedintuxPlugins())
        tkLog::addMessage("main", "started as a plugins of medintux = true");
    else
        tkLog::addMessage("main", "started as a plugins of medintux = false");

    for (int i=1; i<13;++i) {
        tkLog::addMessage(0, conf->medintuxPluginInformation( tkMedintuxConfiguration::PluginsParameters(i) ) );
    }

//    qWarning() << conf.findManagerBinaryPath();
//    qWarning() << conf.setManagerBinaryPath("/Applications/MedinTux-V2.12-Mac-Intel-105/Programmes/Manager/bin/");
    warnPathAndValues(conf);

    warnIniValues(conf->drtuxIniFileName());
    warnIniValues(conf->managerIniFileName());

    //QMainWindow w;
    tkDebugDialog d(0,&s);
    //w.setCentralWidget( d );
    d.exec();
    return 0;
}




