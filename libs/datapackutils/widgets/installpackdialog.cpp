/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "installpackdialog.h"

#include <datapackutils/core.h>
#include <datapackutils/iservermanager.h>

#include <QStandardItemModel>
#include <QPushButton>

#include "ui_installpackdialog.h"

using namespace DataPack;

static inline DataPack::Core *core() {return DataPack::Core::instance();}
static inline DataPack::IServerManager *serverManager() {return DataPack::Core::instance()->serverManager();}

namespace {
const char * const CSS =
        "QTreeView::item:hover {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
        "}"
        "QTreeView::item:selected:active{"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"
        "QTreeView::item:selected:!active {"
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
        "}";
}

InstallPackDialog::InstallPackDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallPackDialog)
{
    ui->setupUi(this);
    m_Model = new QStandardItemModel(this);
    ui->treeView->setModel(m_Model);
    ui->treeView->header()->hide();
    ui->treeView->setStyleSheet(::CSS);
    bInstallAll = ui->buttonBox->addButton(tr("Install all"), QDialogButtonBox::AcceptRole);
    bInstallPackAndDepends = ui->buttonBox->addButton(tr("Install pack and depends"), QDialogButtonBox::AcceptRole);
    bInstallPacksOnly = ui->buttonBox->addButton(tr("Install pack only"), QDialogButtonBox::AcceptRole);
}

InstallPackDialog::~InstallPackDialog()
{
    delete ui;
}

void InstallPackDialog::setPackToInstall(const Pack &pack)
{
    setPackToInstall(QList<Pack>() << pack);
}

void InstallPackDialog::setPackToInstall(const QList<Pack> &packs)
{
    m_Model->clear();
    m_Packs = packs;
    QFont bold;
    bold.setBold(true);
    bool hasDepends = false;
    bool hasRecommends = false;
    for(int z = 0; z < packs.count(); ++z) {
        const Pack &pack = packs.at(z);

        // To install
        QStandardItem *toInstall = new QStandardItem(tr("Installation of"));
        toInstall->setFont(bold);
        toInstall->setEditable(false);
        m_Model->appendRow(toInstall);
        QStandardItem *packToInstall = new QStandardItem(QString("%1 (%2)").arg(pack.name()).arg(pack.version()));
        packToInstall->setEditable(false);
        toInstall->appendRow(packToInstall);

        // Dependencies
        QStandardItem *depends = new QStandardItem(tr("Depends on"));
        QStandardItem *recommends = new QStandardItem(tr("Recommends"));
        depends->setEditable(false);
        depends->setFont(bold);
        recommends->setFont(bold);
        recommends->setEditable(false);

        // check dependencies (depends && recommended)
        for(int i = 0; i < pack.dependencies().count(); ++i) {
            const PackDependencyData &dep = pack.dependencies().at(i);
            QStandardItem *item = new QStandardItem(QString("%1 (%2)").arg(dep.name()).arg(dep.version()));
            item->setEditable(false);
            item->setData(i);
            switch (dep.type()) {
            case PackDependencyData::Depends:
                depends->appendRow(item);
                break;
            case PackDependencyData::Recommends:
                recommends->appendRow(item);
                break;
            default:
                delete item; // no really need this item
            }
        }

        if (depends->rowCount() > 0) {
            toInstall->appendRow(depends);
            hasDepends = true;
        }

        if (recommends->rowCount() > 0) {
            toInstall->appendRow(recommends);
            hasRecommends = true;
        }
    }
    ui->treeView->expandAll();
    bInstallPackAndDepends->setVisible(hasDepends);
//    bInstallPack->setVisible(hasDepends);
}

QList<Pack> InstallPackDialog::packsToInstall() const
{
    if (result()==QDialog::Rejected) {
        return QList<Pack>();
    }
    return QList<Pack>();
}
