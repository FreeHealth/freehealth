/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "formalizeditemmodel.h"

using namespace DrugsDB;

AvailableFormalizedItemNameModel::AvailableFormalizedItemNameModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_ItemNames.append("bleeding");
    m_ItemNamesTooltip.append(tr("Risk for bleeding (1 to 3, 3 is the highest risk)."));

    m_ItemNames.append("mainDailyDoseFrom");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("mainDailyDoseTo");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("mainWeeklyDoseFrom");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("mainWeeklyDoseTo");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("interactorDailyDoseFrom");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("interactorDailyDoseTo");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("mainADEPotentialisation");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("mainWeeklyDoseFrom");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("interactorDailyDoseTo");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("renalFailure");
    m_ItemNamesTooltip.append(tr("Risk of acute renal failure (1 to 3, 3 is the highest risk)"));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

    m_ItemNames.append("");
    m_ItemNamesTooltip.append(tr(""));

}

AvailableFormalizedItemNameModel::~AvailableFormalizedItemNameModel()
{}

QVariant AvailableFormalizedItemNameModel::data(const QModelIndex &/*index*/, int /*role*/) const
{
    return QVariant();
}


//FormalizedItemModel::FormalizedItemModel(QObject *parent) :
//    QAbstractTableModel(parent)
//{
//}
