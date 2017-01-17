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
#include "uitools.h"
#include "scriptwrappers.h"

#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/iformio.h>
#include <formmanagerplugin/formmanager.h>

#include <utils/log.h>
#include <utils/widgets/imageviewer.h>

#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <QListView>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>

#include <QDebug>

using namespace Script;
using namespace Internal;

static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}

UiTools::UiTools(QObject *parent) :
    QObject(parent)
{
    setObjectName("Script::UiTools");
}

static void warnObject(QObject *o, const QString &indent)
{
    qWarning() << indent << o;
    foreach(QObject *obj, o->children()) {
        warnObject(obj, indent+"  ");
    }
}

/** Provided only for debugging purpose. */
void UiTools::printQObjectChildrenList(QObject *obj)
{
    foreach(QObject *o, obj->children()) {
        warnObject(o, "");
    }
}

/** Add item to QComboBox / QListWidget */
bool UiTools::addItem(QWidget *widget, const QString &item)
{
    // QListWidget
    QListWidget *listWidget = qobject_cast<QListWidget*>(widget);
    if (listWidget) {
        listWidget->addItem(item);
        return true;
    }
    // QComboBox
    QComboBox *combo = qobject_cast<QComboBox*>(widget);
    if (combo) {
        combo->addItem(item);
        return true;
    }
    return false;
}

/** Add items to QComboBox / QListWidget */
bool UiTools::addItems(QWidget *widget, const QStringList &items)
{
    // QListWidget
    QListWidget *listWidget = qobject_cast<QListWidget*>(widget);
    if (listWidget) {
        listWidget->addItems(items);
        return true;
    }
    // QComboBox
    QComboBox *combo = qobject_cast<QComboBox*>(widget);
    if (combo) {
        combo->addItems(items);
        return true;
    }
    return false;
}

bool UiTools::addJoinedItem(QWidget *widget, const QString &item, const QString &separator)
{
    QStringList items = item.split(separator);
    return addItems(widget, items);
}

bool UiTools::setItemText(QWidget *widget, const int row, const QString &item)
{
    QAbstractItemView *view = qobject_cast<QAbstractItemView*>(widget);
    if (view) {
        QAbstractItemModel *model = (QAbstractItemModel*)view->model();
        if (model) {
            QModelIndex idx = model->index(row, 0);
            model->setData(idx, item, Qt::DisplayRole);
            return true;
        }
    }
    QComboBox *combo = qobject_cast<QComboBox*>(widget);
    if (combo) {
        combo->setItemText(row, item);
        return true;
    }
    return false;
}

bool UiTools::clear(QWidget *widget)
{
    // QListWidget
    QAbstractItemView *view = qobject_cast<QAbstractItemView*>(widget);
    if (view) {
        if (view->model()) {
            view->model()->removeRows(0, view->model()->rowCount());
            return true;
        }
    }
    // QComboBox
    QComboBox *combo = qobject_cast<QComboBox*>(widget);
    if (combo) {
        combo->clear();
        return true;
    }
    return false;
}

/**
 * Return the list of the selected items in a list that can be represented by:
 * - QComboBox / FormItem Combobox
 * - QAbstractItemView / FormItem lists
 */
QStringList UiTools::selectedItems(QWidget *widget)
{
    QStringList toReturn;
//    // QListWidget
//    QListWidget *listWidget = qobject_cast<QListWidget*>(widget);
//    if (listWidget) {
//        if (listWidget->selectionModel()->hasSelection()) {
//            QModelIndexList sel = listWidget->selectionModel()->selectedIndexes();
//            foreach(const QModelIndex &index, sel) {
//                toReturn << index.data();
//            }
//        }
//        return toReturn;
//    }
    // QListView
    QAbstractItemView *listView = qobject_cast<QAbstractItemView*>(widget);
    if (listView) {
        if (listView->selectionModel()->hasSelection()) {
            QModelIndexList sel = listView->selectionModel()->selectedIndexes();
            qSort(sel);
            foreach(const QModelIndex &index, sel) {
                toReturn << index.data().toString();
            }
        }
        return toReturn;
    }
    // QComboBox
    QComboBox *combo = qobject_cast<QComboBox*>(widget);
    if (combo) {
        toReturn << combo->currentText();
        return toReturn;
    }
    return toReturn;
}

/**
 * Get and show the screenshot for the FormMain \e formUid with the
 * filename \e fileName
 */
void UiTools::showScreenshot(const QString &formUid, const QString &fileName) const
{
    QPixmap pix = formManager().getScreenshot(formUid, fileName);
    Utils::ImageViewer viewer;
    viewer.setPixmap(pix);
    viewer.exec();
}
