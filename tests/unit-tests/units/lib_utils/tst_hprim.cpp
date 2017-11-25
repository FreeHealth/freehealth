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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <utils/hprimparser.h>

#include "../../autotest.h"

#include <QDebug>
#include <QTest>
#include <QCryptographicHash>

/**
 * Run test on:
*/
class tst_Hprim : public QObject
{
    Q_OBJECT
public:

private slots:
    void initTestCase()
    {
    }

    void testHprimParsers_data()
    {
        QTest::addColumn<QString>("hprimSource");
        QTest::addColumn<QString>("headerRawSource");
        QTest::addColumn<QString>("patientId");
        QTest::addColumn<QString>("patientName");
        QTest::addColumn<QString>("patientFirstName");
        QTest::addColumn<QString>("patientAddress1");
        QTest::addColumn<QString>("patientAddress2");
        QTest::addColumn<QString>("patientZip");
        QTest::addColumn<QString>("patientCity");
        QTest::addColumn<QString>("patientDob");
        QTest::addColumn<QString>("patientSN");
        QTest::addColumn<QString>("extraCode");
        QTest::addColumn<QString>("dateExam");
        QTest::addColumn<QString>("sender");
        QTest::addColumn<QString>("receiver");
        QTest::addColumn<QString>("message");
        QTest::addColumn<bool>("hprimContentIsValid");
        QTest::addColumn<bool>("hprimContentIsNull");
        QTest::addColumn<int>("hprim2NumberOfLines");

        QTest::newRow("test1") << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                                  " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****LAB****\r\n"
                                  "TEX\r\n"
                                  "TEX|                 ALLERGIE\r\n"
                                  "TEX\r\n"
                                  "TEX|    I.G.E Totales                   164  UI/ml\r\n"
                                  "RES|I.G.E Totales|IGET 000|N|164|UI/ml|0|3000|N|F\r\n"
                                  "TEX|     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "TEX|                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "TEX|                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "TEX|                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "TEX|                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "TEX\r\n"
                                  "RES||IGET 005|A\r\n"
                                  "****FIN****\r\n"
                                  "****FINFICHIER****\r\n"
                                  "\r\n"
                               << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                               << "0000129610"
                               << "FAKENAME"
                               << "FAKEFIRSTNAME"
                               << "1A RUE DU FOUR"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "62580"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "18/12/2009"
                               << "1234567890123 75"
                               << "506232"
                               << "22/05/2013"
                               << "C LabFake"
                               << "FAKFAK FAKE.NAME"
                               << " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****LAB****\r\n"
                                  "TEX\r\n"
                                  "TEX|                 ALLERGIE\r\n"
                                  "TEX\r\n"
                                  "TEX|    I.G.E Totales                   164  UI/ml\r\n"
                                  "RES|I.G.E Totales|IGET 000|N|164|UI/ml|0|3000|N|F\r\n"
                                  "TEX|     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "TEX|                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "TEX|                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "TEX|                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "TEX|                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "TEX\r\n"
                                  "RES||IGET 005|A\r\n"
                                  "****FIN****\r\n"
                                  "****FINFICHIER****\r\n"
                                  "\r\n"
                               << true  // HPRIM rawContent isValid ?
                               << false // hprimContentIsNull
                               << 2     // HPRIM2 NumberOfLines
                                  ;

        QTest::newRow("test2") << "0000116214\n"
                                  "NEWFAKE\n"
                                  "GLOUBGLOUB\n"
                                  " \n"
                                  "FC U ROUTE D JUUSDF           \n" // 30 chars
                                  "62580 BAILLEUL SIR BERTHOULT  \n" // 30 chars
                                  "10/03/1959\n"
                                  "1234567890123 76\n"
                                  "3135110345\n"
                                  "15/05/2013\n"
                                  "labo.fake Institut factice de biologie\n"
                                  "HUJHY      _\n"
                                  "INSTITUT DE BIOLOGIE CLINIQUE \n"
                                  "Les résultats d'analyses de biologie médicale transmis sont confidentiels\n"
                                  "et soumis au SECRET PROFESSIONNEL. Ils sont destinés à l'usage exclusif\n"
                                  "du destinataire. Si ces résultats ne vous sont pas destinés, veuillez\n"
                                  "nous contacter le plus vite possible.\n"
                                  "Nom du biologiste autorisant la diffusion de cette transmission:\n"
                                  "JSLDKF QSDLFKL                          \n"
                                  "                         Examen(s) du: 15/05/13                  Valeurs de réf. \n"
                                  " \n"
                                  "                                    H E M A T O L O G I E\n"
                                  "HEMOGRAMME\n"
                                  "HEMATIES                               4450000  /mm3           3800000 à 5400000\n"
                                  "  Hémoglobine                             13,3  g/dl              12,5 à 15,5   \n"
                                  "  Hématocrite                             41,4  %                 37,0 à 47,0   \n"
                                  "  V.G.M.                                  93,0  fl                82,0 à 98,0   \n"
                                  "  T.C.M.H.                                29,9  pg                sup. à 27,0   \n"
                                  "  C.C.M.H.                                32,1  g/dl              32,0 à 36,0   \n"
                                  "LEUCOCYTES                               2.920 */mm3             4.000 à 10.000 \n"
                                  "  Polynucléaires neutrophiles             54.0  %                               \n"
                                  "                                          1577  /mm3              1500 à 7500   \n"
                                  "  Polynucléaires éosinophiles              3.0  %                               \n"
                                  "                                            88  /mm3              inf. à 400    \n"
                                  "  Polynucléaires basophiles                0.0  %                               \n"
                                  "                                             0  /mm3              inf. à 200    \n"
                                  "  Lymphocytes                             33.0  %                               \n"
                                  "                                           964 */mm3              1000 à 4000   \n"
                                  "  Monocytes                               10.0  %                               \n"
                                  "                                           292  /mm3               200 à 800    \n"
                                  "Formule controlée sur lame.\n"
                                  "NUMERATION DES PLAQUETTES              224.000  /mm3           150.000 à 400.000\n"
                                  "  Volume plaquettaire moyen                8.1  fl                              \n"
                                  " \n"
                                  "                                   B I O C H I M I E\n"
                                  "PROTEINE C-REACTIVE (CRP)     < 0.12  mg/l \n"
                                  "ESTIMATION DE LA FONCTION RENALE (MDRD simplifiée)\n"
                                  "CREATININE                                 8,3  mg/l               6,0 à 11,0   \n"
                                  "                                          73,5  umol/l            53,1 à 97,4   \n"
                                  "ESTIMATION DE LA FONCTION RENALE          76.1  ml/mn                           \n"
                                  "Interprétation :\n"
                                  "Commentaire :\n"
                                  "UREE                                      0,24  g/l               0,15 à 0,42   \n"
                                  "                                          4,01  mmol/l            2,51 à 7,01   \n"
                                  "ACIDE URIQUE                                50  mg/l                25 à 60     \n"
                                  "                                           298  umol/l             149 à 357    \n"
                                  "GLYCEMIE à jeun                           0,91  g/l               0,74 à 1,06   \n"
                                  "                                          5,05  mmol/l            4,11 à 5,88   \n"
                                  " *++++ EXPLORATION D'UNE ANOMALIE LIPIDIQUE (ldl calculé/dosé) \n"
                                  "ASPECT DU SERUM                        Limpide          \n"
                                  "CHOLESTEROL TOTAL                         2,19 *g/l               inf. à 2,00   \n"
                                  "                                          5,67  mmol/l            inf. à 5,18   \n"
                                  "TRIGLYCERIDES                             0,73  g/l               0,35 à 1,40   \n"
                                  "                                          0,83  mmol/l            0,40 à 1,60   \n"
                                  "CHOLESTEROL HDL                           0,63  g/l               sup. à 0,40   \n"
                                  "                                          1,63  mmol/l            sup. à 1,03   \n"
                                  "LDL CHOLESTEROL                           1,41 *g/l               inf. à 1,30   \n"
                                  "                                          3,65  mmol/l            inf. à 3,35   \n"
                                  "  Rapport chol. total / H.D.L.            3,48                    inf. à 4,90   \n"
                                  "SODIUM                                     143  mEq/l              132 à 146    \n"
                                  "POTASSIUM                                  5,0 *mEq/l              3,5 à 4,8    \n"
                                  "TRANSAMINASE TGO (ASAT)                     21  UI/l              inf. à 34     \n"
                                  "TRANSAMINASE TGP (ALAT)                     10  UI/l                10 à 49     \n"
                                  "GAMMA-GLUTAMYL TRANSFERASE (GGT)             9  U/l               inf. à 38     \n"
                                  " \n"
                                  "                   M A R Q U E U R S  -  V I T A M I N E S\n"
                                  "ANTIGENE CA 15-3                          20,4  U/ml              inf. à 32,4   \n"
                                  "ANTIGENE CARCINO EMBRYONNAIRE(ACE)         1,9  ng/ml             inf. à 5,1    \n"
                                  " \n"
                                  " ****LAB****\n"
                                  "\n"
                                  "TEX|                         Examen(s) du: 15/05/13                  Valeurs de réf.\n"
                                  "TEX|\n"
                                  "TEX|                                    H E M A T O L O G I E\n"
                                  "TEX|HEMOGRAMME\n"
                                  "TEX|HEMATIES                               4450000  /mm3           3800000 à 5400000\n"
                                  "RES|HEMATIES|grcal|N|4450000|/mm3|3800000|5400000|N|F\n"
                                  "TEX|  Hémoglobine                             13,3  g/dl              12,5 à 15,5   \n"
                                  "RES|Hémoglobine|hb|N|13.3|g/dl|12.5|15.5|N|F\n"
                                  "TEX|  Hématocrite                             41,4  %                 37,0 à 47,0   \n"
                                  "RES|Hématocrite|HTE|N|41.4|%|37.0|47.0|N|F\n"
                                  "TEX|  V.G.M.                                  93,0  fl                82,0 à 98,0   \n"
                                  "RES|V.G.M|VGM|N|93.0|fl|82.0|98.0|N|F\n"
                                  "TEX|  T.C.M.H.                                29,9  pg                sup. à 27,0   \n"
                                  "RES|T.C.M.H|tcmh|N|29.9|pg|27.0||N|F\n"
                                  "TEX|  C.C.M.H.                                32,1  g/dl              32,0 à 36,0   \n"
                                  "RES|C.C.M.H|ccmh|N|32.1|g/dl|32.0|36.0|N|F\n"
                                  "TEX|LEUCOCYTES                               2.920 */mm3             4.000 à 10.000 \n"
                                  "RES|LEUCOCYTES|gb|N|2920|/mm3|4000|10000|L|F\n"
                                  "TEX|  Polynucléaires neutrophiles             54.0  %                               \n"
                                  "TEX|                                          1577  /mm3              1500 à 7500   \n"
                                  "RES|Polynucléaires neutrophiles|pn|N|54.0|%|||N|F|1577|/mm3|1500|7500\n"
                                  "TEX|  Polynucléaires éosinophiles              3.0  %                               \n"
                                  "TEX|                                            88  /mm3              inf. à 400    \n"
                                  "RES|Polynucléaires éosinophiles|pe|N|3.0|%|||N|F|88|/mm3||400\n"
                                  "TEX|  Polynucléaires basophiles                0.0  %                               \n"
                                  "TEX|                                             0  /mm3              inf. à 200    \n"
                                  "RES|Polynucléaires basophiles|pb|N|0.0|%|||N|F|0|/mm3||200\n"
                                  "TEX|  Lymphocytes                             33.0  %                               \n"
                                  "TEX|                                           964 */mm3              1000 à 4000   \n"
                                  "RES|Lymphocytes|ly|N|33.0|%|||L|F|964|/mm3|1000|4000\n"
                                  "TEX|  Monocytes                               10.0  %                               \n"
                                  "TEX|                                           292  /mm3               200 à 800    \n"
                                  "RES|Monocytes|mono|N|10.0|%|||N|F|292|/mm3|200|800\n"
                                  "TEX|Formule controlée sur lame.\n"
                                  "TEX|NUMERATION DES PLAQUETTES              224.000  /mm3           150.000 à 400.000\n"
                                  "RES|NUMERATION DES PLAQUETTES|pla|N|224000|/mm3|150000|400000|N|F\n"
                                  "TEX|  Volume plaquettaire moyen                8.1  fl                              \n"
                                  "RES|Volume plaquettaire moyen|vpm|N|8.1|fl||||F\n"
                                  "TEX|\n"
                                  "TEX|                                   B I O C H I M I E\n"
                                  "TEX|PROTEINE C-REACTIVE (CRP)     < 0.12  mg/l \n"
                                  "RES|PROTEINE C-REACTIVE (CRP)|CRPS33|A|    < 0.12  mg/l |||||F\n"
                                  "TEX|ESTIMATION DE LA FONCTION RENALE (MDRD simplifiée)\n"
                                  "TEX|CREATININE                                 8,3  mg/l               6,0 à 11,0   \n"
                                  "TEX|                                          73,5  umol/l            53,1 à 97,4   \n"
                                  "RES|CREATININE|C33|N|8.3|mg/l|6.0|11.0|N|F|73.5|umol/l|53.1|97.4\n"
                                  "TEX|ESTIMATION DE LA FONCTION RENALE          76.1  ml/mn                           \n"
                                  "RES|ESTIMATION DE LA FONCTION RENALE|mdrf3-|N|76.1|ml/mn||||F\n"
                                  "TEX|\n"
                                  "TEX|Interprétation :\n"
                                  "TEX|Commentaire :\n"
                                  "TEX|UREE                                      0,24  g/l               0,15 à 0,42   \n"
                                  "TEX|                                          4,01  mmol/l            2,51 à 7,01   \n"
                                  "RES|UREE|UR33|N|0.24|g/l|0.15|0.42|N|F|4.01|mmol/l|2.51|7.01\n"
                                  "TEX|ACIDE URIQUE                                50  mg/l                25 à 60     \n"
                                  "TEX|                                           298  umol/l             149 à 357    \n"
                                  "RES|ACIDE URIQUE|AU33|N|50|mg/l|25|60|N|F|298|umol/l|149|357\n"
                                  "TEX|GLYCEMIE à jeun                           0,91  g/l               0,74 à 1,06   \n"
                                  "TEX|                                          5,05  mmol/l            4,11 à 5,88   \n"
                                  "RES|GLYCEMIE à jeun|glyc33|N|0.91|g/l|0.74|1.06|N|F|5.05|mmol/l|4.11|5.88\n"
                                  "TEX| *++++ EXPLORATION D'UNE ANOMALIE LIPIDIQUE (ldl calculé/dosé) \n"
                                  "TEX|ASPECT DU SERUM                        Limpide          \n"
                                  "RES|ASPECT DU SERUM|ASPE33|A|Limpide||||N|F\n"
                                  "TEX|CHOLESTEROL TOTAL                         2,19 *g/l               inf. à 2,00   \n"
                                  "TEX|                                          5,67  mmol/l            inf. à 5,18   \n"
                                  "RES|CHOLESTEROL TOTAL|CT33|N|2.19|g/l||2.00|H|F|5.67|mmol/l||5.18\n"
                                  "TEX|TRIGLYCERIDES                             0,73  g/l               0,35 à 1,40   \n"
                                  "TEX|                                          0,83  mmol/l            0,40 à 1,60   \n"
                                  "RES|TRIGLYCERIDES|TG33|N|0.73|g/l|0.35|1.40|N|F|0.83|mmol/l|0.40|1.60\n"
                                  "TEX|CHOLESTEROL HDL                           0,63  g/l               sup. à 0,40   \n"
                                  "TEX|                                          1,63  mmol/l            sup. à 1,03   \n"
                                  "RES|CHOLESTEROL HDL|hdl33|N|0.63|g/l|0.40||N|F|1.63|mmol/l|1.03|\n"
                                  "TEX|LDL CHOLESTEROL                           1,41 *g/l               inf. à 1,30   \n"
                                  "TEX|                                          3,65  mmol/l            inf. à 3,35   \n"
                                  "RES|LDL CHOLESTEROL|ldl33|N|1.41|g/l||1.30|H|F|3.65|mmol/l||3.35\n"
                                  "TEX|  Rapport chol. total / H.D.L.            3,48                    inf. à 4,90   \n"
                                  "RES|Rapport chol. total / H.D.L|cthd33|N|3.48|||4.90|N|F\n"
                                  "TEX|SODIUM                                     143  mEq/l              132 à 146    \n"
                                  "RES|SODIUM|NA33|N|143|mEq/l|132|146|N|F\n"
                                  "TEX|POTASSIUM                                  5,0 *mEq/l              3,5 à 4,8    \n"
                                  "RES|POTASSIUM|K33|N|5.0|mEq/l|3.5|4.8|H|F\n"
                                  "TEX|TRANSAMINASE TGO (ASAT)                     21  UI/l              inf. à 34     \n"
                                  "RES|TRANSAMINASE TGO (ASAT)|ASAT33|N|21|UI/l||34|N|F\n"
                                  "TEX|TRANSAMINASE TGP (ALAT)                     10  UI/l                10 à 49     \n"
                                  "RES|TRANSAMINASE TGP (ALAT)|ALAT33|N|10|UI/l|10|49|N|F\n"
                                  "TEX|GAMMA-GLUTAMYL TRANSFERASE (GGT)             9  U/l               inf. à 38     \n"
                                  "RES|GAMMA-GLUTAMYL TRANSFERASE (GGT)|GGT33|N|9|U/l||38|N|F\n"
                                  "TEX|\n"
                                  "TEX|                   M A R Q U E U R S  -  V I T A M I N E S\n"
                                  "TEX|ANTIGENE CA 15-3                          20,4  U/ml              inf. à 32,4   \n"
                                  "RES|ANTIGENE CA 15-3|C15333|N|20.4|U/ml||32.4|N|F\n"
                                  "TEX|ANTIGENE CARCINO EMBRYONNAIRE(ACE)         1,9  ng/ml             inf. à 5,1    \n"
                                  "RES|ANTIGENE CARCINO EMBRYONNAIRE(ACE)|ACE33|N|1.9|ng/ml||5.1|N|F\n"
                                  "****FIN****\n"
                                  "****FINFICHIER****\n"
                                  "\n"
                               << "0000116214\n"
                                  "NEWFAKE\n"
                                  "GLOUBGLOUB\n"
                                  " \n"
                                  "FC U ROUTE D JUUSDF           \n" // 30 chars
                                  "62580 BAILLEUL SIR BERTHOULT  \n" // 30 chars
                                  "10/03/1959\n"
                                  "1234567890123 76\n"
                                  "3135110345\n"
                                  "15/05/2013\n"
                                  "labo.fake Institut factice de biologie\n"
                                  "HUJHY      _\n"
                               << "0000116214"
                               << "NEWFAKE"
                               << "GLOUBGLOUB"
                               << ""
                               << "FC U ROUTE D JUUSDF"
                               << "62580"
                               << "BAILLEUL SIR BERTHOULT"
                               << "10/03/1959"
                               << "1234567890123 76"
                               << "3135110345"
                               << "15/05/2013"
                               << "labo.fake Institut factice de biologie"
                               << "HUJHY      _"
                               << "INSTITUT DE BIOLOGIE CLINIQUE \n"
                                  "Les résultats d'analyses de biologie médicale transmis sont confidentiels\n"
                                  "et soumis au SECRET PROFESSIONNEL. Ils sont destinés à l'usage exclusif\n"
                                  "du destinataire. Si ces résultats ne vous sont pas destinés, veuillez\n"
                                  "nous contacter le plus vite possible.\n"
                                  "Nom du biologiste autorisant la diffusion de cette transmission:\n"
                                  "JSLDKF QSDLFKL                          \n"
                                  "                         Examen(s) du: 15/05/13                  Valeurs de réf. \n"
                                  " \n"
                                  "                                    H E M A T O L O G I E\n"
                                  "HEMOGRAMME\n"
                                  "HEMATIES                               4450000  /mm3           3800000 à 5400000\n"
                                  "  Hémoglobine                             13,3  g/dl              12,5 à 15,5   \n"
                                  "  Hématocrite                             41,4  %                 37,0 à 47,0   \n"
                                  "  V.G.M.                                  93,0  fl                82,0 à 98,0   \n"
                                  "  T.C.M.H.                                29,9  pg                sup. à 27,0   \n"
                                  "  C.C.M.H.                                32,1  g/dl              32,0 à 36,0   \n"
                                  "LEUCOCYTES                               2.920 */mm3             4.000 à 10.000 \n"
                                  "  Polynucléaires neutrophiles             54.0  %                               \n"
                                  "                                          1577  /mm3              1500 à 7500   \n"
                                  "  Polynucléaires éosinophiles              3.0  %                               \n"
                                  "                                            88  /mm3              inf. à 400    \n"
                                  "  Polynucléaires basophiles                0.0  %                               \n"
                                  "                                             0  /mm3              inf. à 200    \n"
                                  "  Lymphocytes                             33.0  %                               \n"
                                  "                                           964 */mm3              1000 à 4000   \n"
                                  "  Monocytes                               10.0  %                               \n"
                                  "                                           292  /mm3               200 à 800    \n"
                                  "Formule controlée sur lame.\n"
                                  "NUMERATION DES PLAQUETTES              224.000  /mm3           150.000 à 400.000\n"
                                  "  Volume plaquettaire moyen                8.1  fl                              \n"
                                  " \n"
                                  "                                   B I O C H I M I E\n"
                                  "PROTEINE C-REACTIVE (CRP)     < 0.12  mg/l \n"
                                  "ESTIMATION DE LA FONCTION RENALE (MDRD simplifiée)\n"
                                  "CREATININE                                 8,3  mg/l               6,0 à 11,0   \n"
                                  "                                          73,5  umol/l            53,1 à 97,4   \n"
                                  "ESTIMATION DE LA FONCTION RENALE          76.1  ml/mn                           \n"
                                  "Interprétation :\n"
                                  "Commentaire :\n"
                                  "UREE                                      0,24  g/l               0,15 à 0,42   \n"
                                  "                                          4,01  mmol/l            2,51 à 7,01   \n"
                                  "ACIDE URIQUE                                50  mg/l                25 à 60     \n"
                                  "                                           298  umol/l             149 à 357    \n"
                                  "GLYCEMIE à jeun                           0,91  g/l               0,74 à 1,06   \n"
                                  "                                          5,05  mmol/l            4,11 à 5,88   \n"
                                  " *++++ EXPLORATION D'UNE ANOMALIE LIPIDIQUE (ldl calculé/dosé) \n"
                                  "ASPECT DU SERUM                        Limpide          \n"
                                  "CHOLESTEROL TOTAL                         2,19 *g/l               inf. à 2,00   \n"
                                  "                                          5,67  mmol/l            inf. à 5,18   \n"
                                  "TRIGLYCERIDES                             0,73  g/l               0,35 à 1,40   \n"
                                  "                                          0,83  mmol/l            0,40 à 1,60   \n"
                                  "CHOLESTEROL HDL                           0,63  g/l               sup. à 0,40   \n"
                                  "                                          1,63  mmol/l            sup. à 1,03   \n"
                                  "LDL CHOLESTEROL                           1,41 *g/l               inf. à 1,30   \n"
                                  "                                          3,65  mmol/l            inf. à 3,35   \n"
                                  "  Rapport chol. total / H.D.L.            3,48                    inf. à 4,90   \n"
                                  "SODIUM                                     143  mEq/l              132 à 146    \n"
                                  "POTASSIUM                                  5,0 *mEq/l              3,5 à 4,8    \n"
                                  "TRANSAMINASE TGO (ASAT)                     21  UI/l              inf. à 34     \n"
                                  "TRANSAMINASE TGP (ALAT)                     10  UI/l                10 à 49     \n"
                                  "GAMMA-GLUTAMYL TRANSFERASE (GGT)             9  U/l               inf. à 38     \n"
                                  " \n"
                                  "                   M A R Q U E U R S  -  V I T A M I N E S\n"
                                  "ANTIGENE CA 15-3                          20,4  U/ml              inf. à 32,4   \n"
                                  "ANTIGENE CARCINO EMBRYONNAIRE(ACE)         1,9  ng/ml             inf. à 5,1    \n"
                                  " \n"
                                  " ****LAB****\n"
                                  "\n"
                                  "TEX|                         Examen(s) du: 15/05/13                  Valeurs de réf.\n"
                                  "TEX|\n"
                                  "TEX|                                    H E M A T O L O G I E\n"
                                  "TEX|HEMOGRAMME\n"
                                  "TEX|HEMATIES                               4450000  /mm3           3800000 à 5400000\n"
                                  "RES|HEMATIES|grcal|N|4450000|/mm3|3800000|5400000|N|F\n"
                                  "TEX|  Hémoglobine                             13,3  g/dl              12,5 à 15,5   \n"
                                  "RES|Hémoglobine|hb|N|13.3|g/dl|12.5|15.5|N|F\n"
                                  "TEX|  Hématocrite                             41,4  %                 37,0 à 47,0   \n"
                                  "RES|Hématocrite|HTE|N|41.4|%|37.0|47.0|N|F\n"
                                  "TEX|  V.G.M.                                  93,0  fl                82,0 à 98,0   \n"
                                  "RES|V.G.M|VGM|N|93.0|fl|82.0|98.0|N|F\n"
                                  "TEX|  T.C.M.H.                                29,9  pg                sup. à 27,0   \n"
                                  "RES|T.C.M.H|tcmh|N|29.9|pg|27.0||N|F\n"
                                  "TEX|  C.C.M.H.                                32,1  g/dl              32,0 à 36,0   \n"
                                  "RES|C.C.M.H|ccmh|N|32.1|g/dl|32.0|36.0|N|F\n"
                                  "TEX|LEUCOCYTES                               2.920 */mm3             4.000 à 10.000 \n"
                                  "RES|LEUCOCYTES|gb|N|2920|/mm3|4000|10000|L|F\n"
                                  "TEX|  Polynucléaires neutrophiles             54.0  %                               \n"
                                  "TEX|                                          1577  /mm3              1500 à 7500   \n"
                                  "RES|Polynucléaires neutrophiles|pn|N|54.0|%|||N|F|1577|/mm3|1500|7500\n"
                                  "TEX|  Polynucléaires éosinophiles              3.0  %                               \n"
                                  "TEX|                                            88  /mm3              inf. à 400    \n"
                                  "RES|Polynucléaires éosinophiles|pe|N|3.0|%|||N|F|88|/mm3||400\n"
                                  "TEX|  Polynucléaires basophiles                0.0  %                               \n"
                                  "TEX|                                             0  /mm3              inf. à 200    \n"
                                  "RES|Polynucléaires basophiles|pb|N|0.0|%|||N|F|0|/mm3||200\n"
                                  "TEX|  Lymphocytes                             33.0  %                               \n"
                                  "TEX|                                           964 */mm3              1000 à 4000   \n"
                                  "RES|Lymphocytes|ly|N|33.0|%|||L|F|964|/mm3|1000|4000\n"
                                  "TEX|  Monocytes                               10.0  %                               \n"
                                  "TEX|                                           292  /mm3               200 à 800    \n"
                                  "RES|Monocytes|mono|N|10.0|%|||N|F|292|/mm3|200|800\n"
                                  "TEX|Formule controlée sur lame.\n"
                                  "TEX|NUMERATION DES PLAQUETTES              224.000  /mm3           150.000 à 400.000\n"
                                  "RES|NUMERATION DES PLAQUETTES|pla|N|224000|/mm3|150000|400000|N|F\n"
                                  "TEX|  Volume plaquettaire moyen                8.1  fl                              \n"
                                  "RES|Volume plaquettaire moyen|vpm|N|8.1|fl||||F\n"
                                  "TEX|\n"
                                  "TEX|                                   B I O C H I M I E\n"
                                  "TEX|PROTEINE C-REACTIVE (CRP)     < 0.12  mg/l \n"
                                  "RES|PROTEINE C-REACTIVE (CRP)|CRPS33|A|    < 0.12  mg/l |||||F\n"
                                  "TEX|ESTIMATION DE LA FONCTION RENALE (MDRD simplifiée)\n"
                                  "TEX|CREATININE                                 8,3  mg/l               6,0 à 11,0   \n"
                                  "TEX|                                          73,5  umol/l            53,1 à 97,4   \n"
                                  "RES|CREATININE|C33|N|8.3|mg/l|6.0|11.0|N|F|73.5|umol/l|53.1|97.4\n"
                                  "TEX|ESTIMATION DE LA FONCTION RENALE          76.1  ml/mn                           \n"
                                  "RES|ESTIMATION DE LA FONCTION RENALE|mdrf3-|N|76.1|ml/mn||||F\n"
                                  "TEX|\n"
                                  "TEX|Interprétation :\n"
                                  "TEX|Commentaire :\n"
                                  "TEX|UREE                                      0,24  g/l               0,15 à 0,42   \n"
                                  "TEX|                                          4,01  mmol/l            2,51 à 7,01   \n"
                                  "RES|UREE|UR33|N|0.24|g/l|0.15|0.42|N|F|4.01|mmol/l|2.51|7.01\n"
                                  "TEX|ACIDE URIQUE                                50  mg/l                25 à 60     \n"
                                  "TEX|                                           298  umol/l             149 à 357    \n"
                                  "RES|ACIDE URIQUE|AU33|N|50|mg/l|25|60|N|F|298|umol/l|149|357\n"
                                  "TEX|GLYCEMIE à jeun                           0,91  g/l               0,74 à 1,06   \n"
                                  "TEX|                                          5,05  mmol/l            4,11 à 5,88   \n"
                                  "RES|GLYCEMIE à jeun|glyc33|N|0.91|g/l|0.74|1.06|N|F|5.05|mmol/l|4.11|5.88\n"
                                  "TEX| *++++ EXPLORATION D'UNE ANOMALIE LIPIDIQUE (ldl calculé/dosé) \n"
                                  "TEX|ASPECT DU SERUM                        Limpide          \n"
                                  "RES|ASPECT DU SERUM|ASPE33|A|Limpide||||N|F\n"
                                  "TEX|CHOLESTEROL TOTAL                         2,19 *g/l               inf. à 2,00   \n"
                                  "TEX|                                          5,67  mmol/l            inf. à 5,18   \n"
                                  "RES|CHOLESTEROL TOTAL|CT33|N|2.19|g/l||2.00|H|F|5.67|mmol/l||5.18\n"
                                  "TEX|TRIGLYCERIDES                             0,73  g/l               0,35 à 1,40   \n"
                                  "TEX|                                          0,83  mmol/l            0,40 à 1,60   \n"
                                  "RES|TRIGLYCERIDES|TG33|N|0.73|g/l|0.35|1.40|N|F|0.83|mmol/l|0.40|1.60\n"
                                  "TEX|CHOLESTEROL HDL                           0,63  g/l               sup. à 0,40   \n"
                                  "TEX|                                          1,63  mmol/l            sup. à 1,03   \n"
                                  "RES|CHOLESTEROL HDL|hdl33|N|0.63|g/l|0.40||N|F|1.63|mmol/l|1.03|\n"
                                  "TEX|LDL CHOLESTEROL                           1,41 *g/l               inf. à 1,30   \n"
                                  "TEX|                                          3,65  mmol/l            inf. à 3,35   \n"
                                  "RES|LDL CHOLESTEROL|ldl33|N|1.41|g/l||1.30|H|F|3.65|mmol/l||3.35\n"
                                  "TEX|  Rapport chol. total / H.D.L.            3,48                    inf. à 4,90   \n"
                                  "RES|Rapport chol. total / H.D.L|cthd33|N|3.48|||4.90|N|F\n"
                                  "TEX|SODIUM                                     143  mEq/l              132 à 146    \n"
                                  "RES|SODIUM|NA33|N|143|mEq/l|132|146|N|F\n"
                                  "TEX|POTASSIUM                                  5,0 *mEq/l              3,5 à 4,8    \n"
                                  "RES|POTASSIUM|K33|N|5.0|mEq/l|3.5|4.8|H|F\n"
                                  "TEX|TRANSAMINASE TGO (ASAT)                     21  UI/l              inf. à 34     \n"
                                  "RES|TRANSAMINASE TGO (ASAT)|ASAT33|N|21|UI/l||34|N|F\n"
                                  "TEX|TRANSAMINASE TGP (ALAT)                     10  UI/l                10 à 49     \n"
                                  "RES|TRANSAMINASE TGP (ALAT)|ALAT33|N|10|UI/l|10|49|N|F\n"
                                  "TEX|GAMMA-GLUTAMYL TRANSFERASE (GGT)             9  U/l               inf. à 38     \n"
                                  "RES|GAMMA-GLUTAMYL TRANSFERASE (GGT)|GGT33|N|9|U/l||38|N|F\n"
                                  "TEX|\n"
                                  "TEX|                   M A R Q U E U R S  -  V I T A M I N E S\n"
                                  "TEX|ANTIGENE CA 15-3                          20,4  U/ml              inf. à 32,4   \n"
                                  "RES|ANTIGENE CA 15-3|C15333|N|20.4|U/ml||32.4|N|F\n"
                                  "TEX|ANTIGENE CARCINO EMBRYONNAIRE(ACE)         1,9  ng/ml             inf. à 5,1    \n"
                                  "RES|ANTIGENE CARCINO EMBRYONNAIRE(ACE)|ACE33|N|1.9|ng/ml||5.1|N|F\n"
                                  "****FIN****\n"
                                  "****FINFICHIER****\n"
                                  "\n"
                               << true  // HPRIM rawContent isValid ?
                               << false // hprimContentIsNull
                               << 33    // HPRIM2 NumberOfLines
                                  ;

        // HPRIM 1 == Ok; HPRIM2 is empty
        QTest::newRow("test3") << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                                  " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****FIN****\r\n"
                                  "****FINFICHIER****\r\n"
                                  "\r\n"
                               << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                               << "0000129610"
                               << "FAKENAME"
                               << "FAKEFIRSTNAME"
                               << "1A RUE DU FOUR"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "62580"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "18/12/2009"
                               << "1234567890123 75"
                               << "506232"
                               << "22/05/2013"
                               << "C LabFake"
                               << "FAKFAK FAKE.NAME"
                               << " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****FIN****\r\n"
                                  "****FINFICHIER****\r\n"
                                  "\r\n"
                               << true  // HPRIM rawContent isValid ?
                               << false // hprimContentIsNull
                               << 0     // HPRIM2 NumberOfLines
                                  ;

        // HPRIM 1 == Wrong content 1
        QTest::newRow("test4") << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                                  " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****FIN****\r\n"
                                  "blabla\r\n"            // <<<---- ERROR
                                  "****FINFICHIER****\r\n"
                                  "\r\n"
                               << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                               << "0000129610"
                               << "FAKENAME"
                               << "FAKEFIRSTNAME"
                               << "1A RUE DU FOUR"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "62580"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "18/12/2009"
                               << "1234567890123 75"
                               << "506232"
                               << "22/05/2013"
                               << "C LabFake"
                               << "FAKFAK FAKE.NAME"
                               << " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****FIN****\r\n"
                                  "blabla\r\n"            // <<<---- ERROR
                                  "****FINFICHIER****\r\n"
                                  "\r\n"
                               << false  // HPRIM rawContent isValid ?
                               << false // hprimContentIsNull
                               << 0     // HPRIM2 NumberOfLines
                                  ;

        // HPRIM 1 == Wrong content 2
        QTest::newRow("test5") << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                                  " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****FIN****\r\n"
                                  "****FINFICHIER****\r\n"
                                  "blabla\r\n"            // <<<---- ERROR
                                  "\r\n"
                               << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                               << "0000129610"
                               << "FAKENAME"
                               << "FAKEFIRSTNAME"
                               << "1A RUE DU FOUR"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "62580"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "18/12/2009"
                               << "1234567890123 75"
                               << "506232"
                               << "22/05/2013"
                               << "C LabFake"
                               << "FAKFAK FAKE.NAME"
                               << " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****FIN****\r\n"
                                  "****FINFICHIER****\r\n"
                                  "blabla\r\n"            // <<<---- ERROR
                                  "\r\n"
                               << false  // HPRIM rawContent isValid ?
                               << false // hprimContentIsNull
                               << 0     // HPRIM2 NumberOfLines
                                  ;

        // HPRIM 1 == Wrong content 3
        QTest::newRow("test6") << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                                  " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****FINFICHIER****\r\n"
                                  "****FIN****\r\n"
                                  "\r\n"
                               << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                               << "0000129610"
                               << "FAKENAME"
                               << "FAKEFIRSTNAME"
                               << "1A RUE DU FOUR"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "62580"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "18/12/2009"
                               << "1234567890123 75"
                               << "506232"
                               << "22/05/2013"
                               << "C LabFake"
                               << "FAKFAK FAKE.NAME"
                               << " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****FINFICHIER****\r\n"
                                  "****FIN****\r\n"
                                  "\r\n"
                               << false  // HPRIM rawContent isValid ?
                               << false // hprimContentIsNull
                               << 0     // HPRIM2 NumberOfLines
                                  ;

        // HPRIM 1 == Wrong content 4
        QTest::newRow("test7") << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                                  " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****FINFICHIER****\r\n"
                                  "\r\n"
                               << "0000129610\r\n"
                                  "FAKENAME\r\n"
                                  "FAKEFIRSTNAME\r\n"
                                  "1A RUE DU FOUR                \r\n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \r\n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT       \r\n" // 36 chars
                                  "18/12/2009\r\n"
                                  "1234567890123 75\r\n"
                                  "    506232\r\n"
                                  "22/05/2013\r\n"
                                  "         C LabFake                  \r\n"
                                  "     FAKFAK FAKE.NAME                                                        \r\n"
                               << "0000129610"
                               << "FAKENAME"
                               << "FAKEFIRSTNAME"
                               << "1A RUE DU FOUR"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "62580"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "18/12/2009"
                               << "1234567890123 75"
                               << "506232"
                               << "22/05/2013"
                               << "C LabFake"
                               << "FAKFAK FAKE.NAME"
                               << " \r\n"
                                  "                 ALLERGIE\r\n"
                                  "\r\n"
                                  "    I.G.E Totales                   164  UI/ml\r\n"
                                  "     Valeurs normales :           jusqu'a 1 an :  0 a 15 UI/ml\r\n"
                                  "                                  de  1 a 5 ans : 0 a 60 UI/ml\r\n"
                                  "                                  de  6 a 9 ans : 0 a 90 UI/ml\r\n"
                                  "                                  de 10 a 15 ans: 0 a 200 UI/ml\r\n"
                                  "                                  adultes     :   0 a 100 UI/ml\r\n"
                                  "\r\n"
                                  "****FINFICHIER****\r\n"
                                  "\r\n"
                               << false  // HPRIM rawContent isValid ?
                               << false // hprimContentIsNull
                               << 0     // HPRIM2 NumberOfLines
                                  ;
    }

    void testHprimParsers()
    {
        QFETCH(QString, hprimSource);
        QFETCH(QString, patientId);
        QFETCH(QString, patientName);
        QFETCH(QString, patientFirstName);
        QFETCH(QString, patientAddress1);
        QFETCH(QString, patientAddress2);
        QFETCH(QString, patientZip);
        QFETCH(QString, patientCity);
        QFETCH(QString, patientDob);
        QFETCH(QString, patientSN);
        QFETCH(QString, extraCode);
        QFETCH(QString, dateExam);
        QFETCH(QString, sender);
        QFETCH(QString, receiver);
        QFETCH(QString, message);
        QFETCH(bool, hprimContentIsValid);
        QFETCH(bool, hprimContentIsNull);
        QFETCH(int, hprim2NumberOfLines);

        // Read HPRIM message
        Utils::HPRIM::HprimMessage hprim = Utils::HPRIM::parseHprimRawSource(hprimSource);
        QVERIFY(hprim.isNull() == (false || hprimContentIsNull));
        QVERIFY(hprim.isValid() == (true && hprimContentIsValid));
        QVERIFY(hprim.header().isNull() == false);
        QVERIFY(hprim.header().isValid() == true);
        QVERIFY(hprim.rawContent().isNull() == hprimContentIsNull);
        QVERIFY(hprim.rawContent().isValid() == hprimContentIsValid);

        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientId), patientId);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientName), patientName);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientFirstName), patientFirstName);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientAddressFirstLine), patientAddress1);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientAddressSecondLine), patientAddress2);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientAddressZipCode), patientZip);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientAddressCity), patientCity);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientDateOfBirth), patientDob);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientSocialNumber), patientSN);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::ExtraCode), extraCode);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::DateOfExamination), dateExam);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::SenderIdentity), sender);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::ReceiverIdentity), receiver);
        QCOMPARE(hprim.rawContent().rawSource(), message);

        // Write HPRIM1
        QCOMPARE(hprim.toRawSource(), hprimSource);

        // Test basics of HPRIM2 content
        Utils::HPRIM::Hprim2Content h2(hprim.rawContent());
        QVERIFY(h2.numberOfLines() == hprim2NumberOfLines);
        if (h2.numberOfLines() == 0) {
            QVERIFY(h2.isNull() == true);
            QVERIFY(h2.isValid() == false);
        } else {
            QVERIFY(h2.isNull() == false);
            QVERIFY(h2.isValid() == true);
        }

        // Success
        QWARN("Test succeeded");
    }

    // TODO: add HPRIM2 data extraction
    // TODO: add createMessageRawContent()

    // test HPRIM v1 full message creation (see HprimMessage::toRawSource())
    void testHprimMessageCreation_data()
    {
        QTest::addColumn<QString>("hprimSource");
        QTest::addColumn<QString>("patientId");
        QTest::addColumn<QString>("patientName");
        QTest::addColumn<QString>("patientFirstName");
        QTest::addColumn<QString>("patientAddress1");
        QTest::addColumn<QString>("patientAddress2");
        QTest::addColumn<QString>("patientZip");
        QTest::addColumn<QString>("patientCity");
        QTest::addColumn<QString>("patientDob");
        QTest::addColumn<QString>("patientSN");
        QTest::addColumn<QString>("extraCode");
        QTest::addColumn<QString>("dateExam");
        QTest::addColumn<QString>("sender");
        QTest::addColumn<QString>("receiver");
        QTest::addColumn<bool>("headerIsValid");
        QTest::addColumn<bool>("dobIsValid");

        QTest::newRow("test1") << "0000129610\n"
                                  "FAKENAME\n"
                                  "FAKEFIRSTNAME\n"
                                  "1A RUE DU FOUR                \n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT\n"
                                  "18/12/2009\n"
                                  "1234567890123 75\n"
                                  "506232\n"
                                  "22/05/2013\n"
                                  "C LabFake Sender\n"
                                  "FAKFAK FAKE.RECEIVER\n"
                                  " \n"
                               << "0000129610"
                               << "FAKENAME"
                               << "FAKEFIRSTNAME"
                               << "1A RUE DU FOUR"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "62580"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "18/12/2009"
                               << "1234567890123 75"
                               << "506232"
                               << "22/05/2013"
                               << "C LabFake Sender"
                               << "FAKFAK FAKE.RECEIVER"
                               << true      // Header is Valid
                               << true      // dobIsValid
                                  ;

        // Wrong dateofbirth
        QTest::newRow("test2") << "0000129610\n"
                                  "FAKENAME\n"
                                  "FAKEFIRSTNAME\n"
                                  "1A RUE DU FOUR                \n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT\n"
                                  "\n"
                                  "1234567890123 75\n"
                                  "506232\n"
                                  "22/05/2013\n"
                                  "C LabFake Sender\n"
                                  "FAKFAK FAKE.RECEIVER\n"
                                  " \n"
                               << "0000129610"
                               << "FAKENAME"
                               << "FAKEFIRSTNAME"
                               << "1A RUE DU FOUR"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "62580"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "18122009"
                               << "1234567890123 75"
                               << "506232"
                               << "22/05/2013"
                               << "C LabFake Sender"
                               << "FAKFAK FAKE.RECEIVER"
                               << false     // Header is Valid
                               << false     // dobIsValid
                                  ;

        // Wrong dateofbirth
        QTest::newRow("test3") << "0000129610\n"
                                  "FAKENAME\n"
                                  "FAKEFIRSTNAME\n"
                                  "1A RUE DU FOUR                \n" // 30 chars
                                  "BAILLEUL SIRE BERTHOULT       \n" // 30 chars
                                  "62580 BAILLEUL SIRE BERTHOULT\n"
                                  "\n"
                                  "1234567890123 75\n"
                                  "506232\n"
                                  "22/05/2013\n"
                                  "C LabFake Sender\n"
                                  "FAKFAK FAKE.RECEIVER\n"
                                  " \n"
                               << "0000129610"
                               << "FAKENAME"
                               << "FAKEFIRSTNAME"
                               << "1A RUE DU FOUR"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "62580"
                               << "BAILLEUL SIRE BERTHOULT"
                               << "18/13/2009"
                               << "1234567890123 75"
                               << "506232"
                               << "22/05/2013"
                               << "C LabFake Sender"
                               << "FAKFAK FAKE.RECEIVER"
                               << false     // Header is Valid
                               << false     // dobIsValid
                                  ;
    }

    void testHprimMessageCreation()
    {
        QFETCH(QString, hprimSource);
        QFETCH(QString, patientId);
        QFETCH(QString, patientName);
        QFETCH(QString, patientFirstName);
        QFETCH(QString, patientAddress1);
        QFETCH(QString, patientAddress2);
        QFETCH(QString, patientZip);
        QFETCH(QString, patientCity);
        QFETCH(QString, patientDob);
        QFETCH(QString, patientSN);
        QFETCH(QString, extraCode);
        QFETCH(QString, dateExam);
        QFETCH(QString, sender);
        QFETCH(QString, receiver);
        QFETCH(bool, headerIsValid);
        QFETCH(bool, dobIsValid);

        // Create the header
        Utils::HPRIM::HprimHeader hdr;
        hdr.setData(Utils::HPRIM::HprimHeader::PatientId, patientId);
        hdr.setData(Utils::HPRIM::HprimHeader::PatientName, patientName);
        hdr.setData(Utils::HPRIM::HprimHeader::PatientFirstName, patientFirstName);
        hdr.setData(Utils::HPRIM::HprimHeader::PatientAddressFirstLine, patientAddress1);
        hdr.setData(Utils::HPRIM::HprimHeader::PatientAddressSecondLine, patientAddress2);
        hdr.setData(Utils::HPRIM::HprimHeader::PatientAddressZipCode, patientZip);
        hdr.setData(Utils::HPRIM::HprimHeader::PatientAddressCity, patientCity);
        hdr.setData(Utils::HPRIM::HprimHeader::PatientSocialNumber, patientSN);
        hdr.setData(Utils::HPRIM::HprimHeader::ExtraCode, extraCode);
        hdr.setData(Utils::HPRIM::HprimHeader::DateOfExamination, dateExam);
        hdr.setData(Utils::HPRIM::HprimHeader::SenderIdentity, sender);
        hdr.setData(Utils::HPRIM::HprimHeader::ReceiverIdentity, receiver);
        QVERIFY(hdr.setData(Utils::HPRIM::HprimHeader::PatientDateOfBirth, patientDob) == dobIsValid);
        QVERIFY(hdr.isValid() == headerIsValid);
        QCOMPARE(hdr.rawSource(), hprimSource);

        // Create a fake message
        QString msg = "bla bla bla\nQQ\nlkjlsf lkj oqsdif&\"'ç!è\n";
        Utils::HPRIM::HprimRawContent content = Utils::HPRIM::createMessageRawContent(msg);

        // Re-Read HPRIM message
        Utils::HPRIM::HprimMessage hprim;
        hprim.setHeader(hdr);
        hprim.setRawContent(content);

        QVERIFY(hprim.isNull() == false);
        QVERIFY(hprim.isValid() == (true && headerIsValid));
        QVERIFY(hprim.header().isNull() == false);
        QVERIFY(hprim.header().isValid() == headerIsValid);
        QVERIFY(hprim.rawContent().isNull() == false);
        QVERIFY(hprim.rawContent().isValid() == true);

        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientId), patientId);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientName), patientName);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientFirstName), patientFirstName);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientAddressFirstLine), patientAddress1);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientAddressSecondLine), patientAddress2);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientAddressZipCode), patientZip);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientAddressCity), patientCity);
        if (dobIsValid)
            QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientDateOfBirth), patientDob);
        else
            QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientDateOfBirth), QString());
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::PatientSocialNumber), patientSN);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::ExtraCode), extraCode);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::DateOfExamination), dateExam);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::SenderIdentity), sender);
        QCOMPARE(hprim.header().data(Utils::HPRIM::HprimHeader::ReceiverIdentity), receiver);

        // Test creation of HPRIM full source
        QString endTags = "****FIN****\n****FINFICHIER****\n";
        QString full = hprimSource + msg + endTags;
        QCOMPARE(hprim.toRawSource(), full);

        QWARN("Test succeeded");
    }

    void cleanupTestCase()
    {}
};

DECLARE_TEST(tst_Hprim)
#include "tst_hprim.moc"

