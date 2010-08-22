#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../../libs/utils/widgets/combowithfancybutton.h"

#include <QModelIndex>
#include <QRect>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QAbstractItemModel>
#include <QPainter>
#include <QIcon>
#include <QComboBox>
#include <QTableView>
#include <QTreeView>
#include <QHeaderView>
#include <QStringListModel>
#include <QWidget>
#include <QGridLayout>


#include <QMouseEvent>
#include <QApplication>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Utils::ComboWithFancyButton *c = new Utils::ComboWithFancyButton(this);
//    QComboBox *c = new QComboBox(this);
    c->addItems(QStringList() << "Essai 1" << "Essai2" << "Essai 3" << "Essai 4");
    c->addItems(QStringList() << "Essai 1" << "Essai2" << "Essai 3" << "Essai 4");
    c->addItem("Porut");
    c->setRemoveLightIcon(QIcon(qApp->applicationDirPath() + "/../../../../../../global_resources/pixmap/16x16/closebuttonlight.png"));
    c->setMoveUpLightIcon(QIcon(qApp->applicationDirPath() + "/../../../../../../global_resources/pixmap/16x16/arrowuplight.png"));
    c->setMoveDownLightIcon(QIcon(qApp->applicationDirPath() + "/../../../../../../global_resources/pixmap/16x16/arrowdownlight.png"));
    c->setMoveItems(true);
    c->setRemoveItems(false);
    QWidget *w = new QWidget(this);
    QGridLayout *lay = new QGridLayout(w);
    lay->addWidget(c);

    this->setCentralWidget(w);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
