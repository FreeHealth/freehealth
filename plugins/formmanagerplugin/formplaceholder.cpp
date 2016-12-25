/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Form::FormPlaceHolder
 * Widget containing these objects:
 *   * formView (form tree view, class Views::TreeView)
 *   * episodeToolBar inside the toolbarLayout
 *   * episodeView (class QTableView)
 *   * formDataMapper (class Form::FormDataWidgetMapper)
 * When an episode is activated by the user, the formDataMapper is set to the
 * corresponding episode data.
 * Data are automatically saved (without any user intervention).
 *
 * +--------------+-----------------------------------------+
 * | formView     |  episodeToolBar                         |
 * |              +-----------------------------------------|
 * |              |             episodeView                 |
 * |              |                                         |
 * |              +-----------------------------------------|
 * |              |                                         |
 * |              |           formDataMapper                |
 * |              |                                         |
 * |              |                                         |
 * |              |                                         |
 * |              |                                         |
 * +--------------+-----------------------------------------+
*/

#include "formplaceholder.h"
#include "formcore.h"
#include "constants_settings.h"
#include "constants_db.h"
#include "formeditordialog.h"
#include "formtreemodel.h"
#include "formdatawidgetmapper.h"
#include "formviewdelegate.h"
#include "subforminsertionpoint.h"
#include "formmanager.h"
#include "iformitem.h"
#include "iformitemdata.h"
#include "iformwidgetfactory.h"
#include "ui_formplaceholder.h"
#include "episodemanager.h"
#include "episodemodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/ipatientbar.h>
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
#include <translationutils/trans_current.h>
#include <translationutils/trans_filepathxml.h>

#include <QTreeView>
#include <QTreeWidgetItem>
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
#include <QSortFilterProxyModel>
#include <QToolTip>

#include <QDebug>

using namespace Form;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Form::EpisodeManager &episodeManager() {return Form::FormCore::instance().episodeManager();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::IDocumentPrinter *printer() {return ExtensionSystem::PluginManager::instance()->getObject<Core::IDocumentPrinter>();}

//static void warnPatient()
//{
//    for(int i=0; i < Core::IPatient::NumberOfColumns; ++i) {
//        qWarning() << patient()->enumToString(Core::IPatient::PatientDataRepresentation(i)) << patient()->data(i);
//    }
//}

namespace Form {
namespace Internal {

FormPlaceHolderCoreListener::FormPlaceHolderCoreListener(FormPlaceHolder *parent) :
    Core::ICoreListener(parent),
    _formPlaceHolder(parent)
{
    Q_ASSERT(parent);
    setObjectName("FormPlaceHolderCoreListener");
}

FormPlaceHolderCoreListener::~FormPlaceHolderCoreListener() {}

bool FormPlaceHolderCoreListener::coreAboutToClose()
{
    _errorMsg.clear();
    if (_formPlaceHolder->isDirty()) {
        bool ok = _formPlaceHolder->saveCurrentEpisode();
        if (!ok) {
            _errorMsg = tr("Unable to save current episode, form: %1")
                    .arg(_formPlaceHolder->currentFormLabel());
            LOG_ERROR(_errorMsg);
        }
        return ok;
    }
    return true;
}

FormPlaceHolderPatientListener::FormPlaceHolderPatientListener(FormPlaceHolder *parent) :
    Core::IPatientListener(parent),
    _formPlaceHolder(parent)
{
    Q_ASSERT(parent);
    setObjectName("FormPlaceHolderPatientListener");
}

FormPlaceHolderPatientListener::~FormPlaceHolderPatientListener() {}

bool FormPlaceHolderPatientListener::currentPatientAboutToChange()
{
    _errorMsg.clear();
    if (_formPlaceHolder->isDirty()) {
        bool ok = _formPlaceHolder->saveCurrentEpisode();
        if (!ok) {
            _errorMsg = tr("Unable to save current episode, form: %1")
                    .arg(_formPlaceHolder->currentFormLabel());
            LOG_ERROR(_errorMsg);
        }
        return ok;
    }
    return true;
}

class FormPlaceHolderPrivate
{
public:
    FormPlaceHolderPrivate(FormPlaceHolder *parent) :
        ui(new Ui::FormPlaceHolder),
        _formTreeModel(0),
        _delegate(0),
        _episodeToolBar(0),
        _coreListener(0),
        _patientListener(0),
        _proxyModel(0),
        _currentEpisodeModel(0),
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

        QStringList actions;
        actions << Constants::A_ADDEPISODE
                << Constants::A_RENEWEPISODE
                << "--"
                << Constants::A_REMOVEEPISODE
                << "--"
                << Constants::A_VALIDATEEPISODE
                << "--"
                << Core::Constants::A_FILE_SAVE
                << Core::Constants::A_FILE_PRINT
                << "--"
                << Constants::A_TAKESCREENSHOT;

        Core::Command *cmd = 0;

        foreach(const QString &action, actions) {
            // Actions are created, managed and connected in the Form::Internal::FormActionHandler
            // We just need to add the user visible actions in the toolbar
            if (action=="--") {
                _episodeToolBar->addSeparator();
                continue;
            }
            cmd = actionManager()->command(Core::Id(action));
            _episodeToolBar->addAction(cmd->action());
        }

        ui->toolbarLayout->addWidget(_episodeToolBar);
    }

    bool isAutosaveOn()
    {
        return settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, false).toBool();
    }

    bool saveCurrentEditingEpisode()
    {
        if (!ui->formDataMapper->currentEditingEpisodeIndex().isValid()) {
            LOG_FOR(q, "Episode not saved, no current editing episode");
            qDebug() << "Episode not saved, no current editing episode";

            return true;
        }

        // Something to save?
        if (!ui->formDataMapper->isDirty()) {
            LOG_FOR(q, "Episode not saved, episode is not dirty");
            qDebug() << "Episode not saved, episode is not dirty";

            return true;
        }

        // Autosave or ask user?
        if (!settings()->value(Core::Constants::S_ALWAYS_SAVE_WITHOUT_PROMPTING, false).toBool()) {
            // Ask user
            bool save = Utils::yesNoMessageBox(QApplication::translate("Form::FormPlaceHolder",
                                                                       "Save episode?"),
                                               QApplication::translate("Form::FormPlaceHolder",
                                                                       "The current episode has been modified. Do you want to save changes?\n"
                                                                       "Answering 'No' will cause definitive data loss."),
                                               "", QApplication::translate("Form::FormPlaceHolder", "Save episode"));
            if (!save)
                return false;
        }
        patient()->patientBar()->showMessage(QApplication::translate("Form::FormPlaceHolder", "Saving episode (%1) from form (%2)")
                                             .arg(ui->formDataMapper->currentEpisodeLabel())
                                             .arg(ui->formDataMapper->currentFormName()));
        bool ok = ui->formDataMapper->submit();
        if (!ok) {
            patient()->patientBar()->showMessage(QApplication::translate("Form::FormPlaceHolder", "WARNING: Episode (%1) from form (%2) cannot be saved")
                                                 .arg(ui->formDataMapper->currentEpisodeLabel())
                                                 .arg(ui->formDataMapper->currentFormName()));
            qDebug() << QString("WARNING: Episode (%1) from form (%2) cannot be saved").arg(ui->formDataMapper->currentEpisodeLabel())
                        .arg(ui->formDataMapper->currentFormName());
        }
        return ok;
    }

    /**
     * Save currently selected episode even if it's not dirty.
     * Used as the last step of episode creation so that an associated script
     * is able to use current (empty) widget data and not dangling widget data
     * from previous episode.
     * See Git issue #70
     * \sa FormPlaceHolder::createEpisode()
     */
    bool forceSaveCurrentEditingEpisode()
    {
        if (!ui->formDataMapper->currentEditingEpisodeIndex().isValid()) {
            LOG_FOR(q, "Episode not saved, no current editing episode");
            qDebug() << "Episode not saved, no current editing episode";
            return false;
        }
        bool ok = ui->formDataMapper->submit();
        if (!ok) {
            patient()->patientBar()->showMessage(QApplication::translate("Form::FormPlaceHolder", "WARNING: Episode (%1) from form (%2) cannot be saved")
                                                 .arg(ui->formDataMapper->currentEpisodeLabel())
                                                 .arg(ui->formDataMapper->currentFormName()));
            qDebug() << QString("WARNING: Episode (%1) from form (%2) cannot be saved").arg(ui->formDataMapper->currentEpisodeLabel())
                        .arg(ui->formDataMapper->currentFormName());
        }
        return ok;
    }

    void checkCurrentEpisodeViewVisibility()
    {
        if (!_formTreeModel)
            return;
        // Assuming the _currentEditingForm is defined and the episodeView model is set
        bool visible = true;

        if (_formTreeModel->isNoEpisode(_currentEditingForm))
            visible = false;
        ui->episodeView->setVisible(visible);
    }

    void clearFormContents()
    {
        if (_formTreeModel)
            _formTreeModel->clearFormContents();
    }

    void setCurrentForm(const QModelIndex &index)
    {
        if (index==_currentEditingForm)
            return;
        clearFormContents();
        ui->formDataMapper->setCurrentEpisode(QModelIndex());

        // If the current form does not handle episode -> show its first child
        QModelIndex current = index;
        if (_formTreeModel->isNoEpisode(current)) {
            if (_formTreeModel->rowCount(current) > 0) {
                current = _formTreeModel->index(0, 0, current);
            }
        }

        _currentEditingForm = current;

        if (ui->episodeView->selectionModel())
            QObject::disconnect(ui->episodeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), q, SLOT(episodeChanged(QModelIndex, QModelIndex)));

        ui->formDataMapper->setCurrentForm(_formTreeModel->formForIndex(_currentEditingForm));

        // connect rowChanged from episode model to formtreemodel
        if (_currentEpisodeModel) {
            QObject::disconnect(_currentEpisodeModel, SIGNAL(rowsInserted(QModelIndex,int,int)), q, SLOT(updateFormCount()));
            QObject::disconnect(_currentEpisodeModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), q, SLOT(updateFormCount()));
        }
        _currentEpisodeModel = episodeManager().episodeModel(_formTreeModel->formForIndex(_currentEditingForm));
        QObject::connect(_currentEpisodeModel, SIGNAL(rowsInserted(QModelIndex,int,int)), q, SLOT(updateFormCount()));
        QObject::connect(_currentEpisodeModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), q, SLOT(updateFormCount()));

        // create the episodemodel proxymodel
        if (_proxyModel)
            delete _proxyModel;
        _proxyModel = new QSortFilterProxyModel(q);
        _proxyModel->setSourceModel(_currentEpisodeModel);
        _proxyModel->setDynamicSortFilter(true);
        ui->episodeView->setModel(_proxyModel);

        // set the view columns
        for(int i=0; i < EpisodeModel::MaxData; ++i)
            ui->episodeView->hideColumn(i);
        ui->episodeView->showColumn(EpisodeModel::ValidationStateIcon);
        ui->episodeView->showColumn(EpisodeModel::PriorityIcon);
        ui->episodeView->showColumn(EpisodeModel::UserDateTime);
        ui->episodeView->showColumn(EpisodeModel::CreationDateTime);
        ui->episodeView->showColumn(EpisodeModel::Label);
        ui->episodeView->showColumn(EpisodeModel::UserCreatorName);

        ui->episodeView->horizontalHeader()->setSectionResizeMode(EpisodeModel::ValidationStateIcon,
                                                                  QHeaderView::ResizeToContents);
        ui->episodeView->horizontalHeader()->setSectionResizeMode(EpisodeModel::PriorityIcon, QHeaderView::ResizeToContents);
        ui->episodeView->horizontalHeader()->setSectionResizeMode(EpisodeModel::UserDateTime, QHeaderView::ResizeToContents);
        ui->episodeView->horizontalHeader()->setSectionResizeMode(EpisodeModel::CreationDateTime, QHeaderView::ResizeToContents);
        ui->episodeView->horizontalHeader()->setSectionResizeMode(EpisodeModel::Label, QHeaderView::ResizeToContents);
        ui->episodeView->horizontalHeader()->setSectionResizeMode(EpisodeModel::UserCreatorName, QHeaderView::Stretch);
        ui->episodeView->horizontalHeader()->setSectionsMovable(true);

        QFont small;
        if (Utils::isRunningOnWin() || Utils::isRunningOnLinux())
            small.setPointSize(small.pointSize() - 1);
        else
            small.setPointSize(small.pointSize() - 4);
        ui->episodeView->horizontalHeader()->setFont(small);
        QFile file(":/headerview.qss");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        ui->episodeView->horizontalHeader()->setStyleSheet(styleSheet);
        ui->episodeView->selectionModel()->clearSelection();
        // set the sort order & column to the view/proxymodel
        ui->episodeView->sortByColumn(settings()
                                      ->value(Constants::S_EPISODEVIEW_SORTEDCOLUMN,
                                              EpisodeModel::UserDateTime).toInt(),
                                              Qt::SortOrder(settings()
                                                            ->value(Constants::S_EPISODEVIEW_SORTORDER,
                                                            Qt::DescendingOrder).toInt()));
        ui->episodeView->setSortingEnabled(true);
        checkCurrentEpisodeViewVisibility();
        QObject::connect(ui->episodeView->selectionModel(),
                         SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                         q,
                         SLOT(episodeChanged(QModelIndex, QModelIndex)));

        Q_EMIT q->actionsEnabledStateChanged();
    }

    void selectAndActivateFirstForm()
    {
        if (!_formTreeModel)
            return;
        if (ui->formView->selectionModel() && ui->formView->selectionModel()->hasSelection())
            return;
        // Select the first available form in the tree model
        if (_formTreeModel->rowCount() > 0) {
            QModelIndex index = _formTreeModel->index(0,0);
            setCurrentForm(index);
            ui->formView->selectionModel()->select(index, QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
        }
    }

    void selectAndActivateFirstEpisode()
    {
        // Assuming the _currentEditingForm is defined and the episodeView model is set
        if (ui->episodeView->selectionModel()
                && !ui->episodeView->selectionModel()->hasSelection()) {
            ui->formDataMapper->setCurrentEpisode(QModelIndex());
            if (ui->episodeView->model()->rowCount() > 0) {
                ui->episodeView->selectRow(0);
                // force q->episodeChanged
                q->episodeChanged(ui->episodeView->currentIndex(), QModelIndex());
                ui->formDataMapper->setFormWidgetEnabled(true);
            } else {
                ui->formDataMapper->setFormWidgetEnabled(false);
            }
        }
    }

    QModelIndex currentEditingEpisodeIndex()
    {
        return ui->formDataMapper->currentEditingEpisodeIndex();
    }

public:
    Ui::FormPlaceHolder *ui;
    QModelIndex _currentEditingForm;
    FormTreeModel *_formTreeModel;
    FormViewDelegate *_delegate;
    QToolBar *_episodeToolBar;
    QHash<int, QString> m_StackId_FormUuid;
    Internal::FormPlaceHolderCoreListener *_coreListener;
    Internal::FormPlaceHolderPatientListener *_patientListener;
    QSortFilterProxyModel *_proxyModel;
    EpisodeModel *_currentEpisodeModel;

private:
    FormPlaceHolder *q;
};

}  // namespace Internal
}  // namespace Form

FormPlaceHolder::FormPlaceHolder(QWidget *parent) :
    FormContextualWidget(parent),
    d(new Internal::FormPlaceHolderPrivate(this))
{
    d->ui->setupUi(this);
    if (layout()) {
        layout()->setMargin(0);
        layout()->setSpacing(0);
    }
    d->ui->verticalLayout_2->setMargin(0);
    d->ui->verticalLayout_2->setSpacing(0);
    d->createEpisodeToolBar();

    d->_delegate = new Internal::FormViewDelegate(d->ui->formView);
    d->ui->formDataMapper->initialize();

    // Manage Form File tree view
    d->ui->formView->setActions(0);
    d->ui->formView->setCommands(QStringList()
                                 << Constants::A_ADDFORM
                                 << Constants::A_REMOVEFORM
                                 );
    d->ui->formView->addContext(context()->context());
    d->ui->formView->setDeselectable(false);
    d->ui->formView->disconnectActionsToDefaultSlots();
    d->ui->formView->treeView()->viewport()->setAttribute(Qt::WA_Hover);
    d->ui->formView->treeView()->setItemDelegate(d->_delegate);
    d->ui->formView->treeView()->setFrameStyle(QFrame::NoFrame);
    d->ui->formView->treeView()->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->ui->formView->treeView()->setSelectionMode(QAbstractItemView::SingleSelection);
    d->ui->formView->treeView()->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->ui->formView->treeView()->setAlternatingRowColors(settings()->value(Constants::S_USEALTERNATEROWCOLOR).toBool());
    d->ui->formView->treeView()->setStyleSheet(Constants::FORMTREEVIEW_SHEET);

    connect(d->ui->formView, SIGNAL(clicked(QModelIndex)), this, SLOT(handleClicked(QModelIndex)));
    connect(d->ui->formView, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePressed(QModelIndex)));

    d->ui->episodeView->verticalHeader()->hide();
    d->ui->episodeView->setFrameStyle(QFrame::NoFrame);
    d->ui->episodeView->setSelectionMode(QAbstractItemView::SingleSelection);
    d->ui->episodeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->ui->episodeView->setItemDelegateForColumn(EpisodeModel::UserDateTime, new Utils::DateTimeDelegate(this, false));
    d->ui->episodeView->setItemDelegateForColumn(EpisodeModel::CreationDateTime, new Utils::DateTimeDelegate(this, false));


    int width = size().width();
    int third = width/3;
    d->ui->horizontalSplitter->setSizes(QList<int>() << third << width-third);

    int height = size().height();
    third = height/5;
    d->ui->verticalSplitter->setSizes(QList<int>() << third << height-third);

    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));

    // Autosave feature
    // -> Core Listener
    d->_coreListener = new Internal::FormPlaceHolderCoreListener(this);
    pluginManager()->addObject(d->_coreListener);

    // -> Patient change listener
    d->_patientListener = new Internal::FormPlaceHolderPatientListener(this);
    pluginManager()->addObject(d->_patientListener);

    // TODO: add a User Listener in FormPlaceHolder

    connect(d->ui->episodeView->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(saveSortOrderToSettings(int,Qt::SortOrder)));
}

FormPlaceHolder::~FormPlaceHolder()
{
    if (d->_coreListener) {
        pluginManager()->removeObject(d->_coreListener);
    }
    if (d->_patientListener) {
        pluginManager()->removeObject(d->_patientListener);
    }
    if (d) {
        delete d;
        d = 0;
    }
}

/** Return the enabled state of an action. \sa Form::Internal::FormActionHandler */
bool FormPlaceHolder::enableAction(WidgetAction action) const
{
    if (!d->_formTreeModel || !d->_currentEpisodeModel) {
        qDebug() << Q_FUNC_INFO << "return false";
        return false;
    }
    switch (action) {
    case Action_Clear:
        // Clear only if a form && an episode are selected
        return d->ui->episodeView->selectionModel()->hasSelection()
                && d->ui->formView->selectionModel()->hasSelection();
    case Action_CreateEpisode:
    {
        // No form
        if ( (!d->_formTreeModel)
             || (!d->_currentEditingForm.isValid())
             || ( (d->_currentEpisodeModel->rowCount() > 0)
                  && (d->_formTreeModel->isUniqueEpisode(d->_currentEditingForm)) ) ) {
            return false;
        }
        // Only available for multi episode forms or for unique episode forms
        // with 0 episode
        //bool multiEpisode = d->_formTreeModel->isMultiEpisode(d->_currentEditingForm);
        //return multiEpisode;
        return true;
    }
    case Action_ValidateCurrentEpisode:
    {
        // Validate episode only if
        // - an episode is selected
        // - && episode not already validated
        // - && form is not unique episode
        bool unique = d->_formTreeModel->isUniqueEpisode(d->_currentEditingForm);
        return (d->ui->episodeView->selectionModel()->hasSelection()
                && !d->_currentEpisodeModel->isEpisodeValidated(d->currentEditingEpisodeIndex())
                && !unique);
    }
    case Action_SaveCurrentEpisode:
        // Save episode only if
        // - an episode is selected
        //TODO: - episode is not validated
        return (d->ui->episodeView->selectionModel()->hasSelection());
    case Action_RemoveCurrentEpisode:
    {
        // Save episode only if
        // - form is not an identity form
        // - form is not NoEpisode form
        // - form model is populated
        // - an episode is selected
        bool identity = d->_formTreeModel->isIdentity(d->_currentEditingForm);
        bool modelPopulated = (d->_currentEpisodeModel->rowCount() > 0);
        bool noEpisode = d->_formTreeModel->isNoEpisode(d->_currentEditingForm);
        return (!identity
                && !noEpisode
                && modelPopulated
                && d->ui->episodeView->selectionModel()->hasSelection());
    }
    case Action_RenewCurrentEpisode:
    {
        // Renew an episode only if
        // - form is multi-episode
        // - an episode is selected
        bool multiEpisode = d->_formTreeModel->isMultiEpisode(d->_currentEditingForm);
        bool hasSelection = d->ui->episodeView->selectionModel()->hasSelection();
        return multiEpisode && hasSelection;
    }
    case Action_TakeScreenShot:
    {
        // Take screenshot / Renew an episode only if
        // - an episode is selected
        return (d->ui->episodeView->selectionModel()->hasSelection());
    }
    case Action_AddForm:
        // Add form always enabled
        return true;
    case Action_RemoveSub:
        return d->ui->formView->selectionModel()->hasSelection()
                && d->_formTreeModel->isIncludedRootSubForm(d->ui->formView->currentIndex());
    case Action_PrintCurrentFormEpisode:
        // Print episode only if an episode is selected
        return d->ui->episodeView->selectionModel()->hasSelection();
    }  // end switch
    return false;
}

/**
 * Define the Form::FormMain root item to use for the creation of the patient files.
 * This object will manage deletion of the root item and its children.
 */
void FormPlaceHolder::setFormTreeModel(FormTreeModel *model)
{
    if (!model)
        return;
    if (d->_formTreeModel==model)
        return;
    // Manage Form tree view / model
    if (d->_formTreeModel) {
        disconnect(d->ui->formView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentSelectedFormChanged(QModelIndex, QModelIndex)));
        disconnect(d->_formTreeModel, SIGNAL(modelReset()), this, SLOT(onFormTreeModelReset()));
    }
    d->_formTreeModel = model;

    d->ui->formView->setModel(d->_formTreeModel);
    d->_delegate->setFormTreeModel(d->_formTreeModel);

    onFormTreeModelReset();
    connect(d->_formTreeModel, SIGNAL(modelReset()), this, SLOT(onFormTreeModelReset()));
    connect(d->ui->formView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentSelectedFormChanged(QModelIndex, QModelIndex)));
    Q_EMIT actionsEnabledStateChanged();
}

/**
 * Returns the label of the currently selected form (mainly used for debugging purpose).
 * \sa Forms::Internal::FormPlaceHolderCoreListener
 */
QString FormPlaceHolder::currentFormLabel() const
{
    if (d->_formTreeModel && d->_currentEditingForm.isValid()) {
        QModelIndex index = d->_formTreeModel->index(d->_currentEditingForm.row(), FormTreeModel::Label, d->_currentEditingForm.parent());
        return d->_formTreeModel->data(index).toString();
    }
    return QString::null;
}

/** Clear the form content. The current episode (if one was selected) is not submitted to the model. */
bool FormPlaceHolder::clear()
{
    d->clearFormContents();
    d->ui->formDataMapper->clear();
    //    d->_currentEditingForm = QModelIndex();
    //    Q_EMIT actionsEnabledStateChanged();
    return true;
}

// Used for the delegate
void FormPlaceHolder::handlePressed(const QModelIndex &index)
{
    if (index.column() == FormTreeModel::Label) {
        //        d->_currentEpisodeModel->activateEpisode(index);
    } else if (index.column() == FormTreeModel::EmptyColumn1) {
        d->_delegate->pressedIndex = index;
    }
}

// Used for the delegate
void FormPlaceHolder::handleClicked(const QModelIndex &index)
{
    if (index.column() == FormTreeModel::EmptyColumn1) { // the funky button
        if (!d->_formTreeModel->isNoEpisode(index))
            createEpisode();

        // work around a bug in itemviews where the delegate wouldn't get the QStyle::State_MouseOver
        QPoint cursorPos = QCursor::pos();
        QTreeView *tree = d->ui->formView->treeView();
        QWidget *vp = tree->viewport();
        QMouseEvent e(QEvent::MouseMove, vp->mapFromGlobal(cursorPos), cursorPos, Qt::NoButton, 0, 0);
        QCoreApplication::sendEvent(vp, &e);
    }
}

/** Slot connect to the current episode model (row changed) to inform the formtreemodel of the episode row count changed */
void FormPlaceHolder::updateFormCount()
{
    //    qWarning() << d->_currentEditingForm;
    //    d->_formTreeModel->updateFormCount(d->_currentEditingForm);
}

/** Update actions on current form selection changed */
void FormPlaceHolder::currentSelectedFormChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    setCurrentEditingFormItem(current);
    Q_EMIT actionEnabledStateChanged(Action_RemoveSub);
}

/**
 * When using a FormTreeModel you can set the current form with this.
 * \sa setCurrentForm()
 */
void FormPlaceHolder::setCurrentEditingFormItem(const QModelIndex &index)
{
    if (index != d->_currentEditingForm) {
        // autosave feature
        d->saveCurrentEditingEpisode();
        d->setCurrentForm(index);
        d->selectAndActivateFirstEpisode();
        Q_EMIT actionsEnabledStateChanged();
    }
}

/**
 * Create a new episode for the current selected form.
 * Return true in case of success.
 * Connected to Form::Internal::FormActionHandler
 * \sa Form::EpisodeModel::insertRow()
 */
bool FormPlaceHolder::createEpisode()
{
    if (!d->ui->formView->selectionModel())
        return false;
    if (!d->ui->formView->selectionModel()->hasSelection())
        return false;

    // autosave feature
    if (d->_currentEpisodeModel) {
        if (!d->saveCurrentEditingEpisode()) {
            LOG_ERROR("Unable to save current episode");
            return false;
        }
    } else {
        Q_ASSERT(false);
    }

    // get the form
    QModelIndex index = d->ui->formView->selectionModel()->selectedIndexes().at(0);
    if (d->_formTreeModel->isNoEpisode(index)) {
        LOG_ERROR("Can not create an episode on NoEpisode forms");
        return false;
    }

    if ((d->_formTreeModel->isUniqueEpisode(index))
            && (d->_currentEpisodeModel->rowCount() > 0)) {
        LOG_ERROR("Cannot create more than one episode on IsUniqueEpisode forms");
        return false;
    }
    setCurrentEditingFormItem(index);
    // create a new episode with the selected form and its children
    // FIXME: see bool EpisodeModel::validateEpisode(const QModelIndex &index)
    d->_currentEpisodeModel->setReadOnly(false);
    if (!d->_currentEpisodeModel->insertRow(d->_currentEpisodeModel->rowCount())) {
        LOG_ERROR("Unable to create new episode");
        return false;
    }
    // activate the newly created episode
    QModelIndex source = d->_currentEpisodeModel->index(d->_currentEpisodeModel->rowCount() - 1, EpisodeModel::Label);
    QModelIndex proxy = d->_proxyModel->mapFromSource(source);
    d->ui->episodeView->selectRow(proxy.row());
    d->ui->formDataMapper->setCurrentEpisode(source);
    // save newly created episode (see forceSaveCurrentEditingEpisode())
    if (d->_currentEpisodeModel) {
        if (!d->forceSaveCurrentEditingEpisode())
            LOG_ERROR("Unable to save newly created episode");
    }
    d->_formTreeModel->updateFormCount(d->_currentEditingForm);
    Q_EMIT actionsEnabledStateChanged();
    return true;
}

/**
 * Validate the currently selected episode. The episode content is not modified, only its validation state.
 * Return false in case of error or user annulation.
 * Connected to Form::Internal::FormActionHandler.
 * \sa Form::EpisodeModel::validateEpisode()
 */
bool FormPlaceHolder::validateCurrentEpisode()
{
    if (!d->ui->episodeView->selectionModel()->hasSelection())
        return false;

    // message box
    bool yes = Utils::yesNoMessageBox(tr("Validate the current episode"),
                                      tr("When you validate an episode, you prevent all subsequent amendments. "
                                         "The episode will be shown but will be kept unchanged.\n"
                                         "Do you really want to validate the current episode?"));
    if (!yes)
        return false;

    // get the episodeModel corresponding to the currently selected form
    if (!d->_currentEpisodeModel) {
        qDebug() << "_currentEpisodeModel is false";
        return false;
    }
    if (!d->saveCurrentEditingEpisode()) {
        LOG_ERROR("Unable to save current episode");
        return false;
    }

    // validate episode
    bool ok = d->_currentEpisodeModel->validateEpisode(d->currentEditingEpisodeIndex());
    if (ok) {
        patient()->patientBar()->showMessage(tr("Episode (%1) from form (%2) signed")
                                             .arg(d->ui->formDataMapper->currentEpisodeLabel())
                                             .arg(d->ui->formDataMapper->currentFormName()));
    }
    Q_EMIT actionsEnabledStateChanged();
    return ok;
}

/**
 * Renew the currently selected episode. This member takes the content of
 * the current episode and use it to create a new one. Only the date, episode label
 * and user creator changes.
 * \sa EpisodeModel::renewEpisode
 */
bool FormPlaceHolder::renewEpisode()
{
    if (!d->ui->episodeView->selectionModel()->hasSelection())
        return false;

    // message box
    bool yes = Utils::yesNoMessageBox(tr("Renew the current episode"),
                                      tr("A new episode will created with the exact same content "
                                         "as the currently selected but at the current date, "
                                         "using your user.<br />"
                                         "Do you want to renew the current episode?"));
    if (!yes)
        return false;


    // get the episodeModel corresponding to the currently selected form
    if (!d->_currentEpisodeModel)
        return false;
    if (!d->saveCurrentEditingEpisode()) {
        LOG_ERROR("Unable to save current episode");
        return false;
    }

    // renew an episode
    QModelIndex newEpisode = d->_currentEpisodeModel->renewEpisode(d->currentEditingEpisodeIndex());
    if (newEpisode.isValid()) {
        // message
        patient()->patientBar()->showMessage(tr("Episode (%1) from form (%2) renewed")
                                             .arg(d->ui->formDataMapper->currentEpisodeLabel())
                                             .arg(d->ui->formDataMapper->currentFormName()));

        // select the newly created episode
        QModelIndex proxy = d->_proxyModel->mapFromSource(newEpisode);
        d->ui->episodeView->selectRow(proxy.row());
        d->_formTreeModel->updateFormCount(d->_currentEditingForm);
    }
    Q_EMIT actionsEnabledStateChanged();
    return newEpisode.isValid();
}

/**
 * Save the currently selected episode (episode content is submitted to its Form::EpisodeModel).
 * Are saved:
 * - the user datetime
 * - the user label
 * - the priority
 * - the XML content
 * Return false in case of error.
 * Connected to Form::Internal::FormActionHandler.
 * \sa Form::EpisodeModel::submit()
 */
bool FormPlaceHolder::saveCurrentEpisode()
{
    bool ok = d->saveCurrentEditingEpisode();
    Q_EMIT actionsEnabledStateChanged();
    return ok;
}

/**
 * Remove the currently selected episode
 * Return false in case of error.
 * Connected to Form::Internal::FormActionHandler.
 * \sa Form::EpisodeModel::removeEpisode()
 */
bool FormPlaceHolder::removeCurrentEpisode()
{
#ifdef WITH_EPISODE_REMOVAL
    // message box
    bool yes = Utils::yesNoMessageBox(tr("Remove the current episode"),
                                      tr("You can not completely destroy an episode, "
                                         "but you can remove it from the views.\n"
                                         "The episode will not be shown anymore, but will still be "
                                         "included in the database.\n"
                                         "Do you really want to remove the current episode?"));
    if (!yes)
        return false;
    bool ok = d->_currentEpisodeModel->removeEpisode(d->currentEditingEpisodeIndex());
    if (ok)
        patient()->patientBar()->showMessage(tr("Episode (%1) from form (%2) removed")
                                             .arg(d->ui->formDataMapper->currentEpisodeLabel())
                                             .arg(d->ui->formDataMapper->currentFormName()));

    d->_formTreeModel->updateFormCount(d->_currentEditingForm);
    d->ui->formDataMapper->clear();
    d->ui->formDataMapper->setEnabled(false);
    Q_EMIT actionsEnabledStateChanged();
    return ok;
#else
    return false;
#endif
}

/**
 * Take a screenshot of the currently selected form and episode.
 * Return false in case of error.
 * Connected to Form::Internal::FormActionHandler.
 */
bool FormPlaceHolder::takeScreenshotOfCurrentEpisode()
{
    QString fileName = QFileDialog::getSaveFileName(this, tkTr(Trans::Constants::SAVE_FILE),
                                                    settings()->path(Core::ISettings::UserDocumentsPath),
                                                    tr("Images (*.png)"));
    if (!fileName.isEmpty()) {
        QFileInfo info(fileName);
        if (info.completeSuffix().isEmpty())
            fileName.append(".png");
        QPixmap pix = d->ui->formDataMapper->screenshot();
        return pix.save(fileName);
    }
    return false;
}

/**
 * Add a form to the current mode. Opens a Form::FormEditorDialog and let user select complete and sub-forms.
 * Return false in case of error. Connected to Form::Internal::FormActionHandler
 * \sa Form::FormTreeModel, Form::FormMain, Form::FormEditorDialog
*/
bool FormPlaceHolder::addForm()
{
    //    d->saveCurrentEditingEpisode();

    // open the form editor dialog
    FormEditorDialog dlg(d->_formTreeModel, FormEditorDialog::DefaultMode, this);
    if (dlg.exec()==QDialog::Accepted) {
        // refresh stack widget
        //        d->populateStackLayout();
        // activate last episode synthesis
        //        d->ui->formView->setCurrentIndex(d->_currentEpisodeModel->index(0,0));
        //        showLastEpisodeSynthesis();
    }
    Q_EMIT actionsEnabledStateChanged();
    return true;
}

/** If the currently selected form is a sub-form, remove it */
bool FormPlaceHolder::removeSubForm()
{
    if (!d->ui->formView->selectionModel())
        return false;

    QModelIndex current = d->ui->formView->selectionModel()->currentIndex();
    QModelIndex indexLabel = d->_formTreeModel->index(current.row(), FormTreeModel::Label, current.parent());
    QModelIndex indexUid = d->_formTreeModel->index(current.row(), FormTreeModel::Uuid, current.parent());
    QModelIndex parent = d->_formTreeModel->index(indexUid.parent().row(), FormTreeModel::Uuid, indexUid.parent().parent());
    EpisodeModel *episodeModel = episodeManager().episodeModel(d->_formTreeModel->formForIndex(indexUid));
    QString episodeMsg;
    if (episodeModel->rowCount() > 0)
        episodeMsg = "<span style=\"font-weight:bold; color:darkred\">All recorded episode will be removed as well.</span><br /><br />";

    // message box
    bool yes = Utils::yesNoMessageBox(tr("Remove the current form"),
                                      tr("Trying to remove the sub-form:"
                                         "<br />&nbsp;&nbsp;&nbsp;<b>%1</b>.<br /><br />"
                                         "%2"
                                         "This modification will only affect the current patient:"
                                         "<br />&nbsp;&nbsp;&nbsp;<b>%3</b>.<br /><br />"
                                         "Do you really want to remove the current sub-form?")
                                      .arg(d->_formTreeModel->data(indexLabel).toString().replace(" ", "&nbsp;"))
                                      .arg(episodeMsg)
                                      .arg(patient()->data(Core::IPatient::FullName).toString().replace(" ", "&nbsp;")));
    if (!yes)
        return false;

    // remove episodes
    if (!episodeModel->removeAllEpisodes()) {
        LOG_ERROR("Unable to remove all episodes");
        return false;
    }

    // remove sub-form
    SubFormRemoval remove(d->_formTreeModel->modeUid(), d->_formTreeModel->data(parent).toString(), d->_formTreeModel->data(indexUid).toString());
    formManager().removeSubForm(remove);
    return true;
}

/**
 * Print the current editing episode. Return false in case of error.
 * In order to print an episode (not an empty form), this member uses the
 * Form::FormItemSpec::Spec_HtmlPrintMask of the current editing form. \n
 * Connected to Form::Internal::FormActionHandler
 */
bool FormPlaceHolder::printFormOrEpisode()
{
    if (!d->ui->formView->selectionModel())
        return false;
    Form::FormMain *formMain = d->_formTreeModel->formForIndex(d->ui->formView->currentIndex());
    if (!formMain)
        return false;
    Core::IDocumentPrinter *p = printer();
    if (!p) {
        LOG_ERROR("No IDocumentPrinter found");
        return false;
    }

    QString title = formMain->spec()->label();
    QString htmlToPrint = formManager().formPrintHtmlOutput(formMain);

    p->clearTokens();
    QHash<QString, QVariant> tokens;
    tokens.insert(Core::Constants::TOKEN_DOCUMENTTITLE, title);
    p->addTokens(Core::IDocumentPrinter::Tokens_Global, tokens);

    // print
    if (formMain->isNoHeader()) {
        p->print(htmlToPrint, Core::IDocumentPrinter::Papers_Empty_User, false);
    } else if (formMain->isAdministrativeHeader()) {
        p->print(htmlToPrint, Core::IDocumentPrinter::Papers_Administrative_User, false);
    } else if (formMain->isPrescriptionHeader()) {
        p->print(htmlToPrint, Core::IDocumentPrinter::Papers_Prescription_User, false);
    } else {
        p->print(htmlToPrint, Core::IDocumentPrinter::Papers_Generic_User, false);
    }
    return true;
}

/** Define the cols to show, their sizes... */
void FormPlaceHolder::onFormTreeModelReset()
{
    // Avoid a QHeaderView assert on resizeColumn
    if (d->_formTreeModel->columnCount()==0)
        return;

    d->_currentEditingForm = QModelIndex();
    QTreeView *tree = d->ui->formView->treeView();
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    for(int i=0; i < d->_formTreeModel->columnCount(); ++i)
        tree->setColumnHidden(i, true);
    tree->setColumnHidden(FormTreeModel::Label, false);
    tree->setColumnHidden(FormTreeModel::EmptyColumn1, false);
    tree->header()->hide();
    tree->header()->setStretchLastSection(false);
    tree->header()->setSectionResizeMode(FormTreeModel::Label, QHeaderView::Stretch);
    tree->header()->setSectionResizeMode(FormTreeModel::EmptyColumn1, QHeaderView::Fixed);
    tree->header()->resizeSection(FormTreeModel::EmptyColumn1, 16);
    tree->expandAll();
    d->selectAndActivateFirstForm();
}

void FormPlaceHolder::saveSortOrderToSettings(int col, Qt::SortOrder sort)
{
    settings()->setValue(Constants::S_EPISODEVIEW_SORTEDCOLUMN, col);
    settings()->setValue(Constants::S_EPISODEVIEW_SORTORDER, sort);
}

void FormPlaceHolder::onCurrentPatientChanged()
{
    // reset the ui
    clear();
    QItemSelectionModel *model = d->ui->episodeView->selectionModel();
    if (model)
        model->clearSelection();
    model = d->ui->formView->selectionModel();
    if (model)
        model->clearSelection();
    Q_EMIT actionsEnabledStateChanged();
}

void FormPlaceHolder::episodeChanged(const QModelIndex &current, const QModelIndex &previous)
{
    LOG(QString("episodeChanged: current(valid:%1) ; previous(valid:%2)").arg(current.isValid()).arg(previous.isValid()));
    // Autosave is problematic when patient changed
    QModelIndex sourceCurrent = d->_proxyModel->mapToSource(current);
    QModelIndex sourcePrevious = d->_proxyModel->mapToSource(previous);
    if (sourcePrevious.isValid())
        d->saveCurrentEditingEpisode();
    clear();
    if (sourceCurrent.isValid()) {
        d->ui->formDataMapper->setCurrentEpisode(sourceCurrent);
        d->ui->formDataMapper->setEnabled(true);
    } else {
        d->ui->formDataMapper->clear();
        d->ui->formDataMapper->setEnabled(false);
    }
    //    warnPatient();
    Q_EMIT actionsEnabledStateChanged();
}

/** Return true is the Form::Internal::FormDataWidgetMapper included in the view is dirty */
bool FormPlaceHolder::isDirty() const
{
    //    qWarning() << "FormPlaceHolder::isDirty" << d->_formTreeModel << d->_currentEditingForm.isValid() << d->ui->formDataMapper->currentEditingEpisodeIndex().isValid();
    if (d->_formTreeModel
            && d->_currentEditingForm.isValid()
            && d->ui->formDataMapper->currentEditingEpisodeIndex().isValid())
        return d->ui->formDataMapper->isDirty();
    return false;
}

void FormPlaceHolder::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        // if showing patient synthesis or last episode -> retranslate by querying the model
        //        QTreeView *tree = d->ui->formView->treeView();
        //        if (tree->selectionModel()) {
        //            QModelIndex index = tree->selectionModel()->currentIndex();
        //            const QString &formUuid = d->_currentEpisodeModel->index(index.row(), FormTreeModel::FormUuid, index.parent()).data().toString();
        //            if (formUuid==Constants::PATIENTLASTEPISODES_UUID) {
        //                setCurrentForm(formUuid);
        //            }
        //        }
    }
    QWidget::changeEvent(event);
}

void FormPlaceHolder::hideEvent(QHideEvent *event)
{
    LOG(QString("HideEvent: %1").arg(isVisible() ? "isVisible" : "isHidden"));
    // autosave feature
    d->saveCurrentEditingEpisode();
    QWidget::hideEvent(event);
}

void FormPlaceHolder::showEvent(QShowEvent *event)
{
    LOG(QString("ShowEvent: %1").arg(isVisible() ? "isVisible" : "isHidden"));

    if (d->ui->formDataMapper->isDirty()) {
        LOG_ERROR("**** During ShowEvent FormDataMapper is Dirty: " + d->_formTreeModel->formForIndex(d->_currentEditingForm)->uuid());
        d->saveCurrentEditingEpisode();
    }

    d->selectAndActivateFirstForm();
    d->selectAndActivateFirstEpisode();
    // make sure that the context is updated to the form context
    d->ui->formDataMapper->setFocus();
    // then update actions
    Q_EMIT actionsEnabledStateChanged();

    // Update sort order according to the current settings
    if (d->_proxyModel) {
        if (d->_proxyModel->sortColumn() != settings()->value(Constants::S_EPISODEVIEW_SORTEDCOLUMN).toInt()
                || d->_proxyModel->sortOrder() != Qt::SortOrder(settings()->value(Constants::S_EPISODEVIEW_SORTORDER).toInt())) {
            d->ui->episodeView->sortByColumn(settings()->value(Constants::S_EPISODEVIEW_SORTEDCOLUMN, EpisodeModel::UserDateTime).toInt(),
                                             Qt::SortOrder(settings()->value(Constants::S_EPISODEVIEW_SORTORDER, Qt::DescendingOrder).toInt()));
        }
    }
    QWidget::showEvent(event);
}
