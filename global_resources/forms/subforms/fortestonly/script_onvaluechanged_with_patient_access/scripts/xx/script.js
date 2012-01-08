function getClearance()
{
    freemedforms.forms.namespaceInUse = "Test::Script::ScriptCalculator::WithPatient::FakeClCrGroup";
    var w = freemedforms.forms.item("Weight").currentValue;
    var wUnit = freemedforms.forms.item("WeightUnit").currentText;
    var cr = freemedforms.forms.item("Creatinine").currentValue;
    var crUnit = freemedforms.forms.item("CreatinineUnit").currentText;
    var ageYears = freemedforms.patient.yearsOld;

    // Clairance créatinine (ml / min) = [(140 - âge (années)) x Poids (kg) x F] / [7,2 x créatininémie (mg/l)]

    // Manage Gender modulation in Cockroft-Gault formula
    // for males == 1.04
    var genderMod = 1.04;
    // for females == 0.85
    if (freemedforms.patient.isFemale)
        genderMod = 0.85;

    // Ensure weight is in Kg
    if (wUnit=="lbl") {
        w *= 0.45359237;
    }

    // Ensure creatinine is in mg/L
    if (crUnit != "mg/L") {
        // Creatinine	mg/L	* 8.85	 -> µmol/L
        cr /= 8.85;
    }

    // ClCr
    var clcr = (((140-ageYears)*w) / (7.2*cr)) * genderMod;
    clcr += " ml/min";

    var html = "<p>Test using the Cockroft&Gault formula<br />";
    html += "Used formula: <b>(((140-ageYears)*w) / (7.2*cr)) * genderMod</b><br />";
    html += "Used values are:<br />";
    html += "&nbsp;&nbsp;- age in years: "+ageYears + "<br />";
    html += "&nbsp;&nbsp;- weight in Kg: "+w + "<br />";
    html += "&nbsp;&nbsp;- creatinine in mg/L: "+cr + "<br />";
    html += "&nbsp;&nbsp;- gender modulator: "+genderMod + "<br />";
    html += "Result is:<b>" + clcr + " ml/min</b><br />";
    html += "</p>";
    return html;
}

