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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGIN_INTERACTORMODE_H
#define DDIMANAGER_DDIPLUGIN_INTERACTORMODE_H

#include <coreplugin/modemanager/imode.h>

#include <QObject>

/**
 * \file interactormode.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 14 Oct 2013
*/

namespace DDI {
class InteractorEditorWidget;
namespace Internal {
class InteractorMode : public Core::IMode
{
    Q_OBJECT
public:
    InteractorMode(QObject *parent);
    ~InteractorMode();

    void postCoreInitialization();

#ifdef WITH_TESTS
public Q_SLOTS:
    void test_runWidgetTests();
#endif

private:
    InteractorEditorWidget *_widget;
};

} // namespace Internal
} // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_INTERACTORMODE_H
