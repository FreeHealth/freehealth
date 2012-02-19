#include "mainwindow.h"
#include "zipcountrycompleters.h"
#include "frenchsocialnumber.h"

#include "ui_mainwindow.h"


#include <QDir>
#include <QDebug>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ZipCountryCompleters *completer = new ZipCountryCompleters(this);
    ui->zipLineEdit->setText("62000");
    ui->cityLineEdit->setText("ARRAES");
    completer->setCountryComboBox(ui->comboBox);
    completer->setZipLineEdit(ui->zipLineEdit);
    completer->setCityLineEdit(ui->cityLineEdit);
    completer->checkData();

    // add NumSS
    ui->gridLayout->addWidget(new FrenchSocialNumber(this), 10, 0, 1, 2);

//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "ZIPS");
//    db.setDatabaseName(QDir::cleanPath(qApp->applicationDirPath() + "/../../../../../global_resources/databases/zipcodes/zipcodes.db"));
//    db.open();
//    ui->listView->setModel(new ZipCountryModel(this, db));
//    ui->listView->setModelColumn(ZipCountryModel::ZipCity);
//    qWarning() << ui->listView->modelColumn();
}

MainWindow::~MainWindow()
{
    delete ui;
}
