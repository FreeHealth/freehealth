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
#include "findreceiptsvalues.h"
#include "constants.h"
#include "receiptsio.h"
#include "tools.h"

#include <utils/global.h>
#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QSqlQuery>
#include <QSqlTableModel>

enum { WarnDebugMessage = false };

using namespace AccountDB;
using namespace Constants;
using namespace Trans::ConstantTranslations;
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

static QString databasePath()
{
    QString dbRelPath = QString("/%1/%2").arg(Constants::DATAPACK_DB).arg(Constants::DATAPACK_ACCOUNTANCY_FILENAME);
    QString tmp;
    tmp = settings()->dataPackInstallPath() + dbRelPath;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " tmp =" << tmp ;
    if (QFileInfo(tmp).exists())
        return settings()->dataPackInstallPath()+ QDir::separator() + Constants::DATAPACK_DB;
    tmp = settings()->dataPackApplicationInstalledPath() + dbRelPath;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " databasePath =" << settings()->dataPackApplicationInstalledPath()+ QDir::separator() + Constants::DATAPACK_ACCOUNTANCY;
    return settings()->dataPackApplicationInstalledPath()+ QDir::separator() + Constants::DATAPACK_DB;
}

static QString databaseFileName()
{
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " databaseFileName =" << databasePath() + QDir::separator() + Constants::DATAPACK_ACCOUNTANCY_FILENAME ;
    return databasePath() + QDir::separator() + Constants::DATAPACK_ACCOUNTANCY_FILENAME;
}


findReceiptsValues::findReceiptsValues(QWidget * parent):QDialog(parent)
{
  ui = new Ui::findValueDialog;
  ui->setupUi(this);
  ui->nextButton->hide();
  ui->nameRadioButton->setChecked(true);
  ui->modifSpinBox->setValue(1.0);
  ui->abstractTextEdit->setText("");
  ui->othersTextEdit->setText("");
  ui->abstractTextEdit->setReadOnly(true);
  ui->othersTextEdit->setReadOnly(true);
  //m_db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
  //test of choice of user base or datapack
  if (datapackIsAvalaible())//verify
  {
      ui->datapackRadioButton->setChecked(true);
      chooseDatapackModel(true);
      qWarning() << __FILE__ << QString::number(__LINE__) << "DATAPACK FREEACCOUNT AVALAIBLE" ;
      }
  else
  {
    ui->userRadioButton->setChecked(true);
    chooseUserModel(true);
    qWarning() << __FILE__ << QString::number(__LINE__) << "USER MODEL IS CHOOSEN" ;
      }
  m_modifier = 1.0;
  //fillComboCategories();
  ui->comboBoxCategories->setEditable(false);
  initialize();
  QString comboValue = ui->comboBoxCategories->currentText().trimmed();
  for (int i = 0; i < ui->buttonBox->buttons().size(); ++i)
  {
  	  ui->buttonBox->buttons()[i]->setMinimumSize(100,50);
      }
  //icons and shortcuts
  ui->plusButton->setIcon(QIcon(qApp->applicationDirPath()+"/../../global_resources/pixmap/16x16/next.png"));
  ui->lessButton->setIcon(QIcon(qApp->applicationDirPath()+"/../../global_resources/pixmap/16x16/previous.png"));
  ui->plusButton->setShortcut(QKeySequence("CTRL+p"));
  ui->lessButton->setShortcut(QKeySequence("CTRL+l"));
  ui->plusButton->setToolTip(QKeySequence("CTRL+p").toString());
  ui->lessButton->setToolTip(QKeySequence("CTRL+l").toString());
  ui->buttonBox->button(QDialogButtonBox::Save)->setShortcut(QKeySequence::Save);
  ui->buttonBox->button(QDialogButtonBox::Save)->setToolTip(QKeySequence(QKeySequence::Save).toString());
  ui->buttonBox->button(QDialogButtonBox::Save)->setIcon(theme()->icon(Core::Constants::ICONSAVE));

  ui->buttonBox->button(QDialogButtonBox::Cancel)->setShortcut(QKeySequence::Quit);
  ui->buttonBox->button(QDialogButtonBox::Cancel)->setToolTip(QKeySequence(QKeySequence::Quit).toString());
  ui->buttonBox->button(QDialogButtonBox::Cancel)->setIcon(theme()->icon(Core::Constants::ICONEXIT));

  //flag
  setFlagAccordingToMProcedureCountry();
  //disable lessButton
  ui->lessButton->setEnabled(false);
  //fillListViewValues(comboValue);

  connect(ui->comboBoxCategories,SIGNAL(activated(const QString&)),this,SLOT(fillListViewValues(const QString&)));
  connect(ui->tableViewOfValues,SIGNAL(pressed(const QModelIndex&)),this,SLOT(showInformations(const QModelIndex&)));
  connect(ui->plusButton,SIGNAL(pressed()),this,SLOT(chooseValue()));
  connect(ui->lessButton,SIGNAL(pressed()),this,SLOT(deleteValue()));
  //connect(ui->listchosenWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(supprItemchosen(QListWidgetItem *)));
  connect(ui->nextButton,SIGNAL(pressed()),this,SLOT(showNext()));
  connect(qApp,SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(setModifSpinBox(QWidget*,QWidget*)));
  connect(ui->modifSpinBox,SIGNAL(valueChanged(double)),this,SLOT(setModifier(double)));
  connect(ui->userRadioButton,SIGNAL(clicked(bool)),this,SLOT(chooseUserModel(bool)));
  connect(ui->datapackRadioButton,SIGNAL(clicked(bool)),this,SLOT(chooseDatapackModel(bool)));
  connect(ui->nameRadioButton,SIGNAL(toggled(bool)),this,SLOT(wipeFilterEdit(bool)));
  connect(ui->plusButton,SIGNAL(pressed()),this,SLOT(wipeFilterEdit()));
  connect(ui->listchosenWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(setLessButtonEnabled(QListWidgetItem *)));

}

findReceiptsValues::~findReceiptsValues()
{
  ui->listchosenWidget->clear();
}

void findReceiptsValues::initialize(){
    if(m_hashValueschosen.size()>0){
        m_hashValueschosen.clear();
        }
}

void findReceiptsValues::clear(){
    ui->listchosenWidget->clear();
    m_hashValueschosen.clear();
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
    ui->comboBoxCategories->clear();
    ui->comboBoxCategories->setEditable(true);
    ui->comboBoxCategories->setInsertPolicy(QComboBox::NoInsert);
    ui->comboBoxCategories->addItems(choiceList);
}

void findReceiptsValues::fillListViewValues(const QString & comboItem){
    ui->abstractTextEdit->setText("");
    ui->othersTextEdit->setText("");
    QList<int> counterList;
    const QString baseName = "medical_procedure";
    const QString strItem = comboItem.trimmed();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << strItem  ;
    const QString name = "NAME";
    const QString amount = "AMOUNT";
    const QString explanation = "ABSTRACT";
    const QString others = "OTHERS";
    const QString type = "TYPE";

    QString filter = QString("WHERE %1 = '%2'").arg(type,strItem);
    //add date release test
    //filter += getDateWhereClause();
    QString req = QString("SELECT %1,%2,%3,%4 FROM %5 ")
                 .arg(name,amount,explanation,others,baseName )+filter;
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
        QString n = q.value(NAME).toString();
        QString a = q.value(AMOUNT).toString();
        QString expl = q.value(EXPLANATION).toString();
        QString otherInformation = q.value(OTHERS).toString();
        model->insertRows(row,1,QModelIndex());
        model->setData(model->index(row,0),n,Qt::EditRole);
        model->setData(model->index(row,1),a,Qt::EditRole);
        model->submit();
        m_hashExplanations.insert(row,expl);
        m_otherInformations.insert(row,otherInformation);
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
    ui->tableViewOfValues->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewOfValues->horizontalHeader()->setStretchLastSection ( true );
    ui->tableViewOfValues->setGridStyle(Qt::NoPen);

}

void findReceiptsValues::showInformations(const QModelIndex & index)
{
    int row = index.row();
    ui->abstractTextEdit->clear();
    ui->othersTextEdit->clear();
    QString explanation = m_hashExplanations.value(row);
    explanation = "<html><body><font color=blue size=3>"+explanation+"</font></body></html>";
    QString othersText;
    othersText = "<html><body><font color=blue size=3>";
    QHash<QString,QString> hash = getHashFatherSonFromOthers(index);
    for (int i = 0; i < hash.count(); ++i)
    {
          QString father = hash.keys()[i];
          QString son = hash.values()[i];
          if (!father.isEmpty())
          {
              othersText += father+":"+son+"<br/>";
              }
        }
    othersText += "</font></body></html>";
    ui->abstractTextEdit->insertHtml(explanation);
    ui->othersTextEdit->insertHtml(othersText);
    /*const QAbstractItemModel *abstractModel = index.model();
    const QStandardItemModel * model = static_cast<const QStandardItemModel*>(abstractModel) ;
    QStandardItem *item = model->itemFromIndex(index);
    item->setToolTip(explanation);*/
}

/*void findReceiptsValues::chooseValue(const QModelIndex& index){
    QModelIndex inIndex(index);
    //get data
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
    ui->listchosenWidget->addItem(data);
    m_hashValueschosen.insert(data,amount);
}*/

void findReceiptsValues::chooseValue()
{
    //get data
    QAbstractItemModel * model = ui->tableViewOfValues->model();
    QModelIndex inIndex = ui->tableViewOfValues->currentIndex();
    if (!inIndex.isValid()) {
        Utils::warningMessageBox(tr("No value selected."), tr("Please select a value."));
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
    if (m_modifier < 1.0 || m_modifier > 1.0)
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

    ui->listchosenWidget->addItem(data);
    m_hashValueschosen.insert(data,amount);
}

void findReceiptsValues::deleteValue()
{
    QListWidgetItem * item;
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " in deleteValue " ;
    if (!ui->listchosenWidget->currentIndex().isValid()) {
        Utils::warningMessageBox(tr("No value selected."), tr("Please select a value."));
        return;
    }
    item = ui->listchosenWidget->currentItem();
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " item = " << item->text();
    QString dataToRemove = item->data(Qt::DisplayRole).toString();
    m_hashValueschosen.remove(dataToRemove);
    delete item;
    ui->lessButton->setEnabled(false);
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

/*void findReceiptsValues::supprItemchosen(QListWidgetItem * item){
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " item = " << item->text();
    QString dataToRemove = item->data(Qt::DisplayRole).toString();
    m_hashValueschosen.remove(dataToRemove);
    delete item;
}*/

QHash<QString,QString> findReceiptsValues::getchosenValues(){
    return m_hashValueschosen;
}

/*void findReceiptsValues::on_lineEditFilter_textChanged(const QString & text){
    if (!((itemModel = new QStandardItemModel(this)) == NULL) )
    {
        itemModel->clear();
        }
    QString comboChoice = ui->comboBoxCategories->currentText();
    QString filterText = ""+text+"%";
    QString filter = QString("%1 LIKE '%2' AND %3 LIKE '%4'").arg("TYPE",comboChoice,"NAME",filterText);
    QVariant act = QVariant(tr("Name"));
    QVariant value = QVariant(tr("Value"));
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
    ui->tableViewOfValues->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
    QVariant act = QVariant(tr("Name"));
    QVariant value = QVariant(tr("Value"));
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

void findReceiptsValues::showNext(){//the button is hiden
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

/*QString findReceiptsValues::getDateWhereClause()
{
    QString whereClause;
    receiptsEngine io;
    QString dateOfLastReleaseLessOneDay = io.getJustDayBeforeLastRelease();
    whereClause = QString(" AND WHERE %1 > '%2'").arg(dateOfLastReleaseLessOneDay,"DATE");
    return whereClause;
}*/

bool findReceiptsValues::datapackIsAvalaible()
{
    bool b = true;
    QString datapack = databaseFileName();
    QFile file(datapack);
    if (!file.exists())
    {
          b = false;
        }
    return b;
}

void findReceiptsValues::chooseUserModel(bool b)
{
    if (b)
    {
          m_db = QSqlDatabase::database(DB_ACCOUNTANCY);
          fillComboCategories();
          const QString comboText = ui->comboBoxCategories->currentText();
          fillListViewValues(comboText);
        }
}

void findReceiptsValues::chooseDatapackModel(bool b)
{
    if (b)
    {
          m_db = QSqlDatabase::database(DATAPACK_DB);
          qDebug() << __FILE__ << QString::number(__LINE__) << " database name =" << m_db.databaseName() ;
          fillComboCategories();
          const QString comboText = ui->comboBoxCategories->currentText();
          fillListViewValues(comboText);
        }
}

QHash<QString,QString> findReceiptsValues::getHashFatherSonFromOthers(const QModelIndex & index)
{
    QHash<QString,QString> hash;
    int row = index.row();
    QString othersString = m_otherInformations.value(row);
    othersString.remove("<?xml version=1.0 encoding=ISO-8859-1?>");
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " othersString =" << othersString ;
    othersString.replace("</",";");
    QRegExp exp(";[a-z]*>");
    othersString.replace(exp,";");
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " othersString =" << othersString ;
    QStringList list;
    list = othersString.split(";");
    foreach(QString part,list){
        part.replace("<","");
        part.replace(">",";");
        QStringList partList;
        partList = part.split(";");
        if (partList.size()==2)
        {
              hash.insert(partList[FATHER],partList[SON]);
            }
        if(partList.size()==1)
        {
            hash.insert(partList[FATHER],"NULL");
            }
        }
        for (int i = 0; i < hash.count(); i += 1)
        {
              if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " father =" << hash.keys()[i] ;
              if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " son =" << hash.values()[i] ;
            }
    return hash;
}

void findReceiptsValues::wipeFilterEdit(bool b)
{
    Q_UNUSED(b);
    ui->lineEditFilter->setText("");
}

void findReceiptsValues::wipeFilterEdit()
{
    ui->lineEditFilter->setText("");
}

void findReceiptsValues::setLessButtonEnabled(QListWidgetItem * item)
{
    Q_UNUSED(item);
    ui->lessButton->setEnabled(true);
}

void findReceiptsValues::setFlagAccordingToMProcedureCountry()
{
  int counterId = 0;
  QString reqCountry;
  QString flagText;
  while(reqCountry.isEmpty())
  {
      ++counterId;
      reqCountry = QString("SELECT %1 FROM %2 WHERE %3 = '%4'")
                      .arg("COUNTRY","medical_procedure","MP_ID",QString::number(counterId));
      if (WarnDebugMessage)
      qDebug() << __FILE__ << QString::number(__LINE__) << " reqCountry =" << reqCountry ;
  }
  QSqlQuery queryCountry(m_db);
  if (!queryCountry.exec(reqCountry))
  {
  	  qWarning() << __FILE__ << QString::number(__LINE__) << queryCountry.lastError().text() ;
      }
  while (queryCountry.next())
  {
  	flagText = queryCountry.value(0).toString();
      }
  if (WarnDebugMessage)
  qDebug() << __FILE__ << QString::number(__LINE__) << " flagText =" << flagText ;
  ui->flagLabel->setText(flagText);
  Tools::ReceiptsTools rt;
  ui->flagPixmapLabel->setPixmap(rt.flagsHash().value(flagText));
}

