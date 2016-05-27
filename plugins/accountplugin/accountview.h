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
#ifndef ACCOUNTVIEW_H
#define ACCOUNTVIEW_H

#include <accountplugin/account_exporter.h>
#include <accountbaseplugin/accountmodel.h>
#include <QObject>
#include <QWidget>
#include <QDate>

/**
 * \file accountview.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.1.0
 * \date 13 Apr 2011
*/
namespace Ui{
    class AccountViewWidget;
}

using namespace AccountDB;
namespace Account {
namespace Internal {
class AccountViewPrivate;
}



class ACCOUNT_EXPORT AccountView : public QWidget
{
    Q_OBJECT
public:
    AccountView(QWidget *parent = 0);
    ~AccountView();

private:
    void filterChanged();
    void refresh();
    void calc();
    void setHeadersOfTable();
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void on_startDate_dateChanged(const QDate &date);
    void on_endDate_dateChanged(const QDate &date);
    void on_periodCombo_currentIndexChanged(int index);
    void deleteLine();
    void quitFreeAccount();

private:
    //Internal::AccountViewPrivate *d;
    AccountModel *m_Model;
    Ui::AccountViewWidget *m_ui ;
    QString m_userUuid;
};


}  // End namespace Account

#endif    // End ACCOUNT_PLUGIN_H
