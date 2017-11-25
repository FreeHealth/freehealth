/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \class InteractionsBase
  \brief This class owns the interactions mechanisms and is inherited by mfDrugsBase.

  Interactions can be managed by interactions(), drugHaveInteraction(), getMaximumTypeOfIAM(), getInteractions(),
  getLastIAMFound() and getAllIAMFound().
  You must always in first call interactions() with the list of drugs to test.
  Then you can retrieve interactions found using the other members.

  \ingroup freediams drugswidget
*/

#include "interactionsbase.h"

#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsinteraction.h>
#include <drugsbaseplugin/drugsbase.h>
#include "drugsdatabaseselector.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>

#include <medicalutils/ebmdata.h>

#include <translationutils/constanttranslations.h>

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
#include <QCache>


using namespace DrugsDB;
using namespace Constants;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

namespace DrugsDB {
namespace Internal {

    const char * const SEPARATOR = "|||";

    struct AtcLabel {
        QString lang, label, code;
        int id;
    };

/**
  \brief Private part of mfDrugsBase
  \internal
*/
class InteractionsBasePrivate
{
public:
    InteractionsBasePrivate(InteractionsBase *p) :
            m_Parent(p), m_DB(0), m_LogChrono(false), m_initialized(false)
    {
        m_AtcLabelCache.setMaxCost(200);
        m_AtcCodeCache.setMaxCost(1000);
    }

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
    void retrieveLinkTables()
    {
         if ((!m_AtcToMol.isEmpty()) && (!m_ClassToAtcs.isEmpty()))
             return;

         // TODO: WARNING FULL SQL COMMAND */
         {
             QSqlDatabase drugs = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
             if (!drugs.open())
                 Utils::Log::addError("InteractionBase", "Drugs database not opened", __FILE__, __LINE__);
             QSqlQuery query("SELECT * FROM `LK_MOL_ATC`", drugs);
             if (query.isActive()) {
                 while (query.next()) {
                     m_AtcToMol.insertMulti(query.value(1).toInt(), query.value(0).toInt());
                 }
             } else {
                 Utils::Log::addQueryError("InteractionBase", query, __FILE__, __LINE__);
             }
             query.finish();
         }

         // Retreive Interacting classes (1) ---> (n) ATC tree
         QSqlDatabase DB = QSqlDatabase::database(Constants::DB_IAM_NAME);
         if (!DB.isOpen())
              DB.open();
         QString req = m_DB->select(Table_ATC_CLASS_TREE, QList<int>() << TREE_ID_CLASS << TREE_ID_ATC);

         {
             QSqlQuery query(req , DB);
             if (query.isActive()) {
                 while (query.next())
                     m_ClassToAtcs.insertMulti(query.value(0).toInt(), query.value(1).toInt());
             } else {
                 Utils::Log::addQueryError("DrugsBase", query, __FILE__, __LINE__);
             }
         }

         InteractionsBase::m_InteractionsDatabaseAvailable = m_ClassToAtcs.count() && m_AtcToMol.count();
     }


    bool checkDrugInteraction( DrugsData *drug, const QList<DrugsData *> & drugs );
    QList<DrugsInteraction *> getInteractionsFromDatabase( const int & _id1, const int & _id2 );
    QList<DrugsInteraction *> getAllInteractionsFound();

public:
    InteractionsBase *m_Parent;
    Utils::Database *m_DB;
    QMap<int, int> m_InteractionsIDs;        /*!<  All possible interactions based on ATC IDs*/
    QMultiMap<int, int> m_IamFound;               /*!< modified by checkDrugInteraction() */
    bool m_LogChrono;
    bool m_initialized;

    // These variables are used or speed improvments and database protection
    QMultiHash<int, int> m_AtcToMol;   /*!< Link Iam_Id to Code_Subst */
    QMultiHash<int, int> m_ClassToAtcs;   /*!< Link ClassIam_Id to Iam_Id */
    QCache<int, AtcLabel> m_AtcLabelCache;
    QCache<int, QString> m_AtcCodeCache;
};

}  // End Internal
}  // End Drugs


bool InteractionsBase::m_InteractionsDatabaseAvailable = false;

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
             if ( m_InteractionsIDs.keys( s ).contains( s2 ) )
                 if ( ( ! m_IamFound.contains( s2, s ) ) && ( ! m_IamFound.contains( s, s2 ) ) )
                     m_IamFound.insertMulti( s2, s );

//             Not necessary because interactions are "mirrored" in the database
//             if ( m_InteractionsIDs.keys( s2 ).contains( s ) )
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
QList<DrugsInteraction*> InteractionsBase::calculateInteractions(const QList<DrugsData *> &drugs)
{
     QTime t;
     t.start();

     QList<DrugsInteraction *> toReturn;
     di->m_IamFound.clear();

     // check interactions drug by drug --> stored into di->m_IamFound
     foreach(DrugsData *drug, drugs)
          di->checkDrugInteraction(drug, drugs);

     // prepare cached datas
     toReturn = di->getAllInteractionsFound();

     int id1, id2;
     // for each known drug interaction
     foreach(DrugsInteraction* di, toReturn) {
         id1 = di->value(DrugsInteraction::DI_ATC1).toInt();
         id2 = di->value(DrugsInteraction::DI_ATC2).toInt();
         // test all drugs in the list
         foreach(DrugsData *drg, drugs)  {
             if (drg->allInnAndIamClasses().contains(id1) || drg->allInnAndIamClasses().contains(id2)) {
                     di->addInteractingDrug(drg);
             }
         }
     }
     if (di->m_LogChrono)
         Utils::Log::logTimeElapsed(t, "InteractionsBase", QString("interactions() : %2 drugs")
                               .arg(drugs.count()) );

     return toReturn;
}

/**
 \brief Retrieve from the database the interaction for ids : \e _id1 and \e _id2
 \sa interactions()
*/
QList<DrugsInteraction *> InteractionsBasePrivate::getInteractionsFromDatabase(const int & _id1, const int & _id2)
{
    int id2 = _id2;
    QSqlDatabase DB = m_DB->database();
    QList<DrugsInteraction *> toReturn;
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("InteractionBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()));
            return toReturn;
        }
    }

    // first test : is duplication interaction ?
    if (id2 == -1) {
        DrugsInteraction *dint = 0;
        dint = new DrugsInteraction();
        dint->setValue(DrugsInteraction::DI_Type , "U" );
        dint->setValue(DrugsInteraction::DI_ATC1, _id1 );
        dint->setValue(DrugsInteraction::DI_ATC2, _id1 );
        dint->setValue(DrugsInteraction::DI_RiskFr, tkTr(Trans::Constants::INN_DUPLICATION));
        dint->setValue(DrugsInteraction::DI_RiskEn, Trans::Constants::INN_DUPLICATION);
        dint->setValue(DrugsInteraction::DI_ReferencesLink, QCoreApplication::translate("DrugsBase", "FreeDiams Interactions Engine"));
        id2 = _id1;
        toReturn << dint;
        return toReturn;
    }

    // else retrieve INTERACTION from database
    // construct where clause
    QHashWhere where;
    where.insert(IA_ATC1, QString("=%1").arg(_id1));
    where.insert(IA_ATC2, QString("=%1").arg(_id2));

    // get interaction links
    QString req = m_DB->select(Table_INTERACTIONS, where);
    QSqlQuery query(req, DB);
    if (query.isActive()) {
        while (query.next()) {
            DrugsInteraction *dint = 0;
            dint = new DrugsInteraction();
            dint->setValue(DrugsInteraction::DI_Id, query.value(IA_ID));
            dint->setValue(DrugsInteraction::DI_ATC1, query.value(IA_ATC1));
            dint->setValue(DrugsInteraction::DI_ATC2, query.value(IA_ATC2));
            dint->setValue(DrugsInteraction::DI_LinkId, query.value(IA_IAK_ID));
            toReturn << dint;
        }
    } else {
        Utils::Log::addQueryError("InteractionsBase", query, __FILE__, __LINE__);
    }
    query.finish();

    // get interaction knowledge
    foreach(DrugsInteraction *dint, toReturn) {
        where.clear();
        where.insert(Constants::IAK_ID, QString("=%1").arg(dint->value(DrugsInteraction::DI_LinkId).toInt()));
        req = m_DB->select(Table_INTERACTION_KNOWLEDGE, where);
        if (query.exec(req)) {
            if (query.next()) {
                dint->setValue(DrugsInteraction::DI_Type, query.value(Constants::IAK_TYPE));
                dint->setValue(DrugsInteraction::DI_RiskFr, query.value(Constants::IAK_RISK_FR));
                dint->setValue(DrugsInteraction::DI_ManagementFr, query.value(Constants::IAK_MANAGEMENT_FR));
                dint->setValue(DrugsInteraction::DI_RiskEn, query.value(Constants::IAK_RISK_EN));
                dint->setValue(DrugsInteraction::DI_ManagementEn, query.value(Constants::IAK_MANAGEMENT_EN));
                dint->setValue(DrugsInteraction::DI_ReferencesLink, query.value(Constants::IAK_REFERENCES_LINK));
            }
        } else {
            Utils::Log::addQueryError("InteractionBase", query, __FILE__, __LINE__);
        }
        query.finish();
    }

    // get denomination from iam_denomination where ID1 ID2
    //    dint->setValue(IAM_MAIN, m_Parent->getInnDenomination(_id1));
    //    dint->setValue(IAM_INTERACTOR, m_Parent->getInnDenomination(id2));

    return toReturn;
}

/**
 \brief Returns the list of all interactions founded by interactions()
 \sa interactions()
*/
QList<DrugsInteraction *> InteractionsBasePrivate::getAllInteractionsFound()
{
     // if no interactions were found : return empty list
     QList<DrugsInteraction*> toReturn;
     if ( m_IamFound.isEmpty() )
          return toReturn;

     QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
     if (!DB.isOpen())
          DB.open();

     QMap<int, int>::const_iterator i = m_IamFound.constBegin();
     while (i != m_IamFound.constEnd()) {
          toReturn << getInteractionsFromDatabase(i.key(), i.value());
          ++i;
     }
     qSort(toReturn.begin(), toReturn.end(), DrugsInteraction::greaterThan);
     return toReturn;
}


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

    di->m_DB->addTable(Table_INTERACTIONS, "INTERACTIONS");
    di->m_DB->addTable(Table_INTERACTION_KNOWLEDGE, "INTERACTION_KNOWLEDGE");
    di->m_DB->addTable(Table_ATC, "ATC");
    di->m_DB->addTable(Table_ATC_CLASS_TREE, "ATC_CLASS_TREE");
    di->m_DB->addTable(Table_SOURCES, "SOURCES");

    di->m_DB->addField(Table_ATC, ATC_ID,    "ID");
    di->m_DB->addField(Table_ATC, ATC_CODE,  "CODE");
    di->m_DB->addField(Table_ATC, ATC_EN,    "ENGLISH");
    di->m_DB->addField(Table_ATC, ATC_FR,    "FRENCH");
    di->m_DB->addField(Table_ATC, ATC_DE,    "DEUTSCH");

    di->m_DB->addField(Table_INTERACTIONS, IA_ID,     "ID");
    di->m_DB->addField(Table_INTERACTIONS, IA_ATC1,   "ATC_ID1");
    di->m_DB->addField(Table_INTERACTIONS, IA_ATC2,   "ATC_ID2");
    di->m_DB->addField(Table_INTERACTIONS, IA_IAK_ID, "INTERACTION_KNOWLEDGE_ID");

    di->m_DB->addField(Table_INTERACTION_KNOWLEDGE, IAK_ID,    "ID");
    di->m_DB->addField(Table_INTERACTION_KNOWLEDGE, IAK_TYPE,    "TYPE");
    di->m_DB->addField(Table_INTERACTION_KNOWLEDGE, IAK_RISK_FR,    "RISK_FR");
    di->m_DB->addField(Table_INTERACTION_KNOWLEDGE, IAK_MANAGEMENT_FR,    "MANAGEMENT_FR");
    di->m_DB->addField(Table_INTERACTION_KNOWLEDGE, IAK_RISK_EN,    "RISK_EN");
    di->m_DB->addField(Table_INTERACTION_KNOWLEDGE, IAK_MANAGEMENT_EN,    "MANAGEMENT_EN");
    di->m_DB->addField(Table_INTERACTION_KNOWLEDGE, IAK_REFERENCES_LINK,    "REFERENCES_LINK");


    di->m_DB->addField(Table_ATC_CLASS_TREE, TREE_ID_CLASS,    "ID_CLASS");
    di->m_DB->addField(Table_ATC_CLASS_TREE, TREE_ID_ATC,      "ID_ATC");
    di->m_DB->addField(Table_ATC_CLASS_TREE, TREE_SOURCE_LINK, "SOURCE_LINK");

    di->m_DB->addField(Table_SOURCES, SOURCES_ID, "ID");
    di->m_DB->addField(Table_SOURCES, SOURCES_SOURCE_LINK, "SOURCE_LINK");
    di->m_DB->addField(Table_SOURCES, SOURCES_TYPE, "TYPE");
    di->m_DB->addField(Table_SOURCES, SOURCES_LINK, "LINK");
    di->m_DB->addField(Table_SOURCES, SOURCES_TEXTUAL_REFERENCE, "TEXTUAL_REFERENCE");
    di->m_DB->addField(Table_SOURCES, SOURCES_ABSTRACT, "ABSTRACT");
    di->m_DB->addField(Table_SOURCES, SOURCES_EXPLANATION, "EXPLANATION");
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
bool InteractionsBase::init(bool refreshCache)
{
    // only one base can be initialized
    if (di->m_initialized && !refreshCache)
        return true;

    QString pathToDb = "";

    if (Utils::isRunningOnMac())
        pathToDb = settings()->databasePath() + QDir::separator() + QString(Constants::DB_DRUGS_NAME);
    else
        pathToDb = settings()->databasePath() + QDir::separator() + QString(Constants::DB_DRUGS_NAME);

    di->m_DB->createConnection(DB_IAM_NAME, DB_IAM_FILENAME, pathToDb,
                               Utils::Database::ReadOnly, Utils::Database::SQLite);


    // retrieve interactions into m_InteractionsIDs for speed improvements
    if (!di->m_DB->database().isOpen())
        if (!di->m_DB->database().open())
            Utils::Log::addError("InteractionsBase", QString("Unable to open database. Error : %1").arg(di->m_DB->database().lastError().text()), __FILE__, __LINE__);

    QList<int> fields;
    fields << IA_ATC1 << IA_ATC2;
    QString req = di->m_DB->select(Table_INTERACTIONS, fields);
    QSqlQuery q(req , di->m_DB->database());
    if (q.isActive())
        while (q.next())
            di->m_InteractionsIDs.insertMulti(q.value(0).toInt(), q.value(1).toInt());

    // retrieve links tables for speed improvements
    if (refreshCache) {
        di->m_AtcToMol.clear();
    }
    di->retrieveLinkTables();

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

QString InteractionsBase::getIamWhereClause(const int &tableref, const QHash<int, QString> &conditions) const
{
    return di->m_DB->getWhereClause(tableref, conditions);
}

QString InteractionsBase::selectInteractionsSql(const int &tableref, const QList<int> &fieldsref, const QHash<int, QString> &conditions) const
{
    return di->m_DB->select(tableref, fieldsref, conditions);
}

///**
//  \brief Return the Inn code linked to the molecule code. Returns -1 if no inn is linked to that molecule.
//  \todo this should return a list of integer not an unique integer
//*/
//int InteractionsBase::getAtcCodeForMoleculeId(const int molId) const
//{
//    if (di->m_AtcToMol.values().contains(molId))
//        return di->m_AtcToMol.key(molId);
//    return -1;
//}

//QString InteractionsBase::getAtcLabel(const int atcId) const
//{
//    if (inncode==-1)
//        return QString();

//    const QString &lang = QLocale().name().left(2);
//    if (di->m_AtcLabelCache.contains(inncode)) {
//        const AtcLabel *lbl = di->m_AtcLabelCache[inncode];
//        if (lbl->lang==lang) {
//            return lbl->label;
//        }
//    }

//    int field;
//    if (lang=="fr") {
//        field = ATC_FR;
//    } else if (lang=="de") {
//        field = ATC_DE;
//    } else {
//        field = ATC_EN;
//    }
//    QString toReturn;
//    QString code;
//    QHash<int, QString> where;
//    where.insert(Constants::ATC_ID, QString("=%1").arg(inncode));
//    QSqlQuery query(QSqlDatabase::database(Constants::DB_IAM_NAME));
//    if (!query.exec(di->m_DB->select(Constants::Table_ATC, QList<int>() << field << Constants::ATC_CODE, where))) {
//        Utils::Log::addQueryError("InteractionBase", query, __FILE__, __LINE__);
//    } else {
//        if (query.next()) {
//            toReturn = query.value(0).toString();
//            code = query.value(1).toString();
//        } else {
//            Utils::Log::addQueryError("InteractionsBase", query, __FILE__, __LINE__);
//        }
//    }
//    AtcLabel *lbl = new AtcLabel;
//    lbl->id = inncode;
//    lbl->label = toReturn;
//    lbl->lang = lang;
//    lbl->code = code;
//    di->m_AtcLabelCache.insert(inncode, lbl);
//    return toReturn;
//}

//QString InteractionsBase::getAtcLabel(const QString &code) const
//{
//    const QString &lang = QLocale().name().left(2);
//    foreach(int k, di->m_AtcLabelCache.keys()) {
//        AtcLabel *lbl = di->m_AtcLabelCache[k];
//        if (lbl->code.toLower() == code.toLower()) {
//            if (lbl->lang==lang) {
//                return lbl->label;
//            }
//        }
//    }

//    int field;
//    if (lang=="fr") {
//        field = ATC_FR;
//    } else if (lang=="de") {
//        field = ATC_DE;
//    } else {
//        field = ATC_EN;
//    }
//    QString toReturn;
//    int id;
//    QHash<int, QString> where;
//    where.insert(Constants::ATC_CODE, QString("='%1'").arg(code));
//    QSqlQuery query(QSqlDatabase::database(Constants::DB_IAM_NAME));
//    if (!query.exec(di->m_DB->select(Constants::Table_ATC, QList<int>() << field << Constants::ATC_ID, where))) {
//        Utils::Log::addQueryError("InteractionBase", query, __FILE__, __LINE__);
//    } else {
//        if (query.next()) {
//            toReturn = query.value(0).toString();
//            id = query.value(1).toInt();
//        }
//    }
//    AtcLabel *lbl = new AtcLabel;
//    lbl->id = id;
//    lbl->label = toReturn;
//    lbl->lang = lang;
//    lbl->code = code;
//    di->m_AtcLabelCache.insert(id, lbl);
//    return toReturn;
//}

//QString InteractionsBase::getAtcCode(const int atcId) const
//{
//    if (atc_id==-1)
//        return QString();

//    if (di->m_AtcCodeCache.contains(atc_id)) {
//        return *di->m_AtcCodeCache[atc_id];
//    }

//    QString toReturn;
//    QHash<int, QString> where;
//    where.insert(Constants::ATC_ID, QString("=%1").arg(atc_id));
//    QSqlQuery query(QSqlDatabase::database(Constants::DB_IAM_NAME));
//    if (!query.exec(di->m_DB->select(Constants::Table_ATC, Constants::ATC_CODE, where))) {
//        Utils::Log::addQueryError("InteractionBase", query, __FILE__, __LINE__);
//        return QString();
//    } else {
//        if (query.next())
//            toReturn = query.value(0).toString();
//    }
//    di->m_AtcCodeCache.insert(atc_id, new QString(toReturn));
//    return toReturn;
//}

///** \brief Returns the name of the INN for the substance code \e code_subst. */
//QString InteractionsBase::getInnDenominationFromSubstanceCode(const int molecule_code) const
//{
//     // if molecule is not associated with a dci exit
//     if (!di->m_AtcToMol.values().contains(molecule_code))
//          return QString::null;
//     return getAtcLabel(di->m_AtcToMol.key(molecule_code));
//}

///** \brief Returns the name of the INN for the substance code \e code_subst. */
//QStringList InteractionsBase::getIamClassDenomination(const int &molecule_code)
//{
//     // if molecule is not associated with a dci exit
//     if (!di->m_AtcToMol.values().contains(molecule_code))
//          return QStringList();

//     // if molecule is not associated with a class exit
//     QList<int> list = di->m_ClassToAtcs.keys(di->m_AtcToMol.key(molecule_code));
//     if (list.isEmpty())
//          return QStringList();
//     QStringList toReturn;
//     foreach(int i, list)
//         toReturn << getAtcLabel(i);
//     return toReturn;
//}

///**
//  \brief Returns all INN and IAM classes of MOLECULE \e code_subst.
//  \sa getDrugByUID()
//*/
//QSet<int> InteractionsBase::getAllInnAndIamClassesIndex(const int molecule_code)
//{
//    QSet<int> toReturn;

////    if (di->m_AtcToMol.keys(molecule_code).count()>1)
////        Utils::Log::addError("InteractionBase", "Molecule got multiATC " + QString::number(molecule_code), __FILE__, __LINE__);

//    foreach(int id, di->m_AtcToMol.keys(molecule_code)) {
//        toReturn = di->m_ClassToAtcs.keys(id).toSet();
//    }
//    if (di->m_AtcToMol.values().contains(molecule_code))
//        toReturn << di->m_AtcToMol.key(molecule_code);

////    qWarning() << Q_FUNC_INFO << molecule_code << toReturn;
//    return toReturn;
//}

///** \brief Return the list of the code of the substances linked to the INN code \e code_iam. */
//QList<int> InteractionsBase::getLinkedMoleculeCodes(QList<int> &atc_ids) const
//{
//    QList<int> toReturn;
//    foreach(int i, atc_ids)
//        toReturn << di->m_AtcToMol.values(i);
//    return toReturn;
//}

///** \brief Return the list of the code of the substances linked to the INN code \e code_iam. */
//QList<int> InteractionsBase::getLinkedMoleculeCodes(const int atc_id) const
//{
//    return di->m_AtcToMol.values(atc_id);
//}

///** \brief Retreive from database the Molecules Codes where denomination of the INN is like 'iamDenomination%' */
//QList<int> InteractionsBase::getLinkedMoleculeCodes(const QString &iamDenomination) const
//{
//     QSqlDatabase DB = di->m_DB->database();
//     if (!DB.isOpen())
//          DB.open();

//     // get iamSubstCode
//     QString tmp = iamDenomination;
//     const QString &lang = QLocale().name().left(2);
//     QHash<int, QString> where;
//     if (lang=="fr")
//         where.insert(ATC_FR, QString("LIKE '%1%'").arg(tmp.replace("'", "?")));
//     else if (lang=="de")
//         where.insert(ATC_DE, QString("LIKE '%1%'").arg(tmp.replace("'", "?")));
//     else
//         where.insert(ATC_EN, QString("LIKE '%1%'").arg(tmp.replace("'", "?")));
//     QList<int> atcIds;
//     QString req = di->m_DB->select(Table_ATC, ATC_ID, where);
//     QSqlQuery q(req , di->m_DB->database());
//     if (q.isActive())
//         while (q.next())
//             atcIds << q.value(0).toInt();
//     return getLinkedMoleculeCodes(atcIds);
//}

///** \brief Return the list of the INN code linked to the substances code \e code_subst. */
//QList<int> InteractionsBase::getLinkedAtcIds(const QList<int> &molecule_codes) const
//{
//    QList<int> toReturn;
//    foreach(int i, molecule_codes)
//        toReturn << di->m_AtcToMol.keys(i);
//    return toReturn;
//}

///** \brief Return the list of the INN code linked to the molecule code \e code_subst. */
//QList<int> InteractionsBase::getLinkedAtcIds(const int molecule_code) const
//{
//    return di->m_AtcToMol.keys(molecule_code);
//}

//QList<int> InteractionsBase::getAllMoleculeCodeWithAtcStartingWith(const QString &code) const
//{
//    QSqlDatabase DB = di->m_DB->database();
//    if (!DB.isOpen())
//         DB.open();

//    QHash<int, QString> where;
//    where.insert(ATC_CODE, QString("LIKE '%1%'").arg(code));
//    QList<int> atcIds;
//    QString req = di->m_DB->select(Table_ATC, ATC_ID, where);
//    QSqlQuery q(req , di->m_DB->database());
//    if (q.isActive())
//        while (q.next())
//            atcIds << q.value(0).toInt();
//    return getLinkedMoleculeCodes(atcIds);
//}

QVector<MedicalUtils::EbmData *> InteractionsBase::getAllSourcesFromTree(const QList<int> &allInnAndIamClassIds)
{
    QVector<MedicalUtils::EbmData *> ret;
    if (allInnAndIamClassIds.count() == 0)
        return ret;

    QStringList classIds, innIds;
    foreach(int id, allInnAndIamClassIds) {
        if (id >= 200000)
            classIds << QString::number(id);
        else
            innIds <<QString::number(id);
    }

    // get all source_link
//    QHash<int, QString> where;
//    where.insert(TREE_ID_CLASS, QString("IN (%1)").arg(classIds.join(",")));
//    where.insert(TREE_ID_ATC, QString("IN (%1)").arg(innIds.join(",")));
    QString req = QString("%1, %2 WHERE "
                          "`%2`.`%3` IN (%5) AND `%2`.`%4` IN (%6) AND %7")
            .arg(di->m_DB->select(Table_SOURCES))
            .arg(di->m_DB->table(Table_ATC_CLASS_TREE))
            .arg(di->m_DB->fieldName(Table_ATC_CLASS_TREE, TREE_ID_CLASS))
            .arg(di->m_DB->fieldName(Table_ATC_CLASS_TREE, TREE_ID_ATC))
            .arg(classIds.join(","))
            .arg(innIds.join(","))
            .arg(di->m_DB->fieldEquality(Table_ATC_CLASS_TREE, TREE_SOURCE_LINK,
                                         Table_SOURCES, SOURCES_SOURCE_LINK))
            ;

    QStringList links;
    QSqlQuery query(req, di->m_DB->database());
    if (query.isActive()) {
        while (query.next()) {
            if (links.contains(query.value(SOURCES_LINK).toString()))
                continue;
            links << query.value(SOURCES_LINK).toString();
            MedicalUtils::EbmData *ebm = new MedicalUtils::EbmData;
            ebm->setId(query.value(SOURCES_ID));
            ebm->setLink(query.value(SOURCES_LINK).toString());
            ebm->setReferences(query.value(SOURCES_TEXTUAL_REFERENCE).toString());
            ebm->setAbstract(query.value(SOURCES_ABSTRACT).toString());
            ret << ebm;
        }
    } else {
        Utils::Log::addQueryError("InteractionBase", query, __FILE__, __LINE__);
    }

    return ret;
}
