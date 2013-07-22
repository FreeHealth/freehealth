namespace.module('com.freemedforms.generic.soapWithBio', function (exports, require) {

    exports.extend({
                       'setupUi': setupUi,
                       'computePulsePressure': computePulsePressure
                   });

    // Ui vars (retrieved from the ui)
    var syst, diast, pulse;
//    var weight, weightUnit, height, heightUnit;

    function setupUi() {
        print("com.freemedforms.generic.soapWithBio Setup UI");
        freemedforms.forms.namespaceInUse = "";
        var formItem = freemedforms.forms.item("Subs::Tools::SOAP::WithBio");
        print(formItem);
        formUi = formItem.ui();
        syst = formUi.findChild("bpSyst");
        diast = formUi.findChild("bpDiast");
        pulse = formUi.findChild("bpPulse");
//        weight = formUi.findChild("weight");
//        weightUnit = formUi.findChild("weightUnit");
//        height = formUi.findChild("height");
//        heightUnit = formUi.findChild("heightUnit");

        populateCombos();

        // connect data changed on spins
        syst['valueChanged(int)'].connect(this, computePulsePressure);
        diast['valueChanged(int)'].connect(this, computePulsePressure);

//        weight['valueChanged(int)'].connect(this, computeBMI);
//        height['valueChanged(int)'].connect(this, computeBMI);
//        weightUnit['currentIndexChanged(int)'].connect(this, computeBMI);
//        heightUnit['currentIndexChanged(int)'].connect(this, computeBMI);

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

//    function computeBMI() {
//        var metricWeight = weight.value;
//    }

});

namespace.com.freemedforms.generic.soapWithBio.setupUi();
