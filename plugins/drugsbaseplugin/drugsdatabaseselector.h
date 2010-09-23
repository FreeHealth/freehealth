/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#ifndef DRUGSDATABASESELECTOR_H
#define DRUGSDATABASESELECTOR_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QString>
#include <QDate>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

namespace DrugsDB {
namespace Internal {
//class DatabaseInfosPrivate;
class DrugsDatabaseSelectorPrivate;
}

class DRUGSBASE_EXPORT DatabaseInfos
{
public:
    DatabaseInfos();
    void warn();
    QString translatedName() const;
    QHash<QString, QString> names() const;
    void setDrugsNameConstructor(const QString &s);
    void toTreeWidget(QTreeWidget *tree) const;

    QString name, identifiant, fileName, version, compatVersion, lang_country, connectionName;
    QString provider, author, license, drugsUidName, packUidName;
    QString drugsNameConstructor,  drugsNameConstructorSearchFilter;
    QString weblink, complementaryWebsite, authorComments, licenseTerms;
    bool atcCompatible, iamCompatible;
    QDate date;
    int moleculeLinkCompletion;
};


class DRUGSBASE_EXPORT DrugsDatabaseSelector
{
    DrugsDatabaseSelector();
public:
    static DrugsDatabaseSelector *instance();
    ~DrugsDatabaseSelector();

    void getAllDatabaseInformations(const QStringList &paths = QStringList()) const;
    bool setCurrentDatabase(const QString &fileName);
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

}  // End namespace DrugsDB

#endif // DRUGSDATABASESELECTOR_H
