/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "simpleicdmodel.h"
#include "icddatabase.h"
#include "icdassociation.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <translationutils/constanttranslations.h>

#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QLocale>
#include <QPointer>
#include <QHash>
#include <QList>
#include <QVariant>

#include <QDebug>

enum { WarnFilter=true };

using namespace ICD;

using namespace Trans::ConstantTranslations;

static inline ICD::IcdDatabase *icdBase() {return ICD::IcdDatabase::instance();}

namespace ICD {
namespace Internal {

    enum QueryFields {
        Query_SID = 0,
        Query_LID
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
    SimpleIcdModelPrivate(SimpleIcdModel */*parent*/) :
        m_UseDagDepend(false),
        m_Checkable(false),
        m_GetAllLabels(false) //, q(parent)
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
    QVariant m_DagMainSid;
    QList<int> m_CheckStates;

private:
//    SimpleIcdModel *q;
};


}  // End namespace Internal
}  // End namespace ICD



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
            // Get association
            const Internal::IcdAssociation &asso = icdBase()->getAssociation(d->m_DagMainSid, sid);

            // If association already exists ? --> continue;
            bool alreadyIncluded = false;
            foreach(const Internal::IcdAssociation &recAsso, d->m_Associations) {
                if (recAsso.associatedSid().toInt()==sid) {
                    alreadyIncluded = true;
                    continue;
                }
            }
            if (alreadyIncluded)
                continue;

            // Include association, manage checkstate
            d->m_Associations << asso;
            if (asso.associationIsMandatory())
                d->m_CheckStates << Qt::Checked;
            else
                d->m_CheckStates << Qt::Unchecked;
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
    d->m_DagMainSid = SID;
}

void SimpleIcdModel::setCheckable(bool state)
{
    beginResetModel();
    d->m_Checkable = state;
    endResetModel();
}

int SimpleIcdModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return d->m_Codes.count();
}

int SimpleIcdModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
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
            case Label: return asso.associatedLabel();
            }
        } else {
            Internal::SimpleCode *code = d->m_Codes.at(index.row());
            switch (index.column()) {
            case SID_Code: return code->sid;
            case ICD_Code: return code->code;
            case ICD_CodeWithDagetAndStar: return QString(code->code + code->dag);
            case Daget: return code->dag;
            case Label: return code->systemLabel;
            }
        }
    } else if (role==Qt::CheckStateRole && d->m_Checkable &&
               (index.column()==ICD_Code || index.column()==ICD_CodeWithDagetAndStar)) {
        return d->m_CheckStates.at(index.row());
    } else if (role==Qt::ToolTipRole) {
        if (d->m_UseDagDepend) {
            const Internal::IcdAssociation &asso = d->m_Associations.at(index.row());
            return QString(asso.associatedCodeWithDagStar() + " - " + asso.associatedLabel());
        } else {
            Internal::SimpleCode *code = d->m_Codes.at(index.row());
            return QString(code->code + code->dag + " - " + code->systemLabel);
        }
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

QVector<QVariant> SimpleIcdModel::getCheckedSids() const
{
    if (!d->m_Checkable)
        return QVector<QVariant>();

    QVector<QVariant> toReturn;
    for(int i=0; i < d->m_CheckStates.count(); ++i) {
        if (d->m_CheckStates.at(i)==Qt::Checked) {
            const Internal::SimpleCode *code = d->m_Codes.at(i);
            toReturn << code->sid;
        }
    }
    return toReturn;
}

QVector<Internal::IcdAssociation> SimpleIcdModel::getCheckedAssociations() const
{
    QVector<Internal::IcdAssociation> toReturn;
    if (!d->m_Checkable)
        return toReturn;
    if (!d->m_UseDagDepend)
        return toReturn;

    for(int i = 0; i < d->m_CheckStates.count(); ++i) {
        if (d->m_CheckStates.at(i)==Qt::Checked) {
            toReturn << d->m_Associations.at(i);
        }
    }
    return toReturn;
}

int SimpleIcdModel::numberOfCheckedItems() const
{
    if (!d->m_Checkable)
        return 0;
    int toReturn = 0;
    for(int i=0; i < d->m_CheckStates.count(); ++i) {
        if (d->m_CheckStates.at(i)==Qt::Checked) {
            ++toReturn;
        }
    }
    return toReturn;
}

//QVariant SimpleIcdModel::headerData(int section, Qt::Orientation orientation,
//                            int role) const
//{
//    return QVariant();
//}

 void SimpleIcdModel::updateTranslations()
 {
     beginResetModel();
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
     endResetModel();
 }
