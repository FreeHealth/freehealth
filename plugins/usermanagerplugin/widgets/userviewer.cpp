/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class UserPlugin::UserViewer
  \brief This class is a data wrapper for users.
  When you instanciate this widget, it retreive and show the
  UserPlugin::UserModel::currentUserIndex().\n
  Rights are managed via the UserPlugin::UserModel.\n
  Changes are automaticaly saved into the UserPlugin::UserModel.\n
  You can add pages to the viewer using the virtual class UserPlugin::IUserViewerPage. \n
  \todo limit memory usage.
*/

#include "userviewer.h"
#include "iuserviewerpage.h"
#include "defautuserviewerpages.h"
#include "usermodel.h"

#include <coreplugin/dialogs/pagewidget.h>
#include <extensionsystem/pluginmanager.h>

#include <utils/global.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include "ui_userviewer_identity.h"
#include "ui_userviewer_papers.h"
#include "ui_userviewer_medicalrights.h"
#include "ui_userviewer_professional.h"

#include <QDebug>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

namespace UserPlugin {
namespace Internal {
class UserViewerPrivate
{
public:
    bool canReadRow(int row)
    {
        bool canRead = false;
        int currentUserRow = m_Model->currentUserIndex().row();
        if (currentUserRow == row) {
            // showing currentuser
            Core::IUser::UserRights r = Core::IUser::UserRights(m_Model->currentUserData(Core::IUser::ManagerRights).toInt());
            canRead = (r ^ Core::IUser::ReadOwn);
        } else {
            // not showing currentuser
            Core::IUser::UserRights r = Core::IUser::UserRights(m_Model->currentUserData(Core::IUser::ManagerRights).toInt());
            canRead = (r & Core::IUser::ReadAll);
        }
        return canRead;
    }

public:
    UserModel *m_Model;
    Core::PageWidget *m_Widget;
    QList<IUserViewerPage*> m_pages;
    int m_CurrentRow;
    bool m_CanRead;
};
}  // End Internal
}  // End UserPlugin


UserViewer::UserViewer(QWidget *parent) :
     QWidget(parent),
    d(new UserViewerPrivate)
{
    setObjectName("UserViewer");
    d->m_Model = UserModel::instance(); //new UserModel(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);
    d->m_Widget = new Core::PageWidget(this);
    layout->addWidget(d->m_Widget);

    d->m_pages << new Internal::DefaultUserIdentityPage(this);
    d->m_pages << new Internal::DefaultUserContactPage(this);
    d->m_pages << new Internal::DefaultUserRightsPage(this);
    d->m_pages << new Internal::DefaultUserProfessionalPage(this);

    d->m_pages << new Internal::DefaultUserPapersPage(DefaultUserPapersPage::GenericPaper, this);
    d->m_pages << new Internal::DefaultUserPapersPage(DefaultUserPapersPage::AdministrativePaper, this);
    d->m_pages << new Internal::DefaultUserPapersPage(DefaultUserPapersPage::PrescriptionPaper, this);

    d->m_pages << pluginManager()->getObjects<IUserViewerPage>();

    d->m_Widget->setPages<IUserViewerPage>(d->m_pages);
    d->m_Widget->setSettingKey("UserViewer/Pages");
    d->m_Widget->setupUi(false);

    d->m_Widget->expandAllCategories();

    d->m_Widget->setVisible(d->canReadRow(d->m_Model->currentUserIndex().row()));

    for(int i = 0; i < d->m_Widget->pageWidgets().count(); ++i) {
        IUserViewerWidget *w = qobject_cast<IUserViewerWidget *>(d->m_Widget->pageWidgets().at(i));
        if (w) {
            w->setUserModel(d->m_Model);;
            w->setUserIndex(d->m_Model->currentUserIndex().row());
        }
    }

    connect(pluginManager(), SIGNAL(objectAdded(QObject*)), this, SLOT(pluginManagerObjectAdded(QObject*)));
    connect(pluginManager(), SIGNAL(aboutToRemoveObject(QObject*)), this, SLOT(pluginManagerObjectRemoved(QObject*)));
}

UserViewer::~UserViewer()
{
    if (d)
        delete d;
    d = 0;
}

/** \brief Change current viewing user to \e modelRow from UserModel */
void UserViewer::changeUserTo(const int modelRow)
{
    // manage row changing
    if (d->canReadRow(modelRow)) {
        d->m_CurrentRow = modelRow;
        for(int i = 0; i < d->m_Widget->pageWidgets().count(); ++i) {
            IUserViewerWidget *w = qobject_cast<IUserViewerWidget *>(d->m_Widget->pageWidgets().at(i));
            if (w)
                w->setUserIndex(modelRow);
        }
    } else {
        Utils::informativeMessageBox(tr("You can not access to these datas."), tr("You don't have these rights."), "");
    }
}

void UserViewer::pluginManagerObjectAdded(QObject *o)
{
    IUserViewerPage *page = qobject_cast<IUserViewerPage *>(o);
    if (page) {
        d->m_pages << page;
        d->m_Widget->setPages<IUserViewerPage>(d->m_pages);
        d->m_Widget->setupUi(false);

        for(int i = 0; i < d->m_Widget->pageWidgets().count(); ++i) {
            IUserViewerWidget *w = qobject_cast<IUserViewerWidget *>(d->m_Widget->pageWidgets().at(i));
            if (w) {
                w->setUserModel(d->m_Model);;
                w->setUserIndex(d->m_Model->currentUserIndex().row());
            }
        }
        d->m_Widget->expandAllCategories();
    }
}

void UserViewer::pluginManagerObjectRemoved(QObject *o)
{
    IUserViewerPage *page = qobject_cast<IUserViewerPage *>(o);
    if (page) {
        if (d->m_pages.contains(page)) {
            d->m_pages.removeAll(page);
            d->m_Widget->setPages<IUserViewerPage>(d->m_pages);
            d->m_Widget->setupUi(false);
        }
        d->m_Widget->expandAllCategories();
    }
}
