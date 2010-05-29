/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mainwindowpreferences.h"

#include <utils/log.h>
#include <utils/updatechecker.h>
#include <translationutils/constanttranslations.h>
#include <utils/randomizer.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/theme.h>
#include <coreplugin/constants.h>

#include <usermanagerplugin/usermodel.h>

#include <patientbaseplugin/patientbase.h>
#include <patientbaseplugin/constants_db.h>

#include <formmanagerplugin/episodemodel.h>
#include <formmanagerplugin/episodebase.h>
#include <formmanagerplugin/constants_db.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QUuid>
#include <QProgressDialog>
#include <QByteArray>
#include <QBuffer>


/** \todo create a virtual user creator */

using namespace MainWin::Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline UserPlugin::UserModel *userModel() {return UserPlugin::UserModel::instance();}
static inline Patients::Internal::PatientBase *patientBase()  { return Patients::Internal::PatientBase::instance(); }
static inline Form::Internal::EpisodeBase *episodeBase()  { return Form::Internal::EpisodeBase::instance(); }


MainWindowPreferencesPage::MainWindowPreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("MainWindowPreferencesPage"); }

MainWindowPreferencesPage::~MainWindowPreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString MainWindowPreferencesPage::id() const { return objectName(); }
QString MainWindowPreferencesPage::name() const { return tr("General"); }
QString MainWindowPreferencesPage::category() const { return tr("General"); }

void MainWindowPreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void MainWindowPreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void MainWindowPreferencesPage::finish() { delete m_Widget; }

void MainWindowPreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Utils::Constants::S_CHECKUPDATE, Utils::UpdateChecker::Check_AtStartup);
    defaultvalues.insert(Core::Constants::S_USE_EXTERNAL_DATABASE, false);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *MainWindowPreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new MainWindowPreferencesWidget(parent);
    return m_Widget;
}



MainWindowPreferencesWidget::MainWindowPreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDatasToUi();
    log->setIcon(theme()->icon(Core::Constants::ICONEYES));
    pass->setIcon(theme()->icon(Core::Constants::ICONEYES));
    pass->toogleEchoMode();
    if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool())
        on_testButton_clicked();
}

void MainWindowPreferencesWidget::setDatasToUi()
{
    updateCheckingCombo->setCurrentIndex(settings()->value(Utils::Constants::S_CHECKUPDATE).toInt());
    useExternalDB->setChecked(settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE).toBool());
    host->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST).toByteArray()));
    log->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG).toByteArray()));
    pass->setText(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS).toByteArray()));
    port->setValue(QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT).toByteArray())).toInt());
}

static void saveExternalDatabase(Core::ISettings *s, bool use, const QString &host, const QString &log, const QString &pass, const int port)
{
    if (s->value(Core::Constants::S_USE_EXTERNAL_DATABASE).toBool() != use) {
        s->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, use);
        s->setValue(Core::Constants::S_EXTERNAL_DATABASE_HOST, QString(host.toAscii().toBase64()));
        s->setValue(Core::Constants::S_EXTERNAL_DATABASE_PORT, QString::number(port).toAscii().toBase64());
        s->setValue(Core::Constants::S_EXTERNAL_DATABASE_LOG, QString(log.toAscii().toBase64()));
        s->setValue(Core::Constants::S_EXTERNAL_DATABASE_PASS, QString(pass.toAscii().toBase64()));
        Core::ICore::instance()->databaseServerLoginChanged();
    }
}

void MainWindowPreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(Utils::Constants::S_CHECKUPDATE, updateCheckingCombo->currentIndex());
    saveExternalDatabase(s, useExternalDB->isChecked(), host->text(), log->text(), pass->text(), port->value());
}

void MainWindowPreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
//    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("MainWindowPreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("FreeDiamsMainWindow"));
    s->setValue(Utils::Constants::S_CHECKUPDATE, Utils::UpdateChecker::Check_AtStartup);
    s->setValue(Core::Constants::S_USE_EXTERNAL_DATABASE, false);
    s->sync();
}

void MainWindowPreferencesWidget::on_testButton_clicked()
{
    testConnectionLabel->setText(tr("Test in progress..."));
    {
        QSqlDatabase test = QSqlDatabase::addDatabase("QMYSQL", "FREEDIAMS_MAINWIN_CONNECTION_TESTER");
        test.setHostName(host->text());
        test.setPort(port->value());
        test.setUserName(log->text());
        test.setPassword(pass->text());
        if (!test.open()) {
            testButton->setIcon(theme()->icon(Core::Constants::ICONERROR));
            testConnectionLabel->setText(tr("Connection error: %1").arg(test.lastError().number()));
            testConnectionLabel->setToolTip(test.lastError().text());
        } else {
            testButton->setIcon(theme()->icon(Core::Constants::ICONOK));
            testConnectionLabel->setText(tr("Connected"));
        }
    }
    QSqlDatabase::removeDatabase("FREEDIAMS_MAINWIN_CONNECTION_TESTER");
}


void MainWindowPreferencesWidget::changeEvent(QEvent *e)
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


static inline void createPatient(const QString &name, const QString &secondname, const QString &surname,
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
    query.bindValue(Constants::IDENTITY_SURNAME, surname);
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
        qWarning() << name << secondname << surname << gender << title<< dob<<country<<note<<street<<zip<<city<<uuid<<lkid<<photoFile<<death;
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

VirtualBasePage::VirtualBasePage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("VirtualBasePage"); }

VirtualBasePage::~VirtualBasePage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString VirtualBasePage::id() const { return objectName(); }
QString VirtualBasePage::name() const { return tr("Virtual databases"); }
QString VirtualBasePage::category() const { return tr("General"); }

void VirtualBasePage::finish() { delete m_Widget; }

QWidget *VirtualBasePage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new VirtualBaseWidget(parent);
    return m_Widget;
}

VirtualBaseWidget::VirtualBaseWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
}

void VirtualBaseWidget::on_populateDb_clicked()
{
    QSqlQuery query(patientBase()->database());
    QHash<int, QString> where;
    where.insert(Patients::Constants::IDENTITY_NAME, "LIKE 'DOE'");
    int userLkId = userModel()->practionnerLkIds(userModel()->currentUserData(UserPlugin::User::Uuid).toString()).at(0);
    int c = patientBase()->count(Patients::Constants::Table_IDENT, Patients::Constants::IDENTITY_NAME, patientBase()->getWhereClause(Patients::Constants::Table_IDENT, where));
    if (!c) {
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

    }

    // Prepare virtual patients
    int nb = nbVirtualPatients->value();
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
            sur= r.getRandomSurname(false);
            sec = r.getRandomName();
            title = 2;
        } else {
            g = "M";
            sur= r.getRandomSurname(true);
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

}

void VirtualBaseWidget::on_populateEpisodes_clicked()
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
    int userLkId = userModel()->practionnerLkIds(userModel()->currentUserData(UserPlugin::User::Uuid).toString()).at(0);
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

void VirtualBaseWidget::changeEvent(QEvent *e)
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

void VirtualBaseWidget::on_populateUsers_clicked()
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
        userModel()->setData(userModel()->index(0, User::Name), r.getRandomName());
        userModel()->setData(userModel()->index(0, User::Surname), r.getRandomSurname(genderIndex==1));
        userModel()->setData(userModel()->index(0, User::TitleIndex), 4);
        userModel()->setData(userModel()->index(0, User::GenderIndex), genderIndex);
        userModel()->submitUser(userModel()->index(0, User::Uuid).data().toString());
    }
}
