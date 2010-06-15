/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include <coreplugin/dialogs/settingsdialog.h>

#include <fdmainwindowplugin/mainwindow.h>

#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>

using namespace DrugsWidget;
using namespace DrugsWidget::Constants;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsModel *drugModel() { return DrugsDB::DrugsModel::activeModel(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}


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
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
//    this->setAcceptDrops(true);
//    listView->setMovement(QListView::Snap);
}

/** \brief Defines the model to use. */
void PrescriptionViewer::setModel(DrugsDB::DrugsModel *model)
{
    Q_ASSERT_X(model, "PrescriptionViewer::setModel", "Model must be correctly setted");
    listView->setModel(model);
    listView->setModelColumn(DrugsDB::Constants::Drug::FullPrescription);
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
#ifdef FREEMEDFORMS
            << Constants::A_TOGGLE_DRUGSELECTOR
#endif
            << Core::Constants::A_FILE_OPEN
            << Core::Constants::A_FILE_SAVE
            << Core::Constants::A_FILE_SAVEAS
            << Core::Constants::A_TEMPLATE_CREATE
            << Core::Constants::A_FILE_PRINTPREVIEW
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
            << DrugsWidget::Constants::A_TOGGLE_TESTINGDRUGS
            << DrugsWidget::Constants::A_VIEW_INTERACTIONS
            << DrugsWidget::Constants::A_CHANGE_DURATION;

    foreach(const QString &s, actionsToAdd) {
        cmd = actionManager()->command(s);
        m_ToolBar->addAction(cmd->action());
        m_ToolBar->addSeparator();
    }

    m_ToolBar->setFocusPolicy(Qt::ClickFocus);
}

/** \brief create the context menu on the precription listView */
void PrescriptionViewer::on_listView_customContextMenuRequested(const QPoint &)
{
    if (!drugModel()->rowCount())
        return;

    QMenu *pop = new QMenu(this);
    QStringList actionsToAdd;
    actionsToAdd
            << DrugsWidget::Constants::A_OPENDOSAGEDIALOG
            << DrugsWidget::Constants::A_OPENDOSAGEPREFERENCES
            << DrugsWidget::Constants::A_RESETPRESCRIPTIONSENTENCE_TODEFAULT
            << DrugsWidget::Constants::A_CHANGE_DURATION;

    Core::Command *cmd = 0;
    foreach(const QString &s, actionsToAdd) {
        cmd = actionManager()->command(s);
        pop->addAction(cmd->action());
    }
    pop->exec(QCursor::pos());
    delete pop;
    pop = 0;
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
    int row;
    if (!item.isValid()) {
        row = listView->currentIndex().row();
    } else {
        row = item.row();
    }

    if (row < 0)
        return;

    int UID = drugModel()->index(row, DrugsDB::Constants::Drug::UID).data().toInt();
    bool isTextual = drugModel()->index(row, DrugsDB::Constants::Prescription::IsTextualOnly).data().toBool();
    if (UID!=-1) {
        Internal::DosageDialog dlg(this);
        dlg.changeRow(UID, row);
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
    QPoint pos;
    QString senderTag;
    // get sender --> if null --> drugsmanager drugsactionhandler
    if (sender()) {
        senderTag = "%ù";
        pos = QCursor::pos();
    } else {
        // get the position of the caller
        QAction *a = actionManager()->command(DrugsWidget::Constants::A_CHANGE_DURATION)->action();
        pos = mapToGlobal(m_ToolBar->actionGeometry(a).center());
        senderTag.clear();
    }

    // create the pop menu
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
            a->setObjectName(tkTr(s.toAscii())+":"+QString::number(z+1)+senderTag);
            connect(a,SIGNAL(triggered()), this, SLOT(changeDurationTo()));
        }
        ++i;
    }

    root->popup(pos);
}

/** \brief Changes all drugs duration according to the triggered action. \sa PrescriptionViewer::changeDuration(). */
void PrescriptionViewer::changeDurationTo()
{
    QAction *a = qobject_cast<QAction*>(sender());
    if (!a)
        return;

    QString name = a->objectName().remove("%ù");
    QString scheme = name.left(name.indexOf(":"));
    int duration = name.mid(name.indexOf(":")+1).toInt();

    int i = 0;
    int nb = 0;
    if (a->objectName().contains("%ù")) {
        i = listView->currentIndex().row();
        nb = i + 1;
    } else {
        i = 0;
        nb = drugModel()->rowCount();
    }

    for(i ; i<nb ; ++i) {
        QModelIndex idx = drugModel()->index(i, DrugsDB::Constants::Prescription::DurationScheme);
        drugModel()->setData(idx, scheme);
        idx = drugModel()->index(i, DrugsDB::Constants::Prescription::DurationFrom);
        drugModel()->setData(idx, duration);
        idx = drugModel()->index(i, DrugsDB::Constants::Prescription::DurationUsesFromTo);
        drugModel()->setData(idx, false);
    }
}

/** \brief Opens the protocols preferences page */
void PrescriptionViewer::openProtocolPreferencesDialog()
{
    Core::SettingsDialog dlg(this, tkTr(Trans::Constants::DRUGS), "DrugsPrintOptionsPage");
    dlg.exec();
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
