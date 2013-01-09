#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../plugins/basewidgetsplugin/frenchsocialnumberwidget.h"

#include <QLayout>
#include <QLineEdit>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    BaseWidgets::Internal::FrenchSocialNumberWidget *w = new BaseWidgets::Internal::FrenchSocialNumberWidget(this);
    layout()->addWidget(w);
    w->setNumberWithoutControlKey("154012A111185");
    // qWarning() << w->numberWithControlKey() << w->numberWithoutControlKey();
}

MainWindow::~MainWindow()
{
    delete ui;
}
