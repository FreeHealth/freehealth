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
 *  Main Developers:                                                       *
 *       Eric Maeker <eric.maeker@gmail.com>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef GUARD_INTERNAL_GUARDLINEVIEW_H
#define GUARD_INTERNAL_GUARDLINEVIEW_H

#include <coreplugin/contextmanager/icontext.h>
#include <QWidget>
#include <QList>

/**
 * \file guardlineview.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 12 Oct 2014
*/

namespace Guard {
namespace Internal {
class GuardContext;

class GuardLineView : public QWidget
{
    Q_OBJECT

public:
    explicit GuardLineView(QWidget *parent = 0);
    ~GuardLineView();

    void addContexts(const QList<int> &contexts);

private:
    GuardContext *m_Context;
};

//class GuardContext : public Core::IContext
//{
//    Q_OBJECT
//public:
//    GuardContext(GuardLineView *w) :
//        Core::IContext(w)
//    {
//        setObjectName("GuardContext");
//        setWidget(w);
//    }
//};

} // namespace Internal
} // namespace Guard

#endif // GUARD_INTERNAL_GUARDLINEVIEW_H

