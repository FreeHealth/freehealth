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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <ddiplugin/ddiplugin.h>
#include <ddiplugin/ddicore.h>
#include <ddiplugin/database/ddidatabase.h>
#include <ddiplugin/interactors/druginteractor.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/randomizer.h>

#include <QString>
#include <QVariant>
#include <QTest>

using namespace DDI;
using namespace Internal;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}

namespace {
static int loop = 100;
}

void DDIPlugin::test_drugInteractor()
{
    // Ctor
    DDI::DrugInteractor di;
    QCOMPARE(di.isValid(), false);
    QCOMPARE(di.isClass(), false);
    QCOMPARE(di.isReviewed(), false);
    QCOMPARE(di.id(), QString());
    QCOMPARE(di.parentIds(), QStringList());
    QCOMPARE(di.childrenIds(), QStringList());
    QCOMPARE(di.childrenCount(), 0);
    QCOMPARE(di.allNeededPMIDs(), QStringList());
    QList<int> refs;
    refs << DrugInteractor::ReadOnly_TranslatedLabel
        << DrugInteractor::InitialLabel
        << DrugInteractor::EnLabel
        << DrugInteractor::FrLabel
        << DrugInteractor::DeLabel
        << DrugInteractor::EsLabel
        << DrugInteractor::ClassInformationFr
        << DrugInteractor::ClassInformationEn
        << DrugInteractor::ClassInformationDe
        << DrugInteractor::DoNotWarnDuplicated
        << DrugInteractor::Reference
        << DrugInteractor::PMIDsStringList
        << DrugInteractor::ATCCodeStringList
        << DrugInteractor::Comment
        << DrugInteractor::ReviewersStringList
        << DrugInteractor::DateOfReview
        << DrugInteractor::DateOfCreation
        << DrugInteractor::DateLastUpdate
        << DrugInteractor::IsAutoFound;
    for(int i=0; i < refs.count(); ++i) {
        QCOMPARE(di.data(refs.at(i)), QVariant());
    }

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    QList<DrugInteractor> interactors;
    for(int i=0; i < loop; ++i) {
        DrugInteractor d;
        // test data/setData
        QString words = r.randomWords(r.randomInt(1, 5));
        d.setData(DrugInteractor::InitialLabel, words);
        QCOMPARE(d.data(DrugInteractor::InitialLabel).toString(), words);
        words = r.randomWords(r.randomInt(1, 5));
        d.setData(DrugInteractor::EnLabel, words);
        QCOMPARE(d.data(DrugInteractor::EnLabel).toString(), words);
        words = r.randomWords(r.randomInt(1, 5));
        d.setData(DrugInteractor::FrLabel, words);
        QCOMPARE(d.data(DrugInteractor::FrLabel).toString(), words);
        words = r.randomWords(r.randomInt(1, 5));
        d.setData(DrugInteractor::DeLabel, words);
        QCOMPARE(d.data(DrugInteractor::DeLabel).toString(), words);
        words = r.randomWords(r.randomInt(1, 5));
        d.setData(DrugInteractor::EsLabel, words);
        QCOMPARE(d.data(DrugInteractor::EsLabel).toString(), words);
        words = r.randomWords(r.randomInt(1, 5));
        d.setData(DrugInteractor::ClassInformationFr, words);
        QCOMPARE(d.data(DrugInteractor::ClassInformationFr).toString(), words);
        words = r.randomWords(r.randomInt(1, 5));
        d.setData(DrugInteractor::ClassInformationEn, words);
        QCOMPARE(d.data(DrugInteractor::ClassInformationEn).toString(), words);
        words = r.randomWords(r.randomInt(1, 5));
        d.setData(DrugInteractor::ClassInformationDe, words);
        QCOMPARE(d.data(DrugInteractor::ClassInformationDe).toString(), words);
        words = r.randomWords(r.randomInt(1, 5));
        d.setData(DrugInteractor::Reference, words);
        QCOMPARE(d.data(DrugInteractor::Reference).toString(), words);

        words = r.randomWords(r.randomInt(1, 5));
        d.setData(DrugInteractor::Comment, words);
        QCOMPARE(d.data(DrugInteractor::Comment).toString(), words);

        QStringList list;
        for(int i=0; i < r.randomInt(1, 10); ++i) {
            list << r.randomString(r.randomInt(1, 10));
        }
        d.setData(DrugInteractor::ReviewersStringList, list);
        QCOMPARE(d.data(DrugInteractor::ReviewersStringList).toStringList(), list);
        list.clear();
        for(int i=0; i < r.randomInt(1, 10); ++i) {
            list << r.randomString(r.randomInt(1, 10));
        }
        d.setData(DrugInteractor::PMIDsStringList, list);
        QCOMPARE(d.data(DrugInteractor::PMIDsStringList).toStringList(), list);
        list.clear();
        for(int i=0; i < r.randomInt(1, 10); ++i) {
            list << r.randomString(r.randomInt(1, 10));
        }
        d.setData(DrugInteractor::ATCCodeStringList, list);
        QCOMPARE(d.data(DrugInteractor::ATCCodeStringList).toStringList(), list);

        QDate date = r.randomDate(2000);
        d.setData(DrugInteractor::DateOfReview, date);
        QCOMPARE(d.data(DrugInteractor::DateOfReview).toDate(), date);
        date = r.randomDate(2000);
        d.setData(DrugInteractor::DateOfCreation, date);
        QCOMPARE(d.data(DrugInteractor::DateOfCreation).toDate(), date);
        date = r.randomDate(2000);
        d.setData(DrugInteractor::DateLastUpdate, date);
        QCOMPARE(d.data(DrugInteractor::DateLastUpdate).toDate(), date);

        bool rb = r.randomBool();
        d.setData(DrugInteractor::IsAutoFound, rb);
        QCOMPARE(d.data(DrugInteractor::IsAutoFound).toBool(), rb);
        rb = r.randomBool();
        d.setData(DrugInteractor::IsDuplicated, rb);
        QCOMPARE(d.data(DrugInteractor::IsDuplicated).toBool(), rb);
        rb = r.randomBool();
        d.setData(DrugInteractor::DoNotWarnDuplicated, rb);
        QCOMPARE(d.data(DrugInteractor::DoNotWarnDuplicated).toBool(), rb);

        // Test addChildren
        list.clear();
        for(int i=0; i < r.randomInt(1, 10); ++i) {
            list << r.randomString(r.randomInt(1, 10));
        }
        d.setData(DrugInteractor::ChildrenInteractorsUidStringList, list);
        QCOMPARE(d.childrenIds(), list);
        for(int i=0; i < r.randomInt(1, 10); ++i) {
            QString t = r.randomString(r.randomInt(1, 10));
            list << t;
            d.addChildInteractorUid(t);
        }
        QCOMPARE(d.childrenIds(), list);

        // Test addChildClassificationPMID(s)
        QString child = list.first();
        list.clear();
        for(int i=0; i < r.randomInt(1, 10); ++i) {
            list << r.randomString(r.randomInt(1, 10));
        }
        d.addChildClassificationPMIDs(child, list);
        QCOMPARE(d.childClassificationPMIDs(child), list);

        for(int i=0; i < r.randomInt(1, 10); ++i) {
            QString t = r.randomString(r.randomInt(1, 10));
            list << t;
            d.addChildClassificationPMID(child, t);
        }
        QCOMPARE(d.childClassificationPMIDs(child), list);
        interactors << d;
    }

    // Test XML transformation
    QString xml = DrugInteractor::listToXml(interactors);
    QList<DrugInteractor> fromXml = DrugInteractor::listFromXml(xml);
    QCOMPARE(interactors.count(), fromXml.count());
    // TODO: QCOMPARE(interactors, fromXml);
}
