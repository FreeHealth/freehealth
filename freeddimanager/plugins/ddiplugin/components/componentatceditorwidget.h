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
#ifndef DDIMANAGER_DDIPLUGIN_COMPONENTATCEDITORWIDGET_H
#define DDIMANAGER_DDIPLUGIN_COMPONENTATCEDITORWIDGET_H

#include <QWidget>
#include <QModelIndex>

/**
 * \file componentatceditorwidget.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 21 Apr 2014
*/

namespace DDI {
namespace Internal {
class ComponentAtcEditorWidgetPrivate;

class ComponentAtcEditorWidget : public QWidget
{
    Q_OBJECT
    friend class DDI::Internal::ComponentAtcEditorWidgetPrivate;

public:
    explicit ComponentAtcEditorWidget(QWidget *parent = 0);
    ~ComponentAtcEditorWidget();

//    void processCSVFile();

protected Q_SLOTS:
    void onChangeComponentDrugDatabaseUidRequested(const int index);
    void onComponentViewItemActivated(const QModelIndex &index);
    void onComponentViewItemChanged(const QModelIndex &current, const QModelIndex &previous);
    void onComponentViewItemPressed(const QModelIndex &index);
    void onActionButtonTriggered(QAction *a);
    void onCreateUnreviewedFileRequested();
    void onRemoveUnreviewedRequested();
    void onFilterUnreviewedStateChanged();
    void onModelReset();
    void saveModel();

protected:
    void translateActions();
    void showEvent(QShowEvent *e);
    void changeEvent(QEvent *e);

#ifdef WITH_TESTS
public Q_SLOTS:
    void test_runAllTests();
#endif

private:
    ComponentAtcEditorWidgetPrivate *d;
};

} // namespace Internal
} // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_COMPONENTATCEDITORWIDGET_H
