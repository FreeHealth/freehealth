#include "drugdruginteractionengine.h"
#include "drugsbase.h"
#include "idrug.h"
#include "drugsdata.h"
#include "idruginteraction.h"
#include "constants_databaseschema.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <utils/log.h>
#include <utils/global.h>

#include <translationutils/constanttranslations.h>

#include <QVector>
#include <QCoreApplication>

static inline DrugsDB::Internal::DrugsBase *base() {return DrugsDB::Internal::DrugsBase::instance();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace  {

    const char* const  LIST_BASIC_INFO =
            "<tr>"
            "  <td><b>%1</b></td>\n"
            "  <td>%2 (%3)<br>%4 (%5)</td>\n"
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

class DrugsInteraction : public IDrugInteraction
{
public:
    enum DataRepresentation {
        DI_Id,
        DI_TypeId,
        DI_TypeName,
        DI_ATC1,
        DI_ATC2,
        DI_ATC1_Label,
        DI_ATC2_Label,
        DI_Risk,
        DI_Management,
        DI_RiskId,
        DI_ManagementId,

        // only used for test
        DI_RiskFr,
        DI_RiskEn,
        DI_ManagementFr,
        DI_ManagementEn,
        DI_ReferencesLink
    };


    DrugsInteraction(IDrugEngine *parentEngine) : IDrugInteraction(parentEngine), m_Engine(parentEngine) {}
    ~DrugsInteraction() {}

    void setValue(const int ref, const QVariant &value)
    {
        if (ref == DI_TypeId) {
            QString t = value.toString();
            DrugDrugInteractionEngine::TypesOfIAM r = DrugDrugInteractionEngine::NoIAM;
            if (t.contains("U"))
                r |= DrugDrugInteractionEngine::InnDuplication;
            if (t.contains("P"))
                r |= DrugDrugInteractionEngine::Precaution;
            if (t.contains("C"))
                r |= DrugDrugInteractionEngine::ContreIndication;
            if (t.contains("D"))
                r |= DrugDrugInteractionEngine::Deconseille;
            if (t.contains("T"))
                r |= DrugDrugInteractionEngine::APrendreEnCompte;
            if (t.contains("450"))
                r |= DrugDrugInteractionEngine::P450;
            if (t.contains("I"))
                r |= DrugDrugInteractionEngine::Information;
            if (t.contains("Y"))
                r |= DrugDrugInteractionEngine::GPG;
            m_Infos.insert(ref, int(r));
        } else {
            m_Infos.insert(ref, value);
        }
    }

    QVariant value(const int ref) {return m_Infos.value(ref);}

    // IDrugInteraction interface
    IDrugEngine *engine() const {return m_Engine;}

    bool isDrugDrugInteraction() const {return true;}
    bool isPotentiallyInappropriate() const {return false;}

    QString type() const {return m_Infos.value(DI_TypeName).toString();}

    QList<IDrug *> drugs() const {return m_InteractingDrugs;}

    QString header() const {return base()->getAtcLabel(m_Infos.value(DI_ATC1).toInt()) + " - " + base()->getAtcLabel(m_Infos.value(DI_ATC2).toInt());}
    QString risk(const QString &lang = QString::null) const
    {
        QString l = lang;
        if (l.isEmpty())
            l = QLocale().name().left(2);
        QString r;
        if (l=="fr")
            r = m_Infos.value(DI_RiskFr).toString();
        else
            r = m_Infos.value(DI_RiskEn).toString();
        return r.replace("<br />", "<br>");
    }

    QString management(const QString &lang = QString::null) const
    {
        QString l = lang;
        if (l.isEmpty())
            l = QLocale().name().left(2);
        QString r;
        if (l=="fr")
            r = m_Infos.value(DI_ManagementFr).toString();
        else
            r = m_Infos.value(DI_ManagementEn).toString();
        return r.replace("<br />", "<br>");
    }

    QString referencesLink(const QString &lang = QString::null) const {return m_Infos.value(DI_ReferencesLink).toString();}

    QString toHtml(bool detailled = false) const
    {
        if (m_InteractingDrugs.count() != 2)
            return QString();
        // Link Atc_Ids with interacting drug
        int firstDrugAtcId = 0;
        int secondDrugAtcId = 0;
        if (m_InteractingDrugs.at(0)->atcIdsContains(m_Infos.value(DI_ATC1).toInt())) {
            firstDrugAtcId = m_Infos.value(DI_ATC1).toInt();
            secondDrugAtcId = m_Infos.value(DI_ATC2).toInt();
        } else {
            secondDrugAtcId= m_Infos.value(DI_ATC1).toInt();
            firstDrugAtcId = m_Infos.value(DI_ATC2).toInt();
        }
        // Create the Html header
        QString tmp = QString(LIST_BASIC_INFO)
               .arg(type())
               .arg(drugs().at(0)->brandName())
               .arg(base()->getAtcLabel(firstDrugAtcId))
               .arg(drugs().at(1)->brandName())
               .arg(base()->getAtcLabel(secondDrugAtcId));
        if (detailled) {
            tmp += QString(LIST_FULL_INFO)
                   .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::NATURE_OF_RISK))
                   .arg(risk()
                        .replace("<br>", " ")
                        .replace("<", "&lt;")
                        .replace(">", "&gt;"))
                   .arg(QCoreApplication::translate(Constants::DRUGSBASE_TR_CONTEXT, Constants::MANAGEMENT))
                   .arg(management()
                        .replace("<br>", "__")
                        .replace("<", "&lt;")
                        .replace(">", "&gt;")
                        .replace("__", "<br>"));
        }
        return tmp;
    }

    int sortIndex() const {return m_Infos.value(DI_TypeId).toInt();}

    // Private interface
    int typeId() const {return m_Infos.value(DI_TypeId).toInt();}
    static QString typeToString(const int t)
    {
         QStringList tmp;
         DrugDrugInteractionEngine::TypesOfIAM r = DrugDrugInteractionEngine::TypesOfIAM(t);
         if (r & DrugDrugInteractionEngine::APrendreEnCompte)
              tmp << tkTr(Trans::Constants::TAKE_INTO_ACCOUNT);
         if (r & DrugDrugInteractionEngine::Deconseille)
              tmp << tkTr(Trans::Constants::DISCOURAGED);
         if (r & DrugDrugInteractionEngine::ContreIndication)
              tmp << tkTr(Trans::Constants::CONTRAINDICATION);
         if (r & DrugDrugInteractionEngine::Precaution)
              tmp << tkTr(Trans::Constants::PRECAUTION_FOR_USE);
         if (r & DrugDrugInteractionEngine::P450)
              tmp << tkTr(Trans::Constants::P450_IAM);
         if (r & DrugDrugInteractionEngine::GPG)
              tmp << tkTr(Trans::Constants::GPG_IAM);
         if (r & DrugDrugInteractionEngine::InnDuplication)
              tmp << tkTr(Trans::Constants::INN_DUPLICATION);
         if (r & DrugDrugInteractionEngine::Information)
              tmp << tkTr(Trans::Constants::INFORMATION);
         return tmp.join(", ");
    }

    void addInteractingDrug(IDrug *drug)
    {
        Q_ASSERT(drug);
        if (!m_InteractingDrugs.contains(drug))
            m_InteractingDrugs << drug;
    }

private:
    IDrugEngine *m_Engine;
    QHash<int, QVariant> m_Infos;
    QList<IDrug *> m_InteractingDrugs;
};

}


namespace DrugsDB {
namespace Internal {
class DrugDrugInteractionEnginePrivate
{
public:
    QVector<IDrug *> m_TestedDrugs;
    QVector<IDrugInteraction *> m_Interactions;
    QMap<int, int> m_InteractionsIDs;        /*!<  All possible interactions based on ATC IDs*/
    QMultiMap<int, int> m_DDIFound;               /*!< modified by checkDrugInteraction() */
    bool m_LogChrono;
};
}
}

DrugDrugInteractionEngine::DrugDrugInteractionEngine(QObject *parent) :
        IDrugEngine(parent), d(new DrugDrugInteractionEnginePrivate)
{
    if (Utils::isDebugCompilation()) {
        d->m_LogChrono = true;
    } else {
        d->m_LogChrono = false;
    }
}

bool DrugDrugInteractionEngine::init()
{
    // get all interactions ids and mol <-> atc links
    QList<int> fields;
    fields << Constants::INTERACTIONS_ATC_ID1 << Constants::INTERACTIONS_ATC_ID2;
    QString req = base()->select(Constants::Table_INTERACTIONS, fields);
    QSqlQuery q(req , base()->database());
    if (q.isActive())
        while (q.next())
            d->m_InteractionsIDs.insertMulti(q.value(0).toInt(), q.value(1).toInt());
    return true;
}

QString DrugDrugInteractionEngine::name() const {return tr("Drug-drug interactions engine");}

QString DrugDrugInteractionEngine::uid() const {return Constants::DDI_ENGINE_UID;}

bool DrugDrugInteractionEngine::checkDrugInteraction(IDrug *drug, const QVector<IDrug *> &drugsList)
{
    QTime t;
    t.start();

    if (drug->numberOfInn() == 0)
        return false;

    const QVector<int> &drug_iams = drug->allInnAndInteractingClassesIds();
    QVector<int> d_iams;
    for(int i = 0; i < drugsList.count(); ++i) {
        IDrug *drug2 = drugsList.at(i);
        if (drug2 == drug)
            continue;
        for(int i = 0; i < drug_iams.count(); ++i) {
            int id = drug_iams.at(i);
            if (d_iams.contains(id))
                continue;
            d_iams << id;
        }
    }

    for(int i=0; i < d_iams.count(); ++i) {
        int s = d_iams.at(i);
        for(int j = 0; j < drug_iams.count(); ++j)  {
            int s2 = drug_iams.at(j);
            // foreach iam subst/class test existing interactions with *drug
            if (d->m_InteractionsIDs.keys(s).contains(s2))
                if ((!d->m_DDIFound.contains(s2, s)) && (!d->m_DDIFound.contains(s, s2)))
                    d->m_DDIFound.insertMulti(s2, s);

            //             Not necessary because interactions are "mirrored" in the database
            //             if (m_InteractionsIDs.keys(s2).contains(s))
            //                 if ((!m_DDIFound.contains(s2, s)) && (!m_DDIFound.contains(s, s2)))
            //                     m_DDIFound.insertMulti(s, s2);

            // test same molecules
            if ((s > 999) && (s2 > 999) && (s == s2))
                if (!d->m_DDIFound.contains(s, -1))
                    d->m_DDIFound.insertMulti(s, -1);
        }
    }

    if (d->m_LogChrono)
        Utils::Log::logTimeElapsed(t, "DrugDrugInteractionEngine", QString("checkDrugInteraction : %1 ; %2")
                                   .arg(drug->brandName()).arg(drugsList.count()));

    //      qWarning() << "checkDrugInteraction" << m_DDIFound;
    if (d->m_DDIFound.count() != 0)
        return true;
    return false;
}

int DrugDrugInteractionEngine::calculateInteractions(const QVector<IDrug *> &drugs)
{
    QTime t;
    t.start();
    d->m_DDIFound.clear();
    d->m_TestedDrugs.clear();
    d->m_Interactions.clear();
    foreach(IDrug *drug, drugs)
        checkDrugInteraction(drug, drugs);
    d->m_TestedDrugs = drugs;
    if (d->m_LogChrono)
        Utils::Log::logTimeElapsed(t, "DrugDrugInteractionEngine", QString("interactions() : %1 drugs").arg(drugs.count()));
    return d->m_DDIFound.count();
}

QVector<IDrugInteraction *> DrugDrugInteractionEngine::getInteractionsFromDatabase(const int & _id1, const int & _id2)
{
    int id2 = _id2;
    QSqlDatabase DB = base()->database();
    QVector<IDrugInteraction *> toReturn;
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return toReturn;
        }
    }

    // first test : is a duplication interaction ?
    if (id2 == -1) {
        /** \todo update management of DDI duplication alerts. */
        DrugsInteraction *ddi = 0;
        ddi = new ::DrugsInteraction(this);
        ddi->setValue(DrugsInteraction::DI_TypeId , "U");
        ddi->setValue(DrugsInteraction::DI_ATC1, _id1);
        ddi->setValue(DrugsInteraction::DI_ATC2, _id1);
        ddi->setValue(DrugsInteraction::DI_RiskFr, tkTr(Trans::Constants::INN_DUPLICATION));
        ddi->setValue(DrugsInteraction::DI_RiskEn, Trans::Constants::INN_DUPLICATION);
        ddi->setValue(DrugsInteraction::DI_ReferencesLink, QCoreApplication::translate("DrugsBase", "FreeDiams Interactions Engine"));
        id2 = _id1;
        toReturn << ddi;
        return toReturn;
    }

    // else retreive INTERACTION from database
    // construct where clause
    QHashWhere where;
    where.insert(Constants::INTERACTIONS_ATC_ID1, QString("=%1").arg(_id1));
    where.insert(Constants::INTERACTIONS_ATC_ID2, QString("=%1").arg(_id2));

    Utils::FieldList get;
    Utils::JoinList joins;
    Utils::FieldList conditions;
    get
            << Utils::Field(Constants::Table_IAKNOWLEDGE, Constants::IAKNOWLEDGE_RISK_MASTERLID)
            << Utils::Field(Constants::Table_IAKNOWLEDGE, Constants::IAKNOWLEDGE_MANAGEMENT_MASTERLID)
            << Utils::Field(Constants::Table_IAKNOWLEDGE, Constants::IAKNOWLEDGE_TYPE)
            << Utils::Field(Constants::Table_IAKNOWLEDGE, Constants::IAKNOWLEDGE_WWW)
            << Utils::Field(Constants::Table_INTERACTIONS, Constants::INTERACTIONS_IAID);
    joins
            << Utils::Join(Constants::Table_IA_IAK, Constants::IA_IAK_IAID, Constants::Table_INTERACTIONS, Constants::INTERACTIONS_IAID)
            << Utils::Join(Constants::Table_IA_IAK, Constants::IA_IAK_IAKID, Constants::Table_IAKNOWLEDGE, Constants::IAKNOWLEDGE_IAKID);
    conditions
            << Utils::Field(Constants::Table_INTERACTIONS, Constants::INTERACTIONS_ATC_ID1, QString("=%1").arg(_id1))
            << Utils::Field(Constants::Table_INTERACTIONS, Constants::INTERACTIONS_ATC_ID2, QString("=%1").arg(_id2));
    // get interaction links
    QString req = base()->select(get, joins, conditions);
    QSqlQuery query(req, DB);
    if (query.isActive()) {
        while (query.next()) {
            DrugsInteraction *ddi = 0;
            ddi = new DrugsInteraction(this);
            ddi->setValue(DrugsInteraction::DI_ATC1, _id1);
            ddi->setValue(DrugsInteraction::DI_ATC2, _id2);
            ddi->setValue(DrugsInteraction::DI_RiskId, query.value(0));
            ddi->setValue(DrugsInteraction::DI_ManagementId, query.value(1));
            ddi->setValue(DrugsInteraction::DI_TypeId, query.value(2));
            ddi->setValue(DrugsInteraction::DI_ReferencesLink, query.value(3));
            ddi->setValue(DrugsInteraction::DI_Id, query.value(4));
            toReturn << ddi;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();
    /** \todo get interaction bibliography */
    return toReturn;
}

QVector<IDrugInteraction *> DrugDrugInteractionEngine::getAllInteractionsFound()
{
    // if no interactions were found : return empty list
    QVector<IDrugInteraction *> toReturn;
    if (d->m_DDIFound.isEmpty())
        return toReturn;

    QTime t;
    t.start();

    QSqlDatabase DB = base()->database();
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
        }
    }

    // Retrieve from database
    QMap<int, int>::const_iterator i = d->m_DDIFound.constBegin();
    while (i != d->m_DDIFound.constEnd()) {
        toReturn << getInteractionsFromDatabase(i.key(), i.value());
        ++i;
    }

    // link DDI with IDrug
    int id1, id2;
    foreach(IDrugInteraction* di, toReturn) {
        DrugsInteraction *ddi = static_cast<DrugsInteraction *>(di);
        Q_ASSERT(ddi);
        id1 = ddi->value(DrugsInteraction::DI_ATC1).toInt();
        id2 = ddi->value(DrugsInteraction::DI_ATC2).toInt();
        // test all drugs in the list
        foreach(IDrug *drg, d->m_TestedDrugs)  {
            if (drg->allInnAndInteractingClassesIds().contains(id1) || drg->allInnAndInteractingClassesIds().contains(id2)) {
                ddi->addInteractingDrug(drg);
            }
        }
    }

    qSort(toReturn.begin(), toReturn.end(), IDrugInteraction::greaterThan);
    d->m_Interactions = toReturn;
    if (d->m_LogChrono)
        Utils::Log::logTimeElapsed(t, "DrugDrugInteractionEngine", QString("getAllInteractions : %1 drugs")
                                   .arg(d->m_TestedDrugs.count()));

    return toReturn;
}

QIcon DrugDrugInteractionEngine::maximumInteractingLevelIcon(const QVector<IDrugInteraction *> &interactions, const IDrug *drug, const int levelOfWarning, const int iconsize)
{
    TypesOfIAM level = getMaximumTypeOfIAM(interactions, drug);
    Core::ITheme *th = theme();
    Core::ITheme::IconSize size = Core::ITheme::IconSize(iconsize);
    // Minimal alerts
    if (level & ContreIndication && (levelOfWarning <= Constants::MinimumLevelOfWarning))
        return th->icon(Constants::INTERACTION_ICONCRITICAL, size);
    else if (level & Deconseille && (levelOfWarning <= Constants::MinimumLevelOfWarning))
        return th->icon(Constants::INTERACTION_ICONDECONSEILLEE, size);
    // Moderate alerts
    else if ((level & APrendreEnCompte) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
        return th->icon(Constants::INTERACTION_ICONTAKEINTOACCOUNT, size);
    else if ((level & P450) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
        return th->icon(Constants::INTERACTION_ICONP450, size);
    else if ((level & GPG) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
        return th->icon(Constants::INTERACTION_ICONGPG, size);
    else if ((level & Precaution) && (levelOfWarning <= Constants::ModerateLevelOfWarning))
        return th->icon(Constants::INTERACTION_ICONPRECAUTION, size);
    // Maximum alerts
    else if ((level & Information) && (levelOfWarning == Constants::MaximumLevelOfWarning))
        return th->icon(Constants::INTERACTION_ICONINFORMATION, size);
    else if ((level & InnDuplication) && (levelOfWarning == Constants::MaximumLevelOfWarning))
        return th->icon(Constants::INTERACTION_ICONINFORMATION, size);
    else if (level & NoIAM)
        return th->icon(Constants::INTERACTION_ICONOK, size);
    else
        return th->icon(Constants::INTERACTION_ICONUNKONW, size);
    return QIcon();
}

DrugDrugInteractionEngine::TypesOfIAM DrugDrugInteractionEngine::getMaximumTypeOfIAM(const QVector<IDrugInteraction *> &interactions, const IDrug *drug) const
{
    TypesOfIAM r = DrugDrugInteractionEngine::NoIAM;
    for(int i=0; i < interactions.count(); ++i) {
        IDrugInteraction *ddi = interactions.at(i);
        if (ddi->engine()==this) {
            if (ddi->drugs().contains((IDrug*)drug)) {
                r |= DrugDrugInteractionEngine::TypesOfIAM(ddi->sortIndex());
            }
        }
    }
    return r;
}
