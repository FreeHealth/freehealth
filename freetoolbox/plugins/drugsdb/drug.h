/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUG_H
#define DRUG_H

#include <QVariant>

namespace DrugsDB {
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
        Strength,     // 10
        Forms,
        Routes,       // 12
        FormsId,
        RoutesId,     // 14
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

    static bool saveDrugsIntoDatabase(const QString &connection, QVector<Drug *> drugs, const QString &dbUid);

private:
    DrugPrivate *d;
};

}  // namespace DrugsDB

QDebug operator<<(QDebug dbg, const DrugsDB::Drug *d);


#endif // DRUG_H
