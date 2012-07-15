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
#ifndef ALERT_PLUGIN__ALERTPLACEHOLDERWIDGET_H
#define ALERT_PLUGIN__ALERTPLACEHOLDERWIDGET_H

#include <alertplugin/ialertplaceholder.h>
#include <alertplugin/alertplugin_exporter.h>
#include <alertplugin/alertitem.h>

#include <QToolBar>
#include <QPointer>
#include <QToolButton>

namespace Alert {
class NonBlockingAlertToolButton;

class ALERT_EXPORT AlertPlaceHolderWidget : public IAlertPlaceHolder
{
    Q_OBJECT
    
public:
    explicit AlertPlaceHolderWidget(QObject *parent = 0);
    ~AlertPlaceHolderWidget();
    
    // identification
    virtual QString uuid() const;
    virtual QString name(const QString &lang = QString::null) const;
    virtual QString category(const QString &lang = QString::null) const;
    virtual QString description(const QString &lang = QString::null) const;

    // AlertItem management
    void clear();
    bool addAlert(const AlertItem &alert);
    bool updateAlert(const AlertItem &alert);
    bool removeAlert(const AlertItem &alert);
    bool highlightAlert(const AlertItem &alert);

    QWidget *createWidget(QWidget *parent = 0);

    bool containsAlert(const AlertItem &item);
    bool containsAlertUuid(const QString &alertUid);
    bool removeAlertUuid(const QString &alertUid);

private Q_SLOTS:
    void createAlert();

private:
    void addNewAlertButton();
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QPointer<QToolBar> _widget;
    QList<AlertItem> alerts;
    QList<int> _priorities;
    QHash<QString, NonBlockingAlertToolButton *> _buttons;
    QToolButton *_newButton;
};

}  // namespace Alert

#endif // ALERT_PLUGIN__ALERTPLACEHOLDERWIDGET_H
