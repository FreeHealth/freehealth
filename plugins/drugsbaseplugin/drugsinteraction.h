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
#ifndef DRUGSINTERACTION_H
#define DRUGSINTERACTION_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/constants.h>

// include Qt headers
#include <QIcon>
#include <QVariant>
#include <QObject>

/**
 * \file drugsinteraction.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.2.1
 * \date 25 Oct 2009
*/

/** \todo Some parts should not be Internals */


namespace DrugsDB {
namespace Internal {
class DrugsData;
class InteractionsBase;

class DRUGSBASE_EXPORT DrugsInteraction
{
    friend class InteractionsBase;
    friend class InteractionsBasePrivate;
public:
    enum DataRepresentation {
        DI_Id = 0,
        DI_LinkId,
        DI_Type,
        DI_ATC1,
        DI_ATC2,
        DI_ATC1_Label,
        DI_ATC2_Label,
        DI_Risk,
        DI_Management,

        // only used for set
        DI_RiskFr,
        DI_RiskEn,
        DI_ManagementFr,
        DI_ManagementEn,
        DI_Xml
    };


    DrugsInteraction() {}
    ~DrugsInteraction() {}

    // static getters and viewers
    static QIcon iamIcon( const DrugsData *drug, const int & levelOfWarning = 0 );  // mfDrugsBase::interactions should be called first
    static QString listToHtml( const QList<DrugsInteraction *> & list, bool fullInfos );
    static QString synthesisToHtml( const QList<DrugsInteraction *> & list, bool fullInfos );
    static QString typeToString( const int t );

    // getters
    QList<DrugsData *> drugs() const;
    QVariant value( const int fieldref ) const;
    QString typeOfIAM( const int & t ) const;
    Constants::Interaction::TypesOfIAM type() const;
    QString header() const;
    QString information() const;
    QString whatToDo() const;

    // viewers
    void warn() const;


protected:
    // setters
    void setValue( const int fieldref, const QVariant & value ); // ajouter CIS1 CIS2
    void addInteractingDrug(DrugsData *drug)
    {
        Q_ASSERT(drug);
        if (!m_InteractingDrugs.contains(drug))
            m_InteractingDrugs << drug;
    }

private:
    QHash<int, QVariant> m_Infos;
    QList<DrugsData *> m_InteractingDrugs;
};

}  // End Internal
}  // End DrugsDB

#endif  // DRUGSINTERACTION_H
