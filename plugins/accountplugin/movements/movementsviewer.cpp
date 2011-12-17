/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developpers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "movementsviewer.h"
#include "movementsIO.h"
#include "movementsmanager.h"
#include "../accountwidgetmanager.h"

#include <accountbaseplugin/movementmodel.h>
#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/accountbasecore.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <utils/global.h>

#include "ui_movementsviewer.h"

#include <QMessageBox>
#include <QDebug>
enum { WarnDebugMessage = false };
/*********************/
//todo bank system et deposit
/********************/

using namespace AccountDB;
using namespace Account;
using namespace Constants;

static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }
static inline AccountDB::AccountBaseCore *core() {return AccountDB::AccountBaseCore::instance();}

MovementsViewer::MovementsViewer(QWidget * parent) :
    QWidget(parent),
    ui(new Ui::MovementsViewerWidget)
{
    ui->setupUi(this);
    //instanciate
    m_valid = 0; //bill not received
    ui->valueDoubleSpinBox->setRange(0.00,999999999999999.00);
    ui->percentDoubleSpinBox->setRange(0.00,100.00);
    ui->percentDoubleSpinBox->setValue(100.00);
    ui->dateEdit->setDate(QDate::currentDate());
    fillMovementsComboBox();
    fillYearComboBox();
    fillBankComboBox();
    ui->valAndRecButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    showMovements();
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(ui->recordButton,SIGNAL(pressed()),this,SLOT(recordMovement()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteMovement()));
    connect(ui->validButton,SIGNAL(pressed()),this,SLOT(validMovement()));
    connect(ui->valAndRecButton,SIGNAL(pressed()),this,SLOT(validAndRecord()));
    connect(ui->movementsComboBox,SIGNAL(highlighted(int)),this,
                                  SLOT(setMovementsComboBoxToolTips(int)));
    connect(user(), SIGNAL(userChanged()), this, SLOT(userIsChanged()));
    
}

MovementsViewer::~MovementsViewer()
{
}

void MovementsViewer::userIsChanged()
{
    /** \todo move this into the model */
    showMovements();
}

void MovementsViewer::showMovements()
{
    MovementsIODb mov(this) ;
    QString year = ui->yearComboBox->currentText();
    MovementModel *model = core()->movementModel();  //->.mov.getModelMovements(year);
    model->setYearFilter(year.toInt());
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->horizontalHeader()->setStretchLastSection(false);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setResizeMode(MovementModel::Label, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setResizeMode(MovementModel::Details, QHeaderView::Stretch);
//    ui->tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);

    ui->tableView->hideColumn(MovementModel::Id);
    ui->tableView->hideColumn(MovementModel::AvailableMovementId);
    ui->tableView->hideColumn(MovementModel::UserUid);
    ui->tableView->hideColumn(MovementModel::AccountId);
    ui->tableView->hideColumn(MovementModel::Type);
    ui->tableView->hideColumn(MovementModel::Trace);
    ui->tableView->hideColumn(MovementModel::Comment);
}

void MovementsViewer::recordMovement()
{
    MovementsIODb  mov(this) ;
    MovementsManager manager;
    QHash<int,QVariant>  hashValues;
    QString availableMovement = ui->movementsComboBox->currentText();
    int acMovId = mov.getAvailableMovementId(availableMovement);
    QString userUid = mov.getUserUid();
    QString bank = ui->bankComboBox->currentText();
    int bankId = mov.getBankId(bank);
    int type = mov.getTypeOfMovement(availableMovement);
    QString label = availableMovement;
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    QString dateValue = ui->dateEdit->date().toString("yyyy-MM-dd");
    double valueCalculated = 0.00;
    valueCalculated = manager.getCalculatedValue(ui->valueDoubleSpinBox->value(),ui->percentDoubleSpinBox->value());    
    QString comment;//no comment
    int validity = 0;
    QString trace;// Log all modifications done on this movement
    int isValid = m_valid;
    QString details = ui->detailsEdit->text();
    hashValues = manager.getHashOfValues(acMovId ,
                                         userUid,
                                         bankId,
                                         type,
                                         label,
                                         date,
                                         dateValue ,
                                         valueCalculated ,
                                         comment,
                                         validity ,
                                         trace ,
                                         isValid ,
                                         details);
    
    if (!mov.insertIntoMovements(hashValues)) {
        Utils::warningMessageBox(tr("Error, movement not saved"),
                                 tr("An error occured, the movement was not saved into the database."));
    } else {
        Utils::informativeMessageBox(tr("Movement saved"),
                                     tr("The movement was correctly saved into the database."));
    }
    showMovements();
}

void MovementsViewer::deleteMovement()
{
    QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
    if(!index.isValid()) {
        Utils::warningMessageBox(tr("Can not delete movement."),
                                 tr("You must select a movement first."));
    }
    int row = index.row();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
    MovementsIODb  mov(this) ;
    if (mov.containsFixAsset(row)) {
        Utils::warningMessageBox(tr("Can not delete asset."),
                                 tr("You must use the specific view to edit assets."));
        return;
    }
    if (!mov.deleteMovement(row)) {
        Utils::warningMessageBox(tr("Movement not deleted."),
                                 tr("The specified movement was not deleted."));
    } else {
        Utils::informativeMessageBox(tr("Movement deleted."),
                                     tr("The movement was correctly deleted from the database."));
    }
    showMovements();
}

void MovementsViewer::validMovement()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        Utils::warningMessageBox(tr("Can not valid movement."),
                                 tr("You must select a movement first."));
    }
    index = core()->movementModel()->index(index.row(), MovementModel::IsValid);
    if (!core()->movementModel()->setData(index, 1)) {
        Utils::warningMessageBox(tr("Movement not validated."),
                                 tr("The specified movement was not validated."));
    } else {
        Utils::informativeMessageBox(tr("Movement validated."),
                                     tr("The movement was correctly validated from the database."));
    }
    showMovements();
}

void MovementsViewer::validAndRecord()
{
    m_valid = 1;
    recordMovement();// 1 = invoice,bill received
    m_valid = 0;
}

void MovementsViewer::fillMovementsComboBox()
{
    MovementsIODb mov(this);
    ui->movementsComboBox->setModel(mov.getMovementsComboBoxModel(this));
}

void MovementsViewer::setMovementsComboBoxToolTips(int row)
{
    QHash<QString,QString> hashChildrenAndParents;
    MovementsIODb mov(this);
    hashChildrenAndParents = mov.hashChildrenAndParentsAvailableMovements();
    QString text = ui->movementsComboBox->itemText(row);
    QString parent = hashChildrenAndParents.value(text);
    QString toolTipText = QString("Parent = %1").arg(parent);
    QAbstractItemModel *m = new QStandardItemModel;
    m = ui->movementsComboBox->model();
    QStandardItemModel *model = qobject_cast<QStandardItemModel*> (m);
    QStandardItem *i = model->item(row);
    i->setToolTip(toolTipText);
}

void MovementsViewer::fillYearComboBox()
{
    MovementsIODb mov(this);
    QStringList listOfYears;
    listOfYears = mov.getYearComboBoxModel();
    if (listOfYears.isEmpty()) {
        listOfYears << QString::number(QDate::currentDate().year());
    }
    ui->yearComboBox->addItems(listOfYears);
}

void MovementsViewer::fillBankComboBox()
{
    MovementsIODb mov(this);
    ui->bankComboBox->setModel(mov.getBankComboBoxModel(this));
}

void MovementsViewer::changeEvent(QEvent * e){
    QWidget::changeEvent(e);
    if (e->type()==QEvent::LanguageChange) {
        ui->retranslateUi(this);
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " langage changed " ;
        showMovements();
    }
}
