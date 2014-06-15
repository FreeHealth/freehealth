/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "movementsviewer.h"
#include "movementsio.h"
#include "movementsmanager.h"
#include "../accountwidgetmanager.h"

#include <accountbaseplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include "ui_movementsviewer.h"

#include <QDebug>

using namespace AccountDB;
//using namespace Account;
using namespace Constants;
using namespace Trans::ConstantTranslations;
using namespace Core;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

enum { WarnDebugMessage = false };

/*********************/
// TODO: bank system and deposit
/********************/
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }

MovementsViewer::MovementsViewer(QWidget * parent) :
        QWidget(parent),ui(new Ui::MovementsViewerWidget)
{
    ui->setupUi(this);
    ui->dateEdit->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    ui->quitButton->hide();
    MovementsManager rManager;
    if (rManager.isMedintuxArg()||rManager.isFreaccountIndependant())
    {
        ui->quitButton->show();
        setAttribute(Qt::WA_DeleteOnClose);
        }

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
    if(!showMovements())
        Utils::warningMessageBox(tr("Unable to show movements correctly."), tr("Contact the development team."));

    //icons and shortcuts
    ui->quitButton->setIcon(theme()->icon(Core::Constants::ICONQUIT));
    ui->quitButton->setShortcut(QKeySequence::Quit);
    ui->quitButton->setToolTip(QKeySequence(QKeySequence::Quit).toString());

    ui->deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    ui->deleteButton->setShortcut(QKeySequence::Delete);
    ui->deleteButton->setToolTip(QKeySequence(QKeySequence::Delete).toString());

    ui->validButton->setIcon(theme()->icon(Core::Constants::ICONOK));
    ui->validButton->setShortcut(QKeySequence("CTRL+V"));
    ui->validButton->setToolTip(QKeySequence(QKeySequence("CTRL+V")).toString());

    ui->valAndRecButton->setIcon(theme()->icon(Core::Constants::ICONSAVE));
    ui->valAndRecButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    ui->valAndRecButton->setToolTip(QKeySequence(QKeySequence::InsertParagraphSeparator).toString());

    ui->recordButton->setIcon(theme()->icon(Core::Constants::ICONSAVE));
    ui->recordButton->setShortcut(QKeySequence::Save);
    ui->recordButton->setToolTip(QKeySequence(QKeySequence::Save).toString());

    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(quitFreeaccout()));
    connect(ui->recordButton,SIGNAL(pressed()),this,SLOT(recordMovement()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteMovement()));
    connect(ui->validButton,SIGNAL(pressed()),this,SLOT(validMovement()));
    connect(ui->valAndRecButton,SIGNAL(pressed()),this,SLOT(validAndRecord()));
    connect(ui->movementsComboBox,SIGNAL(highlighted(int)),this,
                                  SLOT(setMovementsComboBoxToolTips(int)));
    connect(ui->yearComboBox,SIGNAL(activated(int)),this,SLOT(setYearIsChanged(int)));
    connect(user(), SIGNAL(userChanged()), this, SLOT(userIsChanged()));
}

MovementsViewer::~MovementsViewer()
{
}

void MovementsViewer::quitFreeaccout()
{
    QApplication::closeAllWindows();
}

void MovementsViewer::userIsChanged(){
    showMovements();
}

bool MovementsViewer::showMovements()
{
    bool ret = true;
    MovementsIODb  mov(this) ;
    QString year = ui->yearComboBox->currentText();
    AccountDB::MovementModel * model = mov.getModelMovements(year);
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " model rowCount =" << QString::number(model->rowCount()) ;
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " model columnCount =" << QString::number(model->columnCount()) ;
    if(!model->setHeaderData(MOV_LABEL,Qt::Horizontal,tr("Label")))
    ret = false;
    model->setHeaderData(MOV_DATE,Qt::Horizontal,tr("Date"));
    model->setHeaderData(MOV_DATEOFVALUE,Qt::Horizontal,tr("Date of value"));
    model->setHeaderData(MOV_AMOUNT,Qt::Horizontal,tr("Acts"));
    model->setHeaderData(MOV_VALIDITY,Qt::Horizontal,tr("Validity"));
    model->setHeaderData(MOV_ISVALID,Qt::Horizontal,tr("Valid"));
    model->setHeaderData(MOV_DETAILS,Qt::Horizontal,tr("Details"));
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
    ui->tableView->setSortingEnabled(true);

#if QT_VERSION < 0x050000
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->tableView->verticalHeader()  ->setResizeMode(QHeaderView::Stretch);
#else
    // Qt5
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->verticalHeader()  ->setSectionResizeMode(QHeaderView::Stretch);
#endif

    ui->tableView->horizontalHeader()->setStretchLastSection ( true );
    ui->tableView->setColumnHidden(MOV_ID,true);
    ui->tableView->setColumnHidden(MOV_AV_MOVEMENT_ID,true);
    ui->tableView->setColumnHidden(MOV_USER_UID,true);
    ui->tableView->setColumnHidden(MOV_ACCOUNT_ID,true);
    ui->tableView->setColumnHidden(MOV_TYPE,true);
    ui->tableView->setColumnHidden(MOV_TRACE,true);
    ui->tableView->setColumnHidden(MOV_COMMENT,true);
    return ret;
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
    QString trace;// ??
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
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Movement not inserted."));
    } else {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Movement inserted."));
    }
    showMovements();
}

void MovementsViewer::deleteMovement()
{
    QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
    QString year = ui->yearComboBox->currentText();
    if(!index.isValid()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Select a line."));
    }
    int row = index.row();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
    MovementsIODb  mov(this) ;
    if (mov.containsFixAsset(row)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("This fixed asset cannot be deleted.\nDo it in assets."));
          return;
    }
    if (!mov.deleteMovement(row,year)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Movement is not deleted."));
    }  else {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Movement deleted."));
    }
    showMovements();
}

void MovementsViewer::validMovement()
{
    QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
    if (!index.isValid()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Select a line."));
    }
    int row = index.row();
    MovementsIODb  mov(this) ;
    if (!mov.validMovement(row)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Movement is not validated."));
    } else {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Movement validated."));
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

void MovementsViewer::setMovementsComboBoxToolTips(int row){
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
    QStringList listOfYearsInverted;
    listOfYears = mov.getYearComboBoxModel();
    if (listOfYears.isEmpty()) {
        listOfYears << QString::number(QDate::currentDate().year());
    }
    for (int i = listOfYears.size()-1; i > -1 ; i--)
    {
        listOfYearsInverted << listOfYears[i];
        }
    ui->yearComboBox->addItems(listOfYearsInverted);
}

void MovementsViewer::fillBankComboBox(){
    MovementsIODb mov(this);
    ui->bankComboBox->setModel(mov.getBankComboBoxModel(this));
}

void MovementsViewer::changeEvent(QEvent * e){
    QWidget::changeEvent(e);
    if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << "before langage changed";
    if (e->type()==QEvent::LanguageChange) {
        ui->retranslateUi(this);
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " langage changed " ;
        showMovements();
        }
    else
    {
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " langage not changed " ;
        showMovements();
        }
    if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << "after langage changed";
}

void MovementsViewer::setYearIsChanged(int row){
    Q_UNUSED(row);
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " in setYearIsChanged ";
    if (!showMovements())
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to show new movements" ;
        }
}
