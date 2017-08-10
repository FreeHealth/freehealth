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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CATEGORY_BASE_H
#define CATEGORY_BASE_H

#include <utils/database.h>
#include <QVector>
#include <QList>


namespace Category {
class CategoryCore;
class CategoryItem;
namespace Internal {
class CategoryBasePrivate;

class CategoryBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class Category::CategoryCore;

protected:
    CategoryBase(QObject *parent = 0);

public:
    // Constructor
    static CategoryBase *instance();
    virtual ~CategoryBase();

    // initialize
    bool initialize();
    bool isInitialized() const {return m_initialized;}

private:
    bool createDatabase(const QString &connectionName,
                        const QString &dbName,
                        const QString &pathOrHostName,
                        TypeOfAccess access,
                        AvailableDrivers driver,
                        const QString &login,
                        const QString &pass,
                        const int port,
                        CreationOption createOption);
    bool updateDatabase();

public:
    QVector<CategoryItem *> getCategories(const QString &mime, const QStringList &uuids = QStringList()) const;
    QList<CategoryItem *> createCategoryTree(const QVector<CategoryItem *> &cats) const;

    bool saveCategory(CategoryItem *category);
    bool saveCategories(const QVector<CategoryItem *> &categories, bool createTransaction = true);
    bool saveCategoryLabels(CategoryItem *category);

    bool removeAllExistingCategories(const QString &mime);

private Q_SLOTS:
    void onCoreDatabaseServerChanged();
    void onCoreFirstRunCreationRequested();

private:
    bool categoryNeedsUpdate(CategoryItem *category);
    bool updateCategory(CategoryItem *category);

private:
    bool m_initialized;
    static CategoryBase *m_Instance;
};

}  // End namespace Internal
}  // End namespace Category


#endif // CATEGORY_BASE_H
