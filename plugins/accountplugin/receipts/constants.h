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
#ifndef ACCOUNTRECEIPTS_CONTANTS_H
#define ACCOUNTRECEIPTS_CONTANTS_H

#include <QObject>
#include <QHash>

namespace ReceiptsConstants{
    enum TypeOfPayment{
        Cash = 0,
        Check,
        Visa,
        Banking,
        Other,
        Due,
        MaxOfTypeOfPayments
    };
    enum HeaderData{
        HDCash = 0,
        HDCheck,
        HDVisa,
        HDBanking,
        HDOther,
        HDDue,
        HDDebtor,
        HDSite,
        HDDistRules,
        HDAct,
        MaxOfHeaderData
    };
    enum ItemsOfTreeView
    {
        THESAURUS_ITEM = 0,
        ALL_VALUES_ITEM,
        PREFERED_VALUE_ITEM,
        ROUND_TRIP_ITEM,
        FREE_VALUE_ITEM,
        DEBTOR_ITEM,
        SITES_ITEM,
        DISTANCE_RULES_ITEM,
        BANK_ITEM,
        RULES_ITEM,
        ItemsOfTreeView_MaxParam
    };
    
}

#endif
