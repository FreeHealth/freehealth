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

#include <utils/log.h>
#include <utils/global.h>
#include <utils/randomizer.h>
#include <extensionsystem/pluginmanager.h>

#include <QDir>
#include <QTest>
#include <QSignalSpy>

// See Issue 359

using namespace Form;
using namespace Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Form::FormCore &formCore() {return Form::FormCore::instance();}
// static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}

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
    QVERIFY(query.getAllAvailableForms() == true);
    QVERIFY(query.getAllAvailableFormDescriptions() == true);

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
    Form::IFormIO *xmlIo = 0;
    foreach(Form::IFormIO *io, list) {
        if (io->name() == "XmlFormIO")
            xmlIo = io;
    }
    QVERIFY(xmlIo != 0);

    // TODO: test getFormFileDescriptions()
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
    QList<Form::FormIODescription*> completeDescriptions = xmlIo->getFormFileDescriptions(query);
    QList<Form::FormIODescription*> subDescriptions = xmlIo->getFormFileDescriptions(query);

    qDebug() << "countComplete" << countComplete << "countSub" << countSub;
    qDebug() << "completeFromQuery" << completeDescriptions.count()
             << "subFromQuery" << subDescriptions.count();

    QVERIFY(completeDescriptions.count() == countComplete);
    QVERIFY(subDescriptions.count() == countSub);

    // query.setForceFileReading(true);

    // TODO: test loadAllRootForms()
    // TODO: test screenShots() with a known central form
    // TODO: test extractFileToTmpPath()
    // TODO: test availableUpdates() with user central forms
    // TODO: test availableUpdates() with user subforms
}
