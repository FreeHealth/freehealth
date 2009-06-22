/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class tkUserManager
  \brief This is the Users' Manager of FreeMedForms.

  You can use it as main app or secondary mainWindow.\n
  You only need to instanciate the tkUserModel and define a current user before using this mainwindow.
  \ingroup usertoolkit widget_usertoolkit
  \ingroup usermanager
*/

#include "tkUserManager.h"
#include "tkUserManager_p.h"

// include toolkit headers
#include <tkLog.h>
#include <tkTheme.h>
#include <QButtonLineEdit.h>

// include usertoolkit headers
#include <tkUser.h>
#include <tkUserViewer.h>
#include <tkUserConstants.h>
#include <tkUserModel.h>
#include <tkUserWizard.h>

// include Qt headers
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

Q_TKUSER_USING_CONSTANTS
Q_TK_USING_CONSTANTS

/**
  \brief Main user interface for User Manager.
  User Model must have been instanciated BEFORE this interface, and a current user must have been setted.\n
  You must instanciate this class as a pointer in order to avoid errors at deletion.
  \sa tkUserModel, tkUserModel::hasCurrentUser()
  \todo Search user by city, search by name & surname,
  \ingroup widget_usertoolkit usertoolkit usermanager
*/
tkUserManager::tkUserManager( QWidget * parent )
    : QMainWindow( parent )
{
    Q_ASSERT_X( tkUserModel::instance()->hasCurrentUser(), "tkUserManager", "NO CURRENT USER" );
    if ( ! tkUserModel::instance()->hasCurrentUser() == QVariant() )
        return;
    setAttribute( Qt::WA_DeleteOnClose );
    d = new tkUserManagerPrivate(this);
    setUnifiedTitleAndToolBarOnMac(true);
}

/** \brief Close the usermanager. Check if modifications have to be saved and ask user. */
void tkUserManager::closeEvent( QCloseEvent *event )
{
    if ( tkUserModel::instance()->hasUserToSave() ) {
        int ret = tkGlobal::withButtonsMessageBox( tr("You've modified the users' list."), tr("Do you want to save your changes ?"), "",
                                         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                         QMessageBox::Save, windowTitle() );
        if ( ret == QMessageBox::Discard )
            event->accept();
        else if ( ret == QMessageBox::Cancel )
            event->ignore();
        else if ( tkUserModel::instance()->submitAll() ) {
            QMessageBox::information( this, windowTitle(), tr( "Changes have been correctly saved." ) );
            event->accept();
        }
        else {
            QMessageBox::information( this, windowTitle(), tr( "Changes can not be correctly saved." ) );
            event->ignore();
        }
    }
    else
        event->accept();
}

/** \brief Destructor */
tkUserManager::~tkUserManager()
{
    if (tkGlobal::isDebugCompilation())
        qWarning() << "~tkUserManager";
}

/**
  \brief UserManager Main Ui interface.
  \internal
  \ingroup widget_usertoolkit usertoolkit usermanager
*/
tkUserManagerPrivate::tkUserManagerPrivate( QMainWindow * parent )
    : QObject( parent ),
    m_Parent(parent),
    m_SearchToolBut(0),
    searchByNameAct(0), searchBySurnameAct(0), searchByNameAndSurnameAct(0), searchByCityAct(0),
    m_PermanentUserName(0),
    m_PermanentWidget(0)
{
    m_SearchBy = User::UserName;
    setupUi(parent);

    toolBar->addActions( menuUser_manager->actions() );
    tkUserModel * model = tkUserModel::instance();

    userTableView->setEditTriggers( QAbstractItemView::NoEditTriggers );
    userTableView->setModel( model );
    userTableView->horizontalHeader()->hide();
    userTableView->verticalHeader()->hide();
    userTableView->hideColumn( USER_ID );
    userTableView->hideColumn( USER_UUID );
    userTableView->hideColumn( USER_LOGIN );
    userTableView->hideColumn( USER_PASSWORD );
    userTableView->hideColumn( USER_LANGUAGE );
    userTableView->hideColumn( USER_MAIL );
    userTableView->hideColumn( USER_VALIDITY );
    userTableView->hideColumn( USER_SECONDNAME );
    userTableView->hideColumn( USER_LASTLOG );
    userTableView->hideColumn( USER_LOCKER );
    userTableView->resizeColumnsToContents();
    userTableView->setSelectionMode( QAbstractItemView::SingleSelection );
    userTableView->setSelectionBehavior( QAbstractItemView::SelectRows );
    // prepare Search Line Edit
    m_SearchToolBut = new QToolButton( searchLineEdit );
    searchLineEdit->setLeftButton( m_SearchToolBut );
    searchByNameAct = new QAction( m_SearchToolBut );
    searchBySurnameAct = new QAction( m_SearchToolBut );
    searchByNameAndSurnameAct = new QAction( m_SearchToolBut );
    searchByCityAct = new QAction( m_SearchToolBut );
    m_SearchToolBut->addAction( searchByNameAct );
    m_SearchToolBut->addAction( searchBySurnameAct );
    //    m_SearchToolBut->addAction( searchByNameAndSurnameAct );
    //    m_SearchToolBut->addAction( searchByCityAct );
    m_SearchToolBut->setPopupMode( QToolButton::InstantPopup );
    retranslate();

    selectUserTableView( tkUserModel::instance()->currentUserIndex().row() );

    analyseCurrentUserRights();

    //    userTableView->hide();
    saveAct->setShortcut( QKeySequence::Save );
    createNewUserAct->setShortcut( QKeySequence::New );

    // connect slots
    connect( saveAct, SIGNAL(triggered()), this, SLOT(on_saveAct_triggered()) );
    connect( createNewUserAct, SIGNAL(triggered()), this, SLOT(on_createNewUserAct_triggered()) );
    connect( clearModificationsAct, SIGNAL(triggered()), this, SLOT(on_clearModificationsAct_triggered()) );
    connect( deleteUserAct,  SIGNAL(triggered()), this, SLOT(on_deleteUserAct_triggered()) );
    connect( quitUserManagerAct,  SIGNAL(triggered()), m_Parent, SLOT(close()) );
    connect( userTableView, SIGNAL( activated( const QModelIndex & ) ),
             this, SLOT( on_userTableView_activated( const QModelIndex & ) ) );
    // connections for search line edit
    connect( searchLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT( on_searchLineEdit_textchanged() ) );
    connect( m_SearchToolBut, SIGNAL(triggered(QAction*)), this, SLOT(on_m_SearchToolButton_triggered(QAction*)) );

    updateStatusBar();
    connect( tkUserModel::instance(), SIGNAL(memoryUsageChanged()), this, SLOT(updateStatusBar()) );

    if (tkGlobal::isDebugCompilation())
        connect(userTableView,SIGNAL(activated(QModelIndex)), this, SLOT(showUserDebugDialog(QModelIndex)));

    // manage theme / icons
    parent->setWindowIcon( tkTheme::icon( ICONUSERMANAGER ) );
    saveAct->setIcon( tkTheme::icon( ICONSAVE, tkTheme::MediumIcon ) );
    createNewUserAct->setIcon( tkTheme::icon( ICONNEWUSER, tkTheme::MediumIcon ) );
    modifyUserAct->setIcon( tkTheme::icon( ICONEDITUSER, tkTheme::MediumIcon ) );
    clearModificationsAct->setIcon( tkTheme::icon( ICONCLEARUSER, tkTheme::MediumIcon ) );
    deleteUserAct->setIcon( tkTheme::icon( ICONDELETEUSER, tkTheme::MediumIcon ) );
    quitUserManagerAct->setIcon( tkTheme::icon( ICONEXIT, tkTheme::MediumIcon ) );
    m_SearchToolBut->setIcon( tkTheme::icon( ICONSEARCH ) );
    searchByNameAct->setIcon( tkTheme::icon( ICONSEARCH ) );
    searchBySurnameAct->setIcon( tkTheme::icon( ICONSEARCH ) );
    searchByNameAndSurnameAct->setIcon( tkTheme::icon( ICONSEARCH ) );
    searchByCityAct->setIcon( tkTheme::icon( ICONSEARCH ) );

    // TODO active userTableView on currentUser
}

tkUserManagerPrivate::~tkUserManagerPrivate()
{
    if (tkGlobal::isDebugCompilation())
        qWarning() << "~tkUserManagerPrivate";
}

void tkUserManagerPrivate::analyseCurrentUserRights()
{
    // retreive user manager rights from model
    tkUserModel *m = tkUserModel::instance();
    User::UserRights r ( m->data( m->index( m->currentUserIndex().row(), User::UserManagerRights ) ).toInt() );
    // translate to bools
    m_CanModify = ( r & User::WriteAll );
    m_CanCreate = ( r & User::Create );
    m_CanViewAllUsers = ( r & User::ReadAll );
    m_CanDelete = ( r & User::Delete );

    // manage ui <-> rights
    createNewUserAct->setEnabled( m_CanCreate );
    modifyUserAct->setEnabled( m_CanModify );
    deleteUserAct->setEnabled( m_CanDelete );

    // manage specific creation widgets
    userTableView->setVisible( m_CanViewAllUsers );
    searchLineEdit->setVisible( m_CanViewAllUsers );
}

/** \brief upadet status bar for current user, and refresh memory usage group */
void tkUserManagerPrivate::updateStatusBar()
{
    tkUserModel *m = tkUserModel::instance();
    if ( ! m_PermanentWidget ) {
        m_PermanentWidget = new QWidget(m_Parent);
        QHBoxLayout * l = new QHBoxLayout( m_PermanentWidget );
        l->setMargin(0);
        if ( ! m_PermanentUserName )
            m_PermanentUserName = new QLabel( m_PermanentWidget );
        l->addWidget( m_PermanentUserName );
    }
    memoryUsageLabel->setText( tr( "Database usage : %1,\nMemory usage : %2" )
                                     .arg( m->rowCount())
                                     .arg(m->numberOfUsersInMemory()) );
    m_PermanentUserName->setText( m->index( m->currentUserIndex().row(), User::UserName ).data().toString() );
    m_Parent->statusBar()->addPermanentWidget(m_PermanentWidget);
}

/** \brief Change the search method for the users's model */
void tkUserManagerPrivate::on_m_SearchToolButton_triggered( QAction * act )
{
    if ( act == searchByNameAct )
        m_SearchBy= User::UserName;
    else if (act == searchBySurnameAct)
        m_SearchBy= User::UserSurname;
    else if (act == searchByNameAndSurnameAct)
        m_SearchBy= -1;
    else if (act == searchByCityAct)
        m_SearchBy= User::UserCity;
}

/**
  \brief Update the users' model filter
  \todo Manage error when user select an action in the toolbutton
  \todo where can only be calculated by model
 */
void tkUserManagerPrivate::on_searchLineEdit_textchanged()
{
    QHash<int, QString> where;
    where.insert( m_SearchBy, QString( "LIKE '%1%'" ).arg( searchLineEdit->searchText() ) );
    tkUserModel::instance()->setFilter( where );
}

/** \brief Create a new user using tkUserWizard. */
void tkUserManagerPrivate::on_createNewUserAct_triggered()
{
    int createdRow = userTableView->model()->rowCount();
    if ( ! userTableView->model()->insertRows( createdRow, 1 ) ) {
        tkLog::addError( this, "Cannot create new user : can not add row to model");
        return;
    }
    QModelIndex index = userTableView->model()->index( createdRow, USER_NAME );
//    userTableView->model()->setData( index, tr( "New User" ) );
    tkUserWizard wiz(m_Parent);
    wiz.setModelRow( createdRow );
    int r = wiz.exec();
    if ( r == QDialog::Rejected ) {
        if ( ! userTableView->model()->removeRows( createdRow, 1 ) ) {
            tkLog::addError( this, "Cannot delete new user : can not delete row to model");
            return;
        } else
            m_Parent->statusBar()->showMessage( tr( "No user created" ), 2000 );
    } else {
        userTableView->selectRow( createdRow );
        on_userTableView_activated( index );
        m_Parent->statusBar()->showMessage( tr( "User created" ), 2000 );
    }
    qApp->setActiveWindow( m_Parent );
    m_Parent->activateWindow();
}

void tkUserManagerPrivate::on_clearModificationsAct_triggered()
{
    if (tkUserModel::instance()->revertAll())
        m_Parent->statusBar()->showMessage( tr( "Modifications cleared" ), 2000 );
    else
        m_Parent->statusBar()->showMessage( tr( "Can not clear modifications" ), 2000 );
}

void tkUserManagerPrivate::on_saveAct_triggered()
{
    if ( ( ! m_CanModify ) || ( ! m_CanCreate ) )
        return;
    // save changes to database
    if ( tkUserModel::instance()->submitAll() )
        m_Parent->statusBar()->showMessage( tr( "User saved" ), 2000 );
}

void tkUserManagerPrivate::on_deleteUserAct_triggered()
{
    if ( tkUserModel::instance()->removeRow( userTableView->currentIndex().row() ) )
        m_Parent->statusBar()->showMessage( tr( "User deleted" ), 2000 );
    else
        m_Parent->statusBar()->showMessage( tr( "User can not be deleted" ), 2000 );
}

void tkUserManagerPrivate::on_userTableView_activated( const QModelIndex & index )
{
    userViewer->changeUserTo( index.row() );
}

void  tkUserManagerPrivate::selectUserTableView( int row )
{
    userTableView->selectRow( row );
}

/** \brief Assume retranslation of ui. */
void tkUserManagerPrivate::changeEvent(QEvent *e)
{
    if ( (e->type() == QEvent::LanguageChange) ) {
        retranslateUi(m_Parent);
        m_Parent->setWindowTitle( tr( "User Manager" ) + " - " + qApp->applicationName() );
    }
    retranslate();
}

void tkUserManagerPrivate::retranslate()
{
    searchByNameAct->setText( tr( "Search user by name" ) );
    searchBySurnameAct->setText( tr( "Search user by surname" ) );
    searchByNameAndSurnameAct->setText( tr( "Search user by name and surname" ) );
    searchByCityAct->setText( tr( "Search user by city" ) );
    searchByNameAct->setToolTip( tr( "Search user by name" ) );
    searchBySurnameAct->setToolTip( tr( "Search user by surname" ) );
    searchByNameAndSurnameAct->setToolTip( tr( "Search user by name and surname" ) );
    searchByCityAct->setToolTip( tr( "Search user by city" ) );
    m_SearchToolBut->setToolTip( tr( "Search a user" ) );
    m_Parent->setWindowTitle( tr( "User Manager" ) + " - " + qApp->applicationName() );
    updateStatusBar();
}

void tkUserManagerPrivate::showUserDebugDialog( const QModelIndex &id )
{
    QStringList list;
    list << tkUserModel::instance()->index(id.row(), User::UserWarnText).data(Qt::DisplayRole).toStringList();
    tkGlobal::quickDebugDialog( list );
}
