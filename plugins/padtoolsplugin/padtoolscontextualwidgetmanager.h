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
 *   Main Developers:                                                      *
 *       Eric Maeker                                                       *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PADTOOLS_INTERNAL_PADTOOLSCONTEXTUALWIDGETMANAGER_H
#define PADTOOLS_INTERNAL_PADTOOLSCONTEXTUALWIDGETMANAGER_H

#include <coreplugin/contextmanager/icontext.h>
#include <QObject>
#include <QPointer>

/**
 * \file padtoolscontextualwidget.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 21 Dec 2012
*/

namespace Core {
class IContext;
}

namespace PadTools {
namespace Internal {
class PadWriter;
class PadToolsContextualWidget;

class PadToolsActionHandler : public QObject
{
    Q_OBJECT
public:
    PadToolsActionHandler(QObject *parent = 0);
    virtual ~PadToolsActionHandler() {}

    void setCurrentView(PadWriter *view);

private Q_SLOTS:
    void clear();
    void onDefaultValuesRequested();
    void showDatabaseInformation();

private Q_SLOTS:
    void updateActions();

protected:
    QAction *aClear;
    QAction *aSetDefaultValues;

    QPointer<PadWriter> m_CurrentView;
};

class PadToolsContextualWidgetManager : public PadToolsActionHandler
{
    Q_OBJECT
    
public:
    explicit PadToolsContextualWidgetManager(QObject *parent = 0);
    ~PadToolsContextualWidgetManager();
    
    PadWriter *currentView() const;
    
private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);
};

} // namespace Internal
} // namespace PadTools

#endif // PADTOOLS_INTERNAL_PADTOOLSCONTEXTUALWIDGETMANAGER_H

