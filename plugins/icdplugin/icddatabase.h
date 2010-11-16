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
 ***************************************************************************/
#ifndef ICDDATABASE_H
#define ICDDATABASE_H

#include <utils/database.h>

/**
 * \file icddatabase.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 13 Oct 2010
*/


namespace ICD {

namespace Internal {
class IcdDatabasePrivate;
}

class IcdDatabase : public QObject, public Utils::Database
{
    Q_OBJECT

    IcdDatabase(QObject *parent = 0);

public:
    static IcdDatabase *instance();
    ~IcdDatabase();

    // Initializer / Checkers
    static bool isInitialized() { return m_initialized; }
    void logChronos(bool state);

    QList<int> getHeadersSID(const QVariant &SID);

    QVariant getIcdCode(const QVariant &SID);
    QString getDagStarCode(const QVariant &SID);
    QString getHumanReadableIcdDaget(const QVariant &SID);
    QVariant getIcdCodeWithDagStar(const QVariant &SID);

    QVector<int> getDagStarDependencies(const QVariant &SID);
    QString getDagStarCodeWithDependency(const QVariant &SID, const QVariant &dependOnSID);
    QString getHumanReadableIcdDagetWithDependency(const QVariant &SID, const QVariant &dependOnSID);

    QString getLabelFromLid(const QVariant &LID);
    QString getSystemLabel(const QVariant &SID);
    QStringList getAllLabels(const QVariant &SID, const int libelleFieldLang = -1);
    QStringList getIncludedLabels(const QVariant &SID);

    QVector<int> getExclusions(const QVariant &SID);

    QString getMemo(const QVariant &SID);

private:
    bool init();

private:
    // intialization state
    static IcdDatabase *m_Instance;
    static bool m_initialized;
    Internal::IcdDatabasePrivate *d;
};

} // End namespace ICD


#endif // ICDDATABASE_H
