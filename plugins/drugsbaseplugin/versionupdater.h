/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef VERSIONUPDATER_H
#define VERSIONUPDATER_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QStringList>

/**
 * \file versionupdater.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 06 Mar 2010
*/

namespace DrugsDB {
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
public:
    static VersionUpdater *instance();
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
    static VersionUpdater *m_Instance;
    VersionUpdaterPrivate *d;
};

}  // end namespace DrugsDB

#endif // VERSIONUPDATER_H
