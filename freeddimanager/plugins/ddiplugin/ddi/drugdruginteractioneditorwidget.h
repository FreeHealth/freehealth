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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGIN_DrugDrugInteractionEditorWidget_H
#define DDIMANAGER_DDIPLUGIN_DrugDrugInteractionEditorWidget_H

#include <QWidget>
#include <QModelIndex>

/**
 * \file drugdruginteractioneditorwiddet.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 14 Oct 2013
*/

namespace DDI {
namespace Internal {
class DrugDrugInteractionEditorWidgetPrivate;
}

class DrugDrugInteractionEditorWidget : public QWidget
{
    Q_OBJECT
public:
    DrugDrugInteractionEditorWidget(QWidget *parent = 0);
    ~DrugDrugInteractionEditorWidget();

protected Q_SLOTS:
    void createNewDDI();
    void filterDrugDrugInteractionTableModel(const QString &filter);
    void edit();
    void revertCurrentEdition();
    void interactionActivated(const QModelIndex &index);
    void save();
    void removeCurrent();
    void onNextActionTriggered(QAction*);
    void splitCurrent();
    void onCheckInteractionErrorsRequested();
    void updateDatabaseStats();

private:
    void setEditorsEnabled(bool state);
    void retranslateUi();

protected:
    void changeEvent(QEvent *e);

#ifdef WITH_TESTS
public Q_SLOTS:
    void test_runAllTests();
    void test_views();
    void test_editorEnabledState(bool state);
    void test_actions();
    void test_itemCreation();
    void test_edition();
#endif // WITH_TESTS

private:
    Internal::DrugDrugInteractionEditorWidgetPrivate *d;
};

}  // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_DrugDrugInteractionEditorWidget_H
