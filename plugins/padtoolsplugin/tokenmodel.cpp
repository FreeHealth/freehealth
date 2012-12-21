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
/**
 * \class PadTools::TokenModel
 * Transform the Core::ITokenPool to a QStandardItemModel (tree model).
 */

#include "tokenmodel.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipadtools.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QStandardItem>
#include <QStringList>
#include <QMimeData>

#include <QDebug>

using namespace PadTools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
static inline Core::ITokenPool *tokenPool() {return Core::ICore::instance()->padTools()->tokenPool();}
const int TOKEN_UID = Qt::UserRole + 1;
}

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

    void createNamespace(const Core::TokenNamespace &ns, QStandardItem *parent = 0)
    {
        if (!parent)
            parent = q->invisibleRootItem();
        QString fullNs = parent->data(TOKEN_UID).toString();
        fullNs.isEmpty() ? fullNs = ns.uid() : fullNs += "." + ns.uid();
        QStandardItem *item = new QStandardItem(ns.humanReadableName());
        item->setData(fullNs, TOKEN_UID);
        if (!ns.tooltip().isEmpty())
            item->setToolTip(ns.tooltip());
        parent->appendRow(item);
        _tokensNamespaceToItem.insert(fullNs, item);
        foreach(const Core::TokenNamespace &child, ns.children()) {
            createNamespace(child, item);
        }
    }

    void createTree()
    {
        // Create namespaces
        for(int i=0; i < tokenPool()->rootNamespaceCount(); ++i) {
            const Core::TokenNamespace &ns = tokenPool()->rootNamespaceAt(i);
            Q_ASSERT(ns.isValid());
            if (!ns.isValid()) {
                LOG_ERROR_FOR("TokenModel", "Namespace not valid?");
                continue;
            }
            createNamespace(ns);
        }

        // Add tokens to namespaces
        _tokens = tokenPool()->tokens();
        for(int i=0; i < _tokens.count(); ++i) {
            Core::IToken *token = _tokens.at(i);
            QStringList ns = tokenNamespaces(token->uid());
            QString name;
            token->humanReadableName().isEmpty() ? name=token->uid() : name=token->humanReadableName();

            // token without namespace
            if (ns.count()==1) {
                QStandardItem *item = new QStandardItem(name);
                _tokensToItem.insert(token, item);
                q->invisibleRootItem()->appendRow(QList<QStandardItem*>() << item << new QStandardItem());
                continue;
            }
            // get NS item
            ns.takeLast();
            QStandardItem *nsItem = 0;
            while (nsItem==0 && ns.count()>1) {
                nsItem = _tokensNamespaceToItem.value(ns.join("."), 0);
                if (!nsItem)
                    ns.takeLast();
            }
            if (!nsItem) {
                LOG_ERROR_FOR("TokenModel", "Namespace not found? " + token->uid());
                nsItem = q->invisibleRootItem();
            }

            // create token item
            token->humanReadableName().isEmpty() ? name=token->uid() : name=token->humanReadableName();
            QStandardItem *item = new QStandardItem(name);
            item->setToolTip(token->tooltip());
            _tokensToItem.insert(token, item);
            nsItem->appendRow(QList<QStandardItem*>() << item << new QStandardItem());
        }
    }


public:
    QMap<QString, QVariant> m_Tokens;
    QVector<Core::IToken *> _tokens;
    QHash<Core::IToken *, QStandardItem *> _tokensToItem;
    QHash<QString, QStandardItem *> _tokensNamespaceToItem;

private:
    TokenModel *q;
};
}  // Internal
}  // PadTools


TokenModel::TokenModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new Internal::TokenModelPrivate(this))
{
    Q_ASSERT(tokenPool());
    d->createTree();
}

Core::ITokenPool *TokenModel::tokenPool() const
{
    return ::tokenPool();
}

void TokenModel::addToken(Core::IToken *token)
{
    d->_tokens.append(token);
}

void TokenModel::addTokens(const QVector<Core::IToken*> &tokens)
{
    for(int i=0; i<tokens.count();++i)
        d->_tokens.append(tokens.at(i));
}

QVariant TokenModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (!index.parent().isValid() &&
            hasChildren(index) &&
            role==Qt::FontRole) {
        QFont bold;
        bold.setBold(true);
        return bold;
    }

    if (role==Qt::ToolTipRole) {
        Core::IToken *token = d->_tokensToItem.key(itemFromIndex(index));
        if (!token)
            return QStandardItemModel::data(index, role);
        return token->tooltip();
    }

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
    Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.isValid()) {
        if (!hasChildren(this->index(index.row(), 0, index.parent()))) {
            return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
        }
    }
    return defaultFlags;
}

Qt::DropActions TokenModel::supportedDropActions() const
{
    return Qt::CopyAction;
}

QStringList TokenModel::mimeTypes() const
{
    QStringList types;
    types << Constants::TOKENVALUE_MIME << Constants::TOKENRAWSOURCE_MIME;
    return types;
}

QMimeData *TokenModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    if (indexes.isEmpty())
        return mimeData;

    // TODO: code multiple line selection ? */
//    foreach (const QModelIndex &index, indexes) {
//        if (index.isValid()) {
//            dt += data(index, Qt::DisplayRole).toString();
//        }
//    }
    Core::IToken *token = d->_tokensToItem.key(itemFromIndex(indexes.at(0)));
    QString name = token->uid(); // d->m_Tokens.keys().at(indexes.at(0).row());
    const QVariant &value = token->value();
    mimeData->setData(Constants::TOKENVALUE_MIME, value.toByteArray());
    mimeData->setData(Constants::TOKENNAME_MIME, name.toUtf8());
    name = QString("%1%2%3%2%4")
            .arg(Constants::TOKEN_OPEN_DELIMITER)
            .arg(Constants::TOKEN_CORE_DELIMITER)
            .arg(name)
            .arg(Constants::TOKEN_CLOSE_DELIMITER);
    mimeData->setData(Constants::TOKENRAWSOURCE_MIME, name.toUtf8());
    return mimeData;
}
