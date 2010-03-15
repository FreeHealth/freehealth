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
#include "drugsdatabaseselector.h"
#include "drugsbase.h"
#include "constants.h"

#include <coreplugin/constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QVector>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFont>

using namespace DrugsDB;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

namespace DrugsDB {
namespace Internal {
//class DatabaseInfosPrivate
//{
//public:
//
//};

class DrugsDatabaseSelectorPrivate
{
public:
    DrugsDatabaseSelectorPrivate()
    {}

    ~DrugsDatabaseSelectorPrivate()
    {
        qDeleteAll(m_FileName_Infos);
        m_FileName_Infos.clear();
        m_Current = 0;
    }

public:
    QHash<QString, DatabaseInfos *> m_FileName_Infos;
    DatabaseInfos *m_Current;
};

}  // End namespace Internal
}  // End namespace DrugsDB

DrugsDatabaseSelector *DrugsDatabaseSelector::m_Instance = 0;

DrugsDatabaseSelector *DrugsDatabaseSelector::instance()
{
    if (!m_Instance) {
        m_Instance = new DrugsDatabaseSelector;
    }
    return m_Instance;
}

void DatabaseInfos::toTreeWidget(QTreeWidget *tree) const
{
    QFont bold;
    bold.setBold(true);
    tree->clear();

    if (!fileName.isEmpty()) {
        QTreeWidgetItem *file = new QTreeWidgetItem(tree, QStringList() << "File name and identifiant");
        file->setFont(0, bold);
        if (fileName == DrugsDB::Constants::DEFAULT_DATABASE_IDENTIFIANT) {
            new QTreeWidgetItem(file, QStringList() << "File" << Trans::Constants::DEFAULTS);
        } else {
            QDir dir;
            QFileInfo fi(fileName);
            if (fi.absolutePath().startsWith(settings()->resourcesPath()))
                dir.setPath(settings()->resourcesPath());
            else if (fi.absolutePath().startsWith(settings()->path(Core::ISettings::BundleResourcesPath)))
                dir.setPath(settings()->path(Core::ISettings::BundleResourcesPath));
            new QTreeWidgetItem(file, QStringList() << "Absolute path" << QFileInfo(fileName).absolutePath());
            new QTreeWidgetItem(file, QStringList() << "Application relative path" << dir.relativeFilePath(QFileInfo(fileName).absoluteFilePath()));
            new QTreeWidgetItem(file, QStringList() << "File" << QFileInfo(fileName).fileName());
        }
        new QTreeWidgetItem(file, QStringList() << "Identifiant" << identifiant);
    }

    if (!connectionName.isEmpty()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree, QStringList() << "Connection Name");
        item->setFont(0, bold);
        new QTreeWidgetItem(item, QStringList() << "Connection" << connectionName);
    }

    QTreeWidgetItem *namesItem = new QTreeWidgetItem(tree, QStringList() << "Names");
    namesItem->setFont(0, bold);
    QHash<QString,QString> langs = names();
    foreach(const QString &k, langs.keys()) {
        QString l = k;
        if (l == "xx") {
            l.replace("xx", "All languages");
        } else {
            QLocale loc(k);
            l = QLocale::languageToString(loc.language());
        }
        new QTreeWidgetItem(namesItem, QStringList() << l << langs.value(k));
    }

    QTreeWidgetItem *countryItem = new QTreeWidgetItem(tree, QStringList() << "Country");
    countryItem->setFont(0, bold);
    if (lang_country.isEmpty()) {
        new QTreeWidgetItem(countryItem, QStringList() << "Not specific");
    } else {
        QLocale l(lang_country);
        new QTreeWidgetItem(countryItem, QStringList() << "Language Specific" << QLocale::languageToString(l.language()));
        new QTreeWidgetItem(countryItem, QStringList() << "Country Specific" << QLocale::countryToString(l.country()));
    }

    QTreeWidgetItem *authorItem = new QTreeWidgetItem(tree,QStringList() << "Provider, Author and License");
    authorItem->setFont(0, bold);
    new QTreeWidgetItem(authorItem, QStringList() << tkTr(Trans::Constants::PROVIDER) << provider);
    new QTreeWidgetItem(authorItem, QStringList() << "Sources link" << weblink);
    new QTreeWidgetItem(authorItem, QStringList() << tkTr(Trans::Constants::AUTHOR) << author);
    new QTreeWidgetItem(authorItem, QStringList() << "License" << license);
    /** \todo put a link instead of the content for licenseTerms */
    new QTreeWidgetItem(authorItem, QStringList() << "License terms" << licenseTerms);

    QTreeWidgetItem *validItem = new QTreeWidgetItem(tree,QStringList() << "Validity");
    validItem->setFont(0, bold);
    new QTreeWidgetItem(validItem, QStringList() << tkTr(Trans::Constants::VERSION) << version);
    new QTreeWidgetItem(validItem, QStringList() << "Compatible FreeDiams Version" << compatVersion);
    new QTreeWidgetItem(validItem, QStringList() << "Date of release" << date.toString(Qt::DefaultLocaleLongDate));
    if (atcCompatible)
        new QTreeWidgetItem(validItem, QStringList() << "ATC validity" << tkTr(Trans::Constants::AVAILABLE));
    else
        new QTreeWidgetItem(validItem, QStringList() << "ATC validity" << tkTr(Trans::Constants::UNAVAILABLE));
    if (iamCompatible)
        new QTreeWidgetItem(validItem, QStringList() << "IAM validity" << tkTr(Trans::Constants::AVAILABLE));
    else
        new QTreeWidgetItem(validItem, QStringList() << "IAM validity" <<tkTr(Trans::Constants::UNAVAILABLE));
    new QTreeWidgetItem(validItem, QStringList() << "DRUGS_NAME_CONSTRUCTOR" << drugsNameConstructor);

    tree->expandAll();
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
}

void DatabaseInfos::warn()
{
    QString tmp = name;
    qWarning() << "DatabaseInfos"
            << "\n  Name" << tmp.replace("\n", "   ;;   ")
            << "\n  Translated name"  << translatedName()
            << "\n  Country" << lang_country
            << "\n  FileName" << fileName
            << "\n  Version" << version
            << "\n  CompatVersion" << compatVersion
            << "\n  Provider" << provider
            << "\n  Author" << author
            << "\n  License" << license
            << "\n  licenseTerms" << licenseTerms
            << "\n  drugsUidName" << drugsUidName
            << "\n  packUidName" << packUidName
            << "\n  weblink" << weblink
            << "\n  atcCompatible" << atcCompatible
            << "\n  iamCompatible" << iamCompatible
            << "\n  date" << date
            ;
}

QString DatabaseInfos::translatedName() const
{
    // 1. parse name string to a hash
    QHash<QString, QString> langs = names();
    QString lang = QLocale().name().left(2);
    if (langs.keys().contains(lang)) {
        return langs.value(lang);
    } else {
        return langs.value("xx");
    }
    return QString();
}

QHash<QString, QString> DatabaseInfos::names() const
{
    QHash<QString, QString> langs;
    int l = 0;
    foreach(const QString &line, name.split("\n")) {
        l++;
        if (line.isEmpty())
            continue;
        QStringList lang = line.split("=");
        if (lang.count() != 2) {
            Utils::Log::addError("DatabaseInfos",
                                 QString("Error while parsing name of the database, "
                                         "line %1 contains %2 = sign instead of 2.\n"
                                         "Database : %3 \n"
                                         "Content : \n%4")
                                 .arg(l)
                                 .arg(lang.count()-1)
                                 .arg(fileName)
                                 .arg(name));
            continue;
        }
        langs.insert(lang.at(0), lang.at(1));
    }
    return langs;
}

DrugsDatabaseSelector::DrugsDatabaseSelector() : d(0)
{
    d = new Internal::DrugsDatabaseSelectorPrivate;
}

DrugsDatabaseSelector::~DrugsDatabaseSelector()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void DrugsDatabaseSelector::getAllDatabaseInformations(const QStringList &paths) const
{
    QStringList allPaths = paths;

    // Add application resources path if is was not included
    QString appDatabases = settings()->databasePath() + QDir::separator() + DrugsDB::Constants::DRUGS_DATABASE_NAME;
    if (!allPaths.contains(appDatabases)) {
        allPaths.append(appDatabases);
    }

    // check all paths
    foreach(const QString &path, allPaths) {

        // get all *.db files
        QDir dir(path, "*.db");
        foreach(const QFileInfo &fi, dir.entryInfoList(QDir::Files | QDir::Readable | QDir::CaseSensitive, QDir::Name | QDir::IgnoreCase)) {
            // try to connect database with SQLite driver
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", fi.fileName());
            db.setDatabaseName(fi.absoluteFilePath());
            if (!db.open()) {
                Utils::Log::addError("DrugsDatabaseSelector", QString("Error %1 when trying to open database %2")
                                     .arg(db.lastError().text()).arg(fi.fileName()), true);
                continue;
            }
            DatabaseInfos *info = DrugsDB::Internal::DrugsBase::instance()->getDatabaseInformations(fi.fileName());
            if (info) {
//                info->fileName = fi.absoluteFilePath();
                info->warn();
                d->m_FileName_Infos.insert(fi.fileName(), info);
            }
        }

        // clear all newly created Qt databases (without console warnings)
        foreach(const QFileInfo &fi, dir.entryInfoList(QDir::Files | QDir::Readable | QDir::CaseSensitive, QDir::Name | QDir::IgnoreCase)) {
            QSqlDatabase::removeDatabase(fi.fileName());
        }
    }
}

bool DrugsDatabaseSelector::setCurrentDatabase(const QString &fileName)
{
    d->m_Current = d->m_FileName_Infos.value(fileName, 0);
    /** \todo reload drugs database */
    return d->m_Current!=0;
}

DatabaseInfos DrugsDatabaseSelector::currentDatabase() const
{
    DatabaseInfos toReturn;
    if (d->m_Current) {
        toReturn = *d->m_Current;
    }
    return toReturn;
}

QVector<DatabaseInfos *> DrugsDatabaseSelector::availableDatabases() const
{
//    qWarning() << d->m_FileName_Infos << d->m_FileName_Infos.values().toVector();
    return d->m_FileName_Infos.values().toVector();
}
