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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PADTOOLS_INTERNAL_PADTOOLSCORE_H
#define PADTOOLS_INTERNAL_PADTOOLSCORE_H

#include <QObject>

/**
 * \file padtoolscore.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 21 Dec 2012
*/

namespace PadTools {
namespace Internal {
class PadToolsPlugin;
class PadToolsCorePrivate;

class PadToolsCore : public QObject
{
    Q_OBJECT
    friend class PadTools::Internal::PadToolsPlugin;

protected:
    explicit PadToolsCore(QObject *parent = 0);
    bool initialize();

public:
    static PadToolsCore &instance();
    ~PadToolsCore();
    
    
Q_SIGNALS:
    
public Q_SLOTS:
    
private:
    Internal::PadToolsCorePrivate *d;
    static PadToolsCore *_instance;
};

} // namespace Internal
} // namespace PadTools

#endif // PADTOOLS_INTERNAL_PADTOOLSCORE_H

