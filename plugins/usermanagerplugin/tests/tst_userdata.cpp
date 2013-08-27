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
#include "../usermanagerplugin.h"
#include "../userdata.h"
#include "../usercore.h"
#include "../database/userbase.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>

#include <QTest>

#define CHECK_MODIFIED QCOMPARE(data.isModified(), true); data.setModified(false); QCOMPARE(data.isModified(), false);

using namespace UserPlugin;
using namespace Internal;

//static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
//static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}

namespace {
const int loop = 10; // number of tests per objects
} // anonymous namespace

void UserManagerPlugin::initTestCase()
{
}

void UserManagerPlugin::test_userdynamicdata_basics()
{
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    // Test Ctor
    UserDynamicData data;
    QCOMPARE(data.isNull(), true);
    QCOMPARE(data.isModified(), false);
    QCOMPARE(data.type(), UserDynamicData::String);
    QCOMPARE(data.name(), QString());
    QCOMPARE(data.id(), -1);
    QCOMPARE(data.value(), QVariant());

    // Test setters/getters
    for(int i = 0; i < loop; ++i) {
        UserDynamicData data;
        int id = r.randomInt(1, 1000);
        QString name = r.randomString(r.randomInt(5, 20));
        QString user = r.randomString(r.randomInt(5, 20));
        QString value = r.randomString(r.randomInt(5, 20));
        data.setId(id); CHECK_MODIFIED;
        data.setName(name); CHECK_MODIFIED;
        data.setUserUuid(user); CHECK_MODIFIED;
        data.setValue(value); CHECK_MODIFIED;
        QCOMPARE(data.id(), id);
        QCOMPARE(data.name(), name);
        QCOMPARE(data.value().toString(), value);

        // Test operator==()
        UserDynamicData data2;
        data2.setId(id);
        data2.setName(name);
        data2.setUserUuid(user);
        data2.setValue(value);
        data2.setModified(false);
        QCOMPARE(data2.id(), data.id());
        QCOMPARE(data2.name(), data.name());
        QCOMPARE(data2.value(), data.value());
        QCOMPARE(data2.type(), data.type());
        QVERIFY(data2 == data);
        QVERIFY(data == data);
        QVERIFY(data2 == data2);
    }

    // TODO: test the following members
//    Print::TextDocumentExtra *extraDocument() const;
//    void prepareQuery(QSqlQuery &bindedQuery) const;
}

void UserManagerPlugin::test_userdata_basics()
{
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    // Test Ctor
    UserData data;
    QCOMPARE(data.isCurrent(), false);
    QCOMPARE(data.isEditable(), true);
    QCOMPARE(data.isModified(), false);
    QCOMPARE(data.isPasswordModified(), false);
    QCOMPARE(data.isNull(), true);
    QCOMPARE(data.isEmpty(), true);

    // Test non editable object
    data.setEditable(false);
    QCOMPARE(data.isEditable(), false);
    for(int i=0; i < Constants::USER_MaxParam; ++i) {
        QVariant control;
        switch (i) {
        case Constants::USER_ID: control = -1; break;
        case Constants::USER_UUID: control = data.uuid(); break;
        case Constants::USER_PASSWORD: control = Utils::cryptPassword(""); break;
        case Constants::USER_ISVIRTUAL: control = false; break;
        case Constants::USER_LOCKER: control = false; break;
        default: break;
        }
        QCOMPARE(data.value(Constants::Table_USERS, i), control);
        data.setValue(Constants::Table_USERS, i, QString("po"));
        QCOMPARE(data.value(Constants::Table_USERS, i), control);
        QCOMPARE(data.isModified(), false);
    }
    QCOMPARE(data.dynamicDataValue("blabla"), QVariant());
    data.setDynamicDataValue("blabla", "-----");
    QCOMPARE(data.dynamicDataValue("blabla"), QVariant());
    QCOMPARE(data.isModified(), false);

    // Test editable object (modified state)
    data.setEditable(true);
    QCOMPARE(data.isEditable(), true);
    QCOMPARE(data.isModified(), false);
    for(int i=0; i < Constants::USER_MaxParam; ++i) {
        data.setValue(Constants::Table_USERS, i, QString("po"));
        QCOMPARE(data.value(Constants::Table_USERS, i), QVariant("po"));
        CHECK_MODIFIED;
    }
    data.setDynamicDataValue("blabla", QVariant("àç"));
    QCOMPARE(data.dynamicDataValue("blabla"), QVariant("àç"));

    // Test setters/getters
    for(int i = 0; i < loop; ++i) {
        UserData data;
        int id = r.randomInt(1, 1000);
        bool current = r.randomBool();
        bool validity = r.randomBool();
        bool isvirtual = r.randomBool();
        bool locker = r.randomBool();
        int titleid = r.randomInt(0, 5);
        int genderid = r.randomInt(0, 2);
        data.setId(id); CHECK_MODIFIED;
        data.setValidity(validity); CHECK_MODIFIED;
        data.setVirtual(isvirtual); CHECK_MODIFIED;
        data.setLocker(locker); CHECK_MODIFIED;
        data.setTitleIndex(titleid); CHECK_MODIFIED;
        data.setGenderIndex(genderid); CHECK_MODIFIED;
        data.setCurrent(current); // does not alter modification state
        QCOMPARE(data.id(), id);
        QCOMPARE(data.validity(), validity);
        QCOMPARE(data.isVirtual(), isvirtual);
        QCOMPARE(data.locker(), locker);
        QCOMPARE(data.titleIndex(), titleid);
        QCOMPARE(data.genderIndex(), genderid);
        QCOMPARE(data.isCurrent(), current);

        QString clearPass = r.randomString(r.randomInt(5, 20));
        data.setClearPassword(clearPass); CHECK_MODIFIED;
        QCOMPARE(data.clearPassword(), clearPass);

        QString usualName = r.randomName();
        QString otherName = r.randomName();
        QString firstName = r.randomFirstName(r.randomBool());
        QString mail = QString("%1 <%2>").arg(r.randomString(r.randomInt(10, 50))).arg(r.randomString(r.randomInt(10, 50)));
        data.setUsualName(usualName); CHECK_MODIFIED;
        data.setOtherNames(otherName); CHECK_MODIFIED;
        data.setFirstname(firstName); CHECK_MODIFIED;
        data.setLanguageIso(QLocale().name().left(2)); CHECK_MODIFIED;
        data.setMail(mail); CHECK_MODIFIED;
        QDate dob = r.randomDate(QDate::currentDate().year() - 50);
        data.setDob(dob); CHECK_MODIFIED;
        // data.setPhoto(const QPixmap &pix);
        QCOMPARE(data.usualName(), usualName);
        QCOMPARE(data.otherNames(), otherName);
        QCOMPARE(data.firstname(), firstName);
        QCOMPARE(data.mail(), mail);
        QCOMPARE(data.dob(), dob);

        QString street = r.randomString(r.randomInt(5, 20));
        QString zip = r.randomString(r.randomInt(5, 20));
        QString prov = r.randomString(r.randomInt(5, 20));
        QString city = r.randomString(r.randomInt(5, 20));
        data.setStreet(street); CHECK_MODIFIED;
        data.setZipcode(zip); CHECK_MODIFIED;
        data.setStateProvince(prov); CHECK_MODIFIED;
        data.setCity(city); CHECK_MODIFIED;
        //data.setCountry();
        //data.setCountryIso();
        QCOMPARE(data.street(), street);
        QCOMPARE(data.zipcode(), zip);
        QCOMPARE(data.stateProvince(), prov);
        QCOMPARE(data.city(), city);
        //QCOMPARE(data.country(), country);

        QString tel1 = r.randomString(r.randomInt(5, 20));
        QString tel2 = r.randomString(r.randomInt(5, 20));
        QString tel3 = r.randomString(r.randomInt(5, 20));
        QString fax = r.randomString(r.randomInt(5, 20));
        data.setTel1(tel1); CHECK_MODIFIED;
        data.setTel2(tel2); CHECK_MODIFIED;
        data.setTel3(tel3); CHECK_MODIFIED;
        data.setFax(fax); CHECK_MODIFIED;
        QStringList tels;
        tels << tel1 << tel2 << tel3;
        QCOMPARE(data.tels(), tels);
        QCOMPARE(data.fax(), fax);

        QStringList practIds;
        practIds << r.randomString(r.randomInt(5, 20)) << r.randomString(r.randomInt(5, 20)) << r.randomString(r.randomInt(5, 20));
        data.setPractitionerIdentifiant(practIds); CHECK_MODIFIED;
        QCOMPARE(data.professionalIdentifiants(), practIds);

        QStringList spe;
        spe << r.randomString(r.randomInt(5, 20)) << r.randomString(r.randomInt(5, 20)) << r.randomString(r.randomInt(5, 20));
        data.setSpecialty(spe); CHECK_MODIFIED;
        QCOMPARE(data.specialty(), spe);

        QStringList qual;
        qual << r.randomString(r.randomInt(5, 20)) << r.randomString(r.randomInt(5, 20)) << r.randomString(r.randomInt(5, 20));
        data.setQualification(qual); CHECK_MODIFIED;
        QCOMPARE(data.qualifications(), qual);

        QString gh = r.randomString(r.randomInt(5, 200));
        QString gf = r.randomString(r.randomInt(5, 200));
        QString gw = r.randomString(r.randomInt(5, 200));
        data.setGenericHeader(gh); CHECK_MODIFIED;
        data.setGenericFooter(gf); CHECK_MODIFIED;
        data.setGenericWatermark(gw); CHECK_MODIFIED;

        QString ah = r.randomString(r.randomInt(5, 200));
        QString af = r.randomString(r.randomInt(5, 200));
        QString aw = r.randomString(r.randomInt(5, 200));
        data.setAdminHeader(ah); CHECK_MODIFIED;
        data.setAdminFooter(af); CHECK_MODIFIED;
        data.setAdminWatermark(aw); CHECK_MODIFIED;

        QString ph = r.randomString(r.randomInt(5, 200));
        QString pf = r.randomString(r.randomInt(5, 200));
        QString pw = r.randomString(r.randomInt(5, 200));
        data.setPrescriptionHeader(ph); CHECK_MODIFIED;
        data.setPrescriptionFooter(pf); CHECK_MODIFIED;
        data.setPrescriptionWatermark(pw); CHECK_MODIFIED;

        QString prefs = r.randomString(r.randomInt(5, 200));
        data.setPreferences(prefs); CHECK_MODIFIED;
    }
}

void UserManagerPlugin::test_usercore_initialization()
{
    QCOMPARE(userCore().isInitialized(), true);
    QCOMPARE(userCore().userBase()->isInitialized(), true);
}

void UserManagerPlugin::test_userbase_basics()
{
    //    bool checkDatabaseVersion();
    //    bool isNewlyCreated() const;
    //    QString getCurrentVersion() const;

    //    bool createDefaultUser();
    //    bool createVirtualUser(const QString &uid, const QString &name, const QString &firstName, int title, int gender,
    //                           const QStringList &specialties, const QStringList &qualifications,
    //                           int medicalRights = 0, int adminRights = 0, int userRights = 0, int agendaRights = 0, int paramedicRights = 0,
    //                           QLocale::Language lang = QLocale().language());

}

void UserManagerPlugin::cleanupTestCase()
{
}
