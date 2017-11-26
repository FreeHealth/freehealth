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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "fullicdcodemodel.h"
#include "simpleicdmodel.h"
#include "icddatabase.h"
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
#include <QStandardItemModel>

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
            m_CodeTreeModel(0), m_LabelModel(0), m_ExcludeModel(0), m_DagStarModel(0), m_Include(0),
            q(parent)
    {
    }

    ~FullIcdCodeModelPrivate()
    {
    }

    void createCodeTreeModel(const QVariant &SID)
    {
        if (!m_CodeTreeModel) {
            m_CodeTreeModel = new QStandardItemModel(0, 1, q);
        } else {
            m_CodeTreeModel->clear();
        }
        // Get all ids of the code
        QList<int> ids = icdBase()->getHeadersSID(SID); // get all ids from 1 to 7
        QStandardItem *parentItem = m_CodeTreeModel->invisibleRootItem();
        QFont bold;
        bold.setBold(true);
        QString systemLabel;
        foreach(int id, ids) {
            if (id == 0)
                break;
            systemLabel = icdBase()->getSystemLabel(id);
            QString title = QString("%1 - %2")
                            .arg(icdBase()->getIcdCode(id).toString())
                            .arg(systemLabel);
            QStandardItem *item = new QStandardItem(title);
            item->setToolTip(title);
            parentItem->appendRow(item);
            item->setFont(bold);
            parentItem = item;
        }
        // Append all other labels
        foreach(const QString &label, icdBase()->getAllLabels(SID)) {
            if (label.isEmpty())
                continue;
            if (label==systemLabel)
                continue;
            QString title = QString("%1 - %2")
                            .arg(icdBase()->getIcdCode(SID).toString())
                            .arg(label);
            QStandardItem *item = new QStandardItem(label);
            item->setToolTip(title);
            parentItem->appendRow(item);
        }
    }

public:
    QStandardItemModel *m_CodeTreeModel;
    SimpleIcdModel *m_LabelModel;
    SimpleIcdModel *m_ExcludeModel;
    SimpleIcdModel *m_DagStarModel;
    QStringListModel *m_Include;
    QVariant m_SID;

private:
    FullIcdCodeModel *q;
};


}  // End namespace Internal
}  // End namespace ICD


FullIcdCodeModel::FullIcdCodeModel(QObject *parent) :
        QAbstractTableModel(parent), d(0)
{
    d = new Internal::FullIcdCodeModelPrivate(this);
    connect(Core::ICore::instance()->translators(), SIGNAL(languageChanged()), this, SLOT(updateTranslations()));
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

    d->m_SID = SID;

    // Create the labels model
    d->m_LabelModel = new SimpleIcdModel(this);  // auto-translations
    d->m_LabelModel->addCodes(QVector<int>() << SID);  // add true if you want the combolabel to show all available labels

    // Create the Include model
    d->m_Include = new QStringListModel(this);

    // Create the Exclude model
    d->m_ExcludeModel = new SimpleIcdModel(this);  // auto-translations
    d->m_ExcludeModel->addCodes(icdBase()->getExclusions(SID));

    // Get dagstar
    d->m_DagStarModel = new SimpleIcdModel(this);  // auto-translations
    d->m_DagStarModel->setUseDagDependencyWithSid(SID);
    d->m_DagStarModel->setCheckable(true);
    d->m_DagStarModel->addCodes(icdBase()->getDagStarDependencies(SID));

    // Get note/memo --> \sa data()

    // Update labels/translations and reset the model
    updateTranslations();
}

QVariant FullIcdCodeModel::getCodeSid() const
{
    return d->m_SID;
}

bool FullIcdCodeModel::codeCanBeUsedAlone() const
{
    return icdBase()->codeCanBeUsedAlone(d->m_SID);
}

bool FullIcdCodeModel::codeMustBeAssociated() const
{
    return (!icdBase()->codeCanBeUsedAlone(d->m_SID));
}

bool FullIcdCodeModel::isSelectionValid() const
{
    // Usable alone ?
    if (icdBase()->codeCanBeUsedAlone(d->m_SID))
        return true;
    // Must be associated -> got associated codes ?
    if (d->m_DagStarModel->numberOfCheckedItems()>=1)
        return true;

    return false;
}

int FullIcdCodeModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    // TODO: code here: check this
    return 1;
}

int FullIcdCodeModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant FullIcdCodeModel::data(const QModelIndex &index, int role) const
{
    if (index.column()==Memo && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        return icdBase()->getMemo(d->m_SID);
    }
    return d->m_LabelModel->data(index, role);
}

QStringListModel *FullIcdCodeModel::labelsModel()
{
    return d->m_LabelModel->labelsModel(d->m_LabelModel->index(0,0));
}

QAbstractItemModel *FullIcdCodeModel::codeTreeModel()
{
    return d->m_CodeTreeModel;
}

QStringListModel *FullIcdCodeModel::includedLabelsModel()
{
    return d->m_Include;
}

QAbstractItemModel *FullIcdCodeModel::excludedModel()
{
    return d->m_ExcludeModel;
}

SimpleIcdModel *FullIcdCodeModel::dagStarModel()
{
    return d->m_DagStarModel;
}

//QVariant FullIcdCodeModel::headerData(int section, Qt::Orientation orientation,
//                            int role) const
//{
//    return QVariant();
//}

void FullIcdCodeModel::updateTranslations()
{
    beginResetModel();
    // Create code tree model according to the language
    d->createCodeTreeModel(d->m_SID);

    // Add stringlists to model according to the language
    d->m_Include->setStringList(icdBase()->getIncludedLabels(d->m_SID));
    endResetModel();
}
