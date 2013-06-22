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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_EDCRBASE_H
#define EDRC_EDCRBASE_H

#include <utils/database.h>

namespace eDRC {
class EdrcCore;
namespace Internal {
class ConsultResultCriteria;

class DrcDatabase : public Utils::Database
{
    friend class eDRC::EdrcCore;

protected:
    DrcDatabase();
    bool initialize(bool createIfNotExists);

public:
    ~DrcDatabase();
    bool isInitialized() const {return _initialized;}

    QHash<int, QString> getRcClasses() const;
    QHash<int, QString> getRcForClasses(int classId) const;
    QHash<int, QString> getSeeAlsoRcForRc(int rcId) const;
    QString getRcLabel(const int rcId, bool onlyValid = true) const;
    QString getRcArguments(const int rcId, bool toHtml = false, bool onlyValid = true) const;
    QStringList getRcAuthprizedDiagnosis(const int rcId, bool onlyValid = true) const;
    QStringList getRcIcd10RelatedCodes(const int rcId, bool onlyValid) const;

    QList<ConsultResultCriteria> getOrderedCriteriasForCR(int crId) const;

private:
    bool createDatabase(const QString &connection, const QString &prefixedDbName,
                        const Utils::DatabaseConnector &connector,
                        CreationOption createOption);

protected:
    bool setVersion(const QString &version);
    QString version() const;
    bool checkDatabaseVersion() const;

public:
//    void toTreeWidget(QTreeWidget *tree) const;

private Q_SLOTS:
//    void onCoreDatabaseServerChanged();
//    void onCoreFirstRunCreationRequested();
private:
    bool _initialized;
};

} // namespace Internal
} // namespace eDRC

#endif  // EDRC_EDCRBASE_H
