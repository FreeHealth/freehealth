namespace.module('com.freemedforms.generic.soapWithBio', function (exports, require) {

    exports.extend({
                       'setupUi': setupUi,
                       'computePulsePressure': computePulsePressure,
                       'computeBMI' : computeBMI,
                       'heightToMeter' : heightToMeter
                   });

    // Ui vars (retrieved from the ui)
    var syst, diast, pulse;
    var bmi, weight, weightUnit, weightUnitItem, height, heightUnit, heightUnitItem, bmiValueLabel, bmiValueLineEdit;

    function setupUi() {
        print("com.freemedforms.generic.soapWithBio Setup UI");

        // Get items to work with
        freemedforms.forms.namespaceInUse = "";
        var formItem = freemedforms.forms.item("Subs::Tools::SOAP::WithBio");
        print(formItem);
        formUi = formItem.ui();
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

//      populateCombos();
        
        // Connect data changed on spins
        // Spin should use valueChanged(int)
        // DoubleSpin should use valueChanged(double)

        syst['valueChanged(int)'].connect(this, computePulsePressure);          
        diast['valueChanged(int)'].connect(this, computePulsePressure);

        weight['valueChanged(double)'].connect(this, computeBMI);
        height['valueChanged(double)'].connect(this, computeBMI);
        weightUnit['currentIndexChanged(int)'].connect(this, computeBMI);
        heightUnit['currentIndexChanged(int)'].connect(this, computeBMI);

        // hide bmiValueLineEdit
        bmiValueLineEdit.visible = false;
    }

//     function populateCombos() {                                                 
       // TODO: add weight and height units                                    
//        freemedforms.uiTools.addItems(cholCombo, totalCholRanges);            
//        freemedforms.uiTools.addItems(hdlCombo, hdlCholRanges);               
//        freemedforms.uiTools.addItems(systCombo, systolicRanges);             
//    } 

    function retranslateUi() {
        var lang = freemedforms.forms.currentLanguage;
    }

    function computePulsePressure() {
        var text = syst.value - diast.value;                                    
        pulse.setText(text);                                                    
    }

    function roundToOne(num) {
        return +(Math.round(num + "e+1")  + "e-1");
    }

    function weightToKilogram(weightUnit, weightValue) {
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

    function heightToMeter(heightUnit, heightValue) {
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

    function computeBMI() {
        var weightUnit = weightUnitItem.currentText;
        var weightValue = Number(weight.value);
        var weightKilogram = weightToKilogram(weightUnit, weightValue);
        var heightUnit = heightUnitItem.currentText;
        var heightValue = Number(height.value);
        var heightMeter = heightToMeter(heightUnit, heightValue);
        var bmi  = (weightKilogram) / ((heightMeter) * (heightMeter));
        bmi = roundToOne(bmi);
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

namespace.com.freemedforms.generic.soapWithBio.setupUi();
