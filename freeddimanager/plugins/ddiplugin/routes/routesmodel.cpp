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
#include "routesmodel.h"
#include <ddiplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QSqlDatabase>
#include <QSqlTableModel>

using namespace DDI;
using namespace Internal;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

namespace {
const int ALL_IV_ID = 100001;
const int ALL_ORAL_ID = 100002;

QString routeCsvAbsoluteFile()
{
    return QString(settings()->path(Core::ISettings::BundleResourcesPath) + Constants::ROUTES_CSV_FILENAME);
}
}

bool Internal::Route::operator<(const Internal::Route &second) const
{
    QString l = QLocale().name().left(2).toLower();
    return this->trLabels.value(l) < second.trLabels.value(l);
}

RoutesModel::RoutesModel(QObject *parent) :
    QAbstractTableModel(parent),
    _initialized(false)
{
    // Read file
    QString content = Utils::readTextFile(routeCsvAbsoluteFile());
    if (content.isEmpty()) {
        LOG_ERROR("Routes file does not exist.\n   " + routeCsvAbsoluteFile());
        return;
    }

    foreach(const QString &line, content.split("\n", QString::SkipEmptyParts)) {
        if (line.startsWith("--"))
            continue;
        Internal::Route route;
        route.isHelper = false;
        // Parse line
        int id = 0;
        foreach(QString value, line.split(",")) {
            value = value.trimmed();
            if (id==0) {
                route.id = value.toInt();
                ++id;
                continue;
            }
            ++id;
            value = value.remove("\"");
            int sep = value.indexOf(":");
            QString lang = value.left(sep);
            if (lang.compare("systemic") != 0) {
                route.trLabels.insertMulti(lang, value.mid(sep + 1));
            }
        }
        route.checkState = Qt::Unchecked;
        m_Routes.append(route);
    }
    qSort(m_Routes);
    // add helpers
    Internal::Route allIv;
    allIv.id = ALL_IV_ID;
    allIv.isHelper = true;
    allIv.trLabels.insert("fr", "Toutes les voies intra-veineuses");
    allIv.trLabels.insert("en", "All intraveinous routes");
    allIv.trLabels.insert("de", "All intraveinous routes");
    allIv.checkState = Qt::Unchecked;
    m_Routes.prepend(allIv);
    Internal::Route allOral;
    allOral.id = ALL_ORAL_ID;
    allOral.isHelper = true;
    allOral.trLabels.insert("fr", "Toutes les voies orales");
    allOral.trLabels.insert("en", "All oral routes");
    allOral.trLabels.insert("de", "All oral routes");
    allOral.checkState = Qt::Unchecked;
    m_Routes.prepend(allOral);

}

RoutesModel::~RoutesModel()
{}

bool RoutesModel::initialize()
{
    if (_initialized)
        return true;
    // TODO: read database content
    _initialized = true;
    return true;
}

void RoutesModel::clear()
{
    setCheckedRouteIds(QList<int>());
}

int RoutesModel::rowCount(const QModelIndex &) const
{
    return m_Routes.count();
}

QVariant RoutesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Internal::Route &route = m_Routes.at(index.row());
    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        switch (index.column()) {
        case Id: return route.id; break;
        case FirstTranslatedName: return route.trLabels.value(QLocale().name().left(2).toLower()); break;
//        case AllTranslatedNames: return route.trLabels.values(QLocale().name().left(2).toLower()); break;
        }
    } else if (role==Qt::CheckStateRole) {
        // Store the (un)checked ids m_CheckedIds
        return route.checkState;
    } else if (role==Qt::FontRole) {
        if (route.isHelper) {
            QFont bold;
            bold.setBold(true);
            return bold;
        }
    }
    return QVariant();
}

bool RoutesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role==Qt::CheckStateRole) {
        Internal::Route &route = m_Routes[index.row()];
        route.checkState = Qt::CheckState(value.toInt());
        if (route.isHelper) {
            QList<int> ids;
            switch (route.id) {
            case ALL_IV_ID:
            {
                // ids = 19, 42, 62, 66, 72
                ids << 19 << 42 << 62 << 66 << 72;
                break;
            }
            case ALL_ORAL_ID:
            {
                // ids = 2, 48, 49, 57
                ids << 2 << 48 << 49 << 57;
                break;
            }
            }  // End switch
            for(int i=0; i < ids.count(); ++i) {
                // find the route according to its id
                int findId = ids.at(i);
                for(int j=0; j < m_Routes.count();++j) {
                    Internal::Route &sec_route = m_Routes[index.row()];
                    if (sec_route.id==findId) {
                        sec_route.checkState = Qt::CheckState(value.toInt());
                        break;
                    }
                }
            }
            Q_EMIT dataChanged(this->index(0, FirstTranslatedName), this->index(rowCount(), FirstTranslatedName));
            return true;
        }
        Q_EMIT dataChanged(index, index);
    }

    return true;
}

Qt::ItemFlags RoutesModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

QList<int> RoutesModel::checkedRouteIds() const
{
    QList<int> ids;
    for(int i=0; i<m_Routes.count();++i) {
        const Internal::Route &route = m_Routes.at(i);
        if (route.isHelper)
            continue;
        if (route.checkState==Qt::Checked)
            ids << route.id;
    }
    return ids;
}

QList<QVariant> RoutesModel::checkedRouteIdsInVariant() const
{
    QList<QVariant> l;
    QList<int> c = checkedRouteIds();
    for(int i=0; i < c.count(); ++i) {
        l << c.at(i);
    }
    return l;
}

void RoutesModel::setCheckedRouteIds(const QList<int> &ids)
{
    for(int i=0; i<m_Routes.count();++i) {
        Internal::Route &route = m_Routes[i];
        if (route.isHelper)
            continue;
        if (ids.contains(route.id))
            route.checkState=Qt::Checked;
        else
            route.checkState=Qt::Unchecked;
    }
    reset();
}

void RoutesModel::setCheckedRouteIds(const QList<QVariant> &ids)
{
    QList<int> _ids;
    for(int i=0; i<ids.count();++i) {
        _ids << ids.at(i).toInt();
    }
    setCheckedRouteIds(_ids);
}

/** Return all route database Id for all the labels \e routeName */
QList<int> RoutesModel::routesId(const QStringList &routeName) const
{
    QList<int> ids;
    foreach(const QString &rname, routeName) {
        foreach(const Internal::Route &route, m_Routes) {
            if (route.trLabels.values().contains(rname))
                ids << route.id;
        }
    }
    return ids;
}

QDebug operator<<(QDebug debug, const DDI::Internal::Route &route)
{
    debug.nospace() << "Route(" << route.id << "," << route.trLabels << ")";
    return debug.nospace();
}
