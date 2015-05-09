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
        freemedforms.forms.namespaceInUse = "";
        var formItem = freemedforms.forms.item("Subs::Tools::SOAP::WithBio");
        print(formItem);
        formUi = formItem.ui();
        syst = formUi.findChild("bpSyst");
        diast = formUi.findChild("bpDiast");
        pulse = formUi.findChild("bpPulse");

//      populateCombos();
        
        // Connect data changed on spins
        // Spin should use valueChanged(int)
        // DoubleSpin should use valueChanged(double)

        syst['valueChanged(int)'].connect(this, computePulsePressure);          
        diast['valueChanged(int)'].connect(this, computePulsePressure);
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
});

namespace.com.freemedforms.generic.soapWithBio.setupUi();
