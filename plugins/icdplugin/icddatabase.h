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
#ifndef ICDDATABASE_H
#define ICDDATABASE_H

#include <icdplugin/icd_exporter.h>

#include <utils/database.h>

/**
 * \file icddatabase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.4
 * \date 13 Feb 2012
*/

namespace DataPack {
class Pack;
}

namespace ICD {

namespace Internal {
class IcdDatabasePrivate;
class IcdPlugin;
class IcdAssociation;
}

class ICD_EXPORT IcdDatabase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class ICD::Internal::IcdPlugin;
    IcdDatabase(QObject *parent = 0);

public:
    static IcdDatabase *instance();
    ~IcdDatabase();

    // Initializer / Checkers
    static bool isInitialized() { return m_initialized; }
    void logChronos(bool state);
    QString getDatabaseVersion();
    bool isDownloadAndPopulatingNeeded() const;

    QList<int> getHeadersSID(const QVariant &SID);

    QVariant getSid(const QString &code);
    QVariant getIcdCode(const QVariant &SID);
    QString getDagStarCode(const QVariant &SID);
    QString invertDagCode(const QString &dagCode) const;
    bool isDagetADag(const QString &dagCode) const;
    QString getHumanReadableIcdDaget(const QVariant &SID);
    QVariant getIcdCodeWithDagStar(const QVariant &SID);

    QVector<int> getDagStarDependencies(const QVariant &SID);
    Internal::IcdAssociation getAssociation(const QVariant &mainSID, const QVariant &associatedSID);

    bool codeCanBeUsedAlone(const QVariant &SID);

    QString getLabelFromLid(const QVariant &LID);
    QString getSystemLabel(const QVariant &SID);
    QStringList getAllLabels(const QVariant &SID, const int libelleFieldLang = -1);
    QString getAssociatedLabel(const QVariant &mainSID, const QVariant &associatedSID);
    QStringList getIncludedLabels(const QVariant &SID);

    QVector<int> getExclusions(const QVariant &SID);

    QString getMemo(const QVariant &SID);

Q_SIGNALS:
    void databaseInitialized();

protected:
    bool initialize();
    bool refreshDatabase();

private Q_SLOTS:
    void packChanged(const DataPack::Pack &pack);
    void refreshLanguageDependCache();

Q_SIGNALS:
    void databaseChanged();

private:
    // intialization state
    static IcdDatabase *m_Instance;
    static bool m_initialized;
    Internal::IcdDatabasePrivate *d;
};

} // End namespace ICD


#endif // ICDDATABASE_H
