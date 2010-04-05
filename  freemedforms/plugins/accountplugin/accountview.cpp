/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
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
            << AccountDB::Constants::ACCOUNT_ISVALID;
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

    // Calculate sums of paiements
    d->m_ui->cash->setText(QString::number(d->m_Model->sum(AccountDB::Constants::ACCOUNT_CASHAMOUNT)));
    d->m_ui->cheque->setText(QString::number(d->m_Model->sum(AccountDB::Constants::ACCOUNT_CHEQUEAMOUNT)));
    d->m_ui->du->setText(QString::number(d->m_Model->sum(AccountDB::Constants::ACCOUNT_DUEAMOUNT)));
    d->m_ui->insurance->setText(QString::number(d->m_Model->sum(AccountDB::Constants::ACCOUNT_INSURANCEAMOUNT)));
    d->m_ui->visa->setText(QString::number(d->m_Model->sum(AccountDB::Constants::ACCOUNT_VISAAMOUNT)));
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
