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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CORE_CONTEXTMANAGERPRIVATE_H
#define CORE_CONTEXTMANAGERPRIVATE_H
#include "contextmanager.h"
#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QMap>

#include <coreplugin/contextmanager/icontext.h>

/**
 * \file contextmanager.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 20 Sept 2012
*/

namespace Core {
namespace Internal {

class ContextManagerPrivate : public ContextManager
{
    Q_OBJECT
public:
    ContextManagerPrivate(QMainWindow *mainWin);
    ContextManagerPrivate() {}

    IContext *contextObject(QWidget *widget);
    void addContextObject(IContext *context);
    void removeContextObject(IContext *context);
    void resetContext();

    IContext *currentContextObject() const;
    void addAdditionalContext(int context);
    void updateAdditionalContexts(const Context &remove, const Context &add);
    void removeAdditionalContext(int context);
    bool hasContext(int context) const;

    void updateContext();

private Q_SLOTS:
    void updateFocusWidget(QWidget *old, QWidget *now);

private:
    void updateContextObject(IContext *context);

private:
    Context m_globalContext;
    Context m_additionalContexts;
    IContext *m_activeContext;
    QMap<QWidget *, IContext *> m_contextWidgets;
    QMainWindow *m_mainWindow;
};


} // End Internal
} // End Core

#endif // CORE_CONTEXTMANAGERPRIVATE_H
