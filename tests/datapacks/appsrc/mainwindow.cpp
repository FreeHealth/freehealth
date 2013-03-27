/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:                                                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <datapackutils/datapackcore.h>
#include <datapackutils/iservermanager.h>
#include <datapackutils/serverdescription.h>
#include <datapackutils/widgets/serverpackeditor.h>

#include <utils/widgets/genericdescriptioneditor.h>
#include <utils/widgets/genericinformationeditordialog.h>
#include <utils/global.h>

#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxyQuery>

#include <QDebug>

static QString configurationFile() {return QDir::homePath() + "/datapack/servers.conf.xml";}

void testInternet()
{
    QNetworkProxyQuery npq(QUrl("http://www.google.com"));
    QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);
    foreach(const QNetworkProxy &p, listOfProxies) {
        qWarning() << p.capabilities() << p.hostName() << p.port() << p.password() << p.type() << p.user();
    }
}


static DataPack::ServerDescription getDescription()
{
    DataPack::ServerDescription desc;
    desc.setData(DataPack::ServerDescription::Uuid, "uuuid");
    desc.setData(DataPack::ServerDescription::Version, "myversion");
    desc.setData(DataPack::ServerDescription::Author, "me");
    desc.setData(DataPack::ServerDescription::URL, "url://uu");
    desc.setData(DataPack::ServerDescription::AbsFileName, "file://");
    desc.setData(DataPack::ServerDescription::Vendor, "vendor__");
    desc.setData(DataPack::ServerDescription::Validity, "val");
    desc.setData(DataPack::ServerDescription::FreeMedFormsCompatVersion, "fmf v");
    desc.setData(DataPack::ServerDescription::FreeDiamsCompatVersion, "fd v");
    desc.setData(DataPack::ServerDescription::FreeAccountCompatVersion, "fa v");
    desc.setData(DataPack::ServerDescription::CreationDate, "createdate");
    desc.setData(DataPack::ServerDescription::LastModificationDate, "lastModDate");
    desc.setData(DataPack::ServerDescription::GeneralIcon, "__theme__/ok.png");

    desc.addNonTranslatableExtraData(DataPack::ServerDescription::NonTranslatableExtraData + 1, "TrucNonTrans");
    desc.setData(DataPack::ServerDescription::NonTranslatableExtraData + 1, "Blabla nontrans extra");

    desc.setData(DataPack::ServerDescription::Category, "CatFr", "fr");
    desc.setData(DataPack::ServerDescription::Category, "CatXx", "xx");
    desc.setData(DataPack::ServerDescription::ShortDescription, "shortDesc_En", "en");

    desc.addTranslatableExtraData(DataPack::ServerDescription::TranslatableExtraData + 1, "TrucTrans");
    desc.setData(DataPack::ServerDescription::NonTranslatableExtraData + 1, "Trans Extra 1");
    desc.addTranslatableExtraData(DataPack::ServerDescription::TranslatableExtraData + 2, "TrucTrans2");
    desc.setData(DataPack::ServerDescription::NonTranslatableExtraData + 2, "Trans Extra 2");

    Utils::GenericUpdateInformation up;
    up.setFromVersion("0.3.0");
    up.setToVersion("0.4.0");
    up.setText(QString::fromUtf8("Mon texte en français"), "fr");
    up.setText("My text in english (xx)", "xx");
    desc.addUpdateInformation(up);

    up.setFromVersion("0.4.0");
    up.setToVersion("0.4.4");
    up.setText(QString::fromUtf8("Wawww 0.4.4 ?"), "fr");
    up.setText("Four point four", "xx");
    desc.addUpdateInformation(up);
    return desc;
}

static void testServerDescription()
{
    // Test ServerDescription
    DataPack::ServerDescription desc = getDescription();

    DataPack::ServerDescription descCheck;
    descCheck.addNonTranslatableExtraData(DataPack::ServerDescription::NonTranslatableExtraData + 1, "TrucNonTrans");
    descCheck.addTranslatableExtraData(DataPack::ServerDescription::TranslatableExtraData + 1, "TrucTrans");
    descCheck.addTranslatableExtraData(DataPack::ServerDescription::TranslatableExtraData + 2, "TrucTrans2");
    descCheck.fromXmlContent(desc.toXml());

    qWarning() << "** Testing Server Description result";
    if (descCheck.toXml() == desc.toXml())
        qWarning() << "     Ok";
    else
        qWarning() << "     Wrong";
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qWarning();

    testInternet();

    // get the global_resources directory from the command line argument
    QDir resourcesDir;
    if (Utils::isRunningOnLinux()) {
        resourcesDir.setPath(QCoreApplication::applicationDirPath() + "/../../global_resources");
    } else {
        resourcesDir.setPath(QCoreApplication::applicationDirPath() + "/../../../../../global_resources");
    }

    // Create and configure DataPack lib
    DataPack::DataPackCore &core = DataPack::DataPackCore::instance();
//    QNetworkProxy proxy;
//     proxy.setType(QNetworkProxy::HttpProxy);
//     proxy.setHostName("chaisa1");
//     proxy.setPort(1080);
//     proxy.setUser("urg");
//     proxy.setPassword("debut1");
//     core.setNetworkProxy(proxy);

    core.init();
    core.serverManager()->setGlobalConfiguration(Utils::readTextFile(configurationFile()));
    core.setTemporaryCachePath(QDir::tempPath());
    core.setPersistentCachePath(QDir::homePath() + "/datapack/tmp/");
    core.setInstallPath(QDir::homePath() + "/datapack/install/");
    core.registerPathTag("__userDocuments__", QDir::homePath() + "/datapacks/user/");

    // Check internet connection
//    if (!core.isInternetConnexionAvailable())
//        qWarning() << "********* NO INTERNET CONNECTION FOUND *********";

#ifdef Q_OS_MAC
    core.setThemePath(DataPack::DataPackCore::SmallPixmaps, resourcesDir.absoluteFilePath("pixmap/16x16"));
    core.setThemePath(DataPack::DataPackCore::MediumPixmaps, resourcesDir.absoluteFilePath("pixmap/32x32"));
    core.setThemePath(DataPack::DataPackCore::BigPixmaps, resourcesDir.absoluteFilePath("pixmap/64x64"));
#else
    core.setThemePath(DataPack::DataPackCore::SmallPixmaps, resourcesDir.absoluteFilePath("pixmap/16x16"));
    core.setThemePath(DataPack::DataPackCore::MediumPixmaps, resourcesDir.absoluteFilePath("pixmap/32x32"));
    core.setThemePath(DataPack::DataPackCore::BigPixmaps, resourcesDir.absoluteFilePath("pixmap/64x64"));
#endif

    // Add servers
#ifdef Q_OS_MAC
    // Test 1: local
    core.serverManager()->addServer("file://" + resourcesDir.absoluteFilePath("datapacks/default/"));
#else
    // @GUILLAUME --> Change les chemins vers ton SVN ici ::
    // Test 1: local
    core.serverManager()->addServer("file://" + resourcesDir.absoluteFilePath("datapacks/default/"));
    core.serverManager()->addServer("http://localhost/");
#endif

//    // Test 2: HttpPseudoSecuredZipped
//    DataPack::Server http("http://test.freemedforms.com");
//    http.setUrlStyle(DataPack::Server::HttpPseudoSecuredAndZipped);
//    core.serverManager()->addServer(http);

//    // Test 3: HttpPseudoSecuredZipped
//    DataPack::Server ftp("ftp://localhost/");
//    ftp.setUrlStyle(DataPack::Server::Ftp);
//    core.serverManager()->addServer(ftp);

    // Start the check server & pack version
    // TODO: run the core.serverManager()->getAllDescriptionFile(); in a thread ?
    // TODO: Connect the serverManager -> allDescriptionFiles available -> checkForUpdates (in a thread too)
    // TODO: if a Pack update is available -> ask user for the installation

    // Create the ServerEditor view for the test
    DataPack::ServerPackEditor *packManager = new DataPack::ServerPackEditor(this);
    setCentralWidget(packManager);

//    Utils::GenericInformationEditorDialog dlg(this);
//    dlg.setDescription(serverDescr);
//    dlg.exec();

    resize(900,600);
}

MainWindow::~MainWindow()
{
    Utils::saveStringToFile(DataPack::DataPackCore::instance().serverManager()->xmlConfiguration(), configurationFile(), Utils::Overwrite, Utils::DontWarnUser);
    delete ui;
}

