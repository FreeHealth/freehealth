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
  \class UserPlugin::UserManager
  \brief This is the Users' Manager of FreeMedForms.

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

#include <coreplugin/contextmanager/icontext.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>

#include <usermanagerplugin/userdata.h>
#include <usermanagerplugin/widgets/userviewer.h>
#include <usermanagerplugin/constants.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/widgets/userwizard.h>

#include <QUuid>
#include <QModelIndex>
#include <QTableView>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QSplitter>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QToolButton>
#include <QLabel>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>

using namespace UserPlugin::Internal;
using namespace UserPlugin;

static inline  Core::ContextManager *contextManager() {return  Core::ICore::instance()->contextManager();}

namespace UserPlugin {
namespace Internal {

class UserManagerContext : public Core::IContext
{
public:
    UserManagerContext(UserManager *parent) : Core::IContext(parent), wgt(parent)
    {
        setObjectName("UserManagerContext");
        ctx << Core::ICore::instance()->uniqueIDManager()->uniqueIdentifier(Core::Constants::C_USERMANAGER);
    }
    ~UserManagerContext() {}

    QList<int> context() const {return ctx;}
    QWidget *widget() {return wgt;}

private:
    UserManager *wgt;
    QList<int> ctx;
};

}  // End  Internal
}  // End UserPlugin


/**
  \brief Main user interface for User Manager.
  User Model must have been instanciated BEFORE this interface, and a current user must have been setted.\n
  You must instanciate this class as a pointer in order to avoid errors at deletion.
  \sa UserModel, UserModel::hasCurrentUser()
  \todo Search user by city, search by name & firstname,
  \ingroup widget_usertoolkit usertoolkit usermanager
*/
UserManager::UserManager(QWidget * parent)
    : QMainWindow(parent)
{
    Q_ASSERT_X(UserModel::instance()->hasCurrentUser(), "UserManager", "NO CURRENT USER");
    if (!UserModel::instance()->hasCurrentUser())
        return;
    setAttribute(Qt::WA_DeleteOnClose);
    d = new UserManagerPrivate(this);
    setUnifiedTitleAndToolBarOnMac(true);
}

bool UserManager::initialize()
{
    d->m_Context = new UserManagerContext(this);
    contextManager()->addContextObject(d->m_Context);
    d->initialize();
    return true;
}

/** \brief Close the usermanager. Check if modifications have to be saved and ask user. */
void UserManager::closeEvent(QCloseEvent *event)
{
    if (UserModel::instance()->hasUserToSave()) {
        int ret = Utils::withButtonsMessageBox(tr("You've modified the users' list."), tr("Do you want to save your changes ?"), "",
                                         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                         QMessageBox::Save, windowTitle());
        if (ret == QMessageBox::Discard)
            event->accept();
        else if (ret == QMessageBox::Cancel)
            event->ignore();
        else if (UserModel::instance()->submitAll()) {
            QMessageBox::information(this, windowTitle(), tr("Changes have been correctly saved."));
            event->accept();
        }
        else {
            QMessageBox::information(this, windowTitle(), tr("Changes can not be correctly saved."));
            event->ignore();
        }
    }
    else
        event->accept();
}

/** \brief Destructor */
UserManager::~UserManager()
{
    if (Utils::isDebugCompilation())
        qWarning() << "~UserManager";
    contextManager()->removeContextObject(d->m_Context);
    if (d) {
        delete d;
        d = 0;
    }
}

/**
  \brief UserManager Main Ui interface.
  \internal
  \ingroup widget_usertoolkit usertoolkit usermanager
*/
UserManagerPrivate::UserManagerPrivate(QMainWindow * parent) :
        QObject(parent),
        m_Parent(parent),
        m_SearchToolBut(0),
        searchByNameAct(0), searchByFirstnameAct(0), searchByNameAndFirstnameAct(0), searchByCityAct(0),
        m_PermanentUserName(0),
        m_PermanentWidget(0),
        m_Context(0)
{
    m_SearchBy = Core::IUser::Name;
}

bool UserManagerPrivate::initialize()
{
    setupUi(m_Parent);

    toolBar->addAction(createNewUserAct);
    toolBar->addAction(modifyUserAct);
    toolBar->addAction(saveAct);
    toolBar->addAction(deleteUserAct);
    toolBar->addAction(clearModificationsAct);
    toolBar->addAction(quitUserManagerAct);
    UserModel *model = UserModel::instance();

    userTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    userTableView->setModel(model);
    userTableView->horizontalHeader()->hide();
    userTableView->verticalHeader()->hide();
    userTableView->hideColumn(USER_ID);
    userTableView->hideColumn(USER_UUID);
    userTableView->hideColumn(USER_LOGIN);
    userTableView->hideColumn(USER_PASSWORD);
    userTableView->hideColumn(USER_LANGUAGE);
    userTableView->hideColumn(USER_MAIL);
    userTableView->hideColumn(USER_VALIDITY);
    userTableView->hideColumn(USER_SECONDNAME);
    userTableView->hideColumn(USER_LASTLOG);
    userTableView->hideColumn(USER_LOCKER);
    userTableView->resizeColumnsToContents();
    userTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    userTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTableView->horizontalHeader()->setStretchLastSection(true);
    // prepare Search Line Edit
    m_SearchToolBut = new QToolButton(searchLineEdit);
    searchLineEdit->setLeftButton(m_SearchToolBut);
    searchByNameAct = new QAction(m_SearchToolBut);
    searchByFirstnameAct = new QAction(m_SearchToolBut);
    searchByNameAndFirstnameAct = new QAction(m_SearchToolBut);
    searchByCityAct = new QAction(m_SearchToolBut);
    m_SearchToolBut->addAction(searchByNameAct);
    m_SearchToolBut->addAction(searchByFirstnameAct);
    //    m_SearchToolBut->addAction(searchByNameAndFirstnameAct);
    //    m_SearchToolBut->addAction(searchByCityAct);
    m_SearchToolBut->setPopupMode(QToolButton::InstantPopup);
    retranslate();

    selectUserTableView(UserModel::instance()->currentUserIndex().row());

    analyseCurrentUserRights();

    saveAct->setShortcut(QKeySequence::Save);
    createNewUserAct->setShortcut(QKeySequence::New);


    // connect slots
    connect(saveAct, SIGNAL(triggered()), this, SLOT(on_saveAct_triggered()));
    connect(createNewUserAct, SIGNAL(triggered()), this, SLOT(on_createNewUserAct_triggered()));
    connect(clearModificationsAct, SIGNAL(triggered()), this, SLOT(on_clearModificationsAct_triggered()));
    connect(deleteUserAct,  SIGNAL(triggered()), this, SLOT(on_deleteUserAct_triggered()));
    connect(quitUserManagerAct,  SIGNAL(triggered()), m_Parent, SLOT(close()));
    connect(userTableView, SIGNAL(activated(const QModelIndex &)),
             this, SLOT(on_userTableView_activated(const QModelIndex &)));
    // connections for search line edit
    connect(searchLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(on_searchLineEdit_textchanged()));
    connect(m_SearchToolBut, SIGNAL(triggered(QAction*)), this, SLOT(on_m_SearchToolButton_triggered(QAction*)));

    updateStatusBar();
    connect(UserModel::instance(), SIGNAL(memoryUsageChanged()), this, SLOT(updateStatusBar()));

//    if (Utils::isDebugCompilation())
//        connect(userTableView,SIGNAL(activated(QModelIndex)), this, SLOT(showUserDebugDialog(QModelIndex)));

    // manage theme / icons
    Core::ITheme *th = Core::ICore::instance()->theme();
    m_Parent->setWindowIcon(th->icon(Core::Constants::ICONUSERMANAGER));
    saveAct->setIcon(th->icon(Core::Constants::ICONSAVE, Core::ITheme::MediumIcon));
    createNewUserAct->setIcon(th->icon(Core::Constants::ICONNEWUSER, Core::ITheme::MediumIcon));
    modifyUserAct->setIcon(th->icon(Core::Constants::ICONEDITUSER, Core::ITheme::MediumIcon));
    clearModificationsAct->setIcon(th->icon(Core::Constants::ICONCLEARUSER, Core::ITheme::MediumIcon));
    deleteUserAct->setIcon(th->icon(Core::Constants::ICONDELETEUSER, Core::ITheme::MediumIcon));
    quitUserManagerAct->setIcon(th->icon(Core::Constants::ICONEXIT, Core::ITheme::MediumIcon));
    m_SearchToolBut->setIcon(th->icon(Core::Constants::ICONSEARCH));
    searchByNameAct->setIcon(th->icon(Core::Constants::ICONSEARCH));
    searchByFirstnameAct->setIcon(th->icon(Core::Constants::ICONSEARCH));
    searchByNameAndFirstnameAct->setIcon(th->icon(Core::Constants::ICONSEARCH));
    searchByCityAct->setIcon(th->icon(Core::Constants::ICONSEARCH));

    // TODO active userTableView on currentUser
    return true;
}

UserManagerPrivate::~UserManagerPrivate()
{
    if (Utils::isDebugCompilation())
        qWarning() << "~UserManagerPrivate";
}

void UserManagerPrivate::analyseCurrentUserRights()
{
    // retreive user manager rights from model
    UserModel *m = UserModel::instance();
    Core::IUser::UserRights r (m->data(m->index(m->currentUserIndex().row(), Core::IUser::ManagerRights)).toInt());
    // translate to bools
    m_CanModify = (r & Core::IUser::WriteAll);
    m_CanCreate = (r & Core::IUser::Create);
    m_CanViewAllUsers = (r & Core::IUser::ReadAll);
    m_CanDelete = (r & Core::IUser::Delete);

    // manage ui <-> rights
    createNewUserAct->setEnabled(m_CanCreate);
    modifyUserAct->setEnabled(m_CanModify);
    deleteUserAct->setEnabled(m_CanDelete);

    // manage specific creation widgets
    userTableView->setVisible(m_CanViewAllUsers);
    searchLineEdit->setVisible(m_CanViewAllUsers);
}

/** \brief upadet status bar for current user, and refresh memory usage group */
void UserManagerPrivate::updateStatusBar()
{
    UserModel *m = UserModel::instance();
    if (! m_PermanentWidget) {
        m_PermanentWidget = new QWidget(m_Parent);
        QHBoxLayout * l = new QHBoxLayout(m_PermanentWidget);
        l->setMargin(0);
        if (! m_PermanentUserName)
            m_PermanentUserName = new QLabel(m_PermanentWidget);
        l->addWidget(m_PermanentUserName);
    }
    memoryUsageLabel->setText(tr("Database usage : %1,\nMemory usage : %2")
                                     .arg(m->rowCount())
                                     .arg(m->numberOfUsersInMemory()));
    m_PermanentUserName->setText(m->index(m->currentUserIndex().row(), Core::IUser::Name).data().toString());
    m_Parent->statusBar()->addPermanentWidget(m_PermanentWidget);
}

/** \brief Change the search method for the users's model */
void UserManagerPrivate::on_m_SearchToolButton_triggered(QAction * act)
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

/**
  \brief Update the users' model filter
  \todo Manage error when user select an action in the toolbutton
  \todo where can only be calculated by model
 */
void UserManagerPrivate::on_searchLineEdit_textchanged()
{
    QHash<int, QString> where;
    where.insert(m_SearchBy, QString("LIKE '%1%'").arg(searchLineEdit->searchText()));
    UserModel::instance()->setFilter(where);
}

/** \brief Create a new user using tkUserWizard. */
void UserManagerPrivate::on_createNewUserAct_triggered()
{
    int createdRow = userTableView->model()->rowCount();
    if (! userTableView->model()->insertRows(createdRow, 1)) {
        Utils::Log::addError(this, "Cannot create new user : can not add row to model");
        return;
    }
    QModelIndex index = userTableView->model()->index(createdRow, USER_NAME);
//    userTableView->model()->setData(index, tr("New User"));
    UserWizard wiz(m_Parent);
    wiz.setModelRow(createdRow);
    int r = wiz.exec();
    if (r == QDialog::Rejected) {
        if (! userTableView->model()->removeRows(createdRow, 1)) {
            Utils::Log::addError(this, "Cannot delete new user : can not delete row to model");
            return;
        } else
            m_Parent->statusBar()->showMessage(tr("No user created"), 2000);
    } else {
        userTableView->selectRow(createdRow);
        on_userTableView_activated(index);
        m_Parent->statusBar()->showMessage(tr("User created"), 2000);
    }
    qApp->setActiveWindow(m_Parent);
    m_Parent->activateWindow();
}

void UserManagerPrivate::on_clearModificationsAct_triggered()
{
    if (UserModel::instance()->revertAll())
        m_Parent->statusBar()->showMessage(tr("Modifications cleared"), 2000);
    else
        m_Parent->statusBar()->showMessage(tr("Can not clear modifications"), 2000);
}

void UserManagerPrivate::on_saveAct_triggered()
{
    if ((!m_CanModify) || (! m_CanCreate))
        return;
    // redefine focus
    m_Parent->statusBar()->setFocus();
    // save changes to database
    if (UserModel::instance()->submitAll())
        m_Parent->statusBar()->showMessage(tr("User saved"), 2000);
}

void UserManagerPrivate::on_deleteUserAct_triggered()
{
    if (UserModel::instance()->removeRow(userTableView->currentIndex().row()))
        m_Parent->statusBar()->showMessage(tr("User deleted"), 2000);
    else
        m_Parent->statusBar()->showMessage(tr("User can not be deleted"), 2000);
}

void UserManagerPrivate::on_userTableView_activated(const QModelIndex & index)
{
    userViewer->changeUserTo(index.row());
}

void  UserManagerPrivate::selectUserTableView(int row)
{
    userTableView->selectRow(row);
    userViewer->changeUserTo(row);
}

/** \brief Assume retranslation of ui. */
void UserManagerPrivate::changeEvent(QEvent *e)
{
    if ((e->type() == QEvent::LanguageChange)) {
        retranslateUi(m_Parent);
        m_Parent->setWindowTitle(tr("User Manager") + " - " + qApp->applicationName());
    }
    retranslate();
}

void UserManagerPrivate::retranslate()
{
    searchByNameAct->setText(tr("Search user by name"));
    searchByFirstnameAct->setText(tr("Search user by firstname"));
    searchByNameAndFirstnameAct->setText(tr("Search user by name and firstname"));
    searchByCityAct->setText(tr("Search user by city"));
    searchByNameAct->setToolTip(tr("Search user by name"));
    searchByFirstnameAct->setToolTip(tr("Search user by firstname"));
    searchByNameAndFirstnameAct->setToolTip(tr("Search user by name and firstname"));
    searchByCityAct->setToolTip(tr("Search user by city"));
    m_SearchToolBut->setToolTip(tr("Search a user"));
    m_Parent->setWindowTitle(tr("User Manager") + " - " + qApp->applicationName());
    updateStatusBar();
}

void UserManagerPrivate::showUserDebugDialog(const QModelIndex &id)
{
    QStringList list;
    list << UserModel::instance()->index(id.row(), Core::IUser::WarnText).data(Qt::DisplayRole).toStringList();
    Utils::quickDebugDialog(list);
}
