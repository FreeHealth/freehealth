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
 * \class UserPlugin::UserViewer
 * \brief This class is a data widget wrapper for users.
 * This mapper has its own UserPlugin::UserModel independent from the
 * Core::IUser and filter it on the user uuid. Data are sent to this
 * model and all usermodel must be updated after a submition of data.
 */

#include "userviewer.h"
#include <usermanagerplugin/widgets/iuserviewerpage.h>
#include <usermanagerplugin/widgets/defaultuserviewerpages.h>
#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/usermanagermodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QDebug>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}

// TODO
// Get pages from the model
//

namespace UserPlugin {
namespace Internal {

UserViewerModelCoreListener::UserViewerModelCoreListener(UserPlugin::UserViewer *parent) :
        Core::ICoreListener(parent)
{
    Q_ASSERT(parent);
    _viewer = parent;
}
UserViewerModelCoreListener::~UserViewerModelCoreListener() {}

bool UserViewerModelCoreListener::coreAboutToClose()
{
    qWarning() << Q_FUNC_INFO;
    return true;
}

class UserViewerPrivate
{
public:
    UserViewerPrivate(UserViewer *parent) :
        m_userModel(0),
        m_stackedWidgets(0),
        m_Listener(0),
        m_CurrentRow(-1),
        m_identityPageIndex(-1),
        m_CanRead(false),
        m_userManagerModel(0),
        q(parent)
    {}

    bool canReadRow(int row)
    {
        bool canRead = false;
        int currentUserRow = m_userModel->currentUserIndex().row();
        if (currentUserRow == row) {
            // showing currentuser
            Core::IUser::UserRights r = Core::IUser::UserRights(m_userModel->currentUserData(Core::IUser::ManagerRights).toInt());
            canRead = (r ^ Core::IUser::ReadOwn);
        } else {
            // not showing currentuser
            Core::IUser::UserRights r = Core::IUser::UserRights(m_userModel->currentUserData(Core::IUser::ManagerRights).toInt());
            canRead = (r & Core::IUser::ReadAll);
        }
        return canRead;
    }

    // We must create the stack BEFORE
    void setUserModel(UserModel *model)
    {
        for(int i = 0; i < m_widgets.count(); ++i) {
            IUserViewerWidget *w = m_widgets.at(i);
            w->setUserModel(model);
        }
    }

    void setUserModelRow(int row)
    {
        for(int i = 0; i < m_widgets.count(); ++i) {
            IUserViewerWidget *w = m_widgets.at(i);
            w->setUserIndex(row);
        }
    }

    void populateStackedWidget()
    {
        foreach(IUserViewerPage *page, m_userManagerModel->pages()) {
            // add a scrollarea with the widget's page to add
            QWidget *pageWidget = page->createPage(m_stackedWidgets);
            IUserViewerWidget *w = qobject_cast<IUserViewerWidget*>(pageWidget);
            Q_ASSERT(w);
            m_widgets << w;
            pageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            if (pageWidget->layout())
                pageWidget->layout()->setMargin(0);
            QScrollArea *scroll = new QScrollArea(q);
            scroll->setWidget(pageWidget);
            scroll->setWidgetResizable(true);
            scroll->setFrameShape(QFrame::NoFrame);
            m_stackedWidgets->addWidget(scroll);
        }
    }

public:
    UserModel *m_userModel;
    QStackedWidget *m_stackedWidgets;
    QList<IUserViewerWidget *> m_widgets;
    UserViewerModelCoreListener *m_Listener;
    int m_CurrentRow, m_identityPageIndex;
    bool m_CanRead;
    UserManagerModel *m_userManagerModel;
    QString m_currentUserUuid;

private:
    UserViewer *q;
};
}  // End Internal
}  // End UserPlugin

UserViewer::UserViewer(QWidget *parent) :
    QWidget(parent),
    d(new UserViewerPrivate(this))
{
    setObjectName("UserViewer");
    d->m_Listener = new UserViewerModelCoreListener(this);
    pluginManager()->addObject(d->m_Listener);

    // Manage user model && stackedwidget
    d->m_userModel = new UserModel(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);
    d->m_stackedWidgets = new QStackedWidget(this);
    layout->addWidget(d->m_stackedWidgets);
}

/** Dtor */
UserViewer::~UserViewer()
{
    pluginManager()->removeObject(d->m_Listener);
    if (d)
        delete d;
    d = 0;
}

/** Define the UserPlugin::Internal::UserManagerModel to use in the mapper */
bool UserViewer::initialize(Internal::UserManagerModel *model)
{
    d->m_userManagerModel = model;
    d->m_userModel->initialize();
    d->populateStackedWidget();
    d->setUserModel(d->m_userModel);
    return true;
}

/** \brief Change current viewing user to \e modelRow from UserModel */
void UserViewer::setCurrentUser(const QString &userUid)
{
    qWarning() << "UserViewer::setCurrentUser" << userUid;
    if (d->m_currentUserUuid == userUid)
        return;
    d->m_currentUserUuid = userUid;
    QHash<int, QString> where;
    where.insert(Core::IUser::Uuid, QString("='%1'").arg(userUid));
    d->m_userModel->setFilter(where);
//    if (d->canReadRow(modelRow)) {
        d->m_CurrentRow = 0;
        d->setUserModelRow(0);
//    } else {
//        Utils::informativeMessageBox(tr("You can not access to these data."), tr("You don't have access rights."), "");
//    }
}

/** Set the current IUserViewerPage to present in the viewer */
void UserViewer::setCurrentPage(int index)
{
    qWarning() << "UserViewer::setCurrentPage"<<index;
    if (index == -1)
        d->m_stackedWidgets->setCurrentIndex(0);
    d->m_stackedWidgets->setCurrentIndex(index);
}

void UserViewer::submitChangesToModel()
{
    for(int i = 0; i < d->m_widgets.count(); ++i) {
        IUserViewerWidget *w = d->m_widgets.at(i);
        if (w) {
            if (!w->submit())
                LOG_ERROR(w->objectName() + " submition error");
        }
    }
}
