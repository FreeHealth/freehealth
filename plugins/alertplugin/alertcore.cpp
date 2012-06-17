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
#include "ialertplaceholder.h"

#include <coreplugin/icore.h>

#include <extensionsystem/pluginmanager.h>

// TEST
#include "alertitemeditordialog.h"
#include "dynamicalertdialog.h"
#include "alertplaceholdertest.h"
#include <QToolButton>
#include <QVBoxLayout>
#include <QPointer>
// END TEST

using namespace Alert;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

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
        m_alertManager(0),
        _placeholdertest(0)

    {}

    ~AlertCorePrivate()
    {
    }

public:
    AlertBase *m_alertBase;
    AlertManager *m_alertManager;
    QPointer<AlertPlaceHolderTest> _placeholdertest;
};
}
}

/** Construct the core of the Alert plugin. */
AlertCore::AlertCore(QObject *parent) :
    QObject(parent),
    d(new Internal::AlertCorePrivate)
{
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

AlertCore::~AlertCore()
{
    if (d->_placeholdertest) {
        pluginManager()->removeObject(d->_placeholdertest);
        delete d->_placeholdertest;
        d->_placeholdertest = 0;
    }
    if (d) {
        delete d;
        d = 0;
    }
}

/** Initialize the core. */
bool AlertCore::initialize()
{
    // Create all instance
    d->m_alertBase = new Internal::AlertBase(this);
    d->m_alertManager = new AlertManager(this);
    if (!d->m_alertBase->init())
        return false;
    return true;
}

/** Return all the Alert::AlertItem recorded in the database related to the current user. */
QVector<AlertItem> AlertCore::getAlertItemForCurrentUser() const
{
    Internal::AlertBaseQuery query;
    query.addCurrentUserAlerts();
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    return d->m_alertBase->getAlertItems(query);
}

/** Return all the Alert::AlertItem recorded in the database related to the current patient. */
QVector<AlertItem> AlertCore::getAlertItemForCurrentPatient() const
{
    Internal::AlertBaseQuery query;
    query.addCurrentPatientAlerts();
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    return d->m_alertBase->getAlertItems(query);
}

/** Return all the Alert::AlertItem recorded in the database related to the current application. */
QVector<AlertItem> AlertCore::getAlertItemForCurrentApplication() const
{
    Internal::AlertBaseQuery query;
    query.addApplicationAlerts(qApp->applicationName().toLower());
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    return d->m_alertBase->getAlertItems(query);
}

/** Save the Alert::AlertItem \e item into the database and update some of its values. The \e item will be modified. */
bool AlertCore::saveAlert(AlertItem &item)
{
    return d->m_alertBase->saveAlertItem(item);
}

/** Check all database recorded alerts for the current patient, the current user and the current application. */
bool AlertCore::checkAlerts(AlertsToCheck check)
{
    // Prepare the query
    Internal::AlertBaseQuery query;
    if (check & CurrentUserAlerts)
        query.addCurrentUserAlerts();
    if (check & CurrentPatientAlerts)
        query.addCurrentPatientAlerts();
    if (check & CurrentApplicationAlerts)
        query.addApplicationAlerts(qApp->applicationName().toLower());
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);

    // Get the alerts
    QVector<AlertItem> alerts = d->m_alertBase->getAlertItems(query);

    processAlerts(alerts);

    return true;
}

/**
  Register a new Alert::AlertItem to the core without saving it to the database.
  \sa Alert::AlertCore::saveAlert()
*/
bool AlertCore::registerAlert(const AlertItem &item)
{
    processAlerts(QVector<AlertItem>() << item);
    return true;
}

/**
  Update an already registered Alert::AlertItem. \n
  If the alert view type is a static alert, inform all IAlertPlaceHolder of the update otherwise
  execute the dynamic alert. \n
  The modification are not saved into the database.
  \sa Alert::AlertCore::saveAlert(), Alert::AlertCore::registerAlert()
*/
bool AlertCore::updateAlert(const AlertItem &item)
{
    if (item.viewType() == AlertItem::DynamicAlert && !item.isUserValidated() && item.isValid()) {
        DynamicAlertDialog::executeDynamicAlert(item);
    } else {
        // Get static place holders
        QList<Alert::IAlertPlaceHolder*> placeHolders = pluginManager()->getObjects<Alert::IAlertPlaceHolder>();
        foreach(Alert::IAlertPlaceHolder *ph, placeHolders) {
            ph->updateAlert(item);
        }
    }
    return true;
}

/**
 Process alerts:\n
   - Execute dynamic alerts if needed
   - Feed Alert::IAlertPlaceHolder
*/
void AlertCore::processAlerts(const QVector<AlertItem> &alerts)
{
    // Get static place holders
    QList<Alert::IAlertPlaceHolder*> placeHolders = pluginManager()->getObjects<Alert::IAlertPlaceHolder>();

    // Process alerts
    QList<AlertItem> dynamics;
    for(int i = 0; i < alerts.count(); ++i) {
        const AlertItem &item = alerts.at(i);
        if (!item.isValid() || item.isUserValidated())
            continue;

        if (item.viewType() == AlertItem::DynamicAlert) {
            dynamics << item;
        } else {
            foreach(Alert::IAlertPlaceHolder *ph, placeHolders) {
                ph->addAlert(item);
            }
        }
    }

    if (!dynamics.isEmpty()) {
        DynamicAlertDialog::executeDynamicAlert(dynamics);
    }
}

void AlertCore::postCoreInitialization()
{
    // TESTS
    AlertItem item = d->m_alertBase->createVirtualItem();
    AlertItem item2 = d->m_alertBase->createVirtualItem();

    AlertItem item3 = item2;
//    item3.setUuid("LKLKLK");
    item3.setLabel("Double label");
    item3.setDescription("Double Description Double Description Double Description v Double Description v v vvvDouble Description Double Description Double DescriptionDouble Description Double Description Double Description");
    AlertItem item4 = item2;
//    item4.setUuid("qsdkygvuihe");
    item4.setLabel("Double label Double label");
    item4.setDescription("Double Description Double Description Double Description v Double Description v v vvvDouble Description Double Description Double DescriptionDouble Description Double Description Double Description");
    AlertItem item5 = item2;
    item5.setUuid("fokoe,rf");
    item5.setLabel("Double label Double label Double label");
    item5.setDescription("Double Description Double Description Double Description v Double Description v v vvvDouble Description Double Description Double DescriptionDouble Description Double Description Double Description");
    AlertItem item6 = item2;
    item6.setUuid("dfqdf qsf");
    item6.setLabel("Double labelDouble label Double label Double label Double label");
    item6.setDescription("Double Description Double Description Double Description v Double Description v v vvvDouble Description Double Description Double DescriptionDouble Description Double Description Double Description");

    // Db save/get
    if (true) {
        item3 = d->m_alertBase->createVirtualItem();
//        item4 = d->m_alertBase->createVirtualItem();

        AlertRelation rel;
        // patients
        rel.setRelatedTo(AlertRelation::RelatedToPatient);
        rel.setRelatedToUid("patient1");
        item.addRelation(rel);
        rel.setRelatedToUid("patient2.1");
        item2.addRelation(rel);
        rel.setRelatedToUid("patient3");
        item3.addRelation(rel);
        // users
        rel.setRelatedTo(AlertRelation::RelatedToUser);
        rel.setRelatedToUid("user1");
        item4.addRelation(rel);
        // timings
        AlertTiming timing;
        timing.setStart(QDateTime::currentDateTime().addSecs(-60*60*24));
        timing.setEnd(QDateTime::currentDateTime().addSecs(60*60*24));
        item.clearTimings();
        item.addTiming(timing);
        item2.clearTimings();
        item2.addTiming(timing);
        item3.clearTimings();
        item3.addTiming(timing);
        item4.clearTimings();
        item4.addTiming(timing);

        item.setViewType(AlertItem::StaticAlert);
        item2.setViewType(AlertItem::StaticAlert);

        if (!d->m_alertBase->saveAlertItem(item))
            qWarning() << "ITEM WRONG";
        if (!d->m_alertBase->saveAlertItem(item2))
            qWarning() << "ITEM2 WRONG";
        if (!d->m_alertBase->saveAlertItem(item3))
            qWarning() << "ITEM3 WRONG";
        if (!d->m_alertBase->saveAlertItem(item4))
            qWarning() << "ITEM4 WRONG";

        Internal::AlertBaseQuery query;
        query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
//        query.setAlertValidity(Internal::AlertBaseQuery::InvalidAlerts);
        query.addUserAlerts("user1");
        query.addUserAlerts("user2");
        query.addPatientAlerts("patient1");
        query.addPatientAlerts("patient2");
        query.addPatientAlerts("patient3");
//        query.addUserAlerts();
        QVector<AlertItem> test = d->m_alertBase->getAlertItems(query);
        qWarning() << test.count();
        for(int i=0; i < test.count(); ++i) {
            qWarning() << "\n\n" << test.at(i).timingAt(0).start() << test.at(i).timingAt(0).end() << test.at(i).relationAt(1).relatedToUid();
        }
        qWarning() << "\n\n";
        //    AlertItem t = AlertItem::fromXml(item.toXml());
        //    qWarning() << (t.toXml() == item.toXml());
    }

    // Dynamic alerts
    if (false) {
        item.setViewType(AlertItem::DynamicAlert);
        item.setOverrideRequiresUserComment(true);
        QToolButton *test = new QToolButton;
        test->setText("Houlala");
        test->setToolTip("kokokokokok");
        QList<QAbstractButton*> buttons;
        buttons << test;

        DynamicAlertDialog::executeDynamicAlert(QList<AlertItem>() <<  item << item2 << item3 << item4 << item5<<item6, buttons);
        //    DynamicAlertDialog::executeDynamicAlert(item4);
    }

    // Alert editor
    if (false) {
        AlertItemEditorDialog dlg;
        dlg.setEditableParams(AlertItemEditorDialog::FullDescription | AlertItemEditorDialog::Timing);
        dlg.setEditableParams(AlertItemEditorDialog::Label | AlertItemEditorDialog::Timing);
        dlg.setEditableParams(AlertItemEditorDialog::Label | AlertItemEditorDialog::Timing | AlertItemEditorDialog::Types);

        AlertTiming &time = item.timingAt(0);
        time.setCycling(true);
        time.setCyclingDelayInDays(10);
        dlg.setAlertItem(item);
        if (dlg.exec()==QDialog::Accepted) {
            dlg.submit(item);
        }
        qWarning() << item.toXml();
    }

    // PlaceHolders
    if (true) {
        // Put placeholder in the plugin manager object pool
        d->_placeholdertest = new AlertPlaceHolderTest; // object should not be deleted
        pluginManager()->addObject(d->_placeholdertest);

        // Create the dialog && the placeholder
        QDialog dlg;
        QVBoxLayout lay(&dlg);
        dlg.setLayout(&lay);
        lay.addWidget(d->_placeholdertest->createWidget(&dlg));

        // Check alerts
        checkAlerts(CurrentPatientAlerts | CurrentUserAlerts | CurrentApplicationAlerts);

        // Exec the dialog
        dlg.exec();
    }
    // END TESTS
}

void AlertCore::showIHMaccordingToType(int type)
{
    d->m_alertManager->initializeWithType(type);
}
