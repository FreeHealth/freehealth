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
#ifndef CONTROLRECEIPTSIO_H
#define CONTROLRECEIPTSIO_H

#include <accountbaseplugin/accountmodel.h>
#include <accountbaseplugin/constants.h>
#include <accountplugin/account_exporter.h>

#include <QWidget>
#include <QPoint>
#include <QRect>
#include <QMap>

namespace Ui{
    class ControlReceiptsWidget;
}

using namespace AccountDB;
class ACCOUNT_EXPORT ControlReceipts : public QWidget
{
    Q_OBJECT
    enum HeadersForPrint
    {
        PATIENT_NAME_HEADER = 0,
        DATE_HEADER,
        ACTS_HEADER,
        CASH_HEADER,
        CHQ_HEADER,
        VISA_HEADER,
        BANKING_HEADER,
        OTHER_HEADER,
        DUE_HEADER,
        DUE_BY_HEADER,
        HeadersForPrint_MaxParam
        };
    public :
        ControlReceipts(QWidget * parent);
        ~ControlReceipts();
        void resizeControlReceipts(QWidget * parent);
    signals :
        void isClosing();
    private slots :
        void search();
        void deleteLine();
        void printDues();
        void print();
        void closeAction();
    private:
        void coloringDoubles();
        void refresh();
        void refreshFilter(const QString & filter);
        QString textOfSums(AccountModel * model);
        void changeEvent(QEvent *e);
        void print(QString & html);
        QString getHtmlDocAccordingToTableView();
        QStringList getListOfSums();
        Ui::ControlReceiptsWidget * ui;
        AccountModel * m_accountModel;
        QString m_userUuid;
        QMap<QString,QString> m_mapCombo;
        QString m_typeOfMoney;
};

#endif

