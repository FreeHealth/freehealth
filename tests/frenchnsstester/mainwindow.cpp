#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../plugins/basewidgetsplugin/frenchsocialnumberwidget.h"


#include <QLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    layout()->addWidget(new BaseWidgets::Internal::FrenchSocialNumberWidget(this));
}

MainWindow::~MainWindow()
{
    delete ui;
}
