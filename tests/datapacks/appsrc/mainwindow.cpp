#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <datapackutils/serverdescription.h>
#include <utils/widgets/genericdescriptioneditor.h>

#include <QLayout>
#include <QDebug>

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
    qWarning() << descCheck.toXml();
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qWarning();
    testServerDescription();
    serverDescr = getDescription();
    Utils::GenericDescriptionEditor *editor = new Utils::GenericDescriptionEditor(this);
    editor->setDescription(serverDescr);
    layout()->addWidget(editor);
}

MainWindow::~MainWindow()
{
    delete ui;
}
