namespace.module('com.freemedforms.prescriptions.fr.bizone', function (exports, require) {
    var bz_affectionExoneranteHeaderTextEdit;
    var bz_maladiesIntercurrentesHeaderTextEdit;
    var bz_bizoneCheckUi;
    var bz_bizoneCheckItem;
    var orderText;
    var bz_orderItem;
    var bz_orderUi;
    var bz_affectionExoneranteItem;
    var bz_maladiesIntercurrentesItem;
    var bz_affectionExoneranteUi;
    var bz_maladiesIntercurrentesUi;
    var bz_rppsLineEdit;
    var bz_headerUi;
    var bz_current;
    var bz_headerItem;
    var bz_radioItem;
    var bz_bizoneCollapsibleGroupItem;
    var bz_bizoneCollapsibleGroupUi;
    var bz_bizoneWarningLabelUi;
    var warning ="Vous devez retourner en mode bizone et effacer la prescription relative à l'ALD.";
    var originalAndWarning = "";
    var original= "";
    var bz_warningUi;
    var bz_warningItem;

  exports.extend({
    'setupUi': setupUi,
    'bz_onCheckChanged': bz_onCheckChanged
  });

                     function setupUi() {
                         bz_getUiElements();
                         bz_connectUiElements();
                         bz_onCheckChanged();
                     }


function bz_getUiElements() {
        freemedforms.forms.namespaceInUse = "";
        var formUi = freemedforms.forms.item("Prescription::Fr::Bizone");
        freemedforms.forms.namespaceInUse = "Prescription::Fr::Bizone";
        var ui = formUi.ui();
        bz_bizoneCheckUi = ui.findChild("bizoneCheckBox");
        bz_bizoneCheckItem = freemedforms.forms.item("Bizone::Check");
        bz_affectionExoneranteItem = freemedforms.forms.item("Affection::Exonerante");
        bz_maladiesIntercurrentesItem = freemedforms.forms.item("Maladies::Intercurrentes");
        bz_maladiesIntercurrentesUi = ui.findChild("maladiesIntercurrentesTextEdit");
        bz_headerUi = ui.findChild("headerLineEdit");
        bz_headerItem = freemedforms.forms.item("Header");
        bz_warningUi = ui.findChild("warningLineEdit");
        bz_warningItem = freemedforms.forms.item("Warning");
        bz_affectionExoneranteHeaderTextEdit = ui.findChild("affectionExoneranteHeaderTextEdit");
        bz_maladiesIntercurrentesHeaderTextEdit = ui.findChild("maladiesIntercurrentesHeaderTextEdit");
        //bz_radioItem = freemedforms.forms.item("Radio");
        bz_bizoneCollapsibleGroupItem = freemedforms.forms.item("Bizone::Group");
        bz_bizoneCollapsibleGroupUi = ui.findChild("bizoneGroupBox");
        bz_bizoneWarningLabelUi = ui.findChild("bizoneWarningLabel");
    }

function bz_connectUiElements()
    {
        bz_bizoneCollapsibleGroupUi['toggled(bool)'].connect(this, bz_onCheckChanged);
        //bz_bizoneCheckUi['stateChanged(int)'].connect(this, bz_onCheckChanged);
    }

function bz_onCheckChanged()
{
    print("function bz_onCheckChanged()");

    var checkValue = bz_bizoneCheckItem.checked;

    print ("typeof radioValue", typeof checkValue);

    if (checkValue === false) {
        print("checkValue: ",checkValue);
        bz_headerItem.currentText = "print";
        bz_affectionExoneranteHeaderTextEdit.visible = false;
        bz_maladiesIntercurrentesHeaderTextEdit.visible = false;

        bz_current = bz_maladiesIntercurrentesItem.currentText;
        var warningString = bz_warningItem.currentText;
        // if (ALD order is not empty and warning lineedit is empty),
        // display a warning
        if (Boolean(bz_affectionExoneranteItem.currentText) === true) {
            bz_warningUi.visible = true;
            if (bz_warningItem.currentText === "") {
               bz_warningItem.currentText = warning;
            }
        }
     } else {
        print ("checkValue: ", checkValue);
        // delete warning about the need to delete ALD order
        bz_warningItem.currentText = "";
        bz_warningUi.visible = false;

        // delete the headerItem that triggers printing of non ALD order header
        bz_headerItem.currentText = "";

        // hide ALD headers
        bz_affectionExoneranteHeaderTextEdit.visible = true;
        bz_maladiesIntercurrentesHeaderTextEdit.visible = true;
        }
}


/*!
  This function is called whenever the user changes the text of "intercurrentes maladies"
  If bizone is checked, set header item to empty string to prevent printing of normal header order
  If bizone is not checked, set header item to "print" to trigger printing of normal order header
*/
function bz_header()
{
    var checkValue = bz_bizoneCheckItem.checked;
    if (checkValue === false) {
        bz_headerItem.currentText = "print";
    } else {
        bz_headerItem.currentText = "";
    }
}

});

// Setup Ui
namespace.com.freemedforms.prescriptions.fr.bizone.setupUi();



