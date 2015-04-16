/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:  Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef PADTOOLS_IMPL_H
#define PADTOOLS_IMPL_H

#include <coreplugin/ipadtools.h>

/**
 * \file padtoolsimpl.h
 * \author Eric Maeker, Guillaume Denry
 * \version 0.8.0
 * \date 25 Apr 2012
*/

namespace Core {
class ITokenPool;
}

namespace PadTools {
namespace Internal {
class TokenPool;

class PadToolsImpl : public Core::IPadTools
{
    Q_OBJECT
public:
    PadToolsImpl(QObject *parent = 0);
    ~PadToolsImpl();

    Core::ITokenPool *tokenPool() const;

    QString processPlainText(const QString &plainText);
    QString processHtml(const QString &html);

    Core::IPadWriter *createWriter(QWidget *parent = 0);

private:
    TokenPool *_pool;
};

} // namespace Internal
} // namespace PadTools

#endif  // PADTOOLS_IMPL_H
