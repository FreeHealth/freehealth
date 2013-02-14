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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "prescriptionviewer.h"

#include <drugsplugin/constants.h>
#include <drugsplugin/drugswidget/druginfo.h>
#include <drugsplugin/dosagedialog/mfDosageCreatorDialog.h>
#include <drugsplugin/dosagedialog/mfDosageDialog.h>
#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/drugswidget/textualprescriptiondialog.h>
#include <drugsplugin/drugswidget/interactionsynthesisdialog.h>

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/drugsio.h>

#include <coreplugin/constants_menus.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/dialogs/settingsdialog.h>
#include <coreplugin/dialogs/simpletextdialog.h>

#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QTreeWidget>
#include <QClipboard>
#include <QMimeData>

#include <QDebug>

using namespace DrugsWidget;
using namespace DrugsWidget::Constants;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsIO &drugsIo() {return DrugsDB::DrugBaseCore::instance().drugsIo();}
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}

/** \brief Constructor. You must call initialize() after instanciation */
PrescriptionViewer::PrescriptionViewer(QWidget *parent) :
    QWidget(parent),
    m_ToolBar(0),
    m_DrugsModel(0)
{
    setObjectName("PrescriptionViewer");
    setupUi(this);
}

/** \brief Creates actions, popup menus and toolbars */
void PrescriptionViewer::initialize()
{
    createActionsAndToolbar();

    // TODO: don't insert here, better in DrugSelector?
    // then the toggle button is *above* the DrugSelector and doesn't jump up/down
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
    Q_ASSERT_X(model, "PrescriptionViewer::setModel", "Model must be set correctly");
    m_DrugsModel = model;
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

#ifdef FREEMEDFORMS
    actionsToAdd
            << Constants::A_TOGGLE_DRUGSELECTOR
            << Core::Constants::A_FILE_OPEN
            << Constants::A_SAVE_PRESCRIPTION
            << Core::Constants::A_TEMPLATE_CREATE
            << Core::Constants::A_FILE_PRINTPREVIEW
            << DrugsWidget::Constants::A_PRINT_PRESCRIPTION;
#else
    actionsToAdd
            << Core::Constants::A_FILE_OPEN
            << Core::Constants::A_FILE_SAVE
            << Core::Constants::A_FILE_SAVEAS
            << Core::Constants::A_TEMPLATE_CREATE
            << Core::Constants::A_FILE_PRINTPREVIEW
            << DrugsWidget::Constants::A_PRINT_PRESCRIPTION;
#endif
    actionsToAdd << "--";
    actionsToAdd
            << DrugsWidget::Constants::A_CLEAR_PRESCRIPTION
            << Core::Constants::A_LIST_REMOVE
            << Core::Constants::A_LIST_MOVEDOWN
            << Core::Constants::A_LIST_MOVEUP
            << Core::Constants::A_LIST_SORT
            ;
    actionsToAdd << "--";
    actionsToAdd
            << DrugsWidget::Constants::A_TOGGLE_TESTINGDRUGS
            << DrugsWidget::Constants::A_VIEW_INTERACTIONS
            << DrugsWidget::Constants::A_CHANGE_DURATION
            << DrugsWidget::Constants::A_SHOWDRUGPRECAUTIONS;

    foreach(const QString &s, actionsToAdd) {
        if (s=="--") {
            m_ToolBar->addSeparator();
            continue;
        }
        cmd = actionManager()->command(Core::Id(s));
        if (cmd)
            m_ToolBar->addAction(cmd->action());
    }
    m_ToolBar->setFocusPolicy(Qt::ClickFocus);
}

/** \brief create the context menu on the precription listView */
void PrescriptionViewer::on_listView_customContextMenuRequested(const QPoint &)
{
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return;
    if (!m_DrugsModel->rowCount())
        return;

    QMenu *pop = new QMenu(this);
    QStringList actionsToAdd;
    actionsToAdd
            << DrugsWidget::Constants::A_COPYPRESCRIPTIONITEM
            << DrugsWidget::Constants::A_OPENDOSAGEDIALOG
            << DrugsWidget::Constants::A_OPENDOSAGEPREFERENCES
            << DrugsWidget::Constants::A_RESETPRESCRIPTIONSENTENCE_TODEFAULT
            << DrugsWidget::Constants::A_CHANGE_DURATION;

    Core::Command *cmd = 0;
    foreach(const QString &s, actionsToAdd) {
        cmd = actionManager()->command(Core::Id(s));
        pop->addAction(cmd->action());
    }
    pop->exec(QCursor::pos());
    delete pop;
    pop = 0;
}

/** Save the prescription */
bool PrescriptionViewer::savePrescription()
{
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return false;
    QHash<QString, QString> extra;
    return drugsIo().savePrescription(m_DrugsModel, extra);
}

/** Save the prescription, asking a filename to the user */
bool PrescriptionViewer::saveAsPrescription()
{
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return false;
    QHash<QString, QString> extra;
    return drugsIo().savePrescription(m_DrugsModel, extra);
}

/** Clears the prescription (remove all drugs in the drugs model) */
void PrescriptionViewer::clear()
{
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return;
    m_DrugsModel->clearDrugsList();
    m_DrugsModel->setModified(false);
}

/** Removes the selected drug from the prescription.*/
void PrescriptionViewer::removeTriggered()
{
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return;
    if (!listView->selectionModel())
        return;
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
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return;
    int row = listView->currentIndex().row();
    m_DrugsModel->moveUp(listView->currentIndex());
    listView->setCurrentIndex(m_DrugsModel->index(row-1,0));
//    checkMovableItem(m_DrugsModel->index(row-1,0));
}

/** \brief Move the selected drug down. */
void PrescriptionViewer::moveDown()
{
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return;
    int row = listView->currentIndex().row();
    m_DrugsModel->moveDown(listView->currentIndex());
    listView->setCurrentIndex(m_DrugsModel->index(row+1,0));
//     checkMovableItem(m_DrugsModel->index(row+1,0));
}

/** \brief Sorts the drugs of the prescription. \sa mfDrugsModel::sort() */
void PrescriptionViewer::sortDrugs()
{
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return;
    m_DrugsModel->sort(0);
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
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return;
    if (m_DrugsModel->isSelectionOnlyMode())
        return;

    int row;
    if (!item.isValid())
        row = listView->currentIndex().row();
    else
        row = item.row();
    if (row < 0)
        return;

    const QVariant &drugUid = m_DrugsModel->index(row, DrugsDB::Constants::Drug::DrugId).data();
    bool isTextual = m_DrugsModel->index(row, DrugsDB::Constants::Prescription::IsTextualOnly).data().toBool();
    if (isTextual) {
        TextualPrescriptionDialog dlg(this);
        dlg.setDrugLabel(m_DrugsModel->index(row,DrugsDB::Constants::Drug::Denomination).data().toString());
        dlg.setDrugNote(m_DrugsModel->index(row,DrugsDB::Constants::Prescription::Note).data().toString());
        dlg.setALD(m_DrugsModel->index(row,DrugsDB::Constants::Prescription::IsALD).data().toBool());
        int r = dlg.exec();
        if (r==QDialog::Accepted) {
            m_DrugsModel->setData(m_DrugsModel->index(row, DrugsDB::Constants::Drug::Denomination), dlg.drugLabel());
            m_DrugsModel->setData(m_DrugsModel->index(row, DrugsDB::Constants::Prescription::Note), dlg.drugNote());
            m_DrugsModel->setData(m_DrugsModel->index(row, DrugsDB::Constants::Prescription::IsALD), dlg.isALD());
        }
    } else if (drugUid.toInt()!=-1 && !drugUid.isNull()) {
        Internal::DosageDialog dlg(this);
        dlg.changeRow(drugUid, row);
        dlg.exec();
    }
    // This is used to force listView to redraw all rows with the good height if user changes note or whatever
    listView->setViewMode(QListView::ListMode);
}

/** \brief Opens the InteractionDialog. */
void PrescriptionViewer::viewInteractions()
{
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return;
    InteractionSynthesisDialog dlg(m_DrugsModel, this);
    Utils::resizeAndCenter(&dlg, Core::ICore::instance()->mainWindow());
    dlg.exec();

//    Core::SimpleTextDialog dlg(tr("Synthetic interactions") + " - " + qApp->applicationName(),
//                               Constants::S_INTERACTIONVIEW_ZOOM,
//                               Core::ICore::instance()->mainWindow());
//    dlg.setHtml(drugModel()->index(0, DrugsDB::Constants::Interaction::FullSynthesis).data().toString());
//    dlg.setPrintDuplicata(true);
//    dlg.setUserPaper(Core::IDocumentPrinter::Papers_Prescription_User);
//    dlg.setHelpPageUrl("iamtesteur.html#synthetiseur_iam");
//    dlg.exec();
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

    // create the popup menu
    QMenu *root = new QMenu(this);
    QStringList subs = QStringList()
                       << Trans::Constants::DAY_S
                       << Trans::Constants::WEEK_S
                       << Trans::Constants::MONTH_S
                       << Trans::Constants::QUARTER_S;
    QList<int> quantity = QList<int>() << 31 << 15 << 12 << 4;
    int i = 0;
    foreach(const QString &s, subs) {
        QMenu *submenu = new QMenu(tkTr(s.toUtf8()), root);
        root->addMenu(submenu);
        int j = quantity[i];
        for(int z=0; z<j;++z) {
            QAction *a = submenu->addAction(QString::number(z+1));
            a->setObjectName(tkTr(s.toUtf8())+":"+QString::number(z+1)+senderTag);
            connect(a,SIGNAL(triggered()), this, SLOT(changeDurationTo()));
        }
        ++i;
    }

    root->popup(pos);
}

/*! \brief Changes all drugs duration according to the triggered action. \sa PrescriptionViewer::changeDuration(). */
void PrescriptionViewer::changeDurationTo()
{
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return;
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
        nb = m_DrugsModel->rowCount();
    }

    for(int j = i ; j<nb ; ++j) {
        QModelIndex idx = m_DrugsModel->index(j, DrugsDB::Constants::Prescription::DurationScheme);
        m_DrugsModel->setData(idx, scheme);
        idx = m_DrugsModel->index(j, DrugsDB::Constants::Prescription::DurationFrom);
        m_DrugsModel->setData(idx, duration);
        idx = m_DrugsModel->index(j, DrugsDB::Constants::Prescription::DurationUsesFromTo);
        m_DrugsModel->setData(idx, false);
    }
}

/** \brief Opens the protocols preferences page */
void PrescriptionViewer::openProtocolPreferencesDialog()
{
    Core::SettingsDialog dlg(this, tkTr(Trans::Constants::DRUGS), "DrugsPrintOptionsPage");
    dlg.exec();
}

void PrescriptionViewer::copyPrescriptionItem()
{
    Q_ASSERT(m_DrugsModel);
    if (!m_DrugsModel)
        return;
    if (!listView->selectionModel())
        return;
    if (!listView->selectionModel()->hasSelection())
        return;
    QModelIndexList list = listView->selectionModel()->selectedRows();
    qSort(list);
    QString html;
    for(int i=0; i < list.count() ; ++i) {
        int row = list.at(i).row();
        QModelIndex idx = m_DrugsModel->index(row, DrugsDB::Constants::Prescription::ToHtml);
        html += idx.data().toString();
    }
    QMimeData *data = new QMimeData;
    data->setHtml(html);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(data);
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
