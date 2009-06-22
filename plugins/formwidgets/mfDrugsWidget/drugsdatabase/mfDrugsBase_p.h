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
#ifndef MFDRUGSBASE_P_H
#define MFDRUGSBASE_P_H
class mfDrugs;
class mfDrugInteraction;
#include <QMultiHash>
#include <QMap>
#include <QMultiMap>
#include <QList>

class mfDrugsBasePrivate
{
public:
    mfDrugsBasePrivate( mfDrugsBase * base );
    // connections creator
    bool createConnections( const QString & path, const QString & db, const QString & dbName, bool readwrite );

    // Link tables
    void retreiveLinkTables();

    // private members for interactions
    QString getIamSubst( const int & code_subst );
    QStringList getIamClass( const int & code_subst );
    bool checkDrugInteraction( mfDrugs * drug, const QList<mfDrugs*> & drugs );
    mfDrugInteraction * getIAM( const int & _id1, const int & _id2 );

public:
    // These variables are used or speed improvments and database protection
    QMultiHash< int, int >    m_Lk_iamCode_substCode;   // Link Iam_Id to Code_Subst
    QMultiHash< int, int >    m_Lk_classCode_iamCode;   // Link ClassIam_Id to Iam_Id
    QMap<int, int>            m_Iams;                   // All possible interactions based on Iam_Ids

    QMultiMap< int, int>      m_IamFound;               // modified by checkDrugInteraction()

    // cached datas for speed improvments
    QList<mfDrugInteraction*>             m_DrugInteractionList;      // First filled by interactions()
    QMap<int, mfDrugInteraction* >        m_Map_CIS_DrugInteraction;  // idem
    mfDrugsBase * m_DrugsBase;
};
#endif // MFDRUGSBASE_P_H
