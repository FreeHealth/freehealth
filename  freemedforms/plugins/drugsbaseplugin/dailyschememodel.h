/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#ifndef DAILYSCHEMEMODEL_H
#define DAILYSCHEMEMODEL_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <translationutils/constanttranslations.h>

#include <QString>
#include <QAbstractTableModel>

/**
 * \file dailyschememodel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
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
