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
#include "alertitem.h"
#include "ialertplaceholder.h"

#include <coreplugin/icore.h>
#include <coreplugin/iscriptmanager.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

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
static inline Core::IScriptManager *scriptManager() {return Core::ICore::instance()->scriptManager();}

AlertCore *AlertCore::_instance = 0;

AlertCore *AlertCore::instance()
{
    Q_ASSERT(_instance);
    if (!_instance)
        _instance = new AlertCore(qApp);
    return _instance;
}

namespace Alert {
namespace Internal {
class AlertCorePrivate
{
public:
    AlertCorePrivate() :
        _alertBase(0),
        _placeholdertest(0)

    {}

    ~AlertCorePrivate()
    {
    }

public:
    AlertBase *_alertBase;
    QPointer<AlertPlaceHolderTest> _placeholdertest;
};
}
}

/** Construct the core of the Alert plugin. */
AlertCore::AlertCore(QObject *parent) :
    QObject(parent),
    d(new Internal::AlertCorePrivate)
{
    _instance = this;
    setObjectName("AlertCore");

    // Create all instance
    d->_alertBase = new Internal::AlertBase(this);

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
    if (!d->_alertBase->initialize())
        return false;
    return true;
}

/** Return all the Alert::AlertItem recorded in the database related to the current user. */
QVector<AlertItem> AlertCore::getAlertItemForCurrentUser() const
{
    Internal::AlertBaseQuery query;
    query.addCurrentUserAlerts();
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    return d->_alertBase->getAlertItems(query);
}

/** Return all the Alert::AlertItem recorded in the database related to the current patient. */
QVector<AlertItem> AlertCore::getAlertItemForCurrentPatient() const
{
    Internal::AlertBaseQuery query;
    query.addCurrentPatientAlerts();
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    return d->_alertBase->getAlertItems(query);
}

/** Return all the Alert::AlertItem recorded in the database related to the current application. */
QVector<AlertItem> AlertCore::getAlertItemForCurrentApplication() const
{
    Internal::AlertBaseQuery query;
    query.addApplicationAlerts(qApp->applicationName().toLower());
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    return d->_alertBase->getAlertItems(query);
}

/** Save the Alert::AlertItem \e item into the database and update some of its values. The \e item will be modified. */
bool AlertCore::saveAlert(AlertItem &item)
{
    return d->_alertBase->saveAlertItem(item);
}

/** Save the Alert::AlertItem list \e items into the database and update some of its values. All the items will be modified in the list. */
bool AlertCore::saveAlerts(QList<AlertItem> &items)
{
    bool ok = true;
    for(int i=0; i < items.count(); ++i) {
        AlertItem &item = items[i];
        if (!d->_alertBase->saveAlertItem(item))
            ok = false;
    }
    return ok;
}

/**
  Check all database recorded alerts for the current patient,
  the current user and the current application.\n
  If a script defines the validity of the alert it is executed and the valid state of alert is modified.
  \sa Alert::AlertScript::CheckValidityOfAlert, Alert::AlertItem::isValid()
*/
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
    QVector<AlertItem> alerts = d->_alertBase->getAlertItems(query);

    processAlerts(alerts);

    return true;
}

/**
  Register a new Alert::AlertItem to the core without saving it to the database.
  \sa Alert::AlertCore::saveAlert()
*/
bool AlertCore::registerAlert(const AlertItem &item)
{
    QVector<AlertItem> items;
    items << item;
    processAlerts(items);
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
    if (item.viewType() == AlertItem::DynamicAlert) {
        if (item.isUserValidated() || !item.isValid())
            return true;
        DynamicAlertDialog::executeDynamicAlert(item);
    } else if (item.viewType() == AlertItem::StaticAlert) {
        // Get static place holders
        QList<Alert::IAlertPlaceHolder*> placeHolders = pluginManager()->getObjects<Alert::IAlertPlaceHolder>();
        foreach(Alert::IAlertPlaceHolder *ph, placeHolders) {
            ph->updateAlert(item);
        }
    }
    return true;
}

/**
  Remove a registered static alert (do nothing with dynamic alerts).\n
  Inform all IAlertPlaceHolder of the removal of the alert.\n
  The modification are not saved into the database.
  \sa Alert::AlertCore::saveAlert(), Alert::AlertCore::registerAlert()
*/
bool AlertCore::removeAlert(const AlertItem &item)
{
    bool ok =true;
    if (item.viewType() == AlertItem::StaticAlert) {
        // Get static place holders
        QList<Alert::IAlertPlaceHolder*> placeHolders = pluginManager()->getObjects<Alert::IAlertPlaceHolder>();
        foreach(Alert::IAlertPlaceHolder *ph, placeHolders) {
            if (!ph->removeAlert(item))
                ok = false;
        }
    }
    return ok;
}

/**
 Process alerts:\n
   - Execute check scripts
   - Execute dynamic alerts if needed
   - Feed Alert::IAlertPlaceHolder
*/
void AlertCore::processAlerts(QVector<AlertItem> &alerts)
{
    // Get static place holders
    QList<Alert::IAlertPlaceHolder*> placeHolders = pluginManager()->getObjects<Alert::IAlertPlaceHolder>();

    // Process alerts
    QList<AlertItem> dynamics;
    for(int i = 0; i < alerts.count(); ++i) {
        AlertItem &item = alerts[i];

        // Check script ?
        bool checked = true;
        const AlertScript &checkScript = item.scriptType(AlertScript::CheckValidityOfAlert);
        if (!checkScript.isNull()) {
            QScriptValue v = scriptManager()->evaluate(checkScript.script());
            LOG(tr("Checking alert validity using the 'CheckScript': %1; validity: %2").arg(item.label()).arg(v.toBool()));
            if (!v.toBool()) {
                checked = false;
            }
        }
        if (!checked)
            continue;

        // Cycling scripts
        for(int i =0; i < item.timings().count(); ++i) {
            AlertTiming &timing = item.timingAt(i);
            if (timing.isCycling()) {
                const AlertScript &cyclingDateScript = item.scriptType(AlertScript::CyclingStartDate);
                if (!cyclingDateScript.isNull()) {
                    QScriptValue v = scriptManager()->evaluate(cyclingDateScript.script());
                    LOG(tr("Cycling alert script: 'CyclingStartDate': %1; date: %2").arg(item.label()).arg(v.toDateTime().toString(Qt::ISODate)));
                    if (v.isDate()) {
                        // TODO: correctly the nearest starting date
                        timing.setCycleStartDate(v.toDateTime());
                        timing.setCycleExpirationDate(timing.start().addSecs(timing.cyclingDelayInMinutes()*60));
                    }
                }
            }
        }

        if (item.viewType() == AlertItem::DynamicAlert) {
            if (!item.isValid() || item.isUserValidated())
                continue;
            dynamics << item;
        } else {
            foreach(Alert::IAlertPlaceHolder *ph, placeHolders) {
                ph->addAlert(item);
            }
        }
    }

    if (!dynamics.isEmpty()) {
        DynamicAlertResult result = DynamicAlertDialog::executeDynamicAlert(dynamics);
        DynamicAlertDialog::applyResultToAlerts(dynamics, result);
        if (!saveAlerts(dynamics))
            LOG_ERROR("Unable to save validated dynamic alerts");
    }
}

void AlertCore::postCoreInitialization()
{
    // TESTS
    QDateTime start = QDateTime::currentDateTime().addSecs(-60*60*24);
    QDateTime expiration = QDateTime::currentDateTime().addSecs(60*60*24);

    AlertItem item = d->_alertBase->createVirtualItem();
    item.setLabel(item.label() + " (item)");
    item.setThemedIcon("identity.png");
    item.setViewType(AlertItem::StaticAlert);
    item.setPriority(AlertItem::High);
    item.setRemindLaterAllowed(true);
    item.clearRelations();
    item.clearTimings();
    item.addRelation(AlertRelation(AlertRelation::RelatedToPatient, "patient1"));
    item.addTiming(AlertTiming(start, expiration));

    AlertItem item2 = d->_alertBase->createVirtualItem();
    item2.setThemedIcon("next.png");
    item2.setViewType(AlertItem::StaticAlert);
    item2.clearRelations();
    item2.clearTimings();
    item2.addRelation(AlertRelation(AlertRelation::RelatedToPatient, "patient2"));
    item2.addTiming(AlertTiming(start, expiration));

    AlertItem item3;
    item3.setUuid(Utils::Database::createUid());
    item3.setThemedIcon("ok.png");
    item3.setLabel("Just a simple alert (item3)");
    item3.setCategory("Test");
    item3.setDescription("Simple basic static alert that needs a user comment on overriding");
    item3.setViewType(AlertItem::StaticAlert);
    item3.setPriority(AlertItem::Low);
    item3.setOverrideRequiresUserComment(true);
    item3.addRelation(AlertRelation(AlertRelation::RelatedToPatient, "patient1"));
    item3.addTiming(AlertTiming(start, expiration));

    AlertItem item4;
    item4.setUuid(Utils::Database::createUid());
    item4.setThemedIcon("elderly.png");
    item4.setLabel("Related to all patient (item4)");
    item4.setCategory("Test");
    item4.setDescription("Related to all patients and was validated for patient2 by user1.<br /> Static alert");
    item4.setViewType(AlertItem::StaticAlert);
    item4.setPriority(AlertItem::Medium);
    item4.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    item4.addValidation(AlertValidation(QDateTime::currentDateTime(), "user1", "patient2"));
    item4.addTiming(AlertTiming(start, expiration));

    AlertItem item5;
    item5.setUuid(Utils::Database::createUid());
    item5.setLabel("Simple basic dynamic alert test (item5)");
    item5.setCategory("Test");
    item5.setDescription("Aoutch this is a dynamic alert !");
    item5.setViewType(AlertItem::DynamicAlert);
    item5.setRemindLaterAllowed(true);
    item5.setOverrideRequiresUserComment(true);
    item5.addRelation(AlertRelation(AlertRelation::RelatedToPatient, "patient1"));
    item5.addTiming(AlertTiming(start, expiration));

    AlertItem item6;
    item6.setUuid(Utils::Database::createUid());
    item6.setLabel("Simple basic dynamic user alert (item6)");
    item6.setCategory("Test user alert");
    item6.setDescription("Aoutch this is a dynamic alert !<br />For you, <b>user1</b>!");
    item6.setViewType(AlertItem::DynamicAlert);
    item6.setRemindLaterAllowed(true);
    item6.addRelation(AlertRelation(AlertRelation::RelatedToUser, "user1"));
    item6.addTiming(AlertTiming(start, expiration));
    item6.addScript(AlertScript("check_item6", AlertScript::CheckValidityOfAlert, "(1+1)==2;"));
    item6.addScript(AlertScript("onoverride_item6", AlertScript::OnOverride, "(1+1)==2;"));

    AlertItem item7;
    item7.setUuid(Utils::Database::createUid());
    item7.setLabel("Simple basic alert (item7)");
    item7.setCategory("Test validated alert");
    item7.setDescription("Aoutch this is an error you should not see this !<br /><br />Validated for patient1.");
    item7.setViewType(AlertItem::StaticAlert);
    item7.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    item7.addValidation(AlertValidation(QDateTime::currentDateTime(), "user1", "patient1"));
    item7.addTiming(AlertTiming(start, expiration));

    AlertItem item8;
    item8.setUuid(Utils::Database::createUid());
    item8.setLabel("Scripted alert (item8)");
    item8.setCategory("Test scripted alert");
    item8.setDescription("A valid alert with multiple scripts.");
    item8.setViewType(AlertItem::StaticAlert);
    item8.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    item8.addTiming(AlertTiming(start, expiration));
    item8.addScript(AlertScript("check_item8", AlertScript::CheckValidityOfAlert, "(1+1)==2;"));

    AlertItem item9;
    item9.setUuid(Utils::Database::createUid());
    item9.setLabel("INVALID Scripted alert (item9)");
    item9.setCategory("Test scripted alert");
    item9.setDescription("A invalid alert with multiple scripts. YOU SHOULD NOT SEE IT !!!!");
    item9.setViewType(AlertItem::StaticAlert);
    item9.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    item9.addTiming(AlertTiming(start, expiration));
    item9.addScript(AlertScript("check_item9", AlertScript::CheckValidityOfAlert, "(1+1)==3;"));

    AlertItem item10;
    item10.setUuid(Utils::Database::createUid());
    item10.setLabel("Cycling alert for all patients (item10)");
    item10.setCategory("Test cycling alert");
    item10.setDescription("Testing a cycling alert with:<br />- scripts<br />- relation to all patients.<br />Static alert.");
    item10.setViewType(AlertItem::StaticAlert);
    item10.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    AlertTiming cycling(start, expiration);
    cycling.setCyclingDelayInYears(5);
    cycling.setNumberOfCycles(32565);
    item10.addTiming(cycling);
    item10.addScript(AlertScript("check_item10", AlertScript::CheckValidityOfAlert, "(1+1)==2;"));
    item10.addScript(AlertScript("startdate_item10", AlertScript::CyclingStartDate, "var currentDate = new Date(); currentDate.setDate(currentDate.getDate()-2); currentDate;"));

    // Db save/get
    if (true) {
        if (!d->_alertBase->saveAlertItem(item))
            qWarning() << "ITEM WRONG";
        if (!d->_alertBase->saveAlertItem(item2))
            qWarning() << "ITEM2 WRONG";
        if (!d->_alertBase->saveAlertItem(item3))
            qWarning() << "ITEM3 WRONG";
        if (!d->_alertBase->saveAlertItem(item4))
            qWarning() << "ITEM4 WRONG";
        if (!d->_alertBase->saveAlertItem(item5))
            qWarning() << "ITEM5 WRONG";
        if (!d->_alertBase->saveAlertItem(item6))
            qWarning() << "ITEM6 WRONG";
        if (!d->_alertBase->saveAlertItem(item7))
            qWarning() << "ITEM7 WRONG";
        if (!d->_alertBase->saveAlertItem(item8))
            qWarning() << "ITEM8 WRONG";
        if (!d->_alertBase->saveAlertItem(item9))
            qWarning() << "ITEM9 WRONG";
        if (!d->_alertBase->saveAlertItem(item10))
            qWarning() << "ITEM10 WRONG";

        Internal::AlertBaseQuery query;
        query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
//        query.setAlertValidity(Internal::AlertBaseQuery::InvalidAlerts);
        query.addUserAlerts("user1");
        query.addUserAlerts("user2");
        query.addPatientAlerts("patient1");
        query.addPatientAlerts("patient2");
        query.addPatientAlerts("patient3");
//        query.addUserAlerts();
        QVector<AlertItem> test = d->_alertBase->getAlertItems(query);
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

        DynamicAlertDialog::executeDynamicAlert(QList<AlertItem>() <<  item << item2 << item3 << item4 << item5, buttons);
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

