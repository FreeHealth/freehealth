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

#include <translationutils/constants.h>

#include <QApplication>

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
    QList<Core::TokenNamespace> _namespace;
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

//TokenNamespace &TokenPool::createNamespace(const QString &name)
//{
//    TokenNamespace ns(name);
//    d->_namespace << ns;
//    return ns;
//}

//void TokenPool::registerNamespace(const TokenNamespace &ns)
//{
//    d->_namespace << ns;
//}

//const TokenNamespace &TokenPool::getTokenNamespace(const QString &name) const
//{
//    foreach(const TokenNamespace &ns, d->_namespace)
//        if (ns.fullName().compare(name,Qt::CaseInsensitive)==0)
//            return ns;
//    return d->nullNamespace;
//}

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
        if (name.startsWith(t->fullName().left(10), Qt::CaseInsensitive)) {
            if (name.compare(t->fullName(), Qt::CaseInsensitive)==0)
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
