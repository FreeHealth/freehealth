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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "dailyschememodel.h"

#include <QStringList>
#include <QColor>
#include <QDebug>

namespace {
    struct DailySchemeStruct {
    };
}

namespace DrugsDB {
namespace Internal {

class DailyScheme {
public:
    DailyScheme();

    /** \brief Use Trans::DailyScheme enum for \e dayRef */
    void setValue(const int dayRef, const double value)
    {
        m_DayRef_Value.insert(dayRef,value);
    }

    double value(const int dayRef) const
    {
        return m_DayRef_Value.value(dayRef, 0);
    }

private:
    QHash<int, double> m_DayRef_Value;
};


class DailySchemeModelPrivate
{
public:
    DailySchemeModelPrivate() : m_IsScored(false), m_Min(0), m_Max(1), m_HasError(false) {}

    double dailySum()
    {
        if (m_Method==DrugsDB::DailySchemeModel::Repeat) {
            m_HasError = false;
            return 0;
        }
        double toReturn = 0.0;
        foreach(int k, m_DailySchemes.keys()) {
            toReturn += m_DailySchemes.value(k);
        }
        if (toReturn>m_Max)
            m_HasError = true;
        else
            m_HasError = false;
        return toReturn;
    }

    QHash<int, double> m_DailySchemes;
    bool m_IsScored;
    double m_Min, m_Max;
    bool m_HasError;
    DrugsDB::DailySchemeModel::Method m_Method;
};

} // end namespace Internal
} // end namespace DrugsDB


using namespace DrugsDB;

DailySchemeModel::DailySchemeModel(QObject * parent, const QString &) :
        QAbstractTableModel(parent), d(0)
{
    d = new Internal::DailySchemeModelPrivate();
    d->m_Method = Distribute;
}

DailySchemeModel::~DailySchemeModel()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void DailySchemeModel::clear()
{
    d->m_DailySchemes.clear();
}

void DailySchemeModel::setMethod(Method method)
{
    if (method==d->m_Method)
        return;
    beginResetModel();
    if (method==Distribute) {
        d->m_DailySchemes.clear();
    }
    d->m_Method = method;
    d->dailySum();
    Q_EMIT methodChanged();
    endResetModel();
}

DailySchemeModel::Method DailySchemeModel::method() const
{
    return d->m_Method;
}

QString DailySchemeModel::serializedContent() const
{
    QString tmp;
    const QStringList &schemes = Trans::ConstantTranslations::dailySchemeXmlTagList();
    foreach(int k, d->m_DailySchemes.keys()) {
        if (d->m_DailySchemes.value(k))
            tmp += "<" + schemes.at(k) + " value=" + QString::number(d->m_DailySchemes.value(k)) + "/>";
    }
    if (d->m_Method == Repeat) {
        tmp.prepend("<Repeat/>");
    } else {
        tmp.prepend("<Distribute/>");
    }
    return tmp;
}

void DailySchemeModel::setSerializedContent(const QString &content)
{
    beginResetModel();
    d->m_DailySchemes.clear();
    const QStringList &schemes = Trans::ConstantTranslations::dailySchemeXmlTagList();
    QString tmp = content;
    tmp.remove("<Distribute/>");
    tmp.remove("<Repeat/>");
    QStringList xml = tmp.split("/>");
    foreach(const QString &line, xml) {
        QStringList x = line.split(" value=");
        if (x.count() != 2)
            continue;
        d->m_DailySchemes.insert(schemes.indexOf(x[0].remove("<")), x.at(1).toDouble());
    }
    if (content.startsWith("<Repeat/>")) {
        setMethod(Repeat);
    } else {
        setMethod(Distribute);
    }
    endResetModel();
}

void DailySchemeModel::setScored(bool isScored)
{
    d->m_IsScored = isScored;
}

/** \brief Define the total maximum and minimum quantity for the full day. */
void DailySchemeModel::setMaximumDay(double max)
{
    beginResetModel();
    d->m_Max = max;
    d->dailySum();
    endResetModel();
    // TODO: recalculate daily scheme and manage errors ?
}

double DailySchemeModel::sum() const
{
    return d->dailySum();
}

Qt::ItemFlags DailySchemeModel::flags(const QModelIndex &index) const
{
    if (d->m_Method == Repeat) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsTristate | Qt::ItemIsUserCheckable;
    } else if (index.column()==Value) {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    } else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

bool DailySchemeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role==Qt::CheckStateRole) {
        d->m_DailySchemes[index.row()] = value.toDouble();
        Q_EMIT dataChanged(index,index);
        return true;
    } else if ((role==Qt::EditRole) || (role==Qt::DisplayRole)) {
        if (index.column()==Value) {
            if (d->m_HasError) {
                d->m_DailySchemes.clear();
                Q_EMIT dataChanged(index,index);
            }
            if (d->dailySum() + value.toDouble() <= d->m_Max) {
                d->m_DailySchemes[index.row()] = value.toDouble();
                Q_EMIT dataChanged(index,index);
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

QVariant DailySchemeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ((role==Qt::CheckStateRole) && (d->m_Method==Repeat)) {
        return d->m_DailySchemes.value(index.row(), 0)==0 ? Qt::Unchecked : Qt::Checked ;
    } else if ((role==Qt::EditRole) || (role==Qt::DisplayRole)) {
        if (index.column()==Value)
            return d->m_DailySchemes.value(index.row(), 0);
        else if (index.column()==DayReference)
            return Trans::ConstantTranslations::dailyScheme(index.row());
    } else if (role==Qt::BackgroundRole) {
        if (d->m_HasError)
            return QColor("#ffdddd");
    }
    return QVariant();
}

QString DailySchemeModel::humanReadableRepeatedDailyScheme() const
{
    if (d->m_Method!=Repeat)
        return QString();
    QString toReturn;
    const QStringList &schemes = Trans::ConstantTranslations::dailySchemeList();
    foreach(int k, d->m_DailySchemes.keys()) {
        if (d->m_DailySchemes.value(k))
            toReturn +=  schemes.at(k) + ", ";
    }
    toReturn.chop(2);
    return toReturn;
}

QString DailySchemeModel::humanReadableDistributedDailyScheme() const
{
    if (d->m_Method!=Distribute)
        return QString();
    QString toReturn;
    const QStringList &schemes = Trans::ConstantTranslations::dailySchemeList();
    foreach(int k, d->m_DailySchemes.keys()) {
        if (d->m_DailySchemes.value(k))
            toReturn +=  schemes.at(k) + ": " + QString::number(d->m_DailySchemes.value(k)) + "; ";
    }
    toReturn.chop(2);
    return toReturn;
}
