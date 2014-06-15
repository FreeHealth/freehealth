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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TEMPLATESMODEL_H
#define TEMPLATESMODEL_H

#include <templatesplugin/templates_exporter.h>
#include <templatesplugin/itemplates.h>

#include <QAbstractItemModel>
#include <QStringList>
#include <QObject>

/**
 * \file templatesmodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.2.2
 * \date 10 Jan 2009
*/

namespace Templates {
namespace Internal {
class TemplatesModelPrivate;
}  // end namespace Internal

class TEMPLATES_EXPORT TemplatesModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class Internal::TemplatesModelPrivate;
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
    Qt::DropActions supportedDropActions() const;

    bool insertTemplate(const Templates::ITemplate *t);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    QModelIndex getTemplateId(const int id);
    QList<QPersistentModelIndex> getIndexesFromMimeData(const QMimeData *mime);

    bool isTemplate(const QModelIndex &index) const;
    bool isCategory(const QModelIndex &index) const {return !isTemplate(index);}

    void categoriesOnly();
    bool isCategoryOnly() const;
    void setReadOnly(const bool state);

    bool isDirty() const;

    const ITemplate *getTemplate(const QModelIndex &item) const;

public Q_SLOTS:
    bool submit();

private Q_SLOTS:
    void onCoreDatabaseServerChanged();

private:
    Internal::TemplatesModelPrivate *d;
};

}  // end namespace Templates


#endif // TEMPLATESMODEL_H
