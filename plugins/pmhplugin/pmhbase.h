/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef PMHBASE_H
#define PMHBASE_H

#include <utils/database.h>

namespace PMH {
namespace Internal {
class PmhBasePrivate;
class PmhData;
class PmhCategory;
class PmhEpisodeData;

class PmhBase : public QObject, public Utils::Database
{
    Q_OBJECT

protected:
    PmhBase(QObject *parent = 0);

public:
    // Constructor
    static PmhBase *instance();
    virtual ~PmhBase();

    // initialize
    bool init();

    // Database getters
    QVector<PmhData *> getPmh(const QString &patientUid) const;
    QVector<PmhCategory *> getPmhCategory(const QString &patientUid) const;
    QList<PmhCategory *> createCategoryTree(const QVector<PmhCategory *> &cats) const;
    bool linkPmhWithCategory(const QVector<PmhCategory *> &cats, const QVector<PmhData *> &pmhs) const;

    // Database setters
    bool savePmhData(PmhData *pmh);
    bool updatePmhData(PmhData *pmh);
    bool savePmhEpisodeData(PmhEpisodeData *episode);
    bool updatePmhEpsisodeData(PmhEpisodeData *episode);

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

private:
    static bool m_initialized;
    static PmhBase *m_Instance;
    Internal::PmhBasePrivate *d;
};

}  // End namespace Internal
}  // End namespace PMH


#endif // PMHBASE_H
