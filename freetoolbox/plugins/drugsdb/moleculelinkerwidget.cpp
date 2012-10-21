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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "moleculelinkerwidget.h"
#include "extramoleculelinkermodel.h"
#include "searchatcindatabasedialog.h"

#include <coreplugin/ftb_constants.h>

#include <utils/global.h>

#include <QFile>
#include <QList>
#include <QDate>
#include <QDesktopServices>
#include <QUrl>
#include <QMenu>
#include <QClipboard>
#include <QSortFilterProxyModel>

#include <QDebug>

#include "ui_moleculelinkerwidget.h"

using namespace DrugsDB;

QWidget *MoleculeLinkerPage::createPage(QWidget *parent)
{
    return new MoleculeLinkerWidget(parent);
}


MoleculeLinkerWidget::MoleculeLinkerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoleculeLinkerWidget), model(0), proxyModel(0)
{
    setObjectName("MoleculeLinkerWidget");
    ui->setupUi(this);
    model = ExtraMoleculeLinkerModel::instance(qApp);
    ui->availableDrugsDb->addItems(model->availableDrugsDatabases());
    if (model->availableDrugsDatabases().count())
        model->selectDatabase(model->availableDrugsDatabases().at(0));

    if (model->rowCount() || model->canFetchMore()) {
        proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(model);
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        proxyModel->setFilterKeyColumn(ExtraMoleculeLinkerModel::MoleculeName);

        ui->tableView->setModel(proxyModel);
        ui->tableView->verticalHeader()->hide();
        ui->tableView->horizontalHeader()->setStretchLastSection(false);
        ui->tableView->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
        ui->tableView->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setResizeMode(3, QHeaderView::Interactive);
        ui->tableView->horizontalHeader()->setResizeMode(4, QHeaderView::Interactive);
        ui->tableView->horizontalHeader()->setResizeMode(5, QHeaderView::Interactive);
        ui->tableView->horizontalHeader()->setResizeMode(6, QHeaderView::Interactive);
        ui->tableView->horizontalHeader()->setResizeMode(7, QHeaderView::Interactive);
        ui->tableView->setColumnWidth(0,24);
    }

    connect(ui->availableDrugsDb, SIGNAL(activated(int)), this, SLOT(changeDatabase(int)));
    connect(ui->saveButton, SIGNAL(clicked()), model, SLOT(saveModel()));
    connect(ui->reveiwers, SIGNAL(activated(QString)), model, SLOT(setActualReviewer(QString)));
    connect(ui->tableView, SIGNAL(activated(QModelIndex)), this, SLOT(activated(QModelIndex)));
    connect(ui->tableView, SIGNAL(pressed(QModelIndex)), this, SLOT(pressed(QModelIndex)));
    connect(ui->removeUnreviewed, SIGNAL(clicked()), this, SLOT(onRemoveUnreviewedRequested()));
    connect(ui->searchMol, SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterWildcard(QString)));

//        processCSVFile();
}

MoleculeLinkerWidget::~MoleculeLinkerWidget()
{
    delete ui;
}

void MoleculeLinkerWidget::changeDatabase(const int index)
{
    model->selectDatabase(model->availableDrugsDatabases().at(index));
}

void MoleculeLinkerWidget::pressed(const QModelIndex &index)
{
    if (index.column()==ExtraMoleculeLinkerModel::FancyButton) {
        QMenu *menu = new QMenu(this);
        QAction *google = new QAction(tr("Search Google (copy molecule to clipboard)"), menu);
        QAction *who = new QAction(tr("Search WHO (copy molecule to clipboard)"), menu);
        QAction *resip = new QAction(tr("Search RESIP (copy molecule to clipboard)"), menu);
        QAction *copyClip = new QAction(tr("Copy molecule name to clipboard"), menu);
        QAction *atcSearchDialog = new QAction(tr("Open the ATC search dialog"), menu);
        menu->addAction(atcSearchDialog);
        menu->addAction(google);
        menu->addAction(who);
        menu->addAction(resip);
        menu->addAction(copyClip);
        QAction *selected = menu->exec(QCursor::pos());
        if (selected == atcSearchDialog) {
            SearchAtcInDatabaseDialog dlg(this, proxyModel->index(index.row(), ExtraMoleculeLinkerModel::MoleculeName).data().toString());
            if (dlg.exec() == QDialog::Accepted) {
                proxyModel->setData(proxyModel->index(index.row(), ExtraMoleculeLinkerModel::ATC_Code), dlg.getSelectedCodes().join(","));
                proxyModel->setData(proxyModel->index(index.row(), ExtraMoleculeLinkerModel::Review), Qt::Checked, Qt::CheckStateRole);
            }
        } else if (selected == google) {
            QDesktopServices::openUrl(QUrl(QString("http://www.google.fr/search?rls=en&q=%1+atc&ie=UTF-8&oe=UTF-8&redir_esc=").arg(proxyModel->index(index.row(), ExtraMoleculeLinkerModel::MoleculeName).data().toString())));
        } else if (selected == who) {
            QDesktopServices::openUrl(QUrl(QString("http://www.whocc.no/atc_ddd_index/?name=%1").arg(proxyModel->index(index.row(), ExtraMoleculeLinkerModel::MoleculeName).data().toString())));
        } else if (selected == resip) {
            QApplication::clipboard()->setText(proxyModel->index(index.row(), ExtraMoleculeLinkerModel::MoleculeName).data().toString());
            QDesktopServices::openUrl(QUrl("http://www.portailmedicaments.resip.fr/bcb_recherche/classes.asp?cc=1"));
        } else if (selected == copyClip) {
            QApplication::clipboard()->setText(proxyModel->index(index.row(), ExtraMoleculeLinkerModel::MoleculeName).data().toString());
        }
    }
}

void MoleculeLinkerWidget::activated(const QModelIndex &index)
{
    if (!proxyModel)
        return;
    if (index.column()==ExtraMoleculeLinkerModel::MoleculeName) {
        SearchAtcInDatabaseDialog dlg(this, proxyModel->index(index.row(), ExtraMoleculeLinkerModel::MoleculeName).data().toString());
        if (dlg.exec() == QDialog::Accepted) {
            proxyModel->setData(proxyModel->index(index.row(), ExtraMoleculeLinkerModel::ATC_Code), dlg.getSelectedCodes().join(","));
            proxyModel->setData(proxyModel->index(index.row(), ExtraMoleculeLinkerModel::Review), Qt::Checked, Qt::CheckStateRole);
        }
    }
}

void MoleculeLinkerWidget::onRemoveUnreviewedRequested()
{
    model = ExtraMoleculeLinkerModel::instance();
    int nb = model->removeUnreviewedMolecules();
    Utils::informativeMessageBox(tr("Removed %1 unreviewed item(s) from the model").arg(nb),"");
}



struct StructMol {
    QString name, atc, reference, comment;
};

//void MoleculeLinkerWidget::processCSVFile()
//{
//    // This code was used to retrieve Jim's molecules association.
//    // ***** Should not be reused *****
//    qWarning() << "processCSVFile";
//    QFile file(qApp->applicationDirPath() + Core::Constants::MACBUNDLE + "/../global_resources/sql/ca_atc_script_orphans_improved_tab.txt");
//    QString content;
//    file.open(QIODevice::ReadOnly | QIODevice::Text);
//    content = file.readAll();
////    content.replace("\t\n", "\t\t\n");
//    QStringList lines = content.split("\n");
//    qWarning() << lines.count() << content.count("\n");
//    QList<StructMol> mols;
//    foreach(const QString &line, lines) {
//        const QStringList &vals = line.split("\t");
//        StructMol mol;
//        if (vals.count() != 5) {
//            if (vals.count() >= 2) {
//                mol.name = vals.at(0).simplified();
//                mol.name.remove("\"");
//                mol.atc = vals.at(1);
//                mol.atc.remove("\"");
//                mol.atc.remove(" ");
//                mol.atc = mol.atc.simplified();
//                mols << mol;
//                continue;
//            } else {
//                qWarning() << vals;
//                continue;
//            }
//        }
//        mol.name = vals.at(0).simplified();
//        mol.name.remove("\"");
//        mol.atc = vals.at(1).simplified();
//        mol.atc.remove("\"");
//        mol.atc.remove(" ");
//        mol.atc = mol.atc.simplified();
//        mol.reference = vals.at(3).simplified();
//        mol.comment = vals.at(4).simplified();
//        mols << mol;
//    }
//    content.clear();
//    foreach(const StructMol &mol, mols) {
//        if (mol.atc.isEmpty())
//            content += "    <Molecule name=\""+ mol.name +"\" AtcCode=\"\" review=\"true\" reviewer=\"Jim Busser, MD (CA)\" reference=\"\" comment=\"No ATC Code found\" dateofreview=\""+QDate::currentDate().toString(Qt::ISODate)+"\"/>\n";
//        else
//            content += "    <Molecule name=\""+ mol.name +"\" AtcCode=\""+mol.atc+"\" review=\"true\" reviewer=\"Jim Busser, MD (CA)\" reference=\""+mol.reference+"\" comment=\""+mol.comment+"\" dateofreview=\""+QDate::currentDate().toString(Qt::ISODate)+"\"/>\n";
//    }
//    QFile out(qApp->applicationDirPath() + Core::Constants::MACBUNDLE + "/../global_resources/sql/mols.xml");
//    out.open(QFile::WriteOnly | QFile::Text);
//    out.write(content.toUtf8());
//    out.close();

//}

//void MoleculeLinkerWidget::processCSVFile()
//{
//    // This code was used to retrieve Eric's molecules association.
//    // ***** Should not be reused *****
//    qWarning() << "processCSVFile";
//    QFile file(qApp->applicationDirPath() + Core::Constants::MACBUNDLE + "/../global_resources/sql/handmadeatclinks.csv");
//    QString content;
//    file.open(QIODevice::ReadOnly | QIODevice::Text);
//    content = file.readAll();
//    QStringList lines = content.split("\n");
//    qWarning() << lines.count() << content.count("\n");
//    QList<StructMol> mols;
//    foreach(const QString &line, lines) {
//        if (line.startsWith("//"))
//            continue;
//        QStringList vals = line.split(";");
//        StructMol mol;
//        if (vals.count() < 2) {
//            qWarning() << vals;
//            continue;
//        }
//        mol.name = vals.takeFirst().simplified().toUpper();
//        mol.name.remove("\"");
//        mol.atc = vals.join(",");
//        mols << mol;
//    }
//    content.clear();
//    foreach(const StructMol &mol, mols) {
//        if (mol.atc.isEmpty())
//            content += "    <Molecule name=\""+ mol.name +"\" AtcCode=\"\" review=\"true\" reviewer=\"Eric Maeker, MD (FR)\" reference=\"\" comment=\"No ATC Code found\" dateofreview=\""+QDate::currentDate().toString(Qt::ISODate)+"\"/>\n";
//        else
//            content += "    <Molecule name=\""+ mol.name +"\" AtcCode=\""+mol.atc+"\" review=\"true\" reviewer=\"Eric Maeker, MD (FR)\" reference=\""+mol.reference+"\" comment=\""+mol.comment+"\" dateofreview=\""+QDate::currentDate().toString(Qt::ISODate)+"\"/>\n";
//    }
//    QFile out(qApp->applicationDirPath() + Core::Constants::MACBUNDLE + "/../global_resources/sql/mols.xml");
//    out.open(QFile::WriteOnly | QFile::Text);
//    out.write(content.toUtf8());
//    out.close();
//}

void MoleculeLinkerWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
