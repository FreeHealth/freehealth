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
#ifndef AVAILABLEMOVEMENTSMODEL_H
#define AVAILABLEMOVEMENTSMODEL_H

#include <accountbaseplugin/accountbase_exporter.h>

#include <QSqlTableModel>
#include <QStringList>
#include <QObject>

/**
 * \file templatesmodel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.2
 * \date 10 Jan 2010
*/


namespace AccountDB {
namespace Internal {
class AvailableMovementsModelPrivate;
}  // end namespace Internal


class ACCOUNTBASE_EXPORT AvailableMovementsModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class Internal::AvailableMovementsModelPrivate;
public:
    TemplatesModel(QObject * parent = 0);
    ~TemplatesModel();
    bool setCurrentUser(const QString &uuid);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    bool reparentIndex(const QModelIndex &item, const QModelIndex &parent);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant data(const QModelIndex & item, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertRow(int row, const QModelIndex &parent = QModelIndex()) {return insertRows(row,1,parent);}
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRow(int row, const QModelIndex &parent = QModelIndex()) {return removeRows(row,1,parent);}

//    Qt::DropActions supportedDropActions() const;
//    QStringList mimeTypes() const;
//    QMimeData *mimeData(const QModelIndexList &indexes) const;
//    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

//    QModelIndex getTemplateId(const int id);
//    QList<QPersistentModelIndex> getIndexesFromMimeData(const QMimeData *mime);

    bool isTemplate(const QModelIndex &index) const;
    bool isCategory(const QModelIndex &index) const {return !isTemplate(index);}

    void categoriesOnly();
    bool isCategoryOnly() const;
    void setReadOnly(const bool state);

    bool isDirty() const;

public Q_SLOTS:
    bool submit();

private:
    Internal::AvailableMovementsModelPrivate *d;
};

}  // end namespace Templates


#endif // AVAILABLEMOVEMENTSMODEL_H
