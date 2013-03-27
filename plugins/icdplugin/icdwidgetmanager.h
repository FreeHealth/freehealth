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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ICDWIDGETMANAGER_H
#define ICDWIDGETMANAGER_H

#include <icdplugin/icd_exporter.h>
#include <coreplugin/contextmanager/icontext.h>

#include <QWidget>
#include <QObject>
#include <QAction>
#include <QPointer>

/**
 * \file icdwidgetmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 14 Oct 2010
 * \internal
*/

namespace ICD {
class IcdWidgetManager;
class IcdDownloader;
class IcdCentralWidget;

namespace Internal {

class IcdActionHandler : public QObject
{
    Q_OBJECT
public:
    enum Modes {
        Prescriber = 0,
        SelectOnly
    };

    IcdActionHandler(QObject *parent = 0);
    virtual ~IcdActionHandler() {}

    void setCurrentView(IcdCentralWidget *view);

private Q_SLOTS:
    void recreateDatabase();
    void showDatabaseInformation();
    void searchActionChanged(QAction*);
    void modeActionChanged(QAction*);
    void toggleSelector();
    void clear();
    void removeItem();
    void print();

private:
    void updateActions();

protected:
    QAction *aRecreateDatabase;
    QAction *aShowDatabaseInformation;
    QAction *aSearchByLabel, *aSearchByCode;
    QActionGroup *gSearchMethod, *gModes;
    QAction *aSelectorSimpleMode, *aSelectorFullMode;
    QAction *aCollectionModelFullMode, *aCollectionModelSimpleMode;
    QAction *aToggleSelector, *aClear, *aRemoveRow, *aPrint, *aPrintPreview;
    QPointer<IcdCentralWidget> m_CurrentView;
    QPointer<IcdDownloader> m_Downloader;
};

}  // End Internal
}  // End DrugsWidget


namespace ICD {
class IcdPlugin;

class ICD_EXPORT IcdWidgetManager : public Internal::IcdActionHandler
{
    Q_OBJECT
    friend class ICD::IcdPlugin;

public:
    static IcdWidgetManager *instance();
    ~IcdWidgetManager() {}

    IcdCentralWidget *currentView() const;

protected:
    void refreshViews();


private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);

private:
    IcdWidgetManager(QObject *parent = 0);
    static IcdWidgetManager *m_Instance;
};

}  // End namespace ICD

#endif // ICDWIDGETMANAGER_H
