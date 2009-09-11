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
  \class InteractionsManager
  Interactions can be managed by interactions(), drugHaveInteraction(), getMaximumTypeOfIAM(), getInteractions(),
  getLastIAMFound() and getAllIAMFound().
  You must always in first call interactions() with the list of drugs to test.
  Then you can retreive interactions found using the other members.

  \sa DrugInteraction, mfDrugsBases
  \ingroup freediams drugswidget
*/

#include "mfInteractionsManager.h"

//include drugswidget headers
#include <drugsmodel/mfDrugs.h>
#include <drugsdatabase/mfDrugsBase.h>
#include <drugsmodel/mfDrugInteraction.h>

#include <utils/log.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

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

namespace DrugInteractionConstants {
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

using namespace DrugInteractionConstants;

using namespace Drugs::Internal;

namespace Drugs {
namespace Internal {

/**
  \brief Private part of mfInteractionsManager
  \internal
*/
class InteractionsManagerPrivate
{
public:
    InteractionsManagerPrivate() :
            m_LogChrono(false)
    {
        m_DrugInteractionList.clear();
        m_DrugsList.clear();
    }

    ~InteractionsManagerPrivate()
    {
        qDeleteAll(m_DrugInteractionList);
    }

    QList<DrugInteraction*> getDrugSpecificInteractions(const DrugsData *drug) const
    {
        QList<DrugInteraction*> list;
        foreach(DrugInteraction *i, m_DrugInteractionList) {
            if (i->drugs().contains((DrugsData*)drug))
                list << i;
        }
        return list;
    }

public:
    QList<DrugInteraction*>  m_DrugInteractionList;      /*!< First filled by interactions()*/
    QList<DrugsData *>       m_DrugsList;
    bool                     m_LogChrono;
};

}  // End Internal
}  // End Drugs

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Managing drugs interactions -----------------------------------
//--------------------------------------------------------------------------------------------------------
InteractionsManager::InteractionsManager(QObject *parent) :
        QObject(parent), d(0)
{
    static int handler = 0;
    ++handler;
    d = new InteractionsManagerPrivate();
    setObjectName("InteractionsManager" + QString::number(handler));

}

InteractionsManager::~InteractionsManager()
{
    if (d) delete d;
    d=0;
}

void InteractionsManager::setDrugsList(const QList<DrugsData *> &list)
{
    clearDrugsList();
    d->m_DrugsList = list;
}

void InteractionsManager::addDrug(DrugsData *drug)
{
    d->m_DrugsList.append(drug);
}

void InteractionsManager::removeLastDrug()
{
    d->m_DrugsList.removeLast();
}

void InteractionsManager::clearDrugsList()
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
  of interacting drugs linked to the list of DrugInteraction.\n
*/
bool InteractionsManager::checkInteractions()
{
    QTime t;
    t.start();

    // check all drugs in the list
    bool toReturn = false;

    // clear cached datas
    qDeleteAll(d->m_DrugInteractionList);
    d->m_DrugInteractionList.clear();

    // get interactions list from drugsbase
    d->m_DrugInteractionList = DrugsBase::instance()->calculateInteractions(d->m_DrugsList);

    if (d->m_LogChrono)
        Utils::Log::logTimeElapsed(t, "InteractionsManager", QString("interactions() : %2 drugs")
                                   .arg(d->m_DrugsList.count()) );
    return toReturn;
}

/**
 \brief Return the list of interactions of a selected \e drug. You must in first call interactions(),
        otherwise you will have undesired behavior.
 \sa interactions()
*/
QList<DrugInteraction*> InteractionsManager::getInteractions( const DrugsData *drug ) const
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
Interaction::TypesOfIAM InteractionsManager::getMaximumTypeOfIAM(const DrugsData *drug) const
{
    if ( d->m_DrugInteractionList.isEmpty() )
        return Interaction::noIAM;
    const QList<DrugInteraction*> & list = d->getDrugSpecificInteractions(drug);
    Interaction::TypesOfIAM r;
    foreach( DrugInteraction* di, list )
        r |= di->type();
    return r;
}

/**
 \brief Return true if the \e drug have interactions. You must in first call interactions(),
        otherwise you will have undesired behavior.
 \sa interactions()
*/
bool InteractionsManager::drugHaveInteraction( const DrugsData *drug ) const
{
     if ( d->m_DrugInteractionList.isEmpty() )
          return false;
     return (d->getDrugSpecificInteractions(drug).count() != 0);
}

/**
 \todo Return the last interaction founded after calling interactions().
 \sa interactions()
*/
DrugInteraction *InteractionsManager::getLastInteractionFound() const
{
    qWarning() << d->m_DrugInteractionList << d->m_DrugInteractionList.isEmpty();
     if (!d->m_DrugInteractionList.isEmpty())
          return d->m_DrugInteractionList.at(0);
     return 0;
}

/**
 \brief Returns the list of all interactions founded by interactions()
 \sa interactions()
*/
QList<DrugInteraction *> InteractionsManager::getAllInteractionsFound() const
{
    return d->m_DrugInteractionList;
}


/** \brief Returns the icon of the interaction regarding the \e levelOfWarning for a selected \e drug. */
QIcon InteractionsManager::iamIcon(const DrugsData * drug, const int &levelOfWarning) const
{
    Core::ITheme *th = Core::ICore::instance()->theme();
    if ( drugHaveInteraction(drug) ) {
        Interaction::TypesOfIAM r = getMaximumTypeOfIAM( drug );
        if ( r & Interaction::ContreIndication )
            return th->icon( INTERACTION_ICONCRITICAL );
        else if ( r & Interaction::Deconseille )
            return th->icon( INTERACTION_ICONDECONSEILLEE );
        else if ( ( r & Interaction::APrendreEnCompte ) && ( levelOfWarning <= 1 ) )
            return th->icon( INTERACTION_ICONTAKEINTOACCOUNT );
        else if ( ( r & Interaction::Precaution ) && ( levelOfWarning <= 1 ) )
            return th->icon( INTERACTION_ICONPRECAUTION );
        else if ( ( r & Interaction::Information ) && ( levelOfWarning == 0 ) )
            return th->icon( INTERACTION_ICONINFORMATION );
        else if ( r & Interaction::noIAM )
            return th->icon( INTERACTION_ICONOK );
        else
            return th->icon( INTERACTION_ICONUNKONW );
    } else if ( levelOfWarning <= 1 ) {
        if ( ! DrugsBase::instance()->drugsINNIsKnown( drug ) )
            return th->icon( INTERACTION_ICONUNKONW );
        else return th->icon( INTERACTION_ICONOK );
    }
    return QIcon();
}

/** \brief Transforms a list of interactions to human readable Html (static). */
QString InteractionsManager::listToHtml( const QList<DrugInteraction*> & list, bool fullInfos ) // static
{
     QString tmp, toReturn;
     QList<int> id_di;
     foreach( DrugInteraction * di, list ) {
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
QString InteractionsManager::synthesisToHtml( const QList<DrugInteraction*> & list, bool fullInfos ) // static
{
     QString tmp, toReturn;
     QList<int> id_di;
     foreach( DrugInteraction * di, list ) {
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
