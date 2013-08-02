/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "ledgerviewer.h"
#include "ui_ledgerviewer.h"
#include "ledgerio.h"

#include "../receipts/receiptsmanager.h"

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants.h>

#include <QRect>
#include <QGraphicsDropShadowEffect>
#include <QPainter>

#include <QDebug>

using namespace Trans::ConstantTranslations;
using namespace Constants;

enum { WarnDebugMessage = false };

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

LedgerViewer::LedgerViewer(QWidget * parent): QWidget(parent),ui(new Ui::LedgerViewerWidget){
    ui->setupUi(this);
    m_currency = tr("euro");
    m_lm = new LedgerManager(this);
    m_ledgerEdit = new LedgerEdit(this);
    m_ledgerEdit->hide();
    /*"QMenuBar {"
                           " background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                           "stop:0 lightgray, stop:1 darkgray);}"*/
    m_menuBar = new QMenuBar(this);
    m_menuBar->setAttribute(Qt::WA_TranslucentBackground);
    QString menuBarStyle = "QMenuBar {"
                           " background-color:lightgray ;}"
                           "QMenuBar::item {"
                           "spacing: 3px; // spacing between menu bar items"
                           "padding: 1px 4px;"
                           "background: transparent;"
                           "border-radius: 4px;}"
                           "QMenuBar::item:selected {" // when selected using mouse or keyboard
                           "background: #a8a8a8;}"
                           "QMenuBar::item:pressed {"
                           "background: #888888;}";
    m_menuBar->setStyleSheet(menuBarStyle);
    QStringList listOfMonths;
    listOfMonths = m_lm->getListOfMonths();
    ui->monthsComboBox->addItems(listOfMonths);
    QStringList listOfYears;
    QString currentDate = QDate::currentDate().toString("yyyy");
    listOfYears << currentDate;
    listOfYears << m_lm->getListOfYears();
    listOfYears.removeDuplicates();
    ui->yearsComboBox->addItems(listOfYears);
    ui->tableView->setShowGrid(false);
    if(createActions()){
        createMenus();
        fillMenuBar();
        }
    connect(ui->monthsComboBox,SIGNAL(activated(const QString&)),this,
                               SLOT(monthsComboBoxcurrentIndexChanged(const QString&)));

}

LedgerViewer::~LedgerViewer(){}

void LedgerViewer::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    if (e->type()==QEvent::LanguageChange) {
        ui->retranslateUi(this);
        if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << "LedgerViewer::changeEvent(QEvent *e)"  ;
        /*m_menuWidgetAction = new QMenu(QObject::tr("&Program","Ledger file"),this);
        m_menuAnalyze = new QMenu(tr("&Analyse"),this);
        m_ledger = new QMenu(tr("&Ledger"),this);*/
        delete m_menuWidgetAction;
        delete m_menuAnalyze;
        delete m_ledger;
        if(createActions()){
        createMenus();
        fillMenuBar();
        }
    }
}

void LedgerViewer::fillMenuBar(){

    m_menuBar->addMenu(m_menuWidgetAction);
    m_menuBar->addMenu(m_menuAnalyze);
    m_menuBar->addMenu(m_ledger);
    ui->menuHorizontalLayout->addWidget(m_menuBar);
}

void LedgerViewer::createMenus(){

    m_menuWidgetAction = new QMenu(QObject::tr("&Program","Ledger file"),this);
    m_menuWidgetAction->addAction(m_closeAction);
    m_menuAnalyze = new QMenu(tr("&Analyse"),this);
    m_menuAnalyze->addAction(m_monthlyReceiptsAnalysis);
    m_menuAnalyze->addAction(m_monthlyAndTypeReceiptsAnalysis);
    m_menuAnalyze->addAction(m_yearlyAndTypeReceiptsAnalysis);
    m_menuAnalyze->addAction(m_monthlyMovementsAnalysis);
    m_menuAnalyze->addAction(m_monthlyAndTypeMovementsAnalysis);
    m_menuAnalyze->addAction(m_yearlyAndTypeMovementsAnalysis);
    m_ledger = new QMenu(tr("&Ledger"),this);
    m_ledger->addAction(m_ledgerActionShow);
}

bool LedgerViewer::createActions(){
    bool b = true;
    m_closeAction = new QAction(this);
    m_closeAction->setText(tr("E&xit"));
    m_closeAction->setShortcut(QKeySequence::Close);
    m_closeAction->setIcon(theme()->icon(Core::Constants::ICONQUIT));
    m_closeAction->setIconVisibleInMenu(true);
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_closeAction icon =" << qApp->applicationDirPath()+"/../../global_resources/pixmap/16x16/exit.png";
    m_closeAction->setStatusTip(tr("Close Ledger"));
    m_hashTextAndAction.insert(m_closeAction->text(),m_closeAction);
    b = connect(m_closeAction, SIGNAL(triggered()), this, SLOT(quitFreeAccount()));

    m_monthlyReceiptsAnalysis = new QAction(tr("Receipts by month"),this);
    m_monthlyReceiptsAnalysis->setStatusTip(tr("See receipts by month."));
    m_hashTextAndAction.insert(m_monthlyReceiptsAnalysis->text(),m_monthlyReceiptsAnalysis);
    b = connect(m_monthlyReceiptsAnalysis, SIGNAL(triggered()), this, SLOT(monthlyReceiptsAnalysis()));

    m_monthlyAndTypeReceiptsAnalysis = new QAction(tr("Receipts by month and type"),this);
    m_monthlyAndTypeReceiptsAnalysis->setStatusTip(tr("See receipts by month and type."));
    m_hashTextAndAction.insert(m_monthlyAndTypeReceiptsAnalysis->text(),m_monthlyAndTypeReceiptsAnalysis);
    b = connect(m_monthlyAndTypeReceiptsAnalysis, SIGNAL(triggered()), this, SLOT(monthlyAndTypeReceiptsAnalysis()));

    m_yearlyAndTypeReceiptsAnalysis = new QAction(tr("Receipts by year and type"),this);
    m_yearlyAndTypeReceiptsAnalysis->setStatusTip(tr("See receipts by year and type."));
    m_hashTextAndAction.insert(m_yearlyAndTypeReceiptsAnalysis->text(),m_yearlyAndTypeReceiptsAnalysis);
    b = connect(m_yearlyAndTypeReceiptsAnalysis, SIGNAL(triggered()), this, SLOT(yearlyAndTypeReceiptsAnalysis()));

    m_monthlyMovementsAnalysis = new QAction(tr("Movements by month"),this);
    m_monthlyMovementsAnalysis->setStatusTip(tr("See receipts by month."));
    m_hashTextAndAction.insert(m_monthlyMovementsAnalysis->text(),m_monthlyMovementsAnalysis);
    b = connect(m_monthlyMovementsAnalysis, SIGNAL(triggered()), this, SLOT(monthlyMovementsAnalysis()));

    m_monthlyAndTypeMovementsAnalysis = new QAction(tr("Movements by month and type"),this);
    m_monthlyAndTypeMovementsAnalysis->setStatusTip(tr("See receipts by month and type."));
    m_hashTextAndAction.insert(m_monthlyAndTypeMovementsAnalysis->text(),m_monthlyAndTypeMovementsAnalysis);
    b = connect(m_monthlyAndTypeMovementsAnalysis, SIGNAL(triggered()), this, SLOT(monthlyAndTypeMovementsAnalysis()));

    m_yearlyAndTypeMovementsAnalysis = new QAction(tr("Movements by year and type"),this);
    m_yearlyAndTypeMovementsAnalysis->setStatusTip(tr("See receipts by year and type."));
    m_hashTextAndAction.insert(m_monthlyAndTypeMovementsAnalysis->text(),m_monthlyAndTypeMovementsAnalysis);
    b = connect(m_yearlyAndTypeMovementsAnalysis, SIGNAL(triggered()), this, SLOT(yearlyAndTypeMovementsAnalysis()));

    m_ledgerActionShow = new QAction(tr("&Ledger"),this);
    m_ledgerActionShow->setStatusTip(tr("See ledger."));
    m_ledgerActionShow->setShortcut(QKeySequence("CTRL+L"));
    m_ledgerActionShow->setIcon(theme()->icon(Core::Constants::ICONEDIT));
    m_ledgerActionShow->setIconVisibleInMenu(true);
    m_hashTextAndAction.insert(m_ledgerActionShow->text(),m_ledgerActionShow);
    b = connect(m_ledgerActionShow, SIGNAL(triggered()), this, SLOT(ledgerActionShow()));
    return b;
}

void LedgerViewer::monthlyReceiptsAnalysis(){
    m_actionText = m_monthlyReceiptsAnalysis->text();
    QString month = ui->monthsComboBox->currentText();
    QString year = ui->yearsComboBox->currentText();
    AccountModel * model = m_lm->getModelMonthlyReceiptsAnalysis(this,
                                                              month,
                                                              year);
    model->setHeaderData( ACCOUNT_PATIENT_NAME, Qt::Horizontal, tr("PATIENT NAME"), Qt::EditRole );
    model->setHeaderData( ACCOUNT_SITE_ID, Qt::Horizontal, tr("SITE ID"), Qt::EditRole );
    model->setHeaderData( ACCOUNT_INSURANCE_ID, Qt::Horizontal, tr("INSURANCE ID"), Qt::EditRole );
    model->setHeaderData( ACCOUNT_MEDICALPROCEDURE_TEXT, Qt::Horizontal, tr("TEXT"), Qt::EditRole );
    ui->tableView->setModel(model);
    QList<int> listOff ;
    listOff << ACCOUNT_ID
            << ACCOUNT_UID
            << ACCOUNT_USER_UID
            << ACCOUNT_PATIENT_UID
            << ACCOUNT_MEDICALPROCEDURE_XML
            << ACCOUNT_TRACE;
    for (int i = 0; i < listOff.size(); i += 1)
    {
        ui->tableView->setColumnHidden(listOff[i],true);
        }

    QString labelText = "Total = "+QString::number(m_lm->getSums())+" "+m_currency;
    ui->sumLabel->setText(labelText);
}
void LedgerViewer::monthlyAndTypeReceiptsAnalysis(){
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " in monthlyAndTypeReceiptsAnalysis " ;
    m_actionText = m_monthlyAndTypeReceiptsAnalysis->text();
    QString month = ui->monthsComboBox->currentText();
    QString year = ui->yearsComboBox->currentText();
    QItemSelectionModel *m = ui->tableView->selectionModel();
    delete m;
    QStandardItemModel * modelStd = m_lm->getModelMonthlyAndTypeReceiptsAnalysis(this,
                                                                                 month,
                                                                                 year);
    modelStd->setHeaderData( 0, Qt::Horizontal, tr("TYPE"), Qt::EditRole );
    modelStd->setHeaderData( 1, Qt::Horizontal, tr("VALUE"), Qt::EditRole );
    ui->tableView->setModel(modelStd);
    ui->tableView->resizeColumnsToContents();
    QString labelText = "Total = "+QString::number(m_lm->getSums())+" "+m_currency;
    ui->sumLabel->setText(labelText);
}
void LedgerViewer::yearlyAndTypeReceiptsAnalysis(){
    m_actionText = m_yearlyAndTypeReceiptsAnalysis->text();
    QString year = ui->yearsComboBox->currentText();
    QStandardItemModel * model = m_lm->getModelYearlyAndTypeReceiptsAnalysis(this,year);
    model->setHeaderData( 0, Qt::Horizontal, tr("TYPE"), Qt::EditRole );
    model->setHeaderData( 1, Qt::Horizontal, tr("VALUE"), Qt::EditRole );
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    QString labelText = "Total = "+QString::number(m_lm->getSums())+" "+m_currency;
    ui->sumLabel->setText(labelText);
}
void LedgerViewer::monthlyMovementsAnalysis(){
    m_actionText = m_monthlyMovementsAnalysis->text();
    QString month = ui->monthsComboBox->currentText();
    QString year = ui->yearsComboBox->currentText();
    MovementModel * model = m_lm->getModelMonthlyMovementsAnalysis(this,month,year);
    ui->tableView->setModel(model);
    QList<int> listOff ;
    listOff << MOV_ID
            << MOV_AV_MOVEMENT_ID
            << ACCOUNT_USER_UID
            << MOV_ACCOUNT_ID
            << MOV_TRACE;
    for (int i = 0; i < listOff.size(); i += 1)
    {
        ui->tableView->setColumnHidden(listOff[i],true);
        }
    QString labelText = "Total = "+QString::number(m_lm->getSums())+" "+m_currency;
    ui->sumLabel->setText(labelText);

}
void LedgerViewer::monthlyAndTypeMovementsAnalysis(){
    m_actionText = m_monthlyAndTypeMovementsAnalysis->text();
    QString month = ui->monthsComboBox->currentText();
    QString year = ui->yearsComboBox->currentText();
    QStandardItemModel * model = m_lm->getModelMonthlyAndTypeMovementAnalysis(this,
                                                                              month,
                                                                              year);
    model->setHeaderData( 0, Qt::Horizontal, tr("TYPE"), Qt::EditRole );
    model->setHeaderData( 1, Qt::Horizontal, tr("VALUE"), Qt::EditRole );
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    QString labelText = "Total = "+QString::number(m_lm->getSums())+" "+m_currency;
    ui->sumLabel->setText(labelText);
}

void LedgerViewer::yearlyAndTypeMovementsAnalysis(){
    m_actionText = m_yearlyAndTypeMovementsAnalysis->text();
    QString year = ui->yearsComboBox->currentText();
    QStandardItemModel * model = m_lm->getModelYearlyAndTypeMovementAnalysis(this,year);
    model->setHeaderData( 0, Qt::Horizontal, tr("TYPE"), Qt::EditRole );
    model->setHeaderData( 1, Qt::Horizontal, tr("VALUE"), Qt::EditRole );
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    QString labelText = "Total = "+QString::number(m_lm->getSums())+" "+m_currency;
    ui->sumLabel->setText(labelText);
}

void LedgerViewer::ledgerActionShow(){
    ui->sumLabel->setText("");
    m_ledgerEdit->show();
}

void LedgerViewer::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
    ui->sumLabel->setText("");
    m_ledgerEdit->resizeLedgerEdit(this);
}

void LedgerViewer::monthsComboBoxcurrentIndexChanged(const QString& month){
    Q_UNUSED(month);
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " in monthsComboBoxcurrentIndexChanged , month = "+month  ;
    if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " m_actionText =" << m_actionText ;
    if (m_actionText.isEmpty()) {
        //Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Choose an action."));
        return;
    } else {
        QAction * chosenAction = m_hashTextAndAction.value(m_actionText);
        chosenAction->activate(QAction::Trigger);
    }
}

void LedgerViewer::quitFreeAccount()
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
