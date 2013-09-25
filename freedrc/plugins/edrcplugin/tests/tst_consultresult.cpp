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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <edrcplugin/edrcplugin.h>

#include <edrcplugin/consultresult.h>
#include <edrcplugin/consultresultvalidator.h>
#include <edrcplugin/database/edrcbase.h>

#include <utils/global.h>

#include <QDebug>
#include <QTest>

#define CR_CSV_BLANK_LINE
using namespace eDRC;
using namespace Internal;

namespace {
const int loop = 10;
}

/**
 * Run tests on the eDRC::Internal::ConsultResultValidator
*/
void EdrcPlugin::testConsultResultObject()
{
    ConsultResult r;
    QCOMPARE(r.isEmpty(), true);
    QCOMPARE(r.isValid(), false);
    QCOMPARE(r.consultResultId(), -1);
    QCOMPARE(r.dateOfExamination(), QDateTime());
    QCOMPARE(int(r.chronicDiseaseState()), int(ConsultResult::ChronicDiseaseStateUndefined));
    QCOMPARE(int(r.diagnosisPosition()), int(ConsultResult::DiagnosisPositionUndefined));
    QCOMPARE(r.htmlCommentOnCR(), QString());
    QCOMPARE(r.htmlCommentOnCriterias(), QString());
    QCOMPARE(int(r.medicalFollowUp()), int(ConsultResult::MedicalFollowUpUndefined));
    QCOMPARE(r.selectedCriterias(), QList<int>());
    QCOMPARE(int(r.symptomaticState()), int(ConsultResult::SymptomaticStateUndefined));

    r.setConsultResult(1);
    r.setChronicDiseaseState(ConsultResult::ChronicDisease);
    r.setDiagnosisPosition(ConsultResult::A);
    QVERIFY(r.isEmpty() == false);
    QVERIFY(r.isValid() == false);
    r.setMedicalFollowUp(ConsultResult::N);
    r.setSymptomaticState(ConsultResult::Symptomatic);
    QVERIFY(r.isEmpty() == false);
    QVERIFY(r.isValid() == false);
    r.setSelectedCriterias(QList<int>() << 1 << 2);
    QVERIFY(r.isEmpty() == false);
    QVERIFY(r.isValid() == true);
    QCOMPARE(r.consultResultId(), 1);
    QCOMPARE(int(r.chronicDiseaseState()), int(ConsultResult::ChronicDisease));
    QCOMPARE(int(r.diagnosisPosition()), int(ConsultResult::N));
    QCOMPARE(int(r.symptomaticState()), int(ConsultResult::Symptomatic));
    QCOMPARE(r.selectedCriterias(), QList<int>() << 1 << 2);

    // Check operator==()
    ConsultResult r2;
    r2.setConsultResult(1);
    r2.setChronicDiseaseState(ConsultResult::ChronicDisease);
    r2.setDiagnosisPosition(ConsultResult::A);
    r2.setMedicalFollowUp(ConsultResult::N);
    r2.setSymptomaticState(ConsultResult::Symptomatic);
    r2.setSelectedCriterias(QList<int>() << 1 << 2);
    QVERIFY(r == r2);
    r2.setConsultResult(2);
    QVERIFY(r != r2);
    r2.setConsultResult(1);
    QCOMPARE(r.consultResultId(), r2.consultResultId());
    QVERIFY(r == r2);
    r2.setChronicDiseaseState(ConsultResult::NotChronicDisease);
    QVERIFY(r != r2);
    r2.setChronicDiseaseState(ConsultResult::ChronicDiseaseStateUndefined);
    QVERIFY(r != r2);
    r2.setChronicDiseaseState(ConsultResult::ChronicDisease);
    QCOMPARE(r.chronicDiseaseState(), r2.chronicDiseaseState());
    QVERIFY(r == r2);
    r2.setDiagnosisPosition(ConsultResult::DiagnosisPositionUndefined);
    QVERIFY(r != r2);
    r2.setDiagnosisPosition(ConsultResult::B);
    QVERIFY(r != r2);
    r2.setDiagnosisPosition(ConsultResult::C);
    QVERIFY(r != r2);
    r2.setDiagnosisPosition(ConsultResult::D);
    QVERIFY(r != r2);
    r2.setDiagnosisPosition(ConsultResult::Z);
    QVERIFY(r != r2);
    r2.setDiagnosisPosition(ConsultResult::A);
    QCOMPARE(r.diagnosisPosition(), r2.diagnosisPosition());
    QVERIFY(r == r2);
    r2.setMedicalFollowUp(ConsultResult::MedicalFollowUpUndefined);
    QVERIFY(r != r2);
    r2.setMedicalFollowUp(ConsultResult::P);
    QVERIFY(r != r2);
    r2.setMedicalFollowUp(ConsultResult::R);
    QVERIFY(r != r2);
    r2.setMedicalFollowUp(ConsultResult::N);
    QCOMPARE(r.medicalFollowUp(), r2.medicalFollowUp());
    QVERIFY(r == r2);

    r2.setSymptomaticState(ConsultResult::NotSymptomatic);
    QVERIFY(r != r2);
    r2.setSymptomaticState(ConsultResult::Symptomatic);

    QCOMPARE(r.symptomaticState(), r2.symptomaticState());
    QVERIFY(r == r2);
    r2.setSelectedCriterias(QList<int>() << 1 << 3);
    QVERIFY(r != r2);
    r2.setSelectedCriterias(QList<int>() << 1 << 2);
    QCOMPARE(r.selectedCriterias(), r2.selectedCriterias());
    QVERIFY(r == r2);

    // Check with comments
    r.setHtmlCommentOnCR("<p>This the CR<b>comment</b><br></p>");
    r2.setHtmlCommentOnCR("<p>This the CR<b>comment</b><br></p>");
    QCOMPARE(r.htmlCommentOnCR(), r2.htmlCommentOnCR());
    QVERIFY(r == r2);

    r.setHtmlCommentOnCriterias("<p>This the criteria<b>comment</b><br></p>");
    r2.setHtmlCommentOnCriterias("<p>This the criteria<b>comment</b><br></p>");
    QCOMPARE(r.htmlCommentOnCriterias(), r2.htmlCommentOnCriterias());
    QVERIFY(r == r2);
    r2.setHtmlCommentOnCR("<p>This another CR<b>comment</b><br></p>");
    QVERIFY(r != r2);
    r2.setHtmlCommentOnCriterias("<p>This another criteria<b>comment</b><br></p>");
    QVERIFY(r != r2);

    // Check date
    QDateTime dt = QDateTime::currentDateTime();
    r.setDateOfExamination(dt);
    r2.setDateOfExamination(dt);
    QCOMPARE(r.dateOfExamination(), r2.dateOfExamination());
    QVERIFY(r == r2);
    r2.setDateOfExamination(dt.addDays(-1));
    QVERIFY(r != r2);

    r.clear();
    QVERIFY(r.isEmpty() == true);
    QVERIFY(r.isValid() == false);
}

void EdrcPlugin::testConsultResultXml()
{
    // Test XML
    ConsultResult r2;
    r2.setConsultResult(1);
    r2.setChronicDiseaseState(ConsultResult::ChronicDisease);
    r2.setDiagnosisPosition(ConsultResult::A);
    r2.setMedicalFollowUp(ConsultResult::N);
    r2.setSymptomaticState(ConsultResult::Symptomatic);
    r2.setSelectedCriterias(QList<int>() << 1 << 2);
    r2.setHtmlCommentOnCR("<p>This the CR<b>comment</b><br></p>");
    r2.setHtmlCommentOnCriterias("<p>This the criteria<b>comment</b><br></p>");

    QList<ConsultResult> list = ConsultResult::fromXml(r2.toXml());
    QCOMPARE(list.count(), 1);
    QVERIFY(r2 == list.at(0));
    QVERIFY(r2.toXml() == list.at(0).toXml());

    // TODO: test with extraXml code
}
