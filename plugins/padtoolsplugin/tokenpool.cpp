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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>                  *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
 * \class PadTools::TokenPool
 * Manage the token. Deletes all registered token in dtor
 */

#include "tokenpool.h"

#include <utils/global.h>
#include <translationutils/constants.h>

#include <QApplication>
#include <QDebug>

using namespace PadTools;
using namespace Internal;


namespace PadTools {
namespace Internal {
class TokenPoolPrivate
{
public:
    TokenPoolPrivate() {}
    ~TokenPoolPrivate() {}

public:
    QVector<Core::IToken*> _tokens;
    QList<Core::TokenNamespace*> _namespace;
    Core::TokenNamespace nullNamespace;
};
}
}

TokenPool::TokenPool(QObject *parent) :
    Core::ITokenPool(parent),
    d(new Internal::TokenPoolPrivate)
{}

TokenPool::~TokenPool()
{
    qDeleteAll(d->_tokens);
    d->_tokens.clear();
    if (d) {
        delete d;
        d = 0;
    }
}

void TokenPool::registerNamespace(const Core::TokenNamespace &ns)
{
    Core::TokenNamespace *pns = new Core::TokenNamespace(ns);
    d->_namespace << pns;
}

int TokenPool::rootNamespaceCount() const
{
    return d->_namespace.count();
}

const Core::TokenNamespace &TokenPool::rootNamespaceAt(int index) const
{
    if (IN_RANGE_STRICT_MAX(index, 0, d->_namespace.count()))
        return *d->_namespace.at(index);
    return d->nullNamespace;
}

static QStringList tokenNamespaces(const QString &token)
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

Core::TokenNamespace TokenPool::getTokenNamespace(const QString &name) const
{
    // Split token uid to namespaces + tokenUid
    QStringList namespaces = tokenNamespaces(name);
    if (namespaces.isEmpty())
        return d->nullNamespace;

    // Get the root namespace
    QString root = namespaces.takeFirst();
    Core::TokenNamespace *rootNs = 0;
    for(int i = 0; i < d->_namespace.count(); ++i) {
        if (d->_namespace.at(i)->uid().compare(root, Qt::CaseInsensitive)==0) {
            rootNs = d->_namespace.at(i);
            break;
        }
    }
    if (!rootNs)
        return d->nullNamespace;

    // remove token uid
    namespaces.takeLast();

    // create a copy of the root namespace
    Core::TokenNamespace returnNs(*rootNs);
    returnNs.clearChildren();

    Core::TokenNamespace *lastChild = rootNs;
    Core::TokenNamespace *addTo = const_cast<Core::TokenNamespace *>(&returnNs);
    // scan all children of the ns
    foreach(const QString &ns, namespaces) {
        foreach(const Core::TokenNamespace &chNs, lastChild->children()) {
            if (chNs.uid() == ns) {
                Core::TokenNamespace addMe(chNs);
                addMe.clearChildren();
                addTo->addChild(addMe);
                lastChild = const_cast<Core::TokenNamespace *>(&chNs);
                addTo = const_cast<Core::TokenNamespace *>(&addMe);
                break;
            }
        }
    }
    return returnNs;
}

void TokenPool::addToken(Core::IToken *token)
{
    d->_tokens << token;
}

void TokenPool::addTokens(QVector<Core::IToken *> &tokens)
{
    for(int i=0; i < tokens.count(); ++i)
        d->_tokens.append(tokens.at(i));
}

Core::IToken *TokenPool::token(const QString &name)
{
    foreach(Core::IToken *t, d->_tokens) {
        if (name.startsWith(t->uid().left(10), Qt::CaseInsensitive)) {
            if (name.compare(t->uid(), Qt::CaseInsensitive)==0)
                return t;
        }
    }
    return 0;
}

void TokenPool::removeToken(Core::IToken *token)
{
    if (d->_tokens.contains(token))
        d->_tokens.remove(d->_tokens.indexOf(token));
}

QVector<Core::IToken *> TokenPool::tokens() const
{
    return d->_tokens;
}

QVariant TokenPool::tokenTestingValue(const QString &name)
{
    Core::IToken *t = token(name);
    if (!t)
        return QVariant();
    return t->testValue();
}

QVariant TokenPool::tokenCurrentValue(const QString &name)
{
    Core::IToken *t = token(name);
    if (!t)
        return QVariant();
    return t->value();
}
