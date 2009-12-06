/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
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

DailySchemeModel::DailySchemeModel(QObject * parent, const QString &serializedContent) :
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

void DailySchemeModel::setMethod(Method method)
{
    if (method==d->m_Method)
        return;
    if (method==Distribute) {
        d->m_DailySchemes.clear();
    }
    d->m_Method = method;
    d->dailySum();
    Q_EMIT methodChanged();
    reset();
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
            tmp += "<" + schemes.at(k) + "=" + QString::number(d->m_DailySchemes.value(k)) + ">";
    }
    if (d->m_Method == Repeat) {
        tmp.prepend("<Repeat>");
    } else {
        tmp.prepend("<Distribute>");
    }
    return tmp;
}

void DailySchemeModel::setSerializedContent(const QString &content)
{
    d->m_DailySchemes.clear();
    const QStringList &schemes = Trans::ConstantTranslations::dailySchemeXmlTagList();
    QStringList xml = content.split(">");
    foreach(const QString &line, xml) {
        QStringList x = line.split("=");
        if (x.count() != 2)
            continue;
        d->m_DailySchemes.insert(schemes.indexOf(x[0].remove("<")), x.at(1).toDouble());
    }
    if (content.startsWith("<Repeat>")) {
        setMethod(Repeat);
    } else {
        setMethod(Distribute);
    }
    reset();
}

void DailySchemeModel::setScored(bool isScored)
{
    d->m_IsScored = isScored;
}

/** \brief Define the total maximum and minimum quantity for the full day. */
void DailySchemeModel::setMaximumDay(double max)
{
    d->m_Max = max;
    d->dailySum();
    reset();
    /** \todo recalculate daily scheme and manage errors ? */
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
            toReturn +=  schemes.at(k) + " : " + QString::number(d->m_DailySchemes.value(k)) + "; ";
    }
    toReturn.chop(2);
    return toReturn;
}
