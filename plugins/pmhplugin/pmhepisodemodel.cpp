/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pmhepisodemodel.h"
#include "pmhdata.h"
#include "pmhcore.h"

#include <utils/log.h>

#include <QVector>

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline PmhCore *pmhCore() {return PmhCore::instance();}

namespace PMH {
namespace Internal {

class PmhEpisodeModelPrivate
{
public:
    PmhEpisodeModelPrivate(PmhEpisodeModel *parent) :
            m_Pmh(0),
            q(parent)
    {
    }

    ~PmhEpisodeModelPrivate()
    {
        // Don't delete PmhData
    }


public:
    PmhData *m_Pmh;

private:
    PmhEpisodeModel *q;

};

}  // End namespace Internal
}  // End namespace PMH


PmhEpisodeModel::PmhEpisodeModel(Internal::PmhData *pmh, QObject *parent) :
    QAbstractListModel(parent), d(new PmhEpisodeModelPrivate(this))
{
    Q_ASSERT(pmh);
    d->m_Pmh = pmh;
}

PmhEpisodeModel::~PmhEpisodeModel()
{
    if (d)
        delete d;
    d = 0;
}

int PmhEpisodeModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    if (d->m_Pmh)
        return d->m_Pmh->episodes().count();
    return 0;
}

int PmhEpisodeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant PmhEpisodeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.column() == EmptyColumn)
        return QVariant();

    if (index.row() >= d->m_Pmh->episodes().count())
        return QVariant();

    PmhEpisodeData *ep = d->m_Pmh->episodes().at(index.row());

    switch (role)
    {
    case Qt::EditRole :
        {
            switch (index.column())
            {
            case DateStart: return ep->data(PmhEpisodeData::DateStart);
            case DateEnd: return ep->data(PmhEpisodeData::DateEnd);
            case Label: return ep->data(PmhEpisodeData::Label);
            case IcdCodeList: return ep->data(PmhEpisodeData::IcdCodeList);
            case IcdLabelStringList : return ep->data(PmhEpisodeData::IcdLabelStringList);
            case IcdXml: return ep->data(PmhEpisodeData::IcdXml);
            }
            break;
        }
    case Qt::DisplayRole :
        {
            switch (index.column())
            {
            case DateStart: return ep->data(PmhEpisodeData::DateStart).toDate();
            case DateEnd: return ep->data(PmhEpisodeData::DateEnd).toDate();
            case Label: return ep->data(PmhEpisodeData::Label);
            case IcdCodeList: return ep->data(PmhEpisodeData::IcdCodeList);
            case IcdLabelStringList : return ep->data(PmhEpisodeData::IcdLabelStringList);
            default : return QVariant();
            }
            break;
        }
    case Qt::ToolTipRole :
        {
            return ep->data(PmhEpisodeData::IcdLabelHtmlList);
        }
    }

//    case Qt::ForegroundRole :
//        {
//            if (index.column()!=Label)
//                return QVariant();
//            if (it->isCategory()) {
//                return QColor(settings()->value(Constants::S_FOREGROUND_CATEGORIES, "darkblue").toString());
//            } else {
//                return QColor(settings()->value(Constants::S_FOREGROUND_TEMPLATES, "#000").toString());
//            }
//        }
//    case Qt::BackgroundRole :
//        {
//            if (index.column()!=Label)
//                return QVariant();
//            QColor c;
//            if (it->isCategory()) {
//                c = QColor(settings()->value(Constants::S_BACKGROUND_CATEGORIES, "white").toString());
//            } else {
//                c = QColor(settings()->value(Constants::S_BACKGROUND_TEMPLATES, "white").toString());
//            }
//            if (c.name()=="#ffffff")
//                return QVariant();
//            c.setAlpha(125);
//            return c;
//        }
//    }

    return QVariant();
}

bool PmhEpisodeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (index.column() == EmptyColumn)
        return false;

    if (index.row() >= d->m_Pmh->episodes().count())
        return false;

    PmhEpisodeData *ep = d->m_Pmh->episodes().at(index.row());

    switch (role)
    {
    case Qt::EditRole :
    case Qt::DisplayRole :
        {
            bool ok = false;
            switch (index.column())
            {
            case DateStart: ok = ep->setData(PmhEpisodeData::DateStart, value); break;
            case DateEnd: ok = ep->setData(PmhEpisodeData::DateEnd, value); break;
            case Label: ok = ep->setData(PmhEpisodeData::Label, value); break;
            case IcdXml: ok = ep->setData(PmhEpisodeData::IcdXml, value); break;
            }
            if (ok)
                Q_EMIT dataChanged(this->index(index.row(), 0), this->index(index.row(), ColumnCount));
            break;
        }
    }
    return true;
}

bool PmhEpisodeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row+count-1);
    for(int i = 0; i < count; ++i) {
        d->m_Pmh->insertEpisode(row+i, new Internal::PmhEpisodeData);
    }
    endInsertRows();
    return true;
}

bool PmhEpisodeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count-1);
    // Get episodes to remove
    QVector<Internal::PmhEpisodeData *> toRemove;
    int nbEpisodes = d->m_Pmh->episodes().count();
    for(int i = 0; i < count; ++i) {
        int id = row+i;
        if (id < nbEpisodes) {
            Internal::PmhEpisodeData *data = d->m_Pmh->episodes().at(row+i);
            if (!toRemove.contains(data))
                toRemove << data;
        } else {
            LOG_ERROR("Unable to remove data, out of range.");
        }
    }

    // Remove episodes from PmhData
    for(int i = 0; i < toRemove.count(); ++i) {
        d->m_Pmh->removeEpisode(toRemove.at(i));
    }

    endRemoveRows();
    return true;
}

Qt::ItemFlags PmhEpisodeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    if (index.column()==IcdXml || index.column()==IcdCodeList) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant PmhEpisodeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
        return QVariant();
    if (role == Qt::DisplayRole) {
        switch (section) {
        case DateStart: return tr("Starting date");
        case DateEnd: return tr("Ending date");
        case Label: return tr("Label");
        case IcdCodeList: return tr("ICD10 coding");
        }
    }
    return QVariant();
}
