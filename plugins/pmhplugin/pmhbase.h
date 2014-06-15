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
#ifndef PMHBASE_H
#define PMHBASE_H

#include <utils/database.h>

/**
 * \file pmhbase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.6
 * \date 28 June 2012
*/

namespace Category {
class CategoryItem;
}

namespace PMH {
namespace Internal {
class PmhPlugin;
class PmhData;
class PmhCategory;
class PmhEpisodeData;

class PmhBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class PMH::Internal::PmhPlugin;

protected:
    PmhBase(QObject *parent = 0);

public:
    // Constructor
    static PmhBase *instance();
    virtual ~PmhBase();

    // initialize
    bool initialize();

    // Database getters
    QVector<PmhData *> getPmh(const QString &patientUid = QString::null) const;
    QVector<Category::CategoryItem *> getPmhCategory(const QString &uid) const;
    QList<Category::CategoryItem *> createCategoryTree(const QVector<Category::CategoryItem *> &cats) const;
    bool linkPmhWithCategory(const QVector<Category::CategoryItem *> &cats, const QVector<PmhData *> &pmhs) const;

    // Database setters
    bool savePmhData(PmhData *pmh);
    bool updatePmhData(PmhData *pmh);
    bool savePmhEpisodeData(PmhEpisodeData *episode);
    bool updatePmhEpsisodeData(PmhEpisodeData *episode);
    bool savePmhCategory(Category::CategoryItem *category);

private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                          const QString &pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString &login, const QString &pass,
                          const int port,
                          CreationOption createOption
                         );

private Q_SLOTS:
    void onCoreDatabaseServerChanged();
    void onCoreFirstRunCreationRequested();

private:
    bool m_initialized;
    static PmhBase *m_Instance;
};

}  // End namespace Internal
}  // End namespace PMH


#endif // PMHBASE_H
