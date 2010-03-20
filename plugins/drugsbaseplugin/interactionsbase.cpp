/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
  \class InteractionsBase
  \brief This class owns the interactions mechanisms and is inherited by mfDrugsBase.

  Interactions can be managed by interactions(), drugHaveInteraction(), getMaximumTypeOfIAM(), getInteractions(),
  getLastIAMFound() and getAllIAMFound().
  You must always in first call interactions() with the list of drugs to test.
  Then you can retreive interactions found using the other members.

  \ingroup freediams drugswidget
*/

#include "interactionsbase.h"

#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsinteraction.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>

// include Qt headers
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QMap>
#include <QMultiMap>
#include <QMultiHash>
#include <QSet>
#include <QFile>
#include <QDir>

using namespace DrugsDB::Constants;
using namespace DrugsDB::Internal;

namespace DrugsDB {
namespace Internal {

    const char * const SEPARATOR = "|||";

/**
  \brief Private part of mfDrugsBase
  \internal
*/
class InteractionsBasePrivate
{
public:
    InteractionsBasePrivate(InteractionsBase *p) :
            m_Parent(p), m_DB(0), m_LogChrono(false), m_initialized(false) {}

    ~InteractionsBasePrivate()
    {
        if (m_DB) {
            delete m_DB;
            m_DB=0;
        }
    }

    // Link tables
    /**
      \brief Retrieve many values into cached lists.
      Get the link tables that are protected from the SVN diffusion.\n
      Retrieve all iam denomination for speed improvments. \n
      Retrieve all iamids from IAM_EXPORT fro speed improvments.\n
    */
    void retreiveLinkTables()
    {
         if ((!m_Lk_iamCode_substCode.isEmpty()) && (!m_Lk_classCode_iamCode.isEmpty()))
             return;

         QString tmp;
         {
              QFile file(":/inns_molecules_link.csv");
              if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                   return;
              tmp = file.readAll();
              QStringList lines = tmp.split("\n");
              int i = 0;
              foreach(QString l, lines) {
                   if (!l.contains(SEPARATOR)) continue;
                   QStringList val = l.split(SEPARATOR);
                   m_Lk_iamCode_substCode.insertMulti(val[0].toInt(), val[1].toInt());
                   i++;
              }
         }

         {
              QFile file(":/link-class-substances.csv");
              if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                   return;
              tmp = file.readAll();
              QStringList lines = tmp.split("\n");
              int i = 0;
              foreach(QString l, lines) {
                   if (!l.contains(SEPARATOR)) continue;
                   QStringList val = l.split(SEPARATOR);
                   m_Lk_classCode_iamCode.insertMulti(val[0].toInt(), val[1].toInt());
                   i++;
              }
         }
         InteractionsBase::m_InteractionsDatabaseAvailable = m_Lk_classCode_iamCode.count() && m_Lk_iamCode_substCode.count();

         /** \todo release these resources files to limit memory usage */

         QSqlDatabase DB = QSqlDatabase::database(IAM_DATABASE_NAME);
         if (!DB.isOpen())
              DB.open();

         // get m_IamDenominations : all iam denominations known
         {
             QHashWhere where;
             QString req = m_DB->select(Table_IAM_DENOMINATION);

             QSqlQuery q(req , DB);
             if (q.isActive()) {
                 while (q.next())
                     m_IamDenominations.insert(q.value(0).toInt(), q.value(1).toString());
             }
             else
                 Utils::Log::addQueryError("DrugsBase", q);
         }

         /*
              // get m_Lk_iamCode_substCode : link table for iamCode <-> codeSubst
              {
                   QString req = "SELECT `ID_IAM_SUBST`, `ID_CODE_SUBST` "
                                 " FROM `IAM_SUBST_L_CODE_SUBST` "
                                 " ORDER BY `ID_IAM_SUBST`;";

                   QSqlQuery q(req , DB);
                   if (q.isActive())
                   {
                        while (q.next())
                             m_Lk_iamCode_substCode.insertMulti(q.value(0).toInt(), q.value(1).toInt());
                   }
                   else
                        qWarning() << q.lastError().driverText() << q.lastError().databaseText() << q.lastQuery();
              }


              // get m_Lk_classCode_iamCode : link table for iamCode <-> codeSubst
              {
                   QString req = "SELECT `ID_IAM_CLASS`, `ID_IAM_SUBST` "
                                 "FROM `IAM_CLASS_L_SUBST` "
                                 "ORDER BY `ID_IAM_CLASS`;";

                   QSqlQuery q(req , DB);
                   if (q.isActive())
                   {
                        while (q.next())
                             m_Lk_classCode_iamCode.insertMulti(q.value(0).toInt(), q.value(1).toInt());
                   }
                   else
                        qWarning() << q.lastError().driverText() << q.lastError().databaseText() << q.lastQuery();
              }
         */
     }



    bool checkDrugInteraction( DrugsData *drug, const QList<DrugsData *> & drugs );
    DrugsInteraction * getInteractionFromDatabase( const int & _id1, const int & _id2 );
    QList<DrugsInteraction*> getAllInteractionsFound();

public:
    InteractionsBase     *m_Parent;
    Utils::Database      *m_DB;
    QMap<int, int>        m_Iams;                   /*!<  All possible interactions based on Iam_Ids */
    QMultiMap< int, int>  m_IamFound;               /*!< modified by checkDrugInteraction() */
    bool                  m_LogChrono;
    bool                  m_initialized;

    // These variables are used or speed improvments and database protection
    QHash<int, QString>       m_IamDenominations;       /*!< INN and class denominations */
    QMultiHash< int, int >    m_Lk_iamCode_substCode;   /*!< Link Iam_Id to Code_Subst */
    QMultiHash< int, int >    m_Lk_classCode_iamCode;   /*!< Link ClassIam_Id to Iam_Id */

};

}  // End Internal
}  // End Drugs


bool InteractionsBase::m_InteractionsDatabaseAvailable = false;


//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Initializing Database -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
   \brief Constructor.
   \private
*/
InteractionsBase::InteractionsBase()
        : di(0)
{
    di = new InteractionsBasePrivate(this);
    di->m_DB = new Utils::Database();

    di->m_DB->addTable(Table_IAM, "IAM_IMPORT");
    di->m_DB->addTable(Table_IAM_DENOMINATION, "IAM_DENOMINATION");

    di->m_DB->addField(Table_IAM_DENOMINATION, IAM_DENOMINATION_ID, "ID_DENOMINATION");
    di->m_DB->addField(Table_IAM_DENOMINATION, IAM_DENOMINATION,    "DENOMINATION");

    di->m_DB->addField(Table_IAM, IAM_ID,       "IAM_ID");
    di->m_DB->addField(Table_IAM, IAM_ID1,      "ID1");
    di->m_DB->addField(Table_IAM, IAM_ID2,      "ID2");
    di->m_DB->addField(Table_IAM, IAM_TYPE,     "TYPE");
    di->m_DB->addField(Table_IAM, IAM_TEXT_IAM, "TEXT_IAM");
    di->m_DB->addField(Table_IAM, IAM_TEXT_CAT, "TEXT_CAT");
}

/** \brief Destructor. */
InteractionsBase::~InteractionsBase()
{
    if (di) {
        delete di;
        di=0;
    }
}

/** \brief Initializer for the database. Return the error state. */
bool InteractionsBase::init()
{
    // only one base can be initialized
    if (di->m_initialized)
        return true;

    QString pathToDb = "";

    if (Utils::isRunningOnMac())
        pathToDb = Core::ICore::instance()->settings()->databasePath() + QDir::separator() + QString(DRUGS_DATABASE_NAME);
    else
        pathToDb = Core::ICore::instance()->settings()->databasePath() + QDir::separator() + QString(DRUGS_DATABASE_NAME);

    di->m_DB->createConnection(IAM_DATABASE_NAME, IAM_DATABASE_FILENAME, pathToDb,
                               Utils::Database::ReadOnly, Utils::Database::SQLite);

    // retreive iams into m_Iams for speed improvements
    if (!di->m_DB->database().isOpen())
        if (!di->m_DB->database().open())
            Utils::Log::addError("InteractionsBase", QString("Unable to open database. Error : %1").arg(di->m_DB->database().lastError().text()));

    QList<int> fields;
    fields << IAM_ID1 << IAM_ID2;
    QString req = di->m_DB->select(Table_IAM, fields);
    QSqlQuery q(req , di->m_DB->database());
    if (q.isActive())
        while (q.next())
            di->m_Iams.insertMulti( q.value(0).toInt(), q.value(1).toInt());

    // retreive links tables for speed improvements
    di->retreiveLinkTables();

    di->m_initialized = true;
    return true;
}

bool InteractionsBase::isInitialized() const
{
     return di->m_initialized;
}

/**
  \brief This is for debugging purpose. Log timers for some crucial functions.
  \sa checkInteractions(), getDrugsByCIS()
*/
void InteractionsBase::logChronos(bool state)
{
    di->m_LogChrono = state;
}

QString InteractionsBase::iamTable(const int ref) const
{
    return di->m_DB->table(ref);
}

QString InteractionsBase::getIamWhereClause(const int & tableref, const QHash<int, QString> & conditions) const
{
    return di->m_DB->getWhereClause(tableref, conditions);
}

/** \brief Return the interaction's state of a \e drug when prescribed in association with \e drugList. */
bool InteractionsBasePrivate::checkDrugInteraction( DrugsData *drug, const QList<DrugsData *> &drugsList )
{
     QTime t;
     t.start();

     if (drug->numberOfInn() == 0)
         return false;

     const QSet<int> &drug_iams = drug->allInnAndIamClasses();
     QSet<int> d_iams;
     foreach( DrugsData * drug2, drugsList ) {
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
         Utils::Log::logTimeElapsed(t, "mfDrugsBase", QString("checkDrugInteraction : %1 ; %2")
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
QList<DrugsInteraction*> InteractionsBase::calculateInteractions( const QList<DrugsData *> &drugs )
{
     QTime t;
     t.start();

     QList<DrugsInteraction*> toReturn;
     di->m_IamFound.clear();

     // check interactions drug by drug --> stored into di->m_IamFound
     foreach( DrugsData *drug, drugs )
          di->checkDrugInteraction( drug, drugs );

     // prepare cached datas
     toReturn = di->getAllInteractionsFound();

     int id1, id2;
     // for each known drug interaction
     foreach( DrugsInteraction* di, toReturn) {
         id1 = di->value( IAM_ID1 ).toInt();
         id2 = di->value( IAM_ID2 ).toInt();
         // test all drugs in the list
         foreach( DrugsData * drg, drugs )  {
             if (drg->allInnAndIamClasses().contains(id1) || drg->allInnAndIamClasses().contains(id2)) {
                     di->addInteractingDrug(drg);
             }
         }
     }
     if (di->m_LogChrono)
         Utils::Log::logTimeElapsed(t, "mfInteractionsBase", QString("interactions() : %2 drugs")
                               .arg(drugs.count()) );

     return toReturn;
}

/**
 \brief Retrieve from the database the interaction for ids : \e _id1 and \e _id2
 \sa interactions()
*/
DrugsInteraction *InteractionsBasePrivate::getInteractionFromDatabase( const int & _id1, const int & _id2 )
{
    int id2 = _id2;
    QSqlDatabase DB = m_DB->database();
    if (!DB.isOpen())
        DB.open();

    DrugsInteraction *dint = 0;

    // first test if IAM is an alert (value == -1)
    if ( id2 == -1 ) {
        dint = new DrugsInteraction();
        dint->setValue( IAM_TYPE , Interaction::Information );
        dint->setValue( IAM_ID1 , _id1 );
        dint->setValue( IAM_ID2 , _id1 );
        dint->setValue( IAM_TEXT_IAM , QCoreApplication::translate( "DrugsBase", "This INN is present more than one time in this prescrition." )  );
        id2 = _id1;
    } else {
        // else retreive IAM from database
        // construct where clause
        QHashWhere where;
        where.insert( IAM_ID1 , QString( "=%1" ).arg( _id1 ) );
        where.insert( IAM_ID2 , QString( "=%1" ).arg( _id2 ) );

        // get IAM table
        QString req = m_DB->select( Table_IAM, where );
        {
            QSqlQuery q( req , DB );
            if ( q.isActive() ) {
                if ( q.next() ) {
                    dint = new DrugsInteraction();
                    int i;
                    for ( i = 0; i < IAM_MaxParam; ++i )
                        dint->setValue( i, q.value( i ) );
                }
            } else
                Utils::Log::addQueryError( "mfInteractionsBase", q );
        }
    }

    // get denomination from iam_denomination where ID1 ID2
    dint->setValue(IAM_MAIN, m_Parent->getInnDenomination(_id1));
    dint->setValue(IAM_INTERACTOR, m_Parent->getInnDenomination(id2));
    
    return dint;
}

/**
 \brief Returns the list of all interactions founded by interactions()
 \sa interactions()
*/
QList<DrugsInteraction*> InteractionsBasePrivate::getAllInteractionsFound()
{
     // if no interactions were found : return empty list
     QList<DrugsInteraction*> toReturn;
     if ( m_IamFound.isEmpty() )
          return toReturn;

     QSqlDatabase DB = QSqlDatabase::database(DRUGS_DATABASE_NAME);
     if (!DB.isOpen())
          DB.open();

     QMap<int, int>::const_iterator i = m_IamFound.constBegin();
     while (i != m_IamFound.constEnd()) {
          toReturn << getInteractionFromDatabase(i.key(), i.value());
          ++i;
     }
     return toReturn;
}

QString InteractionsBase::getInnDenomination(const int inncode) const
{
     return di->m_IamDenominations.value(inncode);
}

/** \brief Returns the name of the INN for the substance code \e code_subst. */
QString InteractionsBase::getInnDenominationFromSubstanceCode(const int code_subst) const
{
     // if molecule is not associated with a dci exit
     if (!di->m_Lk_iamCode_substCode.values().contains(code_subst))
          return QString::null;
     return di->m_IamDenominations.value(di->m_Lk_iamCode_substCode.key(code_subst));
}


/** \brief Return the Inn code linked to the molecule code. Returns -1 if no inn is linked to that molecule. */
int InteractionsBase::getInnCodeForCodeMolecule(const int code) const
{
    if (di->m_Lk_iamCode_substCode.values().contains(code))
        return di->m_Lk_iamCode_substCode.key(code);
    return -1;
}

/** \brief Return the list of the code of the substances linked to the INN code \e code_iam. */
QList<int> InteractionsBase::getLinkedCodeSubst(QList<int> &code_iam) const
{
    QList<int> toReturn;
    foreach(int i, code_iam)
        toReturn << di->m_Lk_iamCode_substCode.values(i);
    return toReturn;
}

/** \brief Return the list of the code of the substances linked to the INN code \e code_iam. */
QList<int> InteractionsBase::getLinkedCodeSubst(const int code_iam) const
{
    return di->m_Lk_iamCode_substCode.values(code_iam);
}

/** \brief Return the list of the INN code linked to the substances code \e code_subst. */
QList<int> InteractionsBase::getLinkedIamCode(QList<int> &code_subst) const
{
    QList<int> toReturn;
    foreach(int i, code_subst)
        toReturn << di->m_Lk_iamCode_substCode.keys(i);
    return toReturn;
}

/** \brief Return the list of the INN code linked to the substances code \e code_subst. */
QList<int> InteractionsBase::getLinkedIamCode(const int code_subst) const
{
    return di->m_Lk_iamCode_substCode.keys(code_subst);
}

/** \brief Retreive from database the Substances Codes where denomination of the INN is like 'iamDenomination%' */
QList<int> InteractionsBase::getLinkedSubstCode(const QString &iamDenomination) const
{
     QSqlDatabase DB = di->m_DB->database();
     if (!DB.isOpen())
          DB.open();

     // get iamSubstCode
     QString tmp = iamDenomination;
     QHash<int, QString> where;
     where.insert(IAM_DENOMINATION, QString("LIKE '%1%'").arg(tmp.replace("'", "?")));
     QList<int> iamCode;
     QString req = di->m_DB->select(Table_IAM_DENOMINATION, IAM_DENOMINATION_ID, where);
     QSqlQuery q(req , di->m_DB->database());
     if (q.isActive())
         while (q.next())
             iamCode << q.value(0).toInt();
     return getLinkedCodeSubst(iamCode);
}

/** \brief Returns the name of the INN for the substance code \e code_subst. */
QStringList InteractionsBase::getIamClassDenomination(const int & code_subst)
{
     // if molecule is not associated with a dci exit
     if (!di->m_Lk_iamCode_substCode.values().contains(code_subst))
          return QStringList();

     // if molecule is not associated with a class exit
     QList<int> list = di->m_Lk_classCode_iamCode.keys(di->m_Lk_iamCode_substCode.key(code_subst));
     if (list.isEmpty())
          return QStringList();
     QStringList toReturn;
     foreach(int i, list)
         toReturn << di->m_IamDenominations.value(i);

     return toReturn;
}

/**
  \brief Returns all INN and IAM classes of MOLECULE \e code_subst.
  \sa getDrugByUID()
*/
QSet<int> InteractionsBase::getAllInnAndIamClassesIndex(const int code_subst)
{
    QSet<int> toReturn;
    toReturn = di->m_Lk_classCode_iamCode.keys(di->m_Lk_iamCode_substCode.key(code_subst)).toSet();
    if (di->m_Lk_iamCode_substCode.values().contains(code_subst))
        toReturn << di->m_Lk_iamCode_substCode.key(code_subst);
    return toReturn;
}
