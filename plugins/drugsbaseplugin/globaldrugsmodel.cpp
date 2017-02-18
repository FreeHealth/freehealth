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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class DrugsDB::GlobalDrugsModel
  This class provides the drugs model from the entire currently in use drugs database.
*/

#include "globaldrugsmodel.h"

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/protocolsbase.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/idrug.h>
#include <drugsbaseplugin/idrugengine.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatient.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QList>
#include <QColor>
#include <QDir>
#include <QCache>
#include <QSqlError>
#include <QPersistentModelIndex>
#include <QTimer>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QtConcurrent/QtConcurrent>

#include <QFuture>
#include <QFutureWatcher>

#include <QDebug>

using namespace Trans::ConstantTranslations;
using namespace DrugsDB;
using namespace Internal;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::Translators *translators() {return Core::ICore::instance()->translators();}
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static inline DrugsDB::ProtocolsBase &protocolsBase() {return DrugsDB::DrugBaseCore::instance().protocolsBase();}

namespace DrugsDB {
namespace Internal {

class GlobalDrugsModelPrivate
{
public:
    enum ModelPrivateRepresentation {
        Priv_DID = 0,
        Priv_BrandName,
        Priv_Strength,
        Priv_ATC_ID,
        Priv_Market
    };

    GlobalDrugsModelPrivate(GlobalDrugsModel *parent) :
        m_SearchMode(0),
        m_AllergyEngine(0),
        q(parent)
    {
        Q_ASSERT(q);
        ++numberOfInstances;
    }

    ~GlobalDrugsModelPrivate()
    {
        --numberOfInstances;
        if (numberOfInstances == 0) {
            m_CachedAvailableDosageForUID.clear();
            delete m_DrugsPrecautionsModel;
            m_DrugsPrecautionsModel = 0;
//            delete m_Timer;
        }
    }

    void getSourceFilter()
    {
        m_SourceFilter = QString(" `%1`.`%2`=%3 ")
                         .arg(drugsBase().table(Constants::Table_DRUGS))
                         .arg(drugsBase().fieldName(Constants::Table_DRUGS, Constants::DRUGS_SID))
                         .arg(drugsBase().actualDatabaseInformation()->sid);
    }

    void setQueryModelSearchMode(const int searchMode)
    {
        getSourceFilter();
        m_SearchMode = searchMode;
        switch (m_SearchMode)
        {
        case GlobalDrugsModel::SearchByBrandName:
            {
                m_SqlQueryWithoutWhere = drugsBase().select(Constants::Table_DRUGS, QList<int>()
                                     << Constants::DRUGS_DID
                                     << Constants::DRUGS_NAME
                                     << Constants::DRUGS_STRENGTH
                                     << Constants::DRUGS_ATC_ID
                                     << Constants::DRUGS_MARKET);
                break;
            }
        case GlobalDrugsModel::SearchByMolecularName:
            {
                Utils::FieldList get;
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_DID);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_NAME);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_ATC_ID);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_MARKET);
                Utils::JoinList joins;
                joins << Utils::Join(Constants::Table_COMPO, Constants::COMPO_DID, Constants::Table_DRUGS, Constants::DRUGS_DID);
                joins << Utils::Join(Constants::Table_MOLS, Constants::MOLS_MID, Constants::Table_COMPO, Constants::COMPO_MID);
                m_SqlQueryWithoutWhere = drugsBase().select(get, joins);
                break;
            }
        case GlobalDrugsModel::SearchByInnName:
            {
                Utils::FieldList get;
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_DID);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_NAME);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_ATC_ID);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_MARKET);
                Utils::JoinList joins;
                joins << Utils::Join(Constants::Table_COMPO, Constants::COMPO_DID, Constants::Table_DRUGS, Constants::DRUGS_DID);
                joins << Utils::Join(Constants::Table_LK_MOL_ATC, Constants::LK_MID, Constants::Table_COMPO, Constants::COMPO_MID);
                joins << Utils::Join(Constants::Table_ATC_LABELS, Constants::ATC_LABELS_ATCID, Constants::Table_LK_MOL_ATC, Constants::LK_ATC_ID);
                joins << Utils::Join(Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID, Constants::Table_ATC_LABELS, Constants::ATC_LABELS_MASTERLID);
                joins << Utils::Join(Constants::Table_LABELS, Constants::LABELS_LID, Constants::Table_LABELSLINK, Constants::LABELSLINK_LID);
                m_SqlQueryWithoutWhere = drugsBase().select(get, joins);
                break;
            }
        }

//        qWarning() << m_SqlQueryWithoutWhere + "WHERE " + m_SourceFilter + " AND " + m_Filter;

        if (!m_Filter.isEmpty()) {
            q->setQuery(m_SqlQueryWithoutWhere + "WHERE " + m_SourceFilter + " AND " + m_Filter, QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        } else {
            q->setQuery(m_SqlQueryWithoutWhere + "WHERE " + m_SourceFilter, QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        }
    }

    void setQueryModelFilter(const QString &searchFor)
    {
        switch (m_SearchMode)
        {
        case GlobalDrugsModel::SearchByBrandName:
            {
                m_Filter = QString("`%1`.`%2` LIKE '%3%'")
                           .arg(drugsBase().table(Constants::Table_DRUGS))
                           .arg(drugsBase().fieldName(Constants::Table_DRUGS, Constants::DRUGS_NAME))
                           .arg(searchFor);
                break;
            }
        case GlobalDrugsModel::SearchByMolecularName:
            {
                m_Filter = QString("`%1`.`%2` LIKE '%3%'")
                           .arg(drugsBase().table(Constants::Table_MOLS))
                           .arg(drugsBase().fieldName(Constants::Table_MOLS, Constants::MOLS_NAME))
                           .arg(searchFor);
                break;
            }
        case GlobalDrugsModel::SearchByInnName:
            {
                m_Filter = QString("`%1`.`%2` LIKE '%3%'")
                           .arg(drugsBase().table(Constants::Table_LABELS))
                           .arg(drugsBase().fieldName(Constants::Table_LABELS, Constants::LABELS_LABEL))
                           .arg(searchFor);
                break;
            }
        }

//        qWarning() << m_SqlQueryWithoutWhere + "WHERE " + m_SourceFilter + " AND " + m_Filter;

        if (!m_Filter.isEmpty()) {
            q->setQuery(m_SqlQueryWithoutWhere + "WHERE " + m_SourceFilter + " AND " + m_Filter, QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        } else {
            q->setQuery(m_SqlQueryWithoutWhere + "WHERE " + m_SourceFilter, QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        }
    }

    static void updateCachedAvailableDosage()
    {
        m_CachedAvailableDosageForUID.clear();
        foreach(const QVariant &uid, protocolsBase().getAllUIDThatHaveRecordedDosages())
            m_CachedAvailableDosageForUID.append(uid.toString());
    }

    static bool UIDHasRecordedDosage(const QString &uid)
    {
        return m_CachedAvailableDosageForUID.contains(uid);
    }

    QString getConstructedDrugName(const int row)
    {
        const QString &drugName = q->QSqlQueryModel::data(q->index(row, Priv_BrandName)).toString();
        if (drugsBase().actualDatabaseInformation()) {
            QString tmp = drugsBase().actualDatabaseInformation()->drugsNameConstructor;
            if (!tmp.isEmpty()) {
                tmp.replace("NAME", drugName);
                tmp.replace("FORM", q->data(q->index(row, GlobalDrugsModel::Forms)).toString());
                tmp.replace("ROUTE", q->data(q->index(row, GlobalDrugsModel::Routes)).toString());
                QString s = q->QSqlQueryModel::data(q->index(row, Priv_Strength)).toString();
                if (s.count(";") > 3)
                    s.clear();
                tmp.replace("STRENGTH", s);
                return tmp;
            }
        }
        return drugName;
    }


    // For QFuture use
public:
    int m_SearchMode;
    QString m_SqlQueryWithoutWhere, m_Filter, m_SourceFilter;
    QVector< QFutureWatcher<QPersistentModelIndex> * > m_Watchers;

    static QStandardItemModel *m_DrugsPrecautionsModel;
    QTimer *m_Timer;
    static int numberOfInstances;

    IDrugAllergyEngine *m_AllergyEngine;

private:
    static QList<QString> m_CachedAvailableDosageForUID;
    GlobalDrugsModel *q;
};

QList<QString> GlobalDrugsModelPrivate::m_CachedAvailableDosageForUID;
int GlobalDrugsModelPrivate::numberOfInstances;
QStandardItemModel *GlobalDrugsModelPrivate::m_DrugsPrecautionsModel = 0;

}  // End Internal
}  // End DrugsDB

using namespace DrugsDB;

void GlobalDrugsModel::updateAvailableDosages()  // static
{
    Internal::GlobalDrugsModelPrivate::updateCachedAvailableDosage();
}

// TODO: Add drug source management
GlobalDrugsModel::GlobalDrugsModel(const SearchMode searchMode, QObject *parent) :
        QSqlQueryModel(parent),
        d(0)
{
    d = new Internal::GlobalDrugsModelPrivate(this);
    d->m_AllergyEngine = pluginManager()->getObject<DrugsDB::IDrugAllergyEngine>();
    static int handle = 0;
    ++handle;
    setObjectName("GlobalDrugsModel_" + QString::number(handle) + "/" + QString::number(d->numberOfInstances));

    d->updateCachedAvailableDosage();
    d->setQueryModelSearchMode(searchMode);

    connect(&protocolsBase(), SIGNAL(protocolsBaseHasChanged()), this, SLOT(updateCachedAvailableDosage()));
    connect(&drugsBase(), SIGNAL(drugsBaseHasChanged()), this, SLOT(onDrugsDatabaseChanged()));
    connect(translators(), SIGNAL(languageChanged()), this, SLOT(onDrugsDatabaseChanged()));
    if (d->m_AllergyEngine) {
        connect(d->m_AllergyEngine, SIGNAL(allergiesUpdated()), this, SLOT(updateAllergies()));
        connect(d->m_AllergyEngine, SIGNAL(intolerancesUpdated()), this, SLOT(updateAllergies()));
    }
}

GlobalDrugsModel::~GlobalDrugsModel()
{
    qWarning() << "GlobalDrugsModel::~GlobalDrugsModel() // number of instances" << d->numberOfInstances;
    for(int i = 0; i < d->m_Watchers.count() ; ++i) {
        d->m_Watchers.at(i)->cancel();
    }
    qDeleteAll(d->m_Watchers);
    d->m_Watchers.clear();
    if (d) {
        delete d;
        d=0;
    }
}

void GlobalDrugsModel::setSearchMode(const int searchMode)
{
    d->setQueryModelSearchMode(searchMode);
}

void GlobalDrugsModel::onDrugsDatabaseChanged()
{
    d->setQueryModelSearchMode(d->m_SearchMode);
}

QVariant GlobalDrugsModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();
    QModelIndex idx = index(item.row(), GlobalDrugsModelPrivate::Priv_DID);
    const QString &uid = QSqlQueryModel::data(idx).toString();

//    qWarning() << uid << role << QSqlQueryModel::data(index(item.row(), GlobalDrugsModelPrivate::Priv_DID));
    if (role == Qt::DisplayRole) {
        switch (item.column()) {
        case DrugId: return QSqlQueryModel::data(index(item.row(), GlobalDrugsModelPrivate::Priv_DID));
        case BrandName: return d->getConstructedDrugName(item.row());
        case Strength:
            {
                const QString &s = QSqlQueryModel::data(QSqlQueryModel::index(item.row(), GlobalDrugsModelPrivate::Priv_Strength)).toString();
                if (s.count(";") > 3)
                    return QVariant();
                return s;
            }
        case Routes: return drugsBase().getRouteLabels(uid.toInt()).join(", ");
        case Forms: return drugsBase().getFormLabels(uid.toInt()).join(", ");
        }
        return QVariant();
    }

    // Cache processed UIDs for allergies and intolerances
    // If cached  --> start the process in a QFuture ; connect QFuture to Model::dataChanged()
    // If !cached --> ok to return the values

//    if (!d->m_ProcessedUid.contains(uid)) {
//        d->m_ProcessedUid.append(uid);
//        QFuture<QPersistentModelIndex> processPhysiology = QtConcurrent::run(DrugsDB::Internal::GlobalDrugsModelPrivate::checkPhysiology, item, this);
//        processPhysiology.waitForFinished();
//    }

    if (role == Qt::BackgroundRole) {
        // test atc's patient allergies
        // TODO: add drug's atc
//        QSqlQueryModel::data(QSqlQueryModel::index(item.row(), GlobalDrugsModelPrivate::Priv_ATC_ID)).toString();
        if (d->m_AllergyEngine) {
            d->m_AllergyEngine->check(IDrugAllergyEngine::Allergy, uid);
            if (d->m_AllergyEngine->has(IDrugAllergyEngine::Allergy, uid)) {
                QColor c = QColor(settings()->value(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR).toString());
                c.setAlpha(190);
                return c;
            } else if (d->m_AllergyEngine->has(IDrugAllergyEngine::Intolerance, uid)) {
                QColor c = QColor(settings()->value(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR).toString());
                c.setAlpha(190);
                return c;
            }
        }
        if (settings()->value(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES).toBool()) {
            if (d->UIDHasRecordedDosage(uid)) {
                QColor c = QColor(settings()->value(Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR).toString());
                c.setAlpha(125);
                return c;
            }
        }
    } else if (role == Qt::ToolTipRole) {
        QString tmp = "<html><body>";
        // Allergy Intolerance ?
        if (d->m_AllergyEngine) {
            d->m_AllergyEngine->check(IDrugAllergyEngine::Allergy, uid);
            if (d->m_AllergyEngine->has(IDrugAllergyEngine::Allergy, uid)) {
                tmp += QString("<table width=100%><tr><td><img src=\"%1\"></td><td width=100% align=center><span style=\"color:red;font-weight:600\">%2</span></td><td><img src=\"%1\"></span></td></tr></table><br>")
                        .arg(settings()->path(Core::ISettings::SmallPixmapPath) + QDir::separator() + QString(Core::Constants::ICONFORBIDDEN))
                        .arg(tr("KNOWN ALLERGY"));
            } else if (d->m_AllergyEngine->has(IDrugAllergyEngine::Intolerance, uid)) {
                tmp += QString("<table width=100%><tr><td><img src=\"%1\"></td><td width=100% align=center><span style=\"color:red;font-weight:600\">%2</span></td><td><img src=\"%1\"></span></td></tr></table><br>")
                        .arg(settings()->path(Core::ISettings::SmallPixmapPath) + QDir::separator() + QString(Core::Constants::ICONWARNING))
                        .arg(tr("KNOWN INTOLERANCE"));
            }
        }

        // Name, ATC and UID
        QString atc = drugsBase().getAtcCode(QSqlQueryModel::data(QSqlQueryModel::index(item.row(), GlobalDrugsModelPrivate::Priv_ATC_ID)).toInt());
        if (!atc.isEmpty())
            atc.prepend(" ; ATC: ");
        QString uidName = "UID";
        if (drugsBase().actualDatabaseInformation()) {
            if (!drugsBase().actualDatabaseInformation()->drugsUidName.isEmpty())
                uidName = drugsBase().actualDatabaseInformation()->drugsUidName;
        }
        // Marketed infos
        QString mark;
        // TODO: code here
        //        mark = QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_AUTHORIZATION)).toString();
        //        if (!mark.isEmpty()) {
        //            mark.prepend("<br>");
        //            mark += " | " + QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_TYPE_MP)).toString();
        //        }

        // Create tooltip
        tmp += QString("<table border=0 cellpadding=2 cellspacing=2 width=100%>"
                        " <tr>"
                        "   <td colspan=2 rowspan=1 align=center>"
                        "       <span style=\"font-weight: bold;\">%1</span>"
                        "       %2"
                        "   </td>"
                        " </tr>"
                        " <tr>"
                        "   <td colspan=2 rowspan=1>"
                        "       %3"
                        "       <br>%4"
                        "       <br>%5%6%7"
                        "   </td>"
                        " </tr>")
                .arg(d->getConstructedDrugName(item.row()))
                .arg(mark)
                .arg(tr("Form(s): ") + drugsBase().getFormLabels(uid).join(", "))
                .arg(tr("Route(s): ") + drugsBase().getRouteLabels(uid).join(", "))
                .arg(tr("Identifier(s): ") + uidName + ":")
                .arg(uid)
                .arg(atc)
                ;

        // get composition
        if (settings()->value(Constants::S_SELECTOR_SHOWMOLECULES).toBool()) {
            QHash<QString, QString> compo = drugsBase().getDrugFullComposition(uid);
            if (!compo.isEmpty()) {
                QHashIterator<QString, QString> i(compo);
                while (i.hasNext()) {
                    i.next();
                    tmp += QString("<tr><td>%1</td><td>%2</td></tr>")
                           .arg(i.key())
                           .arg(i.value());
                }
            } else {
                QStringList mols = drugsBase().getDrugMolecularComposition(uid);
                foreach(const QString &s, mols) {
                    tmp += QString("<tr><td>%1</td><td>&nbsp;</td></tr>")
                           .arg(s);
                }
            }
            // END
//            tmp += tr("ATC codes (for interaction engine):<br>");
//            if (!atc.isEmpty())
//                tmp += "&nbsp;&nbsp;&nbsp;" + drugsBase().getDrugCompositionAtcCodes(uid).join(";") + ";" + atc + "<br>";
//            else
//                tmp += "&nbsp;&nbsp;&nbsp;" + drugsBase().getDrugCompositionAtcCodes(uid).join(";") + "<br>";
        }
        tmp += "</table></body></html>";

        return tmp;
    } else if (role == Qt::DecorationRole && item.column()==BrandName) {
        if (d->m_AllergyEngine) {
            d->m_AllergyEngine->check(IDrugAllergyEngine::Allergy, uid);
            if (d->m_AllergyEngine->has(IDrugAllergyEngine::Allergy, uid)) {
                return theme()->icon(Core::Constants::ICONFORBIDDEN);
            } else {
                d->m_AllergyEngine->check(IDrugAllergyEngine::Intolerance, uid);
                if (d->m_AllergyEngine->has(IDrugAllergyEngine::Intolerance, uid)) {
                    return theme()->icon(Core::Constants::ICONWARNING);
                }
            }
        }
        return QVariant();
    }
    return QVariant();
}

Qt::ItemFlags GlobalDrugsModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void GlobalDrugsModel::setFilter(const QString &searchFor)
{
    d->setQueryModelFilter(searchFor);
}

void GlobalDrugsModel::updateCachedAvailableDosage()
{
    d->updateCachedAvailableDosage();
}

void GlobalDrugsModel::updateAllergies()
{
    beginResetModel();
    endResetModel();
}
