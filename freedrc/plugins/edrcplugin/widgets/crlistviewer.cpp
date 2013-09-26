/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                      *
 *       Eric Maeker <eric.maeker@gmail.com>                               *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class eDRC::Internal::CrListViewer
 */

#include "crlistviewer.h"
#include <edrcplugin/constants.h>
#include <edrcplugin/consultresult.h>
#include <edrcplugin/models/crtreemodel.h>
#include <edrcplugin/widgets/creditordialog.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>

#include <utils/global.h>

#include <QStyledItemDelegate>
#include <QPainter>

#include "ui_crlistviewer.h"

using namespace eDRC;
using namespace Internal;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IMainWindow *mainWindow() {return Core::ICore::instance()->mainWindow();}

namespace {
const char * const TREEVIEW_SHEET =
        " QTreeView {"
        "    show-decoration-selected: 1;"
        "}"

        "QTreeView::item {"
//        "    border: 0px;"
        "    background: base;"
//        "    border-top-color: transparent;"
//        "    border-bottom-color: transparent;"
        "}"

        "QTreeView::item:hover {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
//        "    border: 0px solid #bfcde4;"
        "}"

//        "QTreeView::branch:hover {"
//        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);"
//        "    border: 0px solid #bfcde4;"
//        "}"

//        "QTreeView::item:selected {"
//        "    border: 0px solid #567dbc;"
//        "}"

        "QTreeView::item:selected {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"

        "QTreeView::branch:selected {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);"
        "}"

//        "QTreeView::item:selected:!active {"
//        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);"
//        "}"
        ;
}

namespace eDRC {
namespace Internal {

TreeViewDelegate::TreeViewDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    _crTreeModel(0)
{}

void TreeViewDelegate::setCrTreeModel(CrTreeModel *model)
{
    _crTreeModel = model;
}

void TreeViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    // Add the fancy button
    if (option.state & QStyle::State_MouseOver) {
        if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
            pressedIndex = QModelIndex();
        QBrush brush = option.palette.alternateBase();
        if (index == pressedIndex)
            brush = option.palette.dark();
        painter->fillRect(option.rect, brush);
    }

    QStyledItemDelegate::paint(painter, option, index);

    // Draw fancy button
    if (index.column()==CrTreeModel::Empty1 &&
            (option.state & QStyle::State_MouseOver)) {
        QIcon icon;
        if (option.state & QStyle::State_Selected) {
            // test the form to be unique or multiple episode
            //                if (_crTreeModel->isUniqueEpisode(index))
            //                    return;
            //                if (_crTreeModel->isNoEpisode(index))
            //                    return;
            icon = theme()->icon(Core::Constants::ICONADDLIGHT);
        } else {
            // test the form to be unique or multiple episode
            //                if (_crTreeModel->isUniqueEpisode(index))
            //                    return;
            //                if (_crTreeModel->isNoEpisode(index))
            //                    return;
            icon = theme()->icon(Core::Constants::ICONADDDARK);
        }

        QRect iconRect(option.rect.right() - option.rect.height(),
                       option.rect.top(),
                       option.rect.height(),
                       option.rect.height());

        icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
    }
}

QSize TreeViewDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    Q_ASSERT(_crTreeModel);
    QSize itemSize(10, 10);
    //        const bool topLevel = !index.parent().isValid();
    //        if (topLevel) {
    //            // For top level item, user can define in the Form::FormMain extraData the height of the item
    //            // If the item has a defined height, use it
    //            // Else check the empty root form height
    //            // Else use the default (10px)
    //            Form::FormMain *form = _formTreeModel->formForIndex(index);
    //            if (form) {
    //                if (form->extraData().contains("rootitemextraheight")) {
    //                    itemSize = QSize(10, form->extraData().value("rootitemextraheight").toInt());
    //                } else {
    //                    if (form->rootFormParent()->extraData().contains("rootitemextraheight")) {
    //                        itemSize = QSize(10, form->rootFormParent()->extraData().value("rootitemextraheight").toInt());
    //                    }
    //                }
    //            }
    //            return QStyledItemDelegate::sizeHint(option, index) + itemSize;
    //        }
    return QStyledItemDelegate::sizeHint(option, index) + itemSize;
}

class CrListViewerPrivate
{
public:
    CrListViewerPrivate(CrListViewer *parent) :
        ui(0),
        m_Context(0),
        _crTreeModel(0),
        _delegate(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~CrListViewerPrivate()
    {
        if (ui)
            delete ui;
    }

public:
    Ui::CrListViewer *ui;
    eDRCContext *m_Context;
    CrTreeModel *_crTreeModel;
    TreeViewDelegate *_delegate;

private:
    CrListViewer *q;
};

} // namespace Internal
} // namespace eDRC

CrListViewer::CrListViewer(QWidget *parent) :
    QWidget(parent),
    d(new CrListViewerPrivate(this))
{
    d->ui = new Ui::CrListViewer;
    d->ui->setupUi(this);

    // Manage CrTreeView options & delegate
    d->ui->treeView->treeView()->viewport()->setAttribute(Qt::WA_Hover);
    d->ui->treeView->treeView()->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->ui->treeView->treeView()->setStyleSheet(::TREEVIEW_SHEET);

    d->_delegate = new TreeViewDelegate(this);

//    Core::Context context(Constants::C_eDRC_PLUGINS);
//    // Create the context object
//    d->m_Context = new Internal::eDRCContext(this);
//    d->m_Context->setContext(context);
    
//    // Send it to the contextual manager
//    contextManager()->addContextObject(d->m_Context);

    // Connect buttons to global actions
    Core::Command *cmd;
    cmd = actionManager()->command(Core::Constants::A_FILE_OPEN);
    connect(d->ui->openFile, SIGNAL(clicked()), cmd->action(), SLOT(trigger()));

    // Connect buttons to local slots
    connect(d->ui->printHistory, SIGNAL(clicked()), this, SLOT(printHistory()));
    connect(d->ui->createCr, SIGNAL(clicked()), this, SLOT(createConsultResult()));

    // Resize splitter
    int width = size().width();
    int third = width/3;
    d->ui->splitter->setSizes(QList<int>() << third << width-third);
}

CrListViewer::~CrListViewer()
{
    if (d)
        delete d;
    d = 0;
    // Remove contextual object
//    contextManager()->removeContextObject(m_Context);
}

//void CrListViewer::addContexts(const QList<int> &contexts)
//{
//    d->m_Context->addContext(contexts);
//}

/**
 * Set the consult result tree model to use in the viewer
 * \sa eDRC::Internal::CrTreeModel
 */
void CrListViewer::setConsultResultTreeModel(CrTreeModel *model)
{
    Q_ASSERT(model);
    if (!model)
        return;
    if (d->_crTreeModel == model)
        return;

    // Disconnect old model
    disconnect(d->_crTreeModel, SIGNAL(modelReset()), this, SLOT(onModelReset()));

    // Connect new model
    d->_crTreeModel = model;
    connect(d->_crTreeModel, SIGNAL(modelReset()), this, SLOT(onModelReset()));

    // Setup ui
    d->ui->treeView->setModel(d->_crTreeModel);
    d->_delegate->setCrTreeModel(d->_crTreeModel);
    d->ui->treeView->setItemDelegate(d->_delegate);
    onModelReset();
}

void CrListViewer::printHistory()
{
    //    Core::IDocumentPrinter *p = printer();
    //    Q_ASSERT(p);
    //    p->clearTokens();
    //    ConsultResult cr = ui->crEditor->submit();
    //    QString html = edrcCore().toHtml(cr);
    //    QHash<QString, QVariant> tokens;
    //    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, tr("eDRC document"));
    //    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);
    //    return p->print(html,
    //                    Core::IDocumentPrinter::Papers_Generic_User,
    //                    false);
}

/**
 * Internal. Opens a consul result creator dialog, if the dialog is accepted
 * send the coded CR to the internal CrTreeModel
 */
void CrListViewer::createConsultResult()
{
    CrEditorDialog dlg(this);
    Utils::resizeAndCenter(&dlg, mainWindow());
    ConsultResult cr;
    cr.setDateOfExamination(QDateTime::currentDateTime());
    dlg.initialize(cr);
    if (dlg.exec() == QDialog::Accepted) {
        d->_crTreeModel->addConsultResult(dlg.submit());
    }
}

/**
 * Internal. When the internal eDRC::Internal::CrTreeModel is resetted,
 * update the view:
 * - select the history index
 * - show the history widget
 */
void CrListViewer::onModelReset()
{
    for(int i=1; i < CrTreeModel::ColumnCount; ++i) {
        d->ui->treeView->header()->setSectionHidden(i, true);
    }
    d->ui->treeView->header()->setStretchLastSection(false);
    d->ui->treeView->header()->setSectionHidden(CrTreeModel::Label, false);
    d->ui->treeView->header()->setSectionHidden(CrTreeModel::Empty1, false);
    d->ui->treeView->setColumnWidth(CrTreeModel::Empty1, 22);
#if QT_VERSION < 0x050000
    d->ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);
    d->ui->treeView->header()->setResizeMode(1, QHeaderView::Fixed);
#else
    // Qt5
    d->ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    d->ui->treeView->header()->setSectionResizeMode(1, QHeaderView::Fixed);
#endif
}
