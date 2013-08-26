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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_HPRIMFILEMODEL_H
#define TOOLS_INTERNAL_HPRIMFILEMODEL_H

#include <QSortFilterProxyModel>
#include <QFileSystemModel>

/**
 * \file hprimfilemodel.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 24 May 2013
*/

namespace Tools {
namespace Internal {
class HprimFileModelPrivate;

class HprimFileModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    enum DataRepresentation {
        PatientName = 0,
        PatientDateOfBirth,
        FileName,
        FileDate,
        ColumnCount
    };

    explicit HprimFileModel(QObject *parent = 0);
    ~HprimFileModel();
    bool setRootPath(const QString &path);

    int columnCount(const QModelIndex &) const {return ColumnCount;}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QModelIndex fileRootPath() const;
    QFileInfo fileInfo(const QModelIndex &index) const;
    QString fileContent(const QModelIndex &index) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

private Q_SLOTS:
    void _onDirectoryLoaded(const QString &absPath);

private:
    HprimFileModelPrivate *d;
};

} // namespace Internal
} // namespace Tools

#endif // TOOLS_INTERNAL_HPRIMFILEMODEL_H

