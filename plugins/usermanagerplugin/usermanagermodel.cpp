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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class UserPlugin::Internal::UserManagerModel
 * Is a ultra-basic SQL table model wrapper over the User identity database table.
 * Each user index has a suite of children indexes representing all the
 * UserPlugin::IUserViewerPage sorted by their
 * UserPlugin::IUserViewerPage::sortIndex().
 */

#include "usermanagermodel.h"
#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/constants.h>
#include <usermanagerplugin/database/userbase.h>
#include <usermanagerplugin/widgets/defaultuserviewerpages.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <extensionsystem/pluginmanager.h>

#include <QSqlQueryModel>

#include <QDebug>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}

namespace UserPlugin {
namespace Internal {

/// Private data representation for user data
enum UserDataRepresentation {
    Name = 0,
    SecondName,
    FirstName,
    Uuid,
    Title,
    LastLogin,
    Gender,

    NumberOfColumns
};

class UserManagerModelPrivate
{
public:
    UserManagerModelPrivate(UserManagerModel *parent) :
        _sqlModel(0),
        q(parent)
    {
    }
    
    ~UserManagerModelPrivate()
    {
    }

    // Get and sort available pages
    void getUserViewerPages()
    {
        _pages << new DefaultUserContactPage(q);
        _pages << new DefaultUserRightsPage(q);
        _pages << new DefaultUserProfessionalPage(q);

        _pages << new DefaultUserPapersPage(DefaultUserPapersPage::GenericPaper, q);
        _pages << new DefaultUserPapersPage(DefaultUserPapersPage::AdministrativePaper, q);
        _pages << new DefaultUserPapersPage(DefaultUserPapersPage::PrescriptionPaper, q);

        _pages << pluginManager()->getObjects<IUserViewerPage>();

        qSort(_pages.begin(), _pages.end(), Core::IGenericPage::sortIndexLessThan);
    }

    // Get the SQL command filtered. Manages user rights
    QString getSqlQuery(const UserManagerModelFilter &filter)
    {
        QString sql, f;
        Core::IUser::UserRights rights = Core::IUser::UserRights(userModel()->currentUserData(Core::IUser::ManagerRights).toInt());
        bool canReadAll = rights & Core::IUser::ReadAll;
        bool canReadOwn = rights & Core::IUser::ReadOwn;

        if (canReadAll) {
            Utils::FieldList birthNameConds;
            // Birth/second name
            if (filter._birth.isEmpty()) {
                birthNameConds << Utils::Field(Constants::Table_USERS, Constants::USER_NAME, QString("LIKE '%'"));
                birthNameConds << Utils::Field(Constants::Table_USERS, Constants::USER_SECONDNAME, QString("LIKE '%'"));
            } else {
                birthNameConds << Utils::Field(Constants::Table_USERS, Constants::USER_NAME, QString("LIKE '%1%'").arg(filter._birth));
                birthNameConds << Utils::Field(Constants::Table_USERS, Constants::USER_SECONDNAME, QString("LIKE '%1%'").arg(filter._birth));
            }
            f = userBase()->getWhereClause(birthNameConds, Utils::Database::OR);
            f += " AND ";

            // Firstname
            if (filter._first.isEmpty())
                f += userBase()->fieldName(Constants::Table_USERS, Constants::USER_FIRSTNAME) + QString(" LIKE '%'");
            else
                f += userBase()->fieldName(Constants::Table_USERS, Constants::USER_FIRSTNAME) + QString(" LIKE '%1%'").arg(filter._first);
        } else if (canReadOwn) {
            Utils::FieldList conds;
            conds << Utils::Field(Constants::Table_USERS, Constants::USER_UUID, QString("='%1'").arg(userModel()->currentUserData(Core::IUser::Uuid).toString()));
            f = userBase()->getWhereClause(conds);
        }
        // Add the SELECT
        sql = userBase()->select(Constants::Table_USERS, QList<int>()
                                 << Constants::USER_NAME
                                 << Constants::USER_SECONDNAME
                                 << Constants::USER_FIRSTNAME
                                 << Constants::USER_UUID
                                 << Constants::USER_TITLE
                                 << Constants::USER_LASTLOG
                                 << Constants::USER_GENDER) +
                QString(" WHERE (%1)").arg(f);
        return sql;
    }
    
    // Create sub-items for one user \e currentItem with all IUserViewerPages
    void appendPages(QStandardItem *currentItem)
    {
        QMap<QString, QStandardItem *> categories;
        foreach(IUserViewerPage *page, _pages) {
            QStandardItem *item = new QStandardItem;
            item->setText(QString("<span style=\"color:black\">%1</span>").arg(page->displayName()));
            item->setData(page->id());

            QIcon icon = page->categoryIcon();
            if (!icon.isNull())
                item->setIcon(icon);

            QStringList categoriesId = page->category().split(QLatin1Char('|'));
            QStringList trCategories = page->category().split(QLatin1Char('|'));
            QString currentCategory = categoriesId.at(0);

            // Create main category
            QStandardItem *catItem;
            if (!categories.contains(currentCategory)) {
                catItem = new QStandardItem;
                catItem->setText(QString("<span style=\"color:black\">%1</span>").arg(trCategories.at(0)));
                catItem->setData(page->id());
                categories.insert(currentCategory, catItem);
                currentItem->appendRow(catItem);
            }

            // Create sub-categories
            int catCount = 1;

            while (catCount < categoriesId.count()) {
                if (!categories.contains(currentCategory + QLatin1Char('|') + categoriesId.at(catCount))) {
                    catItem = new QStandardItem;
                    currentCategory +=  QLatin1Char('|') + categoriesId.at(catCount);
                    catItem->setText(trCategories.at(catCount));
                    catItem->setData(page->id());
                    categories.value(currentCategory)->appendRow(catItem);
                } else {
                    currentCategory +=  QLatin1Char('|') + categoriesId.at(catCount);
                }
                ++catCount;
            }
            categories.value(currentCategory)->appendRow(item);
        }
    }

public:
    QList<IUserViewerPage *> _pages;
    QSqlQueryModel *_sqlModel;

private:
    UserManagerModel *q;
};
} // namespace Internal
} // end namespace UserPlugin


/*! Constructor of the UserPlugin::Internal::UserManagerModel class */
UserManagerModel::UserManagerModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new UserManagerModelPrivate(this))
{
    setColumnCount(1);
    d->_sqlModel = new QSqlQueryModel(this);
    //Utils::linkSignalsFromFirstModelToSecondModel(d->_sqlModel, this, false);
}

/*! Destructor of the UserPlugin::Internal::UserManagerModel class */
UserManagerModel::~UserManagerModel()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool UserManagerModel::initialize()
{
    d->getUserViewerPages();
    return true;
}

QVariant UserManagerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.parent().isValid())
        return QStandardItemModel::data(index, role);

    switch (role) {
    case Qt::DisplayRole:
    {
        int i = index.row();
        QModelIndex name = d->_sqlModel->index(i, Name);
        const QString &secondname = d->_sqlModel->index(i, SecondName).data().toString();
        QModelIndex firstname = d->_sqlModel->index(i, FirstName);
        QModelIndex title = d->_sqlModel->index(i, Title);
        QModelIndex lastLogin = d->_sqlModel->index(i, LastLogin);
        QString titleString = Trans::ConstantTranslations::titles().at(d->_sqlModel->data(title).toInt());
        QString fullname;
        if (!secondname.isEmpty())
            fullname = name.data().toString() + " - " + secondname + " " + firstname.data().toString();
        else
            fullname = name.data().toString() + " " + firstname.data().toString();
        QString html = QString("<span style=\"font-weight:bold;color:black\">%1<br />%2</span><br />"
                               "<span style=\"font-size:small;color:gray\">%3</span>")
                .arg(titleString.replace(" ", "&nbsp;"))
                .arg(fullname)
                .arg(tr("Last login: %1").arg(d->_sqlModel->data(lastLogin).toString()));
        return html;
    }
    case Qt::DecorationRole:
        return theme()->defaultGenderPixmap(genderIndex(index)).scaled(QSize(24, 24));
    }
    return QVariant();
}

Qt::ItemFlags UserManagerModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/** Filter the model */
void UserManagerModel::setFilter(const UserManagerModelFilter &filter)
{
    // Clear model and apply filter
    clear();
    d->_sqlModel->setQuery(d->getSqlQuery(filter), userBase()->database());
    if (!d->_sqlModel->query().isActive()) {
        LOG_ERROR("Wrong filtering");
        LOG_QUERY_ERROR(d->_sqlModel->query());
        d->_sqlModel->clear();
        clear();
        return;
    }
    setColumnCount(1);

    // Read all indexes from the sqlmodel and create the StandardItems
    for(int i = 0; i < d->_sqlModel->rowCount(); ++i) {
        QModelIndex name = d->_sqlModel->index(i, 0);
        QModelIndex secondname = d->_sqlModel->index(i, 1);
        QModelIndex firstname = d->_sqlModel->index(i, 2);
        QString fullname = name.data().toString() + " " + secondname.data().toString() + " - " + firstname.data().toString();
        QStandardItem *item = new QStandardItem(fullname.simplified());
        invisibleRootItem()->appendRow(item);
        d->appendPages(item);
    }
}

/** Return all registered UserPlugin::IUserViewerPage found and used in this model */
QList<IUserViewerPage *> UserManagerModel::pages() const
{
    return d->_pages;
}

/** Return the user uuid corresponding to the \e index whatever is the branch deep. */
QString UserManagerModel::userUuid(const QModelIndex &index) const
{
    QModelIndex uid = this->index(index.row(), 0, index.parent());
    while (uid.parent().isValid()) uid = uid.parent();
    uid = d->_sqlModel->index(uid.row(), 3);
    return d->_sqlModel->data(uid).toString();
}

/** Return the gender index of the user \sa Trans::ConstantTranslations::genders() */
int UserManagerModel::genderIndex(const QModelIndex &index) const
{
    QModelIndex sql = d->_sqlModel->index(index.row(), 6);
    const QString &g = d->_sqlModel->data(sql).toString();
    if (g=="M")
        return 0;
    else if (g=="F")
        return 1;
    else if (g=="H")
        return 2;
    return 0;
}

QString UserManagerModel::lastLogin(const QModelIndex &index) const
{
    QModelIndex sql = d->_sqlModel->index(index.row(), 5);
    return d->_sqlModel->data(sql).toString();
}

QString UserManagerModel::title(const QModelIndex &index) const
{
    QModelIndex sql = d->_sqlModel->index(index.row(), 4);
    return Trans::ConstantTranslations::titles().at(d->_sqlModel->data(sql).toInt());
}

/**
 * Return the UserPlugin::IUserViewerPage index in the list corresponding to the \e index.
 * Returns -1 in case of an error.
 * \sa pages()
 */
int UserManagerModel::pageIndexFromIndex(const QModelIndex &index) const
{
    QStandardItem *item = itemFromIndex(index);
    const QString &id = item->data().toString();
    for(int i=0; i < d->_pages.count(); ++i) {
        if (d->_pages.at(i)->id()==id)
            return i;
    }
    return -1;
}

//void UserManagerModel::pluginManagerObjectAdded(QObject *o)
//{
//    IUserViewerPage *page = qobject_cast<IUserViewerPage *>(o);
//    if (page) {
//        d->m_pages << page;
//        d->m_Widget->setPages<IUserViewerPage>(d->m_pages);
//        d->m_Widget->setupUi(false);

//        for(int i = 0; i < d->m_Widget->pageWidgets().count(); ++i) {
//            IUserViewerWidget *w = qobject_cast<IUserViewerWidget *>(d->m_Widget->pageWidgets().at(i));
//            if (w) {
//                w->setUserModel(d->m_Model);
//                w->setUserIndex(d->m_CurrentRow);
//            }
//        }
//        d->m_Widget->expandAllCategories();
//    }
//}

//void UserManagerModel::pluginManagerObjectRemoved(QObject *o)
//{
//    IUserViewerPage *page = qobject_cast<IUserViewerPage *>(o);
//    if (page) {
//        if (d->m_pages.contains(page)) {
//            d->m_pages.removeAll(page);
//            d->m_Widget->setPages<IUserViewerPage>(d->m_pages);
//            d->m_Widget->setupUi(false);
//        }

//        for(int i = 0; i < d->m_Widget->pageWidgets().count(); ++i) {
//            IUserViewerWidget *w = qobject_cast<IUserViewerWidget *>(d->m_Widget->pageWidgets().at(i));
//            if (w) {
//                w->setUserModel(d->m_Model);
//                w->setUserIndex(d->m_CurrentRow);
//            }
//        }
//        d->m_Widget->expandAllCategories();
//    }
//}
