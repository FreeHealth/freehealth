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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef MFDRUGS_H
#define MFDRUGS_H

// include drugswidget headers
//#include "drugsdatabase/mfDrugsTables.h"
#include <mfDrugsConstants.h>
class mfDrugDosage;
class mfDrugsBase;
class mfDrugsBasePrivate;
class mfDrugsPrivate;

// include Qt headers
#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QDate>
#include <QSet>

/**
 * \file mfDrugs.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.11
 * \date 12 May 2009
*/

/**
  \brief This class owns the getters, setters and viewers of drugs.
  For each drug, the datas are stored into QHash. The QHash are exact images of the database tables.
  Three tables of the drugs database are used : CIS, COMPO and CIS_CIP.

  The interactions are stored into specialized class : mfDrugInteraction.
  The dosage of drugs are stored into : mfDrugDosage.

  The drug class owns the list of all available dosages, see addDosage() and getDosages().

  \todo Actually inside FMF and drugwidgetplugins mfDrugs are carried via a QList of pointers. This way we can not
  use the operators with mfDrugs. May be we must redefine QList of pointers in QList of references ?
 \ingroup drugsinteractions drugswidget
*/

using namespace mfDrugsConstants;

class mfDrugs : public QObject
{
    Q_OBJECT
    friend class mfDrugsBase;
    friend class mfDrugsBasePrivate;

public:
     mfDrugs( QObject * parent = 0 );
     ~mfDrugs();

     // setters
     void setPrescriptionValue( const int fieldref, const QVariant & value );

     // getters
     const int         CIS() const                { return value( Table_CIS, CIS_CIS ).toInt();  }
     QList<QVariant>   CIPs() const;
     QStringList       CIPsDenominations() const;
     const QString     denomination() const;
     QString           form() const               { return value( Table_CIS, CIS_FORME ).toString(); }

     int               numberOfCodeMolecules() const { return listOfMolecules().count(); }
     QStringList       listOfMolecules() const    { return value( Table_COMPO, COMPO_DENOMINATION ).toStringList(); }
     QVariant          listOfCodeMolecules() const{ return value( Table_COMPO, COMPO_CODE_SUBST ); }

     QStringList       listOfInn() const;
     QStringList       listOfInnWithPosology() const;
     int               numberOfInn() const        { return allInnAndIamClasses().count(); }
     QStringList       listOfInnClasses() const   { return value( Table_COMPO, COMPO_IAM_CLASS_DENOMINATION ).toStringList(); }
     QStringList       innEquivalentsFullNames() const;
     void              setInnEquivalentsFullNames( const QStringList &list );
     QSet<int>         allInnAndIamClasses() const;

     QStringList       dosageOfMolecules() const;
     bool              hasPrescription() const;
     bool              isScoredTablet() const;

     QVariant          prescriptionValue( const int filedref ) const;
     QString           prescriptionToHtml() const;
     QString           prescriptionToPlainText() const;

     // this need to be rewritten because there is N CIP for 1 drug
     //const int CIP()                    { return value( mfDrugsTables::CISP_CIP ).toInt(); }

     // viewers
     QString toHtml() const;
     static QString drugsListToHtml( const QList<mfDrugs*> & list );
     void warn() const;
     void smallDrugWarn() const;

     // sorters
     static bool lessThan( const mfDrugs *drug1, const mfDrugs *drug2 );

protected:
     // setters
     void setValue( const int tableref, const int fieldref, const QVariant & value );
     void addInnAndIamClasses( const QSet<int> &codes );
     void addCIP( const int CIP, const QString & denomination, QDate date = QDate() );

     // getters
     QVariant value( const int tableref, const int fieldref ) const;

private:
     mfDrugsPrivate *d;
};

// Q_DECLARE_METATYPE( mfDrugs )
typedef QList<mfDrugs*> QDrugsList;

#endif
