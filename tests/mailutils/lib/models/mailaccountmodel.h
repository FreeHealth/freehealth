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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MAILUTILS_MAILACCOUNTMODEL_H
#define MAILUTILS_MAILACCOUNTMODEL_H

#include "../mail_exporter.h"
#include <QAbstractTableModel>

/**
 * \file mailaccountmodel.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 13 May 2013
*/

namespace Mail {
class MailAccount;
namespace Internal {
class MailAccountModelPrivate;

class MAIL_EXPORT MailAccountModel : public QAbstractTableModel
{
    Q_OBJECT
    
public:
    enum ColumnDataRepresentation {
        Label = 0,
        Uid,
        UserFullName,
        UserAddress,
        UserLog,
        UserPass,
        HostName,
        HostPort,
        HostUseSsl,
        HostUseStartTsl,
        LastSucceededConnection,
        ColumnCount

    };
    explicit MailAccountModel(QObject *parent = 0);
    ~MailAccountModel();
    bool initialize();

    int columnCount(const QModelIndex &) const {return ColumnCount;}
    int rowCount(const QModelIndex &) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    void addMailAccount(const MailAccount &account);
    void addMailAccounts(const QList<MailAccount> &accounts);

    const QList<MailAccount> &mailAccounts() const;

Q_SIGNALS:
    
public Q_SLOTS:
    
private:
    Internal::MailAccountModelPrivate *d;
};

} // namespace Internal
} // namespace Mail

#endif  // MAILUTILS_MAILACCOUNTMODEL_H

