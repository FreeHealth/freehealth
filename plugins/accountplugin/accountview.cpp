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
#include "accountview.h"
#include "ui_accountview.h"
#include <utils/log.h>

#include <coreplugin/icore.h>

#include <accountbaseplugin/constants.h>

#include <QTableView>
#include <QDebug>

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
using namespace AccountDB;
using namespace Account;

AccountView::AccountView(QWidget *parent) :
        QWidget(parent), m_ui(new Ui::AccountViewWidget)//, d(new Internal::AccountViewPrivate(this))
{
    setObjectName("AccountView");
    m_ui->setupUi(this);

    qDebug() << __FILE__ << QString::number(__LINE__) << " 1 " ;
    m_Model = new AccountDB::AccountModel(this);
    //m_Model->setFilter("");
    m_Model->select();
    m_ui->tableView->setModel(m_Model);
    m_ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    m_ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    m_ui->tableView->show();
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(m_Model->rowCount()) ;
    qDebug() << __FILE__ << QString::number(__LINE__) << " 2 " ;
    m_userUuid = m_Model->getUserUuid();
    qDebug() << __FILE__ << QString::number(__LINE__) << " m_userUuid =" << m_userUuid ;
    //QString filter = QString("%1 = '%2'").arg("USER_UID",m_userUuid);
    //m_Model->setFilter(filter);
    m_ui->startDate->setDate(QDate::currentDate());
    m_ui->endDate->setDate(QDate::currentDate()); 
    //refresh();

}

AccountView::~AccountView()
{
}

void AccountView::refresh(){
    QString dateBeginStr = m_ui->startDate->date().toString("yyyy-MM-dd");
    QString dateEndStr = m_ui->endDate->date().toString("yyyy-MM-dd");
    QString filter = QString("%1 = '%2'").arg("USER_UID",m_userUuid);
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(dateBeginStr,dateEndStr);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    m_Model->setFilter(filter);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_Model->filter() ;
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
    qDebug() << __FILE__ << QString::number(__LINE__) << " in filterChanged " ;
    // Calculate sums of paiements
    /*m_ui->cash->setText(QString::number(m_Model->sum(AccountDB::Constants::ACCOUNT_CASHAMOUNT)));
    m_ui->cheque->setText(QString::number(m_Model->sum(AccountDB::Constants::ACCOUNT_CHEQUEAMOUNT)));
    m_ui->du->setText(QString::number(m_Model->sum(AccountDB::Constants::ACCOUNT_DUEAMOUNT)));
    m_ui->insurance->setText(QString::number(m_Model->sum(AccountDB::Constants::ACCOUNT_INSURANCEAMOUNT)));
    m_ui->visa->setText(QString::number(m_Model->sum(AccountDB::Constants::ACCOUNT_VISAAMOUNT)));*/
    refresh();
    calc();
}

void AccountView::on_startDate_dateChanged(const QDate &date)
{
    //refresh();
    //m_Model->setStartDate(date);
}

void AccountView::on_endDate_dateChanged(const QDate &date)
{
    //refresh();
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

void AccountView::calc(){
    qDebug() << __FILE__ << QString::number(__LINE__) << " in calc " ;
    QHash<int,QLineEdit*> hash;
    hash.insert(AccountDB::Constants::ACCOUNT_CASHAMOUNT,m_ui->cash);
    hash.insert(AccountDB::Constants::ACCOUNT_CHEQUEAMOUNT,m_ui->cheque);
    hash.insert(AccountDB::Constants::ACCOUNT_VISAAMOUNT,m_ui->visa);
    hash.insert(AccountDB::Constants::ACCOUNT_DUEAMOUNT,m_ui->du);
    hash.insert(AccountDB::Constants::ACCOUNT_OTHERAMOUNT,m_ui->other);
    hash.insert(AccountDB::Constants::ACCOUNT_INSURANCEAMOUNT,m_ui->insurance);
    QAbstractItemModel *model = m_ui->tableView->model();
    qDebug() << __FILE__ << QString::number(__LINE__) << " 1 " ;
    double sum = 0.00;
    qDebug() << __FILE__ << QString::number(__LINE__) << " 2 " ;
    for (int col = AccountDB::Constants::ACCOUNT_CASHAMOUNT; col < AccountDB::Constants::ACCOUNT_DUEAMOUNT+1; col += 1)
    {
    	for (int row = 0; row < model->rowCount(); row += 1)
    	{
    		double tot = model->data(model->index(row,col),Qt::DisplayRole).toDouble();
    		
    		qDebug() << __FILE__ << QString::number(__LINE__) << " tot =" << QString::number(tot) ;
    		sum += tot;
    		
    	    }
    	    QString textSum = QString::number(sum);
    	    qDebug() << __FILE__ << QString::number(__LINE__) << " textSum =" << textSum;
    	    hash.value(col)->setText(textSum);
        }
     qDebug() << __FILE__ << QString::number(__LINE__) << " in calc end " ;
}


