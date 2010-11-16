/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "icdmodel.h"
#include "icddatabase.h"
#include "icdassociation.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <translationutils/constanttranslations.h>

#include <QString>
#include <QLocale>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringListModel>
#include <QPointer>

#include <QDebug>

using namespace ICD;

using namespace Trans::ConstantTranslations;

static inline ICD::IcdDatabase *icdBase() {return ICD::IcdDatabase::instance();}

namespace ICD {
namespace Internal {

    enum QueryFields {
        Query_SID = 0,
        Query_LID
    };

class IcdSearchModelPrivate
{
public:
    IcdSearchModelPrivate(IcdSearchModel *parent) :
            m_IcdMaster(0), m_SearchMode(IcdSearchModel::SearchByLabel), q(parent)
    {
        m_IcdMaster = new QSqlQueryModel(q);
        // Master -> valid=1, level=4, code like '%search%'
        // Libelle -> XX_OMS like 'search', valid=1
        // †  -->  &#134;
    }

    QString searchQuery()
    {
        QString req;
        QString fields;
        if (m_SearchMode == IcdSearchModel::SearchByLabel) {
            fields = QString("`%1`.`%2`, `%3`.`%4` ")
                     .arg(icdBase()->table(Constants::Table_System))
                     .arg(icdBase()->field(Constants::Table_System, Constants::SYSTEM_SID))
                     .arg(icdBase()->table(Constants::Table_System))
                     .arg(icdBase()->field(Constants::Table_System, Constants::SYSTEM_LID));

            req = QString("SELECT %1 \nFROM %2 \n"
                          "LEFT JOIN %3 ON %2.%4=%3.%5 \n")
                    .arg(fields)
                    .arg(icdBase()->table(Constants::Table_System))
                    // join libelle
                    .arg(icdBase()->table(Constants::Table_Libelle))
                    .arg(icdBase()->field(Constants::Table_System, Constants::SYSTEM_LID))
                    .arg(icdBase()->field(Constants::Table_Libelle, Constants::LIBELLE_LID));
        } else {
            // Search by ICD Codes
//            SELECT `master`.`SID`, system.LID
//            FROM master, system
//            WHERE  (`master`.`code` like 'B5%') AND (system.SID=master.SID)
            fields = QString("`%1`.`%2`, `%3`.`%4` ")
                     .arg(icdBase()->table(Constants::Table_Master))
                     .arg(icdBase()->field(Constants::Table_Master, Constants::MASTER_SID))
                     .arg(icdBase()->table(Constants::Table_System))
                     .arg(icdBase()->field(Constants::Table_System, Constants::SYSTEM_LID));
            req = QString("SELECT %1 \n"
                          "FROM %2, %3  \n"
                          "WHERE %2.%5 = %3.%6 \n")
                    .arg(fields)
                    .arg(icdBase()->table(Constants::Table_Master))
                    .arg(icdBase()->table(Constants::Table_System))
                    // Where clause
                    .arg(icdBase()->field(Constants::Table_Master, Constants::MASTER_SID))
                    .arg(icdBase()->field(Constants::Table_System, Constants::SYSTEM_SID));
        }
        return req;
    }

    ~IcdSearchModelPrivate() {}

public:
    QSqlQueryModel *m_IcdMaster;
    IcdSearchModel::SearchModes m_SearchMode;
    QString m_LastFilterRequiered;

private:
    IcdSearchModel *q;
};

struct SimpleCode {
    int sid;
    QString code;
    QString dag;
    QString systemLabel;
    QStringList labels;
};

class SimpleIcdModelPrivate
{
public:
    SimpleIcdModelPrivate(SimpleIcdModel *parent) :
            m_UseDagDepend(false), m_Checkable(false), m_GetAllLabels(false), q(parent)
    {
    }

    ~SimpleIcdModelPrivate()
    {
        qDeleteAll(m_Codes);
        m_Codes.clear();
    }

public:
    QList<SimpleCode *> m_Codes;
    QList<Internal::IcdAssociation> m_Associations;
    QHash<int, QPointer<QStringListModel> > m_LabelModels;
    bool m_UseDagDepend, m_Checkable, m_GetAllLabels;
    QVariant m_DagDependOnSid;
    QList<int> m_CheckStates;

private:
    SimpleIcdModel *q;
};


}  // End namespace Internal
}  // End namespace ICD

IcdSearchModel::IcdSearchModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::IcdSearchModelPrivate(this))
{
    // connect lanquage change
    d->m_IcdMaster->setQuery(d->searchQuery(), icdBase()->database());

    connect(d->m_IcdMaster,SIGNAL(layoutChanged()), this, SIGNAL(layoutChanged()));
    connect(d->m_IcdMaster,SIGNAL(layoutAboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()));
    connect(d->m_IcdMaster,SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)), this, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    connect(d->m_IcdMaster,SIGNAL(rowsInserted(QModelIndex, int, int)), this, SIGNAL(rowsInserted(QModelIndex, int, int)));
    connect(d->m_IcdMaster,SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    connect(d->m_IcdMaster,SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    connect(d->m_IcdMaster,SIGNAL(modelAboutToBeReset()), this, SIGNAL(modelAboutToBeReset()));
    connect(d->m_IcdMaster,SIGNAL(modelReset()), this, SIGNAL(modelReset()));
}

IcdSearchModel::~IcdSearchModel()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void IcdSearchModel::setSearchMethod(SearchModes mode)
{
    if (mode==d->m_SearchMode)
        return;
    d->m_SearchMode = mode;
    // modify the query
    // update model ?
}

int IcdSearchModel::rowCount(const QModelIndex &parent) const
{
    return d->m_IcdMaster->rowCount();
}

int IcdSearchModel::columnCount(const QModelIndex &parent) const
{
    return ColumnCount;
}

void IcdSearchModel::fetchMore(const QModelIndex &parent)
{
    d->m_IcdMaster->fetchMore(parent);
}

bool IcdSearchModel::canFetchMore(const QModelIndex &parent) const
{
    return d->m_IcdMaster->canFetchMore(parent);
}

QVariant IcdSearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        switch (index.column()) {
        case SID_Code: return d->m_IcdMaster->index(index.row(), Internal::Query_SID).data();
        case ICD_Code: return icdBase()->getIcdCode(d->m_IcdMaster->index(index.row(), Internal::Query_SID).data());
        case ICD_CodeWithDagetAndStar: return icdBase()->getIcdCodeWithDagStar(d->m_IcdMaster->index(index.row(), Internal::Query_SID).data());
        case Daget: return icdBase()->getHumanReadableIcdDaget(d->m_IcdMaster->index(index.row(), Internal::Query_SID).data());
        case Label: return icdBase()->getSystemLabel(d->m_IcdMaster->index(index.row(), Internal::Query_SID).data());
        }
    }
    return QVariant();
}

bool IcdSearchModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}


QVariant IcdSearchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

bool IcdSearchModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return false;
}

bool IcdSearchModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return false;
}

void IcdSearchModel::setFilter(const QString &searchLabel)
{
    d->m_LastFilterRequiered = searchLabel;
    QHash<int, QString> where;
    QString req;
    if (d->m_SearchMode==SearchByLabel) {
        where.insert(Constants::LIBELLE_FR, QString("like '%1%'").arg(searchLabel));
        req = d->searchQuery() + " WHERE " + icdBase()->getWhereClause(Constants::Table_Libelle, where);
    } else {
        where.insert(Constants::MASTER_CODE, QString("like '%1%'").arg(searchLabel));
        req = d->searchQuery() + " AND " + icdBase()->getWhereClause(Constants::Table_Master, where);
    }
    qWarning() << req;

    d->m_IcdMaster->setQuery(req, icdBase()->database());

    qWarning()<<d->m_IcdMaster->query().lastError().text();
    reset();
}





SimpleIcdModel::SimpleIcdModel(QObject *parent) :
        QAbstractTableModel(parent), d(0)
{
    d = new Internal::SimpleIcdModelPrivate(this);
    connect(Core::ICore::instance()->translators(), SIGNAL(languageChanged()), this, SLOT(updateTranslations()));
}

SimpleIcdModel::~SimpleIcdModel()
{
    if (d)
        delete d;
    d=0;
}

void SimpleIcdModel::addCodes(const QVector<int> &codes, bool getAllLabels)
{
    if (codes.isEmpty())
        return;

    d->m_GetAllLabels = getAllLabels;

    // Construct SimpleCodes without labels
    foreach(const int sid, codes) {
        if (sid==0)
            continue;
        if (d->m_UseDagDepend) {
            d->m_Associations << icdBase()->getAssociation(d->m_DagDependOnSid, sid);
            const Internal::IcdAssociation &asso = d->m_Associations.last();
            if (asso.associationIsMandatory())
                d->m_CheckStates << Qt::Checked;
            else
                d->m_CheckStates << Qt::Unchecked;
            qWarning() << "xxxxxx" << asso.mainCodeWithDagStar() << asso.associatedCodeWithDagStar() << asso.dagCode();
            Internal::SimpleCode *code = new Internal::SimpleCode;
            d->m_Codes.append(code);
            code->sid = sid;
        } else {
            Internal::SimpleCode *code = new Internal::SimpleCode;
            code->sid = sid;
            code->code = icdBase()->getIcdCode(sid).toString();
            code->dag = icdBase()->getHumanReadableIcdDaget(sid);
            d->m_Codes.append(code);
        }
    }

    // Get labels and reset model
    updateTranslations();
}

void SimpleIcdModel::setUseDagDependencyWithSid(const QVariant &SID)
{
    if (!SID.isNull())
        d->m_UseDagDepend = true;
    else
        d->m_UseDagDepend = false;
    d->m_DagDependOnSid = SID;
}

void SimpleIcdModel::setCheckable(bool state)
{
    d->m_Checkable = state;
    reset();
}

int SimpleIcdModel::rowCount(const QModelIndex &parent) const
{
    return d->m_Codes.count();
}

int SimpleIcdModel::columnCount(const QModelIndex &parent) const
{
    return ColumnCount;
}

QVariant SimpleIcdModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= d->m_Codes.count()) {
        return QVariant();
    }

    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        if (d->m_UseDagDepend) {
            const Internal::IcdAssociation &asso = d->m_Associations.at(index.row());
            switch (index.column()) {
            case SID_Code: return asso.associatedSid();
            case ICD_Code: return asso.associatedCode();
            case ICD_CodeWithDagetAndStar: return asso.associatedCodeWithDagStar();
            case Daget: if (asso.associatedIsDag()) return "†"; else return "*";
            case Label: return icdBase()->getSystemLabel(asso.associatedSid());
            }
        } else {
            Internal::SimpleCode *code = d->m_Codes.at(index.row());
            switch (index.column()) {
            case SID_Code: return code->sid;
            case ICD_Code: return code->code;
            case ICD_CodeWithDagetAndStar: return code->code + code->dag;
            case Daget: return code->dag;
            case Label: return code->systemLabel;
            }
        }
    } else if (role==Qt::CheckStateRole && d->m_Checkable &&
               (index.column()==ICD_Code || index.column()==ICD_CodeWithDagetAndStar)) {
        return d->m_CheckStates.at(index.row());
    }

    return QVariant();
}

bool SimpleIcdModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!d->m_Checkable)
        return false;
    if (!index.isValid())
        return false;
    if (index.row() < 0 || index.row() >= d->m_Codes.count())
        return false;
    if (role!=Qt::CheckStateRole)
        return false;

    d->m_CheckStates[index.row()] = value.toInt();
    Q_EMIT dataChanged(index, index);
    return true;
}

Qt::ItemFlags SimpleIcdModel::flags(const QModelIndex &index) const
{
    if (d->m_Checkable &&
        (index.column()==ICD_CodeWithDagetAndStar ||
         index.column()==ICD_Code)) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QStringListModel *SimpleIcdModel::labelsModel(const QModelIndex &index)
{
    if (!index.isValid())
        return 0;

    if (index.row() < 0 || index.row() >= d->m_Codes.count()) {
        return 0;
    }

    QStringListModel *model = d->m_LabelModels[index.row()];
    if (!model) {
        model = new QStringListModel(this);
        d->m_LabelModels.insert(index.row(), model);
    }
    Internal::SimpleCode *code = d->m_Codes.at(index.row());
    // Get system label
    QStringList list;
    list << code->systemLabel;
    foreach(const QString &label, code->labels) {
        if (label==code->systemLabel)
            continue;
        list << label;
    }
    model->setStringList(list);

    return model;
}

QVariant SimpleIcdModel::headerData(int section, Qt::Orientation orientation,
                            int role) const
{
    return QVariant();
}

 void SimpleIcdModel::updateTranslations()
 {
     // Translate codes labels
     foreach(Internal::SimpleCode *code, d->m_Codes) {
         code->systemLabel = icdBase()->getSystemLabel(code->sid);
         // get all language labels
         if (d->m_GetAllLabels) {
             code->labels.clear();
             foreach(const QString &label, icdBase()->getAllLabels(code->sid)) {
                 if (!label.isEmpty())
                     code->labels << label;
             }
         }
     }

     // Update labelsModels foreach row
     foreach(const int id, d->m_LabelModels.keys()) {
         QStringListModel *model = d->m_LabelModels[id];
         if (!model) {
             continue;
         }
         Internal::SimpleCode *code = d->m_Codes.at(id);
         // Get system label
         QStringList list;
         list << code->systemLabel;
         foreach(const QString &label, code->labels) {
             if (label==code->systemLabel)
                 continue;
             list << label;
         }
         model->setStringList(list);
     }

     reset();
 }
