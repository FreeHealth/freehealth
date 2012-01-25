#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <datapackutils/core.h>
#include <datapackutils/iservermanager.h>
#include <datapackutils/serverdescription.h>
#include <datapackutils/widgets/servereditor.h>

#include <utils/widgets/genericdescriptioneditor.h>
#include <utils/widgets/genericinformationeditordialog.h>
#include <utils/global.h>

#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QDebug>

static QString configurationFile() {return QDir::homePath() + "/datapack/servers.conf.xml";}

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

    // get the global_resources directory from the command line argument
    QDir resourcesDir(QCoreApplication::applicationDirPath());
    QStringList list = QCoreApplication::arguments();
    int index = list.indexOf(QRegExp("--config=*", Qt::CaseSensitive, QRegExp::Wildcard));
    if (index >= 0) {
        QString iniFileName = list[index];
        iniFileName = iniFileName.mid(iniFileName.indexOf("=") + 1);

        resourcesDir = QFileInfo(resourcesDir.filePath(iniFileName)).absolutePath();
    } else // ugly shortcut only valid for eric when no --config parameter has been provided :)
        resourcesDir = "/Users/eric/Desktop/Programmation/freemedforms/global_resources";

    // Create and configure DataPack lib
    DataPack::Core &core = DataPack::Core::instance();
    core.serverManager()->setGlobalConfiguration(Utils::readTextFile(configurationFile()));
    core.setTemporaryCachePath(QDir::tempPath());
    core.setPersistentCachePath(QDir::homePath() + "/datapack/tmp/");
    core.setInstallPath(QDir::homePath() + "/datapack/install/");

    // Check internet connection
    core.isInternetConnexionAvailable();

#ifdef Q_OS_MAC
    core.setThemePath(DataPack::Core::SmallPixmaps, resourcesDir.absoluteFilePath("pixmap/16x16"));
    core.setThemePath(DataPack::Core::MediumPixmaps, resourcesDir.absoluteFilePath("pixmap/32x32"));
    core.setThemePath(DataPack::Core::BigPixmaps, resourcesDir.absoluteFilePath("pixmap/64x64"));
    // Test 1: local
    core.serverManager()->addServer("file://" + resourcesDir.absoluteFilePath("datapacks/default/"));
#else
    core.setThemePath(DataPack::Core::SmallPixmaps, resourcesDir.absoluteFilePath("pixmap/16x16"));
    core.setThemePath(DataPack::Core::MediumPixmaps, resourcesDir.absoluteFilePath("pixmap/32x32"));
    core.setThemePath(DataPack::Core::BigPixmaps, resourcesDir.absoluteFilePath("pixmap/64x64"));
    // Test 1: local
    core.serverManager()->addServer("file://" + resourcesDir.absoluteFilePath("datapacks/default/"));
    core.serverManager()->addServer("http://localhost/");
#endif

// Add servers
#ifdef Q_OS_MAC
    // Test 1: local
    core.serverManager()->addServer("file://" + resourcesDir.absoluteFilePath("datapacks/default/"));
#else
    // Test 1: local
    core.serverManager()->addServer("file://" + resourcesDir.absoluteFilePath("datapacks/default/"));
    core.serverManager()->addServer("http://localhost/");
#endif

    // Test 2: HttpPseudoSecuredZipped
    DataPack::Server http("http://test.freemedforms.com");
    http.setUrlStyle(DataPack::Server::HttpPseudoSecuredAndZipped);
    core.serverManager()->addServer(http);

    // Test 3: HttpPseudoSecuredZipped
    DataPack::Server ftp("ftp://localhost/");
    ftp.setUrlStyle(DataPack::Server::Ftp);
    core.serverManager()->addServer(ftp);

    // TEST
    qWarning() << "Drugs installed ?" << core.serverManager()->isDataPackInstalled("freemedforms.certified.drugs.completebase.allcountries","0.6.0");
    // END

    // Start the check server & pack version
    // TODO: run the core.serverManager()->getAllDescriptionFile(); in a thread ?
    // TODO: Connect the serverManager -> allDescriptionFiles available -> checkForUpdates (in a thread too)
    // TODO: if a Pack update is available -> ask user for the installation

    // Create the ServerEditor view for the test
    DataPack::ServerEditor *serverEditor = new DataPack::ServerEditor(this);
    setCentralWidget(serverEditor);

//    Utils::GenericInformationEditorDialog dlg(this);
//    dlg.setDescription(serverDescr);
//    dlg.exec();

    resize(900,600);
}

MainWindow::~MainWindow()
{
    Utils::saveStringToFile(DataPack::Core::instance().serverManager()->xmlConfiguration(), configurationFile(), Utils::Overwrite, Utils::DontWarnUser);
    delete ui;
}

