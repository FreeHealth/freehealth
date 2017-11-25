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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "druginteractortreeproxymodel.h"
#include "druginteractortablemodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QString>
#include <QStandardItem>
#include <QPointer>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

namespace {
const int ENGLISH = Qt::UserRole + 1;
const int FRENCH = ENGLISH + 1;
const int SPANISH = FRENCH + 1;
const int CODE = SPANISH + 1;
}  // anymous namespace

namespace DDI {
namespace Internal {
class DrugInteractorTreeProxyModelPrivate
{
public:
    DrugInteractorTreeProxyModelPrivate(DrugInteractorTreeProxyModel *parent) :
        _sourceModel(0),
        q(parent)
    {
        bold.setBold(true);
    }

    ~DrugInteractorTreeProxyModelPrivate()
    {}

    QModelIndex toSourceIndex(const QModelIndex &index)
    {
        // Map to Code column
        QModelIndex code = q->index(index.row(), DrugInteractorTreeProxyModel::Code, index.parent());
        return _toSourceIndex.value(q->itemFromIndex(code), QModelIndex());
    }

    bool getTree(DrugInteractorTableModel *sourceModel)
    {
        _toSourceIndex.clear();
        _sourceModel = sourceModel;

        // Fetch all data in the source model
        while (_sourceModel->canFetchMore(_sourceModel->index(0, _sourceModel->rowCount())))
            _sourceModel->fetchMore(_sourceModel->index(0, _sourceModel->rowCount()));

        // Sort?

        // Read line by line
        QStandardItem *noClass = new QStandardItem("NO_CLASS");
        QList<QStandardItem*> cols;
        cols << noClass;
        cols << new QStandardItem(QString("No class"));
        cols << new QStandardItem(QString("No class"));
        cols << new QStandardItem(QString("No class"));
        q->invisibleRootItem()->appendRow(cols);
        cols.clear();
        for(int i=0; i < _sourceModel->rowCount(); ++i) {
            QModelIndex uid = _sourceModel->index(i, DrugInteractorTableModel::Uuid);
            QModelIndex fr = _sourceModel->index(i, DrugInteractorTableModel::FrLabel);
            QModelIndex en = _sourceModel->index(i, DrugInteractorTableModel::EnLabel);
            QModelIndex de = _sourceModel->index(i, DrugInteractorTableModel::DeLabel);
            QModelIndex isClass = _sourceModel->index(i, DrugInteractorTableModel::IsInteractingClass);

            QStandardItem *item = new QStandardItem(_sourceModel->data(uid).toString());
            _toSourceIndex.insert(item, uid);
            cols << item;
            cols <<  new QStandardItem(_sourceModel->data(en).toString());
            cols <<  new QStandardItem(_sourceModel->data(fr).toString());
            cols <<  new QStandardItem(_sourceModel->data(de).toString());
            if (_sourceModel->data(isClass).toBool()) {
                // is interacting class --> row to bold
                foreach(QStandardItem *item, cols)
                    item->setFont(bold);
                // get all children and add them to the branch
                QModelIndex childrenIndex = _sourceModel->index(i, DrugInteractorTableModel::ChildrenUuid);
                QStringList children = _sourceModel->data(childrenIndex).toString().split(";");
                foreach(const QString &child, children) {
                    QList<QStandardItem*> childrenItem;
                    QModelIndexList list = _sourceModel->match(_sourceModel->index(0, DrugInteractorTableModel::Uuid), Qt::DisplayRole, child);
                    if (list.count() == 0) {
                        // _toSourceIndex.insert(childUidItem, uid);
                        qWarning() << "Child not found" << _sourceModel->data(fr).toString() << child;
                        childrenItem << new QStandardItem(child);
                        childrenItem << new QStandardItem(QString("Error child not found: %1").arg(child));
                        childrenItem << new QStandardItem(QString("Error child not found: %1").arg(child));
                        childrenItem << new QStandardItem(QString("Error child not found: %1").arg(child));
                    } else if (list.count() > 1) {
                        qWarning() << "More than one child found" << _sourceModel->data(fr).toString() << child;
                        childrenItem << new QStandardItem(child);
                        childrenItem << new QStandardItem(QString("Error more than one child found: %1").arg(child));
                        childrenItem << new QStandardItem(QString("Error more than one child found: %1").arg(child));
                        childrenItem << new QStandardItem(QString("Error more than one child found: %1").arg(child));
                    } else {
                        int i = list.at(0).row();
                        QModelIndex childUid = _sourceModel->index(i, DrugInteractorTableModel::Uuid);
                        QModelIndex childFr = _sourceModel->index(i, DrugInteractorTableModel::FrLabel);
                        QModelIndex childEn = _sourceModel->index(i, DrugInteractorTableModel::EnLabel);
                        QModelIndex childDe = _sourceModel->index(i, DrugInteractorTableModel::DeLabel);
                        QStandardItem *childUidItem = new QStandardItem(_sourceModel->data(childUid).toString());
                        _toSourceIndex.insert(childUidItem, uid);
                        childrenItem << childUidItem;
                        childrenItem <<  new QStandardItem(_sourceModel->data(childEn).toString());
                        childrenItem <<  new QStandardItem(_sourceModel->data(childFr).toString());
                        childrenItem <<  new QStandardItem(_sourceModel->data(childDe).toString());
                    }
                    item->appendRow(childrenItem);
                }
                q->invisibleRootItem()->appendRow(cols);
//            } else {
//                noClass->appendRow(cols);
            }
            cols.clear();
        }
        return true;
    }

public:
    QFont bold;
    QPointer<DrugInteractorTableModel> _sourceModel;
    QHash<QStandardItem *, QPersistentModelIndex> _toSourceIndex;

private:
    DrugInteractorTreeProxyModel *q;
};

}  // namespace Internal
}  // namespace DDI

DrugInteractorTreeProxyModel::DrugInteractorTreeProxyModel(QObject * parent) :
    QStandardItemModel(parent),
    d(new DrugInteractorTreeProxyModelPrivate(this))
{
}

DrugInteractorTreeProxyModel::~DrugInteractorTreeProxyModel()
{}

/** Initialize ATC Model. Read the ATC CSV file in the global_resources */
bool DrugInteractorTreeProxyModel::initialize(DrugInteractorTableModel *sourceModel)
{
    d->getTree(sourceModel);
    return true;
}

Qt::ItemFlags DrugInteractorTreeProxyModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant DrugInteractorTreeProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Code: return tr("Code");
        // case Uid: return tr("Uid");
        case LabelEn: return tr("English");
        case LabelFr: return tr("French");
        case LabelDe: return tr("Deustch");
        // case LabelSp: return tr("Spanish");
        }
    }
    return QVariant();
}

QModelIndex DrugInteractorTreeProxyModel::toSourceIndex(const QModelIndex &index) const
{
    return d->toSourceIndex(index);
}
