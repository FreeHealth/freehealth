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
  \class mfInteractionsBase
  \brief This class owns the interactions mechanisms and is inherited by mfDrugsBase.

  Interactions can be managed by interactions(), drugHaveInteraction(), getMaximumTypeOfIAM(), getInteractions(),
  getLastIAMFound() and getAllIAMFound().
  You must always in first call interactions() with the list of drugs to test.
  Then you can retreive interactions found using the other members.

  \ingroup freediams drugswidget
*/

#include "mfInteractionsBase.h"

//include drugswidget headers
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDrugInteraction.h>

// include toolkit headers
#include <tkLog.h>

// include Qt headers
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QMap>
#include <QMultiMap>
#include <QSet>

using namespace mfDrugsConstants;
using namespace mfDosagesConstants;
using namespace mfInteractionsConstants;

/**
  \brief Private part of mfDrugsBase
  \internal
*/
class mfInteractionsBasePrivate
{
public:
    mfInteractionsBasePrivate(mfInteractionsBase *p) :
            m_Parent(p), m_LogChrono(false), m_initialized(false) {}

    ~mfInteractionsBasePrivate()
    {
    }

    bool checkDrugInteraction( mfDrugs * drug, const QList<mfDrugs*> & drugs );
    mfDrugInteraction * getInteractionFromDatabase( const int & _id1, const int & _id2 );
    QList<mfDrugInteraction*> getAllInteractionsFound();

public:
    mfInteractionsBase   *m_Parent;
    QMap<int, int>        m_Iams;                   /*!<  All possible interactions based on Iam_Ids */
    QMultiMap< int, int>  m_IamFound;               /*!< modified by checkDrugInteraction() */
    bool                  m_LogChrono;
    bool                  m_initialized;
};


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Initializing Database -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
   \brief Constructor.
   \private
*/
mfInteractionsBase::mfInteractionsBase(QObject *parent)
        : tkDatabase(parent), d_interactions(0)
{
    d_interactions = new mfInteractionsBasePrivate(this);
}

/** \brief Destructor. */
mfInteractionsBase::~mfInteractionsBase()
{
    if (d_interactions) delete d_interactions;
    d_interactions=0;
}

/** \brief Initializer for the database. Return the error state. */
bool mfInteractionsBase::init()
{
    // only one base can be initialized
    if ( d_interactions->m_initialized )
        return true;

     // retreive iams into m_Iams for speed improvments
    QSqlDatabase DB = QSqlDatabase::database( DRUGS_DATABASE_NAME );
    if ( !DB.isOpen() )
        DB.open();
    QList<int> fields;
    fields << IAM_ID1 << IAM_ID2;
    QString req = select( Table_IAM, fields );
    QSqlQuery q(req , DB);
    if (q.isActive())
        while (q.next())
            d_interactions->m_Iams.insertMulti( q.value(0).toInt(), q.value(1).toInt());
    d_interactions->m_initialized = true;
    return true;
}

bool mfInteractionsBase::isInitialized() const
{
     return d_interactions->m_initialized;
}

/**
  \brief This is for debugging purpose. Log timers for some crucial functions.
  \sa checkInteractions(), getDrugsByCIS()
*/
void mfInteractionsBase::logChronos( bool state )
{
    d_interactions->m_LogChrono = state;
}

/** \brief Return the interaction's state of a \e drug when prescribed in association with \e drugList. */
bool mfInteractionsBasePrivate::checkDrugInteraction( mfDrugs * drug, const QList<mfDrugs*> & drugsList )
{
     QTime t;
     t.start();

     if (drug->numberOfInn() == 0)
         return false;

     const QSet<int> &drug_iams = drug->allInnAndIamClasses();
     QSet<int> d_iams;
     foreach( mfDrugs * drug2, drugsList ) {
          if ( drug2 == drug )
              continue;
          foreach( const int i, drug2->allInnAndIamClasses()) {
              if (d_iams.contains(i))
                  continue;
              d_iams << i;
          }
     }

     foreach( int s, d_iams ) {
         foreach( int s2, drug_iams )  {
             // foreach iam subst/class test existing interactions with *drug
             if ( m_Iams.keys( s ).contains( s2 ) )
                 if ( ( ! m_IamFound.contains( s2, s ) ) && ( ! m_IamFound.contains( s, s2 ) ) )
                     m_IamFound.insertMulti( s2, s );

//             Not necessary because interactions are "mirrored" in the database
//             if ( m_Iams.keys( s2 ).contains( s ) )
//                 if ( ( ! m_IamFound.contains( s2, s ) ) && ( ! m_IamFound.contains( s, s2 ) ) )
//                     m_IamFound.insertMulti( s, s2 );

             // test same molecules
             if ( ( s > 999 ) && ( s2 > 999 ) && ( s == s2 ) )
                 if ( ! m_IamFound.contains( s, -1 ) )
                     m_IamFound.insertMulti( s, -1 );
         }
     }

     if (m_LogChrono)
         tkLog::logTimeElapsed(t, "mfDrugsBase", QString("checkDrugInteraction : %1 ; %2")
                           .arg(drug->denomination()).arg(drugsList.count()) );

//      qWarning() << "checkDrugInteraction" << m_IamFound;
     if ( m_IamFound.count() != 0 )
         return true;
     return false;
}

/**
  \brief Return the interaction's state of a prescription represented by a list of mfDrugs \e drugs.
  Clear all actual interactions found.\n
  Check interactions drug by drug \sa mfDrugsBasePrivate::checkDrugInteraction()\n
  Retreive all interactions from database \sa mfDrugsBase::getAllInteractionsFound()\n
  Test all substances of drugs and all iammol and classes and create a cached QMap containing the CIS
  of interacting drugs linked to the list of mfDrugInteraction.\n
*/
QList<mfDrugInteraction*> mfInteractionsBase::calculateInteractions( const QList<mfDrugs*> & drugs )
{
     QTime t;
     t.start();

     QList<mfDrugInteraction*> toReturn;
     d_interactions->m_IamFound.clear();

     // check interactions drug by drug --> stored into d_interactions->m_IamFound
     foreach( mfDrugs * drug, drugs )
          d_interactions->checkDrugInteraction( drug, drugs );

     // prepare cached datas
     toReturn = d_interactions->getAllInteractionsFound();

     int id1, id2;
     // for each known drug interaction
     foreach( mfDrugInteraction* di, toReturn) {
         id1 = di->value( IAM_ID1 ).toInt();
         id2 = di->value( IAM_ID2 ).toInt();
         // test all drugs in the list
         foreach( mfDrugs * drg, drugs )  {
             if (drg->allInnAndIamClasses().contains(id1) || drg->allInnAndIamClasses().contains(id2)) {
                     di->addInteractingDrug(drg);
             }
         }
     }
     if (d_interactions->m_LogChrono)
         tkLog::logTimeElapsed(t, "mfInteractionsBase", QString("interactions() : %2 drugs")
                               .arg(drugs.count()) );

     return toReturn;
}

/**
 \brief Retrieve from the database the interaction for ids : \e _id1 and \e _id2
 \sa interactions()
*/
mfDrugInteraction * mfInteractionsBasePrivate::getInteractionFromDatabase( const int & _id1, const int & _id2 )
{
    int id2 = _id2;
    QSqlDatabase DB = QSqlDatabase::database( DRUGS_DATABASE_NAME );
    if ( !DB.isOpen() )
        DB.open();

    mfDrugInteraction * di = 0;

    // first test if IAM is an alert (value == -1)
    if ( id2 == -1 ) {
        di = new mfDrugInteraction();
        di->setValue( IAM_TYPE , Interaction::Information );
        di->setValue( IAM_ID1 , _id1 );
        di->setValue( IAM_ID2 , _id1 );
        di->setValue( IAM_TEXT_IAM , QCoreApplication::translate( "mfDrugsBase", "This INN is present more than one time in this prescrition." )  );
        id2 = _id1;
    } else {
        // else retreive IAM from database
        // construct where clause
        QHashWhere where;
        where.insert( IAM_ID1 , QString( "=%1" ).arg( _id1 ) );
        where.insert( IAM_ID2 , QString( "=%1" ).arg( _id2 ) );

        // get IAM table
        QString req = m_Parent->select( Table_IAM, where );
        {
            QSqlQuery q( req , DB );
            if ( q.isActive() ) {
                if ( q.next() ) {
                    di = new mfDrugInteraction();
                    int i;
                    for ( i = 0; i < IAM_MaxParam; ++i )
                        di->setValue( i, q.value( i ) );
                }
            } else
                tkLog::addQueryError( "mfInteractionsBase", q );
        }
    }

    // get denomination from iam_denomination where ID1 ID2
    di->setValue( IAM_MAIN, m_Parent->getInnDenomination(_id1));
    di->setValue( IAM_INTERACTOR, m_Parent->getInnDenomination(id2));
    
    return di;
}

/**
 \brief Returns the list of all interactions founded by interactions()
 \sa interactions()
*/
QList<mfDrugInteraction*> mfInteractionsBasePrivate::getAllInteractionsFound()
{
     // if no interactions were found : return empty list
     QList<mfDrugInteraction*> toReturn;
     if ( m_IamFound.isEmpty() )
          return toReturn;

     QSqlDatabase DB = QSqlDatabase::database(DRUGS_DATABASE_NAME);
     if ( !DB.isOpen() )
          DB.open();

     QMap<int, int>::const_iterator i = m_IamFound.constBegin();
     while ( i != m_IamFound.constEnd() ) {
          toReturn << getInteractionFromDatabase( i.key(), i.value() );
          ++i;
     }
     return toReturn;
}
