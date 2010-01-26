/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mfPrescriptionViewer.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidget/druginfo.h>
#include <drugsplugin/drugswidget/mfInteractionDialog.h>
#include <drugsplugin/dosagedialog/mfDosageCreatorDialog.h>
#include <drugsplugin/dosagedialog/mfDosageDialog.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/drugswidget/textualprescriptiondialog.h>

#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsmodel.h>

#include <translationutils/constanttranslations.h>

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <fdmainwindowplugin/mainwindow.h>

#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>

using namespace DrugsWidget;
using namespace DrugsWidget::Constants;
using namespace Trans::ConstantTranslations;

inline static DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}


/** \brief Constructor. You must call initialize() after instanciation */
PrescriptionViewer::PrescriptionViewer(QWidget *parent) :
        QWidget(parent),
        m_ToolBar(0)
{
    setObjectName("PrescriptionViewer");
    setupUi(this);
}

/** \brief Creates actions, popup menus and toolbars */
void PrescriptionViewer::initialize()
{
    createActionsAndToolbar();
    verticalLayout->insertWidget(0, m_ToolBar);
    listView->setObjectName("PrescriptionListView");
    setListViewPadding(5);
    listView->setAcceptDrops(true);
    listView->setDragDropMode(QAbstractItemView::DropOnly);
    listView->setDropIndicatorShown(true);
//    this->setAcceptDrops(true);
//    listView->setMovement(QListView::Snap);
}

/** \brief Defines the model to use. */
void PrescriptionViewer::setModel(DrugsDB::DrugsModel *model)
{
    Q_ASSERT_X(model, "PrescriptionViewer::setModel", "Model must be correctly setted");
    listView->setModel(model);
}

/** \brief Defines the model's column to use. */
void PrescriptionViewer::setModelColumn(const int col)
{
    Q_ASSERT_X(static_cast<DrugsDB::DrugsModel*>(listView->model()), "PrescriptionViewer::setModelColumn", "model is not a mfDrugsModel*");
    listView->setModelColumn(col);
}

void PrescriptionViewer::setListViewPadding(const int pad)
{
//    listView->setSpacing(pad);
    listView->setStyleSheet(QString("QListView#PrescriptionListView:item { padding: %1px; }").arg(pad));
}

/** \brief Creates actions and toolbar */
void PrescriptionViewer::createActionsAndToolbar()
{
    Core::Command *cmd = 0;
    // populate toolbar
    m_ToolBar = new QToolBar(this);
#if QT_VERSION < 0x040600
        m_ToolBar->setIconSize(QSize(16,16));
#endif
    QStringList actionsToAdd;
    actionsToAdd
            << Core::Constants::A_FILE_OPEN
            << Core::Constants::A_FILE_SAVE
            << Core::Constants::A_FILE_SAVEAS
            << Core::Constants::A_TEMPLATE_CREATE
            << DrugsWidget::Constants::A_PRINT_PRESCRIPTION;

    foreach(const QString &s, actionsToAdd) {
        cmd = actionManager()->command(s);
        m_ToolBar->addAction(cmd->action());
    }

    actionsToAdd.clear();
    actionsToAdd
            << Core::Constants::A_LIST_CLEAR
            << Core::Constants::A_LIST_REMOVE
            << Core::Constants::A_LIST_MOVEDOWN
            << Core::Constants::A_LIST_MOVEUP
            << Core::Constants::A_LIST_SORT
            ;
    m_ToolBar->addSeparator();
    foreach(const QString &s, actionsToAdd) {
        cmd = actionManager()->command(s);
        m_ToolBar->addAction(cmd->action());
    }
    m_ToolBar->addSeparator();

    actionsToAdd.clear();
    actionsToAdd
            << DrugsWidget::Constants::A_TOOGLE_TESTINGDRUGS
            << Core::Constants::A_VIEW_INTERACTIONS
            << Core::Constants::A_CHANGE_DURATION;

    foreach(const QString &s, actionsToAdd) {
        cmd = actionManager()->command(s);
        m_ToolBar->addAction(cmd->action());
        m_ToolBar->addSeparator();
    }
    m_ToolBar->setFocusPolicy(Qt::ClickFocus);
}

/** \brief Clears the prescription */
void PrescriptionViewer::clearTriggered()
{
    Q_ASSERT_X(static_cast<DrugsDB::DrugsModel*>(listView->model()), "PrescriptionViewer", "model is not a DrugsModel*");
    static_cast<DrugsDB::DrugsModel*>(listView->model())->clearDrugsList();
}

/** \brief Remove the selected drug from the prescription.*/
void PrescriptionViewer::removeTriggered()
{
    Q_ASSERT_X(static_cast<DrugsDB::DrugsModel*>(listView->model()), "PrescriptionViewer", "model is not a DrugsModel*");
    if (!listView->selectionModel()->hasSelection())
        return;
    const QModelIndexList &list = listView->selectionModel()->selectedRows(0);
    foreach(const QModelIndex &index, list) {
        static_cast<DrugsDB::DrugsModel*>(listView->model())->removeRows(index.row(), 1);
    }
}

/** \brief Moves the selected drug up. */
void PrescriptionViewer::moveUp()
{
    Q_ASSERT_X(static_cast<DrugsDB::DrugsModel*>(listView->model()), "mfPrescriptionViewer", "model is not a mfDrugsModel*");
    DrugsDB::DrugsModel *m = static_cast<DrugsDB::DrugsModel*>(listView->model());
    int row = listView->currentIndex().row();
    m->moveUp(listView->currentIndex());
    listView->setCurrentIndex(m->index(row-1,0));
//    checkMovableItem(m->index(row-1,0));
}

/** \brief Move the selected drug down. */
void PrescriptionViewer::moveDown()
{
    Q_ASSERT_X(static_cast<DrugsDB::DrugsModel*>(listView->model()), "mfPrescriptionViewer", "model is not a mfDrugsModel*");
    DrugsDB::DrugsModel *m = static_cast<DrugsDB::DrugsModel*>(listView->model());
    int row = listView->currentIndex().row();
    m->moveDown(listView->currentIndex());
    listView->setCurrentIndex(m->index(row+1,0));
//     checkMovableItem(m->index(row+1,0));
}

/** \brief Sorts the drugs of the prescription. \sa mfDrugsModel::sort() */
void PrescriptionViewer::sortDrugs()
{
    listView->model()->sort(0);
}

/** \brief Opens the drug information dialog. \sa mfDrugInfo */
void PrescriptionViewer::showDrugInfo(const QModelIndex &item)
{
    Internal::DrugInfo di(item.row(), this);
    di.exec();
}

/** \brief Opens the mfDosageDialog for the selected drug. */
void PrescriptionViewer::showDosageDialog(const QModelIndex &item)
{
    int CIS = drugModel()->index(item.row(), DrugsDB::Constants::Drug::CIS).data().toInt();
    bool isTextual = drugModel()->index(item.row(), DrugsDB::Constants::Prescription::IsTextualOnly).data().toBool();
    int row = item.row();
    if (CIS!=-1) {
        Internal::DosageDialog dlg(this);
        dlg.changeRow(CIS, row);
        dlg.exec();
    } else if (isTextual) {
        TextualPrescriptionDialog dlg(this);
        dlg.setDrugLabel(drugModel()->index(row,DrugsDB::Constants::Drug::Denomination).data().toString());
        dlg.setDrugNote(drugModel()->index(row,DrugsDB::Constants::Prescription::Note).data().toString());
        dlg.setALD(drugModel()->index(row,DrugsDB::Constants::Prescription::IsALD).data().toBool());
        int r = dlg.exec();
        if (r==QDialog::Accepted) {
            drugModel()->setData(drugModel()->index(row, DrugsDB::Constants::Drug::Denomination), dlg.drugLabel());
            drugModel()->setData(drugModel()->index(row, DrugsDB::Constants::Prescription::Note), dlg.drugNote());
            drugModel()->setData(drugModel()->index(row, DrugsDB::Constants::Prescription::IsALD), dlg.isALD());
        }
    }
    // This is used to force listView to redraw all rows with the good height is user changes note or whatever
    listView->setViewMode(QListView::ListMode);
}

/** \brief Opens the InteractionDialog. */
void PrescriptionViewer::viewInteractions()
{
     Internal::InteractionDialog dlg(this);
     dlg.resize(Core::ICore::instance()->mainWindow()->size());
     dlg.exec();
}

/** \brief Presents a QMenu to the user, and change duration of all drugs in the prescription */
void PrescriptionViewer::changeDuration()
{
    QMenu *root = new QMenu(this);
    QStringList subs = QStringList()
                       << Trans::Constants::DAYS
                       << Trans::Constants::WEEKS
                       << Trans::Constants::MONTHS
                       << Trans::Constants::QUARTERS;
    QList<int> quantity = QList<int>() << 31 << 15 << 12 << 4;
    int i = 0;
    foreach(const QString &s, subs) {
        QMenu *submenu = new QMenu(tkTr(s.toAscii()), root);
        root->addMenu(submenu);
        int j = quantity[i];
        for(int z=0; z<j;++z) {
            QAction *a = submenu->addAction(QString::number(z+1));
            a->setObjectName(tkTr(s.toAscii())+":"+QString::number(z+1));
            connect(a,SIGNAL(triggered()), this, SLOT(changeDurationTo()));
        }
        ++i;
    }
    QAction *a = actionManager()->command(Core::Constants::A_CHANGE_DURATION)->action();
    root->popup(mapToGlobal(m_ToolBar->actionGeometry(a).center()));
}

/** \brief Changes all drugs duration according to the triggered action. \sa PrescriptionViewer::changeDuration(). */
void PrescriptionViewer::changeDurationTo()
{
    QAction *a = qobject_cast<QAction*>(sender());
    if (!a)
        return;
    QString scheme = a->objectName().left(a->objectName().indexOf(":"));
    int duration = a->objectName().mid(a->objectName().indexOf(":")+1).toInt();
    int nb = drugModel()->rowCount();
    for(int i=0;i<nb;++i) {
        QModelIndex idx = drugModel()->index(i, DrugsDB::Constants::Prescription::DurationScheme);
        drugModel()->setData(idx, scheme);
        idx = drugModel()->index(i, DrugsDB::Constants::Prescription::DurationFrom);
        drugModel()->setData(idx, duration);
        idx = drugModel()->index(i, DrugsDB::Constants::Prescription::DurationUsesFromTo);
        drugModel()->setData(idx, false);
    }
}

/** \brief Returns the listView in use for the prescription. */
QListView *PrescriptionViewer::listview()
{
    return listView;
}

/** \brief Used for translations. \internal */
void PrescriptionViewer::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
//        retranslateActions();
        break;
    default:
        break;
    }
}
