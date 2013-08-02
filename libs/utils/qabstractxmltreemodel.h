/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 ***************************************************************************/
#ifndef UTILS_QABSTRACTXMLTREEMODEL_H
#define UTILS_QABSTRACTXMLTREEMODEL_H

#include <utils/global_exporter.h>
#include <QAbstractItemModel>
QT_BEGIN_NAMESPACE
class QDomNode;
QT_END_NAMESPACE

/**
 * \file qabstractxmltreemodel.h
 * \author Eric MAEKER
 * \version 0.7.6
 * \date 19 Jun 2012
*/

namespace Utils {
namespace Internal {
class QAbstractXmlTreeModelPrivate;
}

class UTILS_EXPORT QAbstractXmlTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit QAbstractXmlTreeModel(const QString &fileName, const QString &mainTag, QObject *parent = 0);
    ~QAbstractXmlTreeModel();

    bool setSubMainTag(const QString &childToMainTag);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

//    virtual bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
//    virtual void fetchMore(const QModelIndex &parent = QModelIndex());

    virtual QDomNode node(const QModelIndex &index) const;

public Q_SLOTS:
    virtual bool saveModel();

private:
    Internal::QAbstractXmlTreeModelPrivate *dqaxtm;
};

}  // End namespace Utils

#endif // UTILS_QABSTRACTXMLTREEMODEL_H
