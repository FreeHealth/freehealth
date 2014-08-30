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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class UserPlugin::UserManagerMainWindow
 * This is the Users' Manager of FreeMedForms. \n
 * You can use it as main app or secondary mainWindow.\n
 * You only need to instanciate the UserModel and define a current user
 * before using this mainwindow.
*/

/**
 * \class UserPlugin::UserManagerDialog
 * User Manager as a dialog.
 * User Model must have been instanciated before creating the dialog,
 * and a current user must have been set.\n
 * \sa UserModel, UserModel::hasCurrentUser()
 */

#include "usermanager.h"
#include "usermanager_p.h"

#include <utils/log.h>
#include <utils/widgets/qbuttonlineedit.h>
#include <utils/widgets/htmldelegate.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>

#include <coreplugin/contextmanager/icontext.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/mainwindowactions.h>

#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/usermanagermodel.h>
#include <usermanagerplugin/userdata.h>
#include <usermanagerplugin/widgets/userviewer.h>
#include <usermanagerplugin/constants.h>
#include <usermanagerplugin/widgets/usercreatorwizard.h>

#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>

#include <QModelIndex>
#include <QTableView>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QSplitter>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QToolButton>
#include <QToolBar>
#include <QLabel>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QCloseEvent>
#include <QPainter>
#include <QDesktopWidget>

#include "ui_usermanagerwidget.h"
#include "ui_userviewer_treedelegate.h"

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}

namespace UserPlugin {
namespace Internal {
/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////    UserManagerContext    ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class UserManagerContext : public Core::IContext
{
public:
    UserManagerContext(QWidget *parent) : Core::IContext(parent)
    {
        setObjectName("UserManagerContext");
        setWidget(parent);
        m_context = Core::Context(Core::Constants::C_USERMANAGER);
    }
};
}  // End  Internal
}  // End UserPlugin

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////   UserManager   ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserManagerMainWindow::UserManagerMainWindow(QWidget * parent) :
    Core::IMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_Widget = new UserManagerWidget(this);
    setCentralWidget(m_Widget);
    setUnifiedTitleAndToolBarOnMac(true);
}

/** Destructor */
UserManagerMainWindow::~UserManagerMainWindow()
{
}

/** Initialize the mainwindow */
bool UserManagerMainWindow::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    Q_ASSERT_X(userModel()->hasCurrentUser(), "UserManagerMainWindow", "NO CURRENT USER");
    if (!userModel()->hasCurrentUser())
        return false;

    Q_ASSERT(actionManager());
    Q_ASSERT(contextManager());

    // create menus
    createFileMenu();
    createEditMenu();
    createFormatMenu();
    createConfigurationMenu();
    createHelpMenu();

    Core::MainWindowActions actions;
    actions.setFileActions(
            Core::MainWindowActions::A_FilePrint |
            Core::MainWindowActions::A_FilePrintPreview |
            Core::MainWindowActions::A_FileQuit
            );

    actions.setConfigurationActions(
            Core::MainWindowActions::A_AppPreferences |
//            Core::MainWindowActions::A_AppConfigurator |
            Core::MainWindowActions::A_PluginsPreferences |
            Core::MainWindowActions::A_LanguageChange
            );

    actions.setHelpActions(
            Core::MainWindowActions::A_AppAbout |
            Core::MainWindowActions::A_PluginsAbout |
            Core::MainWindowActions::A_AppHelp |
            Core::MainWindowActions::A_DebugDialog |
            Core::MainWindowActions::A_AppGoToWebSite
                );

    actions.createEditActions(true);
    createActions(actions);
    if (!m_Widget->initialize())
        return false;
    return true;
}

void UserManagerMainWindow::extensionsInitialized()
{
    // Connect actions
    connectFileActions();
    connectGeneralActions();
    connectConfigurationActions();
    connectHelpActions();

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();

    Utils::resizeAndCenter(this, qApp->desktop());
    raise();
    show();
    setFocus();
}

/** Close the usermanager. Check if modifications have to be saved and ask user. */
void UserManagerMainWindow::closeEvent(QCloseEvent *event)
{
    if (m_Widget->canCloseParent()) {
        event->accept();
    } else {
        event->ignore();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   UserManagerDialog   /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserManagerDialog::UserManagerDialog(QWidget *parent) :
    QDialog(parent)
{
    Q_ASSERT_X(userModel()->hasCurrentUser(), "UserManagerDialog", "NO CURRENT USER");
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);

    if (!userModel()->hasCurrentUser())
        return;
//    setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lay = new QGridLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);
    setLayout(lay);
    m_Widget = new UserManagerWidget(this);
    lay->addWidget(m_Widget, 0, 0);
}

/** Destructor */
UserManagerDialog::~UserManagerDialog()
{
}

/** Initialize the dialog (define the icon and window title) */
bool UserManagerDialog::initialize()
{
    m_Widget->initialize();
    setWindowTitle(tkTr(Trans::Constants::USERMANAGER_TEXT).remove("&"));
    setWindowIcon(theme()->icon(Core::Constants::ICONUSERMANAGER));
    return true;
}

bool UserManagerDialog::initializeAfterShowing()
{
    Q_ASSERT(m_Widget);
    m_Widget->resizeSplitter();
    return true;
}

/** Close the usermanager. Check if modifications have to be saved and ask user. */
void UserManagerDialog::done(int r)
{
    if (m_Widget->canCloseParent()) {
        QDialog::done(r);
    }
}

void UserManagerDialog::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   UserManagerWidget   /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
namespace UserPlugin {
namespace Internal {
class UserManagerWidgetPrivate {
public:
    UserManagerWidgetPrivate(UserManagerWidget *parent) :
        ui(new Ui::UserManagerWidget),
        m_CanModify(false), m_CanCreate(false), m_CanViewAllUsers(false), m_CanViewRestrictedData(false), m_CanDelete(false), m_CanReadOwn(true),
        m_EditingRow(-1),
        m_SearchBy(-1),
        m_ToolBar(0),
        m_SearchToolButton(0),
        aCreateUser(0),
        aModifyUser(0),
        aSave(0),
        aRevert(0),
        aDeleteUser(0),
        aQuit(0),
        aToggleSearchView(0),
        searchByNameAct(0),
        searchByFirstnameAct(0),
        searchByNameAndFirstnameAct(0),
        searchByCityAct(0),
        m_Context(0),
        m_model(0),
        q(parent)
    {}

    ~UserManagerWidgetPrivate()
    {
        delete ui;
    }

    void createUiAndActions()
    {
        ui->setupUi(q);
        ui->userSearchLayout->setMargin(0);
        ui->userSearchLayout->setSpacing(0);

        m_SearchBy = Core::IUser::UsualName;
        aCreateUser = new QAction(q);
        aCreateUser->setObjectName(QString::fromUtf8("aCreateUser"));

        aModifyUser = new QAction(q);
        aModifyUser->setObjectName(QString::fromUtf8("aModifyUser"));
        aModifyUser->setEnabled(false);

        aSave = new QAction(q);
        aSave->setObjectName(QString::fromUtf8("aSave"));
        aSave->setEnabled(false);

        aRevert = new QAction(q);
        aRevert->setObjectName(QString::fromUtf8("aRevert"));
        aRevert->setEnabled(false);

        aDeleteUser = new QAction(q);
        aDeleteUser->setObjectName(QString::fromUtf8("aDeleteUser"));
        aDeleteUser->setEnabled(false);

        aQuit = new QAction(q);
        aQuit->setObjectName(QString::fromUtf8("aQuit"));

        aToggleSearchView = new QAction(q);
        aToggleSearchView->setObjectName(QString::fromUtf8("aToggleSearchView"));
        aToggleSearchView->setCheckable(true);
        aToggleSearchView->setChecked(true);

        // prepare Search Line Edit
        searchByNameAct = new QAction(q);
        searchByFirstnameAct = new QAction(q);
        searchByNameAndFirstnameAct = new QAction(q);
        searchByCityAct = new QAction(q);

        // manage theme / icons
        Core::ITheme *th = theme();
        aSave->setIcon(th->icon(Core::Constants::ICONSAVE, Core::ITheme::MediumIcon));
        aCreateUser->setIcon(th->icon(Core::Constants::ICONNEWUSER, Core::ITheme::MediumIcon));
        aModifyUser->setIcon(th->icon(Core::Constants::ICONEDITUSER, Core::ITheme::MediumIcon));
        aRevert->setIcon(th->icon(Core::Constants::ICONCLEARUSER, Core::ITheme::MediumIcon));
        aDeleteUser->setIcon(th->icon(Core::Constants::ICONDELETEUSER, Core::ITheme::MediumIcon));
        aQuit->setIcon(th->icon(Core::Constants::ICONEXIT, Core::ITheme::MediumIcon));
        aToggleSearchView->setIcon(th->icon(Core::Constants::ICONSEARCHUSER, Core::ITheme::MediumIcon));

        searchByNameAct->setIcon(th->icon(Core::Constants::ICONSEARCH));
        searchByFirstnameAct->setIcon(th->icon(Core::Constants::ICONSEARCH));
        searchByNameAndFirstnameAct->setIcon(th->icon(Core::Constants::ICONSEARCH));
        searchByCityAct->setIcon(th->icon(Core::Constants::ICONSEARCH));
    }

    void createToolBar()
    {
        m_ToolBar = new QToolBar(q);
        m_ToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        m_ToolBar->addAction(aToggleSearchView);
        m_ToolBar->addAction(aCreateUser);
        m_ToolBar->addAction(aModifyUser);
        m_ToolBar->addAction(aSave);
        m_ToolBar->addAction(aDeleteUser);
        m_ToolBar->addAction(aRevert);
    //    m_ToolBar->addAction(aQuit);
        m_ToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        ui->toolbarLayout->addWidget(m_ToolBar);
    }

    void manageSearchLine()
    {
        m_SearchToolButton = new QToolButton(ui->searchLineEdit);
        m_SearchToolButton->addAction(searchByNameAct);
        m_SearchToolButton->addAction(searchByFirstnameAct);
        //    m_SearchToolButton->addAction(searchByNameAndFirstnameAct);
        //    m_SearchToolButton->addAction(searchByCityAct);
        m_SearchToolButton->setPopupMode(QToolButton::InstantPopup);
        m_SearchToolButton->setDefaultAction(searchByNameAct);
        ui->searchLineEdit->setLeftButton(m_SearchToolButton);
    }

    void connectUiAndActions()
    {
        aSave->setShortcut(QKeySequence::Save);
        aCreateUser->setShortcut(QKeySequence::New);

        // connect actions
        QObject::connect(aSave, SIGNAL(triggered()), q, SLOT(onSaveRequested()));
        QObject::connect(aCreateUser, SIGNAL(triggered()), q, SLOT(onCreateUserRequested()));
        QObject::connect(aRevert, SIGNAL(triggered()), q, SLOT(onClearModificationRequested()));
        QObject::connect(aDeleteUser,  SIGNAL(triggered()), q, SLOT(onDeleteUserRequested()));
        QObject::connect(aQuit,  SIGNAL(triggered()), q, SIGNAL(closeRequested()));
        QObject::connect(aToggleSearchView, SIGNAL(toggled(bool)), q, SLOT(toggleSearchView(bool)));

        // connect tableView selector
        QObject::connect(ui->userTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), q, SLOT(onCurrentSelectedIndexChanged(QModelIndex,QModelIndex)));

        // connect search line edit
        QObject::connect(ui->searchLineEdit, SIGNAL(textChanged(const QString &)), q, SLOT(onSearchRequested()));
        QObject::connect(m_SearchToolButton, SIGNAL(triggered(QAction*)), q, SLOT(onSearchToolButtonTriggered(QAction*)));
    }

    // Check the current user rights and adapt the view to them.
    // The UserManagerModel manages the user rights too (in the filter).
    void analyzeCurrentUserRights()
    {
        // retrieve user manager rights from model
        Core::IUser::UserRights r(userModel()->currentUserData(Core::IUser::ManagerRights).toInt());
        // translate to bools
        m_CanReadOwn = (r & Core::IUser::ReadOwn);
        m_CanModify = (r & Core::IUser::WriteAll || r & Core::IUser::WriteOwn);
        m_CanCreate = (r & Core::IUser::Create);
        m_CanViewAllUsers = (r & Core::IUser::ReadAll);
        m_CanDelete = (r & Core::IUser::Delete);

        updateButtons();

        // manage specific creation widgets
        ui->userTreeView->setVisible(m_CanViewAllUsers || m_CanReadOwn);
        ui->searchLineEdit->setVisible(m_CanViewAllUsers);
    }

    // Update buttons and actions according to the current user rights
    void updateButtons()
    {
        const bool enabled = ui->userTreeView->currentIndex().isValid();

        // set enabled states of buttons according to selected user and rights
        aToggleSearchView->setEnabled(m_CanViewAllUsers);
        aCreateUser->setEnabled(m_CanCreate);

        aSave->setEnabled(enabled && m_CanModify);
        aDeleteUser->setEnabled(enabled && m_CanDelete);
        aModifyUser->setEnabled(enabled && m_CanModify);

        //TODO: set this right! just when modified!
        aRevert->setEnabled(enabled);
    }

public:
    Ui::UserManagerWidget *ui;
    bool m_CanModify, m_CanCreate, m_CanViewAllUsers, m_CanViewRestrictedData, m_CanDelete, m_CanReadOwn;
    int m_EditingRow;
    int m_SearchBy;
    QToolBar *m_ToolBar;
    QToolButton *m_SearchToolButton;
    QAction *aCreateUser;
    QAction *aModifyUser;
    QAction *aSave;
    QAction *aRevert;
    QAction *aDeleteUser;
    QAction *aQuit;
    QAction *aToggleSearchView;
    QAction *searchByNameAct, *searchByFirstnameAct, *searchByNameAndFirstnameAct, *searchByCityAct;
    UserManagerContext *m_Context;
    Internal::UserManagerModel *m_model;

private:
    UserManagerWidget *q;
};
} // namespace Internal
} // namespace UserPlugin

UserManagerWidget::UserManagerWidget(QWidget *parent) :
    QWidget(parent),
    d(new UserManagerWidgetPrivate(this))
{
    setObjectName("UserManagerWidget");
    d->createUiAndActions();
    d->createToolBar();
    d->manageSearchLine();
    layout()->setMargin(0);
    layout()->setSpacing(0);
    d->ui->userViewer->setEnabled(false);
}

/** Dtor */
UserManagerWidget::~UserManagerWidget()
{
    delete d;
}

/** Initializes the view, connect actions */
bool UserManagerWidget::initialize()
{
    d->m_model = new Internal::UserManagerModel(this);
    d->m_model->initialize();
    d->m_model->setFilter(Internal::UserManagerModelFilter());

    d->ui->userTreeView->setModel(d->m_model);
    d->ui->userTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->ui->userTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->ui->userTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    d->ui->userViewer->initialize(d->m_model);

    // Add delegate
    Utils::HtmlDelegate *delegate = new Utils::HtmlDelegate(this);
    d->ui->userTreeView->setItemDelegate(delegate);
    d->ui->userTreeView->setIndentation(10);
    d->ui->userTreeView->setAnimated(true);
    d->ui->userTreeView->setUniformRowHeights(false);
    d->ui->userTreeView->setAlternatingRowColors(true);

    d->connectUiAndActions();
    connect(user(), SIGNAL(userChanged()), this, SLOT(onCurrentUserChanged()));
    d->analyzeCurrentUserRights();
    retranslate();
    return true;
}

/**
 * Resize splitter to 1/4 - 3/4  of the current widget width. Call this member after
 * the widget is set to visible.
 */
void UserManagerWidget::resizeSplitter()
{
    Q_ASSERT(d->ui);
    int width = size().width();
    d->ui->splitter->setSizes(QList<int>() << width/4 << width*3/4);
}

/**
 * Checks the content of the view, verify unsaved data.
 * Return \e true if the view can be closed, otherwise return false
 */
bool UserManagerWidget::canCloseParent()
{
    // FIXME: DO NOT USE USERMODEL()
    if (userModel()->isDirty()) {
        int ret = Utils::withButtonsMessageBox(tr("You've modified the users list."), tr("Do you want to save your changes?"), "",
                                         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                         QMessageBox::Save, windowTitle());
        if (ret == QMessageBox::Discard)
            return true;
        else if (ret == QMessageBox::Cancel)
            return false;
        else if (userModel()->submitAll()) {
            QMessageBox::information(this, windowTitle(), tr("Changes have been saved successfully."));
            return true;
        } else {
            QMessageBox::information(this, windowTitle(), tr("Changes can not be saved successfully."));
            return false;
        }
    }
    return true;
}

/** Changes the search method for the users's model */
void UserManagerWidget::onSearchToolButtonTriggered(QAction *act)
{
    if (act == d->searchByNameAct)
        d->m_SearchBy= Core::IUser::UsualName;
    else if (act == d->searchByFirstnameAct)
        d->m_SearchBy= Core::IUser::Firstname;
    else if (act == d->searchByNameAndFirstnameAct)
        d->m_SearchBy= -1;
    else if (act == d->searchByCityAct)
        d->m_SearchBy= Core::IUser::City;
}

/**
 * \internal
 * Reacts to Core::IUser::currentUserChanged
 */
void UserManagerWidget::onCurrentUserChanged()
{
    int row = userModel()->currentUserIndex().row();
    d->ui->userTreeView->setCurrentIndex(d->ui->userTreeView->model()->index(row, Core::IUser::UsualName));
//    ui->userTreeView->selectRow(row);
    d->analyzeCurrentUserRights();
//    ui->userViewer->setCurrentUser(row);
}

/** Updates the users' model filter */
void UserManagerWidget::onSearchRequested()
{
    // TODO: Manage error when user select an action in the toolbutton
    // TODO: where can only be calculated by model
    // FIXME: DO NOT USE USERMODEL()
    QHash<int, QString> where;
    where.insert(d->m_SearchBy, QString("LIKE '%1%'").arg(d->ui->searchLineEdit->text()));
    userModel()->setFilter(where);
}

/** Creates a new user using UserPlugin::UserWizard. */
void UserManagerWidget::onCreateUserRequested()
{
    // FIXME: DO NOT USE TREEVIEW MODEL
    int createdRow = d->ui->userTreeView->model()->rowCount();
    if (!d->ui->userTreeView->model()->insertRows(createdRow, 1)) {
        LOG_ERROR("Error creating new user: cannot add row to model");
        return;
    }
    QModelIndex index = d->ui->userTreeView->model()->index(createdRow, USER_USUALNAME);
    UserCreatorWizard wiz(this);
    // TODO: code here
//    wiz.setModelRow(createdRow);
    int r = wiz.exec();
    if (r == QDialog::Rejected) {
        if (!d->ui->userTreeView->model()->removeRows(createdRow, 1)) {
            LOG_ERROR("Error deleting new user: cannot delete row from model");
            return;
        }
    } else {
        onCurrentSelectedIndexChanged(index, d->ui->userTreeView->currentIndex());
        d->m_model->setFilter(Internal::UserManagerModelFilter());
    }
}

void UserManagerWidget::onClearModificationRequested()
{
    if (userModel()->revertAll())
        Utils::informativeMessageBox(tr("Modification correctly cleared"), "");
}

/**
 * \internal
 * Save the model.
 */
void UserManagerWidget::onSaveRequested()
{
    if ((!d->m_CanModify) && (!d->m_CanCreate))
        return;
    d->m_ToolBar->setFocus();

    // tell all pages to submit data to the model
    d->ui->userViewer->submitChangesToModel();
}

/**
 * \internal
 * Slot connected to the "delete user" action
 */
void UserManagerWidget::onDeleteUserRequested()
{
    // FIXME: DO NOT USE USERMODEL()
    if (!d->ui->userTreeView->selectionModel()->hasSelection())
        return;

    // User can not delete himself
    if (d->ui->userTreeView->currentIndex().row() == userModel()->currentUserIndex().row())
        return;

    // Ask for a confirmation
    QModelIndex index = d->ui->userTreeView->currentIndex();
    while (index.parent().isValid()) index = index.parent();
    bool yes = Utils::yesNoMessageBox(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::DELETE_USER),
                                      tr("You are about to delete the following user<br><br>"
                                         "&nbsp;&nbsp;&nbsp;&nbsp;<b>%1</b><br><br>"
                                         "Do you really want to remove this user?")
                                      .arg(d->m_model->data(index).toString()));
    if (!yes)
        return;

    if (userModel()->removeRow(d->ui->userTreeView->currentIndex().row())) {
        LOG(tr("User deleted"));
    } else {
        LOG(tr("User can not be deleted"));
    }
    d->m_model->setFilter(Internal::UserManagerModelFilter());
    selectuserTreeView(userModel()->currentUserIndex().row());
    d->updateButtons();
}

void UserManagerWidget::toggleSearchView(bool checked)
{
    if (checked) {
        d->ui->splitter->setSizes(QList<int>() << 1 << 3);
    } else {
        d->ui->splitter->setSizes(QList<int>() << 0 << 1);
    }
}

void UserManagerWidget::onCurrentSelectedIndexChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    d->ui->userViewer->setCurrentUser(d->m_model->userUuid(current));
    d->ui->userViewer->setCurrentPage(d->m_model->pageIndexFromIndex(current));
    d->ui->userViewer->setEnabled(true);
    d->updateButtons();
}

void UserManagerWidget::selectuserTreeView(int row)
{
    Q_UNUSED(row);
    // TODO: manage this code
//    d->ui->userViewer->setCurrentUser(row);
}

/** Assume retranslation of UI. */
void UserManagerWidget::changeEvent(QEvent *e)
{
    if ((e->type() == QEvent::LanguageChange)) {
        if (d->ui) {
            d->ui->retranslateUi(this);
            retranslate();
       }
    }
}

/** Retranslate the UI */
void UserManagerWidget::retranslate()
{
    if (!d->searchByNameAct)
        return;
    d->searchByNameAct->setText(tr("Search user by name"));
    d->searchByFirstnameAct->setText(tr("Search user by firstname"));
    d->searchByNameAndFirstnameAct->setText(tr("Search user by name and firstname"));
    d->searchByCityAct->setText(tr("Search user by city"));

    d->searchByNameAct->setToolTip(d->searchByNameAct->text());
    d->searchByFirstnameAct->setToolTip(d->searchByFirstnameAct->text());
    d->searchByNameAndFirstnameAct->setToolTip(d->searchByNameAndFirstnameAct->text());
    d->searchByCityAct->setToolTip(d->searchByCityAct->text());
    d->m_SearchToolButton->setToolTip(d->m_SearchToolButton->text());

    d->aCreateUser->setText(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::CREATE_USER));
    d->aModifyUser->setText(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::MODIFY_USER));
    d->aSave->setText(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::SAVE_USER));
    d->aRevert->setText(tr("Clear modifications"));
    d->aDeleteUser->setText(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::DELETE_USER));
    d->aQuit->setText(tr("Quit User Manager"));
    d->aToggleSearchView->setText(tr("Search user"));
    d->aCreateUser->setToolTip(d->aCreateUser->text());
    d->aModifyUser->setToolTip(d->aModifyUser->text());
    d->aSave->setToolTip(d->aSave->text());
    d->aRevert->setToolTip(d->aRevert->text());
    d->aDeleteUser->setToolTip(d->aDeleteUser->text());
    d->aQuit->setToolTip(d->aQuit->text());
    d->aToggleSearchView->setToolTip(d->aToggleSearchView->text());
}

/** For debugging purpose */
void UserManagerWidget::showUserDebugDialog(const QModelIndex &id)
{
    // FIXME: DO NOT USE USERMODEL()
    QStringList list;
    list << userModel()->index(id.row(), Core::IUser::DebugText).data(Qt::DisplayRole).toStringList();
    Utils::quickDebugDialog(list);
}
