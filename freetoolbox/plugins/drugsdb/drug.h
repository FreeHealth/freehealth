#ifndef DRUG_H
#define DRUG_H

#include <QVariant>

class ComponentPrivate;
class DrugPrivate;

class Component
{
public:
    enum References {
        MID = 0,
        Name,
        Strength,
        StrengthUnit,
        Dose,
        DoseUnit,
        Nature,
        NatureLink,
        AtcIds
    };

    Component();
    ~Component();

    bool setData(const int ref, const QVariant &value, const QString &lang = QString::null);
    QVariant data(const int ref, const QString &lang = QString::null) const;

private:
    ComponentPrivate *d;
};

class Drug
{
public:
    enum References {
        Uid1 = 0,
        Uid2,
        Uid3,
        OldUid,
        DID,
        SID,            // Source
        AID,            // Authorization
        Name,
        AtcCode,
        AtcId,
        Strength,
        Forms,
        Routes,
        FormsId,
        RoutesId,
        Authorization,
        Valid,
        Marketed,
        Spc,
        Xml
    };

    Drug();
    ~Drug();

    bool setData(const int ref, const QVariant &value, const QString &lang = "xx");
    QVariant data(const int ref, const QString &lang = "xx") const;

    void addComponent(Component *compo);
    QVector<Component *> components() const;

    bool toDatabase(const QString &dbConnection,const QHash<int, QString> &mids,const QString &lang = "xx");

    static bool lessThanOnNames(const Drug *s1, const Drug *s2);

    static bool saveDrugsIntoDatabase(const QString &connection, const QVector<Drug *> &drugs, const QString &dbUid);

private:
    DrugPrivate *d;
};

#endif // DRUG_H
