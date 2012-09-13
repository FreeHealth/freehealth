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
  and episode data. Data are automatically saved (without any user intervention).
*/

#include "formplaceholder.h"
#include "constants_settings.h"
#include "constants_db.h"
#include "formeditordialog.h"
#include "formtreemodel.h"

#include "ui_formplaceholder.h"

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
#include <utils/global.h>
#include <utils/widgets/minisplitter.h>
#include <utils/widgets/datetimedelegate.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>

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
#include <QItemSelectionModel>
#include <QToolBar>
#include <QFileDialog>

// Test
#include <QTextBrowser>

#include <QDebug>

using namespace Form;
using namespace Trans::ConstantTranslations;

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
        ui(new Ui::FormPlaceHolder),
            _formMain(0),
            _formTreeModel(0),
            _delegate(0),
            _episodeToolBar(0),
            aNewEpisode(0),
            aValidateEpisode(0),
            aRemoveEpisode(0),
            aTakeScreenShot(0),
            q(parent)
    {
    }

    ~FormPlaceHolderPrivate()
    {
        delete ui;
    }

    void createEpisodeToolBar()
    {
        _episodeToolBar = new QToolBar(q);
        _episodeToolBar->setIconSize(QSize(16,16));

        Core::Command *cmd = actionManager()->command(Constants::A_ADDEPISODE);
        aNewEpisode = cmd->action();
        QObject::connect(cmd->action(), SIGNAL(triggered()), q, SLOT(newEpisode()));
        _episodeToolBar->addAction(cmd->action());

        QAction *a = aRemoveEpisode = new QAction(q);
        a->setIcon(theme()->icon(Core::Constants::ICONREMOVE));
        QObject::connect(a, SIGNAL(triggered()), q, SLOT(removeEpisode()));
        aRemoveEpisode->setEnabled(false);
        _episodeToolBar->addAction(aRemoveEpisode);

        _episodeToolBar->addSeparator();
        a = aValidateEpisode = new QAction(q);
        a->setIcon(theme()->icon(Core::Constants::ICONOK));
        QObject::connect(a, SIGNAL(triggered()), q, SLOT(validateEpisode()));
        aValidateEpisode->setEnabled(false);
        _episodeToolBar->addAction(aValidateEpisode);

        _episodeToolBar->addSeparator();
        a = aSaveEpisode = actionManager()->command(Core::Constants::A_FILE_SAVE)->action();
        QObject::connect(a, SIGNAL(triggered()), q, SLOT(saveCurrentEditingEpisode()));
        a->setEnabled(false);
        _episodeToolBar->addAction(a);

        a = aPrintForm = actionManager()->command(Constants::A_PRINTFORM)->action();
        QObject::connect(a, SIGNAL(triggered()), q, SLOT(printCurrentItem()));
        a->setEnabled(false);
        _episodeToolBar->addAction(a);

        a = aTakeScreenShot = new QAction(q);
        a->setIcon(theme()->icon(Core::Constants::ICONTAKESCREENSHOT));
        QObject::connect(a, SIGNAL(triggered()), q, SLOT(takeScreenShotEpisode()));
        _episodeToolBar->addAction(aTakeScreenShot);

        ui->toolbarLayout->addWidget(_episodeToolBar);
    }

    bool isAutosaveOn()
    {
        return settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, false).toBool();
    }

    bool saveCurrentEditingEpisode()
    {
        // Something to save?
        if (!ui->formDataMapper->isDirty())
            return true;
        // Autosave or ask user?
        if (!isAutosaveOn()) {
            // Ask user
            bool save = Utils::yesNoMessageBox(QApplication::translate("Form::FormPlaceHolder",
                                                                       "Save episode?"),
                                               QApplication::translate("Form::FormPlaceHolder",
                                                                       "The actual episode has been modified. Do you want to save changes in your database?\n"
                                                                       "Answering 'No' will cause definitve data lose."),
                                               "", QApplication::translate("Form::FormPlaceHolder", "Save episode"));
            if (!save)
                return false;
        }
        return ui->formDataMapper->submit();
    }

    void setEpisodeViewVisibility(bool visible)
    {
        ui->episodeView->setVisible(visible);
    }

    void updateEpisodeActions(const QModelIndex &index)
    {
        const EpisodeModel *model = qobject_cast<EpisodeModel*>(ui->episodeView->model());
        const bool enabled = index.isValid();
        const bool unique = _formTreeModel->isUniqueEpisode(index);
        const bool modelHasOneRowAtLeast = model->rowCount()>=1;

        aSaveEpisode->setEnabled(enabled);
        aRemoveEpisode->setEnabled(enabled && !unique);
        if (enabled) {
            Q_ASSERT(model);
            aValidateEpisode->setEnabled(!model->isEpisodeValidated(index));
            if (unique)
                aNewEpisode->setEnabled(!modelHasOneRowAtLeast);
            else
                aNewEpisode->setEnabled(true);
        } else {
            aValidateEpisode->setEnabled(enabled);
            aSaveEpisode->setEnabled(enabled);
            aNewEpisode->setEnabled(enabled);
        }
        aPrintForm->setEnabled(enabled);
    }

public:
    Ui::FormPlaceHolder *ui;
    FormMain *_formMain;
    FormTreeModel *_formTreeModel;
    FormItemDelegate *_delegate;
    QToolBar *_episodeToolBar;
    QAction *aNewEpisode, *aValidateEpisode, *aRemoveEpisode, *aTakeScreenShot, *aSaveEpisode, *aPrintForm;
    QHash<int, QString> m_StackId_FormUuid;

private:
    FormPlaceHolder *q;
};

FormItemDelegate::FormItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    _formTreeModel(0)
{
}

void FormItemDelegate::setFormTreeModel(FormTreeModel *model)
{
    _formTreeModel = model;
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
    if (index.column()==FormTreeModel::EmptyColumn1 &&
            (option.state & QStyle::State_MouseOver)) {
        QIcon icon;
        if (option.state & QStyle::State_Selected) {
            // test the form to be unique or multiple episode
            if (_formTreeModel->isUniqueEpisode(index))
                return;
            if (_formTreeModel->isNoEpisode(index))
                return;
            icon = theme()->icon(Core::Constants::ICONADDLIGHT);
        } else {
            // test the form to be unique or multiple episode
            if (_formTreeModel->isUniqueEpisode(index))
                return;
            if (_formTreeModel->isNoEpisode(index))
                return;
            icon = theme()->icon(Core::Constants::ICONADDDARK);
        }

        QRect iconRect(option.rect.right() - option.rect.height(),
                       option.rect.top(),
                       option.rect.height(),
                       option.rect.height());

        icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
    }
}

}  // namespace Internal
}  // namespace Form

FormPlaceHolder::FormPlaceHolder(QWidget *parent) :
        FormContextualWidget(parent),
        d(new Internal::FormPlaceHolderPrivate(this))
{
    FormManager::instance();
    d->ui->setupUi(this);
    layout()->setMargin(0);
    layout()->setSpacing(0);
    d->ui->verticalLayout_2->setMargin(0);
    d->ui->verticalLayout_2->setSpacing(0);
    d->createEpisodeToolBar();

    d->_delegate = new Internal::FormItemDelegate(d->ui->formView);
    d->ui->formDataMapper->initialize();

    // Manage Form File tree view
    d->ui->formView->setActions(0);
    d->ui->formView->setCommands(QStringList()
                                 << Constants::A_ADDFORM
                                 );
    d->ui->formView->addContexts(contexts());
    d->ui->formView->setDeselectable(false);
    d->ui->formView->disconnectActionsToDefaultSlots();
    d->ui->formView->treeView()->viewport()->setAttribute(Qt::WA_Hover);
    d->ui->formView->treeView()->setItemDelegate(d->_delegate);
    d->ui->formView->treeView()->setFrameStyle(QFrame::NoFrame);
    d->ui->formView->treeView()->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->ui->formView->treeView()->setSelectionMode(QAbstractItemView::SingleSelection);
    d->ui->formView->treeView()->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->ui->formView->treeView()->setAlternatingRowColors(settings()->value(Constants::S_USEALTERNATEROWCOLOR).toBool());
    d->ui->formView->treeView()->setStyleSheet(::TREEVIEW_SHEET);

    connect(d->ui->formView, SIGNAL(clicked(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));
    connect(d->ui->formView, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePressed(QModelIndex)));

    d->ui->episodeView->verticalHeader()->hide();
    d->ui->episodeView->setFrameStyle(QFrame::NoFrame);
    d->ui->episodeView->setSelectionMode(QAbstractItemView::SingleSelection);
    d->ui->episodeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->ui->episodeView->setItemDelegateForColumn(EpisodeModel::UserDate, new Utils::DateTimeDelegate(this, true));

    int width = size().width();
    int third = width/3;
    d->ui->horizontalSplitter->setSizes(QList<int>() << third << width-third);

    int height = size().height();
    third = height/5;
    d->ui->verticalSplitter->setSizes(QList<int>() << third << height-third);

    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(clearAll()));
}

FormPlaceHolder::~FormPlaceHolder()
{
    if (d) {
        delete d;
        d = 0;
    }
}

/**
 * Define the Form::FormMain root item to use for the creation of the patient files.
 * This object will manage deletion of the root item and its children.
 */
void FormPlaceHolder::setRootForm(Form::FormMain *rootForm)
{
    if (!rootForm)
        return;
    d->_formMain = rootForm;

    // Manage Form tree view / model
    if (d->_formTreeModel)
        delete d->_formTreeModel;
    d->_formTreeModel = new FormTreeModel(rootForm, this);
    d->_formTreeModel->initialize();
    d->ui->formView->setModel(d->_formTreeModel);
    d->_delegate->setFormTreeModel(d->_formTreeModel);

    QTreeView *tree = d->ui->formView->treeView();
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    for(int i=0; i < FormTreeModel::MaxData; ++i)
        tree->setColumnHidden(i, true);
    tree->setColumnHidden(FormTreeModel::Label, false);
    tree->setColumnHidden(FormTreeModel::EmptyColumn1, false);
    tree->header()->hide();
    tree->header()->setStretchLastSection(false);
    tree->header()->setResizeMode(FormTreeModel::Label, QHeaderView::Stretch);
    tree->header()->setResizeMode(FormTreeModel::EmptyColumn1, QHeaderView::Fixed);
    tree->header()->resizeSection(FormTreeModel::EmptyColumn1, 16);
    tree->expandAll();

    Core::Command *cmd = actionManager()->command(Constants::A_SHOWPATIENTLASTEPISODES);
    connect(cmd->action(), SIGNAL(triggered()), this, SLOT(showLastEpisodeSynthesis()));

    // Select the first available form in the tree model
    if (rootForm->firstLevelFormMainChildren().count() > 0) {
        setCurrentForm(rootForm->firstLevelFormMainChildren().at(0));
        QModelIndex index = d->_formTreeModel->index(0,0);
        d->ui->formView->selectionModel()->select(index, QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
    }
}

void FormPlaceHolder::clearAll()
{
    if (d->_formMain)
        d->_formMain->clear();
}

// Used for the delegate
void FormPlaceHolder::handlePressed(const QModelIndex &index)
{
    if (index.column() == FormTreeModel::Label) {
//        d->_episodeModel->activateEpisode(index);
    } else if (index.column() == FormTreeModel::EmptyColumn1) {
        d->_delegate->pressedIndex = index;
    }
}

// Used for the delegate
void FormPlaceHolder::handleClicked(const QModelIndex &index)
{
    setCurrentEditingItem(index);
    if (index.column() == FormTreeModel::EmptyColumn1) { // the funky button
        if (!d->_formTreeModel->isNoEpisode(index))
            newEpisode();

        // work around a bug in itemviews where the delegate wouldn't get the QStyle::State_MouseOver
        QPoint cursorPos = QCursor::pos();
        QTreeView *tree = d->ui->formView->treeView();
        QWidget *vp = tree->viewport();
        QMouseEvent e(QEvent::MouseMove, vp->mapFromGlobal(cursorPos), cursorPos, Qt::NoButton, 0, 0);
        QCoreApplication::sendEvent(vp, &e);
    }
}

void FormPlaceHolder::showLastEpisodeSynthesis()
{
    qWarning() << "showMastEpisode";
    setCurrentForm(Constants::PATIENTLASTEPISODES_UUID);
}

/**
 * Define the current editing form.
 * \sa setCurrentEditingItem()
 */
void FormPlaceHolder::setCurrentForm(Form::FormMain *form)
{
    if (form)
        qWarning() << "FormPlaceHolder::setCurrentForm" << form->uuid();
    else
        qWarning() << "FormPlaceHolder::setCurrentForm (0x0)";

    if (d->ui->episodeView->selectionModel())
        disconnect(d->ui->episodeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(episodeChanged(QModelIndex, QModelIndex)));

    d->ui->formDataMapper->setCurrentForm(form);
    EpisodeModel *episodeModel = formManager()->episodeModel(form);
    d->ui->episodeView->setModel(episodeModel);
    for(int i=0; i < EpisodeModel::MaxData; ++i)
        d->ui->episodeView->hideColumn(i);
    d->ui->episodeView->showColumn(EpisodeModel::ValidationStateIcon);
    d->ui->episodeView->showColumn(EpisodeModel::UserDate);
    d->ui->episodeView->showColumn(EpisodeModel::Label);
    d->ui->episodeView->showColumn(EpisodeModel::UserCreatorName);
    //    d->ui->episodeView->showColumn(EpisodeModel::Uuid);

    // Unique épisode || no episode -> hide episodeview
    if (form->episodePossibilities()==FormMain::UniqueEpisode || form->episodePossibilities()==FormMain::NoEpisode) {
        d->setEpisodeViewVisibility(false);
    } else {
        d->setEpisodeViewVisibility(true);
    }

    d->ui->episodeView->horizontalHeader()->setResizeMode(EpisodeModel::ValidationStateIcon, QHeaderView::ResizeToContents);
//    d->ui->episodeView->horizontalHeader()->resizeSection(EpisodeModel::ValidationStateIcon, 22);
    d->ui->episodeView->horizontalHeader()->setResizeMode(EpisodeModel::UserDate, QHeaderView::ResizeToContents);
    d->ui->episodeView->horizontalHeader()->setResizeMode(EpisodeModel::Label, QHeaderView::Stretch);
    d->ui->episodeView->horizontalHeader()->setResizeMode(EpisodeModel::UserCreatorName, QHeaderView::ResizeToContents);
    d->ui->episodeView->horizontalHeader()->hide();

    connect(d->ui->episodeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(episodeChanged(QModelIndex, QModelIndex)));
    if (!d->ui->episodeView->selectionModel()->hasSelection()) {
        d->ui->episodeView->selectRow(0);
        if (episodeModel)
            episodeChanged(episodeModel->index(0,0), QModelIndex());
    }
}

/**
 * Define the current editing form.
 * \sa setCurrentEditingItem()
 */
void FormPlaceHolder::setCurrentForm(const QString &formUid)
{
    setCurrentForm(formManager()->form(formUid));
}

/**
 * When using a FormTreeModel you can set the current for with this.
 * \sa setCurrentForm()
 */
void FormPlaceHolder::setCurrentEditingItem(const QModelIndex &index)
{
    Form::FormMain *form = d->_formTreeModel->formForIndex(index);
    setCurrentForm(form);
}

void FormPlaceHolder::episodeChanged(const QModelIndex &current, const QModelIndex &previous)
{
    qWarning() << "FormPlaceHolder::EpisodeChanged" << current << previous;
    if (previous.isValid())
        d->saveCurrentEditingEpisode();
//    if (!current.isValid())
//        return;
    d->ui->formDataMapper->setCurrentEpisode(current);
    d->updateEpisodeActions(current);
}

/** Private slot. Creates a new episode for the current selected form */
bool FormPlaceHolder::newEpisode()
{
    if (!d->ui->formView->selectionModel())
        return false;
    if (!d->ui->formView->selectionModel()->hasSelection())
        return false;

    // get the form
    QModelIndex index = d->ui->formView->selectionModel()->selectedIndexes().at(0);
    if (d->_formTreeModel->isNoEpisode(index))
        return false;
    if (d->_formTreeModel->isUniqueEpisode(index))
        return false;
    setCurrentEditingItem(index);

    // create a new episode the selected form and its children
    Form::FormMain *form = d->_formTreeModel->formForIndex(index);
    EpisodeModel *model = formManager()->episodeModel(form);
    if (!model->insertRow(0)) {
        LOG_ERROR("Unable to create new episode");
        return false;
    }

    // activate the newly created main episode
    d->ui->episodeView->selectRow(model->rowCount() - 1);
    d->ui->formDataMapper->setCurrentEpisode(model->index(model->rowCount() - 1, EpisodeModel::Label));
    return true;
}

bool FormPlaceHolder::saveCurrentEditingEpisode()
{
    qWarning() << "FormPlaceHolder::saveCurrentEditingEpisode";
    return d->saveCurrentEditingEpisode();
}

void FormPlaceHolder::addForm()
{
    if (!isVisible())
        return;

//    d->saveCurrentEditingEpisode();

    // open the form editor dialog
//    FormEditorDialog dlg(d->_formTreeModel, FormEditorDialog::DefaultMode, this);
//    if (dlg.exec()==QDialog::Accepted) {
        // refresh stack widget
//        d->populateStackLayout();
        // activate last episode synthesis
//        d->ui->formView->setCurrentIndex(d->_episodeModel->index(0,0));
//        showLastEpisodeSynthesis();
//    }
}

/** Print the current editing episode */
void FormPlaceHolder::printCurrentItem()
{
//    qWarning() << Q_FUNC_INFO;
    if (!d->ui->formView->selectionModel())
        return;

    // get the current index
//    QTreeView *tree = d->ui->formView->treeView();
//    QModelIndex index = tree->rootIndex();
//    if (tree->selectionModel()->hasSelection())
//        index = tree->selectionModel()->selectedIndexes().at(0);
//    QModelIndex form = index;
//    while (!d->_episodeModel->isForm(form)) {
//        form = form.parent();
//    }
//    if (!form.isValid())
//        return;


//    QModelIndex formUid = d->_episodeModel->index(form.row(), Form::FormTreeModel::FormUuid, form.parent());
//    if (formUid.data().toString()==Constants::PATIENTLASTEPISODES_UUID) {
//        // Print patient synthesis
//        htmlToPrint = d->_episodeModel->lastEpisodesSynthesis();
//        title = QApplication::translate(Constants::FORM_TR_CONTEXT, Constants::PATIENTLASTEPISODES_TEXT);
//    } else {
//        // Print episode
//        if (d->_episodeModel->isEpisode(index))
//            setCurrentEpisode(index);
//        Form::FormMain *formMain = d->_episodeModel->formForIndex(form);
//        if (formMain) {
//                htmlToPrint = "<html><body>" + formMain->printableHtml(d->_episodeModel->isEpisode(index)) + "</body></html>";
//                title = formMain->spec()->label();
//        }
//    }

    Form::FormMain *formMain = d->_formTreeModel->formForIndex(d->ui->formView->currentIndex());
    if (!formMain)
        return;

    QString htmlToPrint;
    QString title;
    htmlToPrint = "<html><body>" + formMain->printableHtml(true) + "</body></html>";
    title = formMain->spec()->label();

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
}

/**
 * Validate the currently selected episode
 * \sa Form::EpisodeModel::validateEpisode()
 */
void FormPlaceHolder::validateEpisode()
{
    if (!d->ui->episodeView->selectionModel()->hasSelection())
        return;

    // message box
    bool yes = Utils::yesNoMessageBox(tr("Validate the current episode"),
                                      tr("When you validate an episode, you prevent all subsequent amendments. "
                                         "The episode will be shown but will be kept unchanged.\n"
                                         "Do you really want to validate the current episode?"));
    if (!yes)
        return;

    // get the episodeModel corresponding to the currently selected form
    Form::FormMain *form = d->_formTreeModel->formForIndex(d->ui->formView->selectionModel()->currentIndex());
    EpisodeModel *model = formManager()->episodeModel(form);
    if (!model)
        return;
    model->validateEpisode(d->ui->episodeView->currentIndex());
}

/**
 * Remove the currently selected episode
 * \sa Form::EpisodeModel::removeEpisode()
 */
void FormPlaceHolder::removeEpisode()
{
    // message box
    bool yes = Utils::yesNoMessageBox(tr("Remove the current episode"),
                                      tr("You can not completely destroy an episode, "
                                         "but you can remove it from the views.\n"
                                         "The episode will not be shown anymore, but will still be "
                                         "included in the database.\n"
                                         "Do you really want to remove the current episode?"));
    if (!yes)
        return;
}

void FormPlaceHolder::takeScreenShotEpisode()
{
    QPixmap pix = QPixmap::grabWidget(d->ui->formDataMapper);
    QString fileName = QFileDialog::getSaveFileName(this, tkTr(Trans::Constants::SAVE_FILE),
                                                    settings()->path(Core::ISettings::UserDocumentsPath),
                                                    tr("Images (*.png)"));
    if (!fileName.isEmpty()) {
        QFileInfo info(fileName);
        if (info.completeSuffix().isEmpty())
            fileName.append(".png");
        pix.save(fileName);
    }
}

void FormPlaceHolder::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        if (d->aRemoveEpisode)
            d->aRemoveEpisode->setText(tkTr(Trans::Constants::REMOVE_EPISODE));
        if (d->aValidateEpisode)
            d->aValidateEpisode->setText(tkTr(Trans::Constants::VALIDATE_EPISODE));
        if (d->aTakeScreenShot)
            d->aTakeScreenShot->setText(tkTr(Trans::Constants::TAKE_SCREENSHOT));
        // if showing patient synthesis or last episode -> retranslate by querying the model
//        QTreeView *tree = d->ui->formView->treeView();
//        if (tree->selectionModel()) {
//            QModelIndex index = tree->selectionModel()->currentIndex();
//            const QString &formUuid = d->_episodeModel->index(index.row(), FormTreeModel::FormUuid, index.parent()).data().toString();
//            if (formUuid==Constants::PATIENTLASTEPISODES_UUID) {
//                setCurrentForm(formUuid);
//            }
//        }
    }
    QWidget::changeEvent(event);
}
