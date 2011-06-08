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

#include <utils/log.h>

#include <coreplugin/icore.h>

#include <accountbaseplugin/accountmodel.h>
#include <accountbaseplugin/constants.h>

#include <QTableView>
#include <QDebug>

#include "ui_accountview.h"

using namespace Account;

namespace Account {
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
}


AccountView::AccountView(QWidget *parent) :
        QWidget(parent), d(new Internal::AccountViewPrivate(this))
{
    setObjectName("AccountView");
    d->m_ui->setupUi(this);

    d->m_Model = new AccountDB::AccountModel(this);
    d->m_ui->tableView->setModel(d->m_Model);

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
        d->m_ui->tableView->hideColumn(i);
    }

    d->m_ui->startDate->setDate(QDate::currentDate());
    d->m_ui->endDate->setDate(QDate::currentDate());
}

AccountView::~AccountView()
{
    if (d) {
        delete d;
        d=0;
    }
}

void AccountView::filterChanged()
{
    // Filter model
    d->m_Model->setStartDate(d->m_ui->startDate->date());
    d->m_Model->setEndDate(d->m_ui->endDate->date());
    qDebug() << __FILE__ << QString::number(__LINE__) << " in filterChanged " ;
    // Calculate sums of paiements
    /*d->m_ui->cash->setText(QString::number(d->m_Model->sum(AccountDB::Constants::ACCOUNT_CASHAMOUNT)));
    d->m_ui->cheque->setText(QString::number(d->m_Model->sum(AccountDB::Constants::ACCOUNT_CHEQUEAMOUNT)));
    d->m_ui->du->setText(QString::number(d->m_Model->sum(AccountDB::Constants::ACCOUNT_DUEAMOUNT)));
    d->m_ui->insurance->setText(QString::number(d->m_Model->sum(AccountDB::Constants::ACCOUNT_INSURANCEAMOUNT)));
    d->m_ui->visa->setText(QString::number(d->m_Model->sum(AccountDB::Constants::ACCOUNT_VISAAMOUNT)));*/
    
    calc();
}

void AccountView::on_startDate_dateChanged(const QDate &date)
{
    d->m_Model->setStartDate(date);
}

void AccountView::on_endDate_dateChanged(const QDate &date)
{
    d->m_Model->setEndDate(date);
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
    d->m_ui->startDate->setDate(start);
    d->m_ui->endDate->setDate(end);
    blockSignals(false);

    filterChanged();
}

void AccountView::calc(){
    qDebug() << __FILE__ << QString::number(__LINE__) << " in calc " ;
    QHash<int,QLineEdit*> hash;
    hash.insert(AccountDB::Constants::ACCOUNT_CASHAMOUNT,d->m_ui->cash);
    hash.insert(AccountDB::Constants::ACCOUNT_CHEQUEAMOUNT,d->m_ui->cheque);
    hash.insert(AccountDB::Constants::ACCOUNT_VISAAMOUNT,d->m_ui->visa);
    hash.insert(AccountDB::Constants::ACCOUNT_DUEAMOUNT,d->m_ui->du);
    hash.insert(AccountDB::Constants::ACCOUNT_OTHERAMOUNT,d->m_ui->other);
    hash.insert(AccountDB::Constants::ACCOUNT_INSURANCEAMOUNT,d->m_ui->insurance);
    QAbstractItemModel *model = d->m_ui->tableView->model();
    qDebug() << __FILE__ << QString::number(__LINE__) << " 1 " ;
    double sum = 0.00;
    qDebug() << __FILE__ << QString::number(__LINE__) << " 2 " ;
    for (int col = AccountDB::Constants::ACCOUNT_CASHAMOUNT; col < AccountDB::Constants::ACCOUNT_DUEAMOUNT+1; col += 1)
    {
    	for (int row = 0; row < model->rowCount(); row += 1)
    	{
    		sum += model->data(model->index(row,col),Qt::DisplayRole).toDouble();
    		
    	    }
    	    QString textSum = QString::number(sum);
    	    qDebug() << __FILE__ << QString::number(__LINE__) << " textSum =" << textSum;
    	    hash.value(col)->setText(textSum);
        }
     qDebug() << __FILE__ << QString::number(__LINE__) << " in calc end " ;
}
