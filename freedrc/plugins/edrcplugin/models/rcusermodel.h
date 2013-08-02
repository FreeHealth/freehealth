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
#ifndef EDRC_RCUSERMODEL_H
#define EDRC_RCUSERMODEL_H

#include <QObject>

/**
 * \file rcusermodel.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 01 Jun 2013
*/

namespace eDRC {
namespace Internal {
class RcUserModelPrivate;
} // namespace Internal

class RcUserModel : public QObject
{
    Q_OBJECT
    
public:
    explicit RcUserModel(QObject *parent = 0);
    ~RcUserModel();
    
    bool initialize();
    
Q_SIGNALS:
    
public Q_SLOTS:
    
private:
    Internal::RcUserModelPrivate *d;
};

} // namespace eDRC

#endif  // EDRC_RCUSERMODEL_H

