/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "fakehtmlmodel.h"
//#include "AnnouncementItemDelegate.h"

#include "ui_mainwindow.h"

#include <utils/global.h>
#include <utils/widgets/htmldelegate.h>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStandardItemModel *model = new FakeHtmlModel(this);

    ui->listView->setModel(model);
    ui->listView->setItemDelegate(new Utils::HtmlDelegate(this));
    ui->listView->setUniformItemSizes(false);
    ui->listView->setResizeMode(QListView::Adjust);
//    ui->listView->setTextElideMode(Qt::ElideRight);

    ui->treeView->setModel(model);
    ui->treeView->setItemDelegate(new Utils::HtmlDelegate(this));
    ui->treeView->setUniformRowHeights(false);
    ui->treeView->setIndentation(10);

    ui->tableView->setModel(model);
    ui->tableView->setItemDelegate(new Utils::HtmlDelegate(this));

    resize(900,600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

