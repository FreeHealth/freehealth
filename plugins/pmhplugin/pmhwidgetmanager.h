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
 *  Authors:                                                               *
 *       Eric MAEKER <eric.maeker@gmail.com>                               *
 *       Jerome PINGUET <jerome@jerome.cc>                                 *
 ***************************************************************************/

#pragma once

#include <pmhplugin/pmh_exporter.h>
#include <pmhplugin/pmhcontextualwidget.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QObject>
#include <QPointer>

namespace Core {
class IContext;
}

namespace PMH {
class PmhCore;

namespace Internal {
class PmhContextualWidget;

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
    void onClearRequested();
    void onSaveEpisodeRequested();
    void onCreateEpisodeRequested();
    void onValidateEpisodeRequested();
    void onRenewEpisodeRequested();
    void onRemoveEpisodeRequested();
    void onTakeScreenshotRequested();
    void onPrintFormRequested();
    void updateActions();
    void onActionEnabledStateUpdated(PMH::Internal::PmhContextualWidget::WidgetAction action);

protected:
    QAction *aClear;
    QAction *aAddPmh;
    QAction *aRemovePmh;
    QAction *aAddCat;
    //QAction *aCategoryManager;
    QAction *aPmhDatabaseInformation;
    QAction *aCreateEpisode, *aValidateEpisode, *aRenewEpisode, *aRemoveEpisode, *aSaveEpisode;
    QAction *aTakeScreenshot;
    QAction *aPrintForm;

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

private:
    QPointer<Core::IContext> _contextObject;
};

}  // End namespace Internal
}  // End namespace PMH
