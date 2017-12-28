namespace.module('com.freemedforms.generic.soap_with_biomeasurements_version_2', function (exports, require) {
    var syst;
    var diast;
    var pulse;
    var weight;
    var weightUnit;
    var weightUnitItem;
    var height;
    var heightUnit;
    var heightUnitItem;
    var bmiValueLabel;
    var bmiValueLineEdit;
    
    exports.extend({
                       'setupUi': setupUi,
                       'soap_computePulsePressure': soap_computePulsePressure,
                       'soap_computeBMI': soap_computeBMI
                   });

    function setupUi() {
        soap_getUiElements();
        soap_connectUiElements();
        soap_computePulsePressure();
        soap_computeBMI();
    }

    function soap_getUiElements() {
        freemedforms.forms.namespaceInUse = "";
        var formItem = freemedforms.forms.item("Subs::Tools::SOAP::WithBio");
        var formUi = formItem.ui();
        syst = formUi.findChild("bpSyst");
        diast = formUi.findChild("bpDiast");
        pulse = formUi.findChild("bpPulse");
        bmiValueLabel = formUi.findChild("bmiValueLabel");
        bmiValueLineEdit = formUi.findChild("bmiValueLineEdit");
        weight = formUi.findChild("weight");
        weightUnit = formUi.findChild("weightUnit");
        weightUnitItem = freemedforms.forms.item("Subs::Tools::SOAP::WithBio::ObjectiveGroup::Weight::Value");
        height = formUi.findChild("height");
        heightUnit = formUi.findChild("heightUnit");
        heightUnitItem = freemedforms.forms.item("Subs::Tools::SOAP::WithBio::ObjectiveGroup::Height::Value");
        bmiValueLineEdit.visible = false;
    }

    function soap_connectUiElements()
    {
        syst['valueChanged(int)'].connect(this, soap_computePulsePressure);
        diast['valueChanged(int)'].connect(this, soap_computePulsePressure);
        weight['valueChanged(double)'].connect(this, soap_computeBMI);
        height['valueChanged(double)'].connect(this, soap_computeBMI);
        weightUnit['currentIndexChanged(int)'].connect(this, soap_computeBMI);
        heightUnit['currentIndexChanged(int)'].connect(this, soap_computeBMI);
    }

//    function retranslateUi() {
//        var lang = freemedforms.forms.currentLanguage;
//    }

    function soap_computePulsePressure() {
        var text = syst.value - diast.value;
        pulse.setText(text);
    }

    function soap_roundToOne(num) {
        return +(Math.round(num + "e+1")  + "e-1");
    }

    function soap_weightToKilogram(weightUnit, weightValue) {
        var ounceToKilogram = 0.028349523125; //International avoirdupois ounce
        var poundToKilogram = 0.45359237; //International avoirdupois pound
        var gramToKilogram = 0.001;
        var kilogram = "Kilogram";
        var gram = "Gram";
        var ounce_en = "Ounce";
        var ounce_fr = "Once";
        var pound_en = "Pound";
        var pound_fr = "Livre";
        if (~weightUnit.indexOf(gram)) {
            return weightValue*gramToKilogram;
        } else if (~weightUnit.indexOf(kilogram)) {
            return weightValue;
        } else if (~weightUnit.indexOf(ounce_en)||~weightUnit.indexOf(ounce_fr)) {
            return weightValue*ounceToKilogram;
        } else if (~weightUnit.indexOf(pound_en)||~weightUnit.indexOf(pound_fr)) {
            return weightValue*poundToKilogram;
        }
    }

    function soap_heightToMeter(heightUnit, heightValue) {
        var centimeterToMeter = 0.01;
        var inchToMeter = 0.0254;
        var footToMeter = 0.3048;
        var centimeter_en = "Centimeter";
        var centimeter_fr = "Centimètre";
        var meter_en = "Meter";
        var meter_fr = "Mètre";
        var inch_en = "Inch";
        var inch_fr = "Pouce";
        var foot_en = "Foot";
        var foot_fr = "Pied";
        if (~heightUnit.indexOf(centimeter_en) || ~heightUnit.indexOf(centimeter_fr)) {
            return heightValue*centimeterToMeter;
        } else if (~heightUnit.indexOf(meter_en) || ~heightUnit.indexOf(meter_fr)) {
            return heightValue;
        } else if (~heightUnit.indexOf(inch_en) || ~heightUnit.indexOf(inch_fr)) {
            return heightValue*inchToMeter;
        } else if (~heightUnit.indexOf(foot_en) || ~heightUnit.indexOf(foot_fr)) {
            return heightValue*footToMeter;
        }
    }

    function soap_computeBMI() {
        var weightUnit = weightUnitItem.currentText;
        var weightValue = Number(weight.value);
        var weightKilogram = soap_weightToKilogram(weightUnit, weightValue);
        var heightUnit = heightUnitItem.currentText;
        var heightValue = Number(height.value);
        var heightMeter = soap_heightToMeter(heightUnit, heightValue);
        var bmi  = (weightKilogram) / ((heightMeter) * (heightMeter));
        bmi = soap_roundToOne(bmi);
        var textbmi = bmi.toString();
        if (!isNaN(bmi)) {
            bmiValueLineEdit.setText(textbmi);
            bmiValueLabel.setText(textbmi);
        } else {
            bmiValueLineEdit.setText("");
            bmiValueLabel.setText("");
        }
    }
});

namespace.com.freemedforms.generic.soap_with_biomeasurements_version_2.setupUi();
