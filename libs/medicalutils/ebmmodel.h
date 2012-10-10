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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EBMMODEL_H
#define EBMMODEL_H

#include <medicalutils/medical_exporter.h>
#include <QAbstractListModel>
#include <QVector>

namespace MedicalUtils {
class EbmData;

class MEDICALUTILS_EXPORT EbmModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Link = 0,
        References,
        ShortReferences,
        Abstract,
        AbstractWithReferences,
        AbstractWithReferencesHtml,
        ColumnCount
    };

    EbmModel(QObject *parent = 0);
    ~EbmModel();

    void clear();

    int rowCount(const QModelIndex & = QModelIndex()) const {return m_Ebms.count();}
    int columnCount(const QModelIndex & = QModelIndex()) const {return ColumnCount;}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {Q_UNUSED(index); Q_UNUSED(value); Q_UNUSED(role); return false;}
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void setEbmData(const QVector<EbmData *> &ebms);

private:
    QVector<EbmData *> m_Ebms;
};

}  // End namespace MedicalUtils

#endif // EBMMODEL_H
