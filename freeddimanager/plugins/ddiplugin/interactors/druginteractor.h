/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DDIPLUGIN_DRUGINTERACTOR_H
#define DDIMANAGER_DDIPLUGIN_DRUGINTERACTOR_H

#include <QMultiHash>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QDomElement>

/**
 * \file druginteractor.h
 * \author Eric Maeker
 * \version 0.10
 * \date 10 Oct 2013
*/

namespace DDI {

class DrugInteractor
{
public:
    enum DataRepresentation {
        Id = 0,
        ReadOnly_TranslatedLabel,
        InitialLabel,
        EnLabel,
        FrLabel,
        DeLabel,
        EsLabel,
        IsValid,
        IsClass,
        ClassInformationFr,
        ClassInformationEn,
        ClassInformationDe,
        DoNotWarnDuplicated,
        Reference,
        PMIDsStringList,
        ATCCodeStringList,
        Comment,
        IsReviewed,
        ReviewersStringList,
        DateOfReview,
        DateOfCreation,
        DateLastUpdate,
        IsAutoFound,
        IsDuplicated
    };

    DrugInteractor();
    ~DrugInteractor();

    QVariant data(const int reference) const;
    bool setData(const int reference, const QVariant &value);

    QString label() const;
    QString id() const;
    bool isValid() const;
    bool isClass() const;
    bool isReviewed() const;
    bool isAutoFound() const;

    void addAtcLink(const QString &atcCode);

    void addParentId(const QString &id) {m_ParentIds<<id;}
    QStringList parentIds() const {return m_ParentIds;}
    void removeParentId(const QString &id) {m_ParentIds.removeAll(id);}

    void setChildId(const QStringList &ids) {m_ChildrenId = ids;}
    void addChildId(const QString &id) {m_ChildrenId<<id;}
    void removeChildId(const QString &id) {m_ChildrenId.removeAll(id);}
    QStringList childrenIds() const {return m_ChildrenId;}
    int childrenCount() const {return m_ChildrenId.count();}

    // TODO: simplify management of related PMID to the interactor only (not to its children)
    void addChildClassificationPMID(const QString &childId, const QString &pmid) {m_ChildClassifPMIDs.insertMulti(childId, pmid);}
    void addChildClassificationPMIDs(const QString &childId, const QStringList &pmids);
    QStringList childClassificationPMIDs(const QString &childId) const;
    QStringList allNeededPMIDs() const;

    // Xml members
    QString toXml() const;
    static QString listToXml(const QList<DrugInteractor> &interactors);
    static DrugInteractor &fromDomElement(const QDomElement &element);
    static QList<DrugInteractor> &listFromXml(const QString &xmlContent);

    bool operator==(const DrugInteractor &other) const;
    static bool lowerThan(const DrugInteractor &d1, const DrugInteractor &d2);
    static bool lowerThan(DrugInteractor *d1, DrugInteractor *d2) {return lowerThan(*d1, *d2);}

private:
    QHash<int, QVariant> _data;
    QStringList m_ParentIds, m_ChildrenId, m_AtcLinks;
    QMultiHash<QString, QString> m_ChildClassifPMIDs;
};

}  // namespace DDI

#endif // DDIMANAGER_DDIPLUGIN_DRUGINTERACTOR_H
