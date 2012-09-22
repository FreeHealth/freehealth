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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDAWIDGETMANAGER_H
#define AGENDAWIDGETMANAGER_H

#include <agendaplugin/agenda_exporter.h>

#include <coreplugin/contextmanager/icontext.h>

#include <QWidget>
#include <QObject>
#include <QAction>
#include <QPointer>

/**
 * \file agendawidgetmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 18 Jun 2011
 * \internal
*/

namespace Agenda {
namespace Internal {

//class AgendaContext : public Core::IContext
//{
//public:
//    AgendaContext(DrugsCentralWidget *w) : Core::IContext(w), wgt(w) { setObjectName("AgendaContext"); }
//    void setContext(QList<int> c) { ctx = c; }
//    QList<int> context() const { return ctx; }
//    QWidget *widget() { return wgt; }

//private:
//    QWidget *wgt;
//    QList<int> ctx;
//};

class AGENDA_EXPORT AgendaActionHandler : public QObject
{
    Q_OBJECT
public:
    AgendaActionHandler(QObject *parent = 0);
    virtual ~AgendaActionHandler() {}

    void setCurrentView(QWidget *view);

private Q_SLOTS:
    void clear();
//    void newEvent();
    void printSelection();
    void printPreviewSelection();

    void showAgendaDatabaseInformation();

private:
    void updateActions();

protected:
    QAction *aClear;
    QAction *aNewEvent;
    QAction *aPrintSelection;
    QAction *aPrintPreviewSelection;
    QAction *aAgendaDatabaseInformation;

    QPointer<QWidget> m_CurrentView;
};

}  // End Internal
}  // End Agenda


namespace Agenda {

class AGENDA_EXPORT AgendaWidgetManager : public Internal::AgendaActionHandler
{
    Q_OBJECT
public:
    static AgendaWidgetManager *instance();
    ~AgendaWidgetManager() {}

    QWidget *currentView() const;

private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);

private:
    AgendaWidgetManager(QObject *parent = 0);
    static AgendaWidgetManager *m_Instance;
};

}  // End Agenda

#endif // AGENDAWIDGETMANAGER_H
