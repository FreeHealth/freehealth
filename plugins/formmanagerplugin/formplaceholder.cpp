/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Form::FormPlaceHolder
  Widget containing the Episode treeView and the forms in a QStackedLayout
  When an episode is activated by the user, the formViewer is set to the corresponding form
  and episode datas. Datas are automatically saved (without any user intervention).
*/

#include "formplaceholder.h"
#include "constants_settings.h"
#include "constants_db.h"
#include "formeditordialog.h"

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/episodemodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <listviewplugin/treeview.h>

#include <utils/widgets/minisplitter.h>
#include <utils/log.h>

#include <extensionsystem/pluginmanager.h>

#include <QTreeView>
#include <QTreeWidgetItem>
#include <QStackedLayout>
#include <QScrollArea>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>
#include <QEvent>

#include <QDebug>

using namespace Form;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}


namespace Form {
namespace Internal {
class FormPlaceHolderPrivate
{
public:
    FormPlaceHolderPrivate(FormPlaceHolder *parent) :
            m_RootForm(0),
            m_EpisodeModel(0),
            m_FileTree(0),
            m_Delegate(0),
            m_EpisodesTable(0),
            m_Stack(0),
            m_GeneralLayout(0),
            m_Scroll(0),
            horizSplitter(0),
            q(parent)
    {
    }

    ~FormPlaceHolderPrivate()
    {
        if (m_FileTree) {
            delete m_FileTree; m_FileTree = 0;
        }
        if (m_Stack) {
            delete m_Stack; m_Stack = 0;
        }
        if (m_GeneralLayout) {
            delete m_GeneralLayout; m_GeneralLayout=0;
        }
    }

    void populateStackLayout()
    {
        Q_ASSERT(m_Stack);
        if (!m_Stack)
            return;
        clearStackLayout();
        foreach(FormMain *form, m_RootForm->flattenFormMainChildren()) {
            if (form->formWidget()) {
				QScrollArea *sa = new QScrollArea;
				sa->setWidgetResizable(true);
                QWidget *w = new QWidget();
				sa->setWidget(w);
                QVBoxLayout *vl = new QVBoxLayout(w);
                vl->setSpacing(0);
                vl->setMargin(0);
                vl->addWidget(form->formWidget());
                int id = m_Stack->addWidget(sa);
                m_StackId_FormUuid.insert(id, form->uuid());
            }
        }
    }

    void saveSettings()
    {
        QList<QVariant> sizes;
        foreach(int s, horizSplitter->sizes())
            sizes << s;
        settings()->setValue(QString("%1/%2").arg(Constants::S_PLACEHOLDERSPLITTER_SIZES).arg(q->objectName()), sizes);
    }

    void clearStackLayout()
    {
        /** \todo check leaks */
        for(int i = m_Stack->count(); i>0; --i) {
            QWidget *w = m_Stack->widget(i);
            m_Stack->removeWidget(w);
            delete w;
        }
    }

public:
    FormMain *m_RootForm;
    EpisodeModel *m_EpisodeModel;
    Views::TreeView *m_FileTree;
//    QTreeView *m_FileTree;
    FormItemDelegate *m_Delegate;
    QTableView *m_EpisodesTable;
    QStackedLayout *m_Stack;
    QGridLayout *m_GeneralLayout;
    QScrollArea *m_Scroll;
    QHash<int, QString> m_StackId_FormUuid;
    Utils::MiniSplitter *horizSplitter;

private:
    FormPlaceHolder *q;
};

FormItemDelegate::FormItemDelegate(QObject *parent)
 : QStyledItemDelegate(parent)
{
}

void FormItemDelegate::setEpisodeModel(EpisodeModel *model)
{
    m_EpisodeModel = model;
}

void FormItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
    if (option.state & QStyle::State_MouseOver) {
        if ((QApplication::mouseButtons() & Qt::LeftButton) == 0)
            pressedIndex = QModelIndex();
        QBrush brush = option.palette.alternateBase();
        if (index == pressedIndex)
            brush = option.palette.dark();
        painter->fillRect(option.rect, brush);
    }

    QStyledItemDelegate::paint(painter, option, index);

    if (index.column()==EpisodeModel::EmptyColumn1 && option.state & QStyle::State_MouseOver) {
        QIcon icon;
        if (option.state & QStyle::State_Selected) {
            if (m_EpisodeModel->isEpisode(index)) {
                icon = theme()->icon(Core::Constants::ICONVALIDATELIGHT);
            } else {
                // test the form to be unique or multiple episode
                if (m_EpisodeModel->isUniqueEpisode(index) && m_EpisodeModel->rowCount(index) == 1)
                    return;
                if (m_EpisodeModel->isNoEpisode(index))
                    return;
                icon = theme()->icon(Core::Constants::ICONADDLIGHT);
            }
        } else {
            if (m_EpisodeModel->isEpisode(index)) {
                icon = theme()->icon(Core::Constants::ICONVALIDATEDARK);
            } else {
                // test the form to be unique or multiple episode
                if (m_EpisodeModel->isUniqueEpisode(index) && m_EpisodeModel->rowCount(index) == 1)
                    return;
                if (m_EpisodeModel->isNoEpisode(index))
                    return;
                icon = theme()->icon(Core::Constants::ICONADDDARK);
            }
        }

        QRect iconRect(option.rect.right() - option.rect.height(),
                       option.rect.top(),
                       option.rect.height(),
                       option.rect.height());

        icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
    }
}

}
}

FormPlaceHolder::FormPlaceHolder(QWidget *parent) :
        FormContextualWidget(parent),
        d(new Internal::FormPlaceHolderPrivate(this))
{
    FormManager::instance();
    // create general layout
    d->m_GeneralLayout = new QGridLayout(this);
    d->m_GeneralLayout->setObjectName("FormPlaceHolder::GeneralLayout");
    d->m_GeneralLayout->setMargin(0);
    d->m_GeneralLayout->setSpacing(0);
    setLayout(d->m_GeneralLayout);

    // create the tree view
    /** \todo use the Views::FancyTreeView */
    QWidget *w = new QWidget;
//    d->m_FileTree = new QTreeView(this);
    d->m_FileTree = new Views::TreeView(this,0);
    d->m_FileTree->setActions(0);
    d->m_FileTree->setCommands(QStringList() << Constants::A_ADDEPISODE << Constants::A_VALIDATEEPISODE << Constants::A_ADDFORM);
    d->m_FileTree->addContexts(contexts());
    d->m_FileTree->setDeselectable(false);
    d->m_FileTree->disconnectActionsToDefaultSlots();
    d->m_FileTree->setObjectName("FormTree");
//    d->m_FileTree->setIndentation(10);
    d->m_FileTree->viewport()->setAttribute(Qt::WA_Hover);
    d->m_FileTree->setItemDelegate((d->m_Delegate = new Internal::FormItemDelegate(this)));
    d->m_FileTree->setFrameStyle(QFrame::NoFrame);
    d->m_FileTree->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->m_FileTree->setSelectionMode(QAbstractItemView::SingleSelection);
    d->m_FileTree->setSelectionBehavior(QAbstractItemView::SelectRows);
//    d->m_FileTree->setStyleSheet("QTreeView#FormTree{background:#dee4ea}");

    connect(d->m_FileTree, SIGNAL(clicked(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));
    connect(d->m_FileTree, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePressed(QModelIndex)));
    connect(d->m_FileTree, SIGNAL(activated(QModelIndex)), this, SLOT(setCurrentEpisode(QModelIndex)));
//    connect(d->m_FileTree, SIGNAL(addRequested()), this, SLOT(newEpisode()));
//    connect(d->m_FileTree, SIGNAL(removeRequested()), this, SLOT(removeEpisode()));

    Core::Command *cmd = actionManager()->command(Constants::A_ADDEPISODE);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(newEpisode()));
    cmd = actionManager()->command(Constants::A_ADDFORM);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(addForm()));

//    connect(d->m_FileTree, SIGNAL(customContextMenuRequested(QPoint)),
//            this, SLOT(contextMenuRequested(QPoint)));

    // create the central view
/*    d->m_Scroll = new QScrollArea;
	  d->m_Scroll->setWidgetResizable(true);*/
    d->m_Stack = new QStackedLayout(w);
    d->m_Stack->setObjectName("FormPlaceHolder::StackedLayout");

    // create splitters
    d->horizSplitter = new Utils::MiniSplitter(this);
    d->horizSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->horizSplitter->setObjectName("FormPlaceHolder::MiniSplitter1");
    d->horizSplitter->setOrientation(Qt::Horizontal);

    Utils::MiniSplitter *vertic = new Utils::MiniSplitter(this);
    vertic->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vertic->setObjectName("FormPlaceHolder::MiniSplitter::Vertical");
    vertic->setOrientation(Qt::Vertical);

    /** \todo add the episode table view */
//    d->m_EpisodesTable = new QTableView(this);
//    d->m_EpisodesTable->horizontalHeader()->hide();
//    d->m_EpisodesTable->verticalHeader()->hide();

    d->horizSplitter->addWidget(d->m_FileTree);
//    d->horizSplitter->addWidget(wb);
//    vertic->addWidget(d->m_EpisodesTable);
//    vertic->addWidget(d->m_Scroll);
	vertic->addWidget(w);
    d->horizSplitter->addWidget(vertic);

//    d->horizSplitter->setStretchFactor(0, 1);
//    d->horizSplitter->setStretchFactor(1, 3);
//    vertic->setStretchFactor(0, 1);
//    vertic->setStretchFactor(1, 3);

//    d->m_Scroll->setWidget(w);

    d->m_GeneralLayout->addWidget(d->horizSplitter, 100, 0);
}

FormPlaceHolder::~FormPlaceHolder()
{
    d->saveSettings();
//    if (d->m_RootForm) {
//        // Remove object from the plugin object pool
//        pluginManager()->removeObject(d->m_RootForm);
//        delete d->m_RootForm;
//        d->m_RootForm = 0;
//    }
    if (d) {
        delete d;
        d = 0;
    }
}

/** Overload of the QObject member used to save/load settings. */
void FormPlaceHolder::setObjectName(const QString &name)
{
    QObject::setObjectName(name);
    QList<QVariant> sizesVar = settings()->value(QString("%1/%2").arg(Constants::S_PLACEHOLDERSPLITTER_SIZES).arg(objectName())).toList();
    QList<int> sizes;
    foreach(const QVariant &v, sizesVar)
        sizes << v.toInt();
    d->horizSplitter->setSizes(sizes);
}

/**
  Define the Form::FormMain root item to use for the creation of the patient files.
  This object will manage deletion of the root item and its children.
*/
void FormPlaceHolder::setRootForm(Form::FormMain *rootForm)
{
    if (d->m_EpisodeModel) {
        delete d->m_EpisodeModel;
        d->m_EpisodeModel = 0;
    }
    /** \todo before deleting -> clear stackedLayout of FormPlaceHolder ? */
//    if (d->m_RootForm) {
//        delete d->m_RootForm;
//        d->m_RootForm = 0;
//    }

    // Add object to the plugin object pool
    d->m_RootForm = rootForm;
    d->clearStackLayout();
    if (!d->m_RootForm)
        return;

    // Create models
    d->m_EpisodeModel = new EpisodeModel(rootForm, this);
    d->m_Delegate->setEpisodeModel(d->m_EpisodeModel);
    d->m_FileTree->setModel(d->m_EpisodeModel);
    d->m_FileTree->setSelectionMode(QAbstractItemView::SingleSelection);
    d->m_FileTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    for(int i=0; i < Form::EpisodeModel::MaxData; ++i)
        d->m_FileTree->setColumnHidden(i, true);
    d->m_FileTree->setColumnHidden(Form::EpisodeModel::Label, false);
    d->m_FileTree->setColumnHidden(Form::EpisodeModel::EmptyColumn1, false);
    d->m_FileTree->header()->hide();
    d->m_FileTree->header()->setStretchLastSection(false);
    d->m_FileTree->header()->setResizeMode(Form::EpisodeModel::Label, QHeaderView::Stretch);
    d->m_FileTree->header()->setResizeMode(Form::EpisodeModel::EmptyColumn1, QHeaderView::Fixed);
    d->m_FileTree->header()->resizeSection(Form::EpisodeModel::EmptyColumn1, 16);

    d->m_FileTree->expandAll();
    d->populateStackLayout();
}

void FormPlaceHolder::handlePressed(const QModelIndex &index)
{
    if (index.column() == EpisodeModel::Label) {
//        d->m_EpisodeModel->activateEpisode(index);
    } else if (index.column() == EpisodeModel::EmptyColumn1) {
        d->m_Delegate->pressedIndex = index;
    }
}

void FormPlaceHolder::handleClicked(const QModelIndex &index)
{
    if (index.column() == EpisodeModel::EmptyColumn1) { // the funky button
        if (d->m_EpisodeModel->isForm(index)) {
            newEpisode();
        } else {
            /** \todo validateEpisode */
//            validateEpisode();
        }

        // work around a bug in itemviews where the delegate wouldn't get the QStyle::State_MouseOver
        QPoint cursorPos = QCursor::pos();
        QWidget *vp = d->m_FileTree->viewport();
        QMouseEvent e(QEvent::MouseMove, vp->mapFromGlobal(cursorPos), cursorPos, Qt::NoButton, 0, 0);
        QCoreApplication::sendEvent(vp, &e);
    }
}

QTreeView *FormPlaceHolder::formTree() const
{
    return d->m_FileTree;
}

QStackedLayout *FormPlaceHolder::formStackLayout() const
{
    return d->m_Stack;
}

void FormPlaceHolder::addBottomWidget(QWidget *bottom)
{
    d->m_GeneralLayout->addWidget(bottom, d->m_GeneralLayout->rowCount(), 0, 0, d->m_GeneralLayout->columnCount());
}

void FormPlaceHolder::setCurrentForm(const QString &formUuid)
{
    d->m_Stack->setCurrentIndex(d->m_StackId_FormUuid.key(formUuid));
    if (d->m_Stack->currentWidget())
        qobject_cast<QScrollArea*>(d->m_Stack->currentWidget())->widget()->setEnabled(false);
}

void FormPlaceHolder::setCurrentEpisode(const QModelIndex &index)
{
//    d->m_LastEpisode = index;
    const QString &formUuid = d->m_EpisodeModel->index(index.row(), EpisodeModel::FormUuid, index.parent()).data().toString();
    setCurrentForm(formUuid);
    if (d->m_EpisodeModel->isEpisode(index)) {
        qobject_cast<QScrollArea*>(d->m_Stack->currentWidget())->widget()->setEnabled(true);
        d->m_EpisodeModel->activateEpisode(index, formUuid);
    } else {
        d->m_EpisodeModel->activateEpisode(QModelIndex(), formUuid);
    }
}

//void FormPlaceHolder::reset()
//{
//    d->m_FileTree->update();
//    d->m_FileTree->expandAll();
//    d->populateStackLayout();
//}

void FormPlaceHolder::newEpisode()
{
    if (!d->m_FileTree->selectionModel())
        return;
    // get the parent form
    QModelIndex index;
    if (!d->m_FileTree->selectionModel()->hasSelection())
        return;
    index = d->m_FileTree->selectionModel()->selectedIndexes().at(0);
    while (!d->m_EpisodeModel->isForm(index)) {
        index = index.parent();
    }

    // test the form to be unique or multiple episode
    if (d->m_EpisodeModel->isUniqueEpisode(index) && d->m_EpisodeModel->rowCount(index) == 1)
        return;
    if (d->m_EpisodeModel->isNoEpisode(index))
        return;

    // create a new episode the selected form and its children
    if (!d->m_EpisodeModel->insertRow(0, index)) {
        Utils::Log::addError(this, "Unable to create new episode");
        return;
    }
    // activate the newly created main episode
    d->m_FileTree->selectionModel()->clearSelection();
    d->m_FileTree->selectionModel()->setCurrentIndex(d->m_EpisodeModel->index(0,0,index), QItemSelectionModel::Select);
    const QString &formUuid = d->m_EpisodeModel->index(index.row(), Form::EpisodeModel::FormUuid, index.parent()).data().toString();
    setCurrentForm(formUuid);
    qobject_cast<QScrollArea*>(d->m_Stack->currentWidget())->widget()->setEnabled(true);
    d->m_EpisodeModel->activateEpisode(d->m_EpisodeModel->index(0,0,index), formUuid);
}

void FormPlaceHolder::removeEpisode()
{
    if (!d->m_FileTree->selectionModel())
        return;
    /** \todo removeEpisode */
}

void FormPlaceHolder::validateEpisode()
{
    if (!d->m_FileTree->selectionModel())
        return;
    /** \todo validateEpisode */
}

void FormPlaceHolder::addForm()
{
    if (!isVisible())
        return;
    // Dialog ?
    FormEditorDialog dlg(d->m_EpisodeModel, FormEditorDialog::DefaultMode, this);
    dlg.exec();
}

