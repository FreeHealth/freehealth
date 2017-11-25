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
#ifndef FREEDDIMANAGER_ATCTABLEMODEL_H
#define FREEDDIMANAGER_ATCTABLEMODEL_H

#include <ddiplugin/ddi_exporter.h>
#include <QAbstractTableModel>

/**
 * \file atctablemodel.h
 * \author Eric MAEKER
 * \version 0.10.0
 * \date 01 Jan 2014
*/

namespace DDI {
class DDICore;
namespace Internal {
class AtcTableModelPrivate;
}

class DDI_EXPORT AtcTableModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class DDI::Internal::AtcTableModelPrivate;
    friend class DDI::DDICore;

protected:
    AtcTableModel(QObject * parent = 0);
    bool initialize();
    bool onDdiDatabaseChanged();

public:
    enum DataRepresentation {
        Id = 0,
        IsValid,
        Code,
        Uid,
        LabelFr,
        LabelEn,
        LabelDe,
        LabelSp,
        DateCreation,
        DateUpdate,
        PreviousCode,
        WhoUpdateYear,
        Comment,
        ColumnCount
    };
    ~AtcTableModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual void fetchMore(const QModelIndex &parent);
    virtual bool canFetchMore(const QModelIndex &parent) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
//    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
//                               int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

//    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
//    virtual bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
//    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
//    virtual bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    bool isCodeExists(const QString &code) const;
    bool isUidExists(const QString &uid) const;
    bool createAtcCode(const QString &code, const QString &uid);

public Q_SLOTS:
    bool submit();

private:
    static AtcTableModel *_instance;
    Internal::AtcTableModelPrivate *d;
};

}  //  End namespace DDI

#endif // FREEDDIMANAGER_ATCTABLEMODEL_H
