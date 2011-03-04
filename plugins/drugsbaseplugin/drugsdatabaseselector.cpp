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
#include "drugsdatabaseselector.h"
#include "drugsbase.h"
#include "constants.h"

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
static inline DrugsDB::Internal::DrugsBase *base() {return DrugsDB::Internal::DrugsBase::instance();}



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
        qDeleteAll(m_Infos);
        m_Infos.clear();
        m_Current = 0;
    }

public:
    QVector<DatabaseInfos *> m_Infos;
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

DatabaseInfos::DatabaseInfos() :
        identifiant(Constants::DB_DEFAULT_IDENTIFIANT)
{
    names.insert("xx", "No drug database available");
}

void DatabaseInfos::setDrugsNameConstructor(const QString &s)
{
    drugsNameConstructor = s;
    QRegExp rx("\\b(NAME|FORM|ROUTE|GLOBAL_STRENGTH)\\b");
    QStringList list;
    int pos = 0;
    while ((pos = rx.indexIn(s, pos)) != -1) {
        list << "`DRUGS`.`" + rx.cap(1) +"`";
        pos += rx.matchedLength();
    }
    drugsNameConstructorSearchFilter = list.join(" || ");
}

void DatabaseInfos::toTreeWidget(QTreeWidget *tree) const
{
    QFont bold;
    bold.setBold(true);
    tree->clear();

    if (!fileName.isEmpty()) {
        QTreeWidgetItem *file = new QTreeWidgetItem(tree, QStringList() << "File name and identifiant");
        file->setFont(0, bold);
        if (fileName == DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT) {
            new QTreeWidgetItem(file, QStringList() << "File" << Trans::Constants::DEFAULTS);
        } else {
            QDir dir;
            QFileInfo fi(fileName);
            if (fi.absolutePath().startsWith(settings()->resourcesPath()))
                dir.setPath(settings()->resourcesPath());
            else if (fi.absolutePath().startsWith(settings()->path(Core::ISettings::BundleResourcesPath)))
                dir.setPath(settings()->path(Core::ISettings::BundleResourcesPath));
//            new QTreeWidgetItem(file, QStringList() << "Absolute path" << QFileInfo(fileName).absolutePath());
//            new QTreeWidgetItem(file, QStringList() << "Application relative path" << dir.relativeFilePath(QFileInfo(fileName).absoluteFilePath()));
//            new QTreeWidgetItem(file, QStringList() << "DrugBaseUid" << QFileInfo(fileName).fileName());
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
    foreach(const QString &k, names.keys()) {
        QString l = k;
        if (l == "xx") {
            l.replace("xx", "All languages");
        } else {
            QLocale loc(k);
            l = QLocale::languageToString(loc.language());
        }
        new QTreeWidgetItem(namesItem, QStringList() << l << names.value(k));
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
        new QTreeWidgetItem(validItem, QStringList() << "Interaction engine validity" << tkTr(Trans::Constants::AVAILABLE));
    else
        new QTreeWidgetItem(validItem, QStringList() << "Interaction engine validity" <<tkTr(Trans::Constants::UNAVAILABLE));
    new QTreeWidgetItem(validItem, QStringList() << "Interaction engine completion" << QString("%1 %").arg(moleculeLinkCompletion));
    new QTreeWidgetItem(validItem, QStringList() << "DRUGS_NAME_CONSTRUCTOR" << drugsNameConstructor);
    new QTreeWidgetItem(validItem, QStringList() << "DRUGS_NAME_CONSTRUCTOR (filter)" << drugsNameConstructorSearchFilter);

    tree->expandAll();
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
}

void DatabaseInfos::warn()
{
    QString tmp;
    foreach(const QString &lang, names.keys()) {
        tmp += QString("%1 - %2; ").arg(lang).arg(names.value(lang));
    }
    tmp.chop(2);
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
    QString lang = QLocale().name().left(2);
    if (names.keys().contains(lang)) {
        return names.value(lang);
    } else {
        return names.value("xx");
    }
    return QString();
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

void DrugsDatabaseSelector::getAllDatabaseInformations() const
{
    d->m_Infos.clear();
    d->m_Infos = base()->getAllDrugSourceInformations();
}

bool DrugsDatabaseSelector::setCurrentDatabase(const QString &dbUid)
{
    for(int i=0; i < d->m_Infos.count(); ++i) {
        DatabaseInfos *info = d->m_Infos.at(i);
        if (info->drugsUidName == dbUid) {
            d->m_Current = info;
            return true;
        }
    }
    /** \todo reload drugs database */
    return false;
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
    return d->m_Infos;
}
