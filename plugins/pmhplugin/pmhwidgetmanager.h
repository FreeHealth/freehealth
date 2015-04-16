/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef PMHWIDGETMANAGER_H
#define PMHWIDGETMANAGER_H

#include <pmhplugin/pmh_exporter.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QObject>
#include <QPointer>

/**
 * \file pmhwidgetmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 19 Sept 2012
*/

namespace PMH {
class PmhCore;
class PmhContextualWidget;
namespace Internal {

class PmhActionHandler : public QObject
{
    Q_OBJECT
public:
    PmhActionHandler(QObject *parent = 0);
    virtual ~PmhActionHandler() {}

    void setCurrentView(PmhContextualWidget *view);

private Q_SLOTS:
    void onCurrentPatientChanged();
    void showPmhDatabaseInformation();
    void categoryManager();

private:
    void updateActions();

protected:
    QAction *aAddPmh;
    QAction *aRemovePmh;
    QAction *aAddCat;
    QAction *aCategoryManager;
    QAction *aPmhDatabaseInformation;
    QPointer<PmhContextualWidget> m_CurrentView;
};

class PmhWidgetManager : private PmhActionHandler
{
    Q_OBJECT
    friend class PMH::PmhCore;

protected:
    PmhWidgetManager(QObject *parent = 0);

public:
    PmhContextualWidget *currentView() const;

private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);
};

}  // End namespace Internal
}  // End namespace PMH

#endif // PMHWIDGETMANAGER_H
