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
#include "alertplaceholdertest.h"
#include "alertitem.h"
#include "staticalertwidgets.h"

#include <QDebug>

using namespace Alert;

AlertPlaceHolderTest::AlertPlaceHolderTest(QObject *parent) :
    IAlertPlaceHolder(parent),
    _widget(0)
{
}

AlertPlaceHolderTest::~AlertPlaceHolderTest()
{
    qWarning() << "AlertPlaceHolderTest::~AlertPlaceHolderTest()";
}

QString AlertPlaceHolderTest::uuid() const
{
    return "UID_TEST";
}

// for UI presentation of the place holder
QString AlertPlaceHolderTest::name(const QString &lang) const
{
    return "name";
}

QString AlertPlaceHolderTest::category(const QString &lang) const
{
    return "category";
}

QString AlertPlaceHolderTest::description(const QString &lang) const
{
    return "description";
}

void AlertPlaceHolderTest::clear()
{
    if (_widget)
        _widget->clear();
}

bool AlertPlaceHolderTest::addAlert(const AlertItem &alert)
{
    if (!alerts.contains(alert)) {
        alerts << alert;
        if (_widget) {
            StaticAlertToolButton *but = new StaticAlertToolButton(_widget);
            but->setAlertItem(alert);
            _widget->addWidget(but);
            _buttons.insert(alert.uuid(), but);
        }
    }
    return true;
}

bool AlertPlaceHolderTest::removeAlert(const AlertItem &alert)
{
    if (alerts.contains(alert)) {
        alerts.removeAll(alert);
        if (_widget) {
            _buttons.remove(alert.uuid());
            _widget->clear();
            foreach(StaticAlertToolButton *but, _buttons.values())
                _widget->addWidget(but);
        }
    }
    return true;
}

bool AlertPlaceHolderTest::highlightAlert(const AlertItem &alert)
{
    qWarning() << "highlighAlert" << alert.label();
    return true;
}

QWidget *AlertPlaceHolderTest::createWidget(QWidget *parent)
{
    if (!_widget) {
        _widget = new QToolBar(parent);
        _widget->setIconSize(QSize(16,16));
    }
    for(int i = 0; i < alerts.count(); ++i) {
        StaticAlertToolButton *but = new StaticAlertToolButton(_widget);
        but->setAlertItem(alerts.at(i));
        _widget->addWidget(but);
        // connect button signals
    }
    return _widget;
}
