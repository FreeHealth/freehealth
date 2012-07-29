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
#include "alertscriptmanager.h"
#include "alertpackdescription.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>

#include <datapackutils/datapackcore.h>
#include <datapackutils/ipackmanager.h>
#include <datapackutils/pack.h>

// TEST
#include "alertitemeditordialog.h"
#include "blockingalertdialog.h"
#include "alertplaceholderwidget.h"
#include <QToolButton>
#include <QVBoxLayout>
#include <QPointer>
// END TEST

# include <QDir>

enum { WithTests = false };

using namespace Alert;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

// Manage Datapacks
static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }

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
    AlertCorePrivate(AlertCore *parent) :
        _alertBase(0),
        _placeholdertest(0),
        _alertScriptManager(0),
        q(parent)
    {}

    ~AlertCorePrivate()
    {
    }

    void makeTests();

public:
    AlertBase *_alertBase;
    QPointer<AlertPlaceHolderWidget> _placeholdertest;
    AlertScriptManager *_alertScriptManager;

private:
    AlertCore *q;
};
}
}

/** Construct the core of the Alert plugin. */
AlertCore::AlertCore(QObject *parent) :
    QObject(parent),
    d(new Internal::AlertCorePrivate(this))
{
    _instance = this;
    setObjectName("AlertCore");

    // Create all instance
    d->_alertBase = new Internal::AlertBase(this);
    d->_alertScriptManager = new Internal::AlertScriptManager(this);

    connect(packManager(), SIGNAL(packInstalled(DataPack::Pack)), this, SLOT(packInstalled(DataPack::Pack)));
    connect(packManager(), SIGNAL(packRemoved(DataPack::Pack)), this, SLOT(packRemoved(DataPack::Pack)));
//    connect(packManager(), SIGNAL(packUpdated(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
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
  execute the blocking alert. \n
  The modification are not saved into the database.
  \sa Alert::AlertCore::saveAlert(), Alert::AlertCore::registerAlert()
*/
bool AlertCore::updateAlert(const AlertItem &item)
{
    if (item.viewType() == AlertItem::BlockingAlert) {
        if (item.isUserValidated() || !item.isValid())
            return true;
        BlockingAlertDialog::executeBlockingAlert(item);
    } else if (item.viewType() == AlertItem::NonBlockingAlert) {
        // Get static place holders
        QList<Alert::IAlertPlaceHolder*> placeHolders = pluginManager()->getObjects<Alert::IAlertPlaceHolder>();
        foreach(Alert::IAlertPlaceHolder *ph, placeHolders) {
            ph->updateAlert(item);
        }
    }
    return true;
}

/**
  Remove a registered static alert (do nothing with blocking alerts).\n
  Inform all IAlertPlaceHolder of the removal of the alert.\n
  The modification are not saved into the database.
  \sa Alert::AlertCore::saveAlert(), Alert::AlertCore::registerAlert()
*/
bool AlertCore::removeAlert(const AlertItem &item)
{
    bool ok =true;
    if (item.viewType() == AlertItem::NonBlockingAlert) {
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
 * Register an AlertPack to the database. \n
 * An alert pack is a specific datapack, containing only xml'd alerts. \n
 * Read all alerts in the alert pack \e absPath and save them to database. \n
 * Note that alerts are not processed.
 * \sa Alert::AlertItem::fromXml(), Alert::AlertCore::checkAllAlerts(),Alert::AlertCore::checkPatientAlerts(), Alert::AlertCore::checkUserAlerts()
*/
bool AlertCore::registerAlertPack(const QString &absPath)
{
    LOG(tr("Registering alert pack: %1").arg(QDir::cleanPath(absPath)));
    QDir path(absPath);
    if (!path.exists()) {
        LOG_ERROR(tkTr(Trans::Constants::PATH_1_DOESNOT_EXISTS).arg(QDir::cleanPath(absPath)));
        return false;
    }

    // get the packdescription file and save to database
    QFileInfo descrFile(QString("%1/%2").arg(absPath).arg(Constants::PACK_DESCRIPTION_FILENAME));
    if (!descrFile.exists()) {
        LOG_ERROR(tr("No alert pack description"));
        return false;
    }
    AlertPackDescription descr;
    descr.fromXmlFile(descrFile.absoluteFilePath());
    if (!d->_alertBase->saveAlertPackDescription(descr)) {
        LOG_ERROR("Unable to save alert pack description to database");
        return false;
    }

    // read all alerts
    QFileInfoList files = Utils::getFiles(path, "*.xml");
    if (files.isEmpty()) {
        LOG_ERROR(tkTr(Trans::Constants::PATH_1_IS_EMPTY));
        return false;
    }
    QList<AlertItem> alerts;
    foreach(const QFileInfo &info, files) {
        // don't read the packdescription file here
        if (info.fileName()==QString(Constants::PACK_DESCRIPTION_FILENAME))
            continue;
        // create the alert from the xml file
        alerts << AlertItem::fromXml(Utils::readTextFile(info.absoluteFilePath(), Utils::DontWarnUser));
    }
    return saveAlerts(alerts);
}

/**
 * Remove a registered AlertPack from the database. \n
 * An alert pack is a specific datapack, containing only xml'd alerts. \n
 * All alerts related to this AlertPack will be invalidated in the database, causing them to be ignored.
 * Note that alerts are not (re)processed.
 * \sa Alert::AlertItem::fromXml(), Alert::AlertCore::checkAllAlerts(),Alert::AlertCore::checkPatientAlerts(), Alert::AlertCore::checkUserAlerts()
*/
bool AlertCore::removeAlertPack(const QString &uid)
{
    return d->_alertBase->removeAlertPack(uid);
}

AlertPackDescription AlertCore::getAlertPackDescription(const QString &uuid)
{
    return d->_alertBase->getAlertPackDescription(uuid);
}

/**
 Process alerts:\n
   - Execute check scripts
   - Execute blocking alerts if needed
   - Feed Alert::IAlertPlaceHolder
*/
void AlertCore::processAlerts(QVector<AlertItem> &alerts)
{
    // Get static place holders
    QList<Alert::IAlertPlaceHolder*> placeHolders = pluginManager()->getObjects<Alert::IAlertPlaceHolder>();
    foreach(Alert::IAlertPlaceHolder *ph, placeHolders)
        ph->clear();

    // Process alerts
    QList<AlertItem> blockings;
    for(int i = 0; i < alerts.count(); ++i) {
        AlertItem &item = alerts[i];

        // Check script ?
        QVariant checkValid = d->_alertScriptManager->execute(item, AlertScript::CheckValidityOfAlert);
        if (checkValid.isValid() && checkValid.canConvert(QVariant::Bool) && !checkValid.toBool())
            continue;

        // Cycling scripts
        for(int i =0; i < item.timings().count(); ++i) {
            AlertTiming &timing = item.timingAt(i);
            if (timing.isCycling()) {
                QVariant startDate = d->_alertScriptManager->execute(item, AlertScript::CyclingStartDate);
                if (startDate.isValid() && startDate.canConvert(QVariant::DateTime)) {
                    // TODO: correctly the nearest starting date
                    timing.setCycleStartDate(startDate.toDateTime());
                    timing.setCycleExpirationDate(timing.start().addSecs(timing.cyclingDelayInMinutes()*60));
                }
            }
        }

        if (item.viewType() == AlertItem::BlockingAlert) {
            if (!item.isValid() || item.isUserValidated())
                continue;
            blockings << item;
        } else {
            foreach(Alert::IAlertPlaceHolder *ph, placeHolders) {
                ph->addAlert(item);
            }
        }
    }

    if (!blockings.isEmpty()) {
        BlockingAlertResult result = BlockingAlertDialog::executeBlockingAlert(blockings);
        BlockingAlertDialog::applyResultToAlerts(blockings, result);
        if (!saveAlerts(blockings))
            LOG_ERROR("Unable to save validated blockings alerts");
    }
}

void AlertCore::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;

    QDateTime start = QDateTime::currentDateTime().addSecs(-60*60*24);
    QDateTime expiration = QDateTime::currentDateTime().addSecs(60*60*24);
    AlertItem item5;
    item5.setUuid(Utils::Database::createUid());
    item5.setLabel("Simple basic Blocking alert test (item5)");
    item5.setCategory("Test");
    item5.setDescription("Aoutch this is a Blocking alert !");
    item5.setViewType(AlertItem::BlockingAlert);
    item5.setRemindLaterAllowed(true);
    item5.setOverrideRequiresUserComment(true);
    item5.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    item5.addTiming(AlertTiming(start, expiration));
    d->_alertBase->saveAlertItem(item5);

    if (WithTests)
        d->makeTests();
    if (patient())
        connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(checkPatientAlerts()));
    if (user())
        connect(user(), SIGNAL(userChanged()), this, SLOT(checkUserAlerts()));
}

QVariant AlertCore::execute(AlertItem &item, const int scriptType)
{
    return d->_alertScriptManager->execute(item, scriptType);
}

void Internal::AlertCorePrivate::makeTests()
{
    // TESTS
    QDateTime start = QDateTime::currentDateTime().addSecs(-60*60*24);
    QDateTime expiration = QDateTime::currentDateTime().addSecs(60*60*24);

    AlertItem item = _alertBase->createVirtualItem();
    item.setLabel(item.label() + " (item)");
    item.setThemedIcon("identity.png");
    item.setViewType(AlertItem::NonBlockingAlert);
    item.setPriority(AlertItem::High);
    item.setRemindLaterAllowed(true);
    item.clearRelations();
    item.clearTimings();
    item.addRelation(AlertRelation(AlertRelation::RelatedToPatient, "patient1"));
    item.addTiming(AlertTiming(start, expiration));

    AlertItem item2 = _alertBase->createVirtualItem();
    item2.setThemedIcon("next.png");
    item2.setViewType(AlertItem::NonBlockingAlert);
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
    item3.setViewType(AlertItem::NonBlockingAlert);
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
    item4.setViewType(AlertItem::NonBlockingAlert);
    item4.setPriority(AlertItem::Medium);
    item4.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    item4.addValidation(AlertValidation(QDateTime::currentDateTime(), "user1", "patient2"));
    item4.addTiming(AlertTiming(start, expiration));

    AlertItem item5;
    item5.setUuid(Utils::Database::createUid());
    item5.setLabel("Simple basic Blocking alert test (item5)");
    item5.setCategory("Test");
    item5.setDescription("Aoutch this is a Blocking alert !");
    item5.setViewType(AlertItem::BlockingAlert);
    item5.setRemindLaterAllowed(true);
    item5.setOverrideRequiresUserComment(true);
    item5.addRelation(AlertRelation(AlertRelation::RelatedToPatient, "patient1"));
    item5.addTiming(AlertTiming(start, expiration));

    AlertItem item6;
    item6.setUuid(Utils::Database::createUid());
    item6.setLabel("Simple basic Blocking user alert (item6)");
    item6.setCategory("Test user alert");
    item6.setDescription("Aoutch this is a Blocking alert !<br />For you, <b>user1</b>!");
    item6.setViewType(AlertItem::BlockingAlert);
    item6.setRemindLaterAllowed(true);
    item6.addRelation(AlertRelation(AlertRelation::RelatedToUser, "user1"));
    item6.addTiming(AlertTiming(start, expiration));
    item6.addScript(AlertScript("check_item6", AlertScript::CheckValidityOfAlert, "(1+1)==2;"));
    item6.addScript(AlertScript("onoverride_item6", AlertScript::OnOverridden, "(1+1)==2;"));

    AlertItem item7;
    item7.setUuid(Utils::Database::createUid());
    item7.setLabel("Simple basic alert (item7)");
    item7.setCategory("Test validated alert");
    item7.setDescription("Aoutch this is an error you should not see this !<br /><br />Validated for patient1.");
    item7.setViewType(AlertItem::NonBlockingAlert);
    item7.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    item7.addValidation(AlertValidation(QDateTime::currentDateTime(), "user1", "patient1"));
    item7.addTiming(AlertTiming(start, expiration));

    AlertItem item8;
    item8.setUuid(Utils::Database::createUid());
    item8.setLabel("Scripted alert (item8)");
    item8.setCategory("Test scripted alert");
    item8.setDescription("A valid alert with multiple scripts.");
    item8.setViewType(AlertItem::NonBlockingAlert);
    item8.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    item8.addTiming(AlertTiming(start, expiration));
    item8.addScript(AlertScript("check_item8", AlertScript::CheckValidityOfAlert, "(1+1)==2;"));

    AlertItem item9;
    item9.setUuid(Utils::Database::createUid());
    item9.setLabel("INVALID Scripted alert (item9)");
    item9.setCategory("Test scripted alert");
    item9.setDescription("A invalid alert with multiple scripts. YOU SHOULD NOT SEE IT !!!!");
    item9.setViewType(AlertItem::NonBlockingAlert);
    item9.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    item9.addTiming(AlertTiming(start, expiration));
    item9.addScript(AlertScript("check_item9", AlertScript::CheckValidityOfAlert, "(1+1)==3;"));

    AlertItem item10;
    item10.setUuid(Utils::Database::createUid());
    item10.setLabel("Cycling alert for all patients (item10)");
    item10.setCategory("Test cycling alert");
    item10.setDescription("Testing a cycling alert with:<br />- scripts<br />- relation to all patients.<br />Static alert.");
    item10.setViewType(AlertItem::NonBlockingAlert);
    item10.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    AlertTiming cycling(start, expiration);
    cycling.setCyclingDelayInYears(5);
    cycling.setNumberOfCycles(32565);
    item10.addTiming(cycling);
    item10.addScript(AlertScript("check_item10", AlertScript::CheckValidityOfAlert, "(1+1)==2;"));
    item10.addScript(AlertScript("startdate_item10", AlertScript::CyclingStartDate, "var currentDate = new Date(); currentDate.setDate(currentDate.getDate()-2); currentDate;"));

    AlertItem item11;
    item11.setUuid(Utils::Database::createUid());
    item11.setLabel("Test script interactions (item11)");
    item11.setCategory("Test script");
    item11.setDescription("Test script interaction with alertitem:<br />- redefine priority to HIGH<br />- change the label adding \"WAAAAAAHHHHHHOOUUUUHHH\"<br/>- add a comment");
    item11.setViewType(AlertItem::BlockingAlert);
    item11.addRelation(AlertRelation(AlertRelation::RelatedToAllPatients));
    item11.addTiming(AlertTiming(start, expiration));
    item11.addScript(AlertScript("check_item11", AlertScript::CheckValidityOfAlert,
                                 "print(\"CURRENT ALERT PROPERTY:\"+alert.priority());"
                                 "alert.setPriority(0);"
                                 "print(\"CURRENT ALERT LABEL:\"+alert.label());"
                                 "alert.setLabel(alert.label()+\"<br />WAAAAAAHHHHHHOOUUUUHHH\");"
                                 "alert.setComment(\"Niah niah niah comment added by the script...\");"
                                 "true;"));

    // Db save/get
    if (false) {
        if (!_alertBase->saveAlertItem(item))
            qWarning() << "ITEM WRONG";
        if (!_alertBase->saveAlertItem(item2))
            qWarning() << "ITEM2 WRONG";
        if (!_alertBase->saveAlertItem(item3))
            qWarning() << "ITEM3 WRONG";
        if (!_alertBase->saveAlertItem(item4))
            qWarning() << "ITEM4 WRONG";
        if (!_alertBase->saveAlertItem(item5))
            qWarning() << "ITEM5 WRONG";
        if (!_alertBase->saveAlertItem(item6))
            qWarning() << "ITEM6 WRONG";
        if (!_alertBase->saveAlertItem(item7))
            qWarning() << "ITEM7 WRONG";
        if (!_alertBase->saveAlertItem(item8))
            qWarning() << "ITEM8 WRONG";
        if (!_alertBase->saveAlertItem(item9))
            qWarning() << "ITEM9 WRONG";
        if (!_alertBase->saveAlertItem(item10))
            qWarning() << "ITEM10 WRONG";
        if (!_alertBase->saveAlertItem(item11))
            qWarning() << "ITEM11 WRONG";

        Internal::AlertBaseQuery query;
        query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
//        query.setAlertValidity(Internal::AlertBaseQuery::InvalidAlerts);
        query.addUserAlerts("user1");
        query.addUserAlerts("user2");
        query.addPatientAlerts("patient1");
        query.addPatientAlerts("patient2");
        query.addPatientAlerts("patient3");
//        query.addUserAlerts();
        QVector<AlertItem> test = _alertBase->getAlertItems(query);
        qWarning() << test.count();
//        for(int i=0; i < test.count(); ++i) {
//            qWarning() << "\n\n" << test.at(i).timingAt(0).start() << test.at(i).timingAt(0).end() << test.at(i).relationAt(1).relatedToUid();
//        }
//        qWarning() << "\n\n";
        //    AlertItem t = AlertItem::fromXml(item.toXml());
        //    qWarning() << (t.toXml() == item.toXml());
    }

    // To XML
    if (false) {
        qWarning() << item.toXml();
        qWarning() << item11.toXml();
    }

    // Blocking alerts
    if (false) {
        item.setViewType(AlertItem::BlockingAlert);
        item.setOverrideRequiresUserComment(true);
        QToolButton *test = new QToolButton;
        test->setText("Houlala");
        test->setToolTip("kokokokokok");
        QList<QAbstractButton*> buttons;
        buttons << test;

        BlockingAlertDialog::executeBlockingAlert(QList<AlertItem>() <<  item << item2 << item3 << item4 << item5, buttons);
        //    BlockingAlertDialog::executeBlockingAlert(item4);
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

    // Alert packs
    if (true) {
        q->registerAlertPack(settings()->path(Core::ISettings::BundledAlertPacks) + "/test");
    }

    // PlaceHolders
    if (true) {
        // Put placeholder in the plugin manager object pool
        _placeholdertest = new AlertPlaceHolderWidget; // object should not be deleted
        pluginManager()->addObject(_placeholdertest);

        // Create the dialog && the placeholder
        QDialog dlg;
        QVBoxLayout lay(&dlg);
        dlg.setLayout(&lay);
        lay.addWidget(_placeholdertest->createWidget(&dlg));

        // Check alerts
        q->checkAlerts(AlertCore::CurrentPatientAlerts | AlertCore::CurrentUserAlerts | AlertCore::CurrentApplicationAlerts);

        // Exec the dialog
        dlg.exec();
    }

    // END TESTS
}

void AlertCore::packInstalled(const DataPack::Pack &pack)
{
    if (pack.dataType() == DataPack::Pack::AlertPacks) {
        // register the alertpack
        if (!registerAlertPack(pack.unzipPackToPath())) {
            LOG_ERROR(tr("Unable to register AlertPack. Path: %1").arg(pack.unzipPackToPath()));
            return;
        }
        // force a new alert checking
        checkAllAlerts();
    }
}

void AlertCore::packRemoved(const DataPack::Pack &pack)
{
    if (pack.dataType() == DataPack::Pack::AlertPacks) {
        if (!removeAlertPack(pack.uuid()))
            LOG_ERROR("Unable to remove AlertPack " + pack.uuid());
    }
}
