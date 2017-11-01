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
 ***************************************************************************/
/**
 * \class DrugsDb::Drug
 * \n FreeDDIManager specific class.
 */

/**
 * \class DrugsDb::Component
 * \n FreeDDIManager specific class.
 */

#include "drug.h"

#include <utils/log.h>
#include <translationutils/multilingualclasstemplate.h>

#include <QVector>
#include <QLocale>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <QHash>

using namespace DrugsDb;
using namespace Internal;

namespace DrugsDb {
namespace Internal {
class Book {
public:
    void toTreeWidgetItem(QTreeWidgetItem *) const {}

public:
    QHash<int, QVariant> m_Content;
};

class ComponentPrivate : public Trans::MultiLingualClass<Book>
{
public:
    bool _nothing;
};

class DrugPrivate : public Trans::MultiLingualClass<Book>
{
public:
    QVector<Component *> m_Compo;
};

}  // namespace Internal
}  // namespace DrugsDb

Component::Component() :
    d(new ComponentPrivate)
{
}

Component::~Component()
{
    delete d;
}

/** Set the component data using the language \e lang */
bool Component::setData(const int ref, const QVariant &value, const QString &lang)
{
    Internal::Book *v = 0;
    if (d->hasLanguage(lang.isEmpty() ? Trans::Constants::ALL_LANGUAGE : lang))
        v = d->getLanguage(lang.isEmpty() ? Trans::Constants::ALL_LANGUAGE : lang);
    else
        v = d->createLanguage(lang.isEmpty() ? Trans::Constants::ALL_LANGUAGE : lang);
    v->m_Content.insert(ref, value);
    return true;
}

/** Return the component data using the language \e lang */
QVariant Component::data(const int ref, const QString &lang) const
{
    Internal::Book *v = d->getLanguage(lang.isEmpty() ? Trans::Constants::ALL_LANGUAGE : lang);
    if (!v) {
        v = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!v) {
            v = d->getLanguage("en");
            if (!v)
                return QVariant();
        }
    }
    return v->m_Content.value(ref, QVariant());
}

Drug::Drug() :
    d(new DrugPrivate)
{
}

// TODO: include copy ctor, operator=()
Drug::~Drug()
{
    qDeleteAll(d->m_Compo);
    d->m_Compo.clear();
    delete d;
}

/**
 * \brief Define drugs data.
 * When setting Drug::Forms, labels and link to label are automatically created.
 * Routes and Forms must be defined as QStringList.
*/
bool Drug::setData(const int ref, const QVariant &value, const QString &lang)
{
    Internal::Book *v = 0;
    if (d->hasLanguage(lang.isEmpty() ? Trans::Constants::ALL_LANGUAGE : lang))
        v = d->getLanguage(lang.isEmpty() ? Trans::Constants::ALL_LANGUAGE : lang);
    else
        v = d->createLanguage(lang.isEmpty() ? Trans::Constants::ALL_LANGUAGE : lang);

    switch (ref) {
    case Routes:
    {
        // calculate RIDs
        QList<QVariant> rids;
        QStringList routes = value.toStringList();
        routes.removeAll("");
        routes.removeDuplicates();
        v->m_Content.insert(Drug::Routes, routes);
//        d->m_Content[Drug::Routes].insertMulti(lang, routes);
//        foreach(QString route, routes) {
//            route = route.toLower().simplified();
//            QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
//            QString req;
//            QSqlQuery query(db);
//            // Find the LID of the route
//            //            req = QString("SELECT ROUTES.RID "
//            //                          "FROM LABELS "
//            //                          "JOIN LABELS_LINK ON LABELS_LINK.LID=LABELS.LID "
//            //                          "JOIN ROUTES ON ROUTES.MASTER_LID=LABELS_LINK.MASTER_LID "
//            //                          "WHERE LABELS.LABEL=\"%1\"")
//            //                    .arg(route);
//            Utils::FieldList get;
//            get << Utils::Field(DrugsDB::Constants::Table_ROUTES, DrugsDB::Constants::ROUTES_RID);
//            Utils::JoinList joins;
//            joins << Utils::Join(DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_MASTERLID, DrugsDB::Constants::Table_ROUTES, DrugsDB::Constants::ROUTES_MASTERLID);
//            joins << Utils::Join(DrugsDB::Constants::Table_LABELS, DrugsDB::Constants::LABELS_LID, DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_LID);
//            //            joins << Utils::Join(DrugsDB::Constants::Table_LABELS, DrugsDB::Constants::LABELS_LID, DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_LID);
//            //                  << Utils::Join(DrugsDB::Constants::Table_ROUTES, DrugsDB::Constants::LABELS_LID, DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_LID);
//            Utils::FieldList cond;
//            cond << Utils::Field(DrugsDB::Constants::Table_LABELS, DrugsDB::Constants::LABELS_LABEL, QString("='%1'").arg(route));

//            //            qWarning() << DrugsDB::Tools::drugBase()->select(get,joins,cond);

//            if (query.exec(_database->select(get,joins,cond))) {
//                if (query.next()) {
//                    rids << query.value(0);
//                }
//            } else {
//                LOG_QUERY_ERROR_FOR("Drug", query);
//            }
//            if (rids.count() > 0) {
//                v->m_Content.insert(Drug::RoutesId, rids);
////                d->m_Content[Drug::RoutesId].insertMulti(lang, rids);
//            } else {
//                LOG_ERROR_FOR("Drug", "RouteID not found for " + route);
//            }
//        }
        break;
    }
    case Forms: // TODO: manage drugs forms ?
        break;
    }
    v->m_Content.insert(ref, value);
    return true;
}

/** Get data of the drug using the language \e lang */
QVariant Drug::data(const int ref, const QString &lang) const
{
    Internal::Book *v = d->getLanguage(lang.isEmpty() ? Trans::Constants::ALL_LANGUAGE : lang);
    if (!v) {
        v = d->getLanguage(Trans::Constants::ALL_LANGUAGE);
        if (!v) {
            v = d->getLanguage("en");
            if (!v)
                return QVariant();
        }
    }
    return v->m_Content.value(ref, QVariant());
}

/** Add a drug component */
void Drug::addComponent(Component *compo)
{
    d->m_Compo.append(compo);
}

/** Return all drug components of the drug */
QVector<Component *> Drug::components() const
{
    return d->m_Compo;
}

/** Return all available language used for the drug data */
QStringList Drug::availableLanguages() const
{
    return d->languages();
}

/** Sort help based on the drug name */
bool Drug::lessThanOnNames(const Drug *s1, const Drug *s2)
{
    return s1->data(Name).toString() < s2->data(Name).toString();
}

QDebug operator<<(QDebug dbg, const DrugsDb::Drug *d)
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

