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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "virtualdatabasepreferences.h"

#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>

#include <patientbaseplugin/patientbase.h>
#include <patientbaseplugin/patientcore.h>
#include <patientbaseplugin/constants_db.h>

#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/episodebase.h>
#include <formmanagerplugin/constants_db.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <translationutils/constanttranslations.h>

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QProgressDialog>
#include <QByteArray>
#include <QBuffer>

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Patients::Internal::PatientBase *patientBase()  { return Patients::Internal::PatientBase::instance(); }
static inline Patients::PatientCore *patientCore()  { return Patients::PatientCore::instance(); }
static inline Form::Internal::EpisodeBase *episodeBase()  { return Form::Internal::EpisodeBase::instance(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}

using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;

VirtualDatabasePreferences::VirtualDatabasePreferences(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
}

void VirtualDatabasePreferences::writeDefaultSettings(Core::ISettings *)
{
    LOG_FOR("VirtualDatabasePreferences", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("VirtualDatabasePreferences"));
}

void VirtualDatabasePreferences::on_populateDb_clicked()
{
    writeDefaultSettings(0);
    // Prepare virtual patients
    int nb = nbVirtualPatients->value();
    QProgressDialog dlg(tr("Creating %1 virtual patients").arg(nb), tr("Cancel"), 0, nb, qApp->activeWindow());
    dlg.setWindowModality(Qt::WindowModal);

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    for(int i = 0; i < nb; ++i) {
        if (i % 100 == 0) {
            patientBase()->database().transaction();
            dlg.setValue(i);
        }
        QString name, sur, sec, g;
        int title;
        QDate death, dob;

        while (name.isEmpty())
            name = r.randomName();
        dob = r.randomDate(1910);

        if (r.randomInt(2) == 1) {
            g = "F";
            sur= r.randomFirstName(false);
            sec = r.randomName();
            title = 2;
        } else {
            g = "M";
            sur= r.randomFirstName(true);
            sec = r.randomName();
            title = 1;
        }

        if (r.randomInt(2)) {
            death = r.randomDate(1980);
        }
        QPair<int, QString> p = r.randomFrenchCity();

        // int LkId is set to 0, this column will be removed in a future release
        patientBase()->createVirtualPatient(name,sec, sur,g,title,
                      dob,"France","",r.randomString(65),
                      QString::number(p.first), p.second,
                      Utils::Database::createUid(), 0, "", "", "", death, user()->quuid());

        if (i % 100 == 99)
            patientBase()->database().commit();

        qApp->processEvents();
        if (dlg.wasCanceled())
            break;
    }
    patientBase()->database().commit();

    patientCore()->refreshAllPatientModel();
}

void VirtualDatabasePreferences::on_populateEpisodes_clicked()
{
    // Prepare virtual episodes
    QSqlQuery query(patientBase()->database());
    int nb = nbVirtualEpisodes->value();
    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    // Retrieve all patients and create n episodes for each patient
    QSet<QString> patients;
    if (query.exec(patientBase()->selectDistinct(Patients::Constants::Table_IDENT, Patients::Constants::IDENTITY_UID))) {
        while (query.next())
            patients << query.value(0).toString();
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    QProgressDialog dlg(tr("Creating %1 virtual episodes").arg(nb * patients.count()), tr("Cancel"), 0, nb * patients.count(), qApp->activeWindow());
    dlg.setWindowModality(Qt::WindowModal);

    QList<QString> testingForms;
    testingForms << "episodeTester_1" << "baseWidgetsFormSample";

    int zz = 0;
    qDebug() << "episodeBase()->database()" << episodeBase()->database();
    QSqlQuery episodeQuery(episodeBase()->database());

    const QString &userUid = user()->uuid();
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
                // EPISODES_LK_TOPRACT_LKID column is not used and will be removed in a future release.
                episodeQuery.bindValue(Constants::EPISODES_LK_TOPRACT_LKID, QVariant());
                episodeQuery.bindValue(Constants::EPISODES_ISVALID, QVariant(1));
                episodeQuery.bindValue(Constants::EPISODES_FORM_PAGE_UID, form);
                QString tmp;
                for(int z=0; z < r.randomInt(6); ++z)
                    tmp += r.randomName() + " ";
                tmp.chop(1);
                episodeQuery.bindValue(Constants::EPISODES_LABEL, tmp);
                episodeQuery.bindValue(Constants::EPISODES_USERDATETIME, date);
                episodeQuery.bindValue(Constants::EPISODES_EPISODECREATIONDATETIME, date);
                episodeQuery.bindValue(Constants::EPISODES_USERCREATOR, userUid);
                episodeQuery.bindValue(Constants::EPISODES_PRIORITY, 1);

                qDebug() << episodeQuery.lastQuery();
                qDebug() << episodeBase()->getLastExecutedQuery(episodeQuery);
                if (!episodeQuery.exec()) {
                    LOG_QUERY_ERROR(episodeQuery);
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
    userModel()->createVirtualUsers(nb);
}

