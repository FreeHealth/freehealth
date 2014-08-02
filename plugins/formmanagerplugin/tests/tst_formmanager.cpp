/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "../formmanagerplugin.h"
#include "../formcore.h"
#include "../formmanager.h"
#include "../iformio.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <extensionsystem/pluginmanager.h>

#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QTest>
#include <QSignalSpy>

// See Issue 359

using namespace Form;
using namespace Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Form::FormCore &formCore() {return Form::FormCore::instance();}
// static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}

// Get the XML FormIO object from the object pool
static inline Form::IFormIO *_xmlIo()
{
    Form::IFormIO *xmlIo = 0;
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
    foreach(Form::IFormIO *io, list) {
        if (io->name() == "XmlFormIO")
            xmlIo = io;
    }
    return xmlIo;
}

void FormManagerPlugin::test_FormManager_initialization()
{
    QCOMPARE(formCore().formManager().isInitialized(), true);
}

void FormManagerPlugin::test_FormIOQuery()
{
    Form::FormIOQuery query;

    // Test default values
    QVERIFY(query.typeOfForms() == Form::FormIOQuery::CompleteForms);
    QVERIFY(query.excludeGenderSpecific() == false);
    QVERIFY(query.isoLanguagesAndCountry().isEmpty());
    QVERIFY(query.specialties().isEmpty());
    QVERIFY(query.authors().isEmpty());
    QVERIFY(query.formUuid().isEmpty());
    QVERIFY(query.forceFileReading() == false);
    QVERIFY(query.getAllAvailableForms() == false);
    QVERIFY(query.getAllAvailableFormDescriptions() == false);

    // Test setters/getters
    query.setTypeOfForms(Form::FormIOQuery::CompleteForms);
    QVERIFY(query.typeOfForms() == Form::FormIOQuery::CompleteForms);
    query.setTypeOfForms(Form::FormIOQuery::SubForms);
    QVERIFY(query.typeOfForms() == Form::FormIOQuery::SubForms);
    query.setTypeOfForms(Form::FormIOQuery::CompleteForms | Form::FormIOQuery::SubForms);
    QVERIFY(query.typeOfForms() == (Form::FormIOQuery::CompleteForms | Form::FormIOQuery::SubForms));

    query.setExcludeGenderSpecific(true);
    QVERIFY(query.excludeGenderSpecific() == true);
    query.setExcludeGenderSpecific(false);
    QVERIFY(query.excludeGenderSpecific() == false);

    QStringList test;
    test << "fr_FR" << "en_US";
    query.setIsoLanguageAndCountry(test);
    QVERIFY(query.isoLanguagesAndCountry() == test);

    test.clear();
    test << "Spe1" << "Spe2";
    query.setSpecialties(test);
    QVERIFY(query.specialties() == test);

    test.clear();
    test << "Author1" << "Author2";
    query.setAuthors(test);
    QVERIFY(query.authors() == test);

    query.setFormUuid("FormUid");
    QVERIFY(query.formUuid() == "FormUid");

    query.setForceFileReading(false);
    QVERIFY(query.forceFileReading() == false);
    query.setForceFileReading(true);
    QVERIFY(query.forceFileReading() == true);

    query.setGetAllAvailableForms(false);
    QVERIFY(query.getAllAvailableForms() == false);
    query.setGetAllAvailableForms(true);
    QVERIFY(query.getAllAvailableForms() == true);

    query.setGetAllAvailableFormDescriptions(false);
    QVERIFY(query.getAllAvailableFormDescriptions() == false);
    query.setGetAllAvailableFormDescriptions(true);
    QVERIFY(query.getAllAvailableFormDescriptions() == true);
}

void FormManagerPlugin::test_FormIO_object()
{
    // Test that IFormIO object are correctly declared
    QList<Form::IFormIO *> list = pluginManager()->getObjects<Form::IFormIO>();
    QVERIFY(list.count() >= 1);

    // Test that the XML IO plugin is correctly created and available
    Form::IFormIO *xmlIo = _xmlIo();
    foreach(Form::IFormIO *io, list) {
        if (io->name() == "XmlFormIO")
            xmlIo = io;
    }
    QVERIFY(xmlIo != 0);

    // Test getFormFileDescriptions()
    QDir completeFormsDir(settings()->path(Core::ISettings::CompleteFormsPath));
    QVERIFY(completeFormsDir.exists() == true);
    QDir subFormsDir(settings()->path(Core::ISettings::SubFormsPath));
    QVERIFY(subFormsDir.exists() == true);

    int countComplete = Utils::getFiles(completeFormsDir, "central.xml").count();
    QVERIFY(countComplete >= 1);
    int countSub = Utils::getFiles(subFormsDir, "central.xml").count();
    QVERIFY(countSub >= 1);

    // Check using the XML internal database
    Form::FormIOQuery query;
    query.setTypeOfForms(Form::FormIOQuery::CompleteForms);
    QList<Form::FormIODescription*> completeDescriptions = xmlIo->getFormFileDescriptions(query);
    //    foreach(Form::FormIODescription *desc, completeDescriptions) {
    //        qDebug() << desc->data(Form::FormIODescription::UuidOrAbsPath) << desc->data(Form::FormIODescription::Uuid);
    //    }
    query.setTypeOfForms(Form::FormIOQuery::SubForms);
    QList<Form::FormIODescription*> subDescriptions = xmlIo->getFormFileDescriptions(query);
    //    foreach(Form::FormIODescription *desc, subDescriptions) {
    //        qDebug() << desc->data(Form::FormIODescription::UuidOrAbsPath) << desc->data(Form::FormIODescription::Uuid);
    //    }
    QVERIFY(completeDescriptions.count() == countComplete);
    QVERIFY(subDescriptions.count() == countSub);

    // query.setForceFileReading(true);

    // TODO: test loadAllRootForms()
    // TODO: test availableUpdates() with user central forms
    // TODO: test availableUpdates() with user subforms
}

void FormManagerPlugin::test_FormIO_screenshots()
{
    Form::IFormIO *xmlIo = _xmlIo();

    // Get all local shots for any forms
    // Currently (v0.9.1) all shots are saved in PNG format
    QHash<QString, int> uidCount;
    QFileInfoList files = Utils::getFiles(QDir(settings()->path(Core::ISettings::CompleteFormsPath)), "*.png");
    foreach(const QFileInfo &file, files) {
        QString fileName = file.absoluteFilePath();
        fileName.replace(settings()->path(Core::ISettings::CompleteFormsPath), Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH);

        // Eg: fileName == __completeForms__/gp_basic1/shots/fr/1patientfiles-fr.png
        //     Uid  = __completeForms__/gp_basic1
        //     Shot = fr/1patientfiles-fr.png

        // extract formUid from fileName
        int splitAt = fileName.indexOf("/", fileName.indexOf("/") + 1);
        QString uid = fileName.left(splitAt);
        QString shot = fileName.mid(fileName.indexOf("shots/", splitAt) + 6);
        qDebug() << uid << shot;
        QVERIFY(xmlIo->screenShot(uid, shot).size() == QPixmap(file.absoluteFilePath()).size());
        // QString ioPix = Utils::pixmapToBase64(xmlIo->screenShot(uid, shot));
        // QString filePix = Utils::pixmapToBase64(QPixmap(file.absoluteFilePath()));
        // TODO: this is false with some shots only??? QVERIFY(ioPix == filePix);

        // Count all uid shots
        uidCount.insert(uid, uidCount.value(uid)+1);
    }

    // Test Form::IFormIO::screenShots() with a known central form
    foreach(const QString &uid, uidCount.keys()) {
        const QList<QPixmap> &pix = xmlIo->screenShots(uid);
        QVERIFY(pix.count() == uidCount.value(uid));
    }

}

// TODO: test extractFileToTmpPath()
