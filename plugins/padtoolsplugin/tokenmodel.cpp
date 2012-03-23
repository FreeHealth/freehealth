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
#include "tokenmodel.h"
#include "constants.h"

#include <QStandardItem>
#include <QStringList>
#include <QMimeData>

#include <QDebug>

using namespace PadTools;
using namespace Internal;

namespace PadTools {
namespace Internal {
class TokenModelPrivate
{
public:
    TokenModelPrivate(TokenModel *parent) : q(parent) {}

    QStringList tokenNamespaces(const QString &token)
    {
        // split token using the following separators: :: . :
        QString sep;
        if (token.contains("."))
            sep = ".";
        else if (token.contains("::"))
            sep = "::";
        else if (token.contains(":"))
            sep = ":";
        QStringList ns;
        if (!sep.isEmpty())
            ns = token.split(sep, QString::SkipEmptyParts);
        else
            ns << token;
        return ns;
    }

    void createTree()
    {
        // Create namespaces
        QMapIterator<QString, QVariant> i(m_Tokens);
        while (i.hasNext()) {
            i.next();
            QStringList ns = tokenNamespaces(i.key());

            // token without namespace
            if (ns.count()==1) {
                QStandardItem *item = new QStandardItem(i.key());
                m_TokensToItem.insert(i.key(), item);
                q->invisibleRootItem()->appendRow(QList<QStandardItem*>() << item << new QStandardItem());
                continue;
            }
            // Create root category && all subs
            QStandardItem *item = 0;
            QStandardItem *parent = 0;
            QString fullNs;
            foreach(const QString &n, ns) {
                // get parent
                if (fullNs.isEmpty()) {
                    parent = q->invisibleRootItem();
                } else {
                    parent = m_TokensNamespaceToItem[fullNs];
                    fullNs += ".";
                }
                // recreate ns
                fullNs += n;

                // create ns
                item = m_TokensNamespaceToItem[fullNs];
                if (!item) {
                    item = new QStandardItem(n);
                    m_TokensToItem.insert(i.key(), item);
                    //                    if (n==ns.last()) {
                    //                        item->setToolTip(i.value().toString());
                    //                    }
                    m_TokensNamespaceToItem.insert(fullNs, item);
                    parent->appendRow(QList<QStandardItem*>() << item << new QStandardItem());
                }

            }
        }
    }


public:
    QMap<QString, QVariant> m_Tokens;
    QHash<QString, QStandardItem *> m_TokensToItem;
    QHash<QString, QStandardItem *> m_TokensNamespaceToItem;

private:
    TokenModel *q;
};
}  // Internal
}  // PadTools


TokenModel::TokenModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new Internal::TokenModelPrivate(this))
{
    // tmp: fill with dummy tokens
    d->m_Tokens.insert("Prescription.Drug.DRUG", "drug");
    d->m_Tokens.insert("Prescription.Drug.Q_FROM", "q_from");
    d->m_Tokens.insert("Prescription.Drug.Q_TO", "q_to");
    d->m_Tokens.insert("Prescription.Drug.Q_SCHEME", "q_scheme");
    d->m_Tokens.insert("Prescription.Drug.REPEATED_DAILY_SCHEME", "repeated daily scheme");
    d->m_Tokens.insert("Prescription.Drug.MEAL", "meal");
    d->m_Tokens.insert("PERIOD", "period");
    d->m_Tokens.insert("PERIOD_SCHEME", "period scheme");
    d->m_Tokens.insert("A", "A");
    d->m_Tokens.insert("B", "B");
    d->m_Tokens.insert("C", "C");
    d->m_Tokens.insert("D", "D");
    d->m_Tokens.insert("NULL", "");
    d->m_Tokens.insert("HTMLTOKEN", "<b>htmlToken</b>");

    d->createTree();
}

void TokenModel::setTokens(const QMap<QString, QVariant> &tokens)
{
    d->m_Tokens.clear();
    d->m_Tokens = tokens;
    reset();
}

QMap<QString, QVariant> &TokenModel::tokens()
{
    return d->m_Tokens;
}

//int TokenModel::rowCount(const QModelIndex &parent) const
//{
//    Q_UNUSED(parent);
//    return d->m_Tokens.count();
//}

QVariant TokenModel::data(const QModelIndex &index, int role) const
{
    return QStandardItemModel::data(index, role);
    //    if (!index.isValid())
    //        return QVariant();
    //    if (role==Qt::DisplayRole || role==Qt::EditRole) {
    //        const QString &k = d->m_Tokens.keys().at(index.row());
    //        switch (index.column()) {
    //        case TokenName: return k;
    //        case TokenValue: return d->m_Tokens.value(k);
    //        }
    //    }
    //    return QVariant();
}

bool TokenModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return QStandardItemModel::setData(index, value, role);
    //    if (!index.isValid())
    //        return false;
    //    if (role==Qt::EditRole) {
    //        const QString &k = d->m_Tokens.keys().at(index.row());
    //        switch (index.column()) {
    //        case TokenName: break;
    //        case TokenValue: d->m_Tokens.insert(k, value); break;
    //        }

    //        Q_EMIT dataChanged(index, index);
    //        Q_EMIT tokenChanged(k, value.toString());
    //    }
    //    return true;
}

Qt::ItemFlags TokenModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    if (index.isValid() && !hasChildren(index))
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    return defaultFlags;
}

Qt::DropActions TokenModel::supportedDropActions() const
{
    return Qt::CopyAction;
}

QStringList TokenModel::mimeTypes() const
{
    QStringList types;
    types << Constants::TOKENVALUE_MIME;
    return types;
}

QMimeData *TokenModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QString dt;

    // TODO : dt == token source
    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            dt += data(index, Qt::DisplayRole).toString();
        }
    }

    mimeData->setData(Constants::TOKENVALUE_MIME, dt.toUtf8());
    return mimeData;
}
