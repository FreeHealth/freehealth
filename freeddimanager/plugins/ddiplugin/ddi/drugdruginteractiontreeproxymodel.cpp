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
#include "drugdruginteractionmodel.h"
#include "drugdruginteraction.h"
#include <ddiplugin/constants.h>
#include <ddiplugin/ddicore.h>

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpdownloader.h>
#include <translationutils/constants.h>
#include <translationutils/googletranslator.h>

#include <QHash>
#include <QList>

#include "math.h"

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}

namespace {
const int FETCH_LIMIT = 50;

class DDITreeItem
{
public:
    DDITreeItem(DrugDrugInteraction *ddi, DDITreeItem *parent = 0) :
        m_ddi(ddi), parentItem(parent)
    {
        if (parent)
            parent->addChild(this);
    }

    ~DDITreeItem()
    {
        qDeleteAll(childItems);
    }

    DrugDrugInteraction *ddi() {return m_ddi;}

    void setText(const QString &text) {m_text = text;}
    QString text() const {return m_text;}

    DDITreeItem *child(int number)
    {
        return childItems.value(number);
    }
    void addChild(DDITreeItem *child) {childItems.append(child);}

    int childCount() const {return childItems.count();}
    DDITreeItem *parent() {return parentItem;}
    int childNumber() const
    {
        if (parentItem)
            return parentItem->childItems.indexOf(const_cast<DDITreeItem*>(this));
        return 0;
    }

    void sortChildren()
    {
        qSort(childItems.begin(), childItems.end(), DDITreeItem::lessThan);
        for(int i=0; i < childItems.count(); ++i) {
            childItems.at(i)->sortChildren();
        }
    }

    void removeChild(int childNumber)
    {
        childItems.removeAt(childNumber);
    }

    void removeChild(DDITreeItem *item)
    {
        childItems.removeAll(item);
    }

    void insertChild(int pos, DDITreeItem *item)
    {
        childItems.insert(pos, item);
    }

    static bool lessThan(DDITreeItem *it1, DDITreeItem *it2)
    {
        return (it1->text() < it2->text());
    }

private:
    DrugDrugInteraction *m_ddi;
    QList<DDITreeItem*> childItems;
    DDITreeItem *parentItem;
    QString m_text;
};

}

namespace DDI {
namespace Internal {

class DrugDrugInteractionModelPrivate
{
public:
    DrugDrugInteractionModelPrivate(DrugDrugInteractionModel *parent) :
        rootItem(0),
        m_FetchedRows(0),
        q(parent)
    {
        rootItem = new DDITreeItem(0);
        m_currentFilter = "--@@##--";
    }

    ~DrugDrugInteractionModelPrivate()
    {
        delete rootItem;
    }

    DDITreeItem *getItem(const QModelIndex &index) const
     {
         if (index.isValid()) {
             DDITreeItem *item = static_cast<DDITreeItem*>(index.internalPointer());
             if (item)
                 return item;
         }
         return rootItem;
     }

    void createTree(const QList<DrugDrugInteraction *> &ddis, const QString &filterInteractor = QString::null, bool createMirroredDDI = true)
    {
        QHash<QString, DDITreeItem *> categories;
        // first pass == create categories
        for(int i=0; i < ddis.count(); ++i) {
            DrugDrugInteraction *ddi = ddis.at(i);
            if (!ddi->data(DrugDrugInteraction::IsValid).toBool())
                continue;
            const QString &firstInteractor = ddi->firstInteractor();
            const QString &secondInteractor = ddi->secondInteractor();
            DDITreeItem *cat = 0;
            if (filterInteractor.isEmpty()) {
                if (!categories.keys().contains(firstInteractor)) {
                    cat = new DDITreeItem(0, rootItem);
                    cat->setText(firstInteractor);
                    categories.insert(firstInteractor, cat);
                }
                if (!categories.keys().contains(secondInteractor)) {
                    cat = new DDITreeItem(0, rootItem);
                    cat->setText(secondInteractor);
                    categories.insert(secondInteractor, cat);
                }
            } else {
                if (firstInteractor.startsWith(filterInteractor, Qt::CaseInsensitive) &&
                        !categories.keys().contains(firstInteractor)) {
                    cat = new DDITreeItem(0, rootItem);
                    cat->setText(firstInteractor);
                    categories.insert(firstInteractor, cat);
                }
                if (secondInteractor.startsWith(filterInteractor, Qt::CaseInsensitive) &&
                        !categories.keys().contains(secondInteractor)) {
                    cat = new DDITreeItem(0, rootItem);
                    cat->setText(secondInteractor);
                    categories.insert(secondInteractor, cat);
                }
            }
        }
        // second pass = add interactors to categories
        for(int i=0; i < ddis.count(); ++i) {
            DrugDrugInteraction *ddi = ddis.at(i);
            if (!ddi->isValid())
                continue;
            const QString &firstInteractor = ddi->firstInteractor();
            const QString &secondInteractor = ddi->secondInteractor();
            // create DDI
            DDITreeItem *cat = 0;
            if (createMirroredDDI) {
                cat = categories.value(firstInteractor, 0);
                DDITreeItem *it = new DDITreeItem(ddi, cat);
                it->setText(secondInteractor);
                // create mirrored DDI
                cat = categories.value(secondInteractor);
                it = new DDITreeItem(ddi, cat);
                it->setText(firstInteractor);
            } else {
                cat = categories.value(firstInteractor, 0);
                if (!cat) {
                    cat = categories.value(secondInteractor, 0);
                    DDITreeItem *it = new DDITreeItem(ddi, cat);
                    it->setText(firstInteractor);
                } else {
                    DDITreeItem *it = new DDITreeItem(ddi, cat);
                    it->setText(secondInteractor);
                }
            }
        }
    }

//    DrugInteractor *getInteractor(const QString &uuid, const QList<DrugInteractor *> &interactors)
//    {
//        foreach(DrugInteractor *interactor, interactors) {
//            if (interactor->data(DrugInteractor::Uid).toString()==uuid)
//                return interactor;
//        }
//        return 0;
//    }

    // When all DDI are loaded, read all DDI level and store the stat into the
    // m_levelStatistics (K=level name  ; V=number of ddi recorded)
//    void globalLevelStatistics(const QList<DrugDrugInteraction *> &ddis)
//    {
//        int total = 0;
//        for(int i=0; i < ddis.count(); ++i) {
//            DrugDrugInteraction *ddi = ddis.at(i);
//            int n = 0;
//            DrugInteractor *interactor1 = getInteractor(ddi->firstInteractor(), m_interactors);
//            DrugInteractor *interactor2 = getInteractor(ddi->secondInteractor(), m_interactors);
//            if (!interactor1 || !interactor2) {
//                LOG_ERROR_FOR(q, "Interactor not found? " + ddi->firstInteractor() + " - " + ddi->secondInteractor());
//                continue;
//            }
//            if (interactor1->isClass() && interactor2->isClass()) {
//                n = interactor1->childrenCount() * interactor2->childrenCount();
//            } else if (interactor1->isClass() && !interactor2->isClass()) {
//                n = interactor1->childrenCount();
//            } else if (!interactor1->isClass() && interactor2->isClass()) {
//                n = interactor2->childrenCount();
//            } else {
//                n = 1;
//            }

//            foreach(QString name, ddi->levelName().split(", ")) {
//                if (name.isEmpty()) {
//                    name = ddi->levelCode();
//                    qWarning() << "EMPTY LEVEL" << ddi->firstInteractor() << ddi->secondInteractor();
//                }
//                m_levelStatistics.insert(name, m_levelStatistics.value(name, 0) + n);
//                total += n;
//            }
//        }

//        QHashIterator<QString, int> it(m_levelStatistics);
//        QString out;
//        while (it.hasNext()) {
//            it.next();
//            out += QString("%1 %2 %3%\n")
//                    .arg(it.key().leftJustified(50, '.'))
//                    .arg(QString::number(it.value()).leftJustified(7))
//                    .arg(QString::number(it.value()*100/total, 'G', 2));
//        }
//        out += QString("\n%1 %2 100%\n")
//                .arg(QString("Total DDI per INN").leftJustified(50, '.'))
//                .arg(QString::number(total).leftJustified(7));
//        qWarning() << out;
//    }

    void filter(const QString &interactor = QString::null)
    {
        if (interactor == m_currentFilter)
            return;
        m_currentFilter = interactor;
        delete rootItem;
        rootItem = new DDITreeItem(0,0);
        QFont bold;
        bold.setBold(true);
        if (interactor.isEmpty()) {
            createTree(m_ddis);
        } else {
            // find all DDI related to the interactor
            QList<DrugDrugInteraction *> keepMe;
            for(int i=0; i < m_ddis.count(); ++i) {
                DrugDrugInteraction *ddi = m_ddis.at(i);
                if (!ddi->isValid())
                    continue;
                const QString &firstInteractor = ddi->firstInteractor();
                const QString &secondInteractor = ddi->secondInteractor();
                if (firstInteractor.startsWith(interactor, Qt::CaseInsensitive)) {
                    keepMe << ddi;
                } else if (secondInteractor.startsWith(interactor, Qt::CaseInsensitive)) {
                    keepMe << ddi;
                }
            }
            createTree(keepMe, interactor, false);
        }
        rootItem->sortChildren();
    }

    QString getTooltip(DrugDrugInteraction *ddi)
    {
        QString tmp;
        tmp += QString("<span style=\"color:#980000\"><b>%1 / %2</b> <br />%3</span><br />"
                  "<br /><span style=\"color:#0033CC\"><u>Risk:</u> %4</span><br />"
                  "<br /><span style=\"color:#336600\"><u>Management:</u> %5</span><br />")
                .arg(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())
                .arg(ddi->data(DrugDrugInteraction::SecondInteractorName).toString())
                .arg(ddi->data(DrugDrugInteraction::LevelName).toString())
                .arg(ddi->risk(""))
                .arg(ddi->management(""));
        // dose related ?
        QString dose;
        if (ddi->firstInteractorDose().data(DrugDrugInteractionDose::UsesFrom).toBool()) {
            dose += QString("<br />* From: %1 %2 %3 <br />")
                    .arg(ddi->firstInteractorDose().data(DrugDrugInteractionDose::FromValue).toString())
                    .arg(DrugDrugInteractionModel::unit(ddi->firstInteractorDose().data(DrugDrugInteractionDose::FromUnits).toInt()))
                    .arg(DrugDrugInteractionModel::repartition(ddi->firstInteractorDose().data(DrugDrugInteractionDose::FromRepartition).toInt()));
        }
        if (ddi->firstInteractorDose().data(DrugDrugInteractionDose::UsesTo).toBool()) {
            dose += QString("<br />* To: %1 %2 %3 <br />")
                    .arg(ddi->firstInteractorDose().data(DrugDrugInteractionDose::ToValue).toString())
                    .arg(DrugDrugInteractionModel::unit(ddi->firstInteractorDose().data(DrugDrugInteractionDose::ToUnits).toInt()))
                    .arg(DrugDrugInteractionModel::repartition(ddi->firstInteractorDose().data(DrugDrugInteractionDose::ToRepartition).toInt()));
        }
        if (!dose.isEmpty()) {
            tmp += QString("<br /><span style=\"color:#666600\"><u>First Molecule Dose:</u> %1</span><br />").arg(dose);
            dose.clear();
        }
        if (ddi->secondInteractorDose().data(DrugDrugInteractionDose::UsesFrom).toBool()) {
            dose += QString("* From: %1 %2 %3 <br />")
                    .arg(ddi->secondInteractorDose().data(DrugDrugInteractionDose::FromValue).toString())
                    .arg(DrugDrugInteractionModel::unit(ddi->secondInteractorDose().data(DrugDrugInteractionDose::FromUnits).toInt()))
                    .arg(DrugDrugInteractionModel::repartition(ddi->secondInteractorDose().data(DrugDrugInteractionDose::FromRepartition).toInt()));
        }
        if (ddi->secondInteractorDose().data(DrugDrugInteractionDose::UsesTo).toBool()) {
            dose += QString("* To: %1 %2 %3 <br />")
                    .arg(ddi->secondInteractorDose().data(DrugDrugInteractionDose::ToValue).toString())
                    .arg(DrugDrugInteractionModel::unit(ddi->secondInteractorDose().data(DrugDrugInteractionDose::ToUnits).toInt()))
                    .arg(DrugDrugInteractionModel::repartition(ddi->secondInteractorDose().data(DrugDrugInteractionDose::ToRepartition).toInt()));
        }
        if (!dose.isEmpty()) {
            tmp += QString("<br /><span style=\"color:#666600\"><u>Second Molecule Dose:</u> %1</span><br />").arg(dose);
            dose.clear();
        }
//        dose.clear();
//        const QStringList &routes = ddi->data(DrugDrugInteractionModel::FirstInteractorRouteOfAdministrationIds).toStringList();
//        if (routes.count() > 0) {
//            if (!routes.at(0).isEmpty()) {
//                dose += QString("* Routes: %1<br />")
//                        .arg(routes.join(";"));
//            }
//        }
        dose.clear();
        const QStringList &pmids = ddi->data(DrugDrugInteraction::PMIDsStringList).toStringList();
        if (pmids.count() > 0) {
            if (!pmids.at(0).isEmpty()) {
                foreach(const QString &pmid, pmids) {
                    dose += QString("<br />* PMID: <a href=\"http://www.ncbi.nlm.nih.gov/pubmed/%1\">%1</a>").arg(pmid);
                }
            }
        }
        if (!dose.isEmpty())
            tmp += QString("<br /><span style=\"color:#003333\"><u>Bibliography:</u> %1</span><br />").arg(dose);
        dose.clear();

        // Add model errors
        QStringList errors = m_ddiError.values(ddi);
        if (errors.count()) {
            tmp += QString("<br /><span style=\"color:#FF2020\"><u>Model Errors:</u> %1</span><br />").arg(errors.join("<br />"));
        }

        tmp.chop(6);
        return tmp;
    }

//    QColor interactorsColor(DrugDrugInteraction *ddi)
//    {
//        if (!m_interactorChecking.keys().contains(ddi)) {
//            m_interactorChecking.insert(ddi, 0);  // 0=Ok
//            bool firstFound = false;
//            bool secondFound = false;
//            const QString &first = ddi->firstInteractor();
//            const QString &second = ddi->secondInteractor();
//            DrugInteractor *firstInteractor = 0;
//            DrugInteractor *secondInteractor = 0;
//            for(int i=0; i<m_interactors.count();++i) {
//                const QString &id = m_interactors.at(i)->data(DrugInteractor::Uid).toString();
//                if (!firstFound) {
//                    if (id==first) {
//                        firstFound = true;
//                        firstInteractor = m_interactors.at(i);
//                    }
//                }
//                if (!secondFound) {
//                    if (id==second) {
//                        secondFound = true;
//                        secondInteractor = m_interactors.at(i);
//                    }
//                }
//                if (firstFound && secondFound)
//                    break;
//            }
//            bool ok = (firstFound && secondFound);
//            if (!ok) {
////                const QString &first = ddi->firstInteractor();
////                const QString &second = ddi->secondInteractor();
//                if (!firstFound) {
//                    if (!m_ddiError.values(ddi).contains("First interactor not found"))
//                        m_ddiError.insertMulti(ddi, "First interactor not found");
//                }
//                if (!secondFound) {
//                    if (!m_ddiError.values(ddi).contains("Second interactor not found"))
//                        m_ddiError.insertMulti(ddi, "Second interactor not found");
//                }
//                m_interactorChecking.insert(ddi, 1);  // 1=InteractorUnknown
//            } else {
//                if (!firstInteractor->isClass() && firstInteractor->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty()) {
//                    m_ddiError.insertMulti(ddi, "First interactor does not have ATC link");
//                    m_interactorChecking.insert(ddi, 2);  // 2=InteractorNoATC
//                }
//                if (!secondInteractor->isClass() && secondInteractor->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty()) {
//                    m_ddiError.insertMulti(ddi, "Second interactor does not have ATC link");
//                    m_interactorChecking.insert(ddi, 2);  // 2=InteractorNoATC
//                }
//            }
//        }
//        switch (m_interactorChecking.value(ddi, 0)) {
//        case 0:  // 0=OK
//            return QColor();
//        case 1:  // 1=InteractorUnknown  // red
//            return QColor(255,50,50,125);
//        case 2:  // 2=InteractorNoATC    // purple
//            return QColor(255,50,255,125);
//        }
//        return QColor();
//    }

public:
    DDITreeItem *rootItem;
    QList<DrugDrugInteraction *> m_ddis;
//    QList<DrugInteractor *> m_interactors;
    QMap<DrugDrugInteraction *, int> m_interactorChecking;  // value: 0=OK; 1=InteractorUnknown; 2=InteractorNoATC
    QMultiMap<DrugDrugInteraction *, QString> m_ddiError;
    QString reviewer;
    int m_FetchedRows;
    QString m_currentFilter;
    QHash<QString, int> m_levelStatistics;

private:
    DrugDrugInteractionModel *q;
};
}  // End namespace Internal
}  // End namespace DDI

DrugDrugInteractionModel::DrugDrugInteractionModel(QObject *parent) :
        QAbstractItemModel(parent), d(new Internal::DrugDrugInteractionModelPrivate(this))
{
    setObjectName("DrugDrugInteractionModel");
    d->m_ddis = ddiCore()->getDrugDrugInteractions();
//    d->m_interactors = ddiCore()->getDrugInteractors();
//    d->globalLevelStatistics(d->m_ddis);
    d->filter();
    d->m_FetchedRows = 0;
}

DrugDrugInteractionModel::~DrugDrugInteractionModel()
{
    if (d)
        delete d;
    d = 0;
}

int DrugDrugInteractionModel::rowCount(const QModelIndex &parent) const
{
    DDITreeItem *parentItem = d->getItem(parent);
    return parentItem->childCount();
}

int DrugDrugInteractionModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QModelIndex DrugDrugInteractionModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    DDITreeItem *parentItem = d->getItem(parent);
    DDITreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex DrugDrugInteractionModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    DDITreeItem *childItem = d->getItem(index);
    DDITreeItem *parentItem = childItem->parent();

    if (!parentItem)
        return QModelIndex();

    if (parentItem == d->rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

QVariant DrugDrugInteractionModel::data(const QModelIndex &index, int role) const
{
    DDITreeItem *item = d->getItem(index);
    if (!item) {
        return QVariant();
    }
    DrugDrugInteraction *ddi = item->ddi();
    if (!ddi) {
        if (!index.isValid())
            return QVariant();
        // Working on categories
        if (role==Qt::DisplayRole && index.column()==0) {
            return item->text();
        } else if (role==Qt::ForegroundRole) {
            // test all child DDI level validity
            for(int i = 0; i < item->childCount(); ++i) {
                if (!item->child(i)->ddi()->levelValidity())
                    return QColor(255,50,50,150);
            }
            // test all interactors
            for(int i = 0; i < item->childCount(); ++i) {
                const QColor &c = d->interactorsColor(item->child(i)->ddi());
                if (c.isValid())
                    return c;
            }
            // test translations
            for(int i = 0; i < item->childCount(); ++i) {
                if (item->child(i)->ddi()->risk("en").isEmpty())
                    return QColor(50,50,255,150);
            }
        } else if (role == Qt::FontRole) {
            if (item->childCount() > 0) {
                QFont bold;
                bold.setBold(true);
                return bold;
            }
        }
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case GeneralLabel: return item->text();
        case HumanReadableSynthesis: return d->getTooltip(ddi);
        case FirstInteractorName: return ddi->data(DrugDrugInteraction::FirstInteractorName);
        case SecondInteractorName: return ddi->data(DrugDrugInteraction::SecondInteractorName);
        case LevelCode: return ddi->data(DrugDrugInteraction::LevelCode);
        case LevelName: return ddi->data(DrugDrugInteraction::LevelName);
        case DateCreation: return ddi->data(DrugDrugInteraction::DateCreation);
        case DateLastUpdate: return ddi->data(DrugDrugInteraction::DateLastUpdate);
        case ListOfUpdates: return ddi->data(DrugDrugInteraction::ListOfUpdates);
        case RiskFr: return ddi->risk("fr");
        case RiskEn: return ddi->risk("en");
        case ManagementFr: return ddi->management("fr");
        case ManagementEn: return ddi->management("en");
        case IsReviewed: return ddi->data(DrugDrugInteraction::IsReviewed);
        case IsReviewedCheckState: if (ddi->data(DrugDrugInteraction::IsReviewed).toBool()) return Qt::Checked; else return Qt::Unchecked;
        case ReviewersStringList: return QStringList();
        case InternalUid: return ddi->data(DrugDrugInteraction::InternalUuid);
        case InternalUidWithInteractors:
        {
            QString first = ddi->firstInteractor();
            QString second = ddi->secondInteractor();
            if (first.size() > 10)
                first = first.left(10) + "...";
            if (second.size() > 10)
                second = second.left(10) + "...";
            return QString("%1 - %2 <-> %3")
                    .arg(ddi->data(DrugDrugInteraction::InternalUuid).toString())
                    .arg(first)
                    .arg(second);
        }
        case LevelComboIndex:
        {
            // Be aware that InteractionEditorWidget ui->comboLevel must be sync with this part !!!
            const QString &levelCode = ddi->levelCode();
            if (levelCode=="C")   return 0;
            if (levelCode=="D")   return 1;
            if (levelCode=="450") return 2;
            if (levelCode=="Y")   return 3;
            if (levelCode=="T")   return 4;
            if (levelCode=="P")   return 5;
            if (levelCode=="I")   return 6;
            return -1;
        }
        case FirstInteractorRouteOfAdministrationIds: return ddi->data(DrugDrugInteraction::FirstInteractorRouteOfAdministrationIds);
        case SecondInteractorRouteOfAdministrationIds: return ddi->data(DrugDrugInteraction::SecondInteractorRouteOfAdministrationIds);

        case FirstDoseUseFrom: return ddi->firstInteractorDose().data(DrugDrugInteractionDose::UsesFrom);
        case FirstDoseUsesTo: return ddi->firstInteractorDose().data(DrugDrugInteractionDose::UsesTo);
        case FirstDoseFromValue:
        {
            const QVariant &v = ddi->firstInteractorDose().data(DrugDrugInteractionDose::FromValue);
            if (v.isNull())
                return QString();
            return v;
        }
        case FirstDoseFromUnits:
        {
            const QVariant &v = ddi->firstInteractorDose().data(DrugDrugInteractionDose::FromUnits);
            if (v.isNull())
                return -1;
            return v;
        }
        case FirstDoseFromRepartition:
        {
            const QVariant &v = ddi->firstInteractorDose().data(DrugDrugInteractionDose::FromRepartition);
            if (v.isNull())
                return -1;
            return v;
        }
        case FirstDoseToValue:
        {
            const QVariant &v = ddi->firstInteractorDose().data(DrugDrugInteractionDose::ToValue);
            if (v.isNull())
                return QString();
            return v;
        }
        case FirstDoseToUnits:
        {
            const QVariant &v = ddi->firstInteractorDose().data(DrugDrugInteractionDose::ToUnits);
            if (v.isNull())
                return -1;
            return v;
        }
        case FirstDoseToRepartition:
        {
            const QVariant &v = ddi->firstInteractorDose().data(DrugDrugInteractionDose::ToRepartition);
            if (v.isNull())
                return -1;
            return v;
        }

        case SecondDoseUseFrom: return ddi->secondInteractorDose().data(DrugDrugInteractionDose::UsesFrom);
        case SecondDoseUsesTo: return ddi->secondInteractorDose().data(DrugDrugInteractionDose::UsesTo);
        case SecondDoseFromValue:
        {
            const QVariant &v = ddi->secondInteractorDose().data(DrugDrugInteractionDose::FromValue);
            if (v.isNull())
                return QString();
            return v;
        }
        case SecondDoseFromUnits:
        {
            const QVariant &v = ddi->secondInteractorDose().data(DrugDrugInteractionDose::FromUnits);
            if (v.isNull())
                return -1;
            return v;
        }
        case SecondDoseFromRepartition:
        {
            const QVariant &v = ddi->secondInteractorDose().data(DrugDrugInteractionDose::FromRepartition);
            if (v.isNull())
                return -1;
            return v;
        }
        case SecondDoseToValue:
        {
            const QVariant &v = ddi->secondInteractorDose().data(DrugDrugInteractionDose::ToValue);
            if (v.isNull())
                return QString();
            return v;
        }
        case SecondDoseToUnits:
        {
            const QVariant &v = ddi->secondInteractorDose().data(DrugDrugInteractionDose::ToUnits);
            if (v.isNull())
                return -1;
            return v;
        }
        case SecondDoseToRepartition:
        {
            const QVariant &v = ddi->secondInteractorDose().data(DrugDrugInteractionDose::ToRepartition);
            if (v.isNull())
                return -1;
            return v;
        }
        case PMIDStringList: return ddi->data(DrugDrugInteraction::PMIDsStringList);
        } // End switch
    } else if (role == Qt::CheckStateRole) {
        if (index.column()==IsReviewed) {
            if (ddi->data(DrugDrugInteraction::IsReviewed).toBool())
                return Qt::Checked;
            return Qt::Unchecked;
        }
    } else if (role == Qt::ToolTipRole) {
        return d->getTooltip(ddi);
    } else if (role==Qt::ForegroundRole) {
        if (!ddi->levelValidity()) {
            if (!d->m_ddiError.values(ddi).contains(tr("Level is not valid")))
                d->m_ddiError.insertMulti(ddi, tr("Level is not valid"));
            return QColor(255,50,50,150);
        }
        if (d->interactorsColor(ddi) != QColor()) {
            return d->interactorsColor(ddi);
        }
        if (ddi->risk("en").isEmpty()) {
            if (!d->m_ddiError.values(ddi).contains(tr("Not translated")))
                d->m_ddiError.insertMulti(ddi, tr("Not translated"));
            return QColor(50,50,255,150);
        }
    } else if (role == Qt::DecorationRole) {
        if (item->childCount() == 0)
            return ddi->levelIcon();
    } else if (role == Qt::FontRole) {
        if (item->childCount() > 0) {
            QFont bold;
            bold.setBold(true);
            return bold;
        }
    }
    return QVariant();
}

bool DrugDrugInteractionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    DDITreeItem *item = d->getItem(index);
    if (!item) {
        return false;
    }

    DrugDrugInteraction *ddi = item->ddi();
    if (!ddi || index.column()==0) {
        return false;
    }

    if (role==Qt::EditRole) {
        switch (index.column()) {
        case SecondInteractorName:
            ddi->setData(DrugDrugInteraction::SecondInteractorName, value);
            // if item is the interactor and not the category
            if (item->childCount()==0) {
                item->setText(value.toString());
                QModelIndex generalLabel = this->index(index.row(), GeneralLabel, index.parent());
                Q_EMIT dataChanged(generalLabel, generalLabel);
            }
            break;
        case LevelCode:
        {
            ddi->setData(DrugDrugInteraction::LevelCode, value);
            QModelIndex levelName = this->index(index.row(), LevelName, index.parent());
            Q_EMIT dataChanged(levelName, levelName);
            QModelIndex levelComboIndex = this->index(index.row(), LevelComboIndex, index.parent());
            Q_EMIT dataChanged(levelComboIndex, levelComboIndex);
            break;
        }
        case LevelComboIndex:
        {
            // Be aware that InteractionEditorWidget ui->comboLevel must be sync with this part !!!
            switch (value.toInt()) {
            case 0: ddi->setData(DrugDrugInteraction::LevelCode, "C"); break;
            case 1: ddi->setData(DrugDrugInteraction::LevelCode, "D"); break;
            case 2: ddi->setData(DrugDrugInteraction::LevelCode, "450"); break;
            case 3: ddi->setData(DrugDrugInteraction::LevelCode, "Y"); break;
            case 4: ddi->setData(DrugDrugInteraction::LevelCode, "T"); break;
            case 5: ddi->setData(DrugDrugInteraction::LevelCode, "P"); break;
            case 6: ddi->setData(DrugDrugInteraction::LevelCode, "I"); break;
            }
            QModelIndex levelCode = this->index(index.row(), LevelCode, index.parent());
            Q_EMIT dataChanged(levelCode, levelCode);
            QModelIndex levelName = this->index(index.row(), LevelName, index.parent());
            Q_EMIT dataChanged(levelName, levelName);
            break;
        }
        case Comment:
            ddi->setData(DrugDrugInteraction::Comment, value);
            break;
        case ReviewersStringList:
            ddi->setData(DrugDrugInteraction::ReviewersStringList, value);
            break;
        case DateCreation:
            ddi->setData(DrugDrugInteraction::DateCreation, value);
            break;
        case DateLastUpdate:
            ddi->setData(DrugDrugInteraction::DateLastUpdate, value);
            break;
        case RiskFr:
            ddi->setRisk(value.toString(), "fr");
            break;
        case RiskEn:
            ddi->setRisk(value.toString(), "en");
            break;
        case ManagementFr:
            ddi->setManagement(value.toString(), "fr");
            break;
        case ManagementEn:
            ddi->setManagement(value.toString(), "en");
            break;
        case IsReviewed:
        case IsReviewedCheckState:
        {
            if (value.toInt()>=1)
                ddi->setData(DrugDrugInteraction::IsReviewed, true);
            else
                ddi->setData(DrugDrugInteraction::IsReviewed, false);
            QModelIndex rev = this->index(index.row(), IsReviewed, index.parent());
            Q_EMIT dataChanged(rev, rev);
            break;
        }
        case FirstInteractorRouteOfAdministrationIds:
        {
            ddi->setData(DrugDrugInteraction::FirstInteractorRouteOfAdministrationIds, value);
            break;
        }
        case SecondInteractorRouteOfAdministrationIds:
        {
            ddi->setData(DrugDrugInteraction::SecondInteractorRouteOfAdministrationIds, value);
            break;
        }
        case FirstDoseUseFrom:
        {
            DrugDrugInteractionDose &dose = ddi->firstInteractorDose();
            dose.setData(DrugDrugInteractionDose::UsesFrom, value.toInt());
            break;
        }
        case FirstDoseUsesTo:
        {
            DrugDrugInteractionDose &dose = ddi->firstInteractorDose();
            dose.setData(DrugDrugInteractionDose::UsesTo, value.toInt());
            break;
        }
        case FirstDoseFromValue:
        {
            DrugDrugInteractionDose &dose = ddi->firstInteractorDose();
            dose.setData(DrugDrugInteractionDose::FromValue, value);
            break;
        }
        case FirstDoseFromUnits:
        {
            DrugDrugInteractionDose &dose = ddi->firstInteractorDose();
            dose.setData(DrugDrugInteractionDose::FromUnits, value);
            break;
        }
        case FirstDoseFromRepartition:
        {
            DrugDrugInteractionDose &dose = ddi->firstInteractorDose();
            dose.setData(DrugDrugInteractionDose::FromRepartition, value);
            break;
        }
        case FirstDoseToValue:
        {
            DrugDrugInteractionDose &dose = ddi->firstInteractorDose();
            dose.setData(DrugDrugInteractionDose::ToValue, value);
            break;
        }
        case FirstDoseToUnits:
        {
            DrugDrugInteractionDose &dose = ddi->firstInteractorDose();
            dose.setData(DrugDrugInteractionDose::ToUnits, value);
            break;
        }
        case FirstDoseToRepartition:
        {
            DrugDrugInteractionDose &dose = ddi->firstInteractorDose();
            dose.setData(DrugDrugInteractionDose::ToRepartition, value);
            break;
        }
        case SecondDoseUseFrom:
        {
            DrugDrugInteractionDose &dose = ddi->secondInteractorDose();
            dose.setData(DrugDrugInteractionDose::UsesFrom, value);
            break;
        }
        case SecondDoseUsesTo:
        {
            DrugDrugInteractionDose &dose = ddi->secondInteractorDose();
            dose.setData(DrugDrugInteractionDose::UsesTo, value);
            break;
        }
        case SecondDoseFromValue:
        {
            DrugDrugInteractionDose &dose = ddi->secondInteractorDose();
            dose.setData(DrugDrugInteractionDose::FromValue, value);
            break;
        }
        case SecondDoseFromUnits:
        {
            DrugDrugInteractionDose &dose = ddi->secondInteractorDose();
            dose.setData(DrugDrugInteractionDose::FromUnits, value);
            break;
        }
        case SecondDoseFromRepartition:
        {
            DrugDrugInteractionDose &dose = ddi->secondInteractorDose();
            dose.setData(DrugDrugInteractionDose::FromRepartition, value);
            break;
        }
        case SecondDoseToValue:
        {
            DrugDrugInteractionDose &dose = ddi->secondInteractorDose();
            dose.setData(DrugDrugInteractionDose::ToValue, value);
            break;
        }
        case SecondDoseToUnits:
        {
            DrugDrugInteractionDose &dose = ddi->secondInteractorDose();
            dose.setData(DrugDrugInteractionDose::ToUnits, value);
            break;
        }
        case SecondDoseToRepartition:
        {
            DrugDrugInteractionDose &dose = ddi->secondInteractorDose();
            dose.setData(DrugDrugInteractionDose::ToRepartition, value);
            break;
        }
        case PMIDStringList: return ddi->setData(DrugDrugInteraction::PMIDsStringList, value);
        default:
            return false;
        }
        // DateLastUpdate -> automatic
        ddi->setData(DrugDrugInteraction::DateLastUpdate, QDate::currentDate());
        QModelIndex date = this->index(index.row(), DateLastUpdate, index.parent());
        QModelIndex synth = this->index(index.row(), HumanReadableSynthesis, index.parent());
        Q_EMIT dataChanged(index, index);
        Q_EMIT dataChanged(date, date);
        Q_EMIT dataChanged(synth, synth);
        return true;
    } else if (role==Qt::CheckStateRole && index.column()==IsReviewed) {
        ddi->setData(DrugDrugInteraction::IsReviewed, (value.toInt() == Qt::Checked));
        if (d->reviewer != "Reviewer" && !d->reviewer.isEmpty())
            ddi->addReviewer(d->reviewer);
        ddi->setData(DrugDrugInteraction::DateLastUpdate, QDate::currentDate());
        QModelIndex date = this->index(index.row(), DateLastUpdate, index.parent());
        Q_EMIT dataChanged(index, index);
        Q_EMIT dataChanged(date, date);
        return true;
    }

    return false;
}

// todo -> manage the mirrored DDI ??
bool DrugDrugInteractionModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // insert row to categories only
    if (!parent.isValid())
        return false;
    DDITreeItem *parentItem = d->getItem(parent);
    if (parentItem->childCount()==0)  // parent is an interactor not a category
        return false;
    beginInsertRows(parent, row, row+count);
    for(int i = 0; i < count; ++i) {
        DrugDrugInteraction *ddi = new DrugDrugInteraction;
        d->m_ddis.append(ddi);
        ddi->setData(DrugDrugInteraction::FirstInteractorName, parentItem->text());
        ddi->setData(DrugDrugInteraction::DateCreation, QDate::currentDate());
        ddi->setData(DrugDrugInteraction::InternalUuid, ddiCore()->createInternalUuid());
        DDITreeItem *item = new DDITreeItem(ddi, parentItem);
        parentItem->removeChild(item);
        parentItem->insertChild(row+i, item);
    }
    endInsertRows();
    return true;
}

// todo remove mirrored
bool DrugDrugInteractionModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // only remove rows with a category parent
    if (!parent.isValid())
        return false;
    beginRemoveRows(parent, row, row+count);
    DDITreeItem *parentItem = d->getItem(parent);
    for(int i = 0; i < count; ++i) {
//        const QString &interactor = parentItem->child(row+i)->ddi()->data(DrugDrugInteraction::IsValid, false);
        parentItem->child(row+i)->ddi()->setData(DrugDrugInteraction::IsValid, false);
        parentItem->removeChild(row+i);
        // remove mirrored
    }
    endRemoveRows();
    return true;
}

Qt::ItemFlags DrugDrugInteractionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

//    if (index.column() == Review)
//        f |= Qt::ItemIsUserCheckable;

    f |= Qt::ItemIsEditable;

    return f;
}

bool DrugDrugInteractionModel::splitMultipleLevelInteraction(const QModelIndex &index, bool splitMirrored)
{
    DDITreeItem *itemToSplit = d->getItem(index);
    DDITreeItem *parentItem = d->getItem(index.parent());
    if (!itemToSplit || !parentItem)
        return false;
    // no category
    if (itemToSplit->childCount()>0)
        return false;
    // only if ddi is known
    if (!itemToSplit->ddi())
        return false;
    // only invalid ddi levels
    if (itemToSplit->ddi()->levelValidity())
        return false;

    // keep the splitting ddi index in a persistent index
    QPersistentModelIndex indexToSplit(index);
    QString categoryName = this->index(index.parent().row(), 0, index.parent().parent()).data().toString();
    int uuid = itemToSplit->ddi()->data(DrugDrugInteraction::InternalUuid).toInt();
    QString interactorName = this->index(index.row(), 0, index.parent()).data().toString();

    // 1. Split the level
    QString level = itemToSplit->ddi()->data(DrugDrugInteraction::LevelCode).toString();
    QStringList levels;
    if (level.contains("450")) {
        // create a 450 interaction
        levels.append("450");
        level = level.remove("450");
    }
    for(int i=0; i < level.size(); ++i) {
        levels.append(QString(level.at(i)));
    }

    // 2. Insert new interactions
    beginInsertRows(index.parent(), index.row(), index.row()+levels.count()-1);
    for(int i=0; i < levels.count(); ++i) {
        DrugDrugInteraction *ddi = new DrugDrugInteraction(*itemToSplit->ddi());
        ddi->setData(DrugDrugInteraction::LevelCode, levels.at(i));
        ddi->setData(DrugDrugInteraction::DateLastUpdate, QDate::currentDate());
        d->m_ddis.append(ddi);
        DDITreeItem *newItem = new DDITreeItem(ddi, parentItem);
        parentItem->removeChild(newItem);
        parentItem->insertChild(index.row()+i, newItem);
        const QString &first = ddi->data(DrugDrugInteraction::FirstInteractorName).toString();
        const QString &second = ddi->data(DrugDrugInteraction::SecondInteractorName).toString();
        if (first==categoryName)
            newItem->setText(second);
        else
            newItem->setText(first);
    }
    endInsertRows();

    // 3. Remove the splitted interaction
    removeRow(indexToSplit.row(), indexToSplit.parent());

    // 4. Do the same with the mirrored ddi
    if (splitMirrored) {
        // find mirrored category
        for(int i=0; i < rowCount(); ++i) {
            QModelIndex mirroredCategory = this->index(i, 0);
            if (mirroredCategory.data().toString()==interactorName) {
                // find mirrored interactor
                for(int j=0; j < rowCount(mirroredCategory); ++j) {
                    QModelIndex mirroredDdi = this->index(j, 0, mirroredCategory);
                    DDITreeItem *it = d->getItem(mirroredDdi);
                    if (it->ddi()->data(DrugDrugInteraction::InternalUuid).toInt()==uuid) {
                        splitMultipleLevelInteraction(this->index(j, 0, mirroredCategory), false);
                        return true;
                    }
                }
            }
        }
        LOG_ERROR("Mirrored DDI not found");
        return false;
    }
    return true;
}

QStringList DrugDrugInteractionModel::units()
{
    QStringList l;
    l << "mg";
    l << "µg";
    l << "g";
    l << "UI";
    l << "ml";
    l << "dl";
    l << "cl";
    l << "ml";
    return l;
}

QStringList DrugDrugInteractionModel::repartitions()
{
    QStringList l;
    l << tkTr(Trans::Constants::MINUTES);
    l << tkTr(Trans::Constants::HOURS);
    l << tkTr(Trans::Constants::DAYS);
    l << tkTr(Trans::Constants::WEEKS);
    return l;
}

QString DrugDrugInteractionModel::unit(int index)
{
    const QStringList &l = units();
    if (index < l.count() && index >= 0)
        return l.at(index);
    return QString();
}

QString DrugDrugInteractionModel::repartition(int index)
{
    const QStringList &l = repartitions();
    if (index < l.count() && index >= 0)
        return l.at(index);
    return QString();
}

//bool DrugDrugInteractionModel::canFetchMore(const QModelIndex &parent) const
//{
//    int nbItemRows = 0;
//    DDITreeItem *parentItem = d->getItem(parent);

//    if (parentItem)
//        nbItemRows = parentItem->childCount();

//    qWarning() << Q_FUNC_INFO << (d->m_FetchedRows < nbItemRows) << parent << nbItemRows << d->m_FetchedRows;

//    return (d->m_FetchedRows < nbItemRows);
//}

//void DrugDrugInteractionModel::fetchMore(const QModelIndex &parent)
//{
//    int nbItemRows = 0;
//    DDITreeItem *parentItem = d->getItem(parent);

//    if (parentItem)
//        nbItemRows = parentItem->childCount();

//    int remainder = nbItemRows - d->m_FetchedRows;
//    int itemsToFetch = qMin(FETCH_LIMIT, remainder);

//    qWarning() << Q_FUNC_INFO << parent << d->m_FetchedRows << itemsToFetch;

//    beginInsertRows(parent, d->m_FetchedRows, d->m_FetchedRows + itemsToFetch);
//    d->m_FetchedRows += itemsToFetch;
//    endInsertRows();
//}

/**
 * Computes the statistics of the model and return a QHash with
 * in keys the level names, and in value the number of possible interactions.
 */
QHash<QString, int> DrugDrugInteractionModel::getLevelStatistics() const
{
//    d->globalLevelStatistics(d->m_ddis);
    return d->m_levelStatistics;
}

/**
 * Add an interaction in the model.
 * Return true if the interaction was correctly added. \n
 * NOTE: When the DDI is added, the model gets re-filtered with the previous filter.
 */
bool DrugDrugInteractionModel::addDrugDrugInteraction(const Internal::DrugDrugInteraction &ddi)
{
    if (!ddi.isValid())
        return false;
    d->m_ddis << new DrugDrugInteraction(ddi);
    QString save = d->m_currentFilter;
    d->m_currentFilter.clear();
    d->filter(save);
    return true;
}

void DrugDrugInteractionModel::filterInteractionsForInteractor(const QString &interactorName)
{
    d->filter(interactorName);
    d->m_FetchedRows = 0;
    reset();
}

void DrugDrugInteractionModel::setActualReviewer(const QString &name)
{
    d->reviewer = name;
}

bool DrugDrugInteractionModel::saveModel()
{
    ddiCore()->saveCompleteList(d->m_ddis);
    return true;
}
