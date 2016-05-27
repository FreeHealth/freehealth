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
#ifndef SCRIPT_UITOOLS_H
#define SCRIPT_UITOOLS_H

#include <QObject>
#include <QStringList>
class QWidget;
class QScriptValue;

/**
 * \file uitools.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.6
 * \date 01 Jun 2012
*/

namespace Script {
namespace Internal {

class FormItemScriptWrapper;

class UiTools : public QObject
{
    Q_OBJECT
public:
    explicit UiTools(QObject *parent = 0);

Q_SIGNALS:

public Q_SLOTS:
    void printQObjectChildrenList(QObject *obj);

    bool addItem(QWidget *widget, const QString &item);
    bool addItems(QWidget *widget, const QStringList &items);
    bool addJoinedItem(QWidget *widget, const QString &item, const QString &separator);

    bool setItemText(QWidget *widget, const int row, const QString &item);

    bool clear(QWidget *widget);

    QStringList selectedItems(QWidget *widget);

    void showScreenshot(const QString &formUid, const QString &fileName) const;

};

}  // namespace Internal
}  // namespace Script

#endif // SCRIPT_UITOOLS_H
