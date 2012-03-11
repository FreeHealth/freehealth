/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "findReceiptsValues.h"
#include "constants.h"

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QSqlQuery>
#include <QSqlTableModel>

enum { WarnDebugMessage = false };

using namespace AccountDB;
using namespace Constants;
using namespace Trans::ConstantTranslations;

findReceiptsValues::findReceiptsValues(QWidget * parent):QDialog(parent){
  ui = new Ui::findValueDialog;
  ui->setupUi(this);
  ui->nextButton->hide();
  ui->nameRadioButton->setChecked(true);
  ui->modifSpinBox->setValue(1.0);
  m_modifier = 1.0;
  MedicalProcedureModel model(parent);
  m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__)   ;
  fillComboCategories();
  if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__)   ;
  initialize();
  if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__)   ;
  QString comboValue = ui->comboBoxCategories->currentText().trimmed();
  ui->plusButton->setIcon(QIcon(qApp->applicationDirPath()+"/../../global_resources/pixmap/16x16/next.png"));
  ui->lessButton->setIcon(QIcon(qApp->applicationDirPath()+"/../../global_resources/pixmap/16x16/previous.png"));
  ui->plusButton->setShortcut(QKeySequence("CTRL+p"));
  ui->lessButton->setShortcut(QKeySequence("CTRL+l"));
  fillListViewValues(comboValue);

  connect(ui->comboBoxCategories,SIGNAL(activated(const QString&)),this,SLOT(fillListViewValues(const QString&)));
  connect(ui->tableViewOfValues,SIGNAL(pressed(const QModelIndex&)),this,SLOT(showToolTip(const QModelIndex&)));
  connect(ui->plusButton,SIGNAL(pressed()),this,SLOT(chooseValue()));
  connect(ui->lessButton,SIGNAL(pressed()),this,SLOT(deleteValue()));
  //connect(ui->listChoosenWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(supprItemChoosen(QListWidgetItem *)));
  connect(ui->nextButton,SIGNAL(pressed()),this,SLOT(showNext()));
  connect(qApp,SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(setModifSpinBox(QWidget*,QWidget*)));
  connect(ui->modifSpinBox,SIGNAL(valueChanged(double)),this,SLOT(setModifier(double)));
}

findReceiptsValues::~findReceiptsValues()
{
  delete m_xmlParser;
  ui->listChoosenWidget->clear();
}

void findReceiptsValues::initialize(){
    m_xmlParser = new xmlCategoriesParser;
;
    if(m_hashValuesChoosen.size()>0){
        m_hashValuesChoosen.clear();
        }
}

void findReceiptsValues::clear(){
    ui->listChoosenWidget->clear();
    m_hashValuesChoosen.clear();
}

void findReceiptsValues::fillComboCategories(){
    QStringList choiceList ;
    QSqlQuery q(m_db);
    const QString req = QString("SELECT %1 FROM %2").arg("TYPE","medical_procedure");
    if (!q.exec(req))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
        }
    while (q.next())
    {
    	QString type = q.value(0).toString();
    	choiceList << type;
        }
    choiceList.removeDuplicates();
    ui->comboBoxCategories->setEditable(true);
    ui->comboBoxCategories->setInsertPolicy(QComboBox::NoInsert);
    ui->comboBoxCategories->addItems(choiceList);
}

void findReceiptsValues::fillListViewValues(const QString & comboItem){
    QList<int> counterList;
    const QString baseName = "medical_procedure";
    const QString strItem = comboItem.trimmed();
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << strItem  ;
    const QString name = "NAME";
    const QString amount = "AMOUNT";
    const QString explanation = "ABSTRACT";
    const QString type = "TYPE";
    QString filter = QString("WHERE %1 = '%2'").arg(type,strItem);
    QString req = QString("SELECT %1,%2,%3 FROM %4 ").arg(name,amount,explanation,baseName )+filter;
    QStandardItemModel *model = new QStandardItemModel(0,2,this);
    int row = 0;
    QSqlQuery q(m_db);
    if (!q.exec(req))
    {
    	 qWarning() << __FILE__ << QString::number(__LINE__) 
    	                        << "Error __FILE__"+QString::number(__LINE__)+q.lastError().text() ; 
        }
    while (q.next())
    {
    	QString n = q.value(0).toString();
    	QString a = q.value(1).toString();
    	QString expl = q.value(2).toString();
    	model->insertRows(row,1,QModelIndex());
    	model->setData(model->index(row,0),n,Qt::EditRole);
        model->setData(model->index(row,1),a,Qt::EditRole);
        model->submit();
        m_hashExplanations.insert(row,expl);
        ++row;
        counterList << row;
        }
    if (counterList.size()<1) {
        const QString &explanationText = tr("The medical procedure database is empty.\n"
                                            "You should create defaults in Configuration > Preference > Accountancy.");
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), explanationText);
    }
    ui->tableViewOfValues->setModel(model);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);

}

void findReceiptsValues::showToolTip(const QModelIndex & index)
{
    int row = index.row();
    QString explanation = m_hashExplanations.value(row);
    const QAbstractItemModel *abstractModel = index.model();
    const QStandardItemModel * model = static_cast<const QStandardItemModel*>(abstractModel) ;
    QStandardItem *item = model->itemFromIndex(index);
    item->setToolTip(explanation);
}

/*void findReceiptsValues::chooseValue(const QModelIndex& index){
    QModelIndex inIndex(index);
    //get datas
    int row = inIndex.row();
    QAbstractItemModel * model = ui->tableViewOfValues->model();
    QModelIndex indexData = model->index(row,0,QModelIndex());
    QModelIndex indexAmount = model->index(row,1,QModelIndex());
    QString data = model->data(indexData,Qt::DisplayRole).toString();//NAME
    QString amount = model->data(indexAmount,Qt::DisplayRole).toString();//AMOUNT
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " data = " << data;
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " amount = " << amount;
    ui->listChoosenWidget->addItem(data);
    m_hashValuesChoosen.insert(data,amount);
}*/

void findReceiptsValues::chooseValue()
{
    //get datas
    QAbstractItemModel * model = ui->tableViewOfValues->model();
    QModelIndex inIndex = ui->tableViewOfValues->currentIndex();
    if (!inIndex.isValid())
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "index not valid" ;
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("You have to choose a value !"),
    	  QMessageBox::Ok);
    	  return;  
        }
    int row = inIndex.row();
    QModelIndex indexData = model->index(row,0,QModelIndex());
    QModelIndex indexAmount = model->index(row,1,QModelIndex());
    QString data = model->data(indexData,Qt::DisplayRole).toString();//NAME
    QString amount = model->data(indexAmount,Qt::DisplayRole).toString();//AMOUNT
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " data = " << data;
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " amount = " << amount;
    if (m_modifier < 1.0)
    {
    	  data = data+"*"+QString::number(m_modifier);
    	  double amountDouble = amount.toDouble();
    	  if (amountDouble == 0.0)
             {
        	  qWarning() << __FILE__ << QString::number(__LINE__) << "value null" ;
        	  if (amount.contains(","))
        	  {
        	  	  if (WarnDebugMessage)
        	  	  qDebug() << __FILE__ << QString::number(__LINE__) << " in , "  ;
        	  	  amount.replace(",",QLocale::c().decimalPoint ());
        	  	  amountDouble = amount.toDouble();
        	      }
        	  else if (amount.contains("."))
        	  {
        	  	  amount.replace(".",QLocale::c().decimalPoint ());
        	  	  amountDouble = amount.toDouble();
        	      }
            }
    	  amount = QString::number(amountDouble * m_modifier);
        }
    	      
    ui->listChoosenWidget->addItem(data);
    m_hashValuesChoosen.insert(data,amount);
}

void findReceiptsValues::deleteValue()
{
    QListWidgetItem * item;
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " in deleteValue " ;
    if (!ui->listChoosenWidget->currentIndex().isValid())
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "index not valid" ;
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("You have to choose a value !"),
    	  QMessageBox::Ok);
    	  return;
        }
    item = ui->listChoosenWidget->currentItem();
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " item = " << item->text();
    QString dataToRemove = item->data(Qt::DisplayRole).toString();
    m_hashValuesChoosen.remove(dataToRemove);
    delete item;
}

void findReceiptsValues::setModifSpinBox(QWidget*old,QWidget*newWidget){
    Q_UNUSED(old);
    if (newWidget == ui->modifSpinBox)
    {
    	  if (ui->modifSpinBox->value()== 1.0)
    	  {
    	  	  ui->modifSpinBox->setValue(0.5);
    	      }
    	  else
    	  {
    	  	ui->modifSpinBox->setValue(1.0);
    	      }
        }

}

void findReceiptsValues::setModifier(double modif)
{
    m_modifier = modif;
}

/*void findReceiptsValues::supprItemChoosen(QListWidgetItem * item){
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " item = " << item->text();
    QString dataToRemove = item->data(Qt::DisplayRole).toString();
    m_hashValuesChoosen.remove(dataToRemove);
    delete item;
}*/

QHash<QString,QString> findReceiptsValues::getChoosenValues(){
    return m_hashValuesChoosen;
}

/*void findReceiptsValues::on_lineEditFilter_textChanged(const QString & text){
    if (!((itemModel = new QStandardItemModel(this)) == NULL) )
    {
        itemModel->clear();
        }
    QString comboChoice = ui->comboBoxCategories->currentText();
    QString filterText = ""+text+"%";
    QString filter = QString("%1 LIKE '%2' AND %3 LIKE '%4'").arg("TYPE",comboChoice,"NAME",filterText);
    QVariant act = QVariant(trUtf8("Name"));
    QVariant value = QVariant(trUtf8("Value"));
    model->setFilter(filter);
    int count =   model->rowCountWithFilter(QModelIndex(),filter);
    for (int i = 0; i < count; i += 1)
    {
    	QString name = model->dataWithFilter(model->index(i,MP_NAME),Qt::DisplayRole,filter).toString();
    	if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " names =" << name ;
    	QString value = model->dataWithFilter(model->index(i,MP_AMOUNT),Qt::DisplayRole,filter).toString();
    	QStandardItem *itemName = new QStandardItem(name);
    	QStandardItem *itemValue = new QStandardItem(value);
    	QList<QStandardItem*> list;
    	list << itemName << itemValue;
    	itemModel->appendRow(list);
        }
    model->setFilter("");
    if (!itemModel->setHeaderData(0,Qt::Horizontal,act,Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
    	  }
    if (!itemModel->setHeaderData(1,Qt::Horizontal,value,Qt::EditRole)	)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
        } 
    ui->tableViewOfValues->setModel(itemModel);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
    if (tableViewIsFull(ui->tableViewOfValues->model()))
    {
    	  enableShowNextTable();
        }
}*/

void findReceiptsValues::on_lineEditFilter_textChanged(const QString & text){
    QString comboChoice = ui->comboBoxCategories->currentText();
    QString filterText ;
    QString filter; 
    const QString baseName = "medical_procedure";
    const QString name = "NAME";
    const QString amount = "AMOUNT";
    const QString type = "TYPE";
    if (ui->nameRadioButton->isChecked())
    {
    	  filterText = ""+text+"%";
    	  filter = QString("WHERE %1 LIKE '%2' AND %3 LIKE '%4'").arg("TYPE",comboChoice,"NAME",filterText); 
        }
    else if (ui->abstractRadioButton->isChecked())
    {
    	  filterText = "%"+text+"%";
    	  filter = QString("WHERE %1 LIKE '%2' AND %3 LIKE '%4'").arg("TYPE",comboChoice,"ABSTRACT",filterText); 
        }
    else{
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Check a button."));
    }
    QString req = QString("SELECT %1,%2 FROM %3 ").arg(name,amount,baseName )+filter;
    QStandardItemModel *model = new QStandardItemModel(1,2,this);
    int row = 0;
    QSqlQuery q(m_db);
    if (!q.exec(req))
    {
    	 qWarning() << __FILE__ << QString::number(__LINE__) 
    	                        << "Error __FILE__"+QString::number(__LINE__)+q.lastError().text() ; 
        }
    while (q.next())
    {
    	QString n = q.value(0).toString();
    	QString a = q.value(1).toString();
    	//if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " n and a	= " << n << a;
    	model->insertRows(row,1,QModelIndex());
    	model->setData(model->index(row,0),n,Qt::EditRole);
        model->setData(model->index(row,1),a,Qt::EditRole);
        model->submit();
        //if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " model data =" << model->data(model->index(row,0),Qt::DisplayRole).toString();
        ++row;
        //if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " rows =" << QString::number(row) ;
        }
    ui->tableViewOfValues->setModel(model);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
}

bool findReceiptsValues::tableViewIsFull(QAbstractItemModel * model){
    bool ret = false;
    int count = model->rowCount();
    if (count > 255)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "table view is full" ;
    	  ret = true;
        }
    return ret;
}

void findReceiptsValues::enableShowNextTable(){
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " enableshownet "   ;
    ui->nextButton->show();
}

/*void findReceiptsValues::showNext(){
    QAbstractItemModel * abModel = ui->tableViewOfValues->model();
    int rows = abModel->rowCount();
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(rows) ;
    int numberOfLastRow = abModel->headerData(rows -1,Qt::Vertical,Qt::DisplayRole).toInt();
    QString lastData = abModel->data(abModel->index(numberOfLastRow -1,0 ),Qt::DisplayRole).toString();
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " numberOfLastRow =" << QString::number(numberOfLastRow) ;
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " shownext data =" <<  lastData;
    QString comboChoice = ui->comboBoxCategories->currentText();
    QString afterSqlFilter = QString("%1 LIKE '%2' AND %3 >= '%4'").arg("TYPE",comboChoice,"NAME",lastData);
    model->setFilter(afterSqlFilter);
    int count =   model->rowCountWithFilter(QModelIndex(),afterSqlFilter);
    for (int i = 0; i < count; i += 1)
    {
    	QString name = model->dataWithFilter(model->index(i,MP_NAME),Qt::DisplayRole,afterSqlFilter).toString();
    	//if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " names =" << name ;
    	QString value = model->dataWithFilter(model->index(i,MP_AMOUNT),Qt::DisplayRole,afterSqlFilter).toString();
    	QStandardItem *itemName = new QStandardItem(name);
    	QStandardItem *itemValue = new QStandardItem(value);
    	QList<QStandardItem*> list;
    	list << itemName << itemValue;
    	itemModel->appendRow(list);
        }
    model->setFilter("");
    QVariant act = QVariant(trUtf8("Name"));
    QVariant value = QVariant(trUtf8("Value"));
    if (!itemModel->setHeaderData(0,Qt::Horizontal,act,Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
    	  }
    if (!itemModel->setHeaderData(1,Qt::Horizontal,value,Qt::EditRole)	)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "no header data available";
        } 
    ui->tableViewOfValues->setModel(itemModel);
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
}*/

void findReceiptsValues::showNext(){
    QAbstractItemModel * abModel = ui->tableViewOfValues->model();
    int rows = abModel->rowCount();
    if (WarnDebugMessage)
    	      qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(rows) ;
    int numberOfLastRow = abModel->headerData(rows -1,Qt::Vertical,Qt::DisplayRole).toInt();
    QString lastData = abModel->data(abModel->index(numberOfLastRow -1,0 ),Qt::DisplayRole).toString();
    QString comboChoice = ui->comboBoxCategories->currentText();
    QString afterSqlFilter = QString("%1 LIKE '%2' AND %3 >= '%4'").arg("TYPE",comboChoice,"NAME",lastData);
    MedicalProcedureModel *model = new MedicalProcedureModel(this);
    model->setFilter(afterSqlFilter);
    ui->tableViewOfValues->setModel(model);
    ui->tableViewOfValues->setColumnHidden(MP_ID,true);
    ui->tableViewOfValues->setColumnHidden(MP_UID,true);
    ui->tableViewOfValues->setColumnHidden(MP_USER_UID,true);
    ui->tableViewOfValues->setColumnHidden(MP_INSURANCE_UID,true);
    ui->tableViewOfValues->setColumnHidden(MP_REIMBOURSEMENT,true);
    ui->tableViewOfValues->setColumnHidden(MP_ABSTRACT,true);
    ui->tableViewOfValues->setColumnHidden(MP_TYPE,true);
    ui->tableViewOfValues->setColumnHidden(MP_DATE,true);    
    ui->tableViewOfValues-> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewOfValues-> setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);
}
