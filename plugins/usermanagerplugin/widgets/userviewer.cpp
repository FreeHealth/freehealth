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
#include <coreplugin/itheme.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>

#include <QDebug>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}

namespace {
const char * const CSS = "text-indent:5px;padding:5px;font-weight:bold;background:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.464 rgba(255, 255, 176, 149), stop:1 rgba(255, 255, 255, 0))";
}

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

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////    UserViewerPrivate    /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class UserViewerPrivate
{
public:
    UserViewerPrivate(UserViewer *parent) :
        m_userModel(0),
        m_stackedWidgets(0),
        m_Listener(0),
        m_CurrentRow(-1),
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
        for(int i = 0; i < m_widgets.count(); ++i)
            m_widgets.at(i)->setUserIndex(row);
    }

    void populateStackedWidget()
    {
        foreach(IUserViewerPage *page, m_userManagerModel->pages()) {
            QWidget *container = new QWidget(q);
            QVBoxLayout *lay = new QVBoxLayout(container);
            lay->setMargin(0);
            container->setLayout(lay);

            // add title and line
            QFont bold;
            bold.setBold(true);
            bold.setPointSize(bold.pointSize()+1);
            QLabel *title = new QLabel(container);
            title->setFont(bold);
            title->setWordWrap(true);
            title->setText(page->title());
            title->setStyleSheet(::CSS);
            title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            lay->addWidget(title);

            QFrame *line = new QFrame(container);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            lay->addWidget(line);

            // add a scrollarea with the widget's page to add
            QWidget *pageWidget = page->createPage(container);
            IUserViewerWidget *w = qobject_cast<IUserViewerWidget*>(pageWidget);
            Q_ASSERT(w);
            m_widgets << w;
            pageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            if (pageWidget->layout())
                pageWidget->layout()->setMargin(0);
            QScrollArea *scroll = new QScrollArea(container);
            scroll->setWidget(pageWidget);
            scroll->setWidgetResizable(true);
            scroll->setFrameShape(QFrame::NoFrame);
            lay->addWidget(scroll);

            m_stackedWidgets->addWidget(container);
        }
    }

public:
    UserModel *m_userModel;
    QStackedWidget *m_stackedWidgets;
    QList<IUserViewerWidget *> m_widgets;
    UserViewerModelCoreListener *m_Listener;
    int m_CurrentRow;
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
//    qWarning() << "UserViewer::setCurrentUser" << userUid;
    if (d->m_currentUserUuid == userUid)
        return;
    d->m_currentUserUuid = userUid;
    QHash<int, QString> where;
    where.insert(Core::IUser::Uuid, QString("='%1'").arg(userUid));
    d->m_userModel->setFilter(where);
    d->setUserModelRow(0);
    d->m_CurrentRow = 0;
}

/** Set the current IUserViewerPage to present in the viewer */
void UserViewer::setCurrentPage(int index)
{
//    qWarning() << "UserViewer::setCurrentPage"<<index;
    if (index == -1 || index >= d->m_stackedWidgets->count())
        d->m_stackedWidgets->setCurrentIndex(0);
    d->m_stackedWidgets->setCurrentIndex(index);
}

/**
 * Submit all UserPlugin::IUserViewerWidget modifications to the internal UserPlugin::UserModel
 * \sa UserPlugin::IUserViewerWidget::submit()
 */
void UserViewer::submitChangesToModel()
{
    WARN_FUNC;
    for(int i = 0; i < d->m_widgets.count(); ++i) {
        IUserViewerWidget *w = d->m_widgets.at(i);
        if (w && !w->submit())
            LOG_ERROR(w->objectName() + " submission error: " + w->parentUserViewerPageId());
    }
    d->m_userModel->submitAll();
    userModel()->forceReset();
}
