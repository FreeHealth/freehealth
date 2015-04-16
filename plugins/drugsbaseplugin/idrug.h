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
#ifndef DRUGSDB_IDRUG_H
#define DRUGSDB_IDRUG_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QVariant>
#include <QVector>
#include <QStringList>

/**
 * \file idrug.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.4
 * \date 04 Feb 2012
*/

namespace DrugsDB {
class IDrug;
class IVirtualDrug;
class DrugsIO;
class DrugsBase;

namespace Internal {
class IComponentPrivate;
class IDrugPrivate;
class DrugRoutePrivate;
class DrugsBasePrivate;
class IPrescriptionPrivate;
class DrugsIOPrivate;
}

class DRUGSBASE_EXPORT IComponent
{
    friend class DrugsDB::DrugsBase;
    friend class DrugsDB::Internal::DrugsBasePrivate;

public:
    enum References {
        MID = 0,
        Name,
        Strength,
        StrengthUnit,
        Dose,
        DoseUnit,
        FullDosage,
        Nature,
        NatureLink,
        WebLink,
        IsActiveSubstance,
        AtcLabel,
        AtcCode,
        AtcId,
//        MainAtcId,
//        MainAtcCode,
//        MainAtcName,
//        MainAtcDosage,
        InteractingClassNames
    };

    IComponent(IDrug *parent);
    IComponent(IDrug *parent, const IComponent &copy);

    virtual ~IComponent();

    virtual void linkWithComposition(IComponent *compo);
    virtual bool isLinkedWith(IComponent *compo) const;
    virtual IComponent *linkedWith() const;

    virtual bool isVirtual() const {return false;}
    IDrug *drug() const;

    virtual QVariant data(const int ref, const QString &lang = QString::null) const;

    QString innName() const {return data(AtcLabel).toString();}
//    virtual QString mainInnName() const {return data(MainAtcName).toString();}
//    virtual QString mainInnDosage() const {return data(MainAtcDosage).toString();}
//    virtual int mainInnCode() const {return data(MainAtcId).toInt();}

    QStringList interactingClasses() const {return data(InteractingClassNames).toStringList();}
    QString dosage() const {return data(FullDosage).toString();}
    QString form() const;
    QString moleculeName() const {return data(Name).toString();}

    QString nature() const {return data(Nature).toString();}
    int lkNature() const {return data(NatureLink).toInt();}
    bool isActiveSubstance() const {return data(IsActiveSubstance).toBool();}
    void setIsActiveSubstance(bool state) {setDataFromDb(IsActiveSubstance, state);}

    bool isMainInn() const;
    QVector<int> innAtcIds() const;
    QVector<int> interactingClassAtcIds() const;

    QString toXml() const;

protected:
    bool setDataFromDb(const int ref, const QVariant &value, const QString &lang = "xx");
    void set7CharAtcIds(const QVector<int> &atcIds);
    void setInteractingClassAtcIds(const QVector<int> &atcIds);

private:
    Internal::IComponentPrivate *d_component;
};


class DRUGSBASE_EXPORT IVirtualComponent : public IComponent
{
public:
    IVirtualComponent(IVirtualDrug *parent);
    virtual ~IVirtualComponent() {}

    virtual bool isVirtual() const {return true;}

    virtual QVariant data(const int ref, const QString &lang = QString::null) const = 0;
    virtual bool setData(const int ref, const QVariant &value, const QString &lang = "xx") = 0;

    static IVirtualComponent *fromXml(const QString &xml);
    QString toXml() const;
};


class DRUGSBASE_EXPORT DrugRoute
{
    friend class DrugsDB::DrugsBase;
    friend class DrugsDB::Internal::DrugsBasePrivate;

public:
    enum SystemicEffects {
        NoSystemicEffect = 0,
        UnknownSystemicEffect,
        PartialSystemicEffect,
        HasSystemicEffect
    };

    DrugRoute(IDrug *drug);
    DrugRoute(IDrug *drug, const DrugRoute &copy);

    ~DrugRoute();

    IDrug *drug() const;
    QString label(const QString &lang = QString::null) const;
    SystemicEffects systemicEffect() const;
    int routeId() const;

    static SystemicEffects maximumSystemicEffect(const QVector<DrugRoute *> &routes);
    static SystemicEffects maximumSystemicEffect(const IDrug *drug);

protected:
    void setLabel(const QString &label, const QString &lang = QString::null);
    void setSystemicDatabaseTag(const QString &systemic);
    void setRouteId(const int rid);

private:
    Internal::DrugRoutePrivate *d;
};


class DRUGSBASE_EXPORT IPrescription
{
public:
    IPrescription();
    IPrescription(const IPrescription &copy);

    virtual ~IPrescription();

    // Acces to some IDrug values
    virtual QVariant drugId() const = 0;
    virtual QVector<DrugRoute *> drugRoutes() const = 0;


    // Prescription values
    virtual bool hasPrescription() const;
    virtual void setPrescriptionValue(const int fieldref, const QVariant &value);
    virtual QVariant prescriptionValue(const int filedref) const;

private:
    Internal::IPrescriptionPrivate *d_pres;
};

class DRUGSBASE_EXPORT IDrug : public IPrescription
{
    friend class DrugsDB::DrugRoute;
    friend class DrugsDB::IComponent;
    friend class DrugsDB::DrugsBase;
    friend class DrugsDB::DrugsIO;
    friend class DrugsDB::Internal::DrugsIOPrivate;

public:
    enum References {
        DrugID = 0,
        Uid1,
        Uid2,
        Uid3,
        AllUids,
        OldUid,
        UidName,
        SourceID,                   // Source
        SourceName,
        AuthorizationID,            // Authorization
        Name,
        AtcCode,
        AtcId,
        AtcLabel,
        MainInnCode,
        AllInnsKnown,
        //        AllAtcIds,
        All7CharsAtcLabels,
        All7CharsAtcCodes,
        AllAtcCodes,
        AllInteractingClasses,
        //        AllInteractingClassesIds,
        AllMoleculeNames,
        Strength,
        Forms,
        Routes,
        //        RoutesId,
        //        FormsId,
        Authorization,
        Valid,
        Marketed,
        Spc,
        Xml,
        MaxParam
    };

    IDrug();
    IDrug(const IDrug &copy);

    virtual ~IDrug();

    virtual bool isVirtual() const {return false;}
    virtual QVariant data(const int ref, const QString &lang = QString::null) const;
    static inline bool lessThanOnNames(const IDrug *s1, const IDrug *s2) {return s1->data(Name).toString() < s2->data(Name).toString();}

    QVariant drugId() const {return data(DrugID);}
    QStringList uids() const {return data(AllUids).toStringList();}
    QString drugUidName() const {return data(UidName).toString();}
    QString brandName() const {return data(Name).toString();}
    QString atcLabel() const { return data(AtcLabel).toString(); }
    QString strength() const { return data(Strength).toString(); }
    QStringList forms() const { return data(Forms).toStringList(); }
    QStringList routes() const {return data(Routes).toStringList(); }

    int numberOfCodeMolecules() const;
    int numberOfInn() const { return allSevenCharsAtcIds().count(); }

    QStringList listOfMolecules() const {return data(AllMoleculeNames).toStringList();}
    QStringList listOfInnLabels() const {return data(All7CharsAtcLabels).toStringList();}
    QStringList listOfInteractingClasses() const {return data(AllInteractingClasses).toStringList();}

    QStringList allSevenCharsAtcCodes() const {return data(All7CharsAtcLabels).toStringList();}
    QStringList allAtcCodes() const  {return data(AllAtcCodes).toStringList();}

    QString atcCode() const {return data(AtcCode).toString();}

    // PRIVATE
//    QList<QVariant>   CIPs() const;
//    QStringList       CIPsDenominations() const;
    // END PRIVATE

    int mainInnCode() const;
    QString mainInnName() const;
    QString mainInnDosage() const;
    QString innComposition() const;

    bool isScoredTablet() const {return true;}
    QVector<DrugRoute *> drugRoutes() const;
    QString linkToSCP() const {return data(Spc).toString();}

    bool atcIdsContains(const int atcId);

//    QStringList dosageOfMolecules() const;

    QString compositionToXml();
    QString toHtml() const;

    static bool lessThan(const IDrug *drug1, const IDrug *drug2);
    bool equals(const IDrug *d);

    QVector<int> allInnAndInteractingClassesIds() const;
    QVector<int> allSevenCharsAtcIds() const;
    QVector<int> allAtcIds() const;
    QVector<int> molsIds() const;

    QVector<IComponent *> components() const;

protected:
    bool setDataFromDb(const int ref, const QVariant &value, const QString &lang = QString::null);
    void addComponent(IComponent *compo);
    void addRoute(DrugRoute *route);

    void setAll7CharsAtcIds(const QVector<int> &ids);
    void setAllInteractingClassesIds(const QVector<int> &ids);
    void constructAtcIdsVectorsUsingComponents();

private:
    Internal::IDrugPrivate *d_drug;
};

class DRUGSBASE_EXPORT ITextualDrug : public IDrug
{
public:
    ITextualDrug();
    virtual ~ITextualDrug() {}

    void setDenomination(const QString &denomination);

    // Overwrite some prescriptions values
    virtual void setPrescriptionValue(const int fieldref, const QVariant &value);
    virtual QVariant prescriptionValue(const int filedref) const;

    static ITextualDrug *fromXml(const QString &xml);
    QString toXml() const;
};

class DRUGSBASE_EXPORT IVirtualDrug : public IDrug
{
public:
    IVirtualDrug() : IDrug() {}
    virtual ~IVirtualDrug() {}

    virtual bool isVirtual() const {return true;}

    virtual QVariant data(const int ref, const QString &lang = "xx") const = 0;
    virtual bool setData(const int ref, const QVariant &value, const QString &lang = "xx") = 0;

    static IVirtualDrug *fromXml(const QString &xml);
    QString toXml() const;
};

}  // End namespace DrugsDB

DRUGSBASE_EXPORT QDebug operator<<(QDebug dbg, const DrugsDB::IDrug *c);
DRUGSBASE_EXPORT QDebug operator<<(QDebug dbg, const DrugsDB::IDrug &c);
DRUGSBASE_EXPORT QDebug operator<<(QDebug dbg, const DrugsDB::IComponent *c);
DRUGSBASE_EXPORT QDebug operator<<(QDebug dbg, const DrugsDB::IComponent &c);

#endif // DRUGSDB_IDRUG_H
