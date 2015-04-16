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
#ifndef DRUGSWIDGETMANAGERMANAGER_H
#define DRUGSWIDGETMANAGERMANAGER_H

#include <drugsplugin/drugs_exporter.h>

#include <drugsplugin/drugswidget/drugscentralwidget.h>
#include <drugsbaseplugin/drugsmodel.h>

#include <coreplugin/contextmanager/icontext.h>

#include <QWidget>
#include <QObject>
#include <QAction>
#include <QPointer>
#include <QDockWidget>

/**
 * \file drugswidgetmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 27 Sept 2011
 * \internal
*/

namespace DrugsWidget {
namespace Internal {

class DrugsContext : public Core::IContext
{
public:
    DrugsContext(DrugsCentralWidget *w) : Core::IContext(w)
    {
        setObjectName("DrugsContext");
        setWidget(w);
    }
};

class DRUGS_EXPORT DrugsActionHandler : public QObject
{
    Q_OBJECT
public:
    enum Modes {
        Prescriber = 0,
        SelectOnly
    };

    DrugsActionHandler(QObject *parent = 0);
    virtual ~DrugsActionHandler() {}

    void setCurrentView(DrugsCentralWidget *view);
    void setEditMode(const Modes mode);
    Modes editMode() const;


public Q_SLOTS:
    void drugsModelChanged();

private Q_SLOTS:
    void onDrugsBaseChanged();
    void savePrescription();
    void saveAsPrescription();
    void toggleDrugSelector();
    void moveUp();
    void moveDown();
    void sortDrugs();
    void removeItem();
    void clear();
    void viewInteractions();
    void searchTriggered();
    void listViewItemChanged();
    void searchActionChanged(QAction *a);
    void printPrescription();
    void toggleTestingDrugs();
    void changeDuration();
    void createTemplate();
    void printPreview();

    void showDrugsDatabaseInformation();
    void showDosagesDatabaseInformation();

    void modeActionChanged(QAction *a);
    void openDosageDialog();
    void openProtocolPreferencesDialog();
    void resetPrescriptionSentenceToDefault();

    void toggleDrugPrecautions();

    void copyPrescriptionItem();

private:
    bool canMoveUp() const;
    bool canMoveDown() const;
    void updateActions();

protected:
    QAction *aSavePrescription, *aSaveAsPrescription;
    QAction *aToggleDrugSelector;
    QAction *aAddRow;
    QAction *aRemoveRow;
    QAction *aDown;
    QAction *aUp;
    QAction *aSort;
    QAction *aEdit;
    QAction *aClear;
    QAction *aViewInteractions;
    QAction *aSearch;
    QActionGroup *gSearchMethod;
    QAction *aSearchCommercial;
    QAction *aSearchMolecules;
    QAction *aSearchInn;
    QAction *aPrintPrescription;
    QAction *aPrintPreview;
    QAction *aToggleTestingDrugs;
    QAction *aChangeDuration;
    QAction *aToTemplate;
    QAction *aDrugsDatabaseInformation;
    QAction *aDosagesDatabaseInformation;
    QActionGroup *gModes;
    QAction *aPrescriberMode;
    QAction *aSelectOnlyMode;
    QAction *aOpenDosageDialog;
    QAction *aOpenPrescriptionSentencePreferences;
    QAction *aResetPrescriptionSentenceToDefault;
    QAction *aToggleDrugPrecautions;
    QAction *aCopyPrescriptionItem;

    QPointer<DrugsCentralWidget> m_CurrentView;

    bool m_SelectionOnlyMode;

    QDockWidget *m_PrecautionDock;
};

}  // End Internal
}  // End DrugsWidget


namespace DrugsWidget {

class DRUGS_EXPORT DrugsWidgetManager : public Internal::DrugsActionHandler
{
    Q_OBJECT
public:
    static DrugsWidgetManager *instance();
    ~DrugsWidgetManager() {}

    DrugsWidget::DrugsCentralWidget  *currentView() const;
    DrugsDB::DrugsModel              *currentDrugsModel() const { return DrugsDB::DrugsModel::activeModel(); }

private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);

private:
    DrugsWidgetManager(QObject *parent = 0);
    static DrugsWidgetManager *m_Instance;
};

}  // End DrugsWidget

#endif // DRUGSWIDGETMANAGERMANAGER_H
