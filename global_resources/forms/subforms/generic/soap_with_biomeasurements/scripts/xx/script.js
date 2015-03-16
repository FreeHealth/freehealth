namespace.module('com.freemedforms.generic.soapWithBio', function (exports, require) {

    exports.extend({
                       'setupUi': setupUi,
                       'computePulsePressure': computePulsePressure
                   });

    // Ui vars (retrieved from the ui)
    var syst, diast, pulse;

    function setupUi() {
        print("com.freemedforms.generic.soapWithBio Setup UI");

        // Get items to work with
        freemedforms.forms.namespaceInUse = "Subs::Tools::SOAP::WithBio";
        syst = freemedforms.forms.item("ObjectiveGroup::BloodPressure::Syst");
        diast = freemedforms.forms.item("ObjectiveGroup::BloodPressure::Diast");
        pulse = freemedforms.forms.item("ObjectiveGroup::BloodPressure::Pulse");

        // Connect item data changed
        //syst.ui['valueChanged(int)'].connect(this, computePulsePressure);
        print(syst);
        print(syst.ui);
        //diast.ui['valueChanged(int)'].connect(this, computePulsePressure);
    }

    function retranslateUi() {
        var lang = freemedforms.forms.currentLanguage;
    }

    function computePulsePressure() {
        var text = syst.currentValue - diast.currentValue;
        pulse.currentText = text;
    }
});

namespace.com.freemedforms.generic.soapWithBio.setupUi();
