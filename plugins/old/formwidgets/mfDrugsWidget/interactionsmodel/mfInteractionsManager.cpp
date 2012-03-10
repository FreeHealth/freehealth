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
  \class mfInteractionsManager
  Interactions can be managed by interactions(), drugHaveInteraction(), getMaximumTypeOfIAM(), getInteractions(),
  getLastIAMFound() and getAllIAMFound().
  You must always in first call interactions() with the list of drugs to test.
  Then you can retreive interactions found using the other members.

  \sa mfDrugInteraction, mfDrugsBases
  \ingroup freediams drugswidget
*/

#include "mfInteractionsManager.h"

//include drugswidget headers
#include <drugsmodel/mfDrugs.h>
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugInteraction.h>

// include toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkTheme.h>

// include Qt headers
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QFile>
#include <QDir>
#include <QMultiHash>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QSet>

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



/**
  \brief Private part of mfInteractionsManager
  \internal
*/
class mfInteractionsManagerPrivate
{
public:
    mfInteractionsManagerPrivate() :
            m_LogChrono(false)
    {
    }

    ~mfInteractionsManagerPrivate()
    {
        qDeleteAll(m_DrugInteractionList);
    }

    QList<mfDrugInteraction*> getDrugSpecificInteractions(const mfDrugs *drug) const
    {
        QList<mfDrugInteraction*> list;
        foreach(mfDrugInteraction *i, m_DrugInteractionList) {
            if (i->drugs().contains((mfDrugs*)drug))
                list << i;
        }
        return list;
    }

public:
    QList<mfDrugInteraction*>  m_DrugInteractionList;      /*!< First filled by interactions()*/
    QList<mfDrugs*>            m_DrugsList;
    bool                       m_LogChrono;
};

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Managing drugs interactions -----------------------------------
//--------------------------------------------------------------------------------------------------------
mfInteractionsManager::mfInteractionsManager(QObject *parent) :
        QObject(parent), d(0)
{
    static int handler = 0;
    ++handler;
    d = new mfInteractionsManagerPrivate();
    setObjectName("mfInteractionsManager_" + QString::number(handler));
    tkLog::addMessage(this, "instance created");

}

mfInteractionsManager::~mfInteractionsManager()
{
    if (d) delete d;
    d=0;
}

void mfInteractionsManager::setDrugsList(const QList<mfDrugs*> &list)
{
    clearDrugsList();
    d->m_DrugsList = list;
}

void mfInteractionsManager::addDrug(mfDrugs *drug)
{
    d->m_DrugsList.append(drug);
}

void mfInteractionsManager::removeLastDrug()
{
    d->m_DrugsList.removeLast();
}

void mfInteractionsManager::clearDrugsList()
{
    d->m_DrugsList.clear();
     // clear cached datas
     qDeleteAll(d->m_DrugInteractionList);
     d->m_DrugInteractionList.clear();
}

/**
  \brief Return the interaction's state of a prescription represented by a list of mfDrugs \e drugs.
  Clear all actual interactions found.\n
  Check interactions drug by drug \sa mfDrugsBasePrivate::checkDrugInteraction()\n
  Retreive all interactions from database \sa mfDrugsBase::getAllInteractionsFound()\n
  Test all substances of drugs and all iammol and classes and create a cached QMap containing the CIS
  of interacting drugs linked to the list of mfDrugInteraction.\n
*/
bool mfInteractionsManager::checkInteractions()
{
     QTime t;
     t.start();

     // check all drugs in the list
     bool toReturn = false;

     // clear cached datas
     qDeleteAll(d->m_DrugInteractionList);
     d->m_DrugInteractionList.clear();

     // get interactions list from drugsbase
     d->m_DrugInteractionList = mfDrugsBase::instance()->calculateInteractions(d->m_DrugsList);

     if (d->m_LogChrono)
         tkLog::logTimeElapsed(t, "mfInteractionsManager", QString("interactions() : %2 drugs")
                           .arg(d->m_DrugsList.count()) );
     return toReturn;
}

/**
 \brief Return the list of interactions of a selected \e drug. You must in first call interactions(),
        otherwise you will have undesired behavior.
 \sa interactions()
*/
QList<mfDrugInteraction*> mfInteractionsManager::getInteractions( const mfDrugs * drug ) const
{
     if ( d->m_DrugInteractionList.isEmpty() )
          return d->m_DrugInteractionList;
     return d->getDrugSpecificInteractions(drug);
}

/**
 \brief Return the maximum interaction type for a selected \e drug. You must in first call interactions(),
        otherwise you will have undesired behavior.
 \sa interactions()
*/
Interaction::TypesOfIAM mfInteractionsManager::getMaximumTypeOfIAM(const mfDrugs * drug) const
{
     if ( d->m_DrugInteractionList.isEmpty() )
          return Interaction::noIAM;
     const QList<mfDrugInteraction*> & list = d->getDrugSpecificInteractions(drug);
     Interaction::TypesOfIAM r;
     foreach( mfDrugInteraction* di, list )
         r |= di->type();
     return r;
}

/**
 \brief Return true if the \e drug have interactions. You must in first call interactions(),
        otherwise you will have undesired behavior.
 \sa interactions()
*/
bool mfInteractionsManager::drugHaveInteraction( const mfDrugs *drug ) const
{
     if ( d->m_DrugInteractionList.isEmpty() )
          return false;
     return (d->getDrugSpecificInteractions(drug).count() != 0);
}

/**
 \todo Return the last interaction founded after calling interactions().
 \sa interactions()
*/
mfDrugInteraction *mfInteractionsManager::getLastInteractionFound() const
{
     if (!d->m_DrugInteractionList.isEmpty() )
          return d->m_DrugInteractionList.at(0);
     return 0;
}

/**
 \brief Returns the list of all interactions founded by interactions()
 \sa interactions()
*/
QList<mfDrugInteraction*> mfInteractionsManager::getAllInteractionsFound() const
{
    return d->m_DrugInteractionList;
}


/** \brief Returns the icon of the interaction regarding the \e levelOfWarning for a selected \e drug. */
QIcon mfInteractionsManager::iamIcon(const mfDrugs * drug, const int & levelOfWarning) const
{
    if ( drugHaveInteraction(drug) ) {
        Interaction::TypesOfIAM r = getMaximumTypeOfIAM( drug );
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
    } else if ( levelOfWarning <= 1 ) {
        if ( ! mfDrugsBase::instance()->drugsINNIsKnown( drug ) )
            return tkTheme::icon( INTERACTION_ICONUNKONW );
        else return tkTheme::icon( INTERACTION_ICONOK );
    }
    return QIcon();
}

/** \brief Transforms a list of interactions to human readable Html (static). */
QString mfInteractionsManager::listToHtml( const QList<mfDrugInteraction*> & list, bool fullInfos ) // static
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
                      .arg( tr( "Nature of the risk: " ) )
                      .arg( di->value( IAM_TEXT_IAM ).toString()
                            .replace( "<br>", " " )
                            .replace( "<", "&lt;" )
                            .replace( ">", "&gt;" ) )
                      .arg( tr( "What to do: " ) )
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
QString mfInteractionsManager::synthesisToHtml( const QList<mfDrugInteraction*> & list, bool fullInfos ) // static
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
                      .arg( tr( "Nature of the risk: " ) )
                      .arg( di->value( IAM_TEXT_IAM ).toString()
                            .replace( "<br>", " " )
                            .replace( "<", "&lt;" )
                            .replace( ">", "&gt;" ) )
                      .arg( tr( "What to do: " ) )
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
