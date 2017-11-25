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
#include "atctreeproxymodel.h"
#include "atctablemodel.h"

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
class AtcTreeProxyModelPrivate
{
public:
    AtcTreeProxyModelPrivate(AtcTreeProxyModel *parent) :
        _sourceModel(0),
        q(parent)
    {
        bold.setBold(true);
    }

    ~AtcTreeProxyModelPrivate()
    {}

    // Get the source model index according to this proxymodel index
    QModelIndex toSourceIndex(const QModelIndex &index)
    {
        // Map to Code column
        QModelIndex code = q->index(index.row(), AtcTreeProxyModel::Code, index.parent());
        return _toSourceIndex.value(q->itemFromIndex(code), QModelIndex());
    }

    // Clean the model (remove all cached data)
    // This will cause the model to be cleared (and resetted)
    void clearInternalCache()
    {
        _toSourceIndex.clear();
        q->clear();
    }

    // Create the ATC tree items
    bool getTree(AtcTableModel *sourceModel)
    {
        clearInternalCache();
        _sourceModel = sourceModel;

        // Fetch all data in the source model
        while (_sourceModel->canFetchMore(_sourceModel->index(0, _sourceModel->rowCount())))
            _sourceModel->fetchMore(_sourceModel->index(0, _sourceModel->rowCount()));

        // Source model should be correctly sorted, so we don't need to sort.
        // Read line by line
        QStandardItem *lastOne = 0;
        QStandardItem *lastThree = 0;
        QStandardItem *lastFour = 0;
        QStandardItem *lastFive = 0;
        QStandardItem *parent = 0;
        QList<QStandardItem*> cols;
        for(int i=0; i < _sourceModel->rowCount(); ++i) {
            cols.clear();
            QModelIndex code = _sourceModel->index(i, AtcTableModel::Code);
            QModelIndex fr = _sourceModel->index(i, AtcTableModel::LabelFr);
            QModelIndex en = _sourceModel->index(i, AtcTableModel::LabelEn);
            QModelIndex de = _sourceModel->index(i, AtcTableModel::LabelDe);

            QStandardItem *item = new QStandardItem(_sourceModel->data(code).toString());
            _toSourceIndex.insert(item, code);
            cols << item;
            cols <<  new QStandardItem(_sourceModel->data(en).toString());
            cols <<  new QStandardItem(_sourceModel->data(fr).toString());
            cols <<  new QStandardItem(_sourceModel->data(de).toString());
            if (_sourceModel->data(code).toString().size() == 1) {
                foreach(QStandardItem *item, cols)
                    item->setFont(bold);
            }

            // Find the parent item
            switch (_sourceModel->data(code).toString().count()) {
            case 1: parent = 0; lastOne = item; break;
            case 3: parent = lastOne;    lastThree = item; break;
            case 4: parent = lastThree;  lastFour = item;  break;
            case 5: parent = lastFour;   lastFive = item;  break;
            case 7: parent = lastFive;    break;
            }
            // need to be reparented
            if (!parent)
                parent = q->invisibleRootItem();
            parent->appendRow(cols);
            parent = 0;
        }
        return true;
    }

public:
    QFont bold;
    QPointer<AtcTableModel> _sourceModel;
    QHash<QStandardItem *, QPersistentModelIndex> _toSourceIndex;

private:
    AtcTreeProxyModel *q;
};

}  // namespace Internal
}  // namespace DDI

AtcTreeProxyModel::AtcTreeProxyModel(QObject * parent) :
    QStandardItemModel(parent),
    d(new AtcTreeProxyModelPrivate(this))
{
}

AtcTreeProxyModel::~AtcTreeProxyModel()
{}

/** Initialize ATC Model. Read the ATC CSV file in the global_resources */
bool AtcTreeProxyModel::initialize(AtcTableModel *sourceModel)
{
    d->getTree(sourceModel);
    connect(sourceModel, SIGNAL(modelReset()), this, SLOT(onInternalModelReset()));
    return true;
}

Qt::ItemFlags AtcTreeProxyModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant AtcTreeProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QModelIndex AtcTreeProxyModel::toSourceIndex(const QModelIndex &index) const
{
    return d->toSourceIndex(index);
}

void AtcTreeProxyModel::onInternalModelReset()
{
    initialize(d->_sourceModel);
}
