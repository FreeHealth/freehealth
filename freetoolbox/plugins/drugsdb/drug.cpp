#include "drug.h"

#include <coreplugin/globaltools.h>
#include <coreplugin/ftb_constants.h>

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
  \brief Define drugs datas.
  When setting Drug::Routes, the ids are guessed from the official routes.
  When setting Drug::Forms, labels and link to label are automatically created.
  For Routes and Forms, must be set as QStringList.
*/
bool Drug::setData(const int ref, const QVariant &value, const QString &lang)
{
    switch (ref) {
    case Routes:
        {
            d->m_Content[Drug::Routes].insertMulti(lang, value);
            // calculate RIDs
            QList<QVariant> rids;
            foreach(const QString &route, value.toStringList()) {
                QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
                QString req;
                QSqlQuery query(db);
                // Find the LID of the route
                req = QString("SELECT ROUTES.RID "
                              "FROM LABELS "
                              "JOIN LABELS_LINK ON LABELS_LINK.LID=LABELS.LID "
                              "JOIN ROUTES ON ROUTES.MASTER_LID=LABELS_LINK.MASTER_LID "
                              "WHERE LABELS.LABEL=\"%1\"")
                      .arg(route);
                if (query.exec(req)) {
                    if (query.next()) {
                        rids << query.value(0);
                    }
                } else {
                    LOG_QUERY_ERROR_FOR("Drug", query);
                }
                if (rids.count()) {
                    d->m_Content[Drug::RoutesId].insertMulti(lang, rids);
                    continue;
                }
            }
            break;
        }
    case Forms: /** \todo manage drugs forms ? */  break;
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
    req = QString("INSERT INTO `MASTER` (DID,UID1,UID2,UID3,OLD_UID,SID) VALUES ("
                   "NULL,'%1','%2','%3','%4',%5)")
            .arg(data(Uid1, lang).toString())
            .arg(data(Uid2, lang).toString())
            .arg(data(Uid3, lang).toString())
            .arg(data(OldUid, lang).toString())
            .arg(data(SID, lang).toString())
            ;
    if (query.exec(req)) {
        setData(DID, query.lastInsertId());
    } else {
        Utils::Log::addQueryError("Drugs", query, __FILE__, __LINE__);
        db.rollback();
        return false;
    }
    query.finish();

    // Authorizations
    Core::Tools::addLabels(dbConnection, -1, d->m_Content.value(Authorization));

    // Drugs
    req = QString("INSERT INTO `DRUGS` ("
            "DID,SID,NAME,ATC_ID,STRENGTH,AID_MASTER_LID,VALID,MARKETED,LINK_SPC,EXTRA_XML) VALUES ("
            "%1  ,%2,'%3',  %4,   '%5',        %6,         %7,     %8,   '%9'  ,  '%10')")
                  .arg(data(DID, lang).toString())
                  .arg(data(SID, lang).toString())
                  .arg(data(Name, lang).toString().replace("'","''"))
                  .arg(data(AtcId, lang).toString())
                  .arg(data(Strength, lang).toString())
                  .arg(data(AID, lang).toString())
                  .arg(data(Valid, lang).toString())
                  .arg(data(Marketed, lang).toInt())
                  .arg(data(Spc, lang).toString())
                  .arg(data(Xml, lang).toString())
            ;
    while (req.contains("  "))
        req = req.replace("  ", " ");
    req.replace(", ,", ", NULL ,");
    req.replace(",,", ", NULL ,");
    req.replace(", ,", ", NULL ,");
    req.replace(",,", ", NULL ,");

    if (!query.exec(req)) {
        Utils::Log::addQueryError("Drugs", query, __FILE__, __LINE__);
        db.rollback();
        return false;
    }
    query.finish();

    // Composition
    foreach(Component *compo, d->m_Compo) {
        req = QString("INSERT INTO `COMPOSITION` ("
                      "DID, MID, STRENGTH, STRENGTH_NID, DOSE_REF, DOSE_REF_NID, NATURE, LK_NATURE) VALUES ("
                      "%1  ,%2,   '%3',       %4,          '%5',     %6         , '%7',   %8)")
                .arg(data(DID).toString())
                .arg(mids.key(compo->data(Component::Name).toString().toUpper()))
                .arg(compo->data(Component::Strength).toString().replace("'","''"))
                .arg("NULL")
                .arg(compo->data(Component::Dose).toString().replace("'","''"))
                .arg("NULL")
                .arg(compo->data(Component::Nature).toString())
                .arg(compo->data(Component::NatureLink).toString())
                ;
        if (!query.exec(req)) {
            Utils::Log::addQueryError("Drugs", query, __FILE__, __LINE__);
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
            req = QString("INSERT INTO DRUG_ROUTES (DID, RID) VALUES (%1, %2)")
                  .arg(data(Drug::DID).toInt())
                  .arg(rid.toString());
            if (!query.exec(req)) {
                Utils::Log::addQueryError("Drugs", query, __FILE__, __LINE__);
                db.rollback();
            }
            query.finish();
        }
    }


    // Forms  d->m_Content.value(Drug::Forms).value(lang);
    if (!d->m_Content.value(Drug::Forms).isEmpty()) {
        int formsMasterId = Core::Tools::addLabels(Core::Constants::MASTER_DATABASE_NAME, -1, d->m_Content.value(Drug::Forms));
        if (formsMasterId==-1) {
            Utils::Log::addError("Drug", "Forms not saved", __FILE__, __LINE__);
        }
        // Add formsMasterId to DRUGS record
        req = QString("INSERT INTO DRUG_FORMS (DID,MASTER_LID) VALUES (%1,%2)")
              .arg(data(DID).toString()).arg(formsMasterId);
        if (!query.exec(req)) {
            Utils::Log::addQueryError("Drugs", query, __FILE__, __LINE__);
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
        Utils::Log::addError("Drug", "NO SID DEFINED", __FILE__, __LINE__);
        return false;
    }

    // Clear database
    Core::Tools::executeSqlQuery(QString("DELETE FROM MASTER WHERE SID=%1;").arg(sid), connection);
    Core::Tools::executeSqlQuery(QString("DELETE FROM DRUGS WHERE SID=%1;").arg(sid), connection);
//    Core::Tools::executeSqlQuery(QString("DELETE FROM COMPOSITION WHERE SID=%1;").arg(sid), connection);
//    Core::Tools::executeSqlQuery(QString("DELETE FROM DRUG_ROUTES WHERE SID=%1;").arg(sid), connection);
//    Core::Tools::executeSqlQuery(QString("DELETE FROM DRUG_FORMS WHERE SID=%1;").arg(sid), connection);
    Core::Tools::executeSqlQuery(QString("DELETE FROM MOLS WHERE SID=%1;").arg(sid), connection);
    Core::Tools::executeSqlQuery(QString("DELETE FROM LK_MOL_ATC WHERE SID=%1;").arg(sid), connection);
    Core::Tools::executeSqlQuery(QString("DELETE FROM PACKAGING WHERE SID=%1;").arg(sid), connection);
    /** \todo delete COMPOSITION, DRUG_ROUTES, LABELS_LINK, LABELS */

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
