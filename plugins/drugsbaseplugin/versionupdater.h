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
#ifndef VERSIONUPDATER_H
#define VERSIONUPDATER_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QStringList>

/**
 * \file versionupdater.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 06 Mar 2010
*/

namespace DrugsDB {
class DrugBaseCore;
class DrugsModel;

class GenericUpdateStep
{
public:
    GenericUpdateStep() {}
    virtual ~GenericUpdateStep() {}

    virtual QString fromVersion() const = 0;
    virtual QString toVersion() const = 0;
};

class DosageDatabaseUpdateStep : public GenericUpdateStep
{
public:
    DosageDatabaseUpdateStep() {}
    virtual ~DosageDatabaseUpdateStep() {}

    virtual QString userMessage() const = 0;

    virtual void setConnectionName(const QString &name) = 0;

    virtual bool retreiveValuesToUpdate() const = 0;
    virtual bool updateDatabaseScheme() const = 0;
    virtual bool saveUpdatedValuesToDatabase() const = 0;
};

class DrugsIOUpdateStep : public GenericUpdateStep
{
public:
    DrugsIOUpdateStep() {}
    virtual ~DrugsIOUpdateStep() {}

    virtual bool updateFromXml() const = 0;
    virtual bool executeXmlUpdate(QString &xml) const = 0;

    virtual bool updateFromModel() const = 0;
    virtual bool executeUpdate(DrugsDB::DrugsModel *model, QList<int> rows) const = 0;
};


class VersionUpdaterPrivate;
class DRUGSBASE_EXPORT VersionUpdater
{
    friend class DrugsDB::DrugBaseCore;

public:
    ~VersionUpdater();

    bool isDosageDatabaseUpToDate() const;
    bool updateDosageDatabase();
    QString lastDosageDabaseVersion() const;
    QString lastXmlIOVersion() const;

    bool isXmlIOUpToDate(const QString &xmlContent) const;
    QString xmlVersion(const QString &xmlContent) const;
    QString updateXmlIOContent(const QString &xmlcontent);
    bool updateXmlIOModel(const QString &fromVersion, DrugsDB::DrugsModel *model, const QList<int> &rowsToUpdate);

protected:
    VersionUpdater();

private:
    VersionUpdaterPrivate *d;
};

}  // end namespace DrugsDB

#endif // VERSIONUPDATER_H
