/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DrugsDB::IComponent
 * \brief Represents a molecular component of a drug directly extracted from the drug database.
 * \sa DrugsDb::IDrug
*/

/**
 * \class DrugsDB::IVirtualComponent
 * \brief Represents a molecular component of a virtual drug. The virtual component is not extracted form the drug database but is constructed.
*/

/**
 * \fn DrugsDB::IComponent::IComponent(IDrug *parent)
 * Create a new component and associate it with the IDrug \e drug. When a component is associated to a IDrug,
 * the IDrug manages the deletion of the newly created IComponent.
 * \sa void DrugsDB::IDrug::addComponent()
*/

/**
  \fn virtual void DrugsDB::IComponent::linkWithComposition(IComponent *compo)
*/

/**
  \fn virtual QVariant DrugsDB::IComponent::data(const int ref, const QString &lang = QString::null) const
*/

/**
  \fn QStringList DrugsDB::IComponent::interactingClasses() const
  Return all know interacting classes for this component.
*/

/**
  \fn QString DrugsDB::IComponent::dosage() const
  Return the dosage of this molecular component.
*/

/**
  \fn QString DrugsDB::IComponent::form() const
  Return the form of this component. \sa DrugDB::IDrug::Form.
*/

/**
  \fn QString DrugsDB::IComponent::moleculeName() const
  Return the molecular name of this component.
*/

/**
  \fn QString DrugsDB::IComponent::nature() const
*/

/**
  \fn int DrugsDB::IComponent::lkNature() const
*/

/**
  \fn virtual bool DrugsDB::IVirtualComponent::setData(const int ref, const QVariant &value, const QString &lang = QString::null)
  Set the data of the virtual component.
*/

/**
  \class DrugsDB::IDrug
  \brief Represents a drug directly extracted from the drug database.
  \sa DrugsDb::IDrug


  \class DrugsDB::IVirtualDrug
  \brief Represents a virtual drug. The virtual drug is not extracted form the drug database but is constructed.

*/

#include "idrug.h"

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsbaseinfo.h>
#include <drugsbaseplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_drugs.h>

#include <QVector>
#include <QLocale>
#include <QHash>
#include <QString>

#include <QDebug>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

namespace DrugsDB {
namespace Internal {
class IComponentPrivate
{
public:
    IComponentPrivate() :
        m_Drug(0),
        m_Link(0),
        m_LinkOwned(false)
    {}

    ~IComponentPrivate()
    {}

public:
    QHash<int, QMultiHash<QString, QVariant> > m_Content;
    IDrug *m_Drug;
    QVector<int> m_7CharAtcIds, m_InteractingClassAtcIds;
    IComponent *m_Link;
    bool m_LinkOwned;
};

class IDrugPrivate
{
public:
    QHash<int, QMultiHash<QString, QVariant> > m_Content;
    QVector<IComponent *> m_Compo;
    QVector<int> m_7CharsAtc, m_InteractingClasses;
    QVector<int> m_AllIds; // is the sum of m_7CharsAtc && m_InteractingClasses; provided for computation speed improvements
    QStringList m_AllAtcCodes; // for speed improvements in allergy and intolerance computations
    QVector<DrugRoute *> m_Routes;
    QString m_NoLaboDenomination;
};


class DrugRoutePrivate
{
public:
    QHash<QString, QString> m_Labels;
    DrugRoute::SystemicEffects m_Syst;
    IDrug *m_Drug;
    int m_Rid;
};

class IPrescriptionPrivate
{
public:
    bool m_PrescriptionChanges;
    QHash< int, QVariant> m_PrescriptionValues;
    QMultiHash<int, QPair<QString, QString > > m_Routes;  // Pair: x = lang; y = label
};
}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ICOMPONENT  ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
IComponent::IComponent(IDrug *parent) :
        d_component(new Internal::IComponentPrivate)
{
    d_component->m_Drug = parent;
    if (d_component->m_Drug)
        d_component->m_Drug->addComponent(this);
    setDataFromDb(IsActiveSubstance, true);
}

IComponent::IComponent(IDrug *parent, const IComponent &copy) :
    d_component(new Internal::IComponentPrivate())
{
    d_component->m_Content = copy.d_component->m_Content;
    d_component->m_7CharAtcIds = copy.d_component->m_7CharAtcIds;
    d_component->m_InteractingClassAtcIds = copy.d_component->m_InteractingClassAtcIds;
    d_component->m_Drug = parent;
    d_component->m_LinkOwned = true;
    if (d_component->m_Link && !d_component->m_LinkOwned)
        d_component->m_Link = new IComponent(parent, *copy.d_component->m_Link);
    if (d_component->m_Drug)
        d_component->m_Drug->addComponent(this);
    setDataFromDb(IsActiveSubstance, true);
}

IComponent::~IComponent()
{
    if (d_component->m_LinkOwned) {
        delete d_component->m_Link;
        d_component->m_Link = 0;
    }
    if (d_component)
        delete d_component;
    d_component = 0;
}

/** \brief Set the linked DrugsDB::IComponent (this happens when a molecule is transform to another one which is the active one). */
void IComponent::linkWithComposition(IComponent *link)
{
    Q_ASSERT(link);
    d_component->m_Link = link;
    if (!link->isLinkedWith(this)) {
        link->linkWithComposition(this);
        bool active = (data(Nature).toString()=="SA"); // Nature can be SA: ActiveSubstance or FT: TherapeuticFraction
        setDataFromDb(IsActiveSubstance, active);
        link->setIsActiveSubstance(!active);
    }
}

/**
 * Returns true if the component is linked with the \e compo
 */
bool IComponent::isLinkedWith(IComponent *compo) const
{
    return compo==d_component->m_Link;
}

/**
 * Return the linked component or zero if the component is not linked. \n
 * Information: some components are linked to each other, one represents
 * the therapeutic fraction the other the active substance). \n
 * \warning The returned pointer must not be deleted.
 */
IComponent *IComponent::linkedWith() const
{
    return d_component->m_Link;
}

IDrug *IComponent::drug() const
{
    return d_component->m_Drug;
}

QVariant IComponent::data(const int ref, const QString &lang) const
{
    QString language;
    if (lang.isEmpty())
        language = QString("xx");
    else
        language = lang;

    switch (ref) {
    case Strength:
    {
        QString s = d_component->m_Content.value(ref).value(language).toString();
        s.replace(",000","");
        s.replace(",00","");
        return s;
    }
    case FullDosage:
    {
        QString strength = data(Strength, language).toString() + data(StrengthUnit, language).toString();
        QString refDose = data(Dose, language).toString() + data(DoseUnit, language).toString();
        if (!refDose.isEmpty())
            return QString(strength + "/" + refDose);
        return strength;
    }
    case AtcId:
    {
        // TODO: A component can have multiple AtcIds.
        if (!d_component->m_7CharAtcIds.isEmpty())
            return d_component->m_7CharAtcIds.at(0);
        return -1;
    }
    case AtcLabel:
    {
        if (d_component->m_7CharAtcIds.isEmpty())
            return QString();
        return drugsBase().getAtcLabel(d_component->m_7CharAtcIds.at(0));
    }
    case AtcCode:
    {
        if (d_component->m_7CharAtcIds.isEmpty())
            return QString();
        return drugsBase().getAtcCode(d_component->m_7CharAtcIds.at(0));
    }
    case InteractingClassNames:
    {
        QStringList names;
        for(int i=0; i < d_component->m_InteractingClassAtcIds.count(); ++i) {
            names << drugsBase().getAtcLabel(d_component->m_InteractingClassAtcIds.at(i));
        }
        return names;
    }
    default: return d_component->m_Content.value(ref).value(language);
    }
    return d_component->m_Content.value(ref).value(language);
}

/**
 * Returns the galenic form associated with this component.
 */
QString IComponent::form() const
{
    return d_component->m_Drug->data(IDrug::Forms).toString();
}

/**
 * Returns true if the component owns the main INN molecule.
 * This one will also own the INN label, dosage and form.
 */
bool IComponent::isMainInn() const
{
    // Can be the main INN component only component linked with an ATC code & having an innName defined
    if (d_component->m_7CharAtcIds.count() > 0
            && !innName().isEmpty()) {
        // If the component is linked, test the nature of the component. Only
        // Therapeutic Fraction ('FT') are main INN.
        if (d_component->m_Link) {
            return (nature()=="FT");
        } else {
            return true;
        }
    }
    return false;
}

QVector<int> IComponent::innAtcIds() const
{
    return d_component->m_7CharAtcIds;
}

QVector<int> IComponent::interactingClassAtcIds() const
{
    return d_component->m_InteractingClassAtcIds;
}

/**
 * Not coded
 */
QString IComponent::toXml() const
{
    // TODO: code here
    return QString();
}

/**
 * Set the internal data from the database.
 */
bool IComponent::setDataFromDb(const int ref, const QVariant &value, const QString &lang)
{
    d_component->m_Content[ref].insertMulti(lang, value);
    return true;
}

/**
 * Set the internal data from the database.
 */
void IComponent::set7CharAtcIds(const QVector<int> &atcIds)
{
    d_component->m_7CharAtcIds = atcIds;
}

/**
 * Set the internal data from the database.
 */
void IComponent::setInteractingClassAtcIds(const QVector<int> &atcIds)
{
    d_component->m_InteractingClassAtcIds = atcIds;
}

IVirtualComponent::IVirtualComponent(IVirtualDrug *parent) :
        IComponent(parent)
{
}

IVirtualComponent *IVirtualComponent::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
    return 0;
}

QString IVirtualComponent::toXml() const
{
    return QString();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////  DRUGROUTE  ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
DrugRoute::DrugRoute(IDrug *drug) :
        d(new Internal::DrugRoutePrivate)
{
    d->m_Drug = drug;
    if (d->m_Drug)
        d->m_Drug->addRoute(this);
    d->m_Syst = UnknownSystemicEffect;
}

DrugRoute::DrugRoute(IDrug *drug, const DrugRoute &copy) :
    d(new Internal::DrugRoutePrivate)
{
    d->m_Labels = copy.d->m_Labels;
    d->m_Syst = copy.d->m_Syst;
    d->m_Rid = copy.d->m_Rid;
    d->m_Drug = drug;
    if (d->m_Drug)
        d->m_Drug->addRoute(this);
}

DrugRoute::~DrugRoute()
{
    if (d)
        delete d;
    d = 0;
}

IDrug *DrugRoute::drug() const {return d->m_Drug;}
QString DrugRoute::label(const QString &lang) const
{
    QString l = lang;
    if (lang.isEmpty()) {
        l = QLocale().name().left(2);
        if (d->m_Labels.keys().contains(l)) {
            return d->m_Labels.value(l);
        } else {
            return d->m_Labels.value(Trans::Constants::ALL_LANGUAGE);
        }
    }
    if (d->m_Labels.keys().contains(l)) {
        return d->m_Labels.value(l);
    } else if (d->m_Labels.keys().contains(Trans::Constants::ALL_LANGUAGE)) {
        return d->m_Labels.value(Trans::Constants::ALL_LANGUAGE);
    }
    return QString();
}

DrugRoute::SystemicEffects DrugRoute::systemicEffect() const
{
    return d->m_Syst;
}

void DrugRoute::setSystemicDatabaseTag(const QString &systemic)
{
    if (systemic.compare("yes", Qt::CaseInsensitive)==0) {
        d->m_Syst = HasSystemicEffect;
    } else if (systemic.compare("no", Qt::CaseInsensitive)==0) {
        d->m_Syst = NoSystemicEffect;
    } else if (systemic.compare("partial", Qt::CaseInsensitive)==0) {
        d->m_Syst = PartialSystemicEffect;
    } else {
        d->m_Syst = UnknownSystemicEffect;
    }
}

void DrugRoute::setLabel(const QString &label, const QString &lang)
{
    QString l = lang;
    if (lang.isEmpty()) {
        l = Trans::Constants::ALL_LANGUAGE;
    }
    d->m_Labels.insert(l, label);
}

int DrugRoute::routeId() const
{
    return d->m_Rid;
}

void DrugRoute::setRouteId(const int rid)
{
    d->m_Rid = rid;
}

DrugRoute::SystemicEffects DrugRoute::maximumSystemicEffect(const QVector<DrugRoute *> &routes)  // Static
{
    if (routes.isEmpty())
        return UnknownSystemicEffect;
    DrugRoute::SystemicEffects max = NoSystemicEffect;
    for(int i = 0; i < routes.count(); ++i) {
        DrugRoute *r = routes.at(i);
        if (r->systemicEffect() > max)
            max = r->systemicEffect();
    }
    return max;
}

DrugRoute::SystemicEffects DrugRoute::maximumSystemicEffect(const IDrug *drug)  // Static
{
    return DrugRoute::maximumSystemicEffect(drug->drugRoutes());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////  IPrescription  //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
IPrescription::IPrescription() :
    d_pres(new Internal::IPrescriptionPrivate)
{
    d_pres->m_PrescriptionChanges = false;
}

IPrescription::IPrescription(const IPrescription &copy) :
    d_pres(new Internal::IPrescriptionPrivate(*copy.d_pres))
{
    d_pres->m_PrescriptionChanges = copy.d_pres->m_PrescriptionChanges;
    d_pres->m_PrescriptionValues = copy.d_pres->m_PrescriptionValues;
    d_pres->m_Routes = copy.d_pres->m_Routes;
}

IPrescription::~IPrescription()
{
    if (d_pres)
        delete d_pres;
    d_pres = 0;
}

void IPrescription::setPrescriptionValue(const int fieldref, const QVariant &value)
{
    if (d_pres->m_PrescriptionValues.value(fieldref) == value)
        return;

    if (fieldref==Constants::Prescription::RouteId) {
        // RouteId is coherent with this drug ?
        int routeId = value.toInt();
        for(int i=0; i < drugRoutes().count(); ++i) {
            if (drugRoutes().at(i)->routeId()==routeId) {
                d_pres->m_PrescriptionValues[fieldref] = value;
                d_pres->m_PrescriptionChanges = true;
                setPrescriptionValue(Constants::Prescription::Route, drugRoutes().at(i)->label());
                return;
            }
        }
        // ERROR NOT FOUND
        LOG_ERROR_FOR("IPrescription", "RouteID not found");
        return;
    }
    d_pres->m_PrescriptionChanges = true;
    d_pres->m_PrescriptionValues[fieldref] = value;
}

bool IPrescription::hasPrescription() const
{
    // TODO: this needs improvements ++
    int i = 0;
    foreach(const QVariant &q , d_pres->m_PrescriptionValues) {
        if (!q.isNull())
            i++;
    }
    return i > 2;
}

QVariant IPrescription::prescriptionValue(const int fieldref) const
{
    using namespace DrugsDB::Constants;
    switch (fieldref)
    {
    case Prescription::IntakesFullString:
    {
        QString full;
        const QString &from = d_pres->m_PrescriptionValues.value(Prescription::IntakesFrom).toString();
        if (from.isEmpty())
            return QVariant();
        const QString &to = d_pres->m_PrescriptionValues.value(Prescription::IntakesTo).toString();
        const QString &scheme = d_pres->m_PrescriptionValues.value(Prescription::IntakesScheme).toString();
        bool fromTo = d_pres->m_PrescriptionValues.value(Prescription::IntakesUsesFromTo).toBool();
        if (fromTo) {
            full = tkTr(Trans::Constants::FROM_1_TO_2).arg(from).arg(to);
        } else {
            full = from;
        }
        full += " " + scheme;
        return full;
    }
    case Prescription::DurationFullString:
    {
        QString full;
        const QString &from = d_pres->m_PrescriptionValues.value(Prescription::DurationFrom).toString();
        if (from.isEmpty())
            return QVariant();
        const QString &to = d_pres->m_PrescriptionValues.value(Prescription::DurationTo).toString();
        const QString &scheme = d_pres->m_PrescriptionValues.value(Prescription::DurationScheme).toString();
        bool fromTo = d_pres->m_PrescriptionValues.value(Prescription::DurationUsesFromTo).toBool();
        if (fromTo) {
            full = tkTr(Trans::Constants::FROM_1_TO_2).arg(from).arg(to);
        } else {
            full = from;
        }
        full += " " + scheme;
        return full;
    }
    case Prescription::PeriodFullString:
    {
        QString full;
        const QString &val = d_pres->m_PrescriptionValues.value(Prescription::Period).toString();
        if (val.isEmpty())
            return QVariant();
        const QString &scheme = d_pres->m_PrescriptionValues.value(Prescription::PeriodScheme).toString();
        full = val + " " + scheme;
        return full;
    }
    case Prescription::IntakesIntervalFullString:
    {
        QString full;
        const QString &val = d_pres->m_PrescriptionValues.value(Prescription::IntakesIntervalOfTime).toString();
        if (val.isEmpty())
            return QVariant();
        int scheme = d_pres->m_PrescriptionValues.value(Prescription::IntakesIntervalSchemeIndex).toInt();
        full = QString("%1 %2").arg(val).arg(period(scheme));
        return full;
    }
    case Prescription::IntakesTo :
    {
        if (!d_pres->m_PrescriptionValues.value(Prescription::IntakesUsesFromTo,false).toBool())
            return QVariant();
        break;
    }
    case Prescription::DurationTo :
    {
        if (!d_pres->m_PrescriptionValues.value(Prescription::DurationUsesFromTo,false).toBool())
            return QVariant();
        break;
    }
    case Prescription::Route :
    {
        // TODO: code here
        //            QString lang = QLocale().name().left(2);
        //            typedef QPair<QString, QString> pair;
        //            foreach(const pair &p, d_pres->m_Routes.values(d->m_PrescriptionValues.value(Prescription::Route,-1).toInt())) {
        //                if (p.first == lang)
        //                    return p.second;
        //            }
        break;
    }
    }
    return d_pres->m_PrescriptionValues.value(fieldref);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////  IDRUG  //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
IDrug::IDrug() :
    IPrescription(),
    d_drug(new Internal::IDrugPrivate)
{
}

IDrug::IDrug(const IDrug &copy) :
    IPrescription(copy),
    d_drug(new Internal::IDrugPrivate)
{
    d_drug->m_Content = copy.d_drug->m_Content;
    d_drug->m_7CharsAtc = copy.d_drug->m_7CharsAtc;
    d_drug->m_InteractingClasses = copy.d_drug->m_InteractingClasses;
    d_drug->m_AllIds = copy.d_drug->m_AllIds;
    d_drug->m_AllAtcCodes = copy.d_drug->m_AllAtcCodes;
    d_drug->m_NoLaboDenomination = copy.d_drug->m_NoLaboDenomination;
    // make copies of components
    d_drug->m_Compo.clear();
    foreach(IComponent *compo, copy.d_drug->m_Compo) {
        new IComponent(this, *compo); // component is added to the drug in the ctor
    }
    // make copies of routes
    d_drug->m_Routes.clear();
    foreach(DrugRoute *route, copy.d_drug->m_Routes) {
        new DrugRoute(this, *route); // route is added to the drug in the ctor
    }
}

IDrug::~IDrug()
{
    qDeleteAll(d_drug->m_Compo);
    qDeleteAll(d_drug->m_Routes);
    if (d_drug)
        delete d_drug;
    d_drug = 0;
}

QVariant IDrug::data(const int ref, const QString &lang) const
{
    if (ref < 0 || ref > MaxParam)
        return QVariant();
    switch (ref) {
    case Name:
    {
        if (settings()->value(Constants::S_HIDELABORATORY).toBool()) {
            if (d_drug->m_NoLaboDenomination.isEmpty()) {
                if (lang.isEmpty())
                    d_drug->m_NoLaboDenomination = d_drug->m_Content.value(Name).value(QLocale().name().left(2)).toString();
                if (d_drug->m_NoLaboDenomination.isEmpty() && lang.isEmpty())
                    d_drug->m_NoLaboDenomination = d_drug->m_Content.value(Name).value(Trans::Constants::ALL_LANGUAGE).toString();
                foreach(const QString &name, Constants::LABOS) {
                    if (d_drug->m_NoLaboDenomination.contains(" " + name + " ")) {
                        d_drug->m_NoLaboDenomination.remove(" " + name + " ");
                        return d_drug->m_NoLaboDenomination;
                    }
                }
            }
            return d_drug->m_NoLaboDenomination;
        }
        break;
    }
    case Spc:
    {
        QString toReturn;
        if (lang.isEmpty()) {
            toReturn = d_drug->m_Content.value(Spc).value(QLocale().name().left(2)).toString();
            if (toReturn.isEmpty())
                toReturn = d_drug->m_Content.value(Spc).value(Trans::Constants::ALL_LANGUAGE).toString();
        } else {
            toReturn = d_drug->m_Content.value(Spc).value(lang).toString();
        }
        return toReturn;
    }
    case AllUids:
    {
        QStringList list;
        list << data(Uid1).toString() << data(Uid2).toString() << data(Uid3).toString();
        return list;
    }
    case AtcLabel :
    {
        QStringList toReturn;
        for(int i = 0; i < d_drug->m_7CharsAtc.count(); ++i) {
            toReturn << drugsBase().getAtcLabel(d_drug->m_7CharsAtc.at(i));
        }
        toReturn.removeDuplicates();
        return toReturn;
    }
    case Forms:
    {
        // for textual and virtual drugs, forms are set using setDataFromDb()
        if (d_drug->m_Content.value(ref).value(lang).isNull()) {
            if (d_drug->m_Content.value(ref).value(Trans::Constants::ALL_LANGUAGE).isNull())
                return drugsBase().getFormLabels(d_drug->m_Content.value(DrugID).value(Trans::Constants::ALL_LANGUAGE));
            else
                return d_drug->m_Content.value(ref).value(Trans::Constants::ALL_LANGUAGE);
        }
        return d_drug->m_Content.value(ref).value(lang);
    }
    case Routes:
    {
        QStringList routes;
        for(int i = 0; i < d_drug->m_Routes.count(); ++i) {
            routes << d_drug->m_Routes.at(i)->label(lang);
        }
        //            qWarning() << "IDrug::data::Routes" << routes << drugsBase().getRouteLabels(d_drug->m_Content.value(DrugID).value(Trans::Constants::ALL_LANGUAGE));
        return routes;
    }
    case MainInnCode:
    {
        return mainInnCode();
    }
    case AllInnsKnown:
    {
        foreach(const IComponent *compo, d_drug->m_Compo) {
            // TODO: code here manage virtual components
            if (compo->isMainInn()) {
                if (compo->innAtcIds().isEmpty())
                    return false;
            }
        }
        return true;
    }
    case All7CharsAtcLabels :
    {
        QStringList toReturn;
        foreach(IComponent *compo, d_drug->m_Compo) {
            toReturn << compo->innName();
        }
        toReturn.removeDuplicates();
        toReturn.removeAll("");
        return toReturn;
    }
    case All7CharsAtcCodes:
    {
        QStringList toReturn;
        for(int i = 0; i < d_drug->m_7CharsAtc.count(); ++i) {
            toReturn << drugsBase().getAtcCode(d_drug->m_7CharsAtc.at(i));
        }
        toReturn.removeDuplicates();
        return toReturn;
    }
    case AllInteractingClasses:
    {
        QStringList names;
        for(int i=0; i < d_drug->m_InteractingClasses.count(); ++i) {
            names << drugsBase().getAtcLabel(d_drug->m_InteractingClasses.at(i));
        }
        return names;
    }
    case AllMoleculeNames :
    {
        QStringList toReturn;
        foreach(const IComponent *compo, d_drug->m_Compo)
            toReturn << compo->moleculeName();
        return toReturn;
    }
    case AllAtcCodes:
    {
        if (d_drug->m_AllAtcCodes.isEmpty()) {
            for(int i=0; i < d_drug->m_AllIds.count(); ++i) {
                QString code = drugsBase().getAtcCode(d_drug->m_AllIds.at(i));
                if (!d_drug->m_AllAtcCodes.contains(code))
                    d_drug->m_AllAtcCodes << code;
            }
            d_drug->m_AllAtcCodes << d_drug->m_Content.value(AtcCode).value(Trans::Constants::ALL_LANGUAGE).toString();
        }
        return d_drug->m_AllAtcCodes;
    }
    }
    if (lang.isEmpty())
        return d_drug->m_Content.value(ref).value(Trans::Constants::ALL_LANGUAGE);
    else
        return d_drug->m_Content.value(ref).value(lang);
}

/** Returns the number of molecule the drug contains */
int IDrug::numberOfCodeMolecules() const
{
    return d_drug->m_Compo.count();
}

/**
 * Return the main ATC id linked with this drug.
 * \warning some drugs can contains multiple ATC ids.
*/
int IDrug::mainInnCode() const
{
    // TODO: Manage multiple ATC drugs
    for(int i=0; i < d_drug->m_Compo.count(); ++i) {
        IComponent *compo = d_drug->m_Compo.at(i);
        if (compo->isMainInn())
            return compo->data(IComponent::AtcId).toInt();
    }
    return -1;
}

/**
 * Returns the main INN name of the drug.
 * This is usable with drugs containing only one INN.
 */
QString IDrug::mainInnName() const
{
    // TODO: Manage multiple ATC drugs
    for(int i=0; i < d_drug->m_Compo.count(); ++i) {
        IComponent *compo = d_drug->m_Compo.at(i);
        if (compo->isMainInn())
            return compo->data(IComponent::AtcLabel).toString();
    }
    return QString::null;
}

/**
 * Returns the main INN dosage of the drug.
 * This is usable with drugs containing only one INN.
 */
QString IDrug::mainInnDosage() const
{
    // TODO: Manage multiple ATC drugs
    for(int i=0; i < d_drug->m_Compo.count(); ++i) {
        IComponent *compo = d_drug->m_Compo.at(i);
        if (compo->isMainInn())
            return compo->data(IComponent::FullDosage).toString();
    }
    return QString::null;
}

/**
 * Returns the INN composition of the drug: ATC label and dosage of the ATC.
 */
QString IDrug::innComposition() const
{
    // Screen all component for an INN
    QStringList inns;
    QStringList dosages;
    for(int i=0; i < d_drug->m_Compo.count(); ++i) {
        IComponent *compo = d_drug->m_Compo.at(i);
        if (!compo->innName().isEmpty() && !inns.contains(compo->innName())) {
            inns << compo->innName();
            dosages << compo->data(IComponent::FullDosage).toString();
        }
    }
    QStringList r;
    for(int i = 0; i < inns.count(); ++i)
        r << QString("%1 %2").arg(inns.at(i)).arg(dosages.at(i));
    return r.join("; ");
    // return QString("%1 %2").arg(mainInnName()).arg(mainInnDosage());
}

/**
 * Returns all related routes
 */
QVector<DrugRoute *> IDrug::drugRoutes() const
{
    return d_drug->m_Routes;
}

/** Returns true if the drugs contains the ATC id \e atcId */
bool IDrug::atcIdsContains(const int atcId)
{
    return d_drug->m_AllIds.contains(atcId);
}

namespace {
    const char *const XML_COMPOSITION                  = "Composition";
    const char *const XML_COMPOSITION_INN              = "inn";
    // FIXME: unused? const char *const XML_COMPOSITION_ATC              = "atc";
    const char *const XML_COMPOSITION_FORM             = "form";
    const char *const XML_COMPOSITION_ROUTE            = "route";
    // FIXME: unused? const char *const XML_COMPOSITION_STRENGTH         = "strenght";
    const char *const XML_COMPOSITION_MOLECULAR        = "molecularName";
    const char *const XML_COMPOSITION_NATURE           = "nature";
    const char *const XML_COMPOSITION_NATURE_LK        = "natureLink";
}

/** Returns the composition of the drug in XML format. */
QString IDrug::compositionToXml()
{
    QString tmp;
    // TODO: add ATC to drug composition and XML_COMPOSITION_STRENGTH
    foreach(IComponent *compo, d_drug->m_Compo) {
        tmp += QString("<%1").arg(::XML_COMPOSITION);
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_INN).arg(compo->innName());
//        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_ATC).arg(compo);
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_FORM).arg(compo->form());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_ROUTE).arg(routes().join(", "));
//        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_STRENGTH).arg(compo->innDosage());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_MOLECULAR).arg(compo->moleculeName());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_NATURE).arg(compo->nature());
        tmp += QString(" %1=\"%2\" ").arg(::XML_COMPOSITION_NATURE_LK).arg(compo->lkNature());
        tmp += QString("/>\n");
    }
    return tmp;
}

/** Return a human readable HTML text representing the data of this drug */
QString IDrug::toHtml() const
{
    QString msg;

    const QStringList &iams = this->listOfInnLabels();
    const QStringList &iamClass = this->listOfInteractingClasses();
    QString textIams, textClass;
    if (iams.isEmpty())
        textIams = QCoreApplication::translate("DrugsData", "No INN found.");
    else
        textIams = iams.join("<br>");

    if (iamClass.isEmpty())
        textClass = QCoreApplication::translate("DrugsData", "No interaction class found.");
    else
        textClass = iamClass.join("<br>");

    // TODO: code here: UIDs not UID
    QString uidName = "UID";
    if (drugsBase().actualDatabaseInformation()) {
        if (!drugsBase().actualDatabaseInformation()->drugsUidName.isEmpty())
            uidName = drugsBase().actualDatabaseInformation()->drugsUidName;
    }
    QString atc = atcCode();
    if (!atc.isEmpty()) {
        atc.prepend(" ; ATC: ");
    }
    QString mark;
//    mark = value(Table_DRUGS, DRUGS_AUTHORIZATION).toString();
//    if (!mark.isEmpty()) {
//        mark.prepend("<br>");
//        mark += " | " + value(Table_DRUGS, DRUGS_TYPE_MP).toString();
//    }

    msg += QString("<table border=1 cellpadding=2 cellspacing=2 width=100%>\n"
                    " <tr>\n"
                    "   <td colspan=2 rowspan=1 align=center>\n"
                    "       <span style=\"font-weight: bold;\">%1</span>\n"
                    "       %2"
                    "   </td>\n"
                    " </tr>\n"
                    " <tr>\n"
                    "   <td colspan=2 rowspan=1>\n"
                    "       %3\n"
                    "       <br>%4\n"
                    "       <br>%5 %6: %7 %8\n"
                    "   </td>\n"
                    " </tr>\n")
            .arg(brandName())
            .arg(mark)
            // TODO: code here
            .arg(QCoreApplication::translate("DrugsData", "Form(s): ")) //+ value(Table_DRUGS, DRUGS_FORM).toString())
            .arg(QCoreApplication::translate("DrugsData", "Route(s): ")) //+ value(Table_DRUGS, DRUGS_ROUTE).toString())
            .arg(QCoreApplication::translate("DrugsData", "Identifier(s): "), uidName)
            .arg(uids().join(";"))
            .arg(atc)
            ;

    QString tmp = "";
    QString name;
    QString inn;
    // Add component to the HTML
    foreach(IComponent *compo, d_drug->m_Compo) {
        // Add each molecule names
        name = compo->moleculeName();
        if (compo->isActiveSubstance()) {
            name += QString(" [%1]").arg(tkTr(Trans::Constants::ACTIVE_SUBSTANCE));
        } else {
            name += QString(" [%1]").arg(tkTr(Trans::Constants::THERAPEUTIC_FRACTION));
        }
        tmp += QString("<tr><td>%1</td><td>%2</td></tr>")
                .arg(name)
                .arg(compo->dosage());
        // FIXME: use tkTr(Trans::Constants::_1_COLON_2) "%1: %2" translation
        if (compo->isMainInn())
            inn += QString("<tr><td>%1: %2</td><td>%3</td></tr>")
                    .arg(tkTr(Trans::Constants::INN_PRESCRIPTION))
                    .arg(compo->innName())
                    .arg(compo->dosage());
        name.clear();
    }


    msg += QString(" %1\n"
                   "%2\n"
                   " <tr>\n"
                   "   <td colspan=2 rowspan=1>%3</td>\n"
                   " </tr>\n"
                   "</table>\n\n")
            .arg(tmp)
            .arg(inn)
            .arg(textClass);
    return msg;
}

/** Used to sort the drugs list. */
bool IDrug::lessThan(const IDrug *drug1, const IDrug *drug2)
{
    bool ald1, ald2;
    ald1 = drug1->prescriptionValue(Constants::Prescription::IsALD).toBool();
    ald2 = drug2->prescriptionValue(Constants::Prescription::IsALD).toBool();
//    qWarning() << "lessThan" << drug1->brandName() << drug2->brandName() << ald1 << ald2 << (drug1->brandName() < drug2->brandName());
    // ALD First of all
    if ((ald1)  && (!ald2)) {
        return true;
    } else if ((!ald1)  && (ald2)) {
        return false;
    }
    return drug1->brandName() < drug2->brandName();
}

/** Test equality with another drug. */
bool IDrug::equals(const IDrug *d)
{
    return (this->uids() == d->uids() &&
            this->data(SourceID) == d->data(SourceID) &&
            this->brandName() == d->brandName());
}

/** Insert data from the database */
bool IDrug::setDataFromDb(const int ref, const QVariant &value, const QString &lang)
{
    if (lang.isEmpty())
        d_drug->m_Content[ref].insertMulti(Trans::Constants::ALL_LANGUAGE, value);
    else
        d_drug->m_Content[ref].insertMulti(lang, value);
    return true;
}

/** Add a DrugsDB::IComponent to this drug */
void IDrug::addComponent(IComponent *compo)
{
    d_drug->m_Compo.append(compo);
}

/**
 * Returns the complete list of components of the drug. \n
 * \warning Pointers must not be deleted.
 */
QVector<IComponent *> IDrug::components() const
{
    return d_drug->m_Compo;
}

/** Add a DrugsDB::DrugsRoute to this drug */
void IDrug::addRoute(DrugRoute *route)
{
    d_drug->m_Routes << route;
}

void IDrug::setAll7CharsAtcIds(const QVector<int> &ids)
{
    d_drug->m_7CharsAtc = ids;
    d_drug->m_AllIds = d_drug->m_7CharsAtc + d_drug->m_InteractingClasses;
    d_drug->m_AllAtcCodes.clear();
}

void IDrug::setAllInteractingClassesIds(const QVector<int> &ids)
{
    d_drug->m_InteractingClasses = ids;
    d_drug->m_AllIds = d_drug->m_7CharsAtc + d_drug->m_InteractingClasses;
    d_drug->m_AllAtcCodes.clear();
}

/** Call this once all components have been added to the drug. */
void IDrug::constructAtcIdsVectorsUsingComponents()
{
    d_drug->m_7CharsAtc.clear();
    d_drug->m_InteractingClasses.clear();
    d_drug->m_AllIds.clear();
    foreach(IComponent *compo, d_drug->m_Compo) {
        for(int i = 0; i < compo->innAtcIds().count(); ++i) {
            int id = compo->innAtcIds().at(i);
            if (!d_drug->m_7CharsAtc.contains(id))
                d_drug->m_7CharsAtc << id;
        }
        for(int i = 0; i < compo->interactingClassAtcIds().count(); ++i) {
            int id = compo->interactingClassAtcIds().at(i);
            if (!d_drug->m_InteractingClasses.contains(id))
                d_drug->m_InteractingClasses << id;
        }
    }
    d_drug->m_AllIds << d_drug->m_7CharsAtc;
    d_drug->m_AllIds << d_drug->m_InteractingClasses;
}

QVector<int> IDrug::allInnAndInteractingClassesIds() const
{
    return d_drug->m_AllIds;
}

QVector<int> IDrug::allSevenCharsAtcIds() const
{
    return d_drug->m_7CharsAtc;
}

QVector<int> IDrug::allAtcIds() const
{
    return d_drug->m_AllIds;
}

QVector<int> IDrug::molsIds() const
{
    QVector<int> molsIds;
    foreach(IComponent *compo, d_drug->m_Compo) {
        molsIds << compo->data(IComponent::MID).toInt();
    }
    return molsIds;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ITEXTUALDRUG  //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
ITextualDrug::ITextualDrug() :
        IDrug()
{
    setDataFromDb(Uid1, -1);
}

/** Define the denomination of this textual drug */
void ITextualDrug::setDenomination(const QString &denomination)
{
    setDataFromDb(Name, denomination);
}

void ITextualDrug::setPrescriptionValue(const int fieldref, const QVariant &value)
{
    if (fieldref==Constants::Prescription::IsTextualOnly)
        return;
    return IDrug::setPrescriptionValue(fieldref, value);
}

QVariant ITextualDrug::prescriptionValue(const int fieldref) const
{
    if (fieldref==Constants::Prescription::IsTextualOnly)
        return true;
    return IDrug::prescriptionValue(fieldref);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  IVIRTUALDRUG  //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
IVirtualDrug *IVirtualDrug::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
    return 0;
}

QString IVirtualDrug::toXml() const
{
    return QString();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
QDebug operator<<(QDebug dbg, const DrugsDB::IDrug *c)
{
    if (!c) {
        dbg.nospace() << "IDrug(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}

QDebug operator<<(QDebug dbg, const DrugsDB::IDrug &c)
{
    dbg.nospace() << "IDrug[" << c.brandName() << "]("
                  << "\n    ID:       " << c.drugId().toString()
                  << "\n    Uids:     " << c.uids().join(";")
                  << "\n    AtcLabel: " << c.atcLabel()
                  << "\n    Strength: " << c.strength()
                  << "\n    Forms:    " << c.forms().join(",")
                  << "\n    Routes:   " << c.routes().join(",")
                  << "\n    LinkScp:  " << c.linkToSCP()
                  << "\n    NbMols:   " << c.numberOfCodeMolecules()
                  << "\n    NbInns:   " << c.numberOfInn()
                  << "\n    Mols:     " << c.listOfMolecules().join(";")
                  << "\n    INN list:       " << c.listOfInnLabels().join(";")
                  << "\n    DDIClasses:     " << c.listOfInteractingClasses().join(";")
                  << "\n    MainInnName:    " << c.mainInnName()
                  << "\n    MainInnDosage:  " << c.mainInnDosage()
                  << "\n    InnComposition: " << c.innComposition()
                     ;

    for(int i = 0; i < c.components().count(); ++i) {
        DrugsDB::IComponent *compo = c.components().at(i);
        qWarning() << compo;
    }
    dbg.nospace() << "\n    ~)IDrug[" << c.brandName() << "]\n";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const DrugsDB::IComponent *c)
{
    if (!c) {
        dbg.nospace() << "IComponent(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}

QDebug operator<<(QDebug dbg, const DrugsDB::IComponent &c)
{
    QStringList atcIds, atcLabels;
    for(int i=0; i < c.innAtcIds().count(); ++i) {
        atcIds << QString::number(c.innAtcIds().at(i));
        atcLabels << drugsBase().getAtcLabel(c.innAtcIds().at(i));
    }
    dbg.nospace() << "IComponent[" << c.moleculeName() << "]("
                  << "\n      Form:       " << c.form()
                  << "\n      INN:        " << c.innName()
                  << "\n      IsMain:     " << QString(c.isMainInn()?"true":"false")
                  << "\n      FullDosage: " << c.dosage()
                  << "\n      Nature:     " << c.nature()
                  << "\n      AtcIds:     " << atcIds.join("; ")
                  << "\n      AtcLabels:  " << atcLabels.join("; ")
                  << "\n      DDIClasses: " << c.interactingClasses().join("; ")
                     ;
    if (c.linkedWith())
        dbg.nospace() << "\n      Linked:     " << c.linkedWith()->moleculeName();
    dbg.nospace() << "\n      )";
    return dbg.space();
}
