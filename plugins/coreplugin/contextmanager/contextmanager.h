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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CORE_CONTEXTMANAGER_H
#define CORE_CONTEXTMANAGER_H

// Adapted from QtCreator 2.5.2 coreplugin/mainwindow

#include <coreplugin/core_exporter.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QObject>

/**
 * \file contextmanager.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 20 Sept 2012
*/

namespace Core {

class CORE_EXPORT ContextManager : public QObject
{
    Q_OBJECT
public:
    ContextManager(QObject *parent = 0) : QObject(parent) {}
    virtual ~ContextManager() {}

    virtual IContext *contextObject(QWidget *widget) = 0;
    virtual void addContextObject(IContext *context) = 0;
    virtual void updateContextObject(IContext *context) = 0;
    virtual void removeContextObject(IContext *context) = 0;
    virtual void resetContext() = 0;

    virtual IContext *currentContextObject() const = 0;
    virtual void updateAdditionalContexts(const Context &remove, const Context &add) = 0;
    virtual void addAdditionalContext(int context) = 0;
    virtual void removeAdditionalContext(int context) = 0;
    virtual bool hasContext(int context) const = 0;

    virtual void updateContext() = 0;

Q_SIGNALS:
    void contextAboutToChange(Core::IContext *context);
    void contextChanged(Core::IContext *context, const Core::Context &additionalContexts);
};

} // End Core

#endif // CORE_CONTEXTMANAGER_H
