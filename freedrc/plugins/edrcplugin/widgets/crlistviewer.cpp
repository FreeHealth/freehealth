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
 * \brief Contextualized widget for the eDRC plugin.
 *
 * This contextualized widget has access to all actions that are added in the particular context of
 * eDRC (e.g. eDRC::Constants::C_eDRC).\n
 * You can add context identificators to the widget by using the addContexts() method.
 * The context object is automatically removed from the context manager in its destructor
 * (no special code needed).
 * \sa Core::IContext, Core::ContextManager, Core::ICore::contextManager()
 */

#include "crlistviewer.h"
#include <edrcplugin/constants.h>
#include <edrcplugin/consultresult.h>
#include <edrcplugin/models/crtreemodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/contextmanager/contextmanager.h>

#include <utils/randomizer.h>

#include <QStyledItemDelegate>
#include <QPainter>

#include "ui_crlistviewer.h"

using namespace eDRC;
using namespace Internal;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

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

} // namespace Internal
} // namespace eDRC

CrListViewer::CrListViewer(QWidget *parent) :
    QWidget(parent),
    m_Context(0)
{
    ui = new Ui::CrListViewer;
    ui->setupUi(this);
    ui->treeView->treeView()->viewport()->setAttribute(Qt::WA_Hover);
    ui->treeView->treeView()->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->treeView->treeView()->setStyleSheet(::TREEVIEW_SHEET);

    TreeViewDelegate *delegate = new TreeViewDelegate(this);

//    Core::Context context(Constants::C_eDRC_PLUGINS);
//    // Create the context object
//    m_Context = new Internal::eDRCContext(this);
//    m_Context->setContext(context);
    
//    // Send it to the contextual manager
//    contextManager()->addContextObject(m_Context);


    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    QList<ConsultResult> list;
    for(int i=0; i<10; ++i) {
        ConsultResult cr;
        cr.setConsultResult(r.randomInt(1, 300));
        cr.setDiagnosisPosition(ConsultResult::DiagnosisPosition(r.randomInt(0, 3)));
        cr.setMedicalFollowUp(ConsultResult::MedicalFollowUp(r.randomInt(0, 2)));
        cr.setSymptomaticState(ConsultResult::SymptomaticState(r.randomInt(0, 1)));
        cr.setChronicDiseaseState(ConsultResult::ChronicDiseaseState(r.randomInt(0, 1)));
        cr.setSelectedCriterias(QList<int>() << r.randomInt(0, 300) << r.randomInt(0, 300));
        cr.setHtmlCommentOnCR(r.randomString(r.randomInt(10, 500)));
        cr.setHtmlCommentOnCriterias(r.randomString(r.randomInt(10, 500)));
        cr.setDateOfExamination(r.randomDateTime(QDateTime::currentDateTime().addMonths(-10)));
        list << cr;
    }
    _crTreeModel = new CrTreeModel(this);
    _crTreeModel->setCrList(list);
    ui->treeView->setModel(_crTreeModel);
    for(int i=1; i < CrTreeModel::ColumnCount; ++i) {
        ui->treeView->header()->setSectionHidden(i, true);
    }
    ui->treeView->header()->setStretchLastSection(false);
    ui->treeView->header()->setSectionHidden(CrTreeModel::Label, false);
    ui->treeView->header()->setSectionHidden(CrTreeModel::Empty1, false);
    ui->treeView->setColumnWidth(CrTreeModel::Empty1, 22);
#if QT_VERSION < 0x050000
    ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);
    ui->treeView->header()->setResizeMode(1, QHeaderView::Fixed);
#else
    // Qt5
    ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeView->header()->setSectionResizeMode(1, QHeaderView::Fixed);
#endif

    delegate->setCrTreeModel(_crTreeModel);
    ui->treeView->setItemDelegate(delegate);
}

CrListViewer::~CrListViewer()
{
    // Remove contextual object
//    contextManager()->removeContextObject(m_Context);
}

void CrListViewer::addContexts(const QList<int> &contexts)
{
//    m_Context->addContext(contexts);
}

