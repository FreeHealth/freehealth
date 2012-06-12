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
#include "alertcore.h"
#include "alertbase.h"
#include "alertmanager.h"
#include "alertitem.h"

// TEST
#include "alertitemeditorwidget.h"
#include <QDialog>
#include <QGridLayout>
// END TEST

using namespace Alert;

AlertCore *AlertCore::_instance = 0;

AlertCore *AlertCore::instance(QObject *parent)
{
    if (!_instance)
        _instance = new AlertCore(parent);
    return _instance;
}

namespace Alert {
namespace Internal {
class AlertCorePrivate
{
public:
    AlertCorePrivate() :
        m_alertBase(0),
        m_alertManager(0)
    {}

    ~AlertCorePrivate() {}

public:
    AlertBase *m_alertBase;
    AlertManager *m_alertManager;
};
}
}

AlertCore::AlertCore(QObject *parent) :
    QObject(parent),
    d(new Internal::AlertCorePrivate)
{
}

AlertCore::~AlertCore()
{
    if (d) {
        delete d;
        d = 0;
    }
}

bool AlertCore::initialize()
{
    // Create all instance
    d->m_alertBase = new Internal::AlertBase(this);
    d->m_alertManager = new AlertManager(this);
    if (!d->m_alertBase->init())
        return false;

    // TESTS
    AlertItem item = d->m_alertBase->createVirtualItem();
    AlertItem item2 = item;
    d->m_alertBase->saveAlertItem(item);
    Internal::AlertBaseQuery query;
    query.getAlertItemFromUuid(item.uuid());
    QVector<AlertItem> test = d->m_alertBase->getAlertItems(query);
    qWarning() << test;
    qWarning() << item.toXml();

    AlertItem t = AlertItem::fromXml(item.toXml());
    qWarning() << t.toXml();

    qWarning() << (t.toXml() == item.toXml());

    QDialog dlg;
    AlertItemEditorWidget *w = new AlertItemEditorWidget(&dlg);
    dlg.setLayout(new QGridLayout(&dlg));

    AlertTiming &time = item.timingAt(0);
    time.setCycling(true);
    time.setCyclingDelayInDays(10);
    qWarning() << time.cyclingDelayInMinutes();
    int period, mod;
    time.cyclingDelayPeriodModulo(&period, &mod);
    // period = Trans::Constants::Time::Weeks
    // mod = 11
    qWarning() << period << mod;

    w->setAlertItem(item);
    dlg.layout()->addWidget(w);
    dlg.exec();
    // END TESTS


    return true;
}

void AlertCore::showIHMaccordingToType(int type)
{
    d->m_alertManager->initializeWithType(type);
}
