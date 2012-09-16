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
#include "controlreceiptsio.h"
#include "constants.h"
#include "ui_controlreceipts.h"

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_spashandupdate.h>
//#include <extensionsystem/iplugin.h>
#include <extensionsystem/pluginmanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/idocumentprinter.h> //coreplugin/idocumentprinter.h
#include <coreplugin/constants.h>
#include <QTimer>
#include <QProgressBar>

#include <QDebug>

enum { WarnDebugMessage = false };

using namespace AccountDB;
using namespace Constants;
using namespace Trans::ConstantTranslations;
using namespace ExtensionSystem;
using namespace Core;

static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

ControlReceipts::ControlReceipts(QWidget * parent):QWidget(parent),ui(new Ui::ControlReceiptsWidget)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);
    ui->beginDateEdit->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    ui->endDateEdit->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    int h = parent->height();
    int w = parent->width();
    resize(w,h);
    setAutoFillBackground(true);
    //radioButtons
    ui->resultLabel->setText("");
    ui->resultLabel->setWordWrap(true);
    m_accountModel = new AccountModel(this);
    m_userUuid = m_accountModel->getUserUuid();
    m_typeOfMoney = tr("Euros");
    ui->beginDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());

    //DUES CONTROL
    ui->duesButton->hide();

    //icons and shortcuts
    ui->backButton->setIcon(theme()->icon(Core::Constants::ICONPREVIOUS));
    ui->backButton->setShortcut(QKeySequence::Back);
    ui->backButton->setToolTip(QKeySequence(QKeySequence::Back).toString());
    ui->searchButton->setShortcut(QKeySequence::InsertParagraphSeparator);
    ui->searchButton->setToolTip(QKeySequence(QKeySequence::InsertParagraphSeparator).toString());
    ui->searchButton->setIcon(theme()->icon(Core::Constants::ICONFIND));
    ui->deleteButton->setShortcut(QKeySequence::Delete);
    ui->deleteButton->setToolTip(QKeySequence(QKeySequence::Delete).toString());
    ui->deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    ui->printButton->setShortcut(QKeySequence::Print);
    ui->printButton->setToolTip(QKeySequence(QKeySequence::Print).toString());
    ui->printButton->setIcon(theme()->icon(Core::Constants::ICONPRINT));
    search();
    connect(ui->searchButton,SIGNAL(pressed()),this,SLOT(search()));
    connect(ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteLine()));
    connect(ui->duesButton,SIGNAL(pressed()),this,SLOT(printDues()));
    connect(ui->printButton,SIGNAL(pressed()),this,SLOT(print()));
    connect(ui->backButton,SIGNAL(pressed()),this,SLOT(closeAction()));
}

ControlReceipts::~ControlReceipts()
{
    if (WarnDebugMessage)
        qWarning() << __FILE__ << QString::number(__LINE__) << "ControlReceipts::~ControlReceipts() " ;
}

void ControlReceipts::resizeControlReceipts(QWidget * parent){
    int h = parent->height();
    int w = parent->width();
    resize(w,h);
}

void ControlReceipts::search(){
    refresh();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString dateBeginStr = ui->beginDateEdit->date().toString("yyyy-MM-dd");
    QString dateEndStr = ui->endDateEdit->date().toString("yyyy-MM-dd");
    QString filterEdit = "%"+ui->filterEdit->text()+"%";
    QString fieldKey = ui->fieldComboBox->currentText();
    QString field = m_mapCombo.value(fieldKey);
    QString filter = QString("%1 = '%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("%1 LIKE '%2'").arg(field,filterEdit);
    filter += " AND ";
    filter += QString("%1 NOT LIKE '%2' AND ").arg(field,"0.0");
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    m_accountModel->setFilter(filter);
    while (m_accountModel->canFetchMore())
    {
          m_accountModel->fetchMore();
        }
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_accountModel->filter() ;
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(m_accountModel->rowCount()) ;
        QAbstractItemModel * lastModel = ui->tableView->model();
    if (lastModel)
    {
          delete lastModel;
        }

    ui->tableView->setModel(m_accountModel);
    ui->tableView->setShowGrid(false);
    ui->tableView->setColumnHidden(ACCOUNT_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_USER_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_PATIENT_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_SITE_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_INSURANCE_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_MEDICALPROCEDURE_XML,true);
    ui->tableView->setColumnHidden(ACCOUNT_TRACE,true);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setCascadingSectionResizes (true);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    //ui->tableView->resizeColumnsToContents();
    coloringDoubles();
    QString textResult = textOfSums(m_accountModel);
    ui->resultLabel->setText(textResult);
    QApplication::restoreOverrideCursor();
}

void ControlReceipts::deleteLine(){
  QModelIndex index = ui->tableView->QAbstractItemView::currentIndex();
  if (!index.isValid()) {
      Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Select a line."));
      return;
  }
  int rowToDelete = index.row();
  if(m_accountModel->getDoublesRows.contains(rowToDelete)){
      m_accountModel->getDoublesRows.removeAll(rowToDelete);
      if (m_accountModel->getDoublesRows.size()<1) {
          m_accountModel->getDoublesRows.clear();
      }
  }
  if(m_accountModel->removeRows(rowToDelete,1,QModelIndex())) {
      Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Line deleted."));
  }
  QString textResult = textOfSums(m_accountModel);
  ui->resultLabel->setText(textResult);
  //const QString filter = m_accountModel->filter();
  coloringDoubles();
          search();
  /*if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
  refreshFilter(filter);*/
}

QString ControlReceipts::textOfSums(AccountModel * model){
    QString labelText;
    QString labelTextStr;
    double cash = 0.00;
    double chq = 0.00;
    double visa = 0.00;
    double banking = 0.00;
    double other = 0.00;
    double dues = 0.00;
    double totalReceived = 0.00;
    double totals = 0.00;
   int modelRowCount = model->rowCount(QModelIndex());
   if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " modelRowCount = " << QString::number(modelRowCount);
   for(int i = 0; i < modelRowCount ; i ++){
       QSqlRecord rowRecord = model->record(i);//ligne d'enregistrement
       QString c  = rowRecord.value(ACCOUNT_CASHAMOUNT).toString();
       QString ch = rowRecord.value(ACCOUNT_CHEQUEAMOUNT).toString();
       QString v  = rowRecord.value(ACCOUNT_VISAAMOUNT).toString();
       QString b  = rowRecord.value(ACCOUNT_INSURANCEAMOUNT).toString();
       QString o  = rowRecord.value(ACCOUNT_OTHERAMOUNT).toString();
       QString d  = rowRecord.value(ACCOUNT_DUEAMOUNT).toString();
       QStringList list;
       list << c << ch << v << b << o << d;
       if (c.toDouble() == 0.0 || ch.toDouble() == 0.0 || v.toDouble() == 0.0 || b.toDouble() == 0.0
           || o.toDouble() == 0.0 || d.toDouble() == 0.0)
       {
           foreach(QString s,list){
               s.replace(",",QLocale::c().decimalPoint ());
               s.replace(".",QLocale::c().decimalPoint ());
               }

           }
       cash  += list[ReceiptsConstants::Cash].toDouble();
       chq  += list[ReceiptsConstants::Check].toDouble();
       visa += list[ReceiptsConstants::Visa].toDouble();
       banking += list[ReceiptsConstants::Banking].toDouble();
       other += list[ReceiptsConstants::Other].toDouble();
       dues  += list[ReceiptsConstants::Due].toDouble();
       }
    totals = cash + chq + visa + banking + other + dues;
    totalReceived = cash + chq + banking + other + visa;
    QString cashStr = tr("Cash");
    QString chqStr = tr("Chq");
    QString visaStr = tr("CB");
    QString bankingStr = tr("Banking");
    QString otherStr = tr("Others");
    QString duesStr = tr("Dues");
    QString totStr = "<font size = 3 color = ""blue"">Totaux = </font><font size = 3 color = ""red"">"+QString::number(totals)+" "+m_typeOfMoney+" </font>  ";
    QString totReceived = "<font size = 3 color = ""blue"">Totaux reçus = </font><font size = 3 color = ""red"">"+QString::number(totalReceived)+" "+m_typeOfMoney+" </font><br/>";
    QString sumsStr = "<font size = 3 color = ""blue"">"+cashStr+" = </font><font size = 3 color = ""red"">"+QString::number(cash)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">"+chqStr+" = </font><font size = 3 color = ""red"">"+QString::number(chq)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">"+visaStr+" = </font><font size = 3 color = ""red"">"+QString::number(visa)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">"+bankingStr+" = </font><font size = 3 color = ""red"">"+QString::number(banking)+" "+m_typeOfMoney+"  </font><br/>"+
                "<font size = 3 color = ""blue"">"+otherStr+" = </font><font size = 3 color = ""red"">"+QString::number(other)+" "+m_typeOfMoney+"  </font>"+
                "<font size = 3 color = ""blue"">"+duesStr+" = </font><font size = 3 color = ""red"">"+QString::number(dues)+" "+m_typeOfMoney+"</font>";
    labelTextStr = totStr+totReceived+sumsStr;
    labelText = "<html><body>"+labelTextStr+"</body></html>";
    return labelText;
}

void ControlReceipts::printDues(){
    Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tkTr(Trans::Constants::FEATURE_NOT_IMPLEMENTED));
}

void ControlReceipts::print()
{
    QString html = getHtmlDocAccordingToTableView();
    print(html);
}


void ControlReceipts::print(QString & html)
{
      Core::IDocumentPrinter *p = printer();
      if (!p) {
          Utils::Log::addError(this, "No IDocumentPrinter found", __FILE__, __LINE__);
          return;
      }

      p->clearTokens();
      QHash<QString, QVariant> tokens;
      tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, this->windowTitle());
      p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);
      p->print(html);
}



void ControlReceipts::coloringDoubles(){
    int rowCount = m_accountModel->rowCount();
    int columnCount = m_accountModel->columnCount();
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << QString::number(rowCount) << " " << QString::number(columnCount);
    QList<int> listRows;
    for (int i = 0; i < rowCount; i += 1)
    {
        QList<QVariant> dataRow ;

        for (int c = 1; c < columnCount; c += 1)
        {
                QModelIndex index = m_accountModel->index(i,c,QModelIndex());
            dataRow << m_accountModel->data(index,Qt::DisplayRole);
        }
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " dataRow.size =" << QString::number(dataRow.size());
        for (int j = i; j < rowCount ; j += 1)
        {
                   if(j!=i){
                    QList<QVariant> dataAfterRow ;
                        for (int c = 1; c < columnCount; c += 1)
                        {
                                QModelIndex indexAfter = m_accountModel->index(j,c,QModelIndex());
                            dataAfterRow += m_accountModel->data(indexAfter,Qt::DisplayRole);
                        }

                        if (WarnDebugMessage)
                            qDebug() << __FILE__ << QString::number(__LINE__) << " dataAfterRow.size =" << QString::number(dataAfterRow.size());
                        if (dataAfterRow == dataRow)
                        {
                            if (WarnDebugMessage) {
                                qDebug() << __FILE__ << QString::number(__LINE__) << " dataAfterRow= "<< dataAfterRow;
                                qDebug() << __FILE__ << QString::number(__LINE__) << QString::number(i) << QString::number(j);
                            }
                            listRows << i << j;
                        }
                        else{}
                   }
        }

    }
    m_accountModel->getDoublesRows = listRows;
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " listRows = " << QString::number(listRows.size());

    m_accountModel->submit();
}

void ControlReceipts::refresh(){
    delete m_accountModel;
    m_accountModel = new AccountModel(this);
    if (!m_accountModel->setHeaderData(ACCOUNT_PATIENT_NAME,Qt::Horizontal , tr("Patient") , Qt::EditRole)) {
        if (WarnDebugMessage)
            qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to set header data" ;
    }
    m_accountModel->setHeaderData(ACCOUNT_DATE,Qt::Horizontal , tr("Date") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_MEDICALPROCEDURE_TEXT,Qt::Horizontal , tr("Acts") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_COMMENT,Qt::Horizontal , tr("Comment") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_CASHAMOUNT,Qt::Horizontal , tr("Cash") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_CHEQUEAMOUNT,Qt::Horizontal , tr("Checks") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_VISAAMOUNT,Qt::Horizontal , tr("Credit card") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_INSURANCEAMOUNT,Qt::Horizontal , tr("Insurance") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_OTHERAMOUNT,Qt::Horizontal , tr("Other") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_DUEAMOUNT,Qt::Horizontal , tr("Due") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_DUEBY,Qt::Horizontal , tr("by") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_ISVALID,Qt::Horizontal , tr("Valid") , Qt::EditRole);
    m_mapCombo.clear();
    m_mapCombo.insert(tr("Patient"),"PATIENT_NAME");
    m_mapCombo.insert(tr("Cash"),"CASH");
    m_mapCombo.insert(tr("Check"),"CHEQUE");
    m_mapCombo.insert(tr("Credit card"),"VISA");
    m_mapCombo.insert(tr("Banking"),"BANKING");
    m_mapCombo.insert(tr("Other"),"OTHER");
    m_mapCombo.insert(tr("Due"),"DUE");
    m_mapCombo.insert(tr("Due by"),"DUE_BY");
    QStringList listForCombo;
    listForCombo = m_mapCombo.keys();
    listForCombo.prepend(tr("Patient"));
    listForCombo.removeDuplicates();
    ui->fieldComboBox->clear();
    ui->fieldComboBox->addItems(listForCombo);
    ui->fieldComboBox->setEditText(tr("Patient"));
}

void ControlReceipts::refreshFilter(const QString & filter){
    delete m_accountModel;
    m_accountModel = new AccountModel(this);
    m_accountModel->setFilter(filter);
    if (!m_accountModel->setHeaderData(ACCOUNT_PATIENT_NAME,Qt::Horizontal , tr("Patient") , Qt::EditRole))
    {
        if (WarnDebugMessage)
            qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to set header data" ;
        }
    m_accountModel->setHeaderData(ACCOUNT_DATE,Qt::Horizontal , tr("Date"), Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_MEDICALPROCEDURE_TEXT,Qt::Horizontal, tr("Acts"), Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_COMMENT,Qt::Horizontal, tr("Comment"), Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_CASHAMOUNT,Qt::Horizontal, tr("Cash"), Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_CHEQUEAMOUNT,Qt::Horizontal, tr("Checks"), Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_VISAAMOUNT,Qt::Horizontal, tr("Credit card"), Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_INSURANCEAMOUNT,Qt::Horizontal, tr("Insurance"), Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_OTHERAMOUNT,Qt::Horizontal, tr("Other"), Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_DUEAMOUNT,Qt::Horizontal, tr("Due"), Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_DUEBY,Qt::Horizontal, tr("by") , Qt::EditRole);
    m_accountModel->setHeaderData(ACCOUNT_ISVALID,Qt::Horizontal, tr("Valid"), Qt::EditRole);
    ui->tableView->setModel(m_accountModel);
    ui->tableView->setColumnHidden(ACCOUNT_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_USER_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_PATIENT_UID,true);
    ui->tableView->setColumnHidden(ACCOUNT_SITE_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_INSURANCE_ID,true);
    ui->tableView->setColumnHidden(ACCOUNT_MEDICALPROCEDURE_XML,true);
    ui->tableView->setColumnHidden(ACCOUNT_TRACE,true);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

void ControlReceipts::closeAction(){
    emit isClosing();
    emit close();
}

void ControlReceipts::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    if (e->type()==QEvent::LanguageChange) {
        ui->retranslateUi(this);
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " langage changed " ;
        search();
        }
}

QString ControlReceipts::getHtmlDocAccordingToTableView()
{
    QStringList listOfSums;
    listOfSums = getListOfSums();
    QString htmlDocument = "<html><body><font size=3>";
    htmlDocument += "<TABLE BORDER=1>";
    htmlDocument += "<CAPTION><font color = red size = 6>"+tr("Extract of receipts")+"</font></CAPTION>";
    htmlDocument += "<TR>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Patient")+"</bold></font></TH>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Date")+"</bold></font></TH>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Acts")+"</bold></font></TH>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Cash")+"</bold></font></TH>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Checks")+"</bold></font></TH>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Credit Cards")+"</bold></font></TH>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Insurance")+"</bold></font></TH>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Others")+"</bold></font></TH>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Dues")+"</bold></font></TH>";
    htmlDocument += "<TH><font color = blue><bold>"+tr("Due by")+"</bold></font></TH>";
    htmlDocument += "</TR>";

    QAbstractItemModel *model = ui->tableView->model();
    QList<int> listOfColumnsExcluded;
    listOfColumnsExcluded << ACCOUNT_ID
                          << ACCOUNT_SITE_ID
                          << ACCOUNT_INSURANCE_ID
                          << ACCOUNT_MEDICALPROCEDURE_XML
                          << ACCOUNT_COMMENT
                          << ACCOUNT_UID
                          << ACCOUNT_USER_UID
                          << ACCOUNT_PATIENT_UID
                          << ACCOUNT_ISVALID
                          << ACCOUNT_TRACE ;
    QList<int> listOfHeadersNumbers;
    listOfHeadersNumbers << ACCOUNT_PATIENT_NAME
                         << ACCOUNT_DATE
                         << ACCOUNT_MEDICALPROCEDURE_TEXT
                         << ACCOUNT_CASHAMOUNT
                         << ACCOUNT_CHEQUEAMOUNT
                         << ACCOUNT_VISAAMOUNT
                         << ACCOUNT_INSURANCEAMOUNT
                         << ACCOUNT_OTHERAMOUNT
                         << ACCOUNT_DUEAMOUNT
                         << ACCOUNT_DUEBY ;

    for (int row = 0; row < model->rowCount(); ++row )
    {
          htmlDocument += "<TR>";
          for (int col = 0; col < model->columnCount() ; ++col)
          {

              if (!listOfColumnsExcluded.contains(col))
              {
              htmlDocument += "<TD>";
              htmlDocument += model->data(model->index(row,col),Qt::DisplayRole).toString();
              htmlDocument += "</TD>";
                  }
              }
          htmlDocument += "</TR>";
        }
    htmlDocument += "<TR>";
    for (int cell = 0; cell < HeadersForPrint_MaxParam; ++cell)
    {
          if (cell < CASH_HEADER || cell > DUE_HEADER)
          {
              if (cell == ACTS_HEADER)
              {
                 htmlDocument += "<TD>";
                 htmlDocument+= tr("TOTAL");
                 htmlDocument += "</TD>";
                  }
              else
              {
                  htmlDocument += "<TD>";
                  htmlDocument+= "";
                  htmlDocument += "</TD>";
                  }

              }
          else
          {
              htmlDocument += "<TD>";
              qDebug() << __FILE__ << QString::number(__LINE__) << " listOfSums =" << QString::number(cell-ACTS_HEADER) ;
              htmlDocument+= listOfSums[cell-CASH_HEADER];
              htmlDocument += "</TD>";
              }

        }
    htmlDocument += "</TR>";
    htmlDocument += "</TABLE></font></body></html>";
    return htmlDocument;
}

QStringList ControlReceipts::getListOfSums()
{
    QStringList listOfSums;
    double cash = 0.00;
    double chq = 0.00;
    double visa = 0.00;
    double banking = 0.00;
    double other = 0.00;
    double dues = 0.00;
//    double totalReceived = 0.00;
//    double totals = 0.00;
    QAbstractItemModel *model = ui->tableView->model();
   int modelRowCount = model->rowCount(QModelIndex());
   for(int i = 0; i < modelRowCount ; i ++){
       QString c  = model->data(model->index(i,ACCOUNT_CASHAMOUNT),Qt::DisplayRole).toString();
       QString ch = model->data(model->index(i,ACCOUNT_CHEQUEAMOUNT),Qt::DisplayRole).toString();
       QString v  = model->data(model->index(i,ACCOUNT_VISAAMOUNT),Qt::DisplayRole).toString();
       QString b  = model->data(model->index(i,ACCOUNT_INSURANCEAMOUNT),Qt::DisplayRole).toString();
       QString o  = model->data(model->index(i,ACCOUNT_OTHERAMOUNT),Qt::DisplayRole).toString();
       QString d  = model->data(model->index(i,ACCOUNT_DUEAMOUNT),Qt::DisplayRole).toString();
       QStringList list;
       list << c << ch << v << b << o << d;
       if (c.toDouble() == 0.0 || ch.toDouble() == 0.0 || v.toDouble() == 0.0 || b.toDouble() == 0.0
           || o.toDouble() == 0.0 || d.toDouble() == 0.0)
       {
           foreach(QString s,list){
               s.replace(",",QLocale::c().decimalPoint ());
               s.replace(".",QLocale::c().decimalPoint ());
               }
           }
       cash  += list[ReceiptsConstants::Cash].toDouble();
       chq  += list[ReceiptsConstants::Check].toDouble();
       visa += list[ReceiptsConstants::Visa].toDouble();
       banking += list[ReceiptsConstants::Banking].toDouble();
       other += list[ReceiptsConstants::Other].toDouble();
       dues  += list[ReceiptsConstants::Due].toDouble();
       }
     listOfSums << QString::number(cash)
                << QString::number(chq)
                << QString::number(visa)
                << QString::number(banking)
                << QString::number(other)
                << QString::number(dues) ;
     return listOfSums;
}

