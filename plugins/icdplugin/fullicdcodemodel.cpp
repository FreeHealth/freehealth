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
#include "fullicdcodemodel.h"
#include "icdmodel.h"
#include "icddatabase.h"
#include "constants.h"

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

class FullIcdCodeModelPrivate
{
public:
    FullIcdCodeModelPrivate(FullIcdCodeModel *parent) :
            q(parent), m_LabelModel(0), m_ExcludeModel(0), m_DagStarModel(0)
    {
    }

    ~FullIcdCodeModelPrivate()
    {
    }

public:
    SimpleIcdModel *m_LabelModel;
    SimpleIcdModel *m_ExcludeModel;
    SimpleIcdModel *m_DagStarModel;
    QStringList m_Include;

private:
    FullIcdCodeModel *q;
};


}  // End namespace Internal
}  // End namespace ICD


FullIcdCodeModel::FullIcdCodeModel(QObject *parent) :
        QAbstractTableModel(parent), d(0)
{
    d = new Internal::FullIcdCodeModelPrivate(this);
}

FullIcdCodeModel::~FullIcdCodeModel()
{
    if (d)
        delete d;
    d=0;
}

void FullIcdCodeModel::setCode(const int SID)
{
    if (SID < 0)
        return;

    // Create the Label model
    d->m_LabelModel = new SimpleIcdModel(this);
    d->m_LabelModel->addCodes(QVector<int>() << SID, true);

    // Create the Include model
    d->m_Include = icdBase()->getIncludedLabels(SID);

    // Create the Exclude model
    d->m_ExcludeModel = new SimpleIcdModel(this);
    d->m_ExcludeModel->addCodes(icdBase()->getExclusions(SID));

    // Get dagstar
    d->m_DagStarModel = new SimpleIcdModel(this);
    d->m_DagStarModel->setUseDagDependencyWithSid(SID);
    d->m_DagStarModel->setCheckable(true);
    d->m_DagStarModel->addCodes(icdBase()->getDagStarDependencies(SID));

    // Get note/memo --> sa data()
    reset();
}

int FullIcdCodeModel::rowCount(const QModelIndex &parent) const
{
    return 1;
}

int FullIcdCodeModel::columnCount(const QModelIndex &parent) const
{
    return ColumnCount;
}

QVariant FullIcdCodeModel::data(const QModelIndex &index, int role) const
{
    if (index.column()==Memo && (role == Qt::DisplayRole || Qt::EditRole)) {
        QModelIndex sid_idx = d->m_LabelModel->index(0, SimpleIcdModel::SID_Code);
        return icdBase()->getMemo(d->m_LabelModel->data(sid_idx));
    }
    return d->m_LabelModel->data(index, role);
}

QStringListModel *FullIcdCodeModel::labelsModel()
{
    return d->m_LabelModel->labelsModel(d->m_LabelModel->index(0,0));
}

QStringListModel *FullIcdCodeModel::includedLabelsModel()
{
    static QPointer<QStringListModel> model = 0;
    if (!model) {
        model = new QStringListModel(this);
    }
    model->setStringList(d->m_Include);
    return model;
}

QAbstractItemModel *FullIcdCodeModel::excludedModel()
{
    return d->m_ExcludeModel;
}

QAbstractItemModel *FullIcdCodeModel::dagStarModel()
{
    return d->m_DagStarModel;
}

QVariant FullIcdCodeModel::headerData(int section, Qt::Orientation orientation,
                            int role) const
{
    return QVariant();
}
