namespace.module('com.freemedforms.generic.pediatrics', function (exports, require) {

    exports.extend({
                       'setupUi': setupUi,
                       'computePulsePressure': computePulsePressure,
	               'computeBMI' : computeBMI
                   });

    // Ui vars (retrieved from the ui)
    var syst, diast, pulse;
    var weight, weightUnit, height, heightUnit, bmi;

    function setupUi() {
        print("com.freemedforms.generic.pediatrics Setup UI");
        freemedforms.forms.namespaceInUse = "";
        var formItem = freemedforms.forms.item("Subs::Tools::Pediatrics::WithBio");
        print(formItem);
        formUi = formItem.ui();
        syst = formUi.findChild("BloodPressureSystolicValue");
        diast = formUi.findChild("BloodPressureDiastolicValue");
        pulse = formUi.findChild("BloodPressurePulseValue");
	bmi = formUi.findChild("BmiValue");
        weight = formUi.findChild("WeightValue");
        weightUnit = formUi.findChild("WeightUnit");
        height = formUi.findChild("HeightValue");
        heightUnit = formUi.findChild("HeightUnit");

        populateCombos();

        // connect data changed on spins
        syst['valueChanged(int)'].connect(this, computePulsePressure);
        diast['valueChanged(int)'].connect(this, computePulsePressure);

        weight['valueChanged(int)'].connect(this, computeBMI);
        height['valueChanged(int)'].connect(this, computeBMI);
        weightUnit['currentIndexChanged(int)'].connect(this, computeBMI);
        heightUnit['currentIndexChanged(int)'].connect(this, computeBMI);

    }

    function retranslateUi() {
        var lang = freemedforms.forms.currentLanguage;
    }

    function populateCombos() {
        // TODO: add weight and height units
//        freemedforms.uiTools.addItems(cholCombo, totalCholRanges);
//        freemedforms.uiTools.addItems(hdlCombo, hdlCholRanges);
//        freemedforms.uiTools.addItems(systCombo, systolicRanges);
    }

    function prepareSlider() {

    }

    function computePulsePressure() {
        var text = syst.value - diast.value;
        pulse.setText(text);
    }

    function computeBMI() {
	var metricweight = weight.value;
	var metricheight = height.value;
        var textbmi  = (metricweight) / ((metricheight) * (metricheight));
        bmi.setText(textbmi);
    }

});

namespace.com.freemedforms.generic.pediatrics.setupUi();
