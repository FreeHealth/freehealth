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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGIN_INTERACTOREDITORWIDGET_H
#define DDIMANAGER_DDIPLUGIN_INTERACTOREDITORWIDGET_H

#include <ddiplugin/ddi_exporter.h>
#include <QWidget>
QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/**
 * \file interactoreditorwidget.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 12 Oct 2013
*/

namespace DDI {
namespace Internal {
class InteractorEditorWidgetPrivate;
}  // namespace Internal

class DDI_EXPORT InteractorEditorWidget : public QWidget
{
    Q_OBJECT
public:
    InteractorEditorWidget(QWidget *parent = 0);
    ~InteractorEditorWidget();

private:
    void setEditorsEnabled(bool state);

private Q_SLOTS:
    void save();
    void filterDrugInteractorModel(const QString &text);
    void onNewInteractorRequested();
    void removeCurrent();
    void editCurrent();
    void revertEdition();
    void interactorActivated(const QModelIndex &index);
    void searchButtonActivated(QAction*);
    void toggleClassMolsFilter(QAction*);
    void updateCounts();
    void nextUnreviewedOrUnlinked();
    void onCreatePdfOutputRequested();

private:
    void retranslateUi();
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void onCoreDatabaseChanged();

#ifdef WITH_TESTS
public Q_SLOTS:
    void test_runAllTests();
    void test_views();
    void test_actions();
    void test_itemCreation();
    void test_edition();
#endif

private:
    Internal::InteractorEditorWidgetPrivate *d;
};

}  // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_INTERACTOREDITORWIDGET_H
