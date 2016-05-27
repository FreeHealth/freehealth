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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSDB_DRUGSDATABASESELECTOR_H
#define DRUGSDB_DRUGSDATABASESELECTOR_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QString>
#include <QDate>
#include <QStringList>
#include <QHash>

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

namespace DrugsDB {
class DrugsBase;

namespace Internal {
//class DatabaseInfosPrivate;
class DrugsDatabaseSelectorPrivate;
}

class DRUGSBASE_EXPORT DatabaseInfos
{
    friend class DrugsDB::DrugsBase;

public:
    DatabaseInfos();

    QString translatedName() const;

    void setDrugsNameConstructor(const QString &s);
    void toTreeWidget(QTreeWidget *tree) const;

    QString identifier, fileName, version, compatVersion, lang_country, connectionName;
    QString provider, author, copyright, license, drugsUidName, packUidName;
    QString drugsNameConstructor,  drugsNameConstructorSearchFilter;
    QString weblink, complementaryWebsite, authorComments, licenseTerms;
    bool atcCompatible, iamCompatible;
    QDate date;
    int moleculeLinkCompletion, sid;
    QHash<QString, QString> names;
};


class DRUGSBASE_EXPORT DrugsDatabaseSelector
{
    DrugsDatabaseSelector();
public:
    static DrugsDatabaseSelector *instance();
    ~DrugsDatabaseSelector();

    void getAllDatabaseInformation() const;
    bool setCurrentDatabase(const QString &dbUid);
    DatabaseInfos currentDatabase() const;
    QVector<DatabaseInfos *> availableDatabases() const;

private:
    static DrugsDatabaseSelector *m_Instance;
    Internal::DrugsDatabaseSelectorPrivate *d;
};



//class DRUGSBASE_EXPORT DrugsDatabaseInfoModel : public QAbstractItemModel
//{
//    Q_OBJECT
//public:
//    DrugsDatabaseInfoModel(QObject *parent = 0);
//    ~DrugsDatabaseInfoModel();
//
//    int columnCount(const QModelIndex &parent = QModelIndex()) const;
//    int	rowCount (const QModelIndex &parent = QModelIndex()) const;
//
//    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
//    Qt::ItemFlags flags(const QModelIndex &index) const;
//
//private:
//
//};

DRUGSBASE_EXPORT QDebug operator<<(QDebug dbg, const DrugsDB::DatabaseInfos &c);
DRUGSBASE_EXPORT QDebug operator<<(QDebug dbg, const DrugsDB::DatabaseInfos *c);

}  // End namespace DrugsDB

#endif // DRUGSDB_DRUGSDATABASESELECTOR_H
