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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_TOKENMODEL_H
#define PADTOOLS_TOKENMODEL_H

#include <coreplugin/ipadtools.h>

#include <QStandardItemModel>
#include <QMap>

/**
 * \file tokenmodel.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 05 May 2012
*/

namespace Core {
class IToken;
}

namespace PadTools {
namespace Internal {
class TokenModelPrivate;
}

class TokenModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        TokenName = 0,
//        TokenValue,
        ColumnCount
    };

    explicit TokenModel(QObject *parent = 0);

    Core::ITokenPool *tokenPool() const;

    void addToken(Core::IToken *token);
    void addTokens(const QVector<Core::IToken *> &token);

//    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &) const {return ColumnCount;}

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    
    Qt::ItemFlags flags(const QModelIndex &index) const;
    Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;

Q_SIGNALS:
    void tokenChanged(const QString &token, const QString &value);

private:
    Internal::TokenModelPrivate *d;
};

} // namespace PadTools

#endif // PADTOOLS_TOKENMODEL_H
