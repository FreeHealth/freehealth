/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "assetsviewer.h"
#include "assetsmanager.h"
#include "assetsio.h"
#include "ui_assetsviewer.h"

#include "../receipts/receiptsmanager.h"

#include <accountbaseplugin/assetmodel.h>
#include <accountbaseplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QDebug>

enum { WarnDebugMessage = false };

using namespace AccountDB;
using namespace Constants;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

AssetsViewer::AssetsViewer(QWidget * parent):QWidget(parent),ui(new Ui::AssetsViewerWidget){
    ui->setupUi(this);
    ui->dateEdit->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    //instanciate
    ui->valueDoubleSpinBox->setRange(0.00,999999999999999.00);
    //icons and shortcut
    ui->recordButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    ui->recordButton->setIcon(theme()->icon(Core::Constants::ICONSAVE));
    ui->recordButton->setToolTip(QKeySequence(QKeySequence::InsertParagraphSeparator).toString());
    ui->deleteButton->setShortcut(QKeySequence::Delete);
    ui->deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    ui->deleteButton->setToolTip(QKeySequence(QKeySequence::Delete).toString());
    ui->quitButton->setShortcut(QKeySequence::Close);
    ui->quitButton->setIcon(theme()->icon(Core::Constants::ICONEXIT));
    ui->quitButton->setToolTip(QKeySequence(QKeySequence::Close).toString());

    fillModeComboBox();
    fillBankComboBox();
    ui->dateEdit->setDate(QDate::currentDate());
    ui->yearEdit->setDate(QDate::currentDate());
    ui->yearEdit->setDisplayFormat("yyyy");
    QDate thisYear = ui->yearEdit->date();
    refresh();
    showAssets();
    yearDateChanged(thisYear);
    connect(ui->recordButton,SIGNAL(pressed()),this,SLOT(recordAsset()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteAsset()));
    connect(ui->quitButton,SIGNAL(pressed()),this,SLOT(quitFreeAccount()));
    connect(ui->yearEdit,SIGNAL(dateChanged(const QDate&)),this,SLOT(yearDateChanged(const QDate&)));
    connect(ui->tableView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(writeLabelByRow(const QModelIndex&)));
    connect(user(), SIGNAL(userChanged()), this, SLOT(userIsChanged()));
}

AssetsViewer::~AssetsViewer(){}

void AssetsViewer::userIsChanged(){
    QDate year = ui->yearEdit->date();
    showAssets();
    yearDateChanged(year);
}

void AssetsViewer::showAssets()
{
    AssetsIO  mov(this) ;
    AssetModel * model = mov.getModelAssets();

    // TODO: move header naming into the model
    model->setHeaderData(ASSETS_ACCOUNT_ID,Qt::Horizontal, tr("Account id"));
    model->setHeaderData(ASSETS_LABEL,Qt::Horizontal, tr("Label"));
    model->setHeaderData(ASSETS_DATE,Qt::Horizontal, tr("Date"));
    model->setHeaderData(ASSETS_DURATION,Qt::Horizontal, tr("Duration"));
    model->setHeaderData(ASSETS_MODE,Qt::Horizontal, tr("Mode"));
    model->setHeaderData(ASSETS_VALUE,Qt::Horizontal, tr("Value"));
    model->setHeaderData(ASSETS_YEARLY_RESULT,Qt::Horizontal, tr("Yearly value"));
    model->setHeaderData(ASSETS_RESIDUAL_VALUE,Qt::Horizontal, tr("Residual value"));
    model->setHeaderData(ASSETS_YEARS,Qt::Horizontal, tr("Years to run"));
    model->setHeaderData(ASSETS_RATE,Qt::Horizontal, tr("Rate"));
    model->setHeaderData(ASSETS_MOVEMENT,Qt::Horizontal, tr("Movement"));
    model->setHeaderData(ASSETS_COMMENT,Qt::Horizontal, tr("Comment"));
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::SelectedClicked);
    ui->tableView->setSortingEnabled(true);

#if QT_VERSION < 0x050000
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()  ->setResizeMode(QHeaderView::Stretch);
#else
    // Qt5
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()  ->setSectionResizeMode(QHeaderView::Stretch);
#endif

    ui->tableView->horizontalHeader()->setCascadingSectionResizes (true);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setColumnHidden(ASSETS_ID,true);
    ui->tableView->setColumnHidden(ASSETS_USER_UID,true);
    ui->tableView->setColumnHidden(ASSETS_TAXEACTS,true);
    ui->tableView->setColumnHidden(ASSETS_TRACE,true);
}

void AssetsViewer::recordAsset(){
    QHash<int,QVariant>  hashValues;
    QHash<int,QVariant>  hashValuesMovements;
    QString bankName = ui->bankComboBox->currentText();
    QDate year = ui->yearEdit->date();

    AssetsManager manager;
    AssetsIO asIO(this);
    QString userUid = asIO.getUserUid();
    int accountId = 0;
    accountId = asIO.getIdFromBankName(bankName);
    QString label = ui->detailsEdit->text();
    QDate beginDate = ui->dateEdit->date();
    QString date = beginDate.toString("yyyy-MM-dd");
    QDate currDate = QDate::currentDate();
    QString currentDate = currDate.toString("yyyy-MM-dd");
    double duration = ui->durationDoubleSpinBox->value();
    QString choiceModeComboBox = ui->modeComboBox->currentText();
    int mode = 0;
    mode = manager.getHashForModeComboBox().key(choiceModeComboBox);
    double value = ui->valueDoubleSpinBox->value();
    double taxActs = 0.00;
    double yearlyValue = manager.getYearlyValue(date,currDate,value,mode,duration);
    double residualValue = manager.getCalculatedResidualValue(value,yearlyValue);
    QVariant movement = QVariant();
    QString comments = ui->commentsEdit->text();
    QVariant trace = QVariant();

    int avMovIdForAsset = -2;
    int type = SPENDING;
    QString labelMov = tr("Fixed Asset");
    int validity = 0;
    int isValid = 1;
    QString traceMov = QString();
    QString commentMov = QString();
    double rate = manager.getRate(beginDate,duration,mode);
    hashValuesMovements = manager.getHashOfValuesMovements(//todo
                                         avMovIdForAsset ,
                                         userUid,
                                         accountId,//bank
                                         type,
                                         labelMov,
                                         date,
                                         date ,
                                         value ,
                                         commentMov,
                                         validity ,
                                         traceMov ,
                                         isValid ,
                                         comments);
    if (!asIO.insertIntoMovements(hashValuesMovements))  {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Asset is not recorded in movements."));
    }
    int movementLastId = asIO.getLastMovementId();
    if (movementLastId == -1) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Unable to get last movement id."));
    }
    movement = QVariant(movementLastId);
    hashValues = manager.getHashOfValues(userUid,
                                         accountId,
                                         label,
                                         date,
                                         duration,
                                         mode,
                                         value,
                                         taxActs,
                                         yearlyValue,
                                         residualValue,
                                         int(duration),
                                         rate,
                                         movement,
                                         comments,
                                         trace);

    if (!asIO.insertIntoAssets(hashValues)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Asset not recorded."));
    } else {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Asset recorded."));
    }
    showAssets();
    yearDateChanged(year);
}

void AssetsViewer::deleteAsset(){
    QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
    if (!index.isValid()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Select a line."));
    }
    QString bankName = ui->bankComboBox->currentText();
    int row = index.row();
    AssetsIO  assetIO(this) ;
    int idMovement = assetIO.getMovementId(row);
    int idBank = assetIO.getIdFromBankName(bankName);
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " idMovement =" << QString::number(idMovement) ;
    if (!assetIO.deleteAsset(row)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Asset is not deleted."));
    }
    if (!assetIO.deleteMovement(idMovement,idBank)) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Movement of asset is not deleted."));
    } else {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Asset deleted."));
    }
    showAssets();
}

void AssetsViewer::fillModeComboBox() {
    AssetsManager manager;
    QStringList list = manager.getHashForModeComboBox().values();
    ui->modeComboBox->clear();
    ui->modeComboBox->addItems(list);
}

void AssetsViewer::refresh(){
    AssetsManager manager;
    manager.setRefreshedResidualValue();
}

void AssetsViewer::fillBankComboBox(){
    AssetsIO assIO(this);
    ui->bankComboBox->setModel(assIO.getBankComboBoxModel(this));
}

void AssetsViewer::yearDateChanged(const QDate & year){
    double sumOfYearlyValues = 0.00;
    QString yearString = year.toString("yyyy");
    AssetsManager manager;
    QStandardItemModel *model = new QStandardItemModel(this);
    model = manager.getYearlyValues(year);
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__)<< "model in viewer = "<< model->rowCount();
    for (int i = 0; i < model->rowCount(); i += 1)
    {
        double yearlyValue = model->data(model->index(i,YEARLY_VALUE),Qt::DisplayRole).toDouble();
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " yearlyValue =" << QString::number(yearlyValue) ;
        sumOfYearlyValues += yearlyValue;
        }
    QString textLabel = "Total value to declare for "+yearString+" = "+QString::number(sumOfYearlyValues);
    ui->declareLabel->setText(textLabel);
}

void AssetsViewer::writeLabelByRow(const QModelIndex& index){
    int row = index.row();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " in writeLabelByRow =" << QString::number(row) ;
    QString yearString = ui->yearEdit->date().toString("yyyy");
    AssetsManager manager;
    double yearlyValue = manager.getYearlyValue(yearString ,row);
    QString label = manager.getLabelFromRow(row);
    QString textLabel = QString("Value to declare for %1 and %2 = %3").arg(yearString,
                                                                           label,
                                                                           QString::number(yearlyValue));
    ui->declareLabel->setText(textLabel);
}

void AssetsViewer::changeEvent(QEvent *e){
    QWidget::changeEvent(e);
    if (e->type()== QEvent::LanguageChange)
    {
          ui->retranslateUi(this);
          //fillBankComboBox();
          fillModeComboBox();
        }
}

void AssetsViewer::quitFreeAccount()
{
    ReceiptsManager r;
    if (r.isMedintuxArg())
    {
    	  setAttribute(Qt::WA_DeleteOnClose);
    	  QApplication::closeAllWindows();
        }
    else
    {
    	close();
        }
}
