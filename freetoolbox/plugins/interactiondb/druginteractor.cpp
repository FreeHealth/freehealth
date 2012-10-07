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
#include "druginteractor.h"
#include "drugdruginteractioncore.h"
#include "drugdruginteraction.h"

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QFont>
#include <QLocale>
#include <QDate>
#include <QColor>

#include <QDebug>

using namespace IAMDb;

static inline IAMDb::DrugDrugInteractionCore *core() {return IAMDb::DrugDrugInteractionCore::instance();}

DrugInteractor::DrugInteractor()
{
    setData(IsValid, false);
    setData(IsDuplicated, false);
    setData(DoNotWarnDuplicated, false);
}

DrugInteractor::DrugInteractor(const QDomElement &element)
{
    setData(IsDuplicated, false);
    if (element.tagName()!="I") {
        setData(IsValid, false);
        return;
    }
    // get attribs
    setData(InitialLabel, element.attribute("id"));
    setData(IsValid, element.attribute("v").toInt());
    bool isClass = element.attribute("c").toInt();
    setData(IsClass, isClass);
    setData(DoNotWarnDuplicated, element.attribute("nwd").toInt());
    setData(Reference, element.attribute("r"));
    setData(PMIDsStringList, element.attribute("p").split(";"));
    setData(DateOfCreation, QDate::fromString(element.attribute("dc"), Qt::ISODate));
    setData(DateLastUpdate, QDate::fromString(element.attribute("lu"), Qt::ISODate));

    // get class information
    if (isClass) {
        QDomElement ci = element.firstChildElement("CI");
        while (!ci.isNull()) {
            const QString &l = ci.attribute("l");
            if (l == "fr")
                setData(ClassInformationFr, ci.attribute("t"));
            else if (l == "en")
                setData(ClassInformationEn, ci.attribute("t"));
            else if (l == "de")
                setData(ClassInformationDe, ci.attribute("t"));
//            else if (l == "es")
//                setData(ClassInformationEs, ci.attribute("t"));
            ci = ci.nextSiblingElement("CI");
        }
    }

    // get Labels
    QDomElement label = element.firstChildElement("L");
    while (!label.isNull()) {
        const QString &l = label.attribute("l");
        if (l == "fr")
            setData(FrLabel, label.attribute("n"));
        else if (l == "en")
            setData(EnLabel, label.attribute("n"));
        else if (l == "de")
            setData(DeLabel, label.attribute("n"));
        else if (l == "es")
            setData(EsLabel, label.attribute("n"));
        label = label.nextSiblingElement("L");
    }

    // get Review info
    QDomElement rev = element.firstChildElement("O");
    setData(Comment, rev.attribute("c"));
    setData(IsReviewed, rev.attribute("r"));
    setData(ReviewersStringList, rev.attribute("rv").split(";"));
    setData(DateOfReview, QDate::fromString(element.attribute("d"), Qt::ISODate));
    setData(IsAutoFound, rev.attribute("af"));

    // get children item
    QDomElement child = element.firstChildElement("M");
    while (!child.isNull()) {
        addChildId(child.attribute("n"));
        if (!child.attribute("p").isEmpty()) {
            addChildClassificationPMIDs(child.attribute("n"), child.attribute("p").split(";"));
        }
        child = child.nextSiblingElement("M");
    }

    // get ATC codes
    child = element.firstChildElement("ATC");
    while (!child.isNull()) {
        m_AtcLinks.append(child.attribute("a").split(";"));
        child = child.nextSiblingElement("ATC");
    }

}

DrugInteractor::~DrugInteractor()
{}

QVariant DrugInteractor::data(const int reference, const QString &lang) const
{
    QString l = lang;
    if (lang.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    if (reference == TranslatedLabel) {
        QString loc = QLocale().name().left(2).toLower();
        if (loc=="en")
            return m_TrData.value(EnLabel).value(l);
        else if (loc=="fr")
            return m_TrData.value(FrLabel).value(l);
        else if (loc=="de")
            return m_TrData.value(DeLabel).value(l);
        else if (loc=="es")
            return m_TrData.value(EsLabel).value(l);
    } else if (reference==ATCCodeStringList) {
        return m_AtcLinks;
    }
    return m_TrData.value(reference).value(l);
}

bool DrugInteractor::setData(const int reference, const QVariant &value, const QString &lang)
{
    QString l = lang;
    if (lang.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    if (reference==ATCCodeStringList) {
        m_AtcLinks = value.toStringList();
        return true;
    }
    QHash<QString, QVariant> &data = m_TrData[reference];
    data.insert(l, value);
    return true;
}

void DrugInteractor::addAtcLink(const QString &atcCode)
{
    if (m_AtcLinks.contains(atcCode))
        return;
    m_AtcLinks.append(atcCode);
    m_AtcLinks.sort();
}

void DrugInteractor::addChildClassificationPMIDs(const QString &childId, const QStringList &pmids)
{
    foreach(const QString &pmid, pmids)
        m_ChildClassifPMIDs.insertMulti(childId, pmid);
}

QStringList DrugInteractor::childClassificationPMIDs(const QString &childId) const
{
    return m_ChildClassifPMIDs.values(childId);
}


QStringList DrugInteractor::allNeededPMIDs() const
{
    return m_ChildClassifPMIDs.values();
}

QString DrugInteractor::toXml() const
{
    QString xml = QString("\n  <I id=\"%1\" v=\"%2\" c=\"%3\" r=\"%4\" p=\"%5\" dc=\"%6\" lu=\"%7\" nwd=\"%8\">")
            .arg(data(InitialLabel).toString())
            .arg(data(IsValid).toBool())
            .arg(data(IsClass).toBool())
            .arg(data(Reference).toString())
            .arg(data(PMIDsStringList).toStringList().join(";"))
            .arg(data(DateOfCreation).toDate().toString(Qt::ISODate))
            .arg(data(DateLastUpdate).toDate().toString(Qt::ISODate))
            .arg(data(DoNotWarnDuplicated).toInt())
            ;

    // add class information
    if (isClass()) {
        if (!data(ClassInformationFr).isNull()) {
            xml += QString("\n    <CI l=\"fr\" t=\"%1\"/>").arg(data(ClassInformationFr).toString());
        }
        if (!data(ClassInformationEn).isNull()) {
            xml += QString("\n    <CI l=\"en\" t=\"%1\"/>").arg(data(ClassInformationEn).toString());
        }
        if (!data(ClassInformationDe).isNull()) {
            xml += QString("\n    <CI l=\"de\" t=\"%1\"/>").arg(data(ClassInformationDe).toString());
        }
    }

    // Add labels
    QHash<int, QString> langsForLabels;
    langsForLabels.insert(FrLabel, "fr");
    langsForLabels.insert(EnLabel, "en");
    langsForLabels.insert(DeLabel, "de");
    langsForLabels.insert(EsLabel, "es");
    foreach(const QString &lang, langsForLabels) {
        xml += QString("\n    <L l=\"%1\" n=\"%2\"/>")
                .arg(lang).arg(data(langsForLabels.key(lang)).toString())
                ;
    }

    // Add ATC Codes
    const QStringList &atc = data(ATCCodeStringList).toStringList();
    if (atc.count() > 0) {
        if (!atc.at(0).isEmpty()) {
                // TODO: add to each linked ATC codes: comment, reviewState, reviewers */
//                xml += QString("\n    <ATC a=\"%1\" c=\"%2\" rv=\"\" r=\"\">")
//                        .arg(code).arg("");
            xml += QString("\n    <ATC a=\"%1\"/>")
                    .arg(atc.join(";"));
        }
    }

    // Add children
    QStringList children = childrenIds();
    children.removeDuplicates();
    foreach(const QString &c, children) {
        QStringList pmids = m_ChildClassifPMIDs.values(c);
        pmids.removeAll("");
        pmids.removeDuplicates();
        if (pmids.count()) {
            xml += QString("\n    <M n=\"%1\" p=\"%2\"/>").arg(c).arg(pmids.join(";"));
        } else {
            xml += QString("\n    <M n=\"%1\"/>").arg(c);
        }
    }

    // Add Review info
    xml += QString("\n    <O c=\"%1\" r=\"%2\" rv=\"%3\" d=\"%4\" af=\"%5\"/>")
            .arg(data(Comment).toString())
            .arg(data(IsReviewed).toInt())
            .arg(data(ReviewersStringList).toStringList().join(";"))
            .arg(data(DateOfReview).toStringList().join(";"))
            .arg(data(IsAutoFound).toInt())
            ;

    xml += "\n  </I>";
    return xml;
}

bool DrugInteractor::operator==(const DrugInteractor &other) const
{
    return (other.data(Id).toString()==m_TrData.value(Id).value(Trans::Constants::ALL_LANGUAGE).toString());
}

bool DrugInteractor::lowerThan(const DrugInteractor &d1, const DrugInteractor &d2)
{
    bool d1IsClass, d2IsClass;
    d1IsClass = d1.isClass();
    d2IsClass = d2.isClass();
    if (d1IsClass && !d2IsClass)
        return true;
    if (d2IsClass && !d1IsClass)
        return false;
    return d1.data(DrugInteractor::TranslatedLabel).toString() < d2.data(DrugInteractor::TranslatedLabel).toString();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
namespace {
class DITreeItem
{
public:
    DITreeItem(DrugInteractor *di, DITreeItem *parent = 0) :
        m_di(di), parentItem(parent)
    {
        if (parent)
            parent->addChild(this);
    }

    ~DITreeItem()
    {
        qDeleteAll(childItems);
    }

    DrugInteractor *di() {return m_di;}

    void setText(const QString &text) {m_text = text;}
    QString text() const {return m_text;}

    DITreeItem *child(int number)
    {
        return childItems.value(number);
    }
    void addChild(DITreeItem *child) {childItems.append(child);}

    int childCount() const {return childItems.count();}
    DITreeItem *parent() {return parentItem;}
    int childNumber() const
    {
        if (parentItem)
            return parentItem->childItems.indexOf(const_cast<DITreeItem*>(this));
        return 0;
    }

    void sortChildren()
    {
        qSort(childItems.begin(), childItems.end(), DITreeItem::lessThan);
        for(int i=0; i < childItems.count(); ++i) {
            childItems.at(i)->sortChildren();
        }
    }

    void clearChildren()
    {
        childItems.clear();
    }

    void removeChild(int childNumber)
    {
        childItems.removeAt(childNumber);
    }

    void removeChild(DITreeItem *item)
    {
        childItems.removeAll(item);
    }

    void insertChild(int pos, DITreeItem *item)
    {
        childItems.insert(pos, item);
    }

    static bool lessThan(DITreeItem *it1, DITreeItem *it2)
    {
        return DrugInteractor::lowerThan(*it1->di(), *it2->di());
    }

private:
    DrugInteractor *m_di;
    QList<DITreeItem*> childItems;
    DITreeItem *parentItem;
    QString m_text;
};

}

namespace IAMDb {
namespace Internal {

class DrugInteractorModelPrivate
{
public:
    DrugInteractorModelPrivate(DrugInteractorModel *parent) :
            q(parent)
    {
        rootItem = new DITreeItem(0);
    }

    ~DrugInteractorModelPrivate()
    {
        delete rootItem;
    }

    DITreeItem *getItem(const QModelIndex &index) const
     {
         if (index.isValid()) {
             DITreeItem *item = static_cast<DITreeItem*>(index.internalPointer());
             if (item)
                 return item;
         }
         return rootItem;
     }

    void filter(const QString &interactor = QString::null)
    {
        delete rootItem;
        rootItem = new DITreeItem(0,0);
        QFont bold;
        bold.setBold(true);
        QHash<QString, DITreeItem *> categories;
        if (interactor.isEmpty()) {

            // first pass == create categories
            if (m_ShowData == DrugInteractorModel::InteractingClasses ||
                    m_ShowData == DrugInteractorModel::All) {
                for(int i=0; i < m_interactors.count(); ++i) {
                    DrugInteractor *di = m_interactors.at(i);
                    if (!di->isValid())
                        continue;
                    // if is class -> create a category
                    if (di->isClass()) {
                        DITreeItem *cat = 0;
                        if (!categories.keys().contains(di->data(DrugInteractor::InitialLabel).toString())) {
                            cat = new DITreeItem(di, rootItem);
                            categories.insert(di->data(DrugInteractor::InitialLabel).toString(), cat);
                        }
                    }
                }
            }

            // second pass = add interactors to categories or rootItem
            for(int i=0; i < m_interactors.count(); ++i) {
                DrugInteractor *di = m_interactors.at(i);
                if (!di->isValid())
                    continue;
                // add molecule item to root
                if (!di->isClass() &&
                    (m_ShowData == DrugInteractorModel::All || m_ShowData==DrugInteractorModel::InteractingMolecules)) {
                    new DITreeItem(di, rootItem);
                }

                // find parent class
                DITreeItem *cat = 0;
                foreach(const QString &id, di->parentIds()) {
                    cat = categories.value(id, 0);
                    if (!cat)
                        continue;
                    new DITreeItem(di, cat);
                }
            }
        }
        rootItem->sortChildren();
    }

    QString getTooltip(DITreeItem *item)
    {
        QStringList msg;
        DrugInteractor *di = item->di();
        msg << QString("<b><u>UID:</b></u><br />&nbsp;&nbsp;%1").arg(di->data(DrugInteractor::InitialLabel).toString());

        // check errors
        QStringList errors;
        if (!di->isClass() && di->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty()) {
            errors << QString("Missing ATC link");
        }
        if (di->isClass()) {
            for(int i=0; i < item->childCount();++i) {
                if (item->child(i)->di()->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty()) {
                    errors << QString("One child at least is missing ATC link");
                }
            }
        }
        if (!di->isReviewed()) {
            errors << QString("Item is not reviewed");
        } else if (di->isClass()) {
            for(int i=0; i < item->childCount();++i) {
                if (!item->child(i)->di()->isReviewed())
                    errors << QString("One child at least is not reviewed");
            }
        }
        if (errors.count())
            msg << QString("<span style=\"color:#FF2020\"><b><u>Errors:</b></u><br />&nbsp;&nbsp;* %1</span>").arg(errors.join("<br />&nbsp;&nbsp;* "));


        // get related DDI
        QStringList directDdis;
        QMultiHash<QString, QString> parentDdis;  // key=parent value=second interactor
        for(int i=0; i < m_ddis.count(); ++i) {
            // direct ddis
            DrugDrugInteraction *ddi = m_ddis.at(i);
            const QString &id = di->data(DrugInteractor::InitialLabel).toString();
            if (ddi->data(DrugDrugInteraction::FirstInteractorName).toString()==id) {
                directDdis << QString("&nbsp;&nbsp;* %1 (%2)")
                       .arg(ddi->data(DrugDrugInteraction::SecondInteractorName).toString())
                       .arg(ddi->data(DrugDrugInteraction::LevelName).toString());
            } else if (ddi->data(DrugDrugInteraction::SecondInteractorName).toString()==id) {
                directDdis << QString("&nbsp;&nbsp;* %1 (%2)")
                        .arg(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())
                        .arg(ddi->data(DrugDrugInteraction::LevelName).toString());
            }
            // parent ddis
            if (di->parentIds().contains(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())) {
                parentDdis.insertMulti(ddi->data(DrugDrugInteraction::FirstInteractorName).toString(),
                                       QString("%1 (%2)")
                                       .arg(ddi->data(DrugDrugInteraction::SecondInteractorName).toString())
                                       .arg(ddi->data(DrugDrugInteraction::LevelName).toString()));
            } else if (di->parentIds().contains(ddi->data(DrugDrugInteraction::SecondInteractorName).toString())) {
                parentDdis.insertMulti(ddi->data(DrugDrugInteraction::SecondInteractorName).toString(),
                                       QString("%1 (%2)")
                                       .arg(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())
                                       .arg(ddi->data(DrugDrugInteraction::LevelName).toString()));
            }
        }
        QString ddisText;
        if (directDdis.count()) {
            ddisText = QString("<span style=\"color:#AA10AA\"><b><u>DDI (own):</u></b><br />%1").arg(directDdis.join("<br />"));
        } // <br /><span style=\"color:#FF2020\"><b><u>DDI:</b></u><br />%1</span>
        if (parentDdis.count()) {
            ddisText = QString("<span style=\"color:#AA10AA\"><b><u>DDI (from parents):</u></b><br />");
            foreach(const QString &parent, parentDdis.uniqueKeys()) {
                const QStringList &l = parentDdis.values(parent);
                ddisText += QString("&nbsp;&nbsp;<b>* %1</b><br/>&nbsp;&nbsp;&nbsp;&nbsp;* %2<br/>").arg(parent).arg(l.join("<br/>&nbsp;&nbsp;&nbsp;&nbsp;* "));
            }
        }
        if (!ddisText.isEmpty()) {
            ddisText += "</span>";
            msg << ddisText;
        }

        // TODO: get pims */

        return msg.join("<br />");
    }

public:
    DITreeItem *rootItem;
    QList<DrugInteractor *> m_interactors;
    QList<DrugDrugInteraction *> m_ddis;
    QString reviewer;
    DrugInteractorModel::ShowData m_ShowData;

private:
    DrugInteractorModel *q;
};
}  // End namespace Internal
}  // End namespace IAMDb


DrugInteractorModel::DrugInteractorModel(ShowData show, QObject *parent) :
    QAbstractItemModel(parent),
    d(new Internal::DrugInteractorModelPrivate(this))
{
    setObjectName("DrugInteractorModel");
    d->m_interactors = core()->getDrugInteractors();
    d->m_ddis = core()->getDrugDrugInteractions();
    d->m_ShowData = show;
    d->filter();
    connect(core(), SIGNAL(interactorCreated(DrugInteractor*)), this, SLOT(onInteractorCreated(DrugInteractor*)));
}

DrugInteractorModel::~DrugInteractorModel()
{
    if (d)
        delete d;
    d = 0;
}

int DrugInteractorModel::rowCount(const QModelIndex &parent) const
{
    DITreeItem *parentItem = d->getItem(parent);
    return parentItem->childCount();
}

int DrugInteractorModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QModelIndex DrugInteractorModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    DITreeItem *parentItem = d->getItem(parent);
    DITreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex DrugInteractorModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    DITreeItem *childItem = d->getItem(index);
    DITreeItem *parentItem = childItem->parent();

    if (!parentItem)
        return QModelIndex();

    if (parentItem == d->rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

QVariant DrugInteractorModel::data(const QModelIndex &index, int role) const
{
    DITreeItem *item = d->getItem(index);
    if (!item) {
        return QVariant();
    }
    DrugInteractor *di = item->di();
    if (!di) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case Uuid: return di->data(DrugInteractor::InitialLabel);
        case TrLabel : return di->data(DrugInteractor::TranslatedLabel);
        case EnLabel : return di->data(DrugInteractor::EnLabel);
        case FrLabel : return di->data(DrugInteractor::FrLabel);
        case DeLabel : return di->data(DrugInteractor::DeLabel);
        case EsLabel : return di->data(DrugInteractor::EsLabel);
        case IsInteractingClass : return di->data(DrugInteractor::IsClass);
        case DoNotWarnDuplicated: return di->data(DrugInteractor::DoNotWarnDuplicated);
        case ClassInformationFr : return di->data(DrugInteractor::ClassInformationFr).toString();
        case ClassInformationEn : return di->data(DrugInteractor::ClassInformationEn).toString();
        case ClassInformationDe : return di->data(DrugInteractor::ClassInformationDe).toString();
        case ATCCodeStringList : return di->data(DrugInteractor::ATCCodeStringList);
        case IsReviewed : return di->data(DrugInteractor::IsReviewed);
        case DateOfCreation : return di->data(DrugInteractor::DateOfCreation);
        case DateLastUpdate : return di->data(DrugInteractor::DateLastUpdate);
        case IsAutoFound : return di->data(DrugInteractor::IsAutoFound);
        case Comment : return di->data(DrugInteractor::Comment);
        case PMIDStringList : return di->data(DrugInteractor::PMIDsStringList);
        case ChildrenUuid: return di->childrenIds();
        case Reference: return di->data(DrugInteractor::Reference);
        } // End switch
    } else if (role == Qt::ToolTipRole) {
        return d->getTooltip(item);
    } else if (role==Qt::ForegroundRole) {        
        if (!di->isClass() && di->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty()) {
            return QColor(255,50,50,150);
        }
        if (di->isClass()) {
            for(int i=0; i < item->childCount();++i) {
                if (item->child(i)->di()->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty()) {
                    return QColor(255,50,50,150);
                }
            }
        }
        if (!di->isReviewed()) {
            return QColor(50,250,50,150);
        } else if (di->isClass()) {
            for(int i=0; i < item->childCount();++i) {
                if (!item->child(i)->di()->isReviewed())
                    return QColor(50,255,50,150);
            }
        }
    } else if (role == Qt::DecorationRole) {
//        if (item->childCount() == 0)
//            return ddi->levelIcon();
    } else if (role == Qt::FontRole) {
        if (di->isClass()) {
            QFont bold;
            bold.setBold(true);
            return bold;
        }
    }
    return QVariant();
}

bool DrugInteractorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    DITreeItem *item = d->getItem(index);
    if (!item) {
        return false;
    }

    DrugInteractor *di = item->di();
    if (!di || index.column()==0) {
        return false;
    }

    if (role==Qt::EditRole) {
        switch (index.column()) {
        case EnLabel :
        {
            di->setData(DrugInteractor::EnLabel, value);
            QModelIndex trLab = this->index(index.row(), TrLabel, index.parent());
            Q_EMIT dataChanged(trLab, trLab);
            break;
        }
        case FrLabel :
        {
            di->setData(DrugInteractor::FrLabel, value);
            QModelIndex trLab = this->index(index.row(), TrLabel, index.parent());
            Q_EMIT dataChanged(trLab, trLab);
            break;
        }
        case DeLabel :
        {
            di->setData(DrugInteractor::DeLabel, value);
            QModelIndex trLab = this->index(index.row(), TrLabel, index.parent());
            Q_EMIT dataChanged(trLab, trLab);
            break;
        }
        case EsLabel :
        {
            di->setData(DrugInteractor::EsLabel, value);
            QModelIndex trLab = this->index(index.row(), TrLabel, index.parent());
            Q_EMIT dataChanged(trLab, trLab);
            break;
        }
        case IsInteractingClass : di->setData(DrugInteractor::IsClass, value); break;
        case DoNotWarnDuplicated: di->setData(DrugInteractor::DoNotWarnDuplicated, value); break;
        case ClassInformationFr : di->setData(DrugInteractor::ClassInformationFr, value.toString().replace("\"", "'")); break;
        case ClassInformationEn : di->setData(DrugInteractor::ClassInformationEn, value.toString().replace("\"", "'")); break;
        case ClassInformationDe : di->setData(DrugInteractor::ClassInformationDe, value.toString().replace("\"", "'")); break;
        case ATCCodeStringList :
            di->setData(DrugInteractor::ATCCodeStringList, value);
            Q_EMIT unlinkedCountChanged();
            break;
        case IsReviewed :
            di->setData(DrugInteractor::IsReviewed, value);
            Q_EMIT unreviewedCountChanged();
            break;
        case DateOfCreation : di->setData(DrugInteractor::DateOfCreation, value); break;
        case DateLastUpdate : di->setData(DrugInteractor::DateLastUpdate, value); break;
        case IsAutoFound : di->setData(DrugInteractor::IsAutoFound, value); break;
        case Comment : di->setData(DrugInteractor::Comment, value); break;
        case PMIDStringList : di->setData(DrugInteractor::PMIDsStringList, value); break;
        case ChildrenUuid:
        {
            QStringList list = value.toStringList();
            list.sort();
            di->setChildId(list);
            // TODO: update DI parent */
            beginRemoveRows(index, 0, rowCount(index));
            item->clearChildren();
            endRemoveRows();
            beginInsertRows(index, 0, list.count());
            foreach(const QString &childId, list) {
                // find the related interactor
                for(int i = 0; i < d->m_interactors.count(); ++i) {
                    DrugInteractor *__di = d->m_interactors.at(i);
                    if (__di->data(DrugInteractor::InitialLabel).toString()==childId.toUpper()) {
                        new DITreeItem(__di, item);
                        if (!__di->parentIds().contains(childId, Qt::CaseInsensitive)) {
                            __di->addParentId(di->data(DrugInteractor::InitialLabel).toString());
                        }
                        break;
                    }
                }
            }
            endInsertRows();
            break;
        }
        case Reference: di->setData(DrugInteractor::Reference, value); break;

        default:
            return false;
        }
        // DateLastUpdate -> automatic
        di->setData(DrugInteractor::DateLastUpdate, QDate::currentDate());
        QModelIndex date = this->index(index.row(), DateLastUpdate, index.parent());
        QModelIndex trLabel = this->index(index.row(), TrLabel, index.parent());
        Q_EMIT dataChanged(index, index);
        Q_EMIT dataChanged(date, date);
        Q_EMIT dataChanged(trLabel, trLabel);
        if (item->parent()!=d->rootItem) {
            trLabel = this->index(index.parent().row(), TrLabel, index.parent().parent());
            Q_EMIT dataChanged(trLabel, trLabel);
        }
        return true;
    }

    return false;
}

bool DrugInteractorModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // insert row to categories only
    if (!parent.isValid())
        return false;
    DITreeItem *parentItem = d->getItem(parent);
    if (parentItem->childCount()==0)  // parent is an interactor not a category
        return false;
    beginInsertRows(parent, row, row+count);
    for(int i = 0; i < count; ++i) {
        DrugInteractor *di = new DrugInteractor;
        d->m_interactors.append(di);
        di->setData(DrugInteractor::Id, core()->createInternalUuid());
        DITreeItem *item = new DITreeItem(di, parentItem);
        parentItem->removeChild(item);
        parentItem->insertChild(row+i, item);
    }
    endInsertRows();
    return true;
}

// todo remove mirrored
bool DrugInteractorModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // only remove rows with a category parent
    if (!parent.isValid())
        return false;
    beginRemoveRows(parent, row, row+count);
    DITreeItem *parentItem = d->getItem(parent);
    for(int i = 0; i < count; ++i) {
//        const QString &interactor = parentItem->child(row+i)->ddi()->data(DrugDrugInteraction::IsValid, false);
        parentItem->child(row+i)->di()->setData(DrugInteractor::IsValid, false);
        parentItem->removeChild(row+i);
        // remove mirrored
    }
    endRemoveRows();
    return true;
}

Qt::ItemFlags DrugInteractorModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

//    if (index.column() == Review)
//        f |= Qt::ItemIsUserCheckable;

    f |= Qt::ItemIsEditable;

    return f;
}

void DrugInteractorModel::createInteractingClass(const QString &initialLabel)
{
    for(int i=0; i < d->m_interactors.count(); ++i) {
        if (d->m_interactors.at(i)->data(DrugInteractor::InitialLabel).toString() == initialLabel.toUpper()) {
            return;
        }
    }
    core()->createNewInteractor(initialLabel, true);
}

void DrugInteractorModel::createInteractor(const QString &initialLabel)
{
    for(int i=0; i < d->m_interactors.count(); ++i) {
        if (d->m_interactors.at(i)->data(DrugInteractor::InitialLabel).toString() == initialLabel.toUpper()) {
            return;
        }
    }
    core()->createNewInteractor(initialLabel, false);
}

void DrugInteractorModel::onInteractorCreated(DrugInteractor *di)
{
    d->m_interactors << di;
    d->filter();
    reset();
}

int DrugInteractorModel::numberOfUnreviewed() const
{
    int result = 0;
    for(int i=0; i < d->m_interactors.count();++i) {
        if (d->m_interactors.at(i)->isValid() && !d->m_interactors.at(i)->isReviewed())
            ++result;
    }
    return result;
}

int DrugInteractorModel::numberOfUnlinked() const
{
    int result = 0;
    for(int i=0; i < d->m_interactors.count();++i) {
        if (d->m_interactors.at(i)->isValid() && d->m_interactors.at(i)->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty())
            ++result;
    }
    return result;
}

// TODO: take care to NEWLY CREATED INTERACTORS ??? WILL NOT BE SAVED !!! */
bool DrugInteractorModel::saveModel()
{
    core()->saveCompleteList(d->m_interactors);
    return true;
}
