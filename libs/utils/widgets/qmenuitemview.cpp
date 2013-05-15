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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Utils::QMenuItemView
 * Connect a QMenu to a QAbstractItemModel. The model can be a list/tree model.
 */

#include "qmenuitemview.h"

#include <QAction>
#include <QAbstractItemModel>

#include <QDebug>

Q_DECLARE_METATYPE(QModelIndex)

using namespace Utils;
using namespace Internal;

namespace Utils {
namespace Internal {
class QMenuItemViewPrivate
{
public:
    QMenuItemViewPrivate(QMenuItemView *parent) :
        _model(0),
        q(parent)
    {
    }
    
    ~QMenuItemViewPrivate()
    {
    }

    QAction *createAction(const QModelIndex &index)
    {
        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        QAction *action = new QAction(icon, index.data().toString(), q);
        action->setEnabled(index.flags().testFlag(Qt::ItemIsEnabled));
        QVariant v;
        v.setValue(index);
        action->setData(v);
        return action;
    }

    void createMenu(const QModelIndex &parent, QMenu *parentMenu, QMenu *menu = 0)
    {
        if (!menu) {
            QIcon icon = parent.data(Qt::DecorationRole).value<QIcon>();
            QVariant v;
            v.setValue(parent);

            menu = new QMenu(parent.data().toString(), q);
            menu->setIcon(icon);
            parentMenu->addMenu(menu);
            menu->menuAction()->setData(v);
            menu->setEnabled(parent.flags().testFlag(Qt::ItemIsEnabled));
            // Force model fetching
            while (_model->canFetchMore(parent))
                _model->fetchMore(parent);
            QObject::connect(menu, SIGNAL(aboutToShow()), q, SLOT(aboutToShow()));
            return;
        }

        int end = _model->rowCount(parent);
        for (int i = 0; i < end; ++i) {
            QModelIndex idx = _model->index(i, 0, parent);
            if (_model->hasChildren(idx)) {
                createMenu(idx, menu);
            } else {
                menu->addAction(createAction(idx));
            }
        }
    }

public:
    QAbstractItemModel *_model;
    QPersistentModelIndex _rootIndex;

private:
    QMenuItemView *q;
};
} // namespace Internal
} // namespace Utils


/*! Constructor of the Utils::QMenuItemView class */
QMenuItemView::QMenuItemView(QWidget *parent) :
    QMenu(parent),
    d(new QMenuItemViewPrivate(this))
{
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(triggered(QAction*)));
    connect(this, SIGNAL(hovered(QAction*)), this, SLOT(hovered(QAction*)));
    connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
}

/*! Destructor of the Utils::QMenuItemView class */
QMenuItemView::~QMenuItemView()
{
    if (d)
        delete d;
    d = 0;
}

/** Define the QAbstractItemModel to use to build the menu (can be a tree/list model) */
void QMenuItemView::setModel(QAbstractItemModel *model)
{
    d->_model = model;
}

/** Return the current model in use */
QAbstractItemModel *QMenuItemView::model() const
{
    return d->_model;
}

/** Define the root index \e index to use to generate the menu tree */
void QMenuItemView::setRootIndex(const QModelIndex & index)
{
    d->_rootIndex = index;
}

/** Returns the root index in use */
QModelIndex QMenuItemView::rootIndex() const
{
    return d->_rootIndex;
}

void QMenuItemView::triggered(QAction *action)
{
    QVariant v = action->data();
    if (v.canConvert<QModelIndex>()) {
        QModelIndex idx = qvariant_cast<QModelIndex>(v);
        Q_EMIT triggered(idx);
    }
}

void QMenuItemView::hovered(QAction *action)
{
    QVariant v = action->data();
    if (v.canConvert<QModelIndex>()) {
        QModelIndex idx = qvariant_cast<QModelIndex>(v);
        QString hoveredString = idx.data(Qt::StatusTipRole).toString();
        if (!hoveredString.isEmpty())
            Q_EMIT hovered(hoveredString);
    }
}

void QMenuItemView::aboutToShow()
{
    QMenu *menu = qobject_cast<QMenu*>(sender());
    qWarning() << "AboutToShow" << menu;
    if (menu) {
        QVariant v = menu->menuAction()->data();
        qWarning() << " AboutToShow" << v;
        if (v.canConvert<QModelIndex>()) {
            QModelIndex idx = qvariant_cast<QModelIndex>(v);
            d->createMenu(idx, menu, menu);
            disconnect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
            return;
        }
    }

    clear();
    d->createMenu(d->_rootIndex, this, this);
}
