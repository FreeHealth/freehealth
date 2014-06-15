/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "fancytreeview.h"
#include "simplecategorycreator.h"

#include "ui_fancytreeview.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QTreeView>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPainter>
#include <QEvent>
#include <QMenu>

#include <QDebug>

using namespace Views;
using namespace Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }

namespace Views {

    namespace Constants {
        static const char * A_SAVE_MODEL = "FancyTreeView.aSave";
        static const char * A_CREATE_NEW = "FancyTreeView.aCreate";
        static const char * A_REMOVE_ITEM = "FancyTreeView.aRemove";
    }

namespace Internal {
class FancyTreeViewPrivate
{
public:
    FancyTreeViewPrivate() :
            m_HeaderMenu(0), m_SubHMenu(0), m_ItemMenu(0),
            m_Delegate(0), m_Model(0),
            aSave(0), aAddRow(0), aRemoveRow(0), aRevert(0)
    {
    }

    ~FancyTreeViewPrivate()
    {
        if (m_Delegate)
            delete m_Delegate;
        m_Delegate = 0;
    }

    QMenu *m_HeaderMenu, *m_SubHMenu, *m_ItemMenu;
    TreeItemDelegate *m_Delegate;
    QAbstractItemModel *m_Model;
    QAction *aSave, *aAddRow, *aRemoveRow, *aRevert;
};

}
}

TreeItemDelegate::TreeItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent), m_FancyColumn(-1)
{
}

void TreeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    if (m_FancyColumn==-1) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    if (option.state & QStyle::State_MouseOver) {
        if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
            pressedIndex = QModelIndex();
        QBrush brush = option.palette.alternateBase();
        if (index == pressedIndex)
            brush = option.palette.dark();
        painter->fillRect(option.rect, brush);
    }

    QStyledItemDelegate::paint(painter, option, index);

    if (index.column()==m_FancyColumn && option.state & QStyle::State_MouseOver) {
        QIcon icon;
        if (option.state & QStyle::State_Selected) {
            icon = theme()->icon(Core::Constants::ICONADDLIGHT);
        } else {
            icon = theme()->icon(Core::Constants::ICONADDDARK);
        }

        QRect iconRect(option.rect.right() - option.rect.height(),
                       option.rect.top(),
                       option.rect.height(),
                       option.rect.height());

        icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
    }
}




FancyTreeView::FancyTreeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FancyTreeView),
    d(new Internal::FancyTreeViewPrivate)
{
    ui->setupUi(this);
    QTreeView *tree = ui->widget->treeView();

    tree->viewport()->setAttribute(Qt::WA_Hover);
    tree->setItemDelegate((d->m_Delegate = new Internal::TreeItemDelegate(this)));
    tree->setFrameStyle(QFrame::NoFrame);
    tree->setAttribute(Qt::WA_MacShowFocusRect, false);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(tree, SIGNAL(clicked(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));
    connect(tree, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePressed(QModelIndex)));
}

FancyTreeView::~FancyTreeView()
{
    delete ui; ui=0;
    delete d; d=0;
}

QTreeView *FancyTreeView::treeView() const
{
    return ui->widget->treeView();
}

QToolButton *FancyTreeView::button()
{
    if (ui)
        return ui->button;
    return 0;
}

/** \brief Define the \e actions to add in the treeview button. If you just want to connect the signals set \e connectToDefault to false. */
void FancyTreeView::setButtonActions(const ButtonActions &actions, bool connectToDefault)
{
    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context globalContext(Core::Constants::C_GLOBAL);
    if (actions & FTV_SaveModel) {
        a = d->aSave = new QAction(ui->button);
        a->setObjectName("FancyTreeView.aSave");
        a->setIcon(theme()->icon(Core::Constants::ICONSAVE));
        a->setIconVisibleInMenu(true);
        cmd = actionManager()->registerAction(a, Core::Id(Constants::A_SAVE_MODEL), globalContext);
        cmd->setTranslations(Trans::Constants::FILESAVE_TEXT);
        //        cmenu->addAction(cmd, Core::Id(Core::Constants::G_EDIT_LIST));
        if (connectToDefault)
            connect(a, SIGNAL(triggered()), this, SLOT(save()));
        connect(a, SIGNAL(triggered()), this, SIGNAL(saveRequested()));
        ui->button->addAction(cmd->action());
    }
    if (actions & FTV_RevertModel) {
    }
    if (actions & FTV_CreateNew) {
        a = d->aAddRow = new QAction(ui->button);
        a->setObjectName("FancyTreeView.aAddRow");
        a->setIcon(theme()->icon(Core::Constants::ICONADD));
        a->setIconVisibleInMenu(true);
        cmd = actionManager()->registerAction(a, Core::Id(Constants::A_CREATE_NEW), globalContext);
        cmd->setTranslations(Trans::Constants::LISTADD_TEXT);
        //        cmenu->addAction(cmd, Core::Id(Core::Constants::G_EDIT_LIST));
        if (connectToDefault)
            connect(a, SIGNAL(triggered()), this, SLOT(addItem()));
        connect(a, SIGNAL(triggered()), this, SIGNAL(addItemRequested()));
        ui->button->addAction(cmd->action());
    }
    if (actions & FTV_RemoveRow) {
        a = d->aRemoveRow = new QAction(ui->button);
        a->setObjectName("FancyTreeView.aRemoveRow");
        a->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
        a->setIconVisibleInMenu(true);
        a->setText("Remove");
        cmd = actionManager()->registerAction(a, Core::Id(Constants::A_REMOVE_ITEM), globalContext);
        cmd->setTranslations(Trans::Constants::LISTREMOVE_TEXT);
        //        cmenu->addAction(cmd, Core::Id(Core::Constants::G_EDIT_LIST));
        if (connectToDefault)
            connect(a, SIGNAL(triggered()), this, SLOT(removeItem()));
        connect(a, SIGNAL(triggered()), this, SIGNAL(removeItemRequested()));
        ui->button->addAction(cmd->action());
    }

    if (d->aSave)
        ui->button->setDefaultAction(actionManager()->command(Core::Id(Constants::A_SAVE_MODEL))->action());
    else if (d->aAddRow)
        ui->button->setDefaultAction(actionManager()->command(Core::Id(Constants::A_CREATE_NEW))->action());
    else if (d->aRemoveRow)
        ui->button->setDefaultAction(actionManager()->command(Core::Id(Constants::A_REMOVE_ITEM))->action());
    else if (d->aRevert)
        ui->button->setDefaultAction(actionManager()->command(Core::Id(Constants::A_REMOVE_ITEM))->action());
}

/** \brief Returns the search line edit used in the view. */
Utils::QButtonLineEdit *FancyTreeView::searchLine()
{
    return ui->searchLine;
}

/** \brief Define the menu to use with the fancy button and and the context menu on header items (items without parent). */
void FancyTreeView::setHeaderMenu(QMenu *menu)
{
    d->m_HeaderMenu = menu;
}

/** \brief Define the menu to use with the fancy button and and the context menu on subheader items (items with parent and items). */
void FancyTreeView::setSubHeadingMenu(QMenu *menu)
{
    d->m_SubHMenu = menu;
}

/** \brief Define the menu to use with the fancy button and and the context menu on items. */
void FancyTreeView::setItemMenu(QMenu *menu)
{
    d->m_ItemMenu = menu;
}

/** \brief Define the context menu behavior of the treeview. */
void FancyTreeView::useContextMenu(bool state)
{
    if (state)
        ui->widget->treeView()->setContextMenuPolicy(Qt::CustomContextMenu);
    else
        ui->widget->treeView()->setContextMenuPolicy(Qt::NoContextMenu);
}

/**
  \brief Define the model to use with the fancytreeview and define the empty columns to use for the fancy buttons.
  Models should not provide data on the emptycolumns.
*/
void FancyTreeView::setModel(QAbstractItemModel *model, const int columnForFancyButton)
{
    QTreeView *tree = ui->widget->treeView();
    d->m_Model = model;
    tree->setModel(model);
    tree->header()->setStretchLastSection(false);
#if QT_VERSION < 0x050000
    tree->header()->setResizeMode(0, QHeaderView::Stretch);
    tree->header()->setResizeMode(columnForFancyButton, QHeaderView::Fixed);
#else
    // Qt5
    tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    tree->header()->setSectionResizeMode(columnForFancyButton, QHeaderView::Fixed);
#endif
    tree->setColumnWidth(columnForFancyButton, 16);

    d->m_Delegate->setModel(model);
    d->m_Delegate->setFancyColumn(columnForFancyButton);
}

/** \brief Defines the model column to hide in the QTreeView. */
void FancyTreeView::hideColumn(int column)
{
    if (ui)
        ui->widget->treeView()->hideColumn(column);
}

/** \brief Show context menu of the treeview. */
void FancyTreeView::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    QMenu *pop = new QMenu(this);
    pop->addActions(ui->button->actions());
    pop->exec(ui->widget->treeView()->mapToGlobal(pos));
    delete pop;
}

void FancyTreeView::handlePressed(const QModelIndex &index)
{
    if (index.column() == d->m_Delegate->fancyColumn()) {
        d->m_Delegate->pressedIndex = index;
    }
}

void FancyTreeView::handleClicked(const QModelIndex &index)
{
    qWarning() << index << ui->widget->treeView()->selectionModel()->isSelected(index);
//    if (ui->widget->treeView()->selectionModel()->isSelected(index))
//        ui->widget->treeView()->clearSelection();
    if (index.column() == d->m_Delegate->fancyColumn()) { // the fancy button
        qWarning() << "Fancy button called" << index;
        // Header ?
        if (index.parent()==QModelIndex() && d->m_Model->hasChildren(index)) {
            qWarning() << "header";
        } else if (index.parent() != QModelIndex() && d->m_Model->hasChildren(index)) {
            // SubHeader
            qWarning() << "subheader";
        } else if (!d->m_Model->hasChildren(index)) {
            // Item
            qWarning() << "item";
        }
//        // work around a bug in itemviews where the delegate wouldn't get the QStyle::State_MouseOver
//        QPoint cursorPos = QCursor::pos();
//        QWidget *vp = ui->widget->treeView()->viewport();
//        QMouseEvent e(QEvent::MouseMove, vp->mapFromGlobal(cursorPos), cursorPos, Qt::NoButton, 0, 0);
//        QCoreApplication::sendEvent(vp, &e);
    }
}

/** \brief Slot called when user triggers the save action. By default, call submit() on the QTreeView model. */
void FancyTreeView::save()
{
    if (!d->m_Model)
        return;
    if (!d->m_Model->submit())
        LOG_ERROR("Unable to save model.");

}

/** \brief Slot called when user triggers the add item action. */
void FancyTreeView::addItem()
{
    QModelIndex parent = QModelIndex();
    if (ui->widget->treeView()->selectionModel()->hasSelection())
        parent = ui->widget->treeView()->selectionModel()->currentIndex();
//    SimpleCategoryCreator dlg(this);
//    dlg.setModel(d->m_Model, parent);
//    dlg.setLabelColumn(0);
//    dlg.setIconColumn(1);
//    dlg.exec();
    if (!d->m_Model->insertRow(d->m_Model->rowCount(parent), parent))
        return;
    // expand item
    ui->widget->treeView()->expand(parent);
    // open edit item
    ui->widget->treeView()->edit(d->m_Model->index(d->m_Model->rowCount(parent)-1, parent.column(), parent));
}

void FancyTreeView::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
//        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
