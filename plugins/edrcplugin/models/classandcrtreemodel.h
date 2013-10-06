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
#ifndef EDRC_CLASSANDClassAndCrTreeModel_H
#define EDRC_CLASSANDClassAndCrTreeModel_H

#include <QStandardItemModel>

/**
 * \file classandcrtreemodel.h
 * \author Eric Maeker
 * \version 0.9.2
 * \date 05 Oct 2013
*/

namespace eDRC {
namespace Internal {
class ClassAndCrTreeModelPrivate;
} // namespace Internal

class ClassAndCrTreeModel : public QStandardItemModel
{
    Q_OBJECT
    
public:
    enum DataRepresentation {
        Label = 0,
        Id
    };

    explicit ClassAndCrTreeModel(QObject *parent = 0);
    ~ClassAndCrTreeModel();
    bool initialize();

    int id(const QModelIndex &index) const;
    QString arguments(const QModelIndex &index) const;
    QStringList authorizedDiagnosis(const QModelIndex &index) const;

private:
    Internal::ClassAndCrTreeModelPrivate *d;
};

} // namespace eDRC

#endif  // EDRC_CLASSANDClassAndCrTreeModel_H

