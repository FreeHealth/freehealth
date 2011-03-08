/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "virtualdatabasepreferences.h"

#include <usermanagerplugin/usermodel.h>

#include <patientbaseplugin/patientbase.h>
#include <patientbaseplugin/patientmodel.h>
#include <patientbaseplugin/constants_db.h>

#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/episodebase.h>
#include <formmanagerplugin/constants_db.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>

#include <utils/randomizer.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>
#include <QProgressDialog>
#include <QByteArray>
#include <QBuffer>

static inline UserPlugin::UserModel *userModel() {return UserPlugin::UserModel::instance();}
static inline Patients::Internal::PatientBase *patientBase()  { return Patients::Internal::PatientBase::instance(); }
static inline Patients::PatientModel *patientModel()  { return Patients::PatientModel::activeModel(); }
static inline Form::Internal::EpisodeBase *episodeBase()  { return Form::Internal::EpisodeBase::instance(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;


static inline void createPatient(const QString &name, const QString &secondname, const QString &firstname,
                                 const QString &gender, const int title, const QDate &dob,
                                 const QString &country, const QString &note,
                                 const QString &street, const QString &zip, const QString &city,
                                 const QString &uuid, const int lkid,
                                 const QString &photoFile = QString(), const QDate &death = QDate())
{
    using namespace Patients;
    QSqlQuery query(patientBase()->database());
    query.prepare(patientBase()->prepareInsertQuery(Constants::Table_IDENT));
    query.bindValue(Constants::IDENTITY_ID, QVariant());
    query.bindValue(Constants::IDENTITY_UID, uuid);
    query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, lkid);
    query.bindValue(Constants::IDENTITY_FAMILY_UID, "Not yet implemented");
    query.bindValue(Constants::IDENTITY_ISVIRTUAL, 1);
    query.bindValue(Constants::IDENTITY_NAME, name);
    query.bindValue(Constants::IDENTITY_FIRSTNAME, firstname);
    if (secondname.isEmpty())
        query.bindValue(Constants::IDENTITY_SECONDNAME, QVariant());
    else
        query.bindValue(Constants::IDENTITY_SECONDNAME, secondname);
    query.bindValue(Constants::IDENTITY_GENDER, gender);
    query.bindValue(Constants::IDENTITY_TITLE, title);
    query.bindValue(Constants::IDENTITY_DOB, dob);
    query.bindValue(Constants::IDENTITY_MARITAL_STATUS, QVariant());
    if (death.isValid()) {
        query.bindValue(Constants::IDENTITY_ISACTIVE, 0);
        query.bindValue(Constants::IDENTITY_DATEOFDEATH, death);
    } else {
        query.bindValue(Constants::IDENTITY_ISACTIVE, 1);
        query.bindValue(Constants::IDENTITY_DATEOFDEATH, QVariant());
    }
    query.bindValue(Constants::IDENTITY_PROFESSION, QVariant());
    query.bindValue(Constants::IDENTITY_ADDRESS_STREET, street);
    query.bindValue(Constants::IDENTITY_ADDRESS_STREET_NUMBER, QVariant());
    query.bindValue(Constants::IDENTITY_ADDRESS_ZIPCODE, zip);
    query.bindValue(Constants::IDENTITY_ADRESS_CITY, city);
    query.bindValue(Constants::IDENTITY_ADDRESS_COUNTRY, country);
    query.bindValue(Constants::IDENTITY_ADDRESS_NOTE, note);
    query.bindValue(Constants::IDENTITY_MAILS, QVariant());
    query.bindValue(Constants::IDENTITY_TELS, QVariant());
    query.bindValue(Constants::IDENTITY_FAXES, QVariant());

    if (!query.exec()) {
        Utils::Log::addQueryError("VirtualBasePage", query);
        qWarning() << name << secondname << firstname << gender << title<< dob<<country<<note<<street<<zip<<city<<uuid<<lkid<<photoFile<<death;
    }
    query.finish();

    if (!photoFile.isEmpty()) {
        QPixmap pix(photoFile);
        if (pix.isNull())
            return;
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        pix.save(&buffer, "PNG"); // writes image into ba in PNG format {6a247e73-c241-4556-8dc8-c5d532b8457e}
        query.prepare(patientBase()->prepareInsertQuery(Constants::Table_PATIENT_PHOTO));
        query.bindValue(Constants::PHOTO_ID, QVariant());
        query.bindValue(Constants::PHOTO_UID, QUuid::createUuid().toString());
        query.bindValue(Constants::PHOTO_PATIENT_UID, uuid);
        query.bindValue(Constants::PHOTO_BLOB, ba);
        query.exec();
        if (!query.isActive()) {
            Utils::Log::addQueryError("VirtualBasePage", query);
        }
    }
}


static inline void refreshPatientModel()
{
    if (patientModel())
        patientModel()->refreshModel();
}

VirtualDatabasePreferences::VirtualDatabasePreferences(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
}

void VirtualDatabasePreferences::writeDefaultSettings(Core::ISettings *)
{
    Utils::Log::addMessage("VirtualDatabasePreferences", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("VirtualDatabasePreferences"));
    QHash<int, QString> where;
    where.insert(Patients::Constants::IDENTITY_NAME, "LIKE 'DOE'");
    int c = patientBase()->count(Patients::Constants::Table_IDENT, Patients::Constants::IDENTITY_NAME, patientBase()->getWhereClause(Patients::Constants::Table_IDENT, where));
    if (!c) {
        int userLkId = userModel()->practionnerLkIds(userModel()->currentUserData(Core::IUser::Uuid).toString()).at(0);
        QString path = settings()->path(Core::ISettings::BigPixmapPath) + QDir::separator();

        QString uid = QUuid::createUuid().toString();
        createPatient("DOE", "Junior", "John", "M", 1, QDate(1990, 10, 20), "France", "Patient is lost...",
                      "21, street Doe", "21213", "NoWhereLand", uid, userLkId, path+"johndoe.jpg");

        uid = QUuid::createUuid().toString();
        createPatient("DOE", "ComicsGirl", "Jane", "F", 2, QDate(1985, 04, 20), "France", "SuperJane owns to an unknown user",
                      "21, street Jane", "21213", "JaneLand", uid, userLkId);

        uid = QUuid::createUuid().toString();
        createPatient("KIRK", "", "James Tiberius", "M", 6, QDate(1968, 04, 20), "USA", "USS Enterprise",
                      "21, StarFleet Command", "1968", "EarthTown", uid, userLkId, path+"captainkirk.jpg");

        uid = QUuid::createUuid().toString();
        createPatient("PICARD", "", "Jean-Luc", "M", 6, QDate(1968, 04, 20), "USA", "USS Enterprise",
                      "21, StarFleet Command", "1968", "EarthTown", uid, userLkId, path+"captainpicard.jpg");

        refreshPatientModel();
    }
}

void VirtualDatabasePreferences::on_populateDb_clicked()
{
    writeDefaultSettings(0);
    // Prepare virtual patients
    int nb = nbVirtualPatients->value();
    int userLkId = userModel()->practionnerLkIds(userModel()->currentUserData(Core::IUser::Uuid).toString()).at(0);
    QProgressDialog dlg(tr("Creating %1 virtual patients").arg(nb), tr("Cancel"), 0, nb, qApp->activeWindow());
    dlg.setWindowModality(Qt::WindowModal);

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    for(int i = 0; i < nb; ++i) {
        if (i % 100 == 0) {
            dlg.setValue(i);
            patientBase()->database().transaction();
        }
        QString name, sur, sec, g;
        int title, lk;
        QDate death, dob;

        name = r.getRandomName();
        dob = r.randomDate(1910);

        if (r.randomInt(2) == 1) {
            g = "F";
            sur= r.getRandomFirstname(false);
            sec = r.getRandomName();
            title = 2;
        } else {
            g = "M";
            sur= r.getRandomFirstname(true);
            sec = r.getRandomName();
            title = 1;
        }

        if (r.randomInt(2)) {
            death = r.randomDate(1980);
        }
        QPair<int, QString> p = r.getRandomFrenchCity();
        if (r.randomInt(3) == 1)
            lk =  userLkId + 1;
        else
            lk = userLkId;

        createPatient(name,sec, sur,g,title,
                      dob,"France","",r.getRandomString(65),
                      QString::number(p.first), p.second,
                      QUuid::createUuid().toString(), lk, "", death);

        if (i % 100 == 99)
            patientBase()->database().commit();

        qApp->processEvents();
        if (dlg.wasCanceled())
            break;
    }
    patientBase()->database().commit();

    refreshPatientModel();
}

void VirtualDatabasePreferences::on_populateEpisodes_clicked()
{
    // Prepare virtual episodes
    QSqlQuery query(patientBase()->database());
    int nb = nbVirtualEpisodes->value();
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    // Retreive all patients and create n episodes for them
    QSet<QString> patients;
    query.exec(patientBase()->selectDistinct(Patients::Constants::Table_IDENT, Patients::Constants::IDENTITY_UID));
    if (query.isActive()) {
        while (query.next())
            patients << query.value(0).toString();
    } else {
        Utils::Log::addQueryError(this, query);
    }
    query.finish();

    QProgressDialog dlg(tr("Creating %1 virtual episodes").arg(nb * patients.count()), tr("Cancel"), 0, nb * patients.count(), qApp->activeWindow());
    dlg.setWindowModality(Qt::WindowModal);

    QList<QString> testingForms;
    testingForms << "episodeTester_1" << "episodeTester_2" << "episodeTester_2.1"
            << "episodeTester_2.1.1" << "episodeTester_2.2";

    int zz = 0;
    int userLkId = userModel()->practionnerLkIds(userModel()->currentUserData(Core::IUser::Uuid).toString()).at(0);
    QSqlQuery episodeQuery(episodeBase()->database());

    foreach(const QString &form, testingForms) {
        foreach(const QString &uid, patients) {
            using namespace Form;
            QDateTime date = QDateTime(r.randomDate(2008), QTime(r.randomInt(23), r.randomInt(59), r.randomInt(59), 0));

            for(int i = 0; i < nb; ++i) {
                if (zz % 1000 == 0) {
                    dlg.setValue(zz);
                    episodeBase()->database().transaction();
                }
                date = r.randomDateTime(date);
                if (date > QDateTime::currentDateTime())
                    break;

                episodeQuery.prepare(episodeBase()->prepareInsertQuery(Constants::Table_EPISODES));

                episodeQuery.bindValue(Constants::EPISODES_ID, QVariant());
                episodeQuery.bindValue(Constants::EPISODES_PATIENT_UID, uid);
                if (r.randomInt(3) == 1)
                    episodeQuery.bindValue(Constants::EPISODES_LK_TOPRACT_LKID, userLkId + 1);
                else
                    episodeQuery.bindValue(Constants::EPISODES_LK_TOPRACT_LKID, userLkId);
                episodeQuery.bindValue(Constants::EPISODES_FORM_PAGE_UID, form);
                episodeQuery.bindValue(Constants::EPISODES_DATE, date);
                episodeQuery.bindValue(Constants::EPISODES_DATEOFCREATION, date);
                episodeQuery.bindValue(Constants::EPISODES_DATEOFMODIFICATION, QVariant());
                episodeQuery.bindValue(Constants::EPISODES_DATEOFVALIDATION, QVariant());
                episodeQuery.bindValue(Constants::EPISODES_VALIDATED, QVariant());
                QString tmp;
                for(int z=0; z < r.randomInt(6); ++z)
                    tmp += r.getRandomName() + " ";
                tmp.chop(1);
                episodeQuery.bindValue(Constants::EPISODES_LABEL, tmp);

                if (!episodeQuery.exec()) {
                    Utils::Log::addQueryError(this, episodeQuery);
                }
                episodeQuery.finish();

                if (zz % 1000 == 99)
                    episodeBase()->database().commit();

                qApp->processEvents();
                if (dlg.wasCanceled())
                    break;
                ++zz;
            }
            episodeBase()->database().commit();
            if (dlg.wasCanceled())
                break;
        }
        if (dlg.wasCanceled())
            break;
    }
}

void VirtualDatabasePreferences::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void VirtualDatabasePreferences::on_populateUsers_clicked()
{
    int nb = nbVirtualUsers->value();
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    QProgressDialog dlg(tr("Creating %1 virtual users").arg(nb), tr("Cancel"), 0, nb, qApp->activeWindow());
    dlg.setWindowModality(Qt::WindowModal);

    for(int i = 0; i < nb ; ++i) {
        dlg.setValue(i);
        using namespace UserPlugin;
        userModel()->insertRow(0);
        int genderIndex = r.randomInt(1);
        userModel()->setData(userModel()->index(0, Core::IUser::Name), r.getRandomName());
        userModel()->setData(userModel()->index(0, Core::IUser::Firstname), r.getRandomFirstname(genderIndex==1));
        userModel()->setData(userModel()->index(0, Core::IUser::TitleIndex), 4);
        userModel()->setData(userModel()->index(0, Core::IUser::GenderIndex), genderIndex);
        userModel()->submitUser(userModel()->index(0, Core::IUser::Uuid).data().toString());
    }
}
