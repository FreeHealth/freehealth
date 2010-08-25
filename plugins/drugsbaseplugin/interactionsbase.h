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
#ifndef INTERACTIONSBASE_H
#define INTERACTIONSBASE_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <utils/database.h>

#include <QList>

/**
 * \file interactionsbase.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 23 Fev 2010
*/

/** \todo Some parts should not be Internals */


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
    virtual const DatabaseInfos *actualDatabaseInformations() const = 0;

    virtual void logChronos(bool state);

    // link to Utils::Database
    QString iamTable(const int ref) const;
    QString getIamWhereClause(const int &tableref, const QHash<int, QString> &conditions) const;
    QString selectInteractionsSql(const int &tableref, const QList<int> &fieldsref, const QHash<int, QString> &conditions) const;

    // link to DrugsBase
    int getInnCodeForCodeMolecule(const int molecule_code) const;
    QString getAtcLabel(const int inncode) const;
    QString getAtcLabel(const QString &code) const;
    QString getAtcCode(const int atc_id) const;
    QString getInnDenominationFromSubstanceCode(const int molecule_code) const;

    QStringList getIamClassDenomination(const int &molecule_code);
    QSet<int> getAllInnAndIamClassesIndex(const int molecule_code);

    // Interactions base
    QList<DrugsInteraction*> calculateInteractions(const QList<DrugsData *> &drugs);

    QList<int> getLinkedMoleculeCodes(QList<int> &atc_ids) const;
    QList<int> getLinkedMoleculeCodes(const int atc_id) const;
    QList<int> getLinkedMoleculeCodes(const QString &iamDenomination) const;
    QList<int> getLinkedAtcIds(const QList<int> &molecule_codes) const;
    QList<int> getLinkedAtcIds(const int molecule_code) const;

    QList<int> getAllMoleculeCodeWithAtcStartingWith(const QString &code) const;

private:
    InteractionsBasePrivate *di;
    static bool m_InteractionsDatabaseAvailable;
};

}  // End Internal
}  // End DrugsDB

#endif   // INTERACTIONSBASE_H
