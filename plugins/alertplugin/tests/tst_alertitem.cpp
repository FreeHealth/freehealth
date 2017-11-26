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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "../alertplugin.h"
#include "../alertitem.h"
#include "../alertcore.h"
#include "../alertbase.h"
#include "../blockingalertdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>

//#include "alertitemeditordialog.h"
//#include "alertplaceholderwidget.h"

#include <QDir>
#include <QTest>

using namespace Alert;
using namespace Internal;

//static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
//static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Alert::AlertCore &alertCore() {return Alert::AlertCore::instance();}
static inline Alert::Internal::AlertBase &alertBase() {return Alert::AlertCore::instance().alertBase();}

namespace {
const int loop = 10; // number of test per alertitems

AlertScript &createVirtualScript(int scriptType, Utils::Randomizer &r)
{
    QString script = r.randomWords(r.randomInt(5, 25));
    script += "<=>+/-_;&\"'!:%*()|\\";
    AlertScript *alertscript = new AlertScript(Utils::createUid(), AlertScript::ScriptType(scriptType), script);
    alertscript->setId(r.randomInt(10000000));
    alertscript->setValid(r.randomBool());
    alertscript->setModified(false);
    return *alertscript;
}

// Creates a virtual item.
AlertItem &createVirtualItem(bool createAllRelations = false)
{
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    QDir pix(settings()->path(Core::ISettings::SmallPixmapPath));

    AlertItem *item = new AlertItem;
    item->setValidity(r.randomBool());
    item->setUuid(Utils::createUid());
    if (r.randomBool())
        item->setCryptedPassword(r.randomWords(1).toUtf8().toBase64());

    // fr, de, en, xx
    QStringList langs;
    langs << "en" << "fr" << "de" << "xx";
    foreach(const QString &l, langs) {
        item->setLabel(r.randomWords(r.randomInt(2, 10)), l);
        item->setCategory(r.randomWords(r.randomInt(2, 10)), l);
        item->setDescription(r.randomWords(r.randomInt(2, 10)), l);
        item->setComment(r.randomWords(r.randomInt(2, 10)), l);
    }

    item->setViewType(AlertItem::ViewType(r.randomInt(0, AlertItem::NonBlockingAlert)));
    item->setContentType(AlertItem::ContentType(r.randomInt(0, AlertItem::UserNotification)));
    item->setPriority(AlertItem::Priority(r.randomInt(0, AlertItem::Low)));
    item->setCreationDate(r.randomDateTime(QDateTime::currentDateTime()));
    if (r.randomBool())
        item->setLastUpdate(r.randomDateTime(item->creationDate()));
    item->setThemedIcon(r.randomFile(pix, QStringList() << "*.png").fileName());
    if (r.randomBool())
        item->setStyleSheet(r.randomWords(10));
    if (r.randomBool())
        item->setExtraXml(QString("<xml>%1</xml>").arg(r.randomWords(r.randomInt(1, r.randomInt(2, 20))))); // one word at least otherwise QDocDocument can return </tag> instead of <tag></tag>

    // Add relations
    if (createAllRelations || r.randomBool()) {
        AlertRelation rel;
        rel.setRelatedTo(AlertRelation::RelatedToAllPatients);
        item->addRelation(rel);
    }
    if (createAllRelations || r.randomBool()) {
        AlertRelation rel;
        rel.setRelatedTo(AlertRelation::RelatedToPatient);
        rel.setRelatedToUid("patient1");
        item->addRelation(rel);
    }
    if (createAllRelations || r.randomBool()) {
        AlertRelation rel;
        rel.setRelatedTo(AlertRelation::RelatedToFamily);
        rel.setRelatedToUid("family1");
        item->addRelation(rel);
    }
    if (createAllRelations || r.randomBool()) {
        AlertRelation rel;
        rel.setRelatedTo(AlertRelation::RelatedToAllUsers);
        item->addRelation(rel);
    }
    if (createAllRelations || r.randomBool()) {
        AlertRelation rel;
        rel.setRelatedTo(AlertRelation::RelatedToUser);
        rel.setRelatedToUid("user1");
        item->addRelation(rel);
    }
    if (createAllRelations || r.randomBool()) {
        AlertRelation rel;
        rel.setRelatedTo(AlertRelation::RelatedToUserGroup);
        rel.setRelatedToUid("userGroup1");
        item->addRelation(rel);
    }
    if (createAllRelations || r.randomBool()) {
        AlertRelation rel;
        rel.setRelatedTo(AlertRelation::RelatedToApplication);
        rel.setRelatedToUid("application1");
        item->addRelation(rel);
    }

    // Add timing
    AlertTiming time;
    time.setValid(r.randomBool());
    time.setStart(r.randomDateTime(QDateTime::currentDateTime()));
    time.setEnd(time.start().addDays(r.randomInt(10, 5000)));
    if (r.randomBool()) {
        time.setCycling(true);
        time.setCyclingDelayInMinutes(r.randomInt(10*24*60, 1000*24*60));
        time.setNumberOfCycles(r.randomInt(1, 100));
    }
    item->addTiming(time);

    // Add scripts
    for(int i=0; i < AlertScript::OnRemindLater; ++i) {
        item->addScript(createVirtualScript(i, r));
    }

    // Add random validation
    AlertValidation val;
    val.setId(r.randomInt(1, 10000));
    val.setOverriden(r.randomBool());
    val.setDateOfValidation(QDateTime::currentDateTime());
    val.setUserComment(r.randomWords(r.randomInt(1, 10)));
    val.setValidatedUuid(Utils::createUid());
    val.setValidatorUuid(Utils::createUid());
    item->addValidation(val);

    item->setModified(false);
    return *item;
}

static void verifyAlertEquality(const AlertItem &item, const AlertItem &item2)
{
    QVERIFY(item.isValid() == item2.isValid());
    QVERIFY(item.uuid() == item2.uuid());
    QVERIFY(item.packUid() == item2.packUid());
    QVERIFY(item.cryptedPassword() == item2.cryptedPassword());
    QVERIFY(item.availableLanguages() == item2.availableLanguages());
    foreach(const QString &l, item.availableLanguages()) {
        QVERIFY(item.label(l) == item2.label(l));
        QVERIFY(item.toolTip(l) == item2.toolTip(l));
        QVERIFY(item.category(l) == item2.category(l));
        QVERIFY(item.description(l) == item2.description(l));
        QVERIFY(item.comment(l) == item2.comment(l));
    }
    QVERIFY(item.viewType() == item2.viewType());
    QVERIFY(item.contentType() == item2.contentType());
    QVERIFY(item.priority() == item2.priority());
    QVERIFY(item.isOverrideRequiresUserComment() == item2.isOverrideRequiresUserComment());
    QVERIFY(item.mustBeRead() == item2.mustBeRead());
    QVERIFY(item.isRemindLaterAllowed() == item2.isRemindLaterAllowed());
    QVERIFY(item.isEditable() == item2.isEditable());
    QVERIFY(item.creationDate() == item2.creationDate());
    QVERIFY(item.lastUpdate() == item2.lastUpdate());
    QVERIFY(item.themedIcon() == item2.themedIcon());
    QVERIFY(item.styleSheet() == item2.styleSheet());
    QVERIFY(item.priorityBackgroundColor() == item2.priorityBackgroundColor());

    QVERIFY(item.relations().count() == item2.relations().count());
    QVERIFY(item.timings().count() == item2.timings().count());
    QVERIFY(item.scripts().count() == item2.scripts().count());
    QVERIFY(item.validations().count() == item2.validations().count());
    QVERIFY(item.isUserValidated() == item2.isUserValidated());

//    if (item.htmlToolTip(true) != item2.htmlToolTip(true))
//        qWarning() << item.htmlToolTip(true) << item2.htmlToolTip(true);
//    QVERIFY(item.htmlToolTip(true) == item2.htmlToolTip(true));

//    if (item.htmlToolTip(false) != item2.htmlToolTip(false))
//        qWarning() << item.htmlToolTip(false) << item2.htmlToolTip(false);
//    QVERIFY(item.htmlToolTip(false) == item2.htmlToolTip(false));

    if (item.extraXml() != item2.extraXml())
        qWarning() << item.extraXml() << item2.extraXml();
    QVERIFY(item.extraXml() == item2.extraXml());

    QVERIFY(item == item2);
}
} // anonymous namespace

void AlertPlugin::test_alertscript_object()
{
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    // Test AlertScript XML interface
    for(int i= 0; i < loop; ++i) {
        AlertScript script = createVirtualScript(2, r);
        QString xml = script.toXml();
        AlertScript script2 = AlertScript::fromXml(xml);
        QVERIFY(script.id() == script2.id());
        QVERIFY(script.uuid() == script2.uuid());
        QVERIFY(script.type() == script2.type());
        QVERIFY(script.script() == script2.script());
        QVERIFY(script.isNull() == script2.isNull());
        QVERIFY(script.isValid() == script2.isValid());
        QVERIFY(script2.isModified() == false);
    }
}

void AlertPlugin::test_alertrelation_object()
{
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    // Test AlertRelation XML interface
    for(int i= 0; i < loop; ++i) {
        AlertRelation rel;
        rel.setId(r.randomInt(1, 100000));
        rel.setRelatedTo(AlertRelation::RelatedTo(r.randomInt(0, AlertRelation::RelatedToApplication)));
        rel.setRelatedToUid(r.randomWords(1));
        AlertRelation rel2 = AlertRelation::fromXml(rel.toXml());
        QVERIFY(rel.id() == rel2.id());
        QVERIFY(rel.relatedTo() == rel2.relatedTo());
        QVERIFY(rel.relationTypeToString() == rel2.relationTypeToString());
        QVERIFY(rel.relatedToUid() == rel2.relatedToUid());
        QVERIFY(rel2.isModified() == false);
    }
}

void AlertPlugin::test_alertvalidation_object()
{
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    // Test AlertValidation XML interface
    for(int i= 0; i < loop; ++i) {
        AlertValidation val;
        val.setAccepted(r.randomBool());
        val.setDateOfValidation(r.randomDateTime(QDateTime::currentDateTime()));
        val.setId(r.randomInt(1000));
        val.setOverriden(r.randomBool());
        val.setUserComment(r.randomWords(r.randomInt(10)));
        val.setValidatedUuid(Utils::createUid());
        val.setValidatorUuid(Utils::createUid());
        AlertValidation val2 = AlertValidation::fromXml(val.toXml());
        QVERIFY(val.id() == val2.id());
        QVERIFY(val.validatorUid() == val2.validatorUid());
        QVERIFY(val.userComment() == val2.userComment());
        QVERIFY(val.isOverriden() == val2.isOverriden());
        QVERIFY(val.isAccepted() == val2.isAccepted());
        QVERIFY(val.dateOfValidation() == val2.dateOfValidation());
        QVERIFY(val.validatedUid() == val2.validatedUid());
        QVERIFY(val2.isModified() == false);
    }
}

void AlertPlugin::test_alerttiming_object()
{
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    // TODO: test autocomputation of cycling dates

    // Test operator==() && copy ctor
    for(int i= 0; i < loop; ++i) {
        AlertTiming t;
        t.setId(r.randomInt(1000));
        t.setValid(r.randomBool());
        QDateTime dt = QDateTime::currentDateTime();
        dt = r.randomDateTime(dt);
        t.setStart(r.randomDateTime(dt));
        dt = dt.addDays(r.randomInt(1, r.randomInt(2, 20)));
        t.setEnd(dt);
        dt = dt.addDays(r.randomInt(1, r.randomInt(2, 20)));
        t.setExpiration(dt);
        t.setCycling(false);
        AlertTiming t2(t);
        QVERIFY(t.id() == t2.id());
        QVERIFY(t.isValid() == t2.isValid());
        QVERIFY(t.start() == t2.start());
        QVERIFY(t.end() == t2.end());
        QVERIFY(t.expiration() == t2.expiration());
        QVERIFY(t.isCycling() == t2.isCycling());
        QVERIFY(t.numberOfCycles() == t2.numberOfCycles());
        QVERIFY(t.nextDate() == t2.nextDate());
        QVERIFY(t.cyclingDelayInMinutes() == t2.cyclingDelayInMinutes());
        QVERIFY(t.cyclingDelayInHours() == t2.cyclingDelayInHours());
        QVERIFY(t.cyclingDelayInDays() == t2.cyclingDelayInDays());
        QVERIFY(t.cyclingDelayInWeeks() == t2.cyclingDelayInWeeks());
        QVERIFY(t.cyclingDelayInMonth() == t2.cyclingDelayInMonth());
        QVERIFY(t.cyclingDelayInYears() == t2.cyclingDelayInYears());
        QVERIFY(t.cyclingDelayInDecades() == t2.cyclingDelayInDecades());
        QVERIFY(t.currentCycleStartDate() == t2.currentCycleStartDate());
        QVERIFY(t.currentCycleExpirationDate() == t2.currentCycleExpirationDate());
        QVERIFY(t == t2);
    }

    // Test AlertTiming cycling interface
    for(int i= 0; i < loop; ++i) {
        AlertTiming tc;
        tc.setId(r.randomInt(1000));
        tc.setValid(r.randomBool());
        tc.setCycling(true);

        qlonglong delay = r.randomInt(1, 111111); // avoid 0 duration
        int ncycle = r.randomInt(1, 10); // avoid 0 cycle...
        int currentCycle = r.randomInt(1, ncycle);

        QDateTime start = QDateTime::currentDateTime().addSecs(-delay*60*currentCycle - 10);
        QDateTime end = start.addSecs(delay*60*ncycle + 10);
        start.setTime(QTime(start.time().hour(), start.time().minute(), start.time().second()));
        end.setTime(QTime(end.time().hour(), end.time().minute(), end.time().second()));

        tc.setStart(start);
        tc.setEnd(end);
        tc.setExpiration(end.addDays(1));

        // TODO: obsolete
        // tc.setNextDate(r.randomDateTime(dt));
        // END

        tc.setCyclingDelayInMinutes(delay);
        tc.setNumberOfCycles(ncycle);

        // Check computed data
        QVERIFY(tc.isCycling() == true);
        QVERIFY(tc.numberOfCycles() == ncycle);
        QVERIFY(tc.cyclingDelayInMinutes() == delay);
        QVERIFY(tc.currentCycle() == currentCycle);
        QVERIFY(tc.currentCycleStartDate() == start.addSecs(delay*60*currentCycle));
        QVERIFY(tc.currentCycleExpirationDate() == start.addSecs(delay*60*(currentCycle+1)));
    }

    // Test AlertTiming non-cycling XML interface
    for(int i= 0; i < loop; ++i) {
        AlertTiming t;
        t.setId(r.randomInt(1000));
        t.setValid(r.randomBool());
        QDateTime dt = QDateTime::currentDateTime();
        dt = r.randomDateTime(dt);
        t.setStart(r.randomDateTime(dt));
        dt = dt.addDays(r.randomInt(1, r.randomInt(2, 20)));
        t.setEnd(dt);
        dt = dt.addDays(r.randomInt(1, r.randomInt(2, 20)));
        t.setExpiration(dt);
        t.setCycling(false);
        AlertTiming t2 = AlertTiming::fromXml(t.toXml());
        QVERIFY(t.id() == t2.id());
        QVERIFY(t.isValid() == t2.isValid());
        QVERIFY(t.start() == t2.start());
        QVERIFY(t.end() == t2.end());
        QVERIFY(t.expiration() == t2.expiration());

        QVERIFY(t.isCycling() == t2.isCycling());
        QVERIFY(t.nextDate() == t2.nextDate());
        QVERIFY(t.numberOfCycles() == t2.numberOfCycles());

        QVERIFY(t.cyclingDelayInMinutes() == t2.cyclingDelayInMinutes());
        QVERIFY(t.cyclingDelayInHours() == t2.cyclingDelayInHours());
        QVERIFY(t.cyclingDelayInDays() == t2.cyclingDelayInDays());
        QVERIFY(t.cyclingDelayInWeeks() == t2.cyclingDelayInWeeks());
        QVERIFY(t.cyclingDelayInMonth() == t2.cyclingDelayInMonth());
        QVERIFY(t.cyclingDelayInYears() == t2.cyclingDelayInYears());
        QVERIFY(t.cyclingDelayInDecades() == t2.cyclingDelayInDecades());

        QVERIFY(t.currentCycleStartDate() == t2.currentCycleStartDate());
        QVERIFY(t.currentCycleExpirationDate() == t2.currentCycleExpirationDate());
        QVERIFY(t2.isModified() == false);
        t.setModified(false);
        QVERIFY(t == t2);
    }

    // Test AlertTiming cycling XML interface
    for(int i= 0; i < loop; ++i) {
        AlertTiming tc;
        tc.setId(r.randomInt(1000));
        tc.setValid(r.randomBool());
        QDateTime dt = QDateTime::currentDateTime();
        dt = r.randomDateTime(dt);
        tc.setStart(r.randomDateTime(dt));
        dt = dt.addDays(r.randomInt(1, r.randomInt(2, 20)));
        tc.setEnd(dt);
        dt = dt.addDays(r.randomInt(1, r.randomInt(2, 20)));
        tc.setExpiration(dt);
        tc.setCycling(true);
        // TODO: obsolete
        tc.setNextDate(r.randomDateTime(dt));
        // END

        tc.setCyclingDelayInMinutes(r.randomInt(100, 100000)); // avoid 0 duration
        tc.setNumberOfCycles(r.randomInt(1, 100)); // avoid 0 cycle...
        tc.setModified(false);
        AlertTiming tc2 = AlertTiming::fromXml(tc.toXml());
        QVERIFY(tc.id() == tc2.id());
        QVERIFY(tc.isValid() == tc2.isValid());
        QVERIFY(tc.start() == tc2.start());
        QVERIFY(tc.end() == tc2.end());
        QVERIFY(tc.expiration() == tc2.expiration());
        QVERIFY(tc.isCycling() == tc2.isCycling());
        QVERIFY(tc.numberOfCycles() == tc2.numberOfCycles());
        QVERIFY(tc.nextDate() == tc2.nextDate());
        QVERIFY(tc.cyclingDelayInMinutes() == tc2.cyclingDelayInMinutes());
        QVERIFY(tc.cyclingDelayInHours() == tc2.cyclingDelayInHours());
        QVERIFY(tc.cyclingDelayInDays() == tc2.cyclingDelayInDays());
        QVERIFY(tc.cyclingDelayInWeeks() == tc2.cyclingDelayInWeeks());
        QVERIFY(tc.cyclingDelayInMonth() == tc2.cyclingDelayInMonth());
        QVERIFY(tc.cyclingDelayInYears() == tc2.cyclingDelayInYears());
        QVERIFY(tc.cyclingDelayInDecades() == tc2.cyclingDelayInDecades());
        // TODO: manage this
//        qWarning() << tc.currentCycleStartDate() << tc2.currentCycleStartDate();
//        qWarning() << tc.currentCycleExpirationDate() << tc2.currentCycleExpirationDate();
//        QVERIFY(tc.currentCycleStartDate() == tc2.currentCycleStartDate());
//        QVERIFY(tc.currentCycleExpirationDate() == tc2.currentCycleExpirationDate());

        QVERIFY(tc2.isModified() == false);
        tc.setModified(false);
        QVERIFY(tc == tc2);
    }
}

void AlertPlugin::test_alertitem_object()
{
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    // Test the AlertItem XML interface
    for(int i=0; i < loop; ++i) {
        AlertItem item = createVirtualItem(true);
        QVERIFY(item.relations().count() > 0);
        QVERIFY(item.timings().count() > 0);
        QVERIFY(item.scripts().count() > 0);
        QVERIFY(item.validations().count() > 0);
        AlertItem item2 = AlertItem::fromXml(item.toXml());
        verifyAlertEquality(item, item2);
        QVERIFY(item2.isModified() == false);
    }

    // Test sort interface
    AlertItem item1 = createVirtualItem();
    item1.setLabel("1");
    AlertItem item2 = createVirtualItem();
    item2.setLabel("2");
    AlertItem item3 = createVirtualItem();
    item3.setLabel("3");
    AlertItem item4 = createVirtualItem();
    item4.setLabel("4");
    AlertItem item5 = createVirtualItem();
    item5.setLabel("5");
    AlertItem item6 = createVirtualItem();
    item6.setLabel("6");
    item1.setPriority(AlertItem::High);
    item2.setPriority(AlertItem::Low);
    item3.setPriority(AlertItem::High);
    item4.setPriority(AlertItem::Low);
    item5.setPriority(AlertItem::Medium);
    item6.setPriority(AlertItem::High);
    QList<AlertItem> items;
    items << item1 << item2 << item3 << item4 << item5 << item6;
    qSort(items.begin(), items.end(), AlertItem::priorityLowerThan);
    AlertItem::Priority p = AlertItem::High;
    foreach(const AlertItem &i, items) {
        if (i.priority() == p)
            continue;
        QVERIFY(i.priority() > p);
    }
    item1.setCategory("R");
    item2.setCategory("A");
    item3.setCategory("L");
    item4.setCategory("S");
    item5.setCategory("E");
    item6.setCategory("T");
    items.clear();
    items << item1 << item2 << item3 << item4 << item5 << item6;
    qSort(items.begin(), items.end(), AlertItem::categoryLowerThan);
    QString out = "AELRST";
    for(int i = 0; i < items.count(); ++i) {
        QVERIFY(QString(out.at(i)) == items.at(i).category());
    }

    // Test operator==() with multiple timings
    item1.clearTimings();
    item2 = item1;
    QDateTime dt = QDateTime::currentDateTime().addDays(-2);
    AlertTiming time1(dt, dt.addDays(3));
    AlertTiming time2(dt, dt.addDays(2));
    AlertTiming time3(dt.addDays(-2), dt.addDays(10));
    time1.setId(1);
    time3.setId(3);
    item1.addTiming(time1);
    item1.addTiming(time2);
    item1.addTiming(time3);
    item2.addTiming(time1);
    item2.addTiming(time3);
    item2.addTiming(time2);
    QVERIFY2(item1 == item2, "Checking AlertItem::operator==() with non-cycling timings");
    item2.clearTimings();
    item2.addTiming(time1);
    item2.addTiming(time2);
    item2.addTiming(time2);
    QVERIFY2(item1 != item2, "Checking AlertItem::operator==() with non-cycling timings");

    // Test operator==() with cycling timing also
    time1 = AlertTiming(dt, dt.addDays(3));
    time1.setCycling(true);
    time1.setCyclingDelayInDays(r.randomInt(1, 10));
    time1.setNumberOfCycles(r.randomInt(1, 20));
    time2 = AlertTiming(dt, dt.addDays(2));
    time3 = AlertTiming(dt.addDays(-2), dt.addDays(10));
    time3.setCycling(true);
    time3.setCyclingDelayInDays(r.randomInt(1, 10));
    time3.setNumberOfCycles(r.randomInt(1, 20));
    item1.clearTimings();
    item1.addTiming(time1);
    item1.addTiming(time2);
    item1.addTiming(time3);
    item2.clearTimings();
    item2.addTiming(time3);
    item2.addTiming(time1);
    item2.addTiming(time2);
    QVERIFY2(item1 == item2, "Checking AlertItem::operator==() with cycling timings");
    item2.clearTimings();
    item2.addTiming(time3);
    item2.addTiming(time1);
    item2.addTiming(time1);
    QVERIFY2(item1 != item2, "Checking AlertItem::operator==() with cycling timings");

    // Test operator==() with multiple relations
    item1.clearRelations();
    item1 = item2;
    AlertRelation rel1(AlertRelation::RelatedToAllPatients);
    AlertRelation rel2(AlertRelation::RelatedToUser, "user1");
    AlertRelation rel3(AlertRelation::RelatedToPatient, "patient1");
    AlertRelation rel4(AlertRelation::RelatedToApplication, "app1");
    item1.addRelation(rel1);
    QVERIFY2(item1 != item2, "Checking AlertItem::operator==() with multiple AlertRelations");
    item2.addRelation(rel1);
    QVERIFY2(item1 == item2, "Checking AlertItem::operator==() with multiple AlertRelations");
    item1.addRelation(rel2);
    item2.addRelation(rel2);
    QVERIFY2(item1 == item2, "Checking AlertItem::operator==() with multiple AlertRelations");
    item1.addRelation(rel3);
    item1.addRelation(rel4);
    item2.addRelation(rel4);
    item2.addRelation(rel3);
    QVERIFY2(item1 == item2, "Checking AlertItem::operator==() with multiple AlertRelations");

    // Test operator==() with multiple validations
    item1.clearValidations();
    item1 = item2;
    AlertValidation val1(QDateTime::currentDateTime(), "oioioi", "sdfsdqfdsf");
    val1.setAccepted(true);
    item1.addValidation(val1);
    QVERIFY2(item1 != item2, "Checking AlertItem::operator==() with multiple AlertValidation");
    item2.addValidation(val1);
    QVERIFY2(item1 == item2, "Checking AlertItem::operator==() with multiple AlertValidation");
    AlertValidation val2(QDateTime::currentDateTime(), "oioioi", "sdfsdqfdsf");
    val2.setAccepted(false);
    val2.setOverriden(true);
    val2.setUserComment("My comment é'\"--)°");
    item1.addValidation(val2);
    item2.addValidation(val2);
    QVERIFY2(item1 == item2, "Checking AlertItem::operator==() with multiple AlertValidation");
    AlertValidation val3(QDateTime::currentDateTime(), "dsfdfi", "sdf()àsd");
    AlertValidation val4(QDateTime::currentDateTime(), "df(di", "''lskdf");
    item1.addValidation(val3);
    item1.addValidation(val4);
    item2.addValidation(val4);
    item2.addValidation(val3);
    QVERIFY2(item1 == item2, "Checking AlertItem::operator==() with multiple AlertValidation");

    // Test operator==() with multiple scripts
    item1.clearScripts();
    item1 = item2;
    AlertScript script1 = createVirtualScript(r.randomInt(0, 6), r);
    item1.addScript(script1);
    QVERIFY2(item1 != item2, "Checking AlertItem::operator==() with multiple AlertScript");
    item2.addScript(script1);
    QVERIFY2(item1 == item2, "Checking AlertItem::operator==() with multiple AlertScript");
    AlertScript script2 = createVirtualScript(r.randomInt(0, 6), r);
    AlertScript script3 = createVirtualScript(r.randomInt(0, 6), r);
    AlertScript script4 = createVirtualScript(r.randomInt(0, 6), r);
    item1.addScript(script2);
    item1.addScript(script3);
    item1.addScript(script4);
    item2.addScript(script3);
    item2.addScript(script4);
    item2.addScript(script2);
    QVERIFY2(item1 == item2, "Checking AlertItem::operator==() with multiple AlertScript");
}

void AlertPlugin::test_alertcore_init()
{
    QVERIFY(alertCore().isInitialized()==true);
    QVERIFY(alertCore().alertBase().isInitialized()==true);
}

void AlertPlugin::test_alertbase_basics()
{
    QVector<AlertItem> test;
    Internal::AlertBaseQuery query;

    // Unique alert test (save, get, purge)
    for(int i=0; i < loop; ++i) {
        // Create a virtual alert
        AlertItem item = createVirtualItem(true);

        // Save it
        QVERIFY(alertBase().saveAlertItem(item) == true);
        uidsToPurge << item.uuid();
        QVERIFY(item.db(0).toInt() >= 0); // Test internal ID
        QVERIFY(item.isModified() == false);

        // Get it back from database & check equality
        query.getAlertItemFromUuid(item.uuid());
        test = alertBase().getAlertItems(query);
        QVERIFY(test.count() == 1);
        AlertItem &item2 = test[0];
        verifyAlertEquality(item, item2);
        QVERIFY(item2.isModified() == false);
        test.clear();

        // Purge it
        QVERIFY(alertBase().purgeAlertItem(item.uuid()) == true);
        uidsToPurge.removeAll(item.uuid());

        // Check that the alert no longuer exists in the database
        test = alertBase().getAlertItems(query);
        QVERIFY(test.count() == 0);
    }
}

void AlertPlugin::test_alertbase_cycling_alerts()
{
    QVector<AlertItem> test;
    Internal::AlertBaseQuery query;

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    for(int i=0; i < loop; ++i) {
        // Create a virtual cycling alert
        AlertItem item = createVirtualItem(true);
        AlertTiming cycling;
        cycling.setCycling(true);
        QDateTime start = QDateTime::currentDateTime().addMonths(-r.randomInt(1, 10));
        QDateTime expiration = start.addMonths(r.randomInt(1000, 100000));
        cycling.setStart(start);
        cycling.setExpiration(expiration);
        switch (r.randomInt(0, 5)) {
        case 0: cycling.setCyclingDelayInMinutes(r.randomInt(1000, 100000)); break;
        case 1: cycling.setCyclingDelayInHours(r.randomInt(10, 1000)); break;
        case 2: cycling.setCyclingDelayInDays(r.randomInt(10, 100)); break;
        case 3: cycling.setCyclingDelayInWeeks(r.randomInt(10, 100)); break;
        case 4: cycling.setCyclingDelayInMonth(r.randomInt(1, 10)); break;
        case 5: cycling.setCyclingDelayInYears(r.randomInt(1, 10)); break;
        }
        cycling.setNumberOfCycles(r.randomInt(1, 100));
        item.clearTimings();
        item.addTiming(cycling);
        QVERIFY(item.timingAt(0) == cycling);

        // Save it
        QVERIFY(alertBase().saveAlertItem(item) == true);
        uidsToPurge << item.uuid();
        QVERIFY(item.db(0).toInt() >= 0); // Test internal ID
        QVERIFY(item.isModified() == false);

        // Get it back from database & check equality
        query.getAlertItemFromUuid(item.uuid());
        test = alertBase().getAlertItems(query);
        QVERIFY(test.count() == 1);
        AlertItem &item2 = test[0];
        verifyAlertEquality(item, item2);
        QVERIFY(item2.isModified() == false);
        test.clear();

        // Purge it
        QVERIFY(alertBase().purgeAlertItem(item.uuid()) == true);
        uidsToPurge.removeAll(item.uuid());

        // Check that the alert no longuer exists in the database
        test = alertBase().getAlertItems(query);
        QVERIFY(test.count() == 0);
    }

    // TODO: Check alert updates
}

void AlertPlugin::test_alertbase_complex_query()
{
    // Test some specifics
    QVector<AlertItem> test;
    Internal::AlertBaseQuery query;
    QDateTime start = QDateTime::currentDateTime().addSecs(-60*60*24);
    QDateTime expiration = QDateTime::currentDateTime().addSecs(60*60*24);

    AlertItem item; // = alertBase().createVirtualItem();
    item.setUuid(Utils::Database::createUid());
    item.setThemedIcon("identity.png");
    item.setLabel(item.label() + " (item)");
    item.setCategory("Test");
    item.setDescription("Simple basic non-blocking alert");
    item.setViewType(AlertItem::NonBlockingAlert);
    item.setPriority(AlertItem::High);
    item.setRemindLaterAllowed(true);
    item.clearRelations();
    item.addRelation(AlertRelation(AlertRelation::RelatedToPatient, "patient1"));
    item.clearTimings();
    item.addTiming(AlertTiming(start, expiration));

    AlertItem item2; // = alertBase().createVirtualItem();
    item2.setUuid(Utils::Database::createUid());
    item2.setThemedIcon("next.png");
    item2.setLabel("Just a simple alert (item2)");
    item2.setCategory("Test");
    item2.setDescription("Simple basic static alert");
    item2.setViewType(AlertItem::NonBlockingAlert);
    item2.setPriority(AlertItem::Low);
    item2.setOverrideRequiresUserComment(true);
    item2.clearRelations();
    item2.addRelation(AlertRelation(AlertRelation::RelatedToPatient, "patient1"));
    item2.addRelation(AlertRelation(AlertRelation::RelatedToPatient, "patient2"));
    item2.clearTimings();
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

    // Database saving
    QVERIFY(alertBase().saveAlertItem(item) == true);
    uidsToPurge << item.uuid();
    QVERIFY(alertBase().saveAlertItem(item2) == true);
    uidsToPurge << item2.uuid();
    QVERIFY(alertBase().saveAlertItem(item3) == true);
    uidsToPurge << item3.uuid();
    QVERIFY(alertBase().saveAlertItem(item4) == true);
    uidsToPurge << item4.uuid();
    QVERIFY(alertBase().saveAlertItem(item5) == true);
    uidsToPurge << item5.uuid();
    QVERIFY(alertBase().saveAlertItem(item6) == true);
    uidsToPurge << item6.uuid();
    QVERIFY(alertBase().saveAlertItem(item7) == true);
    uidsToPurge << item7.uuid();
    QVERIFY(alertBase().saveAlertItem(item8) == true);
    uidsToPurge << item8.uuid();
    QVERIFY(alertBase().saveAlertItem(item9) == true);
    uidsToPurge << item9.uuid();
    QVERIFY(alertBase().saveAlertItem(item10) == true);
    uidsToPurge << item10.uuid();
    QVERIFY(alertBase().saveAlertItem(item11) == true);
    uidsToPurge << item11.uuid();

    // Get all user1 related alerts -> item6
    query.clear();
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    query.addUserAlerts("user1");
    test = alertBase().getAlertItems(query);
    QVERIFY2(test.count() == 1, "Get all user1 related alerts -> item6");
    QVERIFY(test.at(0).uuid() == item6.uuid());

    // Get all patient1 alerts -> all but item6
    query.clear();
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    query.addPatientAlerts("patient1");
    test = alertBase().getAlertItems(query);
    QVERIFY2(test.count() == 10, "Get all patient1 alerts -> all but item6");
    for(int i=0; i < test.count(); ++i)
        QVERIFY(test.at(i).uuid() != item6.uuid());

    // Get all patient2 alerts -> all but item1 3 5 6
    query.clear();
    query.setAlertValidity(Internal::AlertBaseQuery::ValidAlerts);
    query.addPatientAlerts("patient2");
    test = alertBase().getAlertItems(query);
    QVERIFY2(test.count() == 7, "Get all patient2 alerts -> all but item1 3 5 6");
    for(int i=0; i < test.count(); ++i) {
        QVERIFY(test.at(i).uuid() != item.uuid());
        QVERIFY(test.at(i).uuid() != item3.uuid());
        QVERIFY(test.at(i).uuid() != item5.uuid());
        QVERIFY(test.at(i).uuid() != item6.uuid());
    }
    // TODO: test date invalid alerts in queries (finished or not started)
    // TODO: test cycling alert querying
}

void AlertPlugin::test_alertbase_complex_query_with_cycling_alerts()
{
    QVector<AlertItem> test;
    Internal::AlertBaseQuery query;

    // Create a valid cycling alert (first cycle)
    AlertItem item = createVirtualItem();
    item.setViewType(AlertItem::NonBlockingAlert);
    item.clearTimings();
    // Cycling: start one month ago, expire one year later, cycle every month
    AlertTiming cycling;
    cycling.setCycling(true);
    QDateTime start = QDateTime::currentDateTime().addMonths(-1);
    QDateTime expiration = start.addYears(1);
    cycling.setStart(start);
    cycling.setExpiration(expiration);
    cycling.setCyclingDelayInMonth(1);
    item.addTiming(cycling);
}

void AlertPlugin::test_blockingalert_dialog()
{
    // Alert without required usercomment
    QDateTime start = QDateTime::currentDateTime().addSecs(-60*60*24);
    QDateTime expiration = QDateTime::currentDateTime().addSecs(60*60*24);
    AlertItem item; // = alertBase().createVirtualItem();
    item.setUuid(Utils::Database::createUid());
    item.setThemedIcon("identity.png");
    item.setLabel(item.label() + " (item)");
    item.setCategory("Test");
    item.setDescription("Simple basic blocking alert");
    item.setViewType(AlertItem::BlockingAlert);
    item.setPriority(AlertItem::High);
    item.setOverrideRequiresUserComment(false);
    item.setRemindLaterAllowed(true);
    item.clearRelations();
    item.addRelation(AlertRelation(AlertRelation::RelatedToPatient, "patient1"));
    item.clearTimings();
    item.addTiming(AlertTiming(start, expiration));

    // see doc of void BlockingAlertDialog::test_dialog()
    BlockingAlertResult result = BlockingAlertDialog::executeBlockingAlert(item);
    QVERIFY(result.isAccepted() == true);
    QVERIFY(result.isOverridenByUser() == false);
    QVERIFY(result.isRemindLaterRequested() == false);
    QVERIFY(result.overrideUserComment().isEmpty());
    // TODO: test QVERIFY(result.alertValidation());

    item.setOverrideRequiresUserComment(true);
    result = BlockingAlertDialog::executeBlockingAlert(item);
    QVERIFY(result.isAccepted() == false);
    QVERIFY(result.isOverridenByUser() == true);
    QVERIFY(result.isRemindLaterRequested() == false);
    QVERIFY(!result.overrideUserComment().isEmpty());
    QVERIFY(result.overrideUserComment() == "Override this");

    // TODO: test dialog with a extra-button

//    QToolButton *test = new QToolButton;
//    test->setText("Houlala");
//    test->setToolTip("kokokokokok");
//    QList<QAbstractButton*> buttons;
//    buttons << test;
}

// TODO: test alertscript execution
// TODO: test alertplaceholder (add, remove, update alerts)
// TODO: test alertpacks
// TODO: test alerteditor

void AlertPlugin::cleanupTestCase()
{
    // Database purge
    foreach(const QString &uid, uidsToPurge) {
        QVERIFY(alertBase().purgeAlertItem(uid) == true);
    }
}

// Alert editor
//    if (false) {
//        AlertItemEditorDialog dlg;
//        dlg.setEditableParams(AlertItemEditorDialog::FullDescription | AlertItemEditorDialog::Timing);
//        dlg.setEditableParams(AlertItemEditorDialog::Label | AlertItemEditorDialog::Timing);
//        dlg.setEditableParams(AlertItemEditorDialog::Label | AlertItemEditorDialog::Timing | AlertItemEditorDialog::Types);

//        AlertTiming &time = item.timingAt(0);
//        time.setCycling(true);
//        time.setCyclingDelayInDays(10);
//        dlg.setAlertItem(item);
//        if (dlg.exec()==QDialog::Accepted) {
//            dlg.submit(item);
//        }
//        qWarning() << item.toXml();
//    }

// Alert packs
//    if (true) {
//        q->registerAlertPack(settings()->path(Core::ISettings::BundledAlertPacks) + "/test");
//    }

//    // PlaceHolders
//    if (true) {
//        // Put placeholder in the plugin manager object pool
//        _placeholdertest = new AlertPlaceHolderWidget; // object should not be deleted
//        pluginManager()->addObject(_placeholdertest);

//        // Create the dialog && the placeholder
//        QDialog dlg;
//        QVBoxLayout lay(&dlg);
//        dlg.setLayout(&lay);
//        lay.addWidget(_placeholdertest->createWidget(&dlg));

//        // Check alerts
//        q->checkAlerts(AlertCore::CurrentPatientAlerts | AlertCore::CurrentUserAlerts | AlertCore::CurrentApplicationAlerts);

//        // Exec the dialog
//        dlg.exec();
//    }

