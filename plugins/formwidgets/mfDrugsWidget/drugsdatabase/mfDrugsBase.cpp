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
  \class mfDrugsBase
  \brief This class owns the drugs and dosages database and interactions mechanism.

  0. Terminology\n
  \e Substances are the chimic molecules that compose a drug.\n
  \e IamCode or \e INN are the INN codes and names.\n
  \e IamClass are the classes that regroups INNs into classes of pharmaceutics family.\n
  \e CIP : presentation code of a drug. A drug can be presented into different presentation (15 pills, 30 pills a box...).\n
  \e CIS : speciality code of a drug. Everything is base on this code.
     One CIS code can be associated to many CIP, many Substances, many INNs, and many IamClasses.

  1. Initialization\n
  This class is pure static, so you can not instanciate it. To initialize datas, just do once : init().
  isInitialized() alerts you of the state of intialization.
  These two members returns true if all is ok.

  2. Drugs retreiver\n
  You can retreive drugs using CIS ou CIP code via getDrugByCIS() and getDrufByCIP().

  3. Drugs Interactions\n
  Interactions can be managed by interactions(), drugHaveInteraction(), getMaximumTypeOfIAM(), getInteractions(),
  getLastIAMFound() and getAllIAMFound().
  You must always in first call interactions() with the list of drugs to test.
  Then you can retreive interactions found using the other members.

  4. Dosages retreiver / saver

  \todo Manage user rights when creating dosage database

  \sa mfDrugInteraction, mfDrugs, mfDrugsTables, mfDrugDosage
  \ingroup drugsinteractions drugswidget
*/

#include "mfDrugsBase.h"

// include freemedforms headers
#ifndef DRUGS_INTERACTIONS_STANDALONE
#    include <mfSettings.h>
#    include <mfCore.h>
#endif

//include drugswidget headers
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDrugInteraction.h>
//#include <drugsmodel/mfDosage.h>
#include <drugswidget/mfDrugInfo.h>

// include toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkSettings.h>

// include Qt headers
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QCoreApplication>
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

/**
  \brief Private part of mfDrugsBase
  \internal
*/
class mfDrugsBasePrivate
{
public:
    mfDrugsBasePrivate( mfDrugsBase * base );
    ~mfDrugsBasePrivate()
    {
        qDeleteAll(m_DrugInteractionList);
    }
    // connections creator
    bool createConnections( const QString & path, const QString & db, const QString & dbName, bool readwrite );

    // Link tables
    void retreiveLinkTables();

    // private members for interactions
    QStringList getIamClassDenomination( const int & code_subst );
    QSet<int> getAllInnAndIamClassesIndex(const int code_subst);
    bool checkDrugInteraction( mfDrugs * drug, const QList<mfDrugs*> & drugs );
    mfDrugInteraction * getIAM( const int & _id1, const int & _id2 );

public:
    // These variables are used or speed improvments and database protection
    QHash<int, QString>       m_IamDenominations;       /*!< INN and class denominations */
    QMultiHash< int, int >    m_Lk_iamCode_substCode;   /*!< Link Iam_Id to Code_Subst */
    QMultiHash< int, int >    m_Lk_classCode_iamCode;   /*!< Link ClassIam_Id to Iam_Id */
    QMap<int, int>            m_Iams;                   /*!<  All possible interactions based on Iam_Ids */
    QMultiMap< int, int>      m_IamFound;               /*!< modified by checkDrugInteraction() */

    // cached datas for speed improvments
    QList<mfDrugInteraction*>             m_DrugInteractionList;      /*!< First filled by interactions()*/
    QMap<int, mfDrugInteraction* >        m_Map_CIS_DrugInteraction;  /*!< idem */
    mfDrugsBase * m_DrugsBase;
    bool m_LogChrono;
};

//--------------------------------------------------------------------------------------------------------
//--------------------------------- Initialization of static members -------------------------------------
//--------------------------------------------------------------------------------------------------------
mfDrugsBase * mfDrugsBase::m_Instance = 0;
bool mfDrugsBase::m_initialized      = false;
const QString mfDrugsBase::separator = "|||";

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Initializing Database -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Returns the unique instance of mfDrugsBase. If it does not exists, it is created */
mfDrugsBase *mfDrugsBase::instance()
{
    if ( ! m_Instance ) {
        m_Instance = new mfDrugsBase(qApp);
        m_Instance->init();
    }
    return m_Instance;
}

/**
   \brief Constructor.
   \private
*/
mfDrugsBase::mfDrugsBase( QObject *parent )
        : tkDatabase( parent ), d(0)
{
    Q_ASSERT_X( tkSettings::instance(), "mfDrugsBase", "You MUST instanciate tkSettings BEFORE mfDrugsBase" );
    d = new mfDrugsBasePrivate( this );
    setObjectName("mfDrugsBase");

    // DRUGS DATABASE
    addTable( Table_CIS, "CIS");
    addTable( Table_COMPO, "COMPO");
    addTable( Table_CIS_CIP, "CIS_CIP");
    addTable( Table_IAM, "IAM_IMPORT" );
    addTable( Table_IAM_DENOMINATION, "IAM_DENOMINATION" );

    addField( Table_CIS, CIS_CIS ,            "CIS" );
    addField( Table_CIS, CIS_DENOMINATION ,   "DENOMINATION");
    addField( Table_CIS, CIS_FORME,           "FORME" );
    addField( Table_CIS, CIS_ADMINISTRATION,  "ADMINISTRATION");
    addField( Table_CIS, CIS_AMM,              "AMM");
    addField( Table_CIS, CIS_AUTORISATION,     "AUTORISATION");
    addField( Table_CIS, CIS_COMMERCIALISATION,"COMMERCIALISATION");
    addField( Table_CIS, CIS_CODE_RPC,         "CODE_RPC");

    addField( Table_CIS_CIP, CISP_CIS,         "CIS");
    addField( Table_CIS_CIP, CISP_CIP,         "CIP");
    addField( Table_CIS_CIP, CISP_LIBELLE,     "LIBELLE");
    addField( Table_CIS_CIP, CISP_STATUT,      "STATUT");
    addField( Table_CIS_CIP, CISP_COMMERCIALISATION, "COMMERCIALISATION");
    addField( Table_CIS_CIP, CISP_DATE,        "DATE_STR");
    addField( Table_CIS_CIP, CISP_CIPLONG,     "CIP_LONG");

    addField( Table_COMPO, COMPO_CIS,          "CIS" );
    addField( Table_COMPO, COMPO_NOM,          "NOM");
    addField( Table_COMPO, COMPO_CODE_SUBST,   "CODE_SUBST");
    addField( Table_COMPO, COMPO_DENOMINATION, "DENOMINATION");
    addField( Table_COMPO, COMPO_DOSAGE,       "DOSAGE" );
    addField( Table_COMPO, COMPO_REF_DOSAGE,   "REF_DOSAGE");
    addField( Table_COMPO, COMPO_NATURE,       "NATURE" );
    addField( Table_COMPO, COMPO_LK_NATURE,    "LK_NATURE");

    addField( Table_IAM_DENOMINATION, IAM_DENOMINATION_ID, "ID_DENOMINATION");
    addField( Table_IAM_DENOMINATION, IAM_DENOMINATION,    "DENOMINATION");

    addField( Table_IAM, IAM_ID,       "IAM_ID");
    addField( Table_IAM, IAM_ID1,      "ID1");
    addField( Table_IAM, IAM_ID2,      "ID2");
    addField( Table_IAM, IAM_TYPE,     "TYPE");
    addField( Table_IAM, IAM_TEXT_IAM, "TEXT_IAM");
    addField( Table_IAM, IAM_TEXT_CAT, "TEXT_CAT");
}

/** \brief Destructor. */
mfDrugsBase::~mfDrugsBase()
{
    if (d) delete d;
    d=0;
}

mfDrugsBasePrivate::mfDrugsBasePrivate( mfDrugsBase * base )
        : m_DrugsBase(base), m_LogChrono(false) {}

/** \brief Initializer for the database. Return the error state. */
bool mfDrugsBase::init()
{
    // only one base can be initialized
    if ( m_initialized )
        return true;

    // test driver
     if ( !QSqlDatabase::isDriverAvailable( "QSQLITE" ) ) {
         tkLog::addError(this, tr("FATAL ERROR : SQLite driver is not available.") ) ;
          return false;
      }

     QString pathToDb = "";

#ifdef DRUGS_INTERACTIONS_STANDALONE
     if (tkGlobal::isRunningOnMac())
         pathToDb = tkSettings::instance()->databasePath() + QDir::separator() + QString(DRUGS_DATABASE_NAME); //QDir::cleanPath( qApp->applicationDirPath() + "/../Resources" );
     else
         pathToDb = tkSettings::instance()->databasePath() + QDir::separator() + QString(DRUGS_DATABASE_NAME); //QDir::cleanPath( qApp->applicationDirPath() );
#else
     pathToDb = QDir::cleanPath( mfCore::settings()->databasePath() + QDir::separator() + QString(DRUGS_DATABASE_NAME) );
#endif

     if (tkGlobal::isDebugCompilation()) {
         // This is the debug mode of drugsinteractions ==> use global_resources
         pathToDb = QDir::cleanPath( QString( "%1/databases/%2/" ).arg( FMF_GLOBAL_RESOURCES ).arg(DRUGS_DATABASE_NAME) );
     }

     tkLog::addMessage(this, tr("Searching databases into dir %1").arg(pathToDb));

     createConnection( DRUGS_DATABASE_NAME, DRUGS_DATABASE_FILENAME, pathToDb,
                       tkDatabase::ReadOnly, tkDatabase::SQLite);
     //  const QString & login = QString::null, const QString & pass = QString::null,
     //  CreationOption createOption = WarnOnly );

     createConnection( DOSAGES_DATABASE_NAME, DOSAGES_DATABASE_FILENAME, pathToDb,
                       tkDatabase::ReadWrite, tkDatabase::SQLite, "log", "pas", tkDatabase::CreateDatabase);

     d->retreiveLinkTables();
     // initialize extra static members of other classes
//     mfDosage::initStaticDatas();
     m_initialized = true;
     return true;
}

/**
  \brief This is for debugging purpose. Log timers for some crucial functions.
  \sa checkInteractions(), getDrugsByCIS()
*/
void mfDrugsBase::logChronos( bool state )
{
    d->m_LogChrono = state;
}

/**
  \brief
  \todo documentation
*/
bool mfDrugsBase::createDatabase(  const QString & connectionName , const QString & dbName,
                                   const QString & pathOrHostName,
                                   TypeOfAccess /*access*/, AvailableDrivers driver,
                                   const QString & /*login*/, const QString & /*pass*/,
                                   CreationOption /*createOption*/
                                   )
{
    // TODO ask user if he wants :
    // 1. an empty dosage base
    // 2. to retreive dosages from internet FMF website
    if ( connectionName != DOSAGES_DATABASE_NAME)
        return false;
    tkLog::addMessage( this, QCoreApplication::translate( "mfDrugsBase", "Trying to create empty database. \nLocation : %1 \nFileName: %2" )
                       .arg( pathOrHostName, dbName ) );
        // create an empty database and connect
    QSqlDatabase DB;
    if ( driver == SQLite ) {
        DB = QSqlDatabase::addDatabase( "QSQLITE" , connectionName );
        DB.setDatabaseName( QDir::cleanPath( pathOrHostName + QDir::separator() + dbName ) );
        DB.open();
    }
    else if ( driver == MySQL ) {
        // TODO : how to create a new mysql database ???
    }

    // create db structure
    // before we need to inform tkDatabase of the connectionName to use
    setConnectionName( connectionName );
    // The SQL scheme MUST BE synchronized with the mfDosageConstants Model Enumerator !!!
    if ( executeSQL(       "CREATE TABLE IF NOT EXISTS `DOSAGE` ("
                           "`POSO_ID`               INTEGER        PRIMARY KEY AUTOINCREMENT,"
                           "`POSO_UUID`             varchar(40)    NULL,"    // put NOT NULL
                           "`INN_LK`                int(11)        NULL,"
                           "`INN_DOSAGE`            varchar(100)   NULL,"    // contains the dosage of the SA INN
                           "`CIS_LK`                int(11)        NULL,"
                           "`CIP_LK`                int(11)        NULL,"
                           "`LABEL`                 varchar(300)   NULL,"    // put NOT NULL

                           "`INTAKEFROM`            double         NULL,"    // put NOT NULL
                           "`INTAKETO`              double         NULL,"
                           "`INTAKEFROMTO`          bool           NULL,"
                           "`INTAKESCHEME`          varchar(200)   NULL,"    // put NOT NULL
                           "`INTAKESINTERVALOFTIME` int(10)        NULL,"
                           "`INTAKESINTERVALSCHEME` varchar(200)   NULL,"

                           "`DURATIONFROM`          double         NULL,"    // put NOT NULL
                           "`DURATIONTO`            double         NULL,"
                           "`DURATIONFROMTO`        bool           NULL,"
                           "`DURATIONSCHEME`        varchar(200)   NULL,"    // put NOT NULL

                           "`PERIOD`                int(10)        NULL,"    // put NOT NULL
                           "`PERIODSCHEME`          varchar(200)   NULL,"    // put NOT NULL
                           "`DAILYSCHEME`           int(10)        NULL,"
                           "`MEALSCHEME`            int(10)        NULL,"

                           "`MINAGE`                int(10)        NULL,"
                           "`MAXAGE`                int(10)        NULL,"
                           "`MINAGEREFERENCE`       int(10)        NULL,"
                           "`MAXAGEREFERENCE`       int(10)        NULL,"
                           "`MINWEIGHT`             int(10)        NULL,"
                           "`SEXLIMIT`              int(10)        NULL,"
                           "`MINCLEARANCE`          int(10)        NULL,"
                           "`MAXCLEARANCE`          int(10)        NULL,"
                           "`PREGNANCYLIMITS`       int(10)        NULL,"
                           "`BREASTFEEDINGLIMITS`   int(10)        NULL,"
                           "`PHYSIOLOGICALLIMITS`   int(10)        NULL,"  // Is this really needed ?

                           "`NOTE`                  varchar(500)   NULL,"

                           "`CIM10_LK`              varchar(150)   NULL,"
                           "`EDRC_LK`               varchar(150)   NULL,"

                           "`EXTRAS`                blob           NULL,"
                           "`USERUUID`              varchar(40)    NULL,"    // put NOT NULL
                           "`USERVALIDATOR`         varchar(40)    NULL,"
                           "`CREATIONDATE`          date           NULL,"    // put NOT NULL
                           "`MODIFICATIONDATE`      date           NULL,"
                           "`TRANSMITTED`           date           NULL,"
                           "`ORDER`                 int(10)        NULL"
                           ");", DB
                           )
        ) {
        tkLog::addMessage( this, tr( "Database %1 %2 correctly created" ).arg( connectionName, dbName ) );
        return true;
    } else {
        tkLog::addError( this, tr("ERROR : database can not be created %1 %2 %3")
                         .arg( connectionName, dbName, DB.lastError().text() ) );
    }
    return false;
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Managing Link Tables -----------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
  \brief Retrieve many values into cached lists.
  Get the link tables that are protected from the SVN diffusion.\n
  Retrieve all iam denomination for speed improvments. \n
  Retrieve all iamids from IAM_EXPORT fro speed improvments.\n
*/
void mfDrugsBasePrivate::retreiveLinkTables()
{
     if ( ( ! m_Lk_iamCode_substCode.isEmpty() ) && ( ! m_Lk_classCode_iamCode.isEmpty() ) )
         return;

     QString tmp;
     {
          QFile file( ":/done.csv" );
          if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
               return;
          tmp = file.readAll();
          QStringList lines = tmp.split( "\n" );
          int i = 0;
          foreach( QString l, lines ) {
               if ( !l.contains( mfDrugsBase::separator ) ) continue;
               QStringList val = l.split( mfDrugsBase::separator );
               m_Lk_iamCode_substCode.insertMulti( val[0].toInt(), val[1].toInt() );
               i++;
          }
     }

     {
          QFile file( ":/link-class-substances.csv" );
          if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
               return;
          tmp = file.readAll();
          QStringList lines = tmp.split( "\n" );
          int i = 0;
          foreach( QString l, lines ) {
               if ( !l.contains( mfDrugsBase::separator ) ) continue;
               QStringList val = l.split( mfDrugsBase::separator );
               m_Lk_classCode_iamCode.insertMulti( val[0].toInt(), val[1].toInt() );
               i++;
          }
     }

     // TODO release these to files from resources to limit memory usage

     QSqlDatabase DB = QSqlDatabase::database( DRUGS_DATABASE_NAME );
     if ( !DB.isOpen() )
          DB.open();

     // get m_IamDenominations : all iam denominations known
     {
         QHashWhere where;
         QString req = m_DrugsBase->select(Table_IAM_DENOMINATION);

         QSqlQuery q( req , DB );
         if ( q.isActive() ) {
             while ( q.next() )
                 m_IamDenominations.insert( q.value(0).toInt(), q.value(1).toString() );
         }
         else
             tkLog::addQueryError( m_DrugsBase, q );
     }
     /*
          // get m_Lk_iamCode_substCode : link table for iamCode <-> codeSubst
          {
               QString req = "SELECT `ID_IAM_SUBST`, `ID_CODE_SUBST` "
                             " FROM `IAM_SUBST_L_CODE_SUBST` "
                             " ORDER BY `ID_IAM_SUBST`;";

               QSqlQuery q( req , DB );
               if ( q.isActive() )
               {
                    while ( q.next() )
                         m_Lk_iamCode_substCode.insertMulti( q.value( 0 ).toInt(), q.value( 1 ).toInt() );
               }
               else
                    qWarning() << q.lastError().driverText() << q.lastError().databaseText() << q.lastQuery();
          }


          // get m_Lk_classCode_iamCode : link table for iamCode <-> codeSubst
          {
               QString req = "SELECT `ID_IAM_CLASS`, `ID_IAM_SUBST` "
                             "FROM `IAM_CLASS_L_SUBST` "
                             "ORDER BY `ID_IAM_CLASS`;";

               QSqlQuery q( req , DB );
               if ( q.isActive() )
               {
                    while ( q.next() )
                         m_Lk_classCode_iamCode.insertMulti( q.value( 0 ).toInt(), q.value( 1 ).toInt() );
               }
               else
                    qWarning() << q.lastError().driverText() << q.lastError().databaseText() << q.lastQuery();
          }
     */

     // retreive iams into m_Iams for speed improvments
     {
          QList<int> fields;
          fields << IAM_ID1 << IAM_ID2;
          QString req = m_DrugsBase->select( Table_IAM, fields );

          QSqlQuery q( req , DB );
          if ( q.isActive() )
               while ( q.next() )
                    m_Iams.insertMulti( q.value( 0 ).toInt(), q.value( 1 ).toInt() );
     }

}

/** \brief Return the list of the code of the substances linked to the INN code \e code_iam. */
QList<int> mfDrugsBase::getLinkedCodeSubst( QList<int> & code_iam )
{
    QList<int> toReturn;
    foreach( int i, code_iam )
        toReturn << d->m_Lk_iamCode_substCode.values( i );
    return toReturn;
}

/** \brief Return the list of the INN code linked to the substances code \e code_subst. */
QList<int> mfDrugsBase::getLinkedIamCode( QList<int> & code_subst )
{
    QList<int> toReturn;
    foreach( int i, code_subst )
        toReturn << d->m_Lk_iamCode_substCode.keys( i );
    return toReturn;
}

/** \brief Retreive from database the Substances Codes where denomination of the INN is like 'iamDenomination%' */
QList<int> mfDrugsBase::getLinkedSubstCode( const QString & iamDenomination )
{
     QSqlDatabase DB = QSqlDatabase::database( DRUGS_DATABASE_NAME );
     if ( !DB.isOpen() )
          DB.open();

     // get iamSubstCode
     QString tmp = iamDenomination;
     QHash<int, QString> where;
     where.insert( IAM_DENOMINATION, QString( "LIKE '%1%'" ).arg(tmp.replace( "'", "?" )) );
     QList<int> iamCode;
     QString req = this->select( Table_IAM_DENOMINATION, IAM_DENOMINATION_ID, where );
     QSqlQuery q( req , DB );
     if ( q.isActive() )
         while ( q.next() )
             iamCode << q.value( 0 ).toInt();
     return getLinkedCodeSubst( iamCode );
}

/** \brief Return true if all the INNs of the drug are known. */
bool mfDrugsBase::drugsINNIsKnown( const mfDrugs * drug )
{
    const QList<QVariant> & list = drug->listOfCodeMolecules().toList(); //->value( Table_COMPO, COMPO_CODE_SUBST ).toList();
    foreach( QVariant q, list)
        if ( d->m_Lk_iamCode_substCode.keys(q.toInt()).count() == 0 )
            return false;
    return true;
}

/** \brief Return the list of equivalents (full drug denomination) if they can be founded. Store the list into the drug. */
QStringList mfDrugsBase::findInnEquivalentsNames( const mfDrugs *drug )
{
//    if (!drugsINNIsKnown(drug))
        return QStringList();
//    if (!drug->innEquivalentsFullNames().isEmpty())
//        return drug->innEquivalentsFullNames().isEmpty();
    /** \todo try to find inn equivalents */
}

//--------------------------------------------------------------------------------------------------------
//----------------------------------------- Retreive drugs from database ---------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Returns the name of the INN for the substance code \e code_subst. */
QString mfDrugsBase::getInnDenominationFromSubstanceCode( const int code_subst )
{
     // if molecule is not associated with a dci exit
     if ( !d->m_Lk_iamCode_substCode.values().contains(code_subst) )
          return QString::null;
     return d->m_IamDenominations.value(d->m_Lk_iamCode_substCode.key(code_subst));
}

QString mfDrugsBase::getInnDenomination( const int inncode )
{
     return d->m_IamDenominations.value(inncode);
}

/** \brief Returns the name of the INN for the substance code \e code_subst. */
QStringList mfDrugsBasePrivate::getIamClassDenomination( const int & code_subst )
{
     // if molecule is not associated with a dci exit
     if ( !m_Lk_iamCode_substCode.values().contains( code_subst ) )
          return QStringList();

     // if molecule is not associated with a class exit
     QList<int> list = m_Lk_classCode_iamCode.keys( m_Lk_iamCode_substCode.key( code_subst ) );
     if ( list.isEmpty() )
          return QStringList();
     QStringList toReturn;
     foreach( int i, list )
         toReturn << m_IamDenominations.value(i);

     return toReturn;
}

/**
  \brief Returns all INN and IAM classes of MOLECULE \e code_subst.
  \sa getDrugByCIS()
*/
QSet<int> mfDrugsBasePrivate::getAllInnAndIamClassesIndex(const int code_subst)
{
    QSet<int> toReturn;
    toReturn = m_Lk_classCode_iamCode.keys( m_Lk_iamCode_substCode.key( code_subst ) ).toSet();
    if (m_Lk_iamCode_substCode.values().contains(code_subst))
        toReturn << m_Lk_iamCode_substCode.key(code_subst);
    return toReturn;
}

/** \brief Return the Inn code linked to the molecule code. Returns -1 if no inn is linked to that molecule. */
int mfDrugsBase::getInnCodeForCodeMolecule(const int code)
{
    if (d->m_Lk_iamCode_substCode.values().contains(code))
        return d->m_Lk_iamCode_substCode.key(code);
    return -1;
}

/** \brief Returns the unique code CIS for the CIP code \e CIP. */
int mfDrugsBase::getCISFromCIP( int CIP )
{
    QSqlDatabase DB = QSqlDatabase::database( DRUGS_DATABASE_NAME );
    if ( !DB.isOpen() )
        DB.open();
    // prepare where clause
    QHash<int, QString> where;
    where.insert( CISP_CIP, "=" + QString::number( CIP ) );
    // prepare query
    QString req = this->select( Table_CIS_CIP, CISP_CIS, where );
    QSqlQuery q( req , DB );
    if ( q.isActive() )
        if ( q.next() )
            return q.value( 0 ).toInt();
    return -1;
}

/** \brief Retrieve and return the drug designed by the CIP code \e CIP_id. This function is not used. */
mfDrugs * mfDrugsBase::getDrugByCIP( const QVariant & CIP_id )
{
    // retreive corresponding CIS
    int CIS = getCISFromCIP( CIP_id.toInt() );
    if ( CIS == -1 ) return 0;

    // retreive drugs from CIS
    return getDrugByCIS( CIS );
}

/** \brief Retrieve and return the drug designed by the CIS code \e CIS_id. */
mfDrugs * mfDrugsBase::getDrugByCIS( const QVariant & CIS_id )
{
     QTime t;
     t.start();

     QSqlDatabase DB = QSqlDatabase::database( DRUGS_DATABASE_NAME );
     if ( !DB.isOpen() )
          DB.open();

     // construct the where clause
     QHash<int, QString> where;
     where.insert( CIS_CIS, QString( "=%1" ).arg( CIS_id.toString() ) );

     // get CIS table
     QString req = select( Table_CIS, where );
     mfDrugs * toReturn = 0;
     {
          QSqlQuery q( req , DB );
          if ( q.isActive() ) {
               if ( q.next() ) {
                    if ( q.record().count() != CIS_MaxParam )
                         tkLog::addError( "mfDrugsBase", QCoreApplication::translate( "mfDrugsBase",
                                   "ERROR : will retreiving %1. Wrong number of fields" )
                                          .arg( "drugs/CIS" ) );
                    int i = 0;
                    toReturn = new mfDrugs();
                    for ( i = 0; i < CIS_MaxParam; ++i )
                         toReturn->setValue( Table_CIS, i, q.value(i) );
               }
          }
      }

     // get COMPO table
     where.clear();
     where.insert( COMPO_CIS, QString( "=%1" ).arg( toReturn->CIS() ) );
     QString sort = QString(" ORDER BY %1 ASC").arg(field(Table_COMPO,COMPO_LK_NATURE));
     req = select( Table_COMPO, where ) + sort;
     QSet<int> codeMols;
     {
         mfDrugComposition *compo = 0;
         mfDrugComposition *precedent = 0;
          QSqlQuery q( req , DB );
          if ( q.isActive() ) {
              QList<mfDrugComposition*> list;
               while ( q.next() ) {
                    if ( q.record().count() != COMPO_MaxParam )
                         tkLog::addError( "mfDrugsBase", QCoreApplication::translate( "mfDrugsBase",
                                   "ERROR : will retreiving %1. Wrong number of fields" )
                                          .arg( "drugs composition" ) );
                    compo = new mfDrugComposition();
                    int i = 0;
                    for ( i = 0; i < COMPO_MaxParam; ++i )
                         compo->setValue( i, q.value(i) );
                    compo->setValue(COMPO_IAM_DENOMINATION, getInnDenominationFromSubstanceCode(compo->m_CodeMolecule) );
                    compo->setValue(COMPO_IAM_CLASS_DENOMINATION, d->getIamClassDenomination(compo->m_CodeMolecule) );
                    compo->setInnCode(getInnCodeForCodeMolecule(q.value(COMPO_CODE_SUBST).toInt()));
                    list << compo;
                    codeMols << q.value(COMPO_CODE_SUBST).toInt();
                    if (precedent) {
                        if (q.value(COMPO_LK_NATURE) == precedent->linkId()) {
                            compo->setLinkedSubstance(list.at(list.count()-2));
                        }
                    }
                    precedent = compo;
               }
               foreach( mfDrugComposition *c, list)
                   toReturn->addComposition(c);
          }
      }
     foreach( const int i, codeMols) {
         toReturn->addInnAndIamClasses( d->getAllInnAndIamClassesIndex(i) ) ;
     }

//     toReturn->warn();

     if (d->m_LogChrono)
         tkLog::logTimeElapsed(t, "mfDrugsBase", "getDrugByCIS");

     return toReturn;
}

//--------------------------------------------------------------------------------------------------------
//---------------------------------------- Managing drugs interactions -----------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Return the interaction's state of a \e drug when prescribed in association with \e drugList. */
bool mfDrugsBasePrivate::checkDrugInteraction( mfDrugs * drug, const QList<mfDrugs*> & drugsList )
{
     QTime t;
     t.start();

     if (drug->numberOfInn() == 0)
         return false;

//     const QList<int> &drug_iams = drug->allInnAndIamClasses();
//     QList<int> d_iams;
     const QSet<int> &drug_iams = drug->allInnAndIamClasses();
     QSet<int> d_iams;
     foreach( mfDrugs * drug2, drugsList ) {
          if ( drug2 == drug )
              continue;
          foreach( const int i, drug2->allInnAndIamClasses()) {
              if (d_iams.contains(i))
                  continue;
              d_iams << i;//drug2->allInnAndIamClasses().toSet();
          }
     }

     foreach( int s, d_iams ) {
         foreach( int s2, drug_iams )  {
             // foreach iam subst/class test existing interactions with *drug
             if ( m_Iams.keys( s ).contains( s2 ) )
                 if ( ( ! m_IamFound.contains( s2, s ) ) && ( ! m_IamFound.contains( s, s2 ) ) )
                     m_IamFound.insertMulti( s2, s );

             if ( m_Iams.keys( s2 ).contains( s ) )
                 if ( ( ! m_IamFound.contains( s2, s ) ) && ( ! m_IamFound.contains( s, s2 ) ) )
                     m_IamFound.insertMulti( s, s2 );

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
bool mfDrugsBase::interactions( const QList<mfDrugs*> & drugs )
{
     QTime t;
     t.start();

     // check all drugs in the list
     // store IAM into cache members
     bool toReturn = false;

     // clear cached datas
     qDeleteAll( d->m_DrugInteractionList );
     d->m_DrugInteractionList.clear();
     d->m_Map_CIS_DrugInteraction.clear();
     d->m_IamFound.clear();

     // check interactions drug by drug
     foreach( mfDrugs * drug, drugs )
          d->checkDrugInteraction( drug, drugs );
//          if (!d->checkDrugInteraction( drug, drugs )
//               toReturn = true;

     // prepare cached datas
     getAllInteractionsFound();
//      qWarning() << "mfDrugsBase::interactions" << "drugs=" << drugs.count()
//      << "IAM=" << m_DrugInteractionList.count() <<  m_IamFound.count();

     int id1, id2, iam_subst;

     // for each known drug interaction
     foreach( mfDrugInteraction* di, d->m_DrugInteractionList ) {
          id1 = di->value( IAM_ID1 ).toInt();
          id2 = di->value( IAM_ID2 ).toInt();
          // test all drugs in the list
          foreach( mfDrugs * drg, drugs )  {
               // test molecule by molecule
              foreach( QVariant code_subst, drg->listOfCodeMolecules().toList()) { // ->value( Table_COMPO, COMPO_CODE_SUBST ).toList() ) {
                    iam_subst = d->m_Lk_iamCode_substCode.key( code_subst.toInt() );
                    const QList<int> & iam_class = d->m_Lk_classCode_iamCode.keys( iam_subst );

                    // test iam_subst
                    if (( id1 == iam_subst ) || ( id2 == iam_subst ) ) {
                        d->m_Map_CIS_DrugInteraction.insertMulti( drg->value( Table_CIS, CIS_CIS ).toInt(), di );
                        break;
                    } else {
                        // test all classes
                        foreach( int c, iam_class ) {
                            if (( id1 == c ) || ( id2 == c ) ) {
                                d->m_Map_CIS_DrugInteraction.insertMulti( drg->value( Table_CIS, CIS_CIS ).toInt(), di );
                                break;
                            }
                        }
                    }
                }
           }
      }
     if (d->m_LogChrono)
         tkLog::logTimeElapsed(t, "mfDrugsBase", QString("interactions() : %2 drugs")
                           .arg(drugs.count()) );

     return toReturn;
}

/**
 \brief Return the list of interactions of a selected \e drug. You must in first call interactions(),
        otherwise you will have undesired behavior.
 \sa interactions()
*/
QList<mfDrugInteraction*> mfDrugsBase::getInteractions( const mfDrugs * drug )
{
     if ( d->m_IamFound.isEmpty() )
          return d->m_DrugInteractionList;

     if ( d->m_DrugInteractionList.isEmpty() )
          return d->m_DrugInteractionList;

     return d->m_Map_CIS_DrugInteraction.values( drug->value( Table_CIS, CIS_CIS ).toInt() );
}

/**
 \brief Return the list of interactions of a selected drug defined by its CIS code \e _CIS.
        You must in first call interactions(), otherwise you will have undesired behavior.
 \sa interactions()
*/
QList<mfDrugInteraction*> mfDrugsBase::getInteractions( const int _CIS )
{
    if ( d->m_IamFound.isEmpty() )
        return d->m_DrugInteractionList;

    if ( d->m_DrugInteractionList.isEmpty() )
        return d->m_DrugInteractionList;

    return d->m_Map_CIS_DrugInteraction.values( _CIS );
}

/**
 \brief Return the maximum interaction type for a selected \e drug. You must in first call interactions(),
        otherwise you will have undesired behavior.
 \sa interactions()
*/
Interaction::TypesOfIAM mfDrugsBase::getMaximumTypeOfIAM( const mfDrugs * drug )
{
     if ( d->m_IamFound.isEmpty() )
          return Interaction::noIAM;

     if ( d->m_DrugInteractionList.isEmpty() )
          return Interaction::noIAM;

     const QList<mfDrugInteraction*> & list
     = d->m_Map_CIS_DrugInteraction.values( drug->value( Table_CIS, CIS_CIS ).toInt() );

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
bool mfDrugsBase::drugHaveInteraction(const  mfDrugs * drug )
{
     if ( d->m_IamFound.isEmpty() )
          return false;
     // if m_DrugInteractionList empty return it
     if ( d->m_DrugInteractionList.isEmpty() )
          return false;
     return d->m_Map_CIS_DrugInteraction.values( drug->value( Table_CIS, CIS_CIS ).toInt() ).count() != 0;
}

/**
 \brief Retrieve from the database the interaction for drugs CIS : \e _id1 and \e _id2
 \sa interactions()
*/
mfDrugInteraction * mfDrugsBasePrivate::getIAM( const int & _id1, const int & _id2 )
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
        QString req = m_DrugsBase->select( Table_IAM, where );
//        mfDrugsTables::getQuery( conditions );
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
                tkLog::addQueryError( "mfDrugsBase", q );
        }
    }

    // get denomination from iam_denomination where ID1 ID2
    QHashWhere where;
    where.insert( IAM_DENOMINATION_ID, QString( "IN ( %1, %2 )" ).arg( _id1 ).arg( id2 ) );
    QString req = m_DrugsBase->select( Table_IAM_DENOMINATION, where );
    {
        QSqlQuery q( req , DB );
        if ( q.isActive() )  {
            while ( q.next() )  {
                if ( _id1 == id2 )  {
                    di->setValue( IAM_MAIN, q.value( 1 ) );
                    di->setValue( IAM_INTERACTOR, q.value( 1 ) );
                    break;
                } else {
                    if ( q.value( 0 ).toInt() == _id1 ) {
                        di->setValue( IAM_MAIN, q.value( 1 ) );
                    } else {
                        di->setValue( IAM_INTERACTOR, q.value( 1 ) );
                    }
                }
            }
        }
        else
            tkLog::addQueryError( "mfDrugsBase", q );
    }
    return di;
}

/**
 \todo Return the last interaction founded after calling interactions().
 \sa interactions()
*/
mfDrugInteraction * mfDrugsBase::getLastInteractionFound()
{
     if ( d->m_IamFound.isEmpty() )
          return 0;

     if ( ! d->m_DrugInteractionList.isEmpty() )
          return d->m_DrugInteractionList.at( 0 );

     tkLog::addError( "mfDrugsBase", "ERROR : problem with mfDrugInteraction * mfDrugsBase::getLastInteractionFound" );

     QSqlDatabase DB = QSqlDatabase::database( DRUGS_DATABASE_NAME );
     if ( !DB.isOpen() )
          DB.open();

     return d->getIAM( d->m_IamFound.begin().key(), d->m_IamFound.begin().value() );
}

/**
 \brief Returns the list of all interactions founded by interactions()
 \sa interactions()
*/
QList<mfDrugInteraction*> mfDrugsBase::getAllInteractionsFound()
{
     // if no interactions were found : return empty list
     if ( d->m_IamFound.isEmpty() )
          return d->m_DrugInteractionList;

     // if m_DrugInteractionList not empty return it
     if ( ! d->m_DrugInteractionList.isEmpty() )
          return d->m_DrugInteractionList;

     QSqlDatabase DB = QSqlDatabase::database(DRUGS_DATABASE_NAME);
     if ( !DB.isOpen() )
          DB.open();

     QMap<int, int>::const_iterator i = d->m_IamFound.constBegin();
     while ( i != d->m_IamFound.constEnd() ) {
          d->m_DrugInteractionList << d->getIAM( i.key(), i.value() );
          ++i;
     }
     return d->m_DrugInteractionList;
}
