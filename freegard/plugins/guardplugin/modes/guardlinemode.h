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
 *  Main Developers: Eric MAEKER, <eric.maeker@gmail.com>                  *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef GUARD_GUARDLINEMODE_H
#define GUARD_GUARDLINEMODE_H

#include <coreplugin/modemanager/imode.h>

/**
 * \file guardlinemode.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 12 Oct 2014
*/

namespace Guard {
namespace Internal {

class GuardLineMode : public Core::IMode
{
    Q_OBJECT
public:
    explicit GuardLineMode(QObject *parent = 0);
    ~GuardLineMode();

    void postCoreInitialization();

#ifdef WITH_TESTS
    void test_runWidgetTests();
#endif

private:
//    __Widget *_widget;

};

}  // End namespace Internal
}  // End namespace Guard

#endif // GUARD_GUARDLINEMODE_H
