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
  \class UserPlugin::UserManager
  This is the Users' Manager of FreeMedForms.

  You can use it as main app or secondary mainWindow.\n
  You only need to instanciate the UserModel and define a current user before using this mainwindow.
  \ingroup usertoolkit widget_usertoolkit
  \ingroup usermanager
*/

#include "usermanager.h"
#include "usermanager_p.h"

#include <utils/log.h>
#include <utils/widgets/qbuttonlineedit.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>

#include <coreplugin/contextmanager/icontext.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>

#include <usermanagerplugin/userdata.h>
#include <usermanagerplugin/widgets/userviewer.h>
#include <usermanagerplugin/constants.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/widgets/userwizard.h>

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

#include "ui_usermanagerwidget.h"

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ContextManager *contextManager() {return  Core::ICore::instance()->contextManager();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

namespace UserPlugin {
namespace Internal {

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


/**
  Main user interface for User Manager.
  User Model must have been instanciated BEFORE this interface, and a current user must have been set.\n
  You must instanciate this class as a pointer in order to avoid errors at deletion.
  \sa UserModel, UserModel::hasCurrentUser()
  \todo Search user by city, search by name & firstname,
*/
UserManager::UserManager(QWidget * parent) :
        QMainWindow(parent)
{
    Q_ASSERT_X(UserModel::instance()->hasCurrentUser(), "UserManager", "NO CURRENT USER");
    if (!UserModel::instance()->hasCurrentUser())
        return;
    setAttribute(Qt::WA_DeleteOnClose);
    m_Widget = new UserManagerWidget(this);
    setCentralWidget(m_Widget);
    setUnifiedTitleAndToolBarOnMac(true);
}

bool UserManager::initialize()
{
    m_Widget->initialize();
    return true;
}

/** Close the usermanager. Check if modifications have to be saved and ask user. */
void UserManager::closeEvent(QCloseEvent *event)
{
    if (m_Widget->canCloseParent()) {
        event->accept();
    } else {
        event->ignore();
    }
}

/** Destructor */
UserManager::~UserManager()
{
}



/**
  Main user interface for User Manager.
  User Model must have been instanciated BEFORE this interface, and a current user must have been set.\n
  You must instanciate this class as a pointer in order to avoid errors at deletion.
  \sa UserModel, UserModel::hasCurrentUser()
  \todo Search user by city, search by name & firstname,
*/
UserManagerDialog::UserManagerDialog(QWidget * parent) :
        QDialog(parent)
{
    Q_ASSERT_X(UserModel::instance()->hasCurrentUser(), "UserManagerDialog", "NO CURRENT USER");
    if (!UserModel::instance()->hasCurrentUser())
        return;
//    setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lay = new QGridLayout(this);
    setLayout(lay);
    m_Widget = new UserManagerWidget(this);
    lay->addWidget(m_Widget, 0, 0);
}

bool UserManagerDialog::initialize()
{
    m_Widget->initialize();
    setWindowTitle(tkTr(Trans::Constants::USERMANAGER_TEXT).remove("&"));
    setWindowIcon(theme()->icon(Core::Constants::ICONUSERMANAGER));
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
    Utils::centerWidget(this);
}


/** Destructor */
UserManagerDialog::~UserManagerDialog()
{
}


/** UserManager Main Ui interface. */
UserManagerWidget::UserManagerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserManagerWidget),
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
    m_Context(0)
{
    ui->setupUi(this);
    ui->splitter->setSizes(QList<int>() << 1 << 3);
    m_SearchBy = Core::IUser::Name;
    aCreateUser = new QAction(this);
    aCreateUser->setObjectName(QString::fromUtf8("aCreateUser"));
    aModifyUser = new QAction(this);
    aModifyUser->setObjectName(QString::fromUtf8("aModifyUser"));
    aSave = new QAction(this);
    aSave->setObjectName(QString::fromUtf8("aSave"));
    aRevert = new QAction(this);
    aRevert->setObjectName(QString::fromUtf8("aRevert"));
    aDeleteUser = new QAction(this);
    aDeleteUser->setObjectName(QString::fromUtf8("aDeleteUser"));
    aQuit = new QAction(this);
    aQuit->setObjectName(QString::fromUtf8("aQuit"));
    aToggleSearchView = new QAction(this);
    aToggleSearchView->setCheckable(true);
    aToggleSearchView->setChecked(true);
    aToggleSearchView->setObjectName(QString::fromUtf8("aToggleSearchView"));

    // prepare Search Line Edit
    searchByNameAct = new QAction(this);
    searchByFirstnameAct = new QAction(this);
    searchByNameAndFirstnameAct = new QAction(this);
    searchByCityAct = new QAction(this);

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

    m_ToolBar = new QToolBar(this);
    m_ToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_ToolBar->addAction(aToggleSearchView);
    m_ToolBar->addAction(aCreateUser);
    m_ToolBar->addAction(aModifyUser);
    m_ToolBar->addAction(aSave);
    m_ToolBar->addAction(aDeleteUser);
    m_ToolBar->addAction(aRevert);
//    m_ToolBar->addAction(aQuit);
    m_ToolBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->toolbarLayout->addWidget(m_ToolBar);

    m_SearchToolButton = new QToolButton(ui->searchLineEdit);
    m_SearchToolButton->addAction(searchByNameAct);
    m_SearchToolButton->addAction(searchByFirstnameAct);
    //    m_SearchToolButton->addAction(searchByNameAndFirstnameAct);
    //    m_SearchToolButton->addAction(searchByCityAct);
    m_SearchToolButton->setPopupMode(QToolButton::InstantPopup);
    m_SearchToolButton->setDefaultAction(searchByNameAct);
    ui->searchLineEdit->setLeftButton(m_SearchToolButton);

    ui->userViewer->setEnabled(false);
}

bool UserManagerWidget::initialize()
{
    UserModel *model = UserModel::instance();
    ui->userTableView->setModel(model);
    for(int i=0; i < model->columnCount(); ++i) {
        ui->userTableView->hideColumn(i);
    }
    ui->userTableView->showColumn(Core::IUser::Name);
    ui->userTableView->showColumn(Core::IUser::SecondName);
    ui->userTableView->showColumn(Core::IUser::Firstname);
    ui->userTableView->horizontalHeader()->setStretchLastSection(true);
    ui->userTableView->horizontalHeader()->setResizeMode(Core::IUser::Name, QHeaderView::ResizeToContents);
    ui->userTableView->horizontalHeader()->setResizeMode(Core::IUser::SecondName, QHeaderView::ResizeToContents);
    ui->userTableView->horizontalHeader()->setResizeMode(Core::IUser::Firstname, QHeaderView::ResizeToContents);
    ui->userTableView->horizontalHeader()->hide();
    ui->userTableView->verticalHeader()->hide();
    ui->userTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->userTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->userTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    retranslate();

    aSave->setShortcut(QKeySequence::Save);
    aCreateUser->setShortcut(QKeySequence::New);

    // connect actions
    connect(aSave, SIGNAL(triggered()), this, SLOT(onSaveRequested()));
    connect(aCreateUser, SIGNAL(triggered()), this, SLOT(onCreateUserRequested()));
    connect(aRevert, SIGNAL(triggered()), this, SLOT(onClearModificationRequested()));
    connect(aDeleteUser,  SIGNAL(triggered()), this, SLOT(onDeleteUserRequested()));
    connect(aQuit,  SIGNAL(triggered()), this, SIGNAL(closeRequested()));
    connect(aToggleSearchView, SIGNAL(toggled(bool)), this, SLOT(toggleSearchView(bool)));

    // connect tableView selector
    connect(ui->userTableView, SIGNAL(clicked(QModelIndex)),
             this, SLOT(onUserClicked(const QModelIndex &)));

    // connect search line edit
    connect(ui->searchLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(onSearchRequested()));
    connect(m_SearchToolButton, SIGNAL(triggered(QAction*)), this, SLOT(onSearchToolButtonTriggered(QAction*)));

    connect(user(), SIGNAL(userChanged()), this, SLOT(onCurrentUserChanged()));

    return true;
}

UserManagerWidget::~UserManagerWidget()
{
    delete ui;
}

bool UserManagerWidget::canCloseParent()
{
    if (UserModel::instance()->hasUserToSave()) {
        int ret = Utils::withButtonsMessageBox(tr("You've modified the users list."), tr("Do you want to save your changes?"), "",
                                         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                         QMessageBox::Save, windowTitle());
        if (ret == QMessageBox::Discard)
            return true;
        else if (ret == QMessageBox::Cancel)
            return false;
        else if (UserModel::instance()->submitAll()) {
            QMessageBox::information(this, windowTitle(), tr("Changes have been saved successfully."));
            return true;
        } else {
            QMessageBox::information(this, windowTitle(), tr("Changes can not be saved successfully."));
            return false;
        }
    }
    return true;
}

void UserManagerWidget::analyseCurrentUserRights()
{
    // retreive user manager rights from model
    UserModel *m = UserModel::instance();
    Core::IUser::UserRights r (m->currentUserData(Core::IUser::ManagerRights).toInt());
    // translate to bools
    m_CanModify = (r & Core::IUser::WriteAll);
    m_CanCreate = (r & Core::IUser::Create);
    m_CanViewAllUsers = (r & Core::IUser::ReadAll);
    m_CanDelete = (r & Core::IUser::Delete);

    // manage ui <-> rights
    aToggleSearchView->setEnabled(m_CanViewAllUsers);
    aCreateUser->setEnabled(m_CanCreate);
    aModifyUser->setEnabled(m_CanModify);
    aDeleteUser->setEnabled(m_CanDelete);

    // manage specific creation widgets
    ui->userTableView->setVisible(m_CanViewAllUsers);
    ui->searchLineEdit->setVisible(m_CanViewAllUsers);
}

/** Change the search method for the users's model */
void UserManagerWidget::onSearchToolButtonTriggered(QAction *act)
{
    if (act == searchByNameAct)
        m_SearchBy= Core::IUser::Name;
    else if (act == searchByFirstnameAct)
        m_SearchBy= Core::IUser::Firstname;
    else if (act == searchByNameAndFirstnameAct)
        m_SearchBy= -1;
    else if (act == searchByCityAct)
        m_SearchBy= Core::IUser::City;
}

void UserManagerWidget::onCurrentUserChanged()
{
    int row = UserModel::instance()->currentUserIndex().row();
    ui->userTableView->setCurrentIndex(ui->userTableView->model()->index(row, Core::IUser::Name));
    ui->userTableView->selectRow(row);
    analyseCurrentUserRights();
    ui->userViewer->changeUserTo(row);
}

/**
  Update the users' model filter
  \todo Manage error when user select an action in the toolbutton
  \todo where can only be calculated by model
 */
void UserManagerWidget::onSearchRequested()
{
    QHash<int, QString> where;
    where.insert(m_SearchBy, QString("LIKE '%1%'").arg(ui->searchLineEdit->text()));
    UserModel::instance()->setFilter(where);
}

/** Create a new user using UserPlugin::UserWizard. */
void UserManagerWidget::onCreateUserRequested()
{
    int createdRow = ui->userTableView->model()->rowCount();
    if (!ui->userTableView->model()->insertRows(createdRow, 1)) {
        LOG_ERROR("Error creating new user: cannot add row to model");
        return;
    }
    QModelIndex index = ui->userTableView->model()->index(createdRow, USER_NAME);
    UserWizard wiz(this);
    // TODO: code here
//    wiz.setModelRow(createdRow);
    int r = wiz.exec();
    if (r == QDialog::Rejected) {
        if (!ui->userTableView->model()->removeRows(createdRow, 1)) {
            LOG_ERROR("Error deleting new user: cannot delete row from model");
            return;
        }
    } else {
        ui->userTableView->selectRow(createdRow);
        onUserClicked(index);
    }
}

void UserManagerWidget::onClearModificationRequested()
{
//    if (UserModel::instance()->revertAll())
//        m_Parent->statusBar()->showMessage(tr("Modifications cleared"), 2000);
//    else
//        m_Parent->statusBar()->showMessage(tr("Can not clear modifications"), 2000);
}

void UserManagerWidget::onSaveRequested()
{
    if ((!m_CanModify) || (!m_CanCreate))
        return;
    m_ToolBar->setFocus();

    // tell all pages to submit data to the model
    ui->userViewer->submitChangesToModel();

    // submit user to database
    QString uuid = ui->userTableView->model()->index(ui->userTableView->currentIndex().row(), Core::IUser::Uuid).data().toString();
    if (!UserModel::instance()->submitUser(uuid)) {
        LOG_ERROR("Unable to save user " + uuid);
    }
}

void UserManagerWidget::onDeleteUserRequested()
{
    if (!ui->userTableView->selectionModel()->hasSelection())
        return;

    // User can not delete himself
    if (ui->userTableView->currentIndex().row() == UserModel::instance()->currentUserIndex().row())
        return;

    if (UserModel::instance()->removeRow(ui->userTableView->currentIndex().row())) {
        LOG(tr("User deleted"));
    } else {
        LOG(tr("User can not be deleted"));
    }
    selectUserTableView(UserModel::instance()->currentUserIndex().row());
}

void UserManagerWidget::toggleSearchView(bool checked)
{
    if (checked) {
        ui->splitter->setSizes(QList<int>() << 1 << 3);
    } else {
        ui->splitter->setSizes(QList<int>() << 0 << 1);
    }
}

void UserManagerWidget::onUserClicked(const QModelIndex &index)
{
    ui->userViewer->changeUserTo(index.row());
    ui->userViewer->setEnabled(true);
}

void UserManagerWidget::selectUserTableView(int row)
{
    ui->userViewer->changeUserTo(row);
}

/** Assume retranslation of UI. */
void UserManagerWidget::changeEvent(QEvent *e)
{
    if ((e->type() == QEvent::LanguageChange)) {
        if (ui) {
            ui->retranslateUi(this);
            retranslate();
       }
    }
}

/** Retranslate the UI */
void UserManagerWidget::retranslate()
{
    if (!searchByNameAct)
        return;
    searchByNameAct->setText(tr("Search user by name"));
    searchByFirstnameAct->setText(tr("Search user by firstname"));
    searchByNameAndFirstnameAct->setText(tr("Search user by name and firstname"));
    searchByCityAct->setText(tr("Search user by city"));

    searchByNameAct->setToolTip(searchByNameAct->text());
    searchByFirstnameAct->setToolTip(searchByFirstnameAct->text());
    searchByNameAndFirstnameAct->setToolTip(searchByNameAndFirstnameAct->text());
    searchByCityAct->setToolTip(searchByCityAct->text());
    m_SearchToolButton->setToolTip(m_SearchToolButton->text());

    aCreateUser->setText(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::CREATE_USER));
    aCreateUser->setToolTip(aCreateUser->text());
    aModifyUser->setText(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::MODIFY_USER));
    aModifyUser->setToolTip(aModifyUser->text());
    aSave->setText(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::SAVE_USER));
    aSave->setToolTip(aSave->text());
    aRevert->setText(tr("Clear modifications"));
    aRevert->setToolTip(aRevert->text());
    aDeleteUser->setText(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::DELETE_USER));
    aDeleteUser->setToolTip(aDeleteUser->text());
    aQuit->setText(tr("Quit User Manager"));
    aQuit->setToolTip(aQuit->text());
    aToggleSearchView->setText(tr("Search user"));
    aToggleSearchView->setToolTip(aToggleSearchView->text());
}

/** For debugging purpose */
void UserManagerWidget::showUserDebugDialog(const QModelIndex &id)
{
    QStringList list;
    list << UserModel::instance()->index(id.row(), Core::IUser::WarnText).data(Qt::DisplayRole).toStringList();
    Utils::quickDebugDialog(list);
}
