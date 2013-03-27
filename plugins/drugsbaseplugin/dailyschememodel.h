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
#ifndef DAILYSCHEMEMODEL_H
#define DAILYSCHEMEMODEL_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <translationutils/constanttranslations.h>

#include <QString>
#include <QAbstractTableModel>

/**
 * \file dailyschememodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.2.1
 * \date 07 Dec 2009
*/

namespace DrugsDB {
namespace Internal {
//class DrugsData;
//class DrugInteraction;
//class DrugInfo;
class DailySchemeModelPrivate;
} // end namespace Internal


class DRUGSBASE_EXPORT DailySchemeModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        DayReference = 0,
        Value,
        MaxParam
    };

    enum Method {
        Repeat = 0,
        Distribute
    };

    DailySchemeModel(QObject * parent = 0, const QString &serializedContent = QString::null);
    ~DailySchemeModel();

    void clear();

    void setMethod(Method method);
    Method method() const;

    QString serializedContent() const;
    void setSerializedContent(const QString &content);

    int columnCount(const QModelIndex &) const {return MaxParam;}

    int rowCount(const QModelIndex & = QModelIndex()) const {return Trans::Constants::Time::DS_MaxParam;}

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void setScored(bool isScored);
    void setMaximumDay(double max);
    double sum() const;

    QString humanReadableRepeatedDailyScheme() const;
    QString humanReadableDistributedDailyScheme() const;

Q_SIGNALS:
    void methodChanged();

private:
    Internal::DailySchemeModelPrivate *d;
};


} // end namespace DrugsDB

#endif // DAILYSCHEMEMODEL_H
