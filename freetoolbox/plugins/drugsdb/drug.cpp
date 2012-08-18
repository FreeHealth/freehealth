#include "drug.h"

#include <coreplugin/globaltools.h>
#include <coreplugin/ftb_constants.h>

#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <utils/log.h>

#include <QVector>
#include <QLocale>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <QHash>


class ComponentPrivate
{
public:
    QHash<int, QMultiHash<QString, QVariant> > m_Content;
};


class DrugPrivate
{
public:
    QHash<int, QMultiHash<QString, QVariant> > m_Content;
    QVector<Component *> m_Compo;
};


Component::Component() :
    d(new ComponentPrivate)
{
}

Component::~Component()
{
    delete d;
}

bool Component::setData(const int ref, const QVariant &value, const QString &lang)
{
    d->m_Content[ref].insertMulti(lang, value);
    return true;
}

QVariant Component::data(const int ref, const QString &lang) const
{
    if (!d->m_Content.keys().contains(ref))
        return QVariant();
    return d->m_Content.value(ref).value(lang);
}


Drug::Drug() :
    d(new DrugPrivate)
{
}

Drug::~Drug()
{
    qDeleteAll(d->m_Compo);
    d->m_Compo.clear();
    delete d;
}

/**
  \brief Define drugs data.
  When setting Drug::Routes, the ids are guessed from the official routes.
  When setting Drug::Forms, labels and link to label are automatically created.
  For Routes and Forms, must be set as QStringList.
*/
bool Drug::setData(const int ref, const QVariant &value, const QString &lang)
{
    switch (ref) {
    case Routes:
    {
        // calculate RIDs
        QList<QVariant> rids;
        QStringList routes = value.toStringList();
        routes.removeAll("");
        routes.removeDuplicates();
        d->m_Content[Drug::Routes].insertMulti(lang, routes);
        foreach(QString route, routes) {
            route = route.toLower().simplified();
            QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
            QString req;
            QSqlQuery query(db);
            // Find the LID of the route
//            req = QString("SELECT ROUTES.RID "
//                          "FROM LABELS "
//                          "JOIN LABELS_LINK ON LABELS_LINK.LID=LABELS.LID "
//                          "JOIN ROUTES ON ROUTES.MASTER_LID=LABELS_LINK.MASTER_LID "
//                          "WHERE LABELS.LABEL=\"%1\"")
//                    .arg(route);
            Utils::FieldList get;
            get << Utils::Field(DrugsDB::Constants::Table_ROUTES, DrugsDB::Constants::ROUTES_RID);
            Utils::JoinList joins;
            joins << Utils::Join(DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_MASTERLID, DrugsDB::Constants::Table_ROUTES, DrugsDB::Constants::ROUTES_MASTERLID);
            joins << Utils::Join(DrugsDB::Constants::Table_LABELS, DrugsDB::Constants::LABELS_LID, DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_LID);
//            joins << Utils::Join(DrugsDB::Constants::Table_LABELS, DrugsDB::Constants::LABELS_LID, DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_LID);
//                  << Utils::Join(DrugsDB::Constants::Table_ROUTES, DrugsDB::Constants::LABELS_LID, DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_LID);
            Utils::FieldList cond;
            cond << Utils::Field(DrugsDB::Constants::Table_LABELS, DrugsDB::Constants::LABELS_LABEL, QString("='%1'").arg(route));

//            qWarning() << Core::Tools::drugBase()->select(get,joins,cond);

            if (query.exec(Core::Tools::drugBase()->select(get,joins,cond))) {
                if (query.next()) {
                    rids << query.value(0);
                }
            } else {
                LOG_QUERY_ERROR_FOR("Drug", query);
            }
            if (rids.count() > 0) {
                d->m_Content[Drug::RoutesId].insertMulti(lang, rids);
            } else {
                LOG_ERROR_FOR("Drug", "RouteID not found for " + route);
            }
        }
        break;
    }
    case Forms: // TODO: manage drugs forms ?
        break;
    }
    d->m_Content[ref].insertMulti(lang, value);
    return true;
}

QVariant Drug::data(const int ref, const QString &lang) const
{
    if (!d->m_Content.keys().contains(ref))
        return QVariant();
    return d->m_Content.value(ref).value(lang);
}

void Drug::addComponent(Component *compo)
{
    d->m_Compo.append(compo);
}

QVector<Component *> Drug::components() const
{
    return d->m_Compo;
}

bool Drug::toDatabase(const QString &dbConnection,
                      const QHash<int, QString> &mids,
                      const QString &lang
                      )
{
    QString req;
    QSqlDatabase db = QSqlDatabase::database(dbConnection);
    if (!db.isOpen()) {
        if (!db.open()) {
            return false;
        }
    }
    db.transaction();
    QSqlQuery query(db);

    // Master
//    req = QString("INSERT INTO `MASTER` (DID,UID1,UID2,UID3,OLD_UID,SID) VALUES ("
//                  "NULL,'%1','%2','%3','%4',%5)")
//            .arg(data(Uid1, lang).toString())
//            .arg(data(Uid2, lang).toString())
//            .arg(data(Uid3, lang).toString())
//            .arg(data(OldUid, lang).toString())
//            .arg(data(SID, lang).toString())
//            ;
    query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_MASTER));
    query.bindValue(DrugsDB::Constants::MASTER_DID, QVariant());
    query.bindValue(DrugsDB::Constants::MASTER_UID1, data(Uid1, lang).toString());
    query.bindValue(DrugsDB::Constants::MASTER_UID2, data(Uid2, lang).toString());
    query.bindValue(DrugsDB::Constants::MASTER_UID3, data(Uid3, lang).toString());
    query.bindValue(DrugsDB::Constants::MASTER_OLDUID, data(OldUid, lang).toString());
    query.bindValue(DrugsDB::Constants::MASTER_SID, data(SID, lang).toString());
    if (query.exec()) {
        setData(DID, query.lastInsertId());
    } else {
        LOG_QUERY_ERROR_FOR("Drugs", query);
        db.rollback();
        return false;
    }
    query.finish();

    // Authorizations
    Core::Tools::addLabels(dbConnection, -1, d->m_Content.value(Authorization));

    // Drugs
//    req = QString("INSERT INTO `DRUGS` ("
//                  "DID,SID,NAME,ATC_ID,STRENGTH,AID_MASTER_LID,VALID,MARKETED,LINK_SPC,EXTRA_XML) VALUES ("
//                  "%1  ,%2,'%3',  %4,   '%5',        %6,         %7,     %8,   '%9'  ,  '%10')")
//            .arg(data(DID, lang).toString())
//            .arg(data(SID, lang).toString())
//            .arg(data(Name, lang).toString().replace("'","''"))
//            .arg(data(AtcId, lang).toString())
//            .arg(data(Strength, lang).toString())
//            .arg(data(AID, lang).toString())
//            .arg(data(Valid, lang).toString())
//            .arg(data(Marketed, lang).toInt())
//            .arg(data(Spc, lang).toString())
//            .arg(data(Xml, lang).toString())
//            ;
//    while (req.contains("  "))
//        req = req.replace("  ", " ");
//    req.replace(", ,", ", NULL ,");
//    req.replace(",,", ", NULL ,");
//    req.replace(", ,", ", NULL ,");
//    req.replace(",,", ", NULL ,");
    query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_DRUGS));
    query.bindValue(DrugsDB::Constants::DRUGS_ID, QVariant());
    query.bindValue(DrugsDB::Constants::DRUGS_DID, data(DID, lang).toString());
    query.bindValue(DrugsDB::Constants::DRUGS_SID, data(SID, lang).toString().replace("'","''"));
    query.bindValue(DrugsDB::Constants::DRUGS_NAME, data(Name, lang).toString().replace("'","''"));
    query.bindValue(DrugsDB::Constants::DRUGS_ATC_ID, data(AtcId, lang).toString());
    query.bindValue(DrugsDB::Constants::DRUGS_STRENGTH, data(Strength, lang).toString());
    query.bindValue(DrugsDB::Constants::DRUGS_AID_MASTER_LID, data(AID, lang).toString());
    query.bindValue(DrugsDB::Constants::DRUGS_VALID, data(Valid, lang).toInt());
    query.bindValue(DrugsDB::Constants::DRUGS_MARKET, data(Marketed, lang).toInt());
    query.bindValue(DrugsDB::Constants::DRUGS_LINK_SPC, data(Spc, lang).toString());
    query.bindValue(DrugsDB::Constants::DRUGS_EXTRA_XML, data(Xml, lang).toString());

    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Drugs", query);
        db.rollback();
        return false;
    }
    query.finish();

    // Composition
    foreach(Component *compo, d->m_Compo) {
//        req = QString("INSERT INTO `COMPOSITION` ("
//                      "DID, MID, STRENGTH, STRENGTH_NID, DOSE_REF, DOSE_REF_NID, NATURE, LK_NATURE) VALUES ("
//                      "%1  ,%2,   '%3',       %4,          '%5',     %6         , '%7',   %8)")
//                .arg(data(DID).toString())
//                .arg(mids.key(compo->data(Component::Name).toString().toUpper()))
//                .arg(compo->data(Component::Strength).toString().replace("'","''"))
//                .arg("NULL")
//                .arg(compo->data(Component::Dose).toString().replace("'","''"))
//                .arg("NULL")
//                .arg(compo->data(Component::Nature).toString())
//                .arg(compo->data(Component::NatureLink).toString())
//                ;
        query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_COMPO));
        query.bindValue(DrugsDB::Constants::COMPO_ID, QVariant());
        query.bindValue(DrugsDB::Constants::COMPO_DID, data(DID).toString());
        query.bindValue(DrugsDB::Constants::COMPO_MID, mids.key(compo->data(Component::Name).toString().toUpper()));
        query.bindValue(DrugsDB::Constants::COMPO_STRENGTH, compo->data(Component::Strength).toString().replace("'","''"));
        query.bindValue(DrugsDB::Constants::COMPO_STRENGTH_NID, QVariant());
        query.bindValue(DrugsDB::Constants::COMPO_DOSE_REF, compo->data(Component::Dose).toString().replace("'","''"));
        query.bindValue(DrugsDB::Constants::COMPO_REF_NID, QVariant());
        query.bindValue(DrugsDB::Constants::COMPO_NATURE, compo->data(Component::Nature).toString());
        query.bindValue(DrugsDB::Constants::COMPO_LK_NATURE, compo->data(Component::NatureLink).toString());
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("Drugs", query);
            db.rollback();
            return false;
        }
        query.finish();
    }

    // Routes
    QList<QVariant> routes = data(Drug::RoutesId).toList();
    if (!routes.isEmpty()) {
        // Create the drugs route links
        foreach(const QVariant &rid, routes) {
//            req = QString("INSERT INTO DRUG_ROUTES (DID, RID) VALUES (%1, %2)")
//                    .arg(data(Drug::DID).toInt())
//                    .arg(rid.toString());
            query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_DRUG_ROUTES));
            query.bindValue(DrugsDB::Constants::DRUG_ROUTES_DID, data(Drug::DID).toInt());
            query.bindValue(DrugsDB::Constants::DRUG_ROUTES_RID, rid.toString());
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR("Drugs", query);
                db.rollback();
            }
            query.finish();
        }
    }


    // Forms  d->m_Content.value(Drug::Forms).value(lang);
    if (!d->m_Content.value(Drug::Forms).isEmpty()) {
        int formsMasterId = Core::Tools::addLabels(Core::Constants::MASTER_DATABASE_NAME, -1, d->m_Content.value(Drug::Forms));
        if (formsMasterId==-1) {
            LOG_ERROR_FOR("Drug", "Forms not saved");
        }
        // Add formsMasterId to DRUGS record
//        req = QString("INSERT INTO DRUG_FORMS (DID,MASTER_LID) VALUES (%1,%2)")
//                .arg(data(DID).toString()).arg(formsMasterId);
        query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_DRUG_FORMS));
        query.bindValue(DrugsDB::Constants::DRUG_FORMS_DID, data(DID).toString());
        query.bindValue(DrugsDB::Constants::DRUG_FORMS_MASTERLID, formsMasterId);
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("Drugs", query);
            db.rollback();
            return false;
        }
        query.finish();
    }

    db.commit();

    return true;
}

bool Drug::lessThanOnNames(const Drug *s1, const Drug *s2)
{
    return s1->data(Name).toString() < s2->data(Name).toString();
}

bool Drug::saveDrugsIntoDatabase(const QString &connection, QVector<Drug *> drugs, const QString &dbUid)
{
    int sid = Core::Tools::getSourceId(connection, dbUid);
    if (sid==-1) {
        LOG_ERROR_FOR("Drug", "NO SID DEFINED");
        return false;
    }

    // Clear database
    QHash<int, QString> w;
    Core::Tools::executeSqlQuery(Core::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_MASTER, w), connection);
    Core::Tools::executeSqlQuery(Core::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_DRUGS, w), connection);
    Core::Tools::executeSqlQuery(Core::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_COMPO, w), connection);
    Core::Tools::executeSqlQuery(Core::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_DRUG_ROUTES, w), connection);
    Core::Tools::executeSqlQuery(Core::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_DRUG_FORMS, w), connection);
    Core::Tools::executeSqlQuery(Core::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_MOLS, w), connection);
    Core::Tools::executeSqlQuery(Core::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_LK_MOL_ATC, w), connection);
    Core::Tools::executeSqlQuery(Core::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_PACKAGING, w), connection);
//    Core::Tools::executeSqlQuery(QString("DELETE FROM MASTER WHERE SID=%1;").arg(sid), connection);
//    Core::Tools::executeSqlQuery(QString("DELETE FROM DRUGS WHERE SID=%1;").arg(sid), connection);
//    //    Core::Tools::executeSqlQuery(QString("DELETE FROM COMPOSITION WHERE SID=%1;").arg(sid), connection);
//    //    Core::Tools::executeSqlQuery(QString("DELETE FROM DRUG_ROUTES WHERE SID=%1;").arg(sid), connection);
//    //    Core::Tools::executeSqlQuery(QString("DELETE FROM DRUG_FORMS WHERE SID=%1;").arg(sid), connection);
//    Core::Tools::executeSqlQuery(QString("DELETE FROM MOLS WHERE SID=%1;").arg(sid), connection);
//    Core::Tools::executeSqlQuery(QString("DELETE FROM LK_MOL_ATC WHERE SID=%1;").arg(sid), connection);
//    Core::Tools::executeSqlQuery(QString("DELETE FROM PACKAGING WHERE SID=%1;").arg(sid), connection);
    // TODO: delete COMPOSITION, DRUG_ROUTES, LABELS_LINK, LABELS */

    // get distinct component names
    QStringList molnames;
    foreach(Drug *drug, drugs) {
        drug->setData(Drug::SID, sid);
        foreach(Component *compo, drug->components()) {
            const QString &name = compo->data(Component::Name).toString();
            if (!molnames.contains(name))
                molnames.append(name);
        }
    }

    QHash<int, QString> mids = Core::Tools::generateMids(molnames, sid, connection);

    // Sort and Save
    qSort(drugs.begin(), drugs.end(), Drug::lessThanOnNames);

    foreach(Drug *drug, drugs) {
        drug->toDatabase(Core::Constants::MASTER_DATABASE_NAME, mids);
    }
    return true;
}

QDebug operator<<(QDebug dbg, const Drug *d)
{
    dbg.nospace() << "Drug(Uids:" << d->data(Drug::Uid1).toString() << ";" << d->data(Drug::Uid2).toString() << ";"<< d->data(Drug::Uid3).toString() << ";" << d->data(Drug::OldUid).toString()
                  << "; DID:" << d->data(Drug::DID).toString()
                  << "; SID:" << d->data(Drug::SID).toString()
                  << "\n   Name:" << d->data(Drug::Name).toString()
                  << "\n   Strength:" << d->data(Drug::Strength).toString()
                  << "\n   Forms:" << d->data(Drug::Forms).toStringList().join(",")
                  << "\n   Routes:" << d->data(Drug::Routes).toStringList().join(",")
                  << "\n   FormsId:" << d->data(Drug::FormsId).toStringList().join(",")
                  << "\n   RoutesId:" << d->data(Drug::RoutesId).toStringList().join(",")
                  << ")";
    return dbg.space();
}

