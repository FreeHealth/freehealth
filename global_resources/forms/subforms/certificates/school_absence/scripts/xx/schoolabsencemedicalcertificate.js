namespace.module('com.freemedforms.certificates.schoolabsencemedicalcertificate', function (exports, require) {

  // Ui vars (retrieved from the ui)
  var schoolabsencemedicalcertificate_startDate;
  var schoolabsencemedicalcertificate_endDate;
  var schoolabsencemedicalcertificate_htmlItem;
  var schoolabsencemedicalcertificate_startDateUi;
  var schoolabsencemedicalcertificate_endDateUi;
  var html;
  //var mainSites = new Array();
  var signeeText;
  var signeeArray = ["soussigné", "soussignée"];
  var schoolabsencemedicalcertificate_additionalTextUi;
  //var raySelector = new Array("xx", "fr");

  exports.extend({
    'setupUi': setupUi
  });

                     function setupUi() {
                         schoolabsencemedicalcertificate_getUiElements();
                         schoolabsencemedicalcertificate_connectUiElements();
                         schoolabsencemedicalcertificate_reTranslateUi();
                     }


function schoolabsencemedicalcertificate_getUiElements() {
        freemedforms.forms.namespaceInUse = "";
        var formUi = freemedforms.forms.item("Subforms::Certificates::School::Absence::Medical::Certificate");
        freemedforms.forms.namespaceInUse = "Subforms::Certificates::School::Absence::Medical::Certificate";
        var ui = formUi.ui();
        schoolabsencemedicalcertificate_startDateUi = ui.findChild("startDateEdit");
        schoolabsencemedicalcertificate_endDateUi = ui.findChild("endDateEdit");
        schoolabsencemedicalcertificate_additionalTextUi = ui.findChild("additionalTextLineEdit");
        //siteSelectorListWidget = ui.findChild("siteSelectorListWidget");
        //raySelectorListWidget = ui.findChild("RaySelectorListWidget");
        //leftCheck = ui.findChild("left");
        //rightCheck = ui.findChild("right");
        //addButton = ui.findChild("addButton");
        schoolabsencemedicalcertificate_startDate = freemedforms.forms.item("Start::Date");
        schoolabsencemedicalcertificate_endDate = freemedforms.forms.item("End::Date");


         schoolabsencemedicalcertificate_htmlItem = freemedforms.forms.item("Certificate::Texteditor");
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

    today = dd+'/'+mm+'/'+yyyy;

    return today;
}

function schoolabsencemedicalcertificate_connectUiElements()
{
    schoolabsencemedicalcertificate_startDateUi.dateChanged.connect(this, schoolabsencemedicalcertificate_createhtml);
    schoolabsencemedicalcertificate_endDateUi.dateChanged.connect(this, schoolabsencemedicalcertificate_createhtml);
    schoolabsencemedicalcertificate_additionalTextUi.textChanged.connect(this, schoolabsencemedicalcertificate_createhtml);
}

function schoolabsencemedicalcertificate_reTranslateUi()
{

}

function schoolabsencemedicalcertificate_createhtml()
{
    html = "<div style=\"font-weight:bold;"
            + "font-size:large;"
            + "font-variant:small-caps;"
            + "text-decoration:underline;"
            + "margin-left:auto;"
            + "margin-right:auto;\">";
    html += "Certificat médical pour absence scolaire";
    html += "</div>";

    html += "<p>";
    html += "Je ";
    if (freemedforms.user.isMale) {
        signeeText = signeeArray[0];
        html += signeeText;
    } else {
        signeeText = signeeArray[1];
        html += signeeText;
    }

    html += ", ";
    var fullUserName = freemedforms.user.fullName;
    html += fullUserName;
    html += " atteste que l'état de santé de ";
    var fullPatientName = freemedforms.patient.fullName;
    html += fullPatientName;
    html += " né";
    if (freemedforms.patient.isFemale) {
        html += "e";
    }
    html += " le ";
    var patientDateTimeOfBirth = freemedforms.patient.dateOfBirth;
    print(patientDateTimeOfBirth);
    var indexOfColon = patientDateTimeOfBirth.toString().indexOf(':');
    var indexEndOfDate = indexOfColon - 3;
    var patientDateOfBirth = patientDateTimeOfBirth.toString().slice(0, indexEndOfDate);
    html += patientDateOfBirth;
    html += " justifie d'une absence scolaire du ";
    var startDate = schoolabsencemedicalcertificate_startDate.text;
    html += startDate;
    html += " au ";
    var endDate = schoolabsencemedicalcertificate_endDate.text;
    html += endDate;
    html += ".";
    html += "</p>";

    html += "<p>";
    var additionalText = schoolabsencemedicalcertificate_additionalTextUi.text;
    html += additionalText;
    html += "</p>";
    html += "<div style=\"margin-left: 100%;\">";
    html += "<p>"
    html += "À ";
    var city = freemedforms.user.city;
    html += city;
    html += ", le ";
    var date = today();
    html += date;
    html += "";
    html += "</p>"
    html += "</div>"

    schoolabsencemedicalcertificate_htmlItem.currentText = html;
}

});

// Setup Ui
namespace.com.freemedforms.certificates.schoolabsencemedicalcertificate.setupUi();
