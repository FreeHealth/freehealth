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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IAMDB__DRUGINTERACTOR_H
#define IAMDB__DRUGINTERACTOR_H

#include <QMultiHash>
#include <QVariant>
#include <QString>
#include <QAbstractItemModel>
#include <QStringList>
#include <QDomElement>

namespace IAMDb {

class DrugInteractor
{
public:
    enum DataRepresentation {
        Id = 0,
        TranslatedLabel,
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
    DrugInteractor(const QDomElement &element);
    ~DrugInteractor();

    QVariant data(const int reference, const QString &lang = QString::null) const;
    bool setData(const int reference, const QVariant &value, const QString &lang = QString::null);

    bool isValid() const {return data(IsValid).toBool();}
    bool isClass() const {return data(IsClass).toBool();}
    bool isReviewed() const {return data(IsReviewed).toBool();}

    QString id() const {return data(Id).toString();}

    void addAtcLink(const QString &atcCode);

    void addParentId(const QString &id) {m_ParentIds<<id;}
    QStringList parentIds() const {return m_ParentIds;}
    void removeParentId(const QString &id) {m_ParentIds.removeAll(id);}

    void setChildId(const QStringList &ids) {m_ChildrenId = ids;}
    void addChildId(const QString &id) {m_ChildrenId<<id;}
    QStringList childrenIds() const {return m_ChildrenId;}
    void removeChildId(const QString &id) {m_ChildrenId.removeAll(id);}

    void addChildClassificationPMID(const QString &childId, const QString &pmid) {m_ChildClassifPMIDs.insertMulti(childId, pmid);}
    void addChildClassificationPMIDs(const QString &childId, const QStringList &pmids);
    QStringList childClassificationPMIDs(const QString &childId) const;

    QStringList allNeededPMIDs() const;

    // Xml members
    QString toXml() const;

    bool operator==(const DrugInteractor &other) const;
    static bool lowerThan(const DrugInteractor &d1, const DrugInteractor &d2);
    static bool lowerThan(DrugInteractor *d1, DrugInteractor *d2) {return lowerThan(*d1, *d2);}

private:
    QHash<int, QHash<QString, QVariant> > m_TrData;
    QStringList m_ParentIds, m_ChildrenId, m_AtcLinks;
    QMultiHash<QString, QString> m_ChildClassifPMIDs;
};

namespace Internal {
class DrugInteractorModelPrivate;
}

class DrugInteractorModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        TrLabel = 0,
        Id,
        Uuid,
        EnLabel,
        FrLabel,
        DeLabel,
        EsLabel,
        IsInteractingClass,
        ClassInformationFr,
        ClassInformationEn,
        ClassInformationDe,
        DoNotWarnDuplicated,
        ATCCodeStringList,
        IsReviewed,
        DateOfCreation,
        DateLastUpdate,
        IsAutoFound,
        Comment,
        PMIDStringList,
        ChildrenUuid,
        Reference,
        ColumnCount
    };

    enum ShowData {
        InteractingClasses,
        InteractingMolecules,
        All
    };

    DrugInteractorModel(ShowData show = All, QObject *parent = 0);
    ~DrugInteractorModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void createInteractingClass(const QString &initialLabel);
    void createInteractor(const QString &initialLabel);

    int numberOfUnreviewed() const;
    int numberOfUnlinked() const;

public Q_SLOTS:
    void onInteractorCreated(DrugInteractor *di);
    bool saveModel();

Q_SIGNALS:
    void unreviewedCountChanged();
    void unlinkedCountChanged();

private:
    Internal::DrugInteractorModelPrivate *d;
};

}  // End namespace IAMDb

#endif // IAMDB__DRUGINTERACTOR_H
