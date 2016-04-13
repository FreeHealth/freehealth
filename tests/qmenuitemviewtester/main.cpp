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
#include <QMainWindow>
#include <QVBoxLayout>
#include <QToolButton>

#include <QStandardItemModel>
#include <QFileSystemModel>

#include "../../libs/utils/widgets/qmenuitemview.h"

class TestModel : public QStandardItemModel
{
public:
    TestModel(QObject *parent):
        QStandardItemModel(parent)
    {
        QStandardItem *item1 = new QStandardItem("Item1");
        QStandardItem *item1_1 = new QStandardItem("Item1.1");
        QStandardItem *item1_2 = new QStandardItem("Item1.2");
        QStandardItem *item1_3 = new QStandardItem("Item1.3");
        item1->appendRow(item1_1);
        item1->appendRow(item1_2);
        item1->appendRow(item1_3);
        QStandardItem *item2 = new QStandardItem("Item2");
        QStandardItem *item2_1 = new QStandardItem("Item2.1");
        QStandardItem *item2_2 = new QStandardItem("Item2.2");
        QStandardItem *item2_3 = new QStandardItem("Item2.3");
        item2->appendRow(item2_1);
        item2->appendRow(item2_2);
        item2->appendRow(item2_3);
        invisibleRootItem()->appendRow(item1);
        invisibleRootItem()->appendRow(item2);
    }
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow *mainWin = new QMainWindow;
    QWidget *widget = new QWidget(mainWin);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    widget->setLayout(layout);

//    Utils::QMenuItemView *mv = new Utils::QMenuItemView();
//    mv->setModel(new TestModel(&app));
//    mv->show();

    Utils::QMenuItemView *mv2 = new Utils::QMenuItemView();
    QFileSystemModel *model = new QFileSystemModel(&app);
    model->setReadOnly(true);
    model->setRootPath(QDir::homePath());
    mv2->setModel(model);
    mv2->setRootIndex(model->index(QDir::homePath()));

    QToolButton *button = new QToolButton(widget);
    button->setText("Test Menu");
    button->setPopupMode(QToolButton::InstantPopup);
    button->setMenu(mv2);
    layout->addWidget(button);

    QTreeView *view = new QTreeView(widget);
    view->setModel(model);
    layout->addWidget(view);

    mainWin->setCentralWidget(widget);
    mainWin->show();
    mainWin->raise();

    return app.exec();
}




