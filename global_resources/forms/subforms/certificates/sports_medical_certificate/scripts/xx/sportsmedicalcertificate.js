namespace.module('com.freemedforms.certificates.sportsmedicalcertificate', function (exports, require) {

  // Ui vars (retrieved from the ui)
  var sportsmedicalcertificate_nameOfSportUi;
  var sportsmedicalcertificate_competitionCheckUi;
  var sportsmedicalcertificate_htmlItem;
  var html;
  //var mainSites = new Array();
  var signeeText = new Array("m", "f");
  //var raySelector = new Array("xx", "fr");

  exports.extend({
    'setupUi': setupUi
  });

                     function setupUi() {
                         getUiElements();
                         createVariableContent();
                         connectUiElements();
                         reTranslateUi();
                         test();

                     }


function getUiElements() {
        freemedforms.forms.namespaceInUse = "";
        var formUi = freemedforms.forms.item("Subforms::Certificates::Sports::Medical::Certificate");
        freemedforms.forms.namespaceInUse = "Subforms::Certificates::Sports::Medical::Certificate";
        var ui = formUi.ui();
        sportsmedicalcertificate_nameOfSportUi = ui.findChild("nameOfSportLineEdit");
        sportsmedicalcertificate_competitionCheckUi = ui.findChild("competitionCheckBox");
        //siteSelectorListWidget = ui.findChild("siteSelectorListWidget");
        //raySelectorListWidget = ui.findChild("RaySelectorListWidget");
        //leftCheck = ui.findChild("left");
        //rightCheck = ui.findChild("right");
        //addButton = ui.findChild("addButton");
         sportsmedicalcertificate_htmlItem = freemedforms.forms.item("Certificate::Texteditor");
    }

function createVariableContent()
{
    signeeText["m"]= "soussigné";
    signeeText["f"]= "soussignée";

}

function today()
{
    var today = new Date();
    var dd = today.getDate();
    var mm = today.getMonth()+1; //January is 0!
    var yyyy = today.getFullYear();

    if(dd<10) {
        dd='0'+dd
    }

    if(mm<10) {
        mm='0'+mm
    }

    today = mm+'/'+dd+'/'+yyyy;

    return today;
}

function connectUiElements()
{
    sportsmedicalcertificate_nameOfSportUi.textChanged.connect(this, test);
}

function reTranslateUi()
{

}

function test()
{
    print("freemedforms.patient.isMale=");
    print(freemedforms.patient.isMale);
    print("\n");
    print("freemedforms.patient.isFemale=");
    print(freemedforms.patient.isFemale);
    print("\n");
    print("freemedforms.user=");
    print("\n");
    print(freemedforms.user);
    print("\n");
    print("freemedforms.user.isFemale=");
    print(freemedforms.user.isFemale);
    print("\n");
    print("freemedforms.user.isMale=");
    print(freemedforms.user.isMale);
    print("\n");

    var html = "<span style=\"font-weight:bold;font-size:large;font-variant:small-caps;text-decoration:underline\">"
            + "Certificat médical de non contre-indication à la pratique sportive"
            + "</span><ul>";

    html += "Je ";
    if (freemedforms.patient.isMale) {
        html += signeeText["m"];
    } else {
        signeeText["m"];
    }

    html += " ";

    sportsmedicalcertificate_htmlItem.currentText = html;
}

});

// Setup Ui
namespace.com.freemedforms.certificates.sportsmedicalcertificate.setupUi();
