/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "ebmmodel.h"
#include "ebmdata.h"

#include <QDebug>

using namespace MedicalUtils;

EbmModel::EbmModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

EbmModel::~EbmModel()
{
}

void EbmModel::clear()
{
    beginResetModel();
    m_Ebms.clear();
    endResetModel();
}

QVariant EbmModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_Ebms.count())
        return QVariant();


    if (role==Qt::DisplayRole) {

        EbmData *e = m_Ebms.at(index.row());
        if (!e) // should never the case
            return QVariant();

        switch (index.column()) {
        case Link: return e->link();
        case References: if (e->references().isEmpty()) return e->data(EbmData::ShortReferences); return e->references();
        case Abstract: if (e->abstract().isEmpty()) return e->data(EbmData::AbstractPlainText); return e->abstract();
        case ShortReferences: return e->data(EbmData::ShortReferences);
        case AbstractWithReferences:
            return QString("%1\n\n%2")
                    .arg(e->data(EbmData::CompleteReferences))
                    .arg(e->abstract());
        case AbstractWithReferencesHtml:
            return QString("<b>%1</b><br/><br/>%2")
                    .arg(e->data(EbmData::CompleteReferences))
                    .arg(e->abstract());
        }

    }
    return QVariant();
}

Qt::ItemFlags EbmModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void EbmModel::setEbmData(const QVector<EbmData *> &ebms)
{
    beginResetModel();
    m_Ebms = ebms;
    endResetModel();
}

