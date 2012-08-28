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
#include "formtreemodel.h"

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformitemdata.h>
#include <formmanagerplugin/iformwidgetfactory.h>
#include <formmanagerplugin/episodemodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/idocumentprinter.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/actionmanager/actionmanager.h>

#include <listviewplugin/treeview.h>

#include <patientbaseplugin/constants_db.h>

#include <utils/log.h>
#include <utils/widgets/minisplitter.h>
#include <extensionsystem/pluginmanager.h>

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

// Test
#include <QTextBrowser>



#include <QDebug>

using namespace Form;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IMainWindow *mainWindow()  { return Core::ICore::instance()->mainWindow(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
inline static Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

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

        // Add the synthesis form
        QScrollArea *sa = new QScrollArea(m_Stack->parentWidget());
        sa->setWidgetResizable(true);
        QWidget *w = new QWidget(sa);
//        QPalette palette = w->palette();
//        QString gender = patient()->data(Core::IPatient::Gender).toString();
//        if (gender=="M")
//            palette.setColor(QPalette::Background, Patients::Constants::maleColor.lighter(140));
//        else if (gender=="F")
//            palette.setColor(QPalette::Background, Patients::Constants::femaleColor.lighter(140));
//        else
//            palette.setColor(QPalette::Background, Patients::Constants::hermaColor.lighter());
//        w->setPalette(palette);
        sa->setWidget(w);
        QVBoxLayout *vl = new QVBoxLayout(w);
        vl->setSpacing(0);
        vl->setMargin(0);
        QTextBrowser *t = new QTextBrowser(w);
        t->setReadOnly(true);
        t->setEnabled(true);
        vl->addWidget(t);
        int id = m_Stack->addWidget(sa);
        m_StackId_FormUuid.insert(id, Constants::PATIENTLASTEPISODES_UUID);

        // add all form's widgets
        foreach(FormMain *form, m_RootForm->flattenFormMainChildren()) {
            if (form->formWidget()) {
                QScrollArea *sa = new QScrollArea(m_Stack->parentWidget());
                sa->setWidgetResizable(true);
                QWidget *w = new QWidget(sa);
                sa->setWidget(w);
                QVBoxLayout *vl = new QVBoxLayout(w);
                vl->setSpacing(0);
                vl->setMargin(0);
                vl->addWidget(form->formWidget());
                int id = m_Stack->addWidget(sa);
                m_StackId_FormUuid.insert(id, form->uuid());
                form->formWidget()->setEnabled(false);
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
       // TODO: check leaks
        QWidget *w = m_Stack->parentWidget();
        delete m_Stack;
        m_Stack = new QStackedLayout(w);
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
    QHash<int, QString> m_StackId_FormUuid;
    Utils::MiniSplitter *horizSplitter;

private:
    FormPlaceHolder *q;
};

FormItemDelegate::FormItemDelegate(QObject *parent) :
        QStyledItemDelegate(parent)
{
}

void FormItemDelegate::setEpisodeModel(EpisodeModel *model)
{
    m_EpisodeModel = model;
}

QSize FormItemDelegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    const bool topLevel = !index.parent().isValid();
    if (topLevel) {
        return QStyledItemDelegate::sizeHint(option, index) + QSize(10,10);
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

void FormItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const
{
//    const bool topLevel = !index.parent().isValid();

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
    // TODO: use the Views::FancyTreeView
    QWidget *w = new QWidget;
//    d->m_FileTree = new QTreeView(this);
    d->m_FileTree = new Views::TreeView(this,0);
    d->m_FileTree->setActions(0);
    d->m_FileTree->setCommands(QStringList()
                               << Constants::A_ADDEPISODE
                               << Constants::A_VALIDATEEPISODE
                               << Constants::A_ADDFORM
                               << Constants::A_PRINTFORM);
    d->m_FileTree->addContexts(contexts());
    d->m_FileTree->setDeselectable(false);
    d->m_FileTree->disconnectActionsToDefaultSlots();
    d->m_FileTree->setObjectName("FormTree");

    QTreeView *tree = d->m_FileTree->treeView();
//    tree->setIndentation(10);
    tree->viewport()->setAttribute(Qt::WA_Hover);
    tree->setItemDelegate((d->m_Delegate = new Internal::FormItemDelegate(d->m_FileTree)));
    tree->setFrameStyle(QFrame::NoFrame);
    tree->setAttribute(Qt::WA_MacShowFocusRect, false);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree->setAlternatingRowColors(settings()->value(Constants::S_USEALTERNATEROWCOLOR).toBool());
//    tree->setRootIsDecorated(false);
    QString css = ::TREEVIEW_SHEET;
//    if (settings()->value(Constants::S_USESPECIFICCOLORFORROOTS).toBool()) {
//        css += QString("QTreeView:item:top {background:%1;}")
////                       "QTreeView::branch:top {background:%1;}")
//                .arg(settings()->value(Constants::S_FOREGROUNDCOLORFORROOTS).toString());
//    }
    tree->setStyleSheet(css);

    connect(tree, SIGNAL(clicked(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));
    connect(tree, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePressed(QModelIndex)));
//    connect(tree, SIGNAL(activated(QModelIndex)), this, SLOT(setCurrentEpisode(QModelIndex)));
//    connect(d->m_FileTree, SIGNAL(addRequested()), this, SLOT(newEpisode()));
//    connect(d->m_FileTree, SIGNAL(removeRequested()), this, SLOT(removeEpisode()));

    Core::Command *cmd = actionManager()->command(Constants::A_ADDEPISODE);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(newEpisode()));
    cmd = actionManager()->command(Constants::A_ADDFORM);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(addForm()));
    cmd = actionManager()->command(Constants::A_PRINTFORM);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(printCurrentItem()));


//    connect(d->m_FileTree, SIGNAL(customContextMenuRequested(QPoint)),
//            this, SLOT(contextMenuRequested(QPoint)));

    // create the central view
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

    // TODO: add the episode table view
//    d->m_EpisodesTable = new QTableView(this);
//    d->m_EpisodesTable->horizontalHeader()->hide();
//    d->m_EpisodesTable->verticalHeader()->hide();

    d->horizSplitter->addWidget(d->m_FileTree);
//    d->horizSplitter->addWidget(wb);
//    vertic->addWidget(d->m_EpisodesTable);
	vertic->addWidget(w);
    d->horizSplitter->addWidget(vertic);

    int width = size().width();
    int third = width/3;
    d->horizSplitter->setSizes(QList<int>() << third << width-third);

//    vertic->setStretchFactor(0, 1);
//    vertic->setStretchFactor(1, 3);

    d->m_GeneralLayout->addWidget(d->horizSplitter, 100, 0);
}

FormPlaceHolder::~FormPlaceHolder()
{
//    d->saveSettings();
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
    if (d->m_EpisodeModel)
        d->m_EpisodeModel->setObjectName(name+"EpisodeModel");
//    QList<QVariant> sizesVar = settings()->value(QString("%1/%2").arg(Constants::S_PLACEHOLDERSPLITTER_SIZES).arg(objectName())).toList();
//    QList<int> sizes;
//    foreach(const QVariant &v, sizesVar) {
//        if (!v.isNull() && v.toInt() > 0)
//            sizes << v.toInt();
//    }
//    if (!sizes.isEmpty()) {
//        d->horizSplitter->setSizes(sizes);
//    } else {
//        int width = size().width();
//        int third = width/3;
//        QList<int> s;
//        s << third << width-third;
//        QList<QVariant> v;
//        v << third << width-third;
//        d->horizSplitter->setSizes(s);
//        settings()->setValue(QString("%1/%2").arg(Constants::S_PLACEHOLDERSPLITTER_SIZES).arg(objectName()), v);
//    }
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
    // TODO: before deleting -> clear stackedLayout of FormPlaceHolder ?
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
//    d->m_EpisodeModel = new EpisodeModel(rootForm, this);
//    d->m_EpisodeModel->setObjectName(objectName()+"EpisodeModel");
//    d->m_Delegate->setEpisodeModel(d->m_EpisodeModel);
    FormTreeModel *formTreeModel = new FormTreeModel(rootForm, this);
    formTreeModel->init();
    QTreeView *tree = d->m_FileTree->treeView();
//    tree->setModel(d->m_EpisodeModel);
    tree->setModel(formTreeModel);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    for(int i=0; i < Form::EpisodeModel::MaxData; ++i)
        tree->setColumnHidden(i, true);
    tree->setColumnHidden(Form::EpisodeModel::Label, false);
    tree->setColumnHidden(Form::EpisodeModel::EmptyColumn1, false);
    tree->header()->hide();
    tree->header()->setStretchLastSection(false);
    tree->header()->setResizeMode(Form::EpisodeModel::Label, QHeaderView::Stretch);
    tree->header()->setResizeMode(Form::EpisodeModel::EmptyColumn1, QHeaderView::Fixed);
    tree->header()->resizeSection(Form::EpisodeModel::EmptyColumn1, 16);

    tree->expandAll();
    d->populateStackLayout();

    Core::Command *cmd = actionManager()->command(Constants::A_SHOWPATIENTLASTEPISODES);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(showLastEpisodeSynthesis()));

    // start on the Last Episode Synthesis view
//    tree->setCurrentIndex(d->m_EpisodeModel->index(0,0));
    setCurrentForm(Constants::PATIENTLASTEPISODES_UUID);
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
            // TODO: validateEpisode
//            validateEpisode();
        }

        // work around a bug in itemviews where the delegate wouldn't get the QStyle::State_MouseOver
        QPoint cursorPos = QCursor::pos();
        QTreeView *tree = d->m_FileTree->treeView();
        QWidget *vp = tree->viewport();
        QMouseEvent e(QEvent::MouseMove, vp->mapFromGlobal(cursorPos), cursorPos, Qt::NoButton, 0, 0);
        QCoreApplication::sendEvent(vp, &e);
    } else if(index.column() == EpisodeModel::Label) {
        setCurrentEpisode(index);
    }
}

QTreeView *FormPlaceHolder::formTree() const
{
    return d->m_FileTree->treeView();
}

QStackedLayout *FormPlaceHolder::formStackLayout() const
{
    return d->m_Stack;
}

void FormPlaceHolder::addBottomWidget(QWidget *bottom)
{
    d->m_GeneralLayout->addWidget(bottom, d->m_GeneralLayout->rowCount(), 0, 0, d->m_GeneralLayout->columnCount());
}

void FormPlaceHolder::showLastEpisodeSynthesis()
{
    setCurrentForm(Constants::PATIENTLASTEPISODES_UUID);
}

void FormPlaceHolder::setCurrentForm(const QString &formUuid)
{
    // change the stack and populate as needed
    d->m_Stack->setCurrentIndex(d->m_StackId_FormUuid.key(formUuid));
    if (d->m_Stack->currentWidget()) {
        if (formUuid==Constants::PATIENTLASTEPISODES_UUID) {
            if (!d->m_EpisodeModel)
                return;
            qApp->setOverrideCursor(QCursor(Qt::BusyCursor));
            QTextBrowser *browser = d->m_Stack->currentWidget()->findChild<QTextBrowser*>();
            browser->setText(d->m_EpisodeModel->lastEpisodesSynthesis());
            qApp->restoreOverrideCursor();
        }
    }
}

void FormPlaceHolder::setCurrentEpisode(const QModelIndex &index)
{
//    d->m_LastEpisode = index;
    if (!d) {
        return;
    }
    if (!d->m_EpisodeModel) {
        return;
    }

    const QString &formUuid = d->m_EpisodeModel->index(index.row(), EpisodeModel::FormUuid, index.parent()).data().toString();
    if (formUuid==Constants::PATIENTLASTEPISODES_UUID && d->m_Stack->currentIndex()==0) {
        return;
    }
    setCurrentForm(formUuid);
    bool isEpisode = d->m_EpisodeModel->isEpisode(index);
    if (isEpisode) {
        qobject_cast<QScrollArea*>(d->m_Stack->currentWidget())->widget()->setEnabled(true);
        d->m_EpisodeModel->activateEpisode(index, formUuid);
    } else {
        // TODO: code here : show HtmlSynthesis in a label
        d->m_EpisodeModel->activateEpisode(QModelIndex(), formUuid);
    }
    foreach(Form::FormMain *form, d->m_RootForm->flattenFormMainChildren()) {
        if (form->uuid()==formUuid) {
            form->formWidget()->setEnabled(isEpisode);
            break;
        }
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
    if (!d->m_FileTree->selectionModel()->hasSelection())
        return;
    // get the parent form
    QModelIndex index;
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
        LOG_ERROR("Unable to create new episode");
        return;
    }
    // activate the newly created main episode
    d->m_FileTree->selectionModel()->clearSelection();
    d->m_FileTree->selectionModel()->setCurrentIndex(d->m_EpisodeModel->index(0,0,index), QItemSelectionModel::Select | QItemSelectionModel::Rows);
    const QString &formUuid = d->m_EpisodeModel->index(index.row(), Form::EpisodeModel::FormUuid, index.parent()).data().toString();
    setCurrentForm(formUuid);
    qobject_cast<QScrollArea*>(d->m_Stack->currentWidget())->widget()->setEnabled(true);
    d->m_EpisodeModel->activateEpisode(d->m_EpisodeModel->index(0,0,index), formUuid);
    foreach(Form::FormMain *form, d->m_RootForm->flattenFormMainChildren()) {
        if (form->uuid()==formUuid) {
            form->formWidget()->setEnabled(true);
            break;
        }
    }
}

void FormPlaceHolder::removeEpisode()
{
    if (!d->m_FileTree->selectionModel())
        return;
    // TODO: code here: removeEpisode
}

void FormPlaceHolder::validateEpisode()
{
    if (!d->m_FileTree->selectionModel())
        return;
    // TODO: code here: validateEpisode
}

void FormPlaceHolder::addForm()
{
    if (!isVisible())
        return;
    // save current episode
    if (d->m_FileTree->selectionModel()->hasSelection()) {
        // something to save ?
        QModelIndex index = d->m_FileTree->selectionModel()->selectedIndexes().at(0);
        if (d->m_EpisodeModel->isEpisode(index)) {
            // get the parent form
            QModelIndex formIndex = index.parent();
            while (!d->m_EpisodeModel->isForm(formIndex)) {
                formIndex = formIndex.parent();
            }
            d->m_EpisodeModel->saveEpisode(d->m_FileTree->currentIndex(), d->m_EpisodeModel->index(formIndex.row(), EpisodeModel::FormUuid, formIndex.parent()).data().toString());
        }
    }
    // open the form editor dialog
    FormEditorDialog dlg(d->m_EpisodeModel, FormEditorDialog::DefaultMode, this);
    if (dlg.exec()==QDialog::Accepted) {
        // refresh stack widget
        d->populateStackLayout();
        // activate last episode synthesis
        d->m_FileTree->treeView()->setCurrentIndex(d->m_EpisodeModel->index(0,0));
        showLastEpisodeSynthesis();
//        d->m_Stack->setCurrentIndex(d->m_StackId_FormUuid.key(dlg.lastInsertedFormUid()));
    }
}

void FormPlaceHolder::printCurrentItem()
{
//    qWarning() << Q_FUNC_INFO;
    if (!d->m_FileTree->selectionModel())
        return;

    // get the current index
    QTreeView *tree = d->m_FileTree->treeView();
    QModelIndex index = tree->rootIndex();
    if (tree->selectionModel()->hasSelection())
        index = tree->selectionModel()->selectedIndexes().at(0);
    QModelIndex form = index;
    while (!d->m_EpisodeModel->isForm(form)) {
        form = form.parent();
    }
    if (!form.isValid())
        return;

    QString htmlToPrint;
    QString title;
    QModelIndex formUid = d->m_EpisodeModel->index(form.row(), Form::EpisodeModel::FormUuid, form.parent());
    if (formUid.data().toString()==Constants::PATIENTLASTEPISODES_UUID) {
        // Print patient synthesis
        htmlToPrint = d->m_EpisodeModel->lastEpisodesSynthesis();
        title = QApplication::translate(Constants::FORM_TR_CONTEXT, Constants::PATIENTLASTEPISODES_TEXT);
    } else {
        // Print episode
        if (d->m_EpisodeModel->isEpisode(index))
            setCurrentEpisode(index);
        Form::FormMain *formMain = d->m_EpisodeModel->formForIndex(form);
        if (formMain) {
            htmlToPrint = "<html><body>" + formMain->printableHtml(d->m_EpisodeModel->isEpisode(index)) + "</body></html>";
            title = formMain->spec()->label();
        }
    }

    if (htmlToPrint.isEmpty())
        return;

    Core::IDocumentPrinter *p = printer();
    if (!p) {
        LOG_ERROR("No IDocumentPrinter found");
        return;
    }
    p->clearTokens();
    QHash<QString, QVariant> tokens;

    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, title);
//    // create a token for each FormItem of the FormMain
//    foreach(FormItem *item, formMain->flattenFormItemChildren()) {
//        if (item->itemDatas())
//            tokens.insert(item->uuid(), item->itemDatas()->data(0, Form::IFormItemData::ID_Printable));
//    }
    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);

    // print
    p->print(htmlToPrint, Core::IDocumentPrinter::Papers_Generic_User, false);

//    qWarning() << formMain->uuid();

//    QDialog dlg;
//    QTextBrowser b(&dlg);
//    QVBoxLayout l(&dlg);
//    dlg.setLayout(&l);
//    l.addWidget(&b);
//    b.setHtml("<html><body>" + formMain->printableHtml(d->m_EpisodeModel->isEpisode(index)) + "</body></html>");
//    dlg.exec();

//    qWarning() << formMain->printableHtml(d->m_EpisodeModel->isEpisode(index));
}

void FormPlaceHolder::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        // if showing patient synthesis or last episode -> retranslate by querying the model
        QTreeView *tree = d->m_FileTree->treeView();
        if (tree->selectionModel()) {
            QModelIndex index = tree->selectionModel()->currentIndex();
            const QString &formUuid = d->m_EpisodeModel->index(index.row(), EpisodeModel::FormUuid, index.parent()).data().toString();
            if (formUuid==Constants::PATIENTLASTEPISODES_UUID) {
                setCurrentForm(formUuid);
            }
        }
    }
    QWidget::changeEvent(event);
}
