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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGIN_DRUGDRUGINTERACTION_H
#define DDIMANAGER_DDIPLUGIN_DRUGDRUGINTERACTION_H

#include <ddiplugin/ddi_exporter.h>

#include <QVariant>
#include <QHash>
#include <QDomElement>

/**
 * \file drugdruginteraction.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 16 Jan 2014
*/

namespace DDI {
class DDI_EXPORT DrugDrugInteractionDose {
public:
    enum DataRepresentation {
        Uid = 0,
        UsesFrom,
        UsesTo,
        FromValue,
        FromUnits,
        FromRepartition,
        ToValue,
        ToUnits,
        ToRepartition
    };

    DrugDrugInteractionDose(const QVariant &uid = QVariant());
    ~DrugDrugInteractionDose();

    bool isEmpty() const;
    bool isValid() const;

    QVariant data(const int reference) const;
    bool setData(const int reference, const QVariant &value);

private:
    QVariant m_uid;
    QHash<int, QVariant> m_Data;
};

class DDI_EXPORT DrugDrugInteraction
{
public:
    enum DataRepresentation {
        FirstInteractorName,
        SecondInteractorName,
        FirstInteractorRouteOfAdministrationIds,
        SecondInteractorRouteOfAdministrationIds,
        LevelCode,
        LevelName,
        DateCreation,
        DateLastUpdate,
        ListOfUpdates,
        IsDuplicated,
        IsValid,
        IsReviewed,
        ReviewersStringList,
        Comment,
        PMIDsStringList,
        Source,
        InternalUuid
    };

    DrugDrugInteraction();
    DrugDrugInteraction(const DrugDrugInteraction &copy);
    DrugDrugInteraction(const QDomElement &element);
    ~DrugDrugInteraction();

    QVariant data(const int reference) const;
    bool setData(const int reference, const QVariant &value);

    bool levelValidity() const;

    QString firstInteractor() const {return data(FirstInteractorName).toString();}
    QString secondInteractor() const {return data(SecondInteractorName).toString();}
    QString levelCode() const {return data(LevelCode).toString();}
    QString levelName() const {return data(LevelName).toString();}
    static QString levelName(QString levelCode);

    void setRisk(const QString &risk, const QString &lang);
    void setManagement(const QString &management, const QString &lang);
    QString risk(const QString &lang) const;
    QString management(const QString &lang) const;

    const DrugDrugInteractionDose &firstInteractorDose() const {return m_FirstDose;}
    const DrugDrugInteractionDose &secondInteractorDose() const {return m_SecondDose;}

    QIcon levelIcon() const;
    void addReviewer(const QString &name);

    bool isValid() const {return data(IsValid).toBool();}
    bool isReviewed() const {return data(IsReviewed).toBool();}
    bool isDuplicated() const {return data(IsDuplicated).toBool();}

    // Draft
    void addFormalized(const QString &attr, const QString &value);
    // End Draft

    QString toXml() const;
    bool updateDomElement(QDomElement *element, QDomDocument *doc) const;
    static QList<DrugDrugInteraction> &listFromXml(const QString &xml);

    bool operator==(const DrugDrugInteraction &other) const;
    static bool lowerThan(const DrugDrugInteraction &d1, const DrugDrugInteraction &d2);

private:
    QHash<int, QVariant> m_Data;
    QHash<QString,QString> m_Formalized;
    DrugDrugInteractionDose m_FirstDose, m_SecondDose;
};
}  // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_DRUGDRUGINTERACTION_H
