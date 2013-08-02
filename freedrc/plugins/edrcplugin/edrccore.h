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
#ifndef EDRC_EDRCCORE_H
#define EDRC_EDRCCORE_H

#include <edrcplugin/edrc_exporter.h>
#include <QObject>

/**
 * \file edrccore.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 28 May 2013
*/

namespace eDRC {
namespace Internal {
class EdrcPlugin;
class DrcDatabase;
class EdrcCorePrivate;
} // namespace Internal

class EDRC_EXPORT EdrcCore : public QObject
{
    friend class eDRC::Internal::EdrcPlugin;
    Q_OBJECT

protected:
    explicit EdrcCore(QObject *parent = 0);
    
public:
    static EdrcCore &instance();
    ~EdrcCore();
    
    bool initialize();

//protected:
    Internal::DrcDatabase &edrcBase() const;

private:
    Internal::EdrcCorePrivate *d;
    static EdrcCore *_instance;
};

} // namespace eDRC

#endif  // EDRC_EDRCCORE_H

