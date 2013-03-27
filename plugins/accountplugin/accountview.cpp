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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "accountview.h"
#include "ui_accountview.h"

#include "receipts/receiptsmanager.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants.h>

#include <accountbaseplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QTableView>

#include <QDebug>

using namespace AccountDB;
using namespace Account;
using namespace Trans::ConstantTranslations;
using namespace Constants;

enum { WarnDebugMessage = false };

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

/*namespace Account {
namespace Internal {
class AccountViewPrivate
{
public:
    AccountViewPrivate(AccountView *parent) : m_ui(0), m_Model(0), q(parent)
    {
        m_ui = new Ui::AccountView;
    }
    ~AccountViewPrivate()
    {
        if (m_ui) {
            delete m_ui;
            m_ui = 0;
        }
    }

public:
    Ui::AccountView * m_ui;
    AccountDB::AccountModel *m_Model;

private:
    AccountView *q;
};
}
}*/
AccountView::AccountView(QWidget *parent) :
        QWidget(parent), m_ui(new Ui::AccountViewWidget)//, d(new Internal::AccountViewPrivate(this))
{
    setObjectName("AccountView");
    m_ui->setupUi(this);
    setHeadersOfTable();

    m_userUuid = m_Model->getUserUuid();
    int thisYear = QDate::currentDate().year();
    QDate beginDate = QDate(thisYear,01,01);
    m_ui->quitButton->setEnabled(false);
    m_ui->quitButton->hide();
    ReceiptsManager receiptsManager;
    if (receiptsManager.isMedintuxArg())
    {
        m_ui->quitButton->setEnabled(true);
        m_ui->quitButton->show();
        }
    m_ui->startDate->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    m_ui->endDate->setDisplayFormat(tkTr(Trans::Constants::DATEFORMAT_FOR_EDITOR));
    m_ui->startDate->setDate(beginDate);
    m_ui->endDate->setDate(QDate::currentDate());
    //icons and shortcuts
    m_ui->deleteButton->setShortcut(QKeySequence::Delete);
    m_ui->deleteButton->setToolTip(QKeySequence(QKeySequence::Delete).toString());
    m_ui->deleteButton->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
    //-------------
    const QString sumTextLabel = QString("<html><body><font color=blue size=3>%1</font></body></html>")
                                .arg(tr("Sum"));
    m_ui->sumLabel->setText(sumTextLabel);
    refresh();
    calc();
    connect(m_ui->deleteButton,SIGNAL(pressed()),this,SLOT(deleteLine()));
    connect(m_ui->quitButton,SIGNAL(pressed()),this,SLOT(quitFreeAccount()));

}

AccountView::~AccountView()
{
        qWarning() << "AccountView::~AccountView()" ;
        if (m_ui) {

            delete m_ui;
            m_ui = 0;}
}

void AccountView::setHeadersOfTable(){
    if(m_Model)
    m_Model = NULL;
    m_Model = new AccountDB::AccountModel(this);
    m_Model->select();
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_PATIENT_NAME,Qt::Horizontal, tr("Name"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_SITE_ID,Qt::Horizontal, tr("Site id"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_DATE,Qt::Horizontal, tr("Date"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_MEDICALPROCEDURE_TEXT,Qt::Horizontal, tr("Act"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_COMMENT,Qt::Horizontal, tr("Comment"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_CASHAMOUNT,Qt::Horizontal, tr("Cash"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_CHEQUEAMOUNT,Qt::Horizontal, tr("Check"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_VISAAMOUNT,Qt::Horizontal, tr("Credit Card"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_INSURANCEAMOUNT,Qt::Horizontal, tr("Banking"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_OTHERAMOUNT,Qt::Horizontal, tr("Other"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_DUEAMOUNT,Qt::Horizontal, tr("Due"));
    m_Model->setHeaderData(AccountDB::Constants::ACCOUNT_DUEBY,Qt::Horizontal, tr("Due by"));
    while (m_Model->canFetchMore())
    {
          m_Model->fetchMore();
        }
    m_ui->tableView->setModel(m_Model);
#if QT_VERSION < 0x050000
    m_ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    m_ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#else
    // Qt5
    m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#endif
    m_ui->tableView->show();
}

void AccountView::refresh(){
    QString dateBeginStr = m_ui->startDate->date().toString("yyyy-MM-dd");
    QString dateEndStr = m_ui->endDate->date().toString("yyyy-MM-dd");
    QString filter = QString("%1 = '%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    //AccountDB::AccountModel *model = new AccountDB::AccountModel(this);
    m_Model->setFilter(filter);
    while (m_Model->canFetchMore())
    {
          m_Model->fetchMore();
        }
    if(WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_Model->filter() ;
    if(WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(m_Model->rowCount()) ;
    m_ui->tableView->setModel(m_Model);

    QList<int> hide;
    hide
            << AccountDB::Constants::ACCOUNT_ID
            << AccountDB::Constants::ACCOUNT_UID
            << AccountDB::Constants::ACCOUNT_USER_UID
            << AccountDB::Constants::ACCOUNT_INSURANCE_ID
            << AccountDB::Constants::ACCOUNT_ISVALID
            << AccountDB::Constants::ACCOUNT_PATIENT_UID
            << AccountDB::Constants::ACCOUNT_MEDICALPROCEDURE_XML
            << AccountDB::Constants::ACCOUNT_TRACE;
    foreach(int i, hide) {
        m_ui->tableView->hideColumn(i);
    }
}

void AccountView::filterChanged()
{
    // Filter model
    //m_Model->setStartDate(m_ui->startDate->date());
    //m_Model->setEndDate(m_ui->endDate->date());
    if(WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " in filterChanged " ;
    // Calculate sums of paiements
    /*m_ui->cash->setText(QString::number(m_Model->sum(AccountDB::Constants::ACCOUNT_CASHAMOUNT)));
    m_ui->cheque->setText(QString::number(m_Model->sum(AccountDB::Constants::ACCOUNT_CHEQUEAMOUNT)));
    m_ui->du->setText(QString::number(m_Model->sum(AccountDB::Constants::ACCOUNT_DUEAMOUNT)));
    m_ui->insurance->setText(QString::number(m_Model->sum(AccountDB::Constants::ACCOUNT_INSURANCEAMOUNT)));
    m_ui->visa->setText(QString::number(m_Model->sum(AccountDB::Constants::ACCOUNT_VISAAMOUNT)));*/
    setHeadersOfTable();
    refresh();
    calc();
}

void AccountView::deleteLine(){
    QModelIndex index = m_ui->tableView->QAbstractItemView::currentIndex();
    if(!index.isValid()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Please select a line to delete"));
        return;
    }
    int i = index.row();
    QAbstractItemModel *model = m_ui->tableView->model();
    if (model->removeRows(i,1,QModelIndex())) {
        Utils::informativeMessageBox(tkTr(Trans::Constants::INFORMATION), tr("Line deleted"));
    }
    refresh();
}

void AccountView::on_startDate_dateChanged(const QDate &date)
{
    Q_UNUSED(date);
    refresh();
    calc();
    //m_Model->setStartDate(date);
}

void AccountView::on_endDate_dateChanged(const QDate &date)
{
    Q_UNUSED(date);
    refresh();
    calc();
    //m_Model->setEndDate(date);
}

void AccountView::on_periodCombo_currentIndexChanged(int index)
{
    // All // Today // Yesterday // Week // Month // Year
    QDate start, end;

    switch (index) {
    case 0 : // all
        start = QDate(2000,01,01);
        end = QDate::currentDate();
        break;
    case 1 : // today
        start = QDate::currentDate();
        end = QDate::currentDate();
        break;
    case 2 : // yesterday
        start = QDate::currentDate().addDays(-1);
        end = QDate::currentDate().addDays(-1);
        break;
    case 3 : // week
        start = QDate::currentDate().addDays(-7);
        end = QDate::currentDate();
        break;
    case 4 : // month
        start = QDate::currentDate().addMonths(-1);
        end = QDate::currentDate();
        break;
    case 5 : // Year
        start = QDate::currentDate().addYears(-1);
        end = QDate::currentDate();
        break;
    }

    blockSignals(true);
    m_ui->startDate->setDate(start);
    m_ui->endDate->setDate(end);
    blockSignals(false);

    filterChanged();
}

void AccountView::calc()
{
    if (WarnDebugMessage)
        qDebug() << __FILE__ << QString::number(__LINE__) << " in calc " ;
    QHash<int,QLineEdit*> hash;
    hash.insert(AccountDB::Constants::ACCOUNT_CASHAMOUNT,m_ui->cash);
    hash.insert(AccountDB::Constants::ACCOUNT_CHEQUEAMOUNT,m_ui->cheque);
    hash.insert(AccountDB::Constants::ACCOUNT_VISAAMOUNT,m_ui->visa);
    hash.insert(AccountDB::Constants::ACCOUNT_DUEAMOUNT,m_ui->du);
    hash.insert(AccountDB::Constants::ACCOUNT_OTHERAMOUNT,m_ui->other);
    hash.insert(AccountDB::Constants::ACCOUNT_INSURANCEAMOUNT,m_ui->insurance);
    QAbstractItemModel *model = m_ui->tableView->model();
    double totalSum = 0.00;
    for (int col = AccountDB::Constants::ACCOUNT_CASHAMOUNT; col < AccountDB::Constants::ACCOUNT_DUEAMOUNT+1; col += 1)
    {
        double sum = 0.00;
        for (int row = 0; row < model->rowCount(); row += 1)
        {
            double tot = model->data(model->index(row,col),Qt::DisplayRole).toDouble();
            if (WarnDebugMessage)
                        qDebug() << __FILE__ << QString::number(__LINE__) << " tot =" << QString::number(tot) ;
            sum += tot;
            }
            totalSum += sum;
            QString textSum = QString::number(sum);
            if (WarnDebugMessage)
                        qDebug() << __FILE__ << QString::number(__LINE__) << " textSum =" << textSum;
            hash.value(col)->setText(textSum);
        }
     m_ui->sumEdit->setText(QString::number(totalSum));
}

void AccountView::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    if (e->type()== QEvent::LanguageChange)
    {
          m_ui->retranslateUi(this);
          setHeadersOfTable();
    }
}

void AccountView::quitFreeAccount()
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
