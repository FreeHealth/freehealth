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
 * \class UserPlugin::UserManager
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

#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/usermanagermodel.h>
#include <usermanagerplugin/userdata.h>
#include <usermanagerplugin/widgets/userviewer.h>
#include <usermanagerplugin/constants.h>
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
#include <QPainter>

#include "ui_usermanagerwidget.h"
#include "ui_userviewer_treedelegate.h"

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ContextManager *contextManager() {return  Core::ICore::instance()->contextManager();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
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

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////    UserTreeDelegateWidget    //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserTreeDelegateWidget::UserTreeDelegateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserViewerTreeDelegateWidget)
{
    ui->setupUi(this);
}

UserTreeDelegateWidget::~UserTreeDelegateWidget()
{
    delete ui;
}

void UserTreeDelegateWidget::setTitle(const QString &title)
{
    ui->line1->setText(title);
}

void UserTreeDelegateWidget::setFullName(const QString &fullName)
{
    ui->line2->setText(fullName);
}

void UserTreeDelegateWidget::setGenderPhoto(const QPixmap &pix)
{
    ui->photoLabel->setPixmap(pix.scaled(QSize(24, 24)));
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////    UserTreeDelegate    /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserTreeDelegate::UserTreeDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    _itemWidget(new UserTreeDelegateWidget)
{
}

UserTreeDelegate::~UserTreeDelegate()
{
    delete _itemWidget;
}

void UserTreeDelegate::setUserManagerModel(UserManagerModel *model)
{
    _model = model;
}

void UserTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.parent().isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    _itemWidget->resize(option.rect.size());

    // Update data of widget here.
    const QString &s = index.data().toString();
    _itemWidget->setTitle(_model->title(index));
    _itemWidget->setFullName(s);
    _itemWidget->setGenderPhoto(theme()->defaultGenderPixmap(_model->genderIndex(index), Core::ITheme::BigIcon ));

    // Change the background color of the widget if it is selected.
    QPalette pal;
    if ((option.state & QStyle::State_Selected) == QStyle::State_Selected) {
        pal.setBrush(QPalette::Window, QBrush(QColor(Qt::lightGray)));
    } else {
        pal.setBrush(QPalette::Window, QBrush(QColor(Qt::transparent)));
    }
    _itemWidget->setPalette(pal);

    // Paint the widget now.
    painter->save();
    painter->translate(option.rect.topLeft());
    _itemWidget->render(painter);
    painter->restore();
}

QSize UserTreeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const bool topLevel = !index.parent().isValid();
    if (topLevel)
        return QSize(125, 42);
    return QStyledItemDelegate::sizeHint(option, index);
}

}  // End  Internal
}  // End UserPlugin

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////   UserManager   ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserManager::UserManager(QWidget * parent) :
    QMainWindow(parent)
{
    Q_ASSERT_X(userModel()->hasCurrentUser(), "UserManager", "NO CURRENT USER");
    if (!userModel()->hasCurrentUser())
        return;
    setAttribute(Qt::WA_DeleteOnClose);
    m_Widget = new UserManagerWidget(this);
    setCentralWidget(m_Widget);
    setUnifiedTitleAndToolBarOnMac(true);
}

/** Destructor */
UserManager::~UserManager()
{
}

/** Initialize the mainwindow */
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

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   UserManagerDialog   /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserManagerDialog::UserManagerDialog(QWidget *parent) :
    QDialog(parent)
{
    Q_ASSERT_X(userModel()->hasCurrentUser(), "UserManagerDialog", "NO CURRENT USER");
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

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   UserManagerWidget   /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
namespace UserPlugin {
namespace Internal {
class UserManagerWidgetPrivate {
public:
    UserManagerWidgetPrivate(UserManagerWidget *parent) :
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

        m_SearchBy = Core::IUser::Name;
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
        QObject::connect(ui->userTreeView, SIGNAL(clicked(QModelIndex)), q, SLOT(onUserClicked(const QModelIndex &)));

        // connect search line edit
        QObject::connect(ui->searchLineEdit, SIGNAL(textChanged(const QString &)), q, SLOT(onSearchRequested()));
        QObject::connect(m_SearchToolButton, SIGNAL(triggered(QAction*)), q, SLOT(onSearchToolButtonTriggered(QAction*)));
    }

    // Check the current user rights and adapt the view to them.
    void analyzeCurrentUserRights()
    {
        // retreive user manager rights from model
        UserModel *m = userModel();
        Core::IUser::UserRights r (m->currentUserData(Core::IUser::ManagerRights).toInt());
        // translate to bools
        m_CanModify = (r & Core::IUser::WriteAll);
        m_CanCreate = (r & Core::IUser::Create);
        m_CanViewAllUsers = (r & Core::IUser::ReadAll);
        m_CanDelete = (r & Core::IUser::Delete);

        updateButtons();

        // manage specific creation widgets
        ui->userTreeView->setVisible(m_CanViewAllUsers);
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
    bool m_CanModify, m_CanCreate, m_CanViewAllUsers, m_CanViewRestrictedData, m_CanDelete;
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
//    UserModel *model = userModel();
    d->m_model = new Internal::UserManagerModel(this);
    d->m_model->initialize();
    d->m_model->setFilter(Internal::UserManagerModelFilter());

    d->ui->userTreeView->setModel(d->m_model);
    d->ui->userTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->ui->userTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->ui->userTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    d->ui->userViewer->initialize(d->m_model);

    // Add delegate
//    UserTreeDelegate *delegate = new UserTreeDelegate(this);
//    delegate->setUserManagerModel(d->m_model);
    Utils::HtmlDelegate *delegate = new Utils::HtmlDelegate(this);
    d->ui->userTreeView->setItemDelegate(delegate);
    d->ui->userTreeView->setIndentation(10);
    d->ui->userTreeView->setAnimated(true);
    d->ui->userTreeView->setUniformRowHeights(false);

    d->connectUiAndActions();
    connect(user(), SIGNAL(userChanged()), this, SLOT(onCurrentUserChanged()));
    d->analyzeCurrentUserRights();
    retranslate();
    int width = size().width();
    d->ui->splitter->setSizes(QList<int>() << width/4 << width*3/4);
    return true;
}

/**
 * Checks the content of the view, verify unsaved data.
 * Return \e true if the view can be closed, otherwise return false
 */
bool UserManagerWidget::canCloseParent()
{
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
        d->m_SearchBy= Core::IUser::Name;
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
    d->ui->userTreeView->setCurrentIndex(d->ui->userTreeView->model()->index(row, Core::IUser::Name));
//    ui->userTreeView->selectRow(row);
    d->analyzeCurrentUserRights();
//    ui->userViewer->setCurrentUser(row);
}

/** Updates the users' model filter */
void UserManagerWidget::onSearchRequested()
{
    // TODO: Manage error when user select an action in the toolbutton
    // TODO: where can only be calculated by model
    QHash<int, QString> where;
    where.insert(d->m_SearchBy, QString("LIKE '%1%'").arg(d->ui->searchLineEdit->text()));
    userModel()->setFilter(where);
}

/** Creates a new user using UserPlugin::UserWizard. */
void UserManagerWidget::onCreateUserRequested()
{
    int createdRow = d->ui->userTreeView->model()->rowCount();
    if (!d->ui->userTreeView->model()->insertRows(createdRow, 1)) {
        LOG_ERROR("Error creating new user: cannot add row to model");
        return;
    }
    QModelIndex index = d->ui->userTreeView->model()->index(createdRow, USER_NAME);
    UserWizard wiz(this);
    // TODO: code here
//    wiz.setModelRow(createdRow);
    int r = wiz.exec();
    if (r == QDialog::Rejected) {
        if (!d->ui->userTreeView->model()->removeRows(createdRow, 1)) {
            LOG_ERROR("Error deleting new user: cannot delete row from model");
            return;
        }
    } else {
//        d->ui->userTreeView->selectRow(createdRow);
        onUserClicked(index);
    }
}

void UserManagerWidget::onClearModificationRequested()
{
//    if (userModel()->revertAll())
//        m_Parent->statusBar()->showMessage(tr("Modifications cleared"), 2000);
//    else
//        m_Parent->statusBar()->showMessage(tr("Can not clear modifications"), 2000);
}

/**
 * \internal
 * Save the model.
 */
void UserManagerWidget::onSaveRequested()
{
    if ((!d->m_CanModify) || (!d->m_CanCreate))
        return;
    d->m_ToolBar->setFocus();

    // tell all pages to submit data to the model
    d->ui->userViewer->submitChangesToModel();

    // submit user to database
    QString uuid = d->ui->userTreeView->model()->index(d->ui->userTreeView->currentIndex().row(), Core::IUser::Uuid).data().toString();
    if (!userModel()->submitUser(uuid)) {
        LOG_ERROR("Unable to save user " + uuid);
    }
}

/**
 * \internal
 * Slot connected to the "delete user" action
 */
void UserManagerWidget::onDeleteUserRequested()
{
    if (!d->ui->userTreeView->selectionModel()->hasSelection())
        return;

    // User can not delete himself
    if (d->ui->userTreeView->currentIndex().row() == userModel()->currentUserIndex().row())
        return;

    if (userModel()->removeRow(d->ui->userTreeView->currentIndex().row())) {
        LOG(tr("User deleted"));
    } else {
        LOG(tr("User can not be deleted"));
    }
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

void UserManagerWidget::onUserClicked(const QModelIndex &index)
{
    qWarning() << "UserManagerWidget::onUserClicked" << index;
    d->ui->userViewer->setCurrentUser(d->m_model->userUuid(index));
    d->ui->userViewer->setCurrentPage(d->m_model->pageIndexFromIndex(index));
    d->ui->userViewer->setEnabled(true);
    d->updateButtons();
}

void UserManagerWidget::selectuserTreeView(int row)
{
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
    QStringList list;
    list << userModel()->index(id.row(), Core::IUser::WarnText).data(Qt::DisplayRole).toStringList();
    Utils::quickDebugDialog(list);
}
