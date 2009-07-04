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
  \ingroup drugsinteractions drugswidget
*/

#include "mfDrugInteraction.h"

// include toolkit headers
#include <tkLog.h>
#include <tkTheme.h>

using namespace mfDrugsConstants;
using namespace mfDosagesConstants;
using namespace mfInteractionsConstants;

namespace mfDrugInteractionConstants {
    const char* const  LIST_BASIC_INFO =
            "<tr>"
            "  <td><b>%1</b></td>\n"
            "  <td>%2 &lt;--&gt; %3</td>\n"
            "</tr>\n";
    const char* const LIST_FULL_INFO =
            "<tr>\n"
            "  <td>%1</td>\n"
            "  <td>%2</td>\n"
            "</tr>\n"
            "<tr>\n"
            "  <td>%3</td>\n"
            "  <td>%4</td>\n"
            "</tr>\n";
    const char* const LIST_MASK =
            "<span style=\"font-size:%1pt\"><table border=1 cellpadding=2 cellspacing=2 width=100%>\n"
            "<tr>\n"
            "  <td colspan=2 align=center><b>%2</b></td>\n"
            "</tr>\n"
            "%3\n"
            "</table></span>\n";
}

using namespace mfDrugInteractionConstants;

/** \brief Used by drugs database to feed values. \e fieldref refers to the enum : mfDrugsConstants::IAMfields */
void mfDrugInteraction::setValue( const int fieldref, const QVariant & value )
{
    if ( fieldref == IAM_TYPE )
    {
        if ( value.toInt() == Interaction::Information )
            m_Infos.insert( fieldref, Interaction::Information );
        else
        {
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

/** \brief Returns the icon of the interaction regarding the \e levelOfWarning for a selected \e drug. */
QIcon mfDrugInteraction::iamIcon( const mfDrugs * drug, const int & levelOfWarning )  // static
{
    // mfDrugsBase::interactions() should be called first
    mfDrugsBase *b = mfDrugsBase::instance();
    if ( b->drugHaveInteraction( drug ) ) {
        Interaction::TypesOfIAM r = b->getMaximumTypeOfIAM( drug );
        if ( r & Interaction::ContreIndication )
            return tkTheme::icon( INTERACTION_ICONCRITICAL );
        else if ( r & Interaction::Deconseille )
            return tkTheme::icon( INTERACTION_ICONDECONSEILLEE );
        else if ( ( r & Interaction::APrendreEnCompte ) && ( levelOfWarning <= 1 ) )
            return tkTheme::icon( INTERACTION_ICONTAKEINTOACCOUNT );
        else if ( ( r & Interaction::Precaution ) && ( levelOfWarning <= 1 ) )
            return tkTheme::icon( INTERACTION_ICONPRECAUTION );
        else if ( ( r & Interaction::Information ) && ( levelOfWarning == 0 ) )
            return tkTheme::icon( INTERACTION_ICONINFORMATION );
        else if ( r & Interaction::noIAM )
            return tkTheme::icon( INTERACTION_ICONOK );
        else
            return tkTheme::icon( INTERACTION_ICONUNKONW );
    } else if ( levelOfWarning <= 1 )
    {
        if ( ! b->drugsINNIsKnown( drug ) )
            return tkTheme::icon( INTERACTION_ICONUNKONW );
        else return tkTheme::icon( INTERACTION_ICONOK );
    }
    return QIcon();
}

/** \brief for debugging purpose */
void mfDrugInteraction::warn() const
{
     foreach( const int i, m_Infos.keys() )
         tkLog::addMessage( this, m_Infos.value(i).toString() );
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

/** \brief Transforms a list of interactions to human readable Html (static). */
QString mfDrugInteraction::listToHtml( const QList<mfDrugInteraction*> & list, bool fullInfos ) // static
{
     QString tmp, toReturn;
     QList<int> id_di;
     foreach( mfDrugInteraction * di, list ) {
          if ( id_di.contains( di->value( IAM_ID ).toInt() ) )
               continue;
          id_di << di->value( IAM_ID ).toInt();
          tmp += QString( LIST_BASIC_INFO )
                 .arg( di->value( IAM_TYPE ).toString() )
                 .arg( di->value( IAM_MAIN ).toString() )
                 .arg( di->value( IAM_INTERACTOR ).toString() );
          if ( fullInfos ) {
               tmp += QString( LIST_FULL_INFO )
                      .arg( tr( "Text information: " ) )
                      .arg( di->value( IAM_TEXT_IAM ).toString()
                            .replace( "<br>", " " )
                            .replace( "<", "&lt;" )
                            .replace( ">", "&gt;" ) )
                      .arg( tr( "Text 'todo': " ) )
                      .arg( di->value( IAM_TEXT_CAT ).toString()
                            .replace( "<br>", "__" )
                            .replace( "<", "&lt;" )
                            .replace( ">", "&gt;" )
                            .replace( "__", "<br>" ) );
          }
     }
     toReturn.append( QString( LIST_MASK )
                      .arg("10").arg( tr( "Interaction(s) Found : " ) , tmp ) );
     return toReturn;
}

/** \brief Transform a list of interactions to a human readable synthesis Html */
QString mfDrugInteraction::synthesisToHtml( const QList<mfDrugInteraction*> & list, bool fullInfos ) // static
{
     QString tmp, toReturn;
     QList<int> id_di;
     foreach( mfDrugInteraction * di, list ) {
          if ( id_di.contains( di->value( IAM_ID ).toInt() ) )
               continue;
          id_di << di->value( IAM_ID ).toInt();
          tmp += QString( LIST_BASIC_INFO )
                 .arg( di->value( IAM_TYPE ).toString() )
                 .arg( di->value( IAM_MAIN ).toString() )
                 .arg( di->value( IAM_INTERACTOR ).toString() );
          if ( fullInfos ) {
               tmp += QString( LIST_FULL_INFO )
                      .arg( tr( "Text information: " ) )
                      .arg( di->value( IAM_TEXT_IAM ).toString()
                            .replace( "<br>", " " )
                            .replace( "<", "&lt;" )
                            .replace( ">", "&gt;" ) )
                      .arg( tr( "Text 'todo': " ) )
                      .arg( di->value( IAM_TEXT_CAT ).toString()
                            .replace( "<br>", "__" )
                            .replace( "<", "&lt;" )
                            .replace( ">", "&gt;" )
                            .replace( "__", "<br>" ) );
          }
     }
     toReturn.append( QString( LIST_MASK )
                      .arg("10").arg( tr( "Interaction(s) Found : " ) , tmp ) );
     return toReturn;
}
