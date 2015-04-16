/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef PREFERREDRECEIPTS_H
#define PREFERREDRECEIPTS_H

#include <accountbaseplugin/accountmodel.h>
#include <accountbaseplugin/constants.h>
#include <accountplugin/account_exporter.h>

#include <QWidget>
#include <QHash>

namespace Ui{
    class PreferredReceiptsWidget;
}

class ACCOUNT_EXPORT PreferredReceipts: public QWidget {
    Q_OBJECT

public:
    PreferredReceipts(QWidget * parent);
    ~PreferredReceipts();

private:
    void insertpreferredValuesIntoAccount();
    Ui::PreferredReceiptsWidget *ui;
    int m_typeOfChoice;
    double m_percent;
    QHash<int,double> m_choiceAndPercentagesHash;
    QHash<int,QVariant> m_choiceAndDebtorHash;
    QString m_preferredAct;

private Q_SLOTS:
    void showChoiceDialog();
};

#endif  // PREFERREDRECEIPTS_H

