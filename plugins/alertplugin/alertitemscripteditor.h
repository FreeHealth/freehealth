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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERT_INTERNAL_ALERTITEMSCRIPTEDITOR_H
#define ALERT_INTERNAL_ALERTITEMSCRIPTEDITOR_H

#include <alertplugin/alertitem.h>
#include <QWidget>
#include <QVector>
QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

/**
 * \file alertitemscripteditor.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 30 June 2012
*/

namespace Alert {
namespace Internal {
namespace Ui {
class AlertItemScriptEditor;
}

class AlertItemScriptEditor : public QWidget
{
    Q_OBJECT
public:
    explicit AlertItemScriptEditor(QWidget *parent = 0);
    ~AlertItemScriptEditor();

    void clear();
    void setAlertItem(const AlertItem &alert);

    QVector<AlertScript> scripts() const;

public Q_SLOTS:
    void submit();

private:
    void refreshScriptCombo();

private Q_SLOTS:
    void onTypesSelected(int index);
    void addAction(QAction *a);

private:
    int _previousIndex;
    Ui::AlertItemScriptEditor *ui;
    QList<AlertScript> _scriptsCache, _scripts;
    QMenu *_menu;
};

} // namespace Internal
} // namespace Alert

#endif // ALERT_INTERNAL_ALERTITEMSCRIPTEDITOR_H
