/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
/**
 * \class Alert::AlertCore
 * Central object of the Alert plugin. Manages all singleton objects, alert access,
 * database control...
 * \note Unit-test available (see: Alert:Internal::AlertPlugin)
 */

#include "alertcore.h"
#include "alertbase.h"
#include "alertitem.h"
#include "ialertplaceholder.h"
#include "alertscriptmanager.h"
#include "alertpackdescription.h"
#include "alertplaceholderwidget.h"
#include "constants.h"
#include "blockingalertdialog.h"

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

#include <QDir>

using namespace Alert;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

// Manage Datapacks
static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }

AlertCore *AlertCore::_instance = 0;

/** Return the instance of the object */
AlertCore &AlertCore::instance()
{
    Q_ASSERT(_instance);
    if (!_instance)
        _instance = new AlertCore(qApp);
    return *_instance;
}

namespace Alert {
namespace Internal {
class AlertCorePrivate
{
public:
    AlertCorePrivate(AlertCore *parent) :
        _initialized(false),
        _alertBase(0),
        _alertScriptManager(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~AlertCorePrivate()
    {
    }

public:
    bool _initialized;
    AlertBase *_alertBase;
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
    if (d) {
        delete d;
        d = 0;
    }
}

/** Initialize the core. */
bool AlertCore::initialize()
{
    if (d->_initialized)
        return true;
    if (!d->_alertBase->initialize())
        return false;
    d->_initialized = true;
    return true;
}

/** Returns initialization state of this core */
bool AlertCore::isInitialized() const
{
    return d->_initialized;
}

/** Returns the Alert::Internal::AlertBase single instance object in use (only in unit-test mode) */
#ifdef WITH_TESTS
Internal::AlertBase &AlertCore::alertBase() const
{
    return *d->_alertBase;
}
#endif

/** Return all the Alert::AlertItem recorded in the database related to the current user. */
QVector<AlertItem> AlertCore::getAlertItemForCurrentUser() const
{
    // TODO: unused?
    Internal::AlertBaseQuery query;
    query.addCurrentUserAlerts();
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    return d->_alertBase->getAlertItems(query);
}

/** Return all the Alert::AlertItem recorded in the database related to the current patient. */
QVector<AlertItem> AlertCore::getAlertItemForCurrentPatient() const
{
    // TODO: unused?
    Internal::AlertBaseQuery query;
    query.addCurrentPatientAlerts();
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    return d->_alertBase->getAlertItems(query);
}

/** Return all the Alert::AlertItem recorded in the database related to the current application. */
QVector<AlertItem> AlertCore::getAlertItemForCurrentApplication() const
{
    // TODO: unused?
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
 * Check all database recorded alerts for the current patient,
 * the current user and the current application.\n
 * If a script defines the validity of the alert it is executed and the
 * valid state of alert is modified.
 * \sa Alert::AlertScript::CheckValidityOfAlert, Alert::AlertItem::isValid()
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
    processAlerts(alerts, true);
    return true;
}

/**
 * Register a new Alert::AlertItem to the core without saving it to the database.
 * \sa Alert::AlertCore::saveAlert()
*/
bool AlertCore::registerAlert(const AlertItem &item)
{
    QVector<AlertItem> items;
    items << item;
    processAlerts(items, false);
    return true;
}

/**
 * Update an already registered Alert::AlertItem. \n
 * If the alert view type is a static alert, inform all Alert::IAlertPlaceHolder
 * of the update otherwise execute the blocking alert. \n
 * The modification are not saved into the database.
 * \sa Alert::AlertCore::saveAlert(), Alert::AlertCore::registerAlert()
*/
bool AlertCore::updateAlert(const AlertItem &item)
{
    // Inform all non-blocking place holders of the alert update
    QList<Alert::IAlertPlaceHolder*> placeHolders = pluginManager()->getObjects<Alert::IAlertPlaceHolder>();
    foreach(Alert::IAlertPlaceHolder *ph, placeHolders) {
        ph->updateAlert(item);
    }

    // If alert is a blocking one -> execute it?
    if (item.viewType() == AlertItem::BlockingAlert) {
        if (item.isUserValidated() || !item.isValid())
            return true;
        BlockingAlertDialog::executeBlockingAlert(item);
    }
    return true;
}

/**
 * Remove a registered non-blocking alert (do nothing with blocking alerts).\n
 * Inform all IAlertPlaceHolder of the removal of the alert.\n
 * The modification are not saved into the database.
 * \sa Alert::AlertCore::saveAlert(), Alert::AlertCore::registerAlert()
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
    QFileInfoList files = Utils::getFiles(path, "*.xml", Utils::Recursively);
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
        AlertItem alert = AlertItem::fromXml(Utils::readTextFile(info.absoluteFilePath(), Utils::DontWarnUser));
        if (alert.isValid())
            alerts << alert;
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
 * Process alerts:\n
 *  - Execute check scripts
 *  - Execute blocking alerts if needed
 *  - Feed Alert::IAlertPlaceHolder
 */
void AlertCore::processAlerts(QVector<AlertItem> &alerts, bool clearPlaceHolders)
{
    // Get non-blocking alerts place holders
    QList<Alert::IAlertPlaceHolder*> placeHolders = pluginManager()->getObjects<Alert::IAlertPlaceHolder>();
    if (clearPlaceHolders) {
        foreach(Alert::IAlertPlaceHolder *ph, placeHolders)
            ph->clear();
    }

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
                // Execute alert script for next cycle range computation
                QVariant startDate = d->_alertScriptManager->execute(item, AlertScript::CyclingStartDate);
                if (startDate.isValid() && startDate.canConvert(QVariant::DateTime)) {
                    // TODO: correctly the nearest starting date
                    // timing.setCycleStartDate(startDate.toDateTime());
                    // timing.setCycleExpirationDate(timing.start().addSecs(timing.cyclingDelayInMinutes()*60));
                }
            }
            // TODO: something is missing here: autochecking of next cycle range.
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
    // Member directly called by AlertPlugin object
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    if (patient())
        connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(checkPatientAlerts()));
    if (user())
        connect(user(), SIGNAL(userChanged()), this, SLOT(checkUserAlerts()));
}

/**
 * Execute the alert script and return the script execution value (Eg '2' for a '1+1' script)
 * \sa Alert::AlertScript, Alert::Internal::AlertScriptManager
 */
QVariant AlertCore::execute(AlertItem &item, const int scriptType)
{
    return d->_alertScriptManager->execute(item, scriptType);
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
