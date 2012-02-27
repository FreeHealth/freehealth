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
 *   Main Developer : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class PMH::PmhMode
  \brief Core::IMode to show in the MainWindow
*/

#include "pmhmode.h"
#include "pmhmodel.h"
#include "pmhcore.h"
#include "pmhcategorymodel.h"
#include "pmhcreatordialog.h"
#include "pmhviewer.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <categoryplugin/categoryitem.h>
#include <categoryplugin/categorydialog.h>

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QItemSelectionModel>
#include <QToolBar>
#include <QPushButton>
#include <QScrollBar>

#include "ui_pmhmodewidget.h"


using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }

static inline PmhCore *pmhCore() {return PmhCore::instance();}
static inline PmhCategoryModel *catModel() {return PmhCore::instance()->pmhCategoryModel();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}

namespace {

const char * const TREEVIEW_SHEET =
        " QTreeView {"
        "    show-decoration-selected: 1;"
        "}"

        "QTreeView::item {"
        "    border: 0px solid #d9d9d9;"
//        "    border-top-color: transparent;"
//        "    border-bottom-color: transparent;"
        "}"

        "QTreeView::item:hover {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
//        "    border: 1px solid #bfcde4;"
        "}"

//        "QTreeView::branch:hover {"
//        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
//        "    border: 0px solid #bfcde4;"
//        "}"

//        "QTreeView::item:selected {"
//        "    border: 1px solid #567dbc;"
//        "}"

        "QTreeView::item:selected {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"

//        "QTreeView::branch:selected {"
//        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
//        "}"

//        "QTreeView::item:selected:!active {"
//        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
//        "}"
        ;
}

//namespace PMH {
//namespace Internal {
//PmhItemDelegate::PmhItemDelegate(QObject *parent) :
//        QStyledItemDelegate(parent)
//{
//}

//void PmhItemDelegate::drawHovered()
//{
//}

////void PmhItemDelegate::setPmhCategoryModel(PmhCategoryModel *model)
////{
////    m_PmhModel = model;
////}

////void PmhItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
////{
////    if (option.state & QStyle::State_MouseOver) {
////        if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
////            pressedIndex = QModelIndex();
////        QBrush brush = option.palette.alternateBase();
////        if (index == pressedIndex)
////            brush = option.palette.dark();
////        painter->fillRect(option.rect, brush);
////    }

////    // If item is PMHx -> draw all children
////    if (m_PmhModel->isPmhx(index)) {
////        for(int i = 0; i < m_PmhModel->rowCount(); ++i) {
////            paint()
////        }
////    }
////    // If parent item is PMHx -> draw it

////    QModelIndex pmhParent = index;
////    while (m_PmhModel->isPmhx(pmhParent)) {
////        pmhParent = pmhParent.parent();
////    }

////    QStyledItemDelegate::paint(painter, option, index);

////    if (index.column()==EpisodeModel::EmptyColumn1 && option.state & QStyle::State_MouseOver) {
////        QIcon icon;
////        if (option.state & QStyle::State_Selected) {
////            if (m_EpisodeModel->isEpisode(index)) {
////                icon = theme()->icon(Core::Constants::ICONVALIDATELIGHT);
////            } else {
////                // test the form to be unique or multiple episode
////                if (m_EpisodeModel->isUniqueEpisode(index) && m_EpisodeModel->rowCount(index) == 1)
////                    return;
////                if (m_EpisodeModel->isNoEpisode(index))
////                    return;
////                icon = theme()->icon(Core::Constants::ICONADDLIGHT);
////            }
////        } else {
////            if (m_EpisodeModel->isEpisode(index)) {
////                icon = theme()->icon(Core::Constants::ICONVALIDATEDARK);
////            } else {
////                // test the form to be unique or multiple episode
////                if (m_EpisodeModel->isUniqueEpisode(index) && m_EpisodeModel->rowCount(index) == 1)
////                    return;
////                if (m_EpisodeModel->isNoEpisode(index))
////                    return;
////                icon = theme()->icon(Core::Constants::ICONADDDARK);
////            }
////        }

////        QRect iconRect(option.rect.right() - option.rect.height(),
////                       option.rect.top(),
////                       option.rect.height(),
////                       option.rect.height());

////        icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
////    }
////}
//}
//}


PmhModeWidget::PmhModeWidget(QWidget *parent) :
        PmhContextualWidget(parent), ui(new Ui::PmhModeWidget), m_EditButton(0)
{
    ui->setupUi(this);
    ui->pmhViewer->setEditMode(PmhViewer::ReadOnlyMode);
    ui->treeViewLayout->setMargin(0);
    this->layout()->setMargin(0);

    ui->treeView->setActions(0);
    ui->treeView->setCommands(QStringList()
                              << Constants::A_PMH_NEW
                              << Constants::A_PMH_REMOVE
                              << Constants::A_PMH_CATEGORYMANAGER);
    ui->treeView->addContexts(contexts());
    ui->treeView->setModel(catModel());
    ui->treeView->header()->hide();
    ui->treeView->setStyleSheet(::TREEVIEW_SHEET);
//    ui->treeView->installEventFilter(this);

    // Actions connected in local widget context
    Core::Command *cmd = 0;
    cmd = actionManager()->command(Constants::A_PMH_REMOVE);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(removeItem()));

    // Populate ToolBar and create specific buttons
    m_EditButton = new QPushButton(ui->buttonBox);
    m_EditButton->setText(tkTr(Trans::Constants::M_EDIT_TEXT));
    ui->buttonBox->addButton(m_EditButton, QDialogButtonBox::YesRole);
    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);

    ui->treeView->hideColumn(PmhCategoryModel::Id);
    ui->treeView->hideColumn(PmhCategoryModel::Type);
    ui->treeView->hideColumn(PmhCategoryModel::EmptyColumn);
    ui->treeView->header()->setStretchLastSection(false);
    ui->treeView->header()->setResizeMode(PmhCategoryModel::Label, QHeaderView::Stretch);

    // connect the aAddPmh action
    cmd = actionManager()->command(Constants::A_PMH_NEW);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(createPmh()));

    connect(ui->treeView->selectionModel(), SIGNAL(currentChanged (QModelIndex, QModelIndex)),
            this, SLOT(currentChanged(QModelIndex, QModelIndex)));
    connect(ui->treeView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(pmhModelRowsInserted(QModelIndex,int,int)));
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));

    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onPatientChanged()));
}

PmhModeWidget::~PmhModeWidget()
{
    delete ui;
}

int PmhModeWidget::currentSelectedCategory() const
{
    if (!ui->treeView->selectionModel()->hasSelection())
        return -1;
    QModelIndex item = ui->treeView->selectionModel()->currentIndex();
    while (!catModel()->isCategory(item)) {
        item = item.parent();
    }
    Category::CategoryItem *cat = catModel()->categoryForIndex(item);
    if (cat) {
        return cat->id();
    }
    return -1;
}

void PmhModeWidget::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (!current.isValid())
        return;

    // No active patient ?
    if (patient()->uuid().isEmpty()) {
        return;
    }

    ui->stackedWidget->setCurrentIndex(0);

    if (catModel()->isForm(current)) {
        // Show the form's widget
        const QString &formUid = catModel()->index(current.row(), PmhCategoryModel::Id, current.parent()).data().toString();
        if (m_FormUid_StackId.keys().contains(formUid)) {
            ui->stackedWidget->setCurrentIndex(m_FormUid_StackId.value(formUid));
        } else {
            // Create the stack id
            int id = m_FormUid_StackId.count() + 1;
            m_FormUid_StackId.insert(formUid, id);
            // Insert the form widget into the stack at specified id
            ui->stackedWidget->insertWidget(id, catModel()->formForIndex(current)->formWidget());
            ui->stackedWidget->setCurrentIndex(id);
        }

        // Activate the last episode of the form
        catModel()->activateFormEpisode(current);

    } else if (catModel()->isPmhx(current)) {
        ui->pmhViewer->setPmhData(catModel()->pmhDataforIndex(current));
    }
    ui->scrollArea->horizontalScrollBar()->setValue(0);
    ui->scrollArea->verticalScrollBar()->setValue(0);
}

void PmhModeWidget::onButtonClicked(QAbstractButton *button)
{
    if (button==m_EditButton) {
        ui->pmhViewer->setEditMode(PmhViewer::ReadWriteMode);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
        ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
        return;
    }

    switch (ui->buttonBox->standardButton(button)) {
    case QDialogButtonBox::Save:
        {
            // Get the modified PmhData
            PmhData *pmh = ui->pmhViewer->modifiedPmhData();
            // Inform the model
            catModel()->addPmhData(pmh);
            // Expand the first level of the PMHx
            QModelIndex idx = catModel()->indexForPmhData(pmh);
            ui->treeView->expand(idx);
//            for(int i=0; i < catModel()->rowCount(idx);++i) {
//                ui->treeView->expand(catModel()->index(i,0,idx));
//            }
            ui->pmhViewer->setEditMode(PmhViewer::ReadOnlyMode);
            ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
            ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
            break;
        }
    case QDialogButtonBox::Cancel:
        ui->pmhViewer->revert();
        ui->pmhViewer->setEditMode(PmhViewer::ReadOnlyMode);
        ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
        break;
    default: break;
    }
}

void PmhModeWidget::createCategory()
{
    Category::CategoryDialog dlg(this);
    dlg.setModal(true);
    dlg.setCategoryModel(catModel(), PmhCategoryModel::Label);
    dlg.exec();
}

void PmhModeWidget::removeItem()
{
    if (!ui->treeView->selectionModel()->hasSelection())
        return;
    QModelIndex item = ui->treeView->selectionModel()->currentIndex();

    // Do not delete categories this way (use the CategoryManagerDialog)
    if (catModel()->isCategory(item) || catModel()->isForm(item))
        return;

    // Get the Root PMHx index
    while (true) {
        if (catModel()->isCategory(item.parent()))
            break;
        item = item.parent();
    }

    // Ask user
    bool yes = Utils::yesNoMessageBox(tr("Remove PMHx"),
                                      tr("Do you really want to remove the PMHx called <br />&nbsp;&nbsp;&nbsp;<b>%1</b> ?").arg(item.data().toString()));
    if (yes)
        catModel()->removeRow(item.row(), item.parent());
}

void PmhModeWidget::onPatientChanged()
{
    ui->treeView->expandAll();
    for(int i = 1; i < ui->stackedWidget->count(); ++i) {
        // Remove all form's widgets
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(i));
    }
    m_FormUid_StackId.clear();
}

void PmhModeWidget::createPmh()
{
    PmhCreatorDialog dlg(this);
    if (ui->treeView->selectionModel()->hasSelection()) {
        QModelIndex item = ui->treeView->selectionModel()->currentIndex();
        while (!catModel()->isCategory(item)) {
            item = item.parent();
        }
        dlg.setCategory(catModel()->categoryForIndex(item));
    }
    dlg.exec();
}

void PmhModeWidget::pmhModelRowsInserted(const QModelIndex &parent, int start, int end)
{
    ui->treeView->expand(parent);
    for(int i=start; i != end+1; ++i) {
        ui->treeView->expand(catModel()->index(i, PmhCategoryModel::Label, parent));
    }
}

void PmhModeWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        m_EditButton->setText(tkTr(Trans::Constants::M_EDIT_TEXT));
        break;
    default:
        break;
    }
}


//static void drawItemHovered(const QModelIndex &index, QTreeView *view)
//{
//    QHoverEvent *e = new QHoverEvent(QEvent::HoverMove, view->visualRect(index).center(), QPoint(0, 0));
//    qApp->sendEvent(view, e);
//    qApp->processEvents();
////    for(int i = 0;)
//}

//bool PmhModeWidget::eventFilter(QObject *o, QEvent *e)
//{
//    if (o==ui->treeView && e->type()==QEvent::HoverMove) {
//        QHoverEvent* h = static_cast<QHoverEvent*>(e);
//        QModelIndex parent = ui->treeView->indexAt(h->pos());
//        if (catModel()->isPmhx(parent)) {
//            // find the first parent
//            while (catModel()->isPmhx(parent.parent())) {
//                parent = parent.parent();
//            }
//            drawItemHovered(parent, ui->treeView);
//            qWarning() << "hover" << parent.data().toString();
//            return true;
//        }
//    }
//    return false;
//}


PmhMode::PmhMode(QObject *parent) :
    Core::BaseMode(parent),
    m_inPluginManager(false)
{
    setName(tkTr(Trans::Constants::PMHX));
    setIcon(theme()->icon(Core::Constants::ICONPATIENTHISTORY, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_PATIENT_HISTORY);
    setUniqueModeName(Core::Constants::MODE_PATIENT_HISTORY);
    setPatientBarVisibility(true);

//    const QList<int> &context;
//    setContext();

    m_Widget = new PmhModeWidget;
    setWidget(m_Widget);
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onPatientChanged()));
}

PmhMode::~PmhMode()
{
    if (m_inPluginManager) {
        pluginManager()->removeObject(this);
    }
}

QString PmhMode::name() const
{
    return tkTr(Trans::Constants::PMHX);
}

void PmhMode::onPatientChanged()
{
    if (!m_inPluginManager) {
        pluginManager()->addObject(this);
        m_inPluginManager = true;
    }
}

