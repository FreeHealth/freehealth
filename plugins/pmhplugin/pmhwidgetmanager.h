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
 *   Main Developer : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef PMHWIDGETMANAGER_H
#define PMHWIDGETMANAGER_H

#include <pmhplugin/pmh_exporter.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QObject>
#include <QPointer>


namespace PMH {
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
    void patientChanged();
    void showPmhDatabaseInformations();
    void categoryManager();

private:
    void updateActions();

protected:
    QAction *aAddPmh;
    QAction *aRemovePmh;
    QAction *aAddCat;
    QAction *aCategoryManager;
    QAction *aPmhDatabaseInformations;
    QPointer<PmhContextualWidget> m_CurrentView;
};

}  // End namespace Internal

class PmhWidgetManager : private Internal::PmhActionHandler
{
    Q_OBJECT
public:
    static PmhWidgetManager *instance(QObject *parent = 0);
    PmhContextualWidget *currentView() const;

private Q_SLOTS:
    void updateContext(Core::IContext *object);

private:
    PmhWidgetManager(QObject *parent = 0);
    static PmhWidgetManager *m_Instance;
};


}  // End namespace PMH


#endif // PMHWIDGETMANAGER_H
