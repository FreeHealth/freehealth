/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class DrugsBase
  \brief This class owns the drugs and dosages database and interactions mechanism.

  0. Terminology\n
  \e Substances are the chimic molecules that compose a drug.\n
  \e IamCode or \e INN are the INN codes and names.\n
  \e IamClass are the classes that regroups INNs into classes of pharmaceutics family.\n
  \e CIP : presentation code of a drug. A drug can be presented into different presentation (15 pills, 30 pills a box...).\n
  \e UID : speciality code of a drug. Everything is base on this code.
     One UID code can be associated to many CIP, many Substances, many INNs, and many IamClasses.

  1. Initialization\n
  This class is pure static, so you can not instanciate it. To initialize datas, just do once : init().
  isInitialized() alerts you of the state of intialization.
  These two members returns true if all is ok.

  2. Drugs retreiver\n
  You can retreive drugs using UID ou CIP code via getDrugByUID() and getDrufByCIP().

  3. Dosages retreiver / saver

  \todo Manage user rights when creating dosage database

  \sa database_structure
  \ingroup freediams drugswidget
*/

#include "drugsbase.h"
#include "idrug.h"

#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/drugsearchengine.h>

#include <utils/global.h>
#include <utils/log.h>

#include <medicalutils/ebmdata.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/dialogs/settingsdialog.h>

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
#include <QVector>
#include <QSet>
#include <QCache>


enum { WarnExtractedDrugs=false };

using namespace DrugsDB;
using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DrugsDB::Internal::DrugSearchEngine *searchEngine()  { return DrugsDB::Internal::DrugSearchEngine::instance(); }
static inline QString databaseFileName() {return settings()->databasePath() + QDir::separator() + QString(Constants::DB_DRUGS_NAME) + QDir::separator() + QString(Constants::DB_DRUGS_FILENAME);}

static inline bool connectDatabase(QSqlDatabase &DB, const QString &file, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("DrugsBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 file, line);
            return false;
        }
    }
    return true;
}

namespace {
    struct AtcLabel {
        QString lang, label;
        int id;
    };
}


namespace DrugsDB {
namespace Internal {
class DrugsBasePrivate
{
public:
    DrugsBasePrivate(DrugsBase *base) :
            q(base),
            m_ActualDBInfos(0),
            m_LogChrono(false),
            m_RefreshDrugsBase(false),
            m_RefreshDosageBase(false),
            m_UseRoutes(true)
    {
        m_AtcLabelCache.setMaxCost(200);
        m_AtcCodeCacheIdKeyed.setMaxCost(1000);
//        m_AtcCodeCacheCodeKeyed.setMaxCost(1000);
    }

    ~DrugsBasePrivate()
    {
    }

    void retreiveLinkTables()
    {
        if ((!m_AtcToMol.isEmpty()) && (!m_ClassToAtcs.isEmpty()))
            return;
        QSqlQuery query(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        if (query.exec(q->select(Constants::Table_LK_MOL_ATC))) {
            while (query.next()) {
                m_AtcToMol.insertMulti(query.value(Constants::LK_ATC_ID).toInt(), query.value(Constants::LK_MID).toInt());
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
        }
        query.finish();
    }

    QString getLabel(const int masterLid)
    {
        Utils::Join j(Constants::Table_LABELS, Constants::LABELS_LID, Constants::Table_LABELSLINK, Constants::LABELSLINK_LID);
        Utils::Field f(Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID, QString("=%1").arg(masterLid));

        /** \todo manage language */
        QString req = q->select(Constants::Table_LABELS, j, f);
        QSqlQuery query(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        if (query.exec(req)) {
            if (query.next())
                return query.value(Constants::LABELS_LABEL).toString();
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
        }
        return QString();
    }

    // Returns all labels : QHash key=lang, value=label
    QHash<QString, QString> getAllLabels(const int masterLid)
    {
        Utils::JoinList joins;
        joins << Utils::Join(Constants::Table_LABELS, Constants::LABELS_LID, Constants::Table_LABELSLINK, Constants::LABELSLINK_LID);
        Utils::FieldList cond;
        cond << Utils::Field(Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID, QString("=%1").arg(masterLid));

        QHash<QString, QString> toReturn;
        QString req = q->select(Constants::Table_LABELS, joins, cond);
        QSqlQuery query(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        if (query.exec(req)) {
            while (query.next()) {
                toReturn.insert(query.value(Constants::LABELS_LANG).toString(), query.value(Constants::LABELS_LABEL).toString());
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
        }
        return toReturn;
    }

    void getInteractingClassTree()
    {
        // Retreive Interacting classes (1) ---> (n) ATC tree
        QString req = q->select(Constants::Table_IAM_TREE, QList<int>() << Constants::IAM_TREE_ID_CLASS << Constants::IAM_TREE_ID_ATC);
        QSqlQuery query(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        if (query.exec(req)) {
            while (query.next()) {
                m_ClassToAtcs.insertMulti(query.value(0).toInt(), query.value(1).toInt());
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
        }
    }

    void getSearchEngine()
    {
        // get all Drugs search engines
        searchEngine()->clear();
        QSqlQuery search(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        QString req = q->select(Constants::Table_SEARCHENGINES);
        if (search.exec(req)) {
            while (search.next()) {
                searchEngine()->addNewEngine(search.value(Constants::SEARCHENGINE_LABEL).toString(),
                                             search.value(Constants::SEARCHENGINE_URL).toString());
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, search);
        }
        LOG_FOR(q, QString("Getting %1 Drugs Search Engines").arg(searchEngine()->numberOfEngines()));
    }

    void getDrugComponents(IDrug *drug)
    {
        Utils::FieldList get;
        get << Utils::Field(Constants::Table_COMPO, Constants::COMPO_STRENGTH);
        get << Utils::Field(Constants::Table_COMPO, Constants::COMPO_DOSE_REF);
        get << Utils::Field(Constants::Table_COMPO, Constants::COMPO_NATURE);
        get << Utils::Field(Constants::Table_COMPO, Constants::COMPO_LK_NATURE);
        get << Utils::Field(Constants::Table_MOLS, Constants::MOLS_MID);
        get << Utils::Field(Constants::Table_MOLS, Constants::MOLS_NAME);
        get << Utils::Field(Constants::Table_MOLS, Constants::MOLS_WWW);
        Utils::Join join(Constants::Table_COMPO, Constants::COMPO_MID, Constants::Table_MOLS, Constants::MOLS_MID);
        Utils::Field cond(Constants::Table_COMPO, Constants::COMPO_DID, QString("=%1").arg(drug->data(IDrug::DrugID).toInt()));
        QString sort = QString(" ORDER BY %1 ASC").arg(q->fieldName(Constants::Table_COMPO, Constants::COMPO_LK_NATURE));
        QString req = q->select(get, join, cond) + sort;
        IComponent *compo = 0;
        IComponent *precedent = 0;
        QSqlQuery query(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        if (query.exec(req)) {
            while (query.next()) {
                compo = new IComponent(drug);
                compo->setDataFromDb(IComponent::Strength, query.value(0));
                compo->setDataFromDb(IComponent::Dose, query.value(1));
                compo->setDataFromDb(IComponent::Nature, query.value(2));
                compo->setDataFromDb(IComponent::NatureLink, query.value(3));
                int molId = query.value(4).toInt();
                compo->setDataFromDb(IComponent::MID, molId);
                compo->setDataFromDb(IComponent::Name, query.value(5));
                compo->setDataFromDb(IComponent::WebLink, query.value(6));
                QVector<int> innIds;
                QVector<int> classIds;
                foreach(int i, m_AtcToMol.keys(molId)) {
                    if (i < 200000) {
                        innIds << i;
                        classIds << m_ClassToAtcs.keys(i).toVector();
                    } else {
                        // get call from interacting tree
                        classIds << i;
                    }
                }
                compo->set7CharAtcIds(innIds);
                compo->setInteractingClassAtcIds(classIds);
                if (precedent) {
                    if (compo->lkNature() == precedent->lkNature()) {
                        compo->linkWithComposition(precedent);
                    }
                }
                precedent = compo;
                if (WarnExtractedDrugs)
                    qWarning() << compo->data(IComponent::DebuggingText).toString();
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
        }
    }

    void getRoutes(IDrug *drug)
    {
        Utils::Join j(Constants::Table_DRUG_ROUTES, Constants::DRUG_ROUTES_DID, Constants::Table_ROUTES, Constants::ROUTES_RID);
        Utils::Field cond(Constants::Table_DRUG_ROUTES, Constants::DRUG_ROUTES_DID, QString("=%1").arg(drug->data(IDrug::DrugID).toString()));

        QSqlQuery query(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        if (query.exec(q->select(Constants::Table_ROUTES, j, cond))) {
            while (query.next()) {
                DrugRoute *route = new DrugRoute(drug);
                route->setIsSystemic(query.value(Constants::ROUTES_SYSTEMIC).toBool());
                route->setRouteId(query.value(Constants::ROUTES_RID).toInt());
                QHash<QString, QString> labels = getAllLabels(query.value(Constants::ROUTES_MASTERLID).toInt());
                foreach(const QString &lang, labels) {
                    route->setLabel(lang, labels.value(lang));
                }
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
        }
    }

public:
    DrugsBase *q;
    DatabaseInfos *m_ActualDBInfos;
    bool m_LogChrono, m_RefreshDrugsBase, m_RefreshDosageBase, m_UseRoutes;

    QMultiHash<int, int> m_AtcToMol;   /*!< Link Iam_Id to Code_Subst */
    QMultiHash<int, int> m_ClassToAtcs;   /*!< Link ClassIam_Id to Iam_Id */
    QCache<int, AtcLabel> m_AtcLabelCache;
    /** \todo improve memory usage here */
    QCache<int, QString> m_AtcCodeCacheIdKeyed;
    QHash<QString, int> m_AtcCodeCacheCodeKeyed;
};
}  // End Internal
}  // End DrugsDB


//--------------------------------------------------------------------------------------------------------
//--------------------------------- Initialization of static members -------------------------------------
//--------------------------------------------------------------------------------------------------------
DrugsBase * DrugsBase::m_Instance = 0;
bool DrugsBase::m_initialized = false;

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Initializing Database -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Returns the unique instance of DrugsBase. If it does not exists, it is created */
DrugsBase *DrugsBase::instance()
{
    if (!m_Instance) {
        m_Instance = new DrugsBase(qApp);
        m_Instance->init();
    }
    return m_Instance;
}

DrugsBase::DrugsBase(QObject *parent)
    : QObject(parent), Utils::Database(), d(0)
{
    d = new DrugsBasePrivate(this);
    setObjectName("DrugsBase");

    using namespace Constants;
    // New schema
    addTable(Table_MASTER,          "MASTER");
    addTable(Table_SOURCES,         "SOURCES");
    addTable(Table_LABELS,          "LABELS");
    addTable(Table_LABELSLINK,      "LABELS_LINK");
    addTable(Table_BIB,             "BIBLIOGRAPHY");
    addTable(Table_BIB_LINK,        "BIBLIOGRAPHY_LINKS");
    addTable(Table_DRUGS,           "DRUGS");
    addTable(Table_MOLS,            "MOLS");
    addTable(Table_COMPO,           "COMPOSITION");
    addTable(Table_UNITS,           "UNITS");
    addTable(Table_LK_MOL_ATC,      "LK_MOL_ATC");
    addTable(Table_PACKAGING,       "PACKAGING");
    addTable(Table_DRUG_ROUTES,     "DRUG_ROUTES");
    addTable(Table_DRUG_FORMS,      "DRUG_FORMS");
    addTable(Table_ROUTES,          "ROUTES");
    addTable(Table_SEARCHENGINES,   "SEARCH_ENGINES");
    addTable(Table_VERSION,         "DB_SCHEMA_VERSION");
    addTable(Table_ATC,             "ATC");
    addTable(Table_INTERACTIONS,    "INTERACTIONS");
    addTable(Table_IAKNOWLEDGE,     "IAKNOWLEDGE");
    addTable(Table_IA_IAK,          "IA_IAK");
    addTable(Table_ATC_LABELS,      "ATC_LABELS");
    addTable(Table_IAM_TREE,        "IAM_TREE");

    addField(Table_MASTER, MASTER_DID, "DID");
    addField(Table_MASTER, MASTER_UID1, "UID1");
    addField(Table_MASTER, MASTER_UID2, "UID2");
    addField(Table_MASTER, MASTER_UID3, "UID3");
    addField(Table_MASTER, MASTER_OLDUID, "OLD_UID");
    addField(Table_MASTER, MASTER_SID, "SID");

    addField(Table_SOURCES, SOURCES_SID, "SID");
    addField(Table_SOURCES, SOURCES_DBUID, "DATABASE_UID");
    addField(Table_SOURCES, SOURCES_MASTERLID, "MASTER_LID");
    addField(Table_SOURCES, SOURCES_LANG, "LANG");
    addField(Table_SOURCES, SOURCES_WEB, "WEB");
    addField(Table_SOURCES, SOURCES_COPYRIGHT, "COPYRIGHT");
    addField(Table_SOURCES, SOURCES_DATE, "DATE");
    addField(Table_SOURCES, SOURCES_DRUGS_VERSION, "DRUGS_VERSION");
    addField(Table_SOURCES, SOURCES_AUTHORS, "AUTHORS");
    addField(Table_SOURCES, SOURCES_VERSION, "VERSION");
    addField(Table_SOURCES, SOURCES_PROVIDER, "PROVIDER");
    addField(Table_SOURCES, SOURCES_WEBLINK, "WEBLINK");
    addField(Table_SOURCES, SOURCES_DRUGUID_NAME, "DRUG_UID_NAME");
    addField(Table_SOURCES, SOURCES_ATC, "ATC");
    addField(Table_SOURCES, SOURCES_INTERACTIONS, "INTERACTIONS");
    addField(Table_SOURCES, SOURCES_COMPL_WEBSITE, "COMPLEMENTARY_WEBSITE");
    addField(Table_SOURCES, SOURCES_PACKUID_NAME, "PACK_MAIN_CODE_NAME");
    addField(Table_SOURCES, SOURCES_COMPLETION, "MOL_LINK_COMPLETION");
    addField(Table_SOURCES, SOURCES_AUTHOR_COMMENTS, "AUTHOR_COMMENTS");
    addField(Table_SOURCES, SOURCES_DRUGNAMECONSTRUCTOR, "DRUGS_NAME_CONSTRUCTOR");
    addField(Table_SOURCES, SOURCES_FMFCOMPAT, "FREEMEDFORMS_COMPTA_VERSION");
    addField(Table_SOURCES, SOURCES_OPENREACT_COMPAT, "WEBPORTAL_COMPTA_VERSION");

    addField(Table_LABELS, LABELS_LID, "LID");
    addField(Table_LABELS, LABELS_LANG, "LANG");
    addField(Table_LABELS, LABELS_LABEL, "LABEL");

    addField(Table_LABELSLINK, LABELSLINK_MASTERLID, "MASTER_LID");
    addField(Table_LABELSLINK, LABELSLINK_LID, "LID");

    addField(Table_BIB, BIB_BIBID, "BIB_ID");
    addField(Table_BIB, BIB_TYPE, "TYPE");
    addField(Table_BIB, BIB_LINK, "LINK");
    addField(Table_BIB, BIB_TEXTREF, "TEXTUAL_REFERENCE");
    addField(Table_BIB, BIB_ABSTRACT, "ABSTRACT");
    addField(Table_BIB, BIB_EXPLAIN, "EXPLANATION");

    addField(Table_BIB_LINK, BIB_LINK_MASTERID, "BIB_MASTER_ID");
    addField(Table_BIB_LINK, BIB_LINK_BIBID, "BIB_ID");

    addField(Table_DRUGS, DRUGS_ID ,            "ID");
    addField(Table_DRUGS, DRUGS_DID ,          "DID");
    addField(Table_DRUGS, DRUGS_SID,           "SID");
    addField(Table_DRUGS, DRUGS_NAME,          "NAME");
    addField(Table_DRUGS, DRUGS_ATC_ID,            "ATC_ID");
    addField(Table_DRUGS, DRUGS_STRENGTH,       "STRENGTH");
    addField(Table_DRUGS, DRUGS_VALID,        "VALID");
    addField(Table_DRUGS, DRUGS_MARKET,         "MARKETED");
    addField(Table_DRUGS, DRUGS_AID_MASTER_LID,        "AID_MASTER_LID");
    addField(Table_DRUGS, DRUGS_LINK_SPC,       "LINK_SPC");
    addField(Table_DRUGS, DRUGS_EXTRA_XML,  "EXTRA_XML");

    addField(Table_MOLS, MOLS_MID,  "MID");
    addField(Table_MOLS, MOLS_SID,  "SID");
    addField(Table_MOLS, MOLS_NAME,  "NAME");
    addField(Table_MOLS, MOLS_WWW,  "WWW");

    addField(Table_COMPO, COMPO_DID,          "DID");
    addField(Table_COMPO, COMPO_MID,     "MID");
    addField(Table_COMPO, COMPO_STRENGTH,     "STRENGTH");
    addField(Table_COMPO, COMPO_STRENGTH_NID,     "STRENGTH_NID");
    addField(Table_COMPO, COMPO_DOSE_REF,      "DOSE_REF");
    addField(Table_COMPO, COMPO_REF_NID,       "DOSE_REF_NID");
    addField(Table_COMPO, COMPO_NATURE,       "NATURE");
    addField(Table_COMPO, COMPO_LK_NATURE,    "LK_NATURE");

    addField(Table_UNITS, UNITS_NID,    "NID");
    addField(Table_UNITS, UNITS_VALUE,  "VALUE");

    addField(Table_LK_MOL_ATC, LK_MID,  "MID");
    addField(Table_LK_MOL_ATC, LK_ATC_ID, "ATC_ID");
    addField(Table_LK_MOL_ATC, LK_ATC_SID, "SID");

    addField(Table_PACKAGING, PACK_DID,    "DID");
    addField(Table_PACKAGING, PACK_SID,    "SID");
    addField(Table_PACKAGING, PACK_PACK_UID,    "PACKAGE_UID");
    addField(Table_PACKAGING, PACK_LABEL,       "LABEL");
    addField(Table_PACKAGING, PACK_STATUS,      "STATUS");
    addField(Table_PACKAGING, PACK_MARKET,      "MARKETING");
    addField(Table_PACKAGING, PACK_DATE,        "DATE");
    addField(Table_PACKAGING, PACK_OPTION_CODE, "OPTIONAL_CODE");

    addField(Table_DRUG_ROUTES, DRUG_ROUTES_DID, "DID");
    addField(Table_DRUG_ROUTES, DRUG_ROUTES_RID, "RID");

    addField(Table_DRUG_FORMS, DRUG_FORMS_DID, "DID");
    addField(Table_DRUG_FORMS, DRUG_FORMS_MASTERLID, "MASTER_LID");

    addField(Table_ROUTES, ROUTES_RID, "RID");
    addField(Table_ROUTES, ROUTES_MASTERLID, "MASTER_LID");
    addField(Table_ROUTES, ROUTES_SYSTEMIC, "IS_SYSTEMIC");

    addField(Table_SEARCHENGINES, SEARCHENGINE_ID, "ID");
    addField(Table_SEARCHENGINES, SEARCHENGINE_LABEL, "LABEL");
    addField(Table_SEARCHENGINES, SEARCHENGINE_URL, "URL");

    addField(Table_VERSION, VERSION_ID, "ID");
    addField(Table_VERSION, VERSION_VERSION, "VERSION");
    addField(Table_VERSION, VERSION_DATE, "DATE");
    addField(Table_VERSION, VERSION_COMMENT, "COMMENT");

    // DDI tables and fields
    addField(Table_ATC, ATC_ID, "ATC_ID");
    addField(Table_ATC, ATC_CODE, "CODE");

    addField(Table_INTERACTIONS, INTERACTIONS_IAID, "IAID");
    addField(Table_INTERACTIONS, INTERACTIONS_ATC_ID1, "ATC_ID1");
    addField(Table_INTERACTIONS, INTERACTIONS_ATC_ID2, "ATC_ID2");

    addField(Table_IAKNOWLEDGE, IAKNOWLEDGE_IAKID, "IAKID");
    addField(Table_IAKNOWLEDGE, IAKNOWLEDGE_TYPE, "TYPE");
    addField(Table_IAKNOWLEDGE, IAKNOWLEDGE_RISK_MASTERLID, "RISK_MASTER_LID");
    addField(Table_IAKNOWLEDGE, IAKNOWLEDGE_MANAGEMENT_MASTERLID, "MAN_MASTER_LID");
    addField(Table_IAKNOWLEDGE, IAKNOWLEDGE_BIB_MASTERID, "BIB_MASTER_ID");
    addField(Table_IAKNOWLEDGE, IAKNOWLEDGE_WWW, "WWW");

    addField(Table_IA_IAK, IA_IAK_IAID, "IAID");
    addField(Table_IA_IAK, IA_IAK_IAKID, "IAKID");

    addField(Table_ATC_LABELS, ATC_LABELS_ATCID, "ATC_ID");
    addField(Table_ATC_LABELS, ATC_LABELS_MASTERLID, "MASTER_LID");

    addField(Table_IAM_TREE, IAM_TREE_ID_CLASS, "ID_CLASS");
    addField(Table_IAM_TREE, IAM_TREE_ID_ATC, "ID_ATC");
    addField(Table_IAM_TREE, IAM_TREE_BIBMASTERID, "ID");

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

DrugsBase::~DrugsBase()
{
    if (d) delete d;
    d=0;
}

/** \brief Initializer for the database. Return the error state. */
bool DrugsBase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // remove drugs database connection if exists
    if (d->m_RefreshDrugsBase) {
        if (QSqlDatabase::connectionNames().contains(Constants::DB_DRUGS_NAME)) {
            QSqlDatabase::database(Constants::DB_DRUGS_NAME).close();
            QSqlDatabase::removeDatabase(Constants::DB_DRUGS_NAME);
        }
        d->m_RefreshDrugsBase = false;
    }

    // create drugs database connection
    if (!QSqlDatabase::connectionNames().contains(Constants::DB_DRUGS_NAME)) {
        // test driver
        if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
            LOG_ERROR(tkTr(Trans::Constants::SQLITE_DRIVER_NOT_AVAILABLE));
            Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
                                     tkTr(Trans::Constants::SQLITE_DRIVER_NOT_AVAILABLE_DETAIL),
                                     "", qApp->applicationName());
            return false;
        }

//        QString dbFileName = settings()->value(Constants::S_SELECTED_DATABASE_FILENAME).toString();
//        if (dbFileName.startsWith(Core::Constants::TAG_APPLICATION_RESOURCES_PATH)) {
//            dbFileName.replace(Core::Constants::TAG_APPLICATION_RESOURCES_PATH, settings()->path(Core::ISettings::ReadOnlyDatabasesPath));
//        }

        QString dbFileName = databaseFileName();
        QString drugSource = settings()->value(Constants::S_SELECTED_DATABASE_FILENAME).toString();

        // define is default drug sources (fr_FR)
        if (drugSource == DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT || drugSource.isEmpty()) {
            m_IsDefaultDB = true;
            drugSource = DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT;
        } else {
            m_IsDefaultDB = false;
        }

        // log the path of the database
        QString pathToDb = QFileInfo(dbFileName).absolutePath();
        LOG(tr("Searching databases into dir %1").arg(pathToDb));

        // Connect Drugs Database
        if (createConnection(Constants::DB_DRUGS_NAME, QFileInfo(dbFileName).fileName(), pathToDb,
                             Utils::Database::ReadOnly, Utils::Database::SQLite)) {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(QString("%1(%2)").arg(Constants::DB_DRUGS_NAME).arg(dbFileName)).arg("SQLite"));
            d->m_ActualDBInfos = getDrugSourceInformations(drugSource);
        } else {
            LOG_ERROR(tr("No drugs database found."));
            return false;
        }

        // Use routes ?
        //        d->m_UseRoutes = (count(Table_DRUG_ROUTES, DRUG_ROUTES_UID) > 0);

        /** \todo refresh IDrugEngines cache */
    }

    if (!d->m_ActualDBInfos) {
        Utils::warningMessageBox(tr("Drug database source does not exists."),
                                 tr("Switching to the default drugs database source."));
        d->m_ActualDBInfos = getDrugSourceInformations(DrugsDB::Constants::DB_DEFAULT_IDENTIFIANT);
        m_IsDefaultDB = true;
    }

    // Connect and check Dosage Database
    // Check settings --> SQLite or MySQL ?
    // remove drugs database connection if exists
    if (d->m_RefreshDosageBase) {
        if (QSqlDatabase::connectionNames().contains(Dosages::Constants::DB_DOSAGES_NAME)) {
            QSqlDatabase::removeDatabase(Dosages::Constants::DB_DOSAGES_NAME);
        }
        d->m_RefreshDosageBase = false;
    }

    // create dosage database connection
    if (!QSqlDatabase::connectionNames().contains(Dosages::Constants::DB_DOSAGES_NAME)) {
        if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool()) {
            createConnection(Dosages::Constants::DB_DOSAGES_NAME,
                             Dosages::Constants::DB_DOSAGES_NAME,
                             QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST, QByteArray("localhost").toBase64()).toByteArray())),
                             Utils::Database::ReadWrite,
                             Utils::Database::MySQL,
                             QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG, QByteArray("root").toBase64()).toByteArray())),
                             QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS, QByteArray("").toBase64()).toByteArray())),
                             QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT, QByteArray("").toBase64()).toByteArray())).toInt(),
                             Utils::Database::CreateDatabase);
        } else {
            createConnection(Dosages::Constants::DB_DOSAGES_NAME,
                             Dosages::Constants::DB_DOSAGES_FILENAME,
                             settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + QString(Constants::DB_DRUGS_NAME),
                             Utils::Database::ReadWrite, Utils::Database::SQLite,
                             "log", "pas", 0,
                             Utils::Database::CreateDatabase);
        }
        if (!QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME).open())
            Utils::Log::addError("DrugsBase", tr("WARNING : can not open database %1 : %2 \n %3 ")
                                 .arg(Dosages::Constants::DB_DOSAGES_NAME, QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME).lastError().driverText(), QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME).lastError().databaseText()),
                                 __FILE__, __LINE__);
        checkDosageDatabaseVersion();
    }

    d->retreiveLinkTables();
    d->getSearchEngine();
    d->getInteractingClassTree();

    // Initialize
    m_initialized = true;
    return true;
}

/**
  \brief This is for debugging purpose. Log timers for some crucial functions.
  \sa checkInteractions(), getDrugsByUID()
*/
void DrugsBase::logChronos(bool state)
{
    d->m_LogChrono = state;
}

const DatabaseInfos *DrugsBase::actualDatabaseInformations() const
{
    if (!d->m_ActualDBInfos)
        return new DatabaseInfos;
    return d->m_ActualDBInfos;
}

bool DrugsBase::isDatabaseTheDefaultOne() const
{
    return m_IsDefaultDB;
}

bool DrugsBase::isRoutesAvailable() const
{
    return d->m_UseRoutes;
}

bool DrugsBase::refreshAllDatabases()
{
    m_initialized = false;
    d->m_RefreshDrugsBase = true;
    d->m_RefreshDosageBase = true;
    Q_EMIT drugsBaseIsAboutToChange();
    Q_EMIT dosageBaseIsAboutToChange();
    bool r = init();
    if (r) {
        Q_EMIT drugsBaseHasChanged();
        Q_EMIT dosageBaseHasChanged();
    }
    return r;
}

bool DrugsBase::refreshDrugsBase()
{
    m_initialized = false;
    d->m_RefreshDrugsBase = true;
    Q_EMIT drugsBaseIsAboutToChange();
    bool r = init();
    if (r)
        Q_EMIT drugsBaseHasChanged();
    return r;
}

bool DrugsBase::refreshDosageBase()
{
    m_initialized = false;
    d->m_RefreshDosageBase = true;
    Q_EMIT dosageBaseIsAboutToChange();
    bool r = init();
    if (r)
        Q_EMIT dosageBaseHasChanged();
    return r;
}

QVector<DatabaseInfos *> DrugsBase::getAllDrugSourceInformations()
{
    QVector<DatabaseInfos *> infos;
    QString req = select(Constants::Table_SOURCES);
    QSqlQuery q(req, QSqlDatabase::database(Constants::DB_DRUGS_NAME));
    if (q.isActive()) {
        while (q.next()) {
            DatabaseInfos *info = new DatabaseInfos;
            info->version = q.value(Constants::SOURCES_VERSION).toString();
            info->names = d->getAllLabels(q.value(Constants::SOURCES_MASTERLID).toInt());
            info->identifiant = q.value(Constants::SOURCES_DBUID).toString();
            info->compatVersion = q.value(Constants::SOURCES_FMFCOMPAT).toString();
            info->provider = q.value(Constants::SOURCES_PROVIDER).toString();
            info->weblink = q.value(Constants::SOURCES_WEBLINK).toString();
            info->author = q.value(Constants::SOURCES_AUTHORS).toString();
            info->license = q.value(Constants::SOURCES_COPYRIGHT).toString();
//            info->licenseTerms = q.value(Constants::INFO_LICENSE_TERMS).toString();
            info->date = q.value(Constants::SOURCES_DATE).toDate();
            info->drugsUidName = q.value(Constants::SOURCES_DRUGUID_NAME).toString();
            info->packUidName = q.value(Constants::SOURCES_PACKUID_NAME).toString();
            info->atcCompatible = q.value(Constants::SOURCES_ATC).toBool();
            info->iamCompatible = q.value(Constants::SOURCES_INTERACTIONS).toBool();
            info->authorComments = q.value(Constants::SOURCES_AUTHOR_COMMENTS).toString();
            info->lang_country = q.value(Constants::SOURCES_LANG).toString();
            info->setDrugsNameConstructor(q.value(Constants::SOURCES_DRUGNAMECONSTRUCTOR).toString());
            info->complementaryWebsite = q.value(Constants::SOURCES_COMPL_WEBSITE).toString();
            info->moleculeLinkCompletion = q.value(Constants::SOURCES_COMPLETION).toInt();
//            info->connectionName = drugSourceUid;
            if (QSqlDatabase::database(Constants::DB_DRUGS_NAME).driverName() == "QSQLITE") {
                info->fileName = databaseFileName();
            }
            infos << info;
        }
    } else {
        LOG_QUERY_ERROR(q);
    }
    return infos;
}

DatabaseInfos *DrugsBase::getDrugSourceInformations(const QString &drugSourceUid)
{
    DatabaseInfos *info = 0;
    QHash<int, QString> where;
    where.insert(Constants::SOURCES_DBUID, QString("='%1'").arg(drugSourceUid));
    QString req = select(Constants::Table_SOURCES, where);
    QSqlQuery q(req, QSqlDatabase::database(Constants::DB_DRUGS_NAME));
    if (q.isActive()) {
        if (q.next()) {
            LOG("Drugs database informations correctly read " + drugSourceUid);
            info = new DatabaseInfos;
            info->version = q.value(Constants::SOURCES_VERSION).toString();
            info->names = d->getAllLabels(q.value(Constants::SOURCES_MASTERLID).toInt());
            info->identifiant = drugSourceUid;
            info->compatVersion = q.value(Constants::SOURCES_FMFCOMPAT).toString();
            info->provider = q.value(Constants::SOURCES_PROVIDER).toString();
            info->weblink = q.value(Constants::SOURCES_WEBLINK).toString();
            info->author = q.value(Constants::SOURCES_AUTHORS).toString();
            info->license = q.value(Constants::SOURCES_COPYRIGHT).toString();
//            info->licenseTerms = q.value(Constants::INFO_LICENSE_TERMS).toString();
            info->date = q.value(Constants::SOURCES_DATE).toDate();
            info->drugsUidName = q.value(Constants::SOURCES_DRUGUID_NAME).toString();
            info->packUidName = q.value(Constants::SOURCES_PACKUID_NAME).toString();
            info->atcCompatible = q.value(Constants::SOURCES_ATC).toBool();
            info->iamCompatible = q.value(Constants::SOURCES_INTERACTIONS).toBool();
            info->authorComments = q.value(Constants::SOURCES_AUTHOR_COMMENTS).toString();
            info->lang_country = q.value(Constants::SOURCES_LANG).toString();
            info->setDrugsNameConstructor(q.value(Constants::SOURCES_DRUGNAMECONSTRUCTOR).toString());
            info->complementaryWebsite = q.value(Constants::SOURCES_COMPL_WEBSITE).toString();
            info->moleculeLinkCompletion = q.value(Constants::SOURCES_COMPLETION).toInt();
            info->connectionName = drugSourceUid;
            if (QSqlDatabase::database(Constants::DB_DRUGS_NAME).driverName() == "QSQLITE") {
                info->fileName = databaseFileName();
            }
        } else {
            LOG_ERROR(QString("No drug database source for %1").arg(drugSourceUid));
        }
    } else {
        LOG_QUERY_ERROR(q);
    }
    return info;
}

QString DrugsBase::dosageCreateTableSqlQuery()
{
    /** \todo add UserUuid field or lkid */
    return "CREATE TABLE IF NOT EXISTS `DOSAGE` ("
           "`POSO_ID`               INTEGER        PRIMARY KEY AUTOINCREMENT,"
           "`POSO_UUID`             varchar(40)    NULL,"    // put NOT NULL
           "`DRUGS_DATABASE_IDENTIFIANT` varchar(200) NULL,   "
           "`INN_LK`                int(11)        DEFAULT -1,"
           "`INN_DOSAGE`            varchar(100)   NULL,"    // contains the dosage of the SA INN
           "`DRUG_UID_LK`           varchar(20)    NULL,"
           "`CIP_LK`                int(11)        DEFAULT -1,"
           "`LABEL`                 varchar(300)   NULL,"    // put NOT NULL

           "`INTAKEFROM`            double         NULL,"    // put NOT NULL
           "`INTAKETO`              double         NULL,"
           "`INTAKEFROMTO`          bool           NULL,"
           "`INTAKESCHEME`          varchar(200)   NULL,"    // put NOT NULL
           "`INTAKESINTERVALOFTIME` int(10)        NULL,"
           "`INTAKESINTERVALSCHEME` varchar(200)   NULL,"
           "`ROUTE_ID`              integer        NULL,"

           "`DURATIONFROM`          double         NULL,"    // put NOT NULL
           "`DURATIONTO`            double         NULL,"
           "`DURATIONFROMTO`        bool           NULL,"
           "`DURATIONSCHEME`        varchar(200)   NULL,"    // put NOT NULL

           "`PERIOD`                int(10)        NULL,"    // put NOT NULL
           "`PERIODSCHEME`          varchar(200)   NULL,"    // put NOT NULL
           "`ADMINCHEME`            varchar(100)   NULL,"    // put NOT NULL
           "`DAILYSCHEME`           varchar(250)   NULL,"
           "`MEALSCHEME`            int(10)        NULL,"
           "`ISALD`                 bool           NULL,"
           "`TYPEOFTREATEMENT`      int(10)        NULL,"

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
           "`CIM10_LIMITS_LK`       varchar(150)   NULL,"
           "`EDRC_LK`               varchar(150)   NULL,"

           "`EXTRAS`                blob           NULL,"
           "`USERVALIDATOR`         varchar(200)   NULL,"
           "`CREATIONDATE`          date           NULL,"    // put NOT NULL
           "`MODIFICATIONDATE`      date           NULL,"
           "`TRANSMITTED`           date           NULL,"
           "`ORDER`                 int(10)        NULL"
           ");";
}

/** \brief Create the protocols database if it does not exists. */
bool DrugsBase::createDatabase(const QString &connectionName , const QString &dbName,
                               const QString &pathOrHostName,
                               TypeOfAccess /*access*/, AvailableDrivers driver,
                               const QString & login, const QString & pass,
                               const int port,
                               CreationOption /*createOption*/
                              )
{
    /** \todo  ask user if he wants : 1. an empty dosage base ; 2. to retreive dosages from internet FMF website */
    if (connectionName != Dosages::Constants::DB_DOSAGES_NAME)
        return false;

    Utils::Log::addMessage(this, tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
                           .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE" , connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        DB.open();
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "CREATOR");
            d.setHostName(pathOrHostName);
            /** \todo retreive log/pass */
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tr("Unable to create the Protocol database."),tr("Please contact dev team."));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tr("Unable to create the Protocol database."),tr("Please contact dev team."));
                return false;
            }
            DB.setDatabaseName(dbName);
            if (!DB.open()) {
                Utils::warningMessageBox(tr("Unable to create the Protocol database."),tr("Please contact dev team."));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("CREATOR"))
            QSqlDatabase::removeDatabase("CREATOR");
        DB.open();
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    // The SQL scheme MUST BE synchronized with the Dosages::Constants Model Enumerator !!!
    /** \todo code here */
//    if (executeSQL(QStringList()
//        << dosageCreateTableSqlQuery().remove("AUTOINCREMENT")
//        << "CREATE TABLE IF NOT EXISTS `VERSION` ("
//           "`ACTUAL`                varchar(10)    NULL"
//           ");"
//        << QString("INSERT INTO `VERSION` (`ACTUAL`) VALUES('%1');").arg(VersionUpdater::instance()->lastDosageDabaseVersion())
//        , DB)) {
//        Utils::Log::addMessage(this, tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
//        return true;
//    } else {
//        Utils::Log::addError(this, tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
//                         .arg(dbName, DB.lastError().text()),
//                         __FILE__, __LINE__);
//    }
    return false;
}

void DrugsBase::onCoreDatabaseServerChanged()
{
    refreshDosageBase();
}

/**
  \brief Check the version of the doage database. Do the necessary updates for that database according to the application version number.
  Added from freediams version 0.0.8 stable
*/
void DrugsBase::checkDosageDatabaseVersion()
{
//    if (!DrugsDB::VersionUpdater::instance()->isDosageDatabaseUpToDate())
//        if (!DrugsDB::VersionUpdater::instance()->updateDosageDatabase())
//            Utils::Log::addError(this, "Dosage database can not be correctly updated",
//                                 __FILE__, __LINE__);
}

/** \brief Returns hash that contains dosage uuid has key and the xml'd dosage to transmit as value */
QHash<QString, QString> DrugsBase::getDosageToTransmit()
{
    QHash<QString, QString> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;

    QString req = QString("SELECT * FROM `DOSAGE` WHERE (`TRANSMITTED` IS NULL);");
    {
        QSqlQuery query(req,DB);
        if (query.isActive()) {
            while (query.next()) {
                QHash<QString,QString> toXml;
                int i=0;
                for (i=0;i<query.record().count();++i) {
                    // create a XML of the dosage
                    toXml.insert(query.record().field(i).name(), query.value(i).toString());
                }
                toReturn.insert(toXml.value("POSO_UUID"), Utils::createXml(Dosages::Constants::DB_DOSAGES_TABLE_NAME,toXml,4,false));
            }
        } else
            LOG_QUERY_ERROR(query);
    }

    req = QString("SELECT * FROM `DOSAGE` WHERE (`TRANSMITTED`<`MODIFICATIONDATE`);");
    {
        QSqlQuery query(req,DB);
        if (query.isActive()) {
            while (query.next()) {
                QHash<QString,QString> toXml;
                int i=0;
                for (i=0;i<query.record().count();++i) {
                    // create a XML of the dosage
                    toXml.insert(query.record().field(i).name(), query.value(i).toString());
                }
                toReturn.insert(toXml.value("POSO_UUID"), Utils::createXml(Dosages::Constants::DB_DOSAGES_TABLE_NAME,toXml,4,false));
            }
        } else {
            LOG_QUERY_ERROR(query);
        }
    }
    return toReturn;
}

/** Marks all dosages as transmitted now. \e dosageUuids contains dosage uuid. */
bool DrugsBase::markAllDosageTransmitted(const QStringList &dosageUuids)
{
    if (dosageUuids.count()==0)
        return true;
    QSqlDatabase DB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return false;

    QStringList reqs;
    foreach(const QString &s, dosageUuids) {
        QString req = QString("UPDATE `DOSAGE` SET `TRANSMITTED`='%1' WHERE %2")
                      .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
                      .arg(QString("`POSO_UUID`='%1'").arg(s));
        reqs << req;
    }
    if (!executeSQL(reqs,DB)) {
        Utils::Log::addError(this, tr("Unable to update the protocol's transmission date."),
                             __FILE__, __LINE__);
        return false;
    }
    return true;
}

struct minimalCompo {
    bool isLike(const int _inn, const QString &_dosage) const {
        return (inn == _inn) && (dosage == _dosage);
    }
    int inn;
    QString dosage;
};

///////////////////////////////////////////////////////////////////////////////////////////////
/**
  \brief Returns all CIS that have a recorded dosage. Manages INN dosage type.
  \todo put this in a thread...
*/
QList<QVariant> DrugsBase::getAllUIDThatHaveRecordedDosages() const
{
    QList<QVariant> toReturn;

//    if (!actualDatabaseInformations())
//        return toReturn;

//    QSqlDatabase DosageDB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
//    if (!connectDatabase(DosageDB, __FILE__, __LINE__))
//        return toReturn;

//    QString req;
//    if (m_IsDefaultDB) {
//        req = QString("SELECT DISTINCT `DRUG_UID_LK` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
//              .arg(Constants::DB_DEFAULT_IDENTIFIANT);
//    } else {
//        req = QString("SELECT DISTINCT `DRUG_UID_LK` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
//              .arg(actualDatabaseInformations()->identifiant);
//    }
//    {
//        QSqlQuery query(req,DosageDB);
//        if (query.isActive()) {
//            while (query.next()) {
//                toReturn << query.value(0);
//            }
//        } else {
//            LOG_QUERY_ERROR(query);
//        }
//    }

//    // Get all CIS that contains INN that have available dosage
//    QMultiHash<int, QString> inn_dosageRef = getAllINNThatHaveRecordedDosages();

//    // get all code_subst from INNs
//    QHash<int, QString> where;
//    QString tmp;
//    QList<int> code_subst;
//    QSqlDatabase DrugsDB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
//    if (!connectDatabase(DrugsDB, __FILE__, __LINE__))
//        return toReturn;

//    // get all needed datas from database
//    QMultiHash<QString, minimalCompo> cis_compo;
//    foreach(int inn, inn_dosageRef.keys()) {
//        const QVector<int> &molIds = getLinkedMoleculeCodes(inn);
//        for(int i = 0; i < molIds.count(); ++i) {
//            int code = molIds.at(i);
//            if (!code_subst.contains(code))
//                code_subst << code;
//        }
//    }
//    foreach(int code, code_subst) {
//        tmp += QString::number(code) + ", ";
//    }
//    tmp.chop(2);

//    // Get all Drugs UID that contains the substance + dosage
//    where.clear();
//    req.clear();
//    where.insert(Constants::COMPO_MOL_CODE, QString("IN (%1)").arg(tmp));
//    req = select(Constants::Table_COMPO,
//                 QList<int>() << Constants::COMPO_UID
//                 << Constants::COMPO_MOL_CODE
//                 << Constants::COMPO_DOSAGE,
//                 where);
//    QSqlQuery query(req, DrugsDB);
//    if (query.isActive()) {
//        while (query.next()) {
//            QString cis = query.value(0).toString();
//            minimalCompo compo;
//            compo.inn = getLinkedAtcIds(query.value(1).toInt()).at(0);
//            compo.dosage = query.value(2).toString();
//            cis_compo.insertMulti(cis, compo);
//        }
//    } else {
//        LOG_QUERY_ERROR(query);
//    }

//    // now check every drugs
//    foreach(const QString &uid, cis_compo.uniqueKeys()) {
//        QList<int> innsOfThisDrug;
//        foreach(const minimalCompo &compo, cis_compo.values(uid)) {
//            if (!innsOfThisDrug.contains(compo.inn))
//                innsOfThisDrug << compo.inn;
//            QString d = compo.dosage;
//            foreach(const QString &r, inn_dosageRef.values(compo.inn)) {
//                // remove unneeded strings
//                if (d == r)
//                    toReturn << uid;
//                else if (d.remove(",000") == r)
//                    toReturn << uid;
//                else if (d.remove(",00") == r)
//                    toReturn << uid;

//                // try unit conversion
//                if (d.replace("000 mg", " g") == r) {
//                    toReturn << uid;
//                }
//                QString t = r;
//                if (t.replace("000 mg", " g") == d) {
//                    toReturn << uid;
//                }
//            }
//        }
//        if (innsOfThisDrug.count() > 1)
//            toReturn.removeAll(uid);
//    }
    return toReturn;
}

QMultiHash<int,QString> DrugsBase::getAllINNThatHaveRecordedDosages() const
{
    QMultiHash<int,QString> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Dosages::Constants::DB_DOSAGES_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;

    QString req;
    if (m_IsDefaultDB) {
        req = QString("SELECT DISTINCT `INN_LK`, `INN_DOSAGE` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
              .arg(Constants::DB_DEFAULT_IDENTIFIANT);
    } else {
        req = QString("SELECT DISTINCT `INN_LK`, `INN_DOSAGE` FROM `DOSAGE` WHERE `DRUGS_DATABASE_IDENTIFIANT` = \"%1\";")
              .arg(actualDatabaseInformations()->identifiant);
    }
    QSqlQuery query(req,DB);
    if (query.isActive()) {
        while (query.next()) {
            toReturn.insertMulti(query.value(0).toInt(), query.value(1).toString());
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------------
//-------------------------------- Retreive drugs from database ------------------------------------------
//--------------------------------------------------------------------------------------------------------
QString DrugsBase::getDrugName(const QString &uid1, const QString &uid2, const QString &uid3) const
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString();

    // prepare query
    Utils::FieldList conditions;
    conditions << Utils::Field(Constants::Table_MASTER, Constants::MASTER_UID1, QString("='%1'").arg(uid1));
    if (!uid2.isEmpty())
        conditions << Utils::Field(Constants::Table_MASTER, Constants::MASTER_UID2, QString("='%1'").arg(uid2));
    if (!uid3.isEmpty())
        conditions << Utils::Field(Constants::Table_MASTER, Constants::MASTER_UID3, QString("='%1'").arg(uid3));
    Utils::Join j(Constants::Table_MASTER, Constants::MASTER_DID, Constants::Table_DRUGS, Constants::DRUGS_DID);
    Utils::Field nameField(Constants::Table_DRUGS, Constants::DRUGS_NAME);
    QString req = select(nameField, j, conditions);
    QSqlQuery q(req , DB);
    if (q.isActive()) {
        if (q.next()) {
            return q.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(q);
    }
    return QString();
}

/** \brief Retrieve and return the drug designed by the UID code \e drug_UID. */
IDrug *DrugsBase::getDrugByUID(const QVariant &uid1, const QVariant &uid2, const QVariant &uid3)
{
    if (!d->m_ActualDBInfos) {
        LOG_ERROR(tr("No drug database source selected"));
        IDrug *toReturn = new IDrug;
        toReturn->setDataFromDb(IDrug::Name, tr("No drugs database loaded."));
        return toReturn;
    }
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return 0;

    // if uid1 is -1 get the first available drug
    QString newUid1 = uid1.toString();
    QString newUid2 = uid2.toString();
    QString newUid3 = uid3.toString();
    if (newUid1 == "-1" || newUid1.isEmpty()) {
        LOG(tr("Asking for a drug without UID"));
        QString req = select(Constants::Table_MASTER, QList<int>()
                             << Constants::MASTER_UID1
                             << Constants::MASTER_UID2
                             << Constants::MASTER_UID3) + " LIMIT 1";
        QSqlQuery q(req,DB);
        if (q.isActive()) {
            if (q.next()) {
                newUid1 = q.value(0).toString();
                newUid2 = q.value(1).toString();
                newUid3 = q.value(2).toString();
            }
        } else {
            LOG_ERROR("Can find a valid DRUGS_UID in getDrugByUID where uid==-1");
            LOG_QUERY_ERROR(q);
            return 0;
        }
    }

    // prepare query
    Utils::FieldList condition;
    condition << Utils::Field(Constants::Table_MASTER, Constants::MASTER_UID1, QString("='%1'").arg(newUid1));
    if (!uid2.isNull())
        condition << Utils::Field(Constants::Table_MASTER, Constants::MASTER_UID2, QString("='%1'").arg(newUid2));
    if (!uid3.isNull())
        condition << Utils::Field(Constants::Table_MASTER, Constants::MASTER_UID3, QString("='%1'").arg(newUid3));
    Utils::Join j(Constants::Table_MASTER, Constants::MASTER_DID, Constants::Table_DRUGS, Constants::DRUGS_DID);
    QString req = select(Constants::Table_DRUGS, j, condition);
    IDrug *toReturn = 0;
        QSqlQuery q(req , DB);
        if (q.isActive()) {
            if (q.next()) {
                toReturn = new IDrug();
                toReturn->setDataFromDb(IDrug::Uid1, uid1);
                toReturn->setDataFromDb(IDrug::Uid2, uid2);
                toReturn->setDataFromDb(IDrug::Uid3, uid3);
                /** \todo add olduid */
                toReturn->setDataFromDb(IDrug::OldUid, QVariant());
                toReturn->setDataFromDb(IDrug::DrugID, q.value(Constants::DRUGS_DID));
                toReturn->setDataFromDb(IDrug::SourceID, q.value(Constants::DRUGS_SID));
//                toReturn->setDataFromDb(IDrug::AuthorizationID, q.value(Constants::DRUGS_AID_MASTER_LID));
                toReturn->setDataFromDb(IDrug::Name, q.value(Constants::DRUGS_NAME));
                toReturn->setDataFromDb(IDrug::AtcId, q.value(Constants::DRUGS_ATC_ID));
                toReturn->setDataFromDb(IDrug::AtcCode, getAtcCode(q.value(Constants::DRUGS_ATC_ID).toInt()));
                toReturn->setDataFromDb(IDrug::Strength, q.value(Constants::DRUGS_STRENGTH));
                toReturn->setDataFromDb(IDrug::Valid, q.value(Constants::DRUGS_VALID));
                toReturn->setDataFromDb(IDrug::Marketed, q.value(Constants::DRUGS_MARKET));
                toReturn->setDataFromDb(IDrug::Spc, q.value(Constants::DRUGS_LINK_SPC));
            } else {
                // drug not found --> break
                LOG_QUERY_ERROR(q);
                return toReturn;
            }

            // manage drugs denomination according to the database informations
            QString tmp = d->m_ActualDBInfos->drugsNameConstructor;
            if (!tmp.isEmpty()) {
                tmp.replace(fieldName(Constants::Table_DRUGS, Constants::DRUGS_NAME), toReturn->brandName());
                tmp.replace("FORM", toReturn->forms().join(","));
                tmp.replace("ROUTE", toReturn->routes().join(","));
                // limit strength to three maximum --> if > 3 do not add strength
                if (toReturn->strength().count(";") >= 3)
                    tmp.replace(fieldName(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH), "");
                else
                    tmp.replace(fieldName(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH), toReturn->strength());
                toReturn->setDataFromDb(IDrug::Name, tmp);
            }
        } else {
            LOG_QUERY_ERROR(q);
            return toReturn;
        }
    q.finish();

    // get COMPO table
    d->getDrugComponents(toReturn);

    toReturn->constructAtcIdsVectorsUsingComponents();

    /** \todo add drug ATC code and related ids */

    // get ROUTES
    d->getRoutes(toReturn);

    if (WarnExtractedDrugs && toReturn)
        qWarning() << toReturn->warnText();

    return toReturn;
}

QVariantList DrugsBase::getDrugUids(const QVariant &drugId)
{
    QVariantList list;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return list;
    QHash<int, QString> where;
    where.insert(Constants::MASTER_DID, QString("='%1'").arg(drugId.toString()));
    QString req = select(Constants::Table_MASTER, where);
    QSqlQuery query(req , DB);
    if (query.isActive()) {
        if (query.next()) {
            list << query.value(Constants::MASTER_UID1) << query.value(Constants::MASTER_UID2) << query.value(Constants::MASTER_UID3);
        } else {
            LOG_ERROR(tr("Unable to find drug with DID %1").arg(drugId.toString()));
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return list;
}

IDrug *DrugsBase::getDrugByDrugId(const QVariant &drugId)
{
    QVariantList list = getDrugUids(drugId);
    if (list.count() == 3)
        return getDrugByUID(list.at(0), list.at(1), list.at(2));
    return 0;
}

QStringList DrugsBase::getDrugCompositionAtcCodes(const QVariant &drugId)
{
    Utils::FieldList get;
    get << Utils::Field(Constants::Table_ATC, Constants::ATC_CODE);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_LK_MOL_ATC, Constants::LK_MID, Constants::Table_COMPO, Constants::COMPO_MID);
    joins << Utils::Join(Constants::Table_ATC, Constants::ATC_ID, Constants::Table_LK_MOL_ATC, Constants::LK_ATC_ID);
    Utils::FieldList cond;
    cond << Utils::Field(Constants::Table_COMPO, Constants::COMPO_DID, QString("='%1'").arg(drugId.toString()));

    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QStringList();

    QStringList atcs;
    QString req = select(get,joins,cond);
    req.replace("SELECT", "SELECT DISTINCT");
    QSqlQuery query(req, DB);
    if (query.isActive()) {
        while (query.next()) {
            atcs << query.value(0).toString();
        }
    }
    return atcs;
}

QStringList DrugsBase::getDrugInns(const QVariant &drugId)
{
    QStringList toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;
    QString lang = QLocale().name().left(2);
    if (!(lang=="en" || lang=="fr" || lang=="de"))
        lang = "en";
    Utils::FieldList get;
    get << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LABEL);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_LK_MOL_ATC, Constants::LK_MID, Constants::Table_COMPO, Constants::COMPO_MID);
    joins << Utils::Join(Constants::Table_ATC_LABELS, Constants::ATC_LABELS_ATCID, Constants::Table_LK_MOL_ATC, Constants::LK_ATC_ID);
    joins << Utils::Join(Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID, Constants::Table_ATC_LABELS, Constants::ATC_LABELS_MASTERLID);
    joins << Utils::Join(Constants::Table_LABELS, Constants::LABELS_LID, Constants::Table_LABELSLINK, Constants::LABELSLINK_LID);
    Utils::FieldList cond;
    cond << Utils::Field(Constants::Table_COMPO, Constants::COMPO_DID, QString("='%1'").arg(drugId.toString()));
    cond << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LANG, QString("='%1'").arg(lang));
    QString req = select(get,joins,cond);
    QSqlQuery query(req, DB);
    if (query.isActive()) {
        while (query.next()) {
            toReturn.append(query.value(0).toString());
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

QStringList DrugsBase::getRouteLabels(const QVariant &drugId, const QString &lang)
{
    QStringList toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;

    // get all routes labels
    QString l = lang;
    if (l.isEmpty()) {
        l = QLocale().name().left(2);
    }
    if (!(l=="en" || l=="de" || l=="fr"))
        l = "en";

    Utils::FieldList get;
    get << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LABEL);
    Utils::FieldList cond;
    cond << Utils::Field(Constants::Table_DRUG_ROUTES, Constants::DRUG_ROUTES_DID, QString("='%1'").arg(drugId.toString()));
    cond << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LANG, QString("='%1'").arg(l));
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_DRUG_ROUTES, Constants::DRUG_ROUTES_RID, Constants::Table_ROUTES, Constants::ROUTES_RID);
    joins << Utils::Join(Constants::Table_ROUTES, Constants::ROUTES_MASTERLID, Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID);
    joins << Utils::Join(Constants::Table_LABELSLINK, Constants::LABELSLINK_LID, Constants::Table_LABELS, Constants::LABELS_LID);
    QString req = select(get, joins, cond);
    QSqlQuery query(req, DB);
    if (query.isActive()) {
        while (query.next()) {
            toReturn << query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

QStringList DrugsBase::getFormLabels(const QVariant &drugId, const QString &lang)
{
    QStringList toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;

    // get all routes labels
    QString l = lang;
    if (l.isEmpty()) {
        l = "xx";
    }
    Utils::FieldList get;
    get << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LABEL);
    Utils::FieldList cond;
    cond << Utils::Field(Constants::Table_DRUG_FORMS, Constants::DRUG_FORMS_DID, QString("='%1'").arg(drugId.toString()));
    cond << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LANG, QString("='%1'").arg(l));
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_DRUG_FORMS, Constants::DRUG_FORMS_MASTERLID, Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID);
    joins << Utils::Join(Constants::Table_LABELSLINK, Constants::LABELSLINK_LID, Constants::Table_LABELS, Constants::LABELS_LID);
    QString req = select(get, joins, cond);
    QSqlQuery query(req, DB);
    if (query.isActive()) {
        while (query.next()) {
            toReturn << query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

QHash<QString, QString> DrugsBase::getDrugFullComposition(const QVariant &drugId, const QString &lang)
{
    QHash<QString, QString> toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;

    QString l = lang;
    if (l.isEmpty()) {
        l = QLocale().name().left(2);
    }
    if (!(l=="en" || l=="de" || l=="fr"))
        l = "en";

    Utils::FieldList get;
    get << Utils::Field(Constants::Table_MOLS, Constants::MOLS_NAME);
    get << Utils::Field(Constants::Table_COMPO, Constants::COMPO_STRENGTH);
    get << Utils::Field(Constants::Table_COMPO, Constants::COMPO_DOSE_REF);
    get << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LABEL);
    Utils::FieldList cond;
    cond << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LANG, QString("='%1'").arg(l));
    cond << Utils::Field(Constants::Table_COMPO, Constants::COMPO_DID, QString("='%1'").arg(drugId.toString()));
    Utils::JoinList joins;
    joins
            << Utils::Join(Constants::Table_MOLS, Constants::MOLS_MID, Constants::Table_COMPO, Constants::COMPO_MID)
            << Utils::Join(Constants::Table_LK_MOL_ATC, Constants::LK_MID, Constants::Table_MOLS, Constants::MOLS_MID)
            << Utils::Join(Constants::Table_ATC_LABELS, Constants::ATC_LABELS_ATCID, Constants::Table_LK_MOL_ATC, Constants::LK_ATC_ID)
            << Utils::Join(Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID, Constants::Table_ATC_LABELS, Constants::ATC_LABELS_MASTERLID)
            << Utils::Join(Constants::Table_LABELS, Constants::LABELS_LID, Constants::Table_LABELSLINK, Constants::LABELSLINK_LID);

    QString req = select(get,joins,cond);
    QSqlQuery query(req, DB);
    if (query.isActive()) {
        while (query.next()) {
            QString dosage = query.value(1).toString();
            if (!query.value(2).toString().isEmpty())
                dosage += "/" + query.value(2).toString();
            toReturn.insert(query.value(0).toString() + " " + dosage,
                            query.value(3).toString() + " " + dosage);
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

QStringList DrugsBase::getDrugMolecularComposition(const QVariant &drugId, const QString &lang)
{
    QStringList toReturn;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_DRUGS_NAME);
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return toReturn;

    Utils::FieldList get;
    get << Utils::Field(Constants::Table_MOLS, Constants::MOLS_NAME);
    get << Utils::Field(Constants::Table_COMPO, Constants::COMPO_STRENGTH);
    get << Utils::Field(Constants::Table_COMPO, Constants::COMPO_DOSE_REF);
    Utils::FieldList cond;
    cond << Utils::Field(Constants::Table_COMPO, Constants::COMPO_DID, QString("='%1'").arg(drugId.toString()));
    Utils::JoinList joins;
    joins
            << Utils::Join(Constants::Table_MOLS, Constants::MOLS_MID, Constants::Table_COMPO, Constants::COMPO_MID);

    QString req = select(get,joins,cond);
    QSqlQuery query(req, DB);
    if (query.isActive()) {
        while (query.next()) {
            QString dosage = query.value(1).toString();
            if (!query.value(2).toString().isEmpty())
                dosage += "/" + query.value(2).toString();
            toReturn << query.value(0).toString() + " " + dosage;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

/**
  \brief Return the Inn code linked to the molecule code. Returns -1 if no inn is linked to that molecule.
  \todo this should return a list of integer not an unique integer
*/
int DrugsBase::getAtcCodeForMoleculeId(const int molId) const
{
    if (d->m_AtcToMol.values().contains(molId))
        return d->m_AtcToMol.key(molId);
    return -1;
}

QString DrugsBase::getAtcLabel(const int atcId) const
{
    if (atcId==-1)
        return QString();

    QString lang = QLocale().name().left(2);
    if (!(lang=="fr" || lang=="de" || lang=="en"))
        lang = "en";

    if (d->m_AtcLabelCache.contains(atcId)) {
        const AtcLabel *lbl = d->m_AtcLabelCache[atcId];
        if (lbl->lang==lang) {
            return lbl->label;
        }
    }

    // get allLabels from Table_ATC_LABELS
    Utils::FieldList get;
    get << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LABEL);
    Utils::FieldList cond;
    cond << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LANG, QString("='%1'").arg(lang));
    cond << Utils::Field(Constants::Table_ATC, Constants::ATC_ID, QString("='%1'").arg(atcId));
    Utils::JoinList joins;
    joins   << Utils::Join(Constants::Table_ATC_LABELS, Constants::ATC_LABELS_ATCID, Constants::Table_ATC, Constants::ATC_ID)
            << Utils::Join(Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID, Constants::Table_ATC_LABELS, Constants::ATC_LABELS_MASTERLID)
            << Utils::Join(Constants::Table_LABELS, Constants::LABELS_LID, Constants::Table_LABELSLINK, Constants::LABELSLINK_LID);
    QSqlQuery query(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
    if (!query.exec(select(get,joins,cond))) {
        LOG_QUERY_ERROR(query);
    } else {
        if (query.next()) {
            AtcLabel *lbl = new AtcLabel;
            lbl->id = atcId;
            lbl->lang = lang;
            lbl->label = query.value(0).toString();
            d->m_AtcLabelCache.insert(atcId, lbl);
            return lbl->label;
        } else {
            LOG_QUERY_ERROR(query);
        }
    }
    return QString();
}

QString DrugsBase::getAtcLabel(const QString &code) const
{
    if (d->m_AtcCodeCacheCodeKeyed.keys().contains(code)) {
        return getAtcLabel(d->m_AtcCodeCacheCodeKeyed[code]);
    }

    // get ATC_ID from ATC table
    QHash<int, QString> where;
    where.insert(Constants::ATC_CODE, QString("='%1'").arg(code));
    QSqlQuery query(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
    if (!query.exec(select(Constants::Table_ATC, Constants::ATC_ID, where))) {
        LOG_QUERY_ERROR(query);
    } else {
        if (query.next()) {
            int atcId = query.value(0).toInt();
            d->m_AtcCodeCacheCodeKeyed.insert(code, atcId);
            return getAtcLabel(atcId);
        }
    }
    return QString();
}

QString DrugsBase::getAtcCode(const int atcId) const
{
    if (atcId==-1)
        return QString();

    if (d->m_AtcCodeCacheIdKeyed.contains(atcId)) {
        return *d->m_AtcCodeCacheIdKeyed[atcId];
    }

    QString toReturn;
    QHash<int, QString> where;
    where.insert(Constants::ATC_ID, QString("=%1").arg(atcId));
    QSqlQuery query(QSqlDatabase::database(Constants::DB_DRUGS_NAME));
    if (!query.exec(select(Constants::Table_ATC, Constants::ATC_CODE, where))) {
        Utils::Log::addQueryError("InteractionBase", query, __FILE__, __LINE__);
        return QString();
    } else {
        if (query.next())
            toReturn = query.value(0).toString();
    }
    d->m_AtcCodeCacheIdKeyed.insert(atcId, new QString(toReturn));
    return toReturn;
}

/** \brief Returns the name of the INN for the molecule id \e molId. */
QString DrugsBase::getInnDenominationFromSubstanceCode(const int molId) const
{
     if (!d->m_AtcToMol.values().contains(molId))
          return QString::null;
     return getAtcLabel(d->m_AtcToMol.key(molId));
}

///** \brief Returns the name of the interacting classes for the molecule id \e molId. */
//QStringList DrugsBase::getIamClassDenomination(const int molId)
//{
//     // if molecule is not associated with a dci exit
//     if (!d->m_AtcToMol.values().contains(molId))
//          return QStringList();

//     // if molecule is not associated with a class exit
//     QList<int> list = d->m_ClassToAtcs.keys(d->m_AtcToMol.key(molId));
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
//QVector<int> DrugsBase::getAllInnAndIamClassesIndex(const int molId)
//{
//    /** \todo obsolete ? */
//    QVector<int> toReturn;

////    if (di->m_AtcToMol.keys(molecule_code).count()>1)
////        Utils::Log::addError("InteractionBase", "Molecule got multiATC " + QString::number(molecule_code), __FILE__, __LINE__);

//    foreach(int id, d->m_AtcToMol.keys(molId)) {
//        toReturn = d->m_ClassToAtcs.keys(id).toVector();
//    }
//    if (d->m_AtcToMol.values().contains(molId))
//        toReturn << d->m_AtcToMol.key(molId);

////    qWarning() << Q_FUNC_INFO << molecule_code << toReturn;
//    return toReturn;
//}

/** \brief Return the list of the code of the substances linked to the INN code \e code_iam. */
QVector<int> DrugsBase::getLinkedMoleculeCodes(QVector<int> &atc_ids) const
{
    QVector<int> toReturn;
    foreach(int i, atc_ids)
        toReturn << d->m_AtcToMol.values(i).toVector();
    return toReturn;
}

/** \brief Return the list of the code of the substances linked to the INN code \e code_iam. */
QVector<int> DrugsBase::getLinkedMoleculeCodes(const int atc_id) const
{
    return d->m_AtcToMol.values(atc_id).toVector();
}

/** \brief Retreive from database the Molecules Codes where denomination of the INN is like 'iamDenomination%' */
QVector<int> DrugsBase::getLinkedMoleculeCodes(const QString &atcLabel) const
{
    // get MID
    Utils::Field get(Constants::Table_ATC_LABELS, Constants::ATC_LABELS_ATCID);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_LABELS, Constants::LABELS_LID, Constants::Table_LABELSLINK, Constants::LABELSLINK_LID);
    joins << Utils::Join(Constants::Table_ATC_LABELS, Constants::ATC_LABELS_MASTERLID, Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID);
    Utils::Field cond(Constants::Table_LABELS, Constants::LABELS_LABEL, QString("='%1'").arg(atcLabel));

    QVector<int> atcIds;
    QString req = select(get, joins, cond);
    QSqlQuery q(req , QSqlDatabase::database(Constants::DB_DRUGS_NAME));
    if (q.isActive())
        while (q.next())
            atcIds << q.value(0).toInt();
    return getLinkedMoleculeCodes(atcIds);
}

/** \brief Return the list of the INN code linked to the substances code \e code_subst. */
QVector<int> DrugsBase::getLinkedAtcIds(const QVector<int> &molIds) const
{
    QVector<int> toReturn;
    for(int i = 0; i < molIds.count(); ++i) {
        toReturn << d->m_AtcToMol.keys(molIds.at(i)).toVector();
    }
    return toReturn;
}

/** \brief Return the list of the INN code linked to the molecule code \e code_subst. */
QVector<int> DrugsBase::getLinkedAtcIds(const int molId) const
{
    return d->m_AtcToMol.keys(molId).toVector();
}

QVector<int> DrugsBase::getAllMoleculeCodeWithAtcStartingWith(const QString &code) const
{
    QHash<int, QString> where;
    where.insert(Constants::ATC_CODE, QString("LIKE '%1%'").arg(code));
    QVector<int> atcIds;
    QString req = select(Constants::Table_ATC, Constants::ATC_ID, where);
    QSqlQuery q(req , QSqlDatabase::database(Constants::DB_DRUGS_NAME));
    if (q.isActive())
        while (q.next())
            atcIds << q.value(0).toInt();
    return getLinkedMoleculeCodes(atcIds);
}

QVector<MedicalUtils::EbmData *> DrugsBase::getAllSourcesFromTree(const QList<int> &allInnAndInteractingClassesIds)
{
    /** \todo code this */
    QVector<MedicalUtils::EbmData *> ret;
//    if (allInnAndIamClassIds.count() == 0)
//        return ret;

//    QStringList classIds, innIds;
//    foreach(int id, allInnAndIamClassIds) {
//        if (id >= 200000)
//            classIds << QString::number(id);
//        else
//            innIds <<QString::number(id);
//    }

//    // get all source_link
////    QHash<int, QString> where;
////    where.insert(TREE_ID_CLASS, QString("IN (%1)").arg(classIds.join(",")));
////    where.insert(TREE_ID_ATC, QString("IN (%1)").arg(innIds.join(",")));
//    QString req = QString("%1, %2 WHERE "
//                          "`%2`.`%3` IN (%5) AND `%2`.`%4` IN (%6) AND %7")
//            .arg(di->m_DB->select(Table_SOURCES))
//            .arg(di->m_DB->table(Table_IAM_TREE))
//            .arg(di->m_DB->fieldName(Table_IAM_TREE, TREE_ID_CLASS))
//            .arg(di->m_DB->fieldName(Table_IAM_TREE, TREE_ID_ATC))
//            .arg(classIds.join(","))
//            .arg(innIds.join(","))
//            .arg(di->m_DB->fieldEquality(Table_IAM_TREE, TREE_SOURCE_LINK,
//                                         Table_SOURCES, SOURCES_SOURCE_LINK))
//            ;

//    QStringList links;
//    QSqlQuery query(req, di->m_DB->database());
//    if (query.isActive()) {
//        while (query.next()) {
//            if (links.contains(query.value(SOURCES_LINK).toString()))
//                continue;
//            links << query.value(SOURCES_LINK).toString();
//            MedicalUtils::EbmData *ebm = new MedicalUtils::EbmData;
//            ebm->setId(query.value(SOURCES_ID));
//            ebm->setLink(query.value(SOURCES_LINK).toString());
//            ebm->setReferences(query.value(SOURCES_TEXTUAL_REFERENCE).toString());
//            ebm->setAbstract(query.value(SOURCES_ABSTRACT).toString());
//            ret << ebm;
//        }
//    } else {
//        Utils::Log::addQueryError("InteractionBase", query, __FILE__, __LINE__);
//    }

    return ret;
}
