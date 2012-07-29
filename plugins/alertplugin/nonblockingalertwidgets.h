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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERT_NONBLOCKINGALERTWIDGETS_H
#define ALERT_NONBLOCKINGALERTWIDGETS_H

#include <alertplugin/alertplugin_exporter.h>
#include <alertplugin/alertitem.h>

#include <QToolButton>
#include <QLabel>
#include <QMenu>

namespace Alert {
class AlertItem;

class ALERT_EXPORT NonBlockingAlertToolButton : public QToolButton
{
    Q_OBJECT
public:
    NonBlockingAlertToolButton(QWidget *parent = 0);
    ~NonBlockingAlertToolButton();

    void setAlertItem(const AlertItem &item);
    void setDrawBackgroundUsingAlertPriority(bool useAlertPriority);
    void setAutoSaveOnValidationOrOverriding(bool autosave);
    void setAutoSaveOnEditing(bool autosave);

private Q_SLOTS:
    void validateAlert();
    void editAlert();
    void remindAlert();
    void overrideAlert();

private:
    void refreshStyleSheet();
    void retranslateUi();
    void changeEvent(QEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    QAction *aCategory, *aLabel, *aValidate, *aEdit, *aOverride, *aRemindLater;
    QMenu *_menu;
    AlertItem _item;
    bool _drawBackgroundUsingAlertPriority, _autoSave, _autoSaveOnEdit, _aboutToShowScriptExecuted;
};

class ALERT_EXPORT NonBlockingAlertLabel : public QLabel
{
public:
    NonBlockingAlertLabel(QWidget *parent = 0);

    void setAlertItem(const AlertItem &item);
};


} // namespace Alert

#endif // ALERT_NONBLOCKINGALERTWIDGETS_H
