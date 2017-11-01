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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef INTERACTIONSBASE_H
#define INTERACTIONSBASE_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <utils/database.h>

#include <QList>

/**
 * \file interactionsbase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 23 Fev 2010
*/

/** \todo Some parts should not be Internals */

namespace MedicalUtils {
class EbmData;
}

namespace DrugsDB {
class DatabaseInfos;

namespace Internal {
class InteractionsBasePrivate;
class DrugsInteraction;
class DrugsData;

class DRUGSBASE_EXPORT InteractionsBase
{
    friend class InteractionsBasePrivate;
public:
    InteractionsBase();
    virtual ~InteractionsBase();

    // INITIALIZER
    virtual bool init(bool refreshCache = false);
    bool isInitialized() const;
    static bool isInteractionDatabaseAvailable() {return m_InteractionsDatabaseAvailable;}
    virtual const DatabaseInfos *actualDatabaseInformation() const = 0;

    virtual void logChronos(bool state);

    // link to Utils::Database
    QString iamTable(const int ref) const;
    QString getIamWhereClause(const int &tableref, const QHash<int, QString> &conditions) const;
    QString selectInteractionsSql(const int &tableref, const QList<int> &fieldsref, const QHash<int, QString> &conditions) const;

    // link to DrugsBase
//    int getInnCodeForCodeMolecule(const int molecule_code) const;
//    QString getAtcLabel(const int atcId) const;
//    QString getAtcLabel(const QString &code) const;
//    QString getAtcCode(const int atcId) const;
//    QString getInnDenominationFromSubstanceCode(const int molecule_code) const;

//    QStringList getIamClassDenomination(const int &molecule_code);
//    QSet<int> getAllInnAndIamClassesIndex(const int molecule_code);

    // Interactions base
    QList<DrugsInteraction*> calculateInteractions(const QList<DrugsData *> &drugs);

    QList<int> getLinkedMoleculeCodes(QList<int> &atc_ids) const;
    QList<int> getLinkedMoleculeCodes(const int atc_id) const;
    QList<int> getLinkedMoleculeCodes(const QString &iamDenomination) const;
    QList<int> getLinkedAtcIds(const QList<int> &molecule_codes) const;
    QList<int> getLinkedAtcIds(const int molecule_code) const;

    QList<int> getAllMoleculeCodeWithAtcStartingWith(const QString &code) const;

    QVector<MedicalUtils::EbmData *> getAllSourcesFromTree(const QList<int> &allInnAndIamClassIds);

private:
    InteractionsBasePrivate *di;
    static bool m_InteractionsDatabaseAvailable;
};

}  // End Internal
}  // End DrugsDB

#endif   // INTERACTIONSBASE_H
