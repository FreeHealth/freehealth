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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PRESCRIPTIONVIEWER_H
#define PRESCRIPTIONVIEWER_H

#include <drugsplugin/drugs_exporter.h>

#include <QWidget>
#include <QAction>
#include <QToolBar>
#include <QListView>

#include "ui_prescriptionviewer.h"

/**
 * \file prescriptionviewer.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 13 Feb 2013
*/

namespace DrugsDB {
class DrugsModel;
}

namespace DrugsWidget {

/**
  \brief This widget is the prescription viewer.
  It deals data with a DrugsDB::DrugsModel. The model MUST BE SET using setModel() and setModelColumn().
*/
class DRUGS_EXPORT PrescriptionViewer : public QWidget, private Internal::Ui::PrescriptionViewer
{
    Q_OBJECT
    Q_DISABLE_COPY(PrescriptionViewer)

public:
    explicit PrescriptionViewer(QWidget *parent = 0);
    void initialize();

    QListView *listview();
    void setModel(DrugsDB::DrugsModel *model);
    void setListViewPadding(const int pad);

protected:
    virtual void changeEvent(QEvent *e);

private:
    void createActionsAndToolbar();

public Q_SLOTS:
    bool savePrescription();
    bool saveAsPrescription();

    void clear();
    void removeTriggered();
    void moveUp();
    void moveDown();
    void sortDrugs();
    void showDrugInfo(const QModelIndex &item);
    void showDosageDialog(const QModelIndex &item = QModelIndex());
    void viewInteractions();
    void changeDuration();
    void changeDurationTo();
    void openProtocolPreferencesDialog();
    void copyPrescriptionItem();

private Q_SLOTS:
    void on_listView_customContextMenuRequested(const QPoint &pos);

private:
    QToolBar *m_ToolBar;
    DrugsDB::DrugsModel *m_DrugsModel;
};

}  // End DrugsWidget

#endif // PRESCRIPTIONVIEWER_H
