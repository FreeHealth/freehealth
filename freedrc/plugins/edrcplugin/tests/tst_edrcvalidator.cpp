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

/**
 * Run tests on the eDRC::Internal::ConsultResultValidator
*/
void EdrcPlugin::initTestCase()
{
}

static ConsultResultCriteria &criteria(const QString &csv)
{
    // static int internalId = 972342;
    // CSV == ID;SORT;LABEL;INDENT;WEIGHT
    QStringList values = csv.split(";");
    ConsultResultCriteria *crit = new ConsultResultCriteria();
    crit->setId(values.at(0).toInt());
    crit->setSortIndex(values.at(1).toInt());
    crit->setLabel(values.at(2));
    crit->setIndentation(values.at(3).toInt());
    crit->setWeight(values.at(4).toInt());
    return *crit;
}

static void testValidator(ConsultResultValidator *validator, const QList<int> &selectedIds, const QList<int> &wrongIds, const QString &msg)
{
    QWARN(QString("Testing: %1").arg(msg).toUtf8());
    // Update validator data
    validator->clearSelectedCriterias();
    validator->setSelectedCriterias(selectedIds);
    // Start validator magic
    validator->check();
    // Check reported wrong ids
    qWarning() << "Validator" << validator->wrongCriteriaIds() << "Test" << wrongIds;
    QVERIFY(wrongIds.count() == validator->wrongCriteriaIds().count());
    for(int i=0; i < wrongIds.count(); ++i) {
        QVERIFY(validator->wrongCriteriaIds().contains(wrongIds.at(i)) == true);
    }
    QWARN("    Test succeeded");
}

void EdrcPlugin::testConsultResultObject()
{
    ConsultResult r;
    QVERIFY(r.isEmpty() == true);
    QVERIFY(r.isValid() == false);
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

    QVERIFY(r == r2);
    r2.setChronicDiseaseState(ConsultResult::NotChronicDisease);
    QVERIFY(r != r2);
    r2.setChronicDiseaseState(ConsultResult::ChronicDiseaseStateUndefined);
    QVERIFY(r != r2);
    r2.setChronicDiseaseState(ConsultResult::ChronicDisease);

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

    QVERIFY(r == r2);
    r2.setMedicalFollowUp(ConsultResult::MedicalFollowUpUndefined);
    QVERIFY(r != r2);
    r2.setMedicalFollowUp(ConsultResult::P);
    QVERIFY(r != r2);
    r2.setMedicalFollowUp(ConsultResult::R);
    QVERIFY(r != r2);
    r2.setMedicalFollowUp(ConsultResult::N);

    QVERIFY(r == r2);
    r2.setSymptomaticState(ConsultResult::NotSymptomatic);
    QVERIFY(r != r2);
    r2.setSymptomaticState(ConsultResult::Symptomatic);

    QVERIFY(r == r2);
    r2.setSelectedCriterias(QList<int>() << 1 << 3);
    QVERIFY(r != r2);
    r2.setSelectedCriterias(QList<int>() << 1 << 2);
    QVERIFY(r == r2);

    // Check with comments
    r.setHtmlCommentOnCR("<p>This the CR<b>comment</b><br></p>");
    r2.setHtmlCommentOnCR("<p>This the CR<b>comment</b><br></p>");
    QVERIFY(r == r2);

    r.setHtmlCommentOnCriterias("<p>This the criteria<b>comment</b><br></p>");
    r2.setHtmlCommentOnCriterias("<p>This the criteria<b>comment</b><br></p>");
    QVERIFY(r == r2);

    r2.setHtmlCommentOnCR("<p>This another CR<b>comment</b><br></p>");
    QVERIFY(r != r2);

    r2.setHtmlCommentOnCriterias("<p>This another criteria<b>comment</b><br></p>");
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

    ConsultResult r3 = ConsultResult::fromXml(r2.toXml());
    QVERIFY(r2 == r3);
    QVERIFY(r2.toXml() == r3.toXml());
}

void EdrcPlugin::testValidatorTestOne()
{
    // Create validator & test lists
    ConsultResultValidator validator;
    validator.setCrId(1);
    QList<int> selectedIds;
    QList<int> wrongIds;

    // Create criterias using csv {ID;SORT;LABEL;INDENT;WEIGHT}
    QString csv =
            "0;0;++++ MANDATORY 1;1;1\n"
            "1;1;++++ MANDATORY 2;1;1\n"
            "2;2;++++ MANDATORY 3;1;1\n"
            "3;3;  ++1| MANDATORY 3.CHILD1;2;2\n"
            "4;4;  ++1| MANDATORY 3.CHILD2;2;2\n"
            "5;5;  ++1| MANDATORY 3.CHILD2;2;2\n"
            "6;6; ;1;8\n"
            "7;7;+ - OPTIONAL 1;1;6\n"
            ;
    QList<ConsultResultCriteria> criterias;
    foreach(const QString &line, csv.split("\n", QString::SkipEmptyParts))
        criterias << criteria(line);
    validator.setAvailableCriterias(criterias);


    // Test 1 : missing one mandatory
    selectedIds << 0 << 2 << 3 << 4 << 5;
    wrongIds    << 1;
    testValidator(&validator, selectedIds, wrongIds, "Test1: missing one mandatory 1");
    selectedIds.clear();
    wrongIds.clear();

    // Test 1bis : missing one mandatory
    selectedIds << 0 << 1;
    wrongIds    << 2;
    testValidator(&validator, selectedIds, wrongIds, "Test1: missing one mandatory 2");
    selectedIds.clear();
    wrongIds.clear();

    // Test 1ter : missing one mandatory (child selected but not parent)
    selectedIds << 0 << 1 << 3;
    wrongIds    << 2;
    testValidator(&validator, selectedIds, wrongIds, "Test1: missing one mandatory (child selected but not parent)");
    selectedIds.clear();
    wrongIds.clear();

    // Test 2 : missing children
    selectedIds << 0 << 1 << 2;
    wrongIds    << 3 << 4 << 5;
    testValidator(&validator, selectedIds, wrongIds, "Test1: missing children");
    selectedIds.clear();
    wrongIds.clear();

    // Test 3 : ok
    selectedIds << 0 << 1 << 2 << 3;
    testValidator(&validator, selectedIds, wrongIds, "Test1: Correct coding 1");
    selectedIds.clear();
    wrongIds.clear();

    // Test 4 : ok
    selectedIds << 0 << 1 << 2 << 4;
    testValidator(&validator, selectedIds, wrongIds, "Test1: Correct coding 2");
    selectedIds.clear();
    wrongIds.clear();

    // Test 5 : ok
    selectedIds << 0 << 1 << 2 << 5;
    testValidator(&validator, selectedIds, wrongIds, "Test1: Correct coding 3");
    selectedIds.clear();
    wrongIds.clear();
}

void EdrcPlugin::testValidatorTestTwo()
{
    // Create validator & test lists
    ConsultResultValidator validator;
    validator.setCrId(1);
    QList<int> selectedIds;
    QList<int> wrongIds;

    // Create criterias using csv {ID;SORT;LABEL;INDENT;WEIGHT}
    QString csv =
            "0;0;++++ MANDATORY 1;1;1\n"
            "1;1;++++ MANDATORY 2;1;1\n"
            "2;2;++++ MANDATORY 3;1;1\n"
            "3;3;  ++1| MANDATORY 3.CHILD1;2;2\n"
            "4;4;  ++1| MANDATORY 3.CHILD2;2;2\n"
            "5;5;  ++1| MANDATORY 3.CHILD2;2;2\n"
            "6;6; ;1;8\n"
            "7;7;++1| SIGNIFIANT IN GROUP 1;2;2\n"
            "8;8;++1| SIGNIFIANT IN GROUP 2;2;2\n"
            "9;9;++1| SIGNIFIANT IN GROUP 3;2;2\n"
            "10;10; ;1;8\n"
            "11;11;+ - OPTIONAL 1;1;6\n"
            ;
    QList<ConsultResultCriteria> criterias;
    foreach(const QString &line, csv.split("\n", QString::SkipEmptyParts))
        criterias << criteria(line);
    validator.setAvailableCriterias(criterias);


    // Test 1 : missing one mandatory
    selectedIds << 0 << 2;
    wrongIds    << 1 << 3 << 4 << 5 << 7 << 8 << 9;
    testValidator(&validator, selectedIds, wrongIds, "Test2: missing one mandatory 1");
    selectedIds.clear();
    wrongIds.clear();

    // Test 1bis : missing one mandatory
    selectedIds << 0 << 1 << 9;
    wrongIds    << 2;
    testValidator(&validator, selectedIds, wrongIds, "Test2: missing one mandatory 2");
    selectedIds.clear();
    wrongIds.clear();

    // Test 1ter : missing one signifiant in the second group
    selectedIds << 0 << 1 << 2 << 3;
    wrongIds    << 7 << 8 << 9;
    testValidator(&validator, selectedIds, wrongIds, "Test2: missing one signifiant in the second group");
    selectedIds.clear();
    wrongIds.clear();

    // Test 2 : missing children
    selectedIds << 0 << 1 << 2;
    wrongIds    << 3 << 4 << 5 << 7 << 8 << 9;
    testValidator(&validator, selectedIds, wrongIds, "Test2: missing children");
    selectedIds.clear();
    wrongIds.clear();

    // Test 3 : ok
    selectedIds << 0 << 1 << 2 << 3 << 7;
    testValidator(&validator, selectedIds, wrongIds, "Test2: Correct coding 1");
    selectedIds.clear();
    wrongIds.clear();

    // Test 4 : ok
    selectedIds << 0 << 1 << 2 << 4 << 8;
    testValidator(&validator, selectedIds, wrongIds, "Test2: Correct coding 2");
    selectedIds.clear();
    wrongIds.clear();

    // Test 5 : ok
    selectedIds << 0 << 1 << 2 << 5 << 9;
    testValidator(&validator, selectedIds, wrongIds, "Test2: Correct coding 3");
    selectedIds.clear();
    wrongIds.clear();
}

void EdrcPlugin::testValidatorTestThree()
{
    // Create validator & test lists
    ConsultResultValidator validator;
    validator.setCrId(1);
    QList<int> selectedIds;
    QList<int> wrongIds;

    // Create criterias using csv {ID;SORT;LABEL;INDENT;WEIGHT}
    QString csv =
            "1;2;++1| GROUP 1;1;2\n"
            "2;3;  ++1| MANDATORY 3.CHILD1;2;2\n"
            "3;4;  ++1| MANDATORY 3.CHILD2;2;2\n"
            "4;5;  ++1| MANDATORY 3.CHILD2;2;2\n"
            "20;6; ;1;8\n"
            "5;7;++1| GROUP 2;1;2\n"
            "6;8;  ++1| MANDATORY 3.CHILD1;2;2\n"
            "21;9; ;1;8\n"
            "7;10;+ - OPTIONAL 1;1;6\n"
            "8;11;+ - OPTIONAL 2;1;6\n"
            "9;12;+ - OPTIONAL 3;1;6\n"
            "10;13; ;1;8\n"
            "11;14;+ - OPTIONAL 1;1;6\n"
            ;
    QList<ConsultResultCriteria> criterias;
    foreach(const QString &line, csv.split("\n", QString::SkipEmptyParts))
        criterias << criteria(line);
    validator.setAvailableCriterias(criterias);


    // Test 1 : missing one mandatory
    selectedIds << 7 << 8 << 9 << 11;
    wrongIds    << 1 << 5;
    testValidator(&validator, selectedIds, wrongIds, "Test3: missing one mandatory 1");
    selectedIds.clear();
    wrongIds.clear();

    // Test 1bis : missing one mandatory
    selectedIds << 2 << 7 << 8 << 9;
    wrongIds    << 1 << 5;
    testValidator(&validator, selectedIds, wrongIds, "Test3: missing one mandatory 2");
    selectedIds.clear();
    wrongIds.clear();

    // Test 1ter : missing one signifiant in the second group
    selectedIds << 1 << 2 << 5;
    wrongIds    << 6;
    testValidator(&validator, selectedIds, wrongIds, "Test3: missing one signifiant in the second group");
    selectedIds.clear();
    wrongIds.clear();

    // Test 2 : missing children
    selectedIds << 1 << 4 << 5;
    wrongIds    << 6;
    testValidator(&validator, selectedIds, wrongIds, "Test3: missing children");
    selectedIds.clear();
    wrongIds.clear();

    // Test 3 : ok
    selectedIds << 1 << 2;
    testValidator(&validator, selectedIds, wrongIds, "Test3: Correct coding 1");
    selectedIds.clear();
    wrongIds.clear();

    // Test 4 : ok
    selectedIds << 1 << 2 << 6 << 5;
    testValidator(&validator, selectedIds, wrongIds, "Test3: Correct coding 2");
    selectedIds.clear();
    wrongIds.clear();

    // Test 5 : ok
    selectedIds << 5 << 6;
    testValidator(&validator, selectedIds, wrongIds, "Test3: Correct coding 3");
    selectedIds.clear();
    wrongIds.clear();
}

void EdrcPlugin::testCrToHtml()
{
    QString out("*      - CR_DATABASE_VERSION: 2013.06-eDRC:2005<br>*      - CR_CODING_VALIDITY: Is valid<br>*      - CR_CODING_VALIDATOR_VERSION: 2013.06<br>*      - CR_ID: 148<br>*      - CR_LABEL: ACCIDENT VASCULAIRE CEREBRAL<br>*      - CR_CRITERIAS: <br><br>*      - CRITERIA_LABEL: ++++ D&Eacute;FICIT NEUROLOGIQUE CENTRAL<br>*      - CRITERIA_MODERN_LABEL: &#9745; D&Eacute;FICIT NEUROLOGIQUE CENTRAL<br>*      - CRITERIA_ID: 124<br>*      - CRITERIA_LABEL: ++++ D'INSTALLATION BRUSQUE<br>*      - CRITERIA_MODERN_LABEL: &#9745; D'INSTALLATION BRUSQUE<br>*      - CRITERIA_ID: 125<br>*      - CRITERIA_LABEL: ++1| TROUBLES DE L'&Eacute;LOCUTION, VOIRE APHASIE<br>*      - CRITERIA_MODERN_LABEL: &#10112; TROUBLES DE L'&Eacute;LOCUTION, VOIRE APHASIE<br>*      - CRITERIA_ID: 128<br>*      - CRITERIA_LABEL: ++++ ABSENCE DE TRAUMATISME D&Eacute;CLENCHANT<br>*      - CRITERIA_MODERN_LABEL: &#9745; ABSENCE DE TRAUMATISME D&Eacute;CLENCHANT<br>*      - CRITERIA_ID: 132<br>*      - CRITERIA_LABEL: + - troubles de la conscience voire coma<br>*      - CRITERIA_MODERN_LABEL: + - troubles de la conscience voire coma<br>*      - CRITERIA_ID: 134<br><br><br>*      - CR_DIAGNOSTIC_POSITION_CODE: C<br>*      - CR_DIAGNOSTIC_POSITION_FULL_LABEL: Disease definition<br>*      - CR_FOLLOWUP_CODE: R<br>*      - CR_FOLLOWUP_FULL_LABEL: Revised<br>*      - CR_CHRONIC_DISEASE: Chronic disease<br>*      - CR_SYMPTOMATIC_STATE: Symptomatic<br>*      - CR_ICD10_CODES: I64<br>*      - CR_ICD10_LABELS: I64<br>*      - CR_ICD10_CODES_AND_LABELS: I64<br>");

    ConsultResult cr;
    cr.setConsultResult(148);
    cr.setSymptomaticState(ConsultResult::Symptomatic);
    cr.setChronicDiseaseState(ConsultResult::ChronicDisease);
    cr.setDiagnosisPosition(ConsultResult::C);
    cr.setMedicalFollowUp(ConsultResult::N);
    cr.setSelectedCriterias(QList<int>() << 124<<125<<128<<132<<134);

    DrcDatabase *edrcBase = new DrcDatabase("/Volumes/RamDisk/eric/freemedforms/global_resources/datapacks/appinstalled/");
    edrcBase->initialize(true, "");

    QString html = cr.toHtml("*      - CR_DATABASE_VERSION: [[CR_DATABASE_VERSION]]<br>"
                             "*      - CR_CODING_VALIDITY: [[CR_CODING_VALIDITY]]<br>"
                             "*      - CR_CODING_VALIDATOR_VERSION: [[CR_CODING_VALIDATOR_VERSION]]<br>"
                             "*      - CR_ID: [[CR_ID]]<br>"
                             "*      - CR_LABEL: [[CR_LABEL]]<br>"
                             "*      - CR_CRITERIAS: <br><br>[[CR_CRITERIAS]]<br><br>"
                             "*      - CR_DIAGNOSTIC_POSITION_CODE: [[CR_DIAGNOSTIC_POSITION_CODE]]<br>"
                             "*      - CR_DIAGNOSTIC_POSITION_FULL_LABEL: [[CR_DIAGNOSTIC_POSITION_FULL_LABEL]]<br>"
                             "*      - CR_FOLLOWUP_CODE: [[CR_FOLLOWUP_CODE]]<br>"
                             "*      - CR_FOLLOWUP_FULL_LABEL: [[CR_FOLLOWUP_FULL_LABEL]]<br>"
                             "*      - CR_CHRONIC_DISEASE: [[CR_CHRONIC_DISEASE]]<br>"
                             "*      - CR_SYMPTOMATIC_STATE: [[CR_SYMPTOMATIC_STATE]]<br>"
                             "*      - CR_ICD10_CODES: [[CR_ICD10_CODES]]<br>"
                             "*      - CR_ICD10_LABELS: [[CR_ICD10_LABELS]]<br>"
                             "*      - CR_ICD10_CODES_AND_LABELS: [[CR_ICD10_CODES_AND_LABELS]]<br>",
                             "*      - CRITERIA_LABEL: [[CRITERIA_LABEL]]<br>"
                             "*      - CRITERIA_MODERN_LABEL: [[CRITERIA_MODERN_LABEL]]<br>"
                             "*      - CRITERIA_ID: [[CRITERIA_ID]]<br>",
                             *edrcBase);
    QVERIFY(out == html);
}

void EdrcPlugin::cleanupTestCase()
{}



