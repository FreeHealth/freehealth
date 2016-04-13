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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>                  *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_TOKENPOOL_H
#define PADTOOLS_TOKENPOOL_H

#include <coreplugin/ipadtools.h>

#include <QString>
#include <QVariant>

/**
 * \file itoken.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 26 Apr 2012
*/

namespace PadTools {
namespace Internal {
class TokenPoolPrivate;

class TokenPool : public Core::ITokenPool
{
    Q_OBJECT
public:
    TokenPool(QObject *parent = 0);
    ~TokenPool();

    void registerNamespace(const Core::TokenNamespace &ns);
    int rootNamespaceCount() const;
    const Core::TokenNamespace &rootNamespaceAt(int index) const;
    Core::TokenNamespace getTokenNamespace(const QString &name) const;

    void addToken(Core::IToken *token);
    void addTokens(const QVector<Core::IToken *> &tokens);
    Core::IToken *token(const QString &name);
    void removeToken(Core::IToken *token);

    QList<Core::IToken *> tokens() const;

    QVariant tokenTestingValue(const QString &name);
    QVariant tokenCurrentValue(const QString &name);

private:
    static TokenPool *_instance;
    Internal::TokenPoolPrivate *d;
};

} // namespace Internal
} // namespace PadTools

#endif // PADTOOLS_TOKENPOOL_H
