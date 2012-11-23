/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
/**
  \class mfDrugInteraction
  \brief Drugs Interaction class.
  This class is a 'data class'. It only holds datas about interactions.\n
  You can statically :
  \li get the interaction's icon using the static iamIcon().
  \li transform a list of interactions to human readable Html with listToHtml()
  \li transform a list of interactions to a synthesis human readable Html with synthesisToHtml()
  \li get the type of interaction's name using typeToString().

  You can, non statically :
  \li set the values of the interactions with setValue() (only used by mfDrugsBase).
  \li get the values with value()
  \li get the type of interaction with typeOfIAM() and type()
  \li get information's text of the interaction with information()
  \li get the recommandation's text with whatToDo()
  \ingroup freediams drugswidget
*/

#include "mfDrugInteraction.h"

#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugs.h>
#include <mfDrugsConstants.h>

// include toolkit headers
#include <tkLog.h>
#include <tkTheme.h>

using namespace mfDrugsConstants;
using namespace mfDosagesConstants;
using namespace mfInteractionsConstants;

/** \brief Used by drugs database to feed values. \e fieldref refers to the enum : mfDrugsConstants::IAMfields */
void mfDrugInteraction::setValue( const int fieldref, const QVariant & value )
{
    if ( fieldref == IAM_TYPE )
    {
        if ( value.toInt() == Interaction::Information ) {
            m_Infos.insert( fieldref, Interaction::Information );
        } else {
            int t = value.toInt();
            Interaction::TypesOfIAM r = Interaction::noIAM;
            if (( t % 2 ) == 1 )
                r |= Interaction::Precaution;

            if ( t / 1000 == 1 )
                r |= Interaction::ContreIndication;

            if ( t / 100 == 1 )
                r |= Interaction::Deconseille;

            if ( t / 10 == 1 )
                r |= Interaction::APrendreEnCompte;

            m_Infos.insert( fieldref, int( r ) );
        }
    }
    else
        m_Infos.insert( fieldref, value );
}

/** \brief Get values of the interaction class. \e fieldref refers to the enum : mfDrugsConstants::IAMfields */
QVariant mfDrugInteraction::value( const int fieldref ) const
{
     if ( fieldref == IAM_TYPE )
          return typeOfIAM( m_Infos.value( fieldref ).toInt() );
     else
          if ( m_Infos.contains( fieldref ) )
               return m_Infos.value( fieldref );
     return QVariant();
}

/** \brief Transforms the type \e t to its name. \e t refers to enum : mfInteractionsConstants::Interaction::TypesOfIAM */
QString mfDrugInteraction::typeToString( const int t )
{
     QStringList tmp;
     Interaction::TypesOfIAM r = Interaction::TypesOfIAM( t );
     if ( r & Interaction::APrendreEnCompte )
          tmp << tr( "Take into account" );
     if ( r & Interaction::Deconseille )
          tmp << tr( "Discouraged" );
     if ( r & Interaction::ContreIndication )
          tmp << tr( "Contraindication" );
     if ( r & Interaction::Precaution )
          tmp << tr( "Precaution for use" );
     if ( r & Interaction::Information )
          tmp << tr( "Information" );

     return tmp.join( ", " );
}

/** \brief Transforms the type \e t to its name. \e t refers to enum : mfInteractionsConstants::Interaction::TypesOfIAM */
QString mfDrugInteraction::typeOfIAM( const int & t ) const
{
    return typeToString(t);
}

/** \brief Returns the type of interactions. Type refers to enum : mfInteractionsConstants::Interaction::TypesOfIAM */
Interaction::TypesOfIAM mfDrugInteraction::type() const
{
     if ( m_Infos.uniqueKeys().contains( IAM_TYPE ) )
          return Interaction::TypesOfIAM( m_Infos.value( IAM_TYPE ).toInt() );
     else
          return Interaction::TypesOfIAM( 0 );
}

QList<mfDrugs*> mfDrugInteraction::drugs() const
{
    return m_InteractingDrugs;
}

/** \brief for debugging purpose */
void mfDrugInteraction::warn() const
{
     qWarning() << "mfDrugsInteraction Warning";
     foreach( const int i, m_Infos.keys() )
         qWarning() << i << m_Infos.value(i).toString();
     foreach(mfDrugs* dr, m_InteractingDrugs)
         qWarning() << "drug" << dr->denomination();
}

QString mfDrugInteraction::header() const
{
    return value( IAM_MAIN ).toString() + " - " + value( IAM_INTERACTOR ).toString() ;
}

/** \brief Returns the information's text of the interaction */
QString mfDrugInteraction::information() const
{
    return value( IAM_TEXT_IAM ).toString();
}

/** \brief Returns the recommandation's text of the interaction */
QString mfDrugInteraction::whatToDo() const
{
    return value( IAM_TEXT_CAT ).toString();
}

