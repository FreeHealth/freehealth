/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFDRUGSBASE_H
#define MFDRUGSBASE_H

// include drugswidget headers
#include <mfDrugsConstants.h>
#include <drugsdatabase/mfInteractionsBase.h>
class mfDrugs;
class mfDrugInteraction;
class mfDrugDosage;
class mfDrugInfo;
class mfDrugsBasePrivate;

// include Qt headers
#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QMultiHash>

/**
 * \file mfDrugsBase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.14
 * \date 03 July 2009
*/

using namespace mfInteractionsConstants;

class mfDrugsBase : public mfInteractionsBase
{
    Q_OBJECT
    mfDrugsBase( QObject *parent = 0 );
    bool init();

    friend class mfDrugsModel;
    friend class mfDrugsBasePrivate;
    friend class mfDrugs;
    friend class mfDrugInfo;
    friend class mfDrugInteraction;

public:
    static mfDrugsBase *instance();
    ~mfDrugsBase();
    // INITIALIZER
    static bool isInitialized() { return m_initialized; }
    void checkDosageDatabaseVersion();

    QList<int> getLinkedCodeSubst( QList<int> & code_iam ) const;
    QList<int> getLinkedSubstCode( const QString & iamDenomination );
    QList<int> getLinkedIamCode( QList<int> & code_subst ) const ;
    int        getInnCodeForCodeMolecule(const int code) const;

    mfDrugs * getDrugByCIP( const QVariant & CIP_id );
    mfDrugs * getDrugByCIS( const QVariant & CIS_id );
    int       getCISFromCIP( int CIP );
    QString   getInnDenominationFromSubstanceCode( const int code_subst );
    QString   getInnDenomination( const int inncode ) const;

    void      logChronos( bool state );

    QHash<QString, QString> getDosageToTransmit();
    bool markAllDosageTransmitted( const QStringList &dosageUuids );

//protected:
public:
    static const QString separator;

    // managins drugs
    bool         drugsINNIsKnown( const mfDrugs * drug );

private:
    bool createDatabase(  const QString & connectionName , const QString & dbName,
                          const QString & pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString & /*login*/, const QString & /*pass*/,
                          CreationOption /*createOption*/
                          );

private:
    // intialization state
    static mfDrugsBase *m_Instance;
    static bool m_initialized;
    mfDrugsBasePrivate * d;
};

#endif
