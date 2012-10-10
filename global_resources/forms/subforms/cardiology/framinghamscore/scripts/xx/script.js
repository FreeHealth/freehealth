namespace.module('com.freemedforms.cardiology.framingham.score', function (exports, require) {

                     exports.extend({
                       'setupUi': setupUi,
                       'computeScore': computeScore
                     });

                     // Ui vars (retrieved from the ui)
                     var formUi, cholCombo, hdlCombo, systCombo, tobaccoCheckbox, bpSystTreatedCheckbox;
                     var totalCholRanges = [ "< 1,60" ,
                                            "1,60 - 1,99",
                                            "2,00 - 2,39",
                                            "2,40 - 2,79",
                                            ">= 2,80"];
                     var hdlCholRanges = [ ">= 0,60", "0,59 - 0,50", "0,49 - 0,40", "< 0,40" ];
                     var systolicRanges = [ "<= 120",
                                           "120 - 129",
                                           "130 - 139",
                                           "140 - 149",
                                           "150- 159",
                                           ">= 160"];

                     function setupUi() {
                         freemedforms.forms.namespaceInUse = "";
                         freemedforms.forms.languageChanged.connect(this, retranslateUi);
                         var formItem = freemedforms.forms.item("Subs::Cardiology::Scores::Framingham");
                         formUi = formItem.ui();
                         cholCombo = formUi.findChild("cholTotalCombo");
                         hdlCombo = formUi.findChild("cholHDLCombo");
                         systCombo = formUi.findChild("bpSystCombo");
                         tobaccoCheckbox = formUi.findChild("currentlySmoking");
                         bpSystTreatedCheckbox = formUi.findChild("bpSystTreated");
                         populateCombos();
                         // Update patient data
                         retranslateUi();
                         enableDependAge();
                     }

                     function retranslateUi() {
                         var lang = freemedforms.forms.currentLanguage;
                         // Age
                         var ageLabel = formUi.findChild("patientAge");
                         if (lang == "fr")
                             ageLabel.text = freemedforms.patient.yearsOld + " ans";
                         else
                             ageLabel.text = freemedforms.patient.yearsOld + " years";
                         // Gender
                         var genderLabel = formUi.findChild("patientGender");
                         if (freemedforms.patient.isMale) {
                             if (lang=="fr")
                                 genderLabel.text = "Sexe masculin";
                             else
                                 genderLabel.text = "Male";
                         } else if (freemedforms.patient.isFemale) {
                             if (lang=="fr")
                                 genderLabel.text = "Sexe féminin";
                             else
                                 genderLabel.text = "Female";
                         } else {
                             if (lang=="fr")
                                 genderLabel.text = "Sexe inconnu";
                             else
                                 genderLabel.text = "Not computable";
                         }
                     }

                     function populateCombos() {
                         freemedforms.uiTools.addItems(cholCombo, totalCholRanges);
                         freemedforms.uiTools.addItems(hdlCombo, hdlCholRanges);
                         freemedforms.uiTools.addItems(systCombo, systolicRanges);
                     }

                     function getTotalCholesterolInRange() {
                         return 150 * (cholCombo.currentIndex*40);
                     }

                     function getHdlCholesterolInRange() {
                         return 65 - (hdlCombo.currentIndex * 10);
                     }

                     function getSystolicInRange() {
                         return 115 + (systCombo.currentIndex * 10);
                     }

                     function enableDependAge() {
                         var age =  freemedforms.patient.yearsOld;
                         if (age < 20 || age >= 80 ) {
                         cholCombo.enabled = false;
                         hdlCombo.enabled = false;
                         systCombo.enabled = false;
                         bpSystTreatedCheckbox.enabled = false;
                         tobaccoCheckbox.enabled = false; }
                     }

                     function computeScore() {
                         freemedforms.forms.namespaceInUse = "Subs::Cardiology::Scores::Framingham";
                         var chol = getTotalCholesterolInRange();
                         var hdl = getHdlCholesterolInRange();
                         var tobacco = freemedforms.forms.item("TaboccoUsage").checked;
                         var bp = getSystolicInRange();
                         var bp_treated = freemedforms.forms.item("BloodPresGroup::Treated").checked;
                         var score = 0;
                         var riskscore;
                         var lang = freemedforms.forms.currentLanguage;
                         var enabledforms = freemedforms.forms.item("FraminghamGroup").enabled;
                         var age = freemedforms.patient.yearsOld;

                         // Verification patient age
                         if (age < 20 || age >= 80 ) {
                             if (lang == "xx") {
                                 riskscore ="Patient age not evaluated in the Framingham Score "; return riskscore;
                             } else if (lang=="fr") {
                                 riskscore ="Tranche d'\u00E2ge non \u00E9valu\u00E9e par le score de Framingham"; return riskscore;
                             }
                         }

                         if (freemedforms.patient.isFemale) {
                             //Age
                             if (age>=20 && age < 35) score -=7;
                             else if (age >= 35 && age < 40) score -=3;
                             else if (age >=40 && age < 44) score -= 0;
                             else if (age >=45 && age < 50) score += 3;
                             else if (age >=50 && age < 55) score += 6;
                             else if (age >=55 && age < 60) score += 8;
                             else if (age >=60 && age < 65) score += 10;
                             else if (age >=65 && age < 70) score += 12;
                             else if (age >=70 && age < 75) score += 14;
                             else   score += 16;

                             //chol
                             if (age >= 20 && age < 40) {
                                 if (chol < 160) score -= 0;
                                 else if (chol >= 160 && chol < 200) score += 4;
                                 else if (chol >= 200 && chol < 240) score += 8;
                                 else if (chol >= 240 && chol < 280) score += 11;
                                 else  score += 13;
                             }
                             else if (age >= 40 && age < 50){
                                 if (chol <= 159) score -= 0;
                                 else if (chol >= 160 && chol <= 199) score += 3;
                                 else if (chol >= 200 && chol <= 239) score += 6;
                                 else if (chol >= 240 && chol <= 279) score += 8;
                                 else  score += 10;
                             }
                             else if (age >= 50 && age < 60){
                                 if (chol < 160) score -= 0;
                                 else if (chol >= 160 && chol < 200) score += 2;
                                 else if (chol >= 200 && chol < 240) score += 4;
                                 else if (chol >= 240 && chol < 280) score += 5;
                                 else   score += 7;
                             }
                             else  if (age >= 60 && age < 70){
                                 if (chol < 160) score -= 0;
                                 else if (chol >= 160 && chol < 200) score += 1;
                                 else if (chol >= 200 && chol < 240) score += 2;
                                 else if (chol >= 240 && chol < 280) score += 3;
                                 else   score += 4;
                             }
                             else {
                                 if (chol < 160) score -= 0;
                                 else if (chol >= 160 && chol < 200) score += 1;
                                 else if (chol >= 200 && chol < 240) score += 1;
                                 else if (chol >= 240 && chol < 280) score += 2;
                                 else   score += 2;
                             }
                             //tobacco
                             if (tobacco){
                                 if (age >= 20 && age < 40) score +=9;
                                 else if (age >=40 && age < 50) score += 7;
                                 else if (age >=50 && age < 60) score += 4;
                                 else if (age >=60 && age < 70) score += 2;
                                 else  score += 1;
                             }
                             //HDL Cholesterol
                             if (hdl >=60) score += -1;
                             else if (hdl >=50 && hdl < 60) score+= 0;
                             else if (hdl >=40 && hdl < 50) score+= 1;
                             else   score += 2;
                             // Systolic blood pressure
                             if (!bp_treated){
                                 if (bp < 120) score += 0;
                                 else if (bp >=120 && bp < 130)  score += 1;
                                 else if (bp >=130 && bp < 140)  score += 2;
                                 else if (bp >=140 && bp < 160)  score += 3;
                                 else    score += 4;
                             }
                             else {
                                 if (bp < 120) score += 0;
                                 else if (bp >=120 && bp < 130)  score += 3;
                                 else if (bp >=130 && bp < 140)  score += 4;
                                 else  if (bp >=140 && bp < 160) score += 5;
                                 else  score += 6;
                             }
                             // Calcul of risk score
                             if (score <9) riskscore = "< 1%";
                             else if (score >=9 && score <= 12) riskscore = "1%";
                             else if (score >=13 && score <= 14) riskscore = "2%";
                             else if (score == 15) riskscore = "3%";
                             else if (score == 16) riskscore = "4%";
                             else if (score == 17) riskscore = "5%";
                             else if (score == 18) riskscore = "6%";
                             else if (score == 19) riskscore = "8%";
                             else if (score == 20) riskscore = "11%";
                             else if (score == 21) riskscore = "14%";
                             else  if (score == 22) riskscore = "17%";
                             else  if (score == 23) riskscore = "22%";
                             else if (score == 24) riskscore = "27%";
                             else   riskscore = "\u226530%";
                             return riskscore;
                         }
                         else {  // Patient is male
                             //Age
                             if (age < 35) score -=9;
                             else if (age >=35 && age < 40) score -=4;
                             else if (age >=40 && age < 45) score -= 0;
                             else if (age >=45 && age < 50) score += 3;
                             else if (age >=50 && age < 55) score += 6;
                             else if (age >=55 && age < 60) score += 8;
                             else if (age >=60 && age < 65) score += 10;
                             else if (age >=65 && age < 70) score += 11;
                             else if (age >=70 && age < 75) score += 12;
                             else  score += 13;
                         }
                         //chol
                         if (age >=20 && age <40){
                             if (chol < 160) score -= 0;
                             else if (chol >=160 && chol < 200) score += 4;
                             else if (chol >=200 && chol < 240) score += 7;
                             else if (chol >=240 && chol < 280) score += 9;
                             else  score += 11;
                         }
                         else if (age >=40 && age <50){
                             if (chol < 160) score -= 0;
                             else if (chol >=160 && chol < 200) score += 3;
                             else if (chol >=200 && chol < 240) score += 5;
                             else if (chol >=240 && chol < 280) score += 6;
                             else   score += 8;
                         }
                         else if (age > 49 && age <60){
                             if (chol < 160) score -= 0;
                             else if (chol >=160 && chol < 200) score += 2;
                             else if (chol >=200 && chol < 240) score += 3;
                             else if (chol >=240 && chol < 280) score += 4;
                             else   score += 5;
                         }
                         else  if (age >=60 && age <70){
                             if (chol < 160) score -= 0;
                             else if (chol >=160 && chol < 200) score += 1;
                             else if (chol >=200 && chol < 240) score += 1;
                             else if (chol >=240 && chol > 280) score += 2;
                             else   score += 3;
                         }
                         else   {
                             if (chol < 160) score -= 0;
                             else if (chol >=160 && chol < 200) score += 0;
                             else if (chol >=200 && chol < 240) score += 0;
                             else if (chol >=240 && chol < 280) score += 1;
                             else   score += 1;
                         }
                         //tobacco
                         if (tobacco){
                             if (age >= 20 && age < 40) score +=8;
                             else if (age >=40 && age < 50) score += 5;
                             else if (age >=50 && age < 60) score += 3;
                             else if (age >=60 && age < 70) score += 1;
                             else   score += 1;
                         }
                         //HDL Cholesterol
                         if (hdl >=60) score += -1;
                         else if (hdl >=50 && hdl < 60) score+= 0;
                         else if (hdl >=40 && hdl < 50) score+= 1;
                         else   score += 2;
                         // Systolic blood pressure
                         if (!bp_treated){
                             if (bp < 120) score += 0;
                             else if (bp >=120 && bp < 130)  score += 1;
                             else if (bp >=130 && bp < 140)  score += 2;
                             else  if (bp >=140 && bp < 160) score += 3;
                             else   score += 4;
                         }
                         else {
                             if (bp < 120) score += 0;
                             else if (bp >=120 && bp < 130)  score += 3;
                             else if (bp >=130 && bp < 140)  score += 4;
                             else  if (bp >=140 && bp < 160) score += 5;
                             else   score += 6;
                         }
                         if (score <0) riskscore =" < 1% ";
                         else if (score>=0 && score <= 4) riskscore="1%";
                         else if (score>=5 && score <= 6) riskscore="2%";
                         else if (score==7) riskscore="3%";
                         else if (score==8) riskscore="4%";
                         else if (score==9) riskscore="5%";
                         else if (score ==10)  riskscore = "6%";
                         else if (score ==11) riskscore = "8%";
                         else if (score ==12) riskscore = "10%";
                         else if (score ==13) riskscore = "12%";
                         else if (score ==14) riskscore = "16%";
                         else if (score ==15) riskscore = "20%";
                         else if (score ==16) riskscore = "25%";
                         else   riskscore = "\u226530%";


                         // DEBUG
//                         print("age:" + age + "; " +
//                               "chol:"+ chol + "; " +
//                               "hdl:"+ hdl + "; " +
//                               "tobacco:"+ tobacco + "; " +
//                               "bp:"+ bp + "; " +
//                               "bp_treated:"+ bp_treated + "; " +
//                               "score:"+ score + "; " +
//                               "riskscore:"+ riskscore);
                         return riskscore;
                     }

});

namespace.com.freemedforms.cardiology.framingham.score.setupUi();
