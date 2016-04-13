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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class ICD::IcdCollectionModel
  \brief Tree model for ICD codes collections (a collection is a set of ICD10 codes and/or association of codes).

  Collection model can manage two modes:
  - full collection: where ICD10 coding rules are analyzed when you ask for code addition ;
  - simple list collection mode: where ICD10 coding rules are simply ignored.

  You can add simple ICD10 codes with addCode() or add association of ICD10 codes (dag and star associated)
  with addAssociation. Before the addition is done, the codes and associations are tested using canAddThisCode()
  or canAddThisAssociation(). These functions, according to the mode, manage the coding rules (exclusions from
  already included codes, dag/star verification...).

  The model avoid duplication of codes.

*/

#include "icdcollectionmodel.h"
#include "icdassociation.h"
#include "icddatabase.h"

#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <utils/log.h>


using namespace ICD;

static inline ICD::IcdDatabase *icdBase() {return ICD::IcdDatabase::instance();}


namespace ICD {

namespace Internal {

struct ModelItem {
    ModelItem() : sid(-1) {}
    int sid;
    QString code, label;
    Internal::IcdAssociation association;
};

class IcdCollectionModelPrivate {
public:
    IcdCollectionModelPrivate(IcdCollectionModel *parent) :
            m_IsSimpleList(false),
            q(parent)
    {
    }
    ~IcdCollectionModelPrivate()
    {
    }

    void translateRow(const int row)
    {
        // translate row
        QVariant mainsid = q->index(row, IcdCollectionModel::SID).data();
        q->setData(q->index(row, IcdCollectionModel::Label), icdBase()->getSystemLabel(mainsid));

        // translate all its children
        const QModelIndex &p = q->index(row, 0);
        if (q->hasChildren(p)) {
            for(int i = 0; i < q->rowCount(p); ++i) {
                QVariant sid = q->index(i, IcdCollectionModel::SID, p).data();
                q->setData(q->index(i, IcdCollectionModel::Label, p), icdBase()->getAssociatedLabel(mainsid, sid));
            }
        }
    }

public:
    QVector<int> m_ExcludedSIDs;
    QVector<int> m_SIDs;
    bool m_IsSimpleList;

private:
    IcdCollectionModel *q;
};


}  // End namespace Internal
}  // End namespace ICD

IcdCollectionModel::IcdCollectionModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new Internal::IcdCollectionModelPrivate(this))
{
    setObjectName("IcdCollectionModel");
    connect(Core::ICore::instance()->translators(), SIGNAL(languageChanged()), this, SLOT(languageChanged()));
}

IcdCollectionModel::~IcdCollectionModel()
{
    if (d)
        delete d;
    d = 0;
}

/** \brief Set the collection model as a simple list of ICD10 codes. No coding rules will be used (daget, exclusions, inclusions...). */
void IcdCollectionModel::setCollectionIsSimpleList(bool state)
{
    d->m_IsSimpleList = state;
}

/** \sa setCollectionIsSimpleList() */
bool IcdCollectionModel::isCollectionSimpleList() const
{
    return d->m_IsSimpleList;
}

/**
  \brief Add a single ICD code using its \e SID to the model with/without daget checking.
  This member checks the ICD code exclusion from the already included codes, and
  checks the daget coding validity.
*/
bool IcdCollectionModel::canAddThisCode(const QVariant &SID, bool checkDaget) const
{
    // already included ?
    if (d->m_SIDs.contains(SID.toInt()))
        return false;

    if (d->m_IsSimpleList)
        return true;

    // in exclusions ?
    if (d->m_ExcludedSIDs.contains(SID.toInt()))
        return false;
    foreach(const int sid, icdBase()->getHeadersSID(SID)) {
        if (d->m_ExcludedSIDs.contains(sid)) {
//            qWarning() << "asso header excluded" << d->m_ExcludedSIDs << sid;
            return false;
        }
    }

    // code daget ?
    if (checkDaget) {
        if (!icdBase()->codeCanBeUsedAlone(SID))
            return false;
    }

    return true;
}

/**
  \brief Add an ICD association \e asso to the model with daget checking.
  This member checks the ICD code exclusion from the already included codes, and
  checks the daget coding validity.
*/
bool IcdCollectionModel::canAddThisAssociation(const Internal::IcdAssociation &asso) const
{
    // Association valid ?
    if (!asso.isValid()) {
//        qWarning() << "not valid";
        return false;
    }

    if (d->m_IsSimpleList)
        return true;

    // in exclusions ?
    if (d->m_ExcludedSIDs.contains(asso.mainSid().toInt())) {
//        qWarning() << "main excluded" << d->m_ExcludedSIDs << asso.mainSid().toInt();
        return false;
    }
    foreach(const int sid, icdBase()->getHeadersSID(asso.mainSid().toInt())) {
        if (d->m_ExcludedSIDs.contains(sid)) {
//            qWarning() << "main header excluded" << d->m_ExcludedSIDs << sid;
            return false;
        }
    }

    // already included ?
    if (d->m_SIDs.contains(asso.associatedSid().toInt())) {
//        qWarning() << "asso already";
        return false;
    }
    // in exclusions ?
    if (d->m_ExcludedSIDs.contains(asso.associatedSid().toInt())) {
//        qWarning() << "asso excluded" << d->m_ExcludedSIDs << asso.associatedSid().toInt();
        return false;
    }
    foreach(const int sid, icdBase()->getHeadersSID(asso.associatedSid().toInt())) {
        if (d->m_ExcludedSIDs.contains(sid)) {
//            qWarning() << "asso header excluded" << d->m_ExcludedSIDs << sid;
            return false;
        }
    }

    return true;
}

/**
  \brief Add a single ICD code \e asso to the model.
  This member will firstly check if the code can be added to the model.
*/
bool IcdCollectionModel::addCode(const QVariant &SID)
{
    // Can add this code ?
    if (!canAddThisCode(SID)) {
        LOG_ERROR(tr("Can not add this code: %1").arg(icdBase()->getIcdCode(SID).toString()));
        return false;
    }

    // add Code to model
    d->m_SIDs.append(SID.toInt());

    QStandardItem *parentItem = invisibleRootItem();
    QList<QStandardItem *> list;
    list
            << new QStandardItem(icdBase()->getIcdCode(SID).toString())
            << new QStandardItem(icdBase()->getSystemLabel(SID))
            << new QStandardItem(icdBase()->getIcdCode(SID).toString())  // Code without daget
            << new QStandardItem(QString())            // Human readable daget
            << new QStandardItem(QString())            // DagCode
            << new QStandardItem(SID.toString());  // SID
    parentItem->appendRow(list);

    // get all exclusions
    d->m_ExcludedSIDs << icdBase()->getExclusions(SID);
    return true;
}

bool IcdCollectionModel::addCode(const QString &code)
{
    QVariant sid = icdBase()->getSid(code);
    return addCode(sid);
}

/**
  \brief Add an ICD association \e asso to the model.
  This member will firstly check if the association can be added to the model.
*/
bool IcdCollectionModel::addAssociation(const Internal::IcdAssociation &asso)
{
    // Can add this association ?
    if (!canAddThisAssociation(asso)) {
        Utils::Log::addError(this, tr("Can not add this Association: %1-%2")
                             .arg(icdBase()->getIcdCode(asso.mainSid()).toString())
                             .arg(icdBase()->getIcdCode(asso.associatedSid()).toString()));
        return false;
    }

    // add Code to model
    d->m_SIDs.append(asso.mainSid().toInt());
    d->m_SIDs.append(asso.associatedSid().toInt());

    // Find root item (mainItem) based on the SID
    QStandardItem *parentItem = 0;
    QStandardItem *main = 0;
    QList<QStandardItem *> list;
    if (asso.mainIsDag()) {
        list = findItems(asso.mainCodeWithDagStar(), Qt::MatchExactly, 0);
    } else {
        list = findItems(asso.associatedCodeWithDagStar(), Qt::MatchExactly, 0);
    }
    if (list.count()==0) {
        parentItem = invisibleRootItem();
        if (asso.mainIsDag()) {
            main = new QStandardItem(asso.mainCodeWithDagStar());
            list
                    << main
                    << new QStandardItem(asso.mainLabel())
                    << new QStandardItem(asso.mainCode())     // Code without daget
                    << new QStandardItem(asso.mainDaget())    // Human readable daget
                    << new QStandardItem(asso.dagCode())      // DagCode
                    << new QStandardItem(asso.mainSid().toString())
                    ;
        } else {
            main = new QStandardItem(asso.associatedCodeWithDagStar());
            list
                    << main
                    << new QStandardItem(asso.associatedLabel())
                    << new QStandardItem(asso.associatedCode())     // Code without daget
                    << new QStandardItem(asso.associatedDaget())    // Human readable daget
                    << new QStandardItem(icdBase()->invertDagCode(asso.dagCode()))
                    << new QStandardItem(asso.associatedSid().toString())
                    ;
        }
        parentItem->appendRow(list);
        parentItem = main;
    } else {
        parentItem = list.at(0);
    }
    list.clear();
    if (asso.mainIsDag()) {
        list
                << new QStandardItem(asso.associatedCodeWithDagStar())
                << new QStandardItem(asso.associatedLabel())
                << new QStandardItem(asso.associatedCode())     // Code without daget
                << new QStandardItem(asso.associatedDaget())    // Human readable daget
                << new QStandardItem(icdBase()->invertDagCode(asso.dagCode()))
                << new QStandardItem(asso.associatedSid().toString())
                ;
    } else {
        list
                << new QStandardItem(asso.mainCodeWithDagStar())
                << new QStandardItem(asso.mainLabel())
                << new QStandardItem(asso.mainCode())     // Code without daget
                << new QStandardItem(asso.mainDaget())    // Human readable daget
                << new QStandardItem(asso.dagCode())      // DagCode
                << new QStandardItem(asso.mainSid().toString())
                ;
    }
    parentItem->appendRow(list);

    // get all exclusions
    if (asso.mainIsDag()) {
        d->m_ExcludedSIDs << icdBase()->getExclusions(asso.mainSid());
    } else {
        d->m_ExcludedSIDs << icdBase()->getExclusions(asso.associatedSid());
    }

    return true;
}

/**
  \brief Clear the model. Please do not use the QAbstractItemModel::clear(). Use this instead.
  Using the QAbstractItemModel::clear() member will not properly clean the model. ICD10 rules
  will be wrong.
*/
void IcdCollectionModel::clearCollection()
{
    d->m_SIDs.clear();
    d->m_ExcludedSIDs.clear();
    QStandardItemModel::clear();
}

/**
  \brief Return the full list of included codes.
  Single codes are added as single item of the QStringList,
  Associations are added as single item of the QStringList serialized like (code1/code2).
*/
QStringList IcdCollectionModel::includedCodesWithDaget() const
{
    QStringList codes;
    for(int i = 0; i < rowCount(); ++i) {
        QModelIndex idx = index(i, CodeWithDaget);
        QStringList children;
        if (hasChildren(idx)) {
            for(int j = 0; j < rowCount(idx); ++j) {
                QModelIndex child = index(i, CodeWithDaget, idx);
                children << child.data().toString();
            }
        }
        QString c = children.join(";");
        if (!c.isEmpty()) {
            c.prepend("(");
            c.append(")");
        }
        codes << idx.data().toString() + c;
    }
    return codes;
}

QStringList IcdCollectionModel::includedCodesWithoutDaget() const
{
    QStringList codes;
    for(int i = 0; i < rowCount(); ++i) {
        QModelIndex idx = index(i, CodeWithoutDaget);
        QStringList children;
        if (hasChildren(idx)) {
            for(int j = 0; j < rowCount(idx); ++j) {
                QModelIndex child = index(i, CodeWithoutDaget, idx);
                children << child.data().toString();
            }
        }
        QString c = children.join(";");
        if (!c.isEmpty()) {
            c.prepend("(");
            c.append(")");
        }
        codes << idx.data().toString() + c;
    }
    return codes;
}

/** \brief Return the full list of included labels in HTML format. */
QString IcdCollectionModel::includedLabelsToHtml() const
{
    QString html;
    for(int i = 0; i < rowCount(); ++i) {
        QModelIndex idx = index(i, Label);
        QModelIndex parent = index(i, CodeWithDaget);
        QString children;
        if (hasChildren(parent)) {
            for(int j = 0; j < rowCount(parent); ++j) {
                QModelIndex child = index(j, Label, parent);
                QString l = child.data().toString();
                if (l.compare(idx.data().toString(), Qt::CaseInsensitive)!=0) {
                    children += "<br />&nbsp;&nbsp;&nbsp;*&nbsp;" + l;
                }
            }
        }
        html += idx.data().toString() + children + "<br />";
    }
    return html;
}

QStringList IcdCollectionModel::includedLabels() const
{
    QStringList labels;
    for(int i = 0; i < rowCount(); ++i) {
        QModelIndex idx = index(i, Label);
        QModelIndex parent = index(i, CodeWithDaget);
        QStringList children;
        if (hasChildren(parent)) {
            for(int j = 0; j < rowCount(parent); ++j) {
                QModelIndex child = index(j, Label, parent);
                QString l = child.data().toString();
                if (l.compare(idx.data().toString(), Qt::CaseInsensitive)!=0) {
                    children << l;
                }
            }
        }
        labels << idx.data().toString();
        labels << children;
    }
    return labels;
}

Qt::ItemFlags IcdCollectionModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void IcdCollectionModel::languageChanged()
{
    // Update labels
    for(int i=0; i<rowCount();++i) {
        d->translateRow(i);
    }
}
