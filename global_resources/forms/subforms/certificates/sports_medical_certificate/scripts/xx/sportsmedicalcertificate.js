namespace.module('com.freemedforms.certificates.sportsmedicalcertificate', function (exports, require) {

  // Ui vars (retrieved from the ui)
  var sportsmedicalcertificate_nameOfSportUi;
  var sportsmedicalcertificate_competitionCheckUi;
  var sportsmedicalcertificate_htmlItem;
  var html;
  //var mainSites = new Array();
  var signeeText;
  var signeeArray = ["soussigné", "soussignée"];
  var sportsmedicalcertificate_additionalTextUi;
  //var raySelector = new Array("xx", "fr");
  var daysFr = ["dimanche","lundi","mardi","mercredi","jeudi","vendredi","samedi"];
  var monthsFr = ["janvier","février","mars","avril","mai","juin","juillet","août","septembre","octobre","novembre","décembre"];
    var daysEn = ["Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"];
    var daysEn3 = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
    var monthsEn3 = ["Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"];

  exports.extend({
    'setupUi': setupUi
  });

                     function setupUi() {
                         sportsmedicalcertificate_getUiElements();
                         sportsmedicalcertificate_connectUiElements();
                         sportsmedicalcertificate_reTranslateUi();
                     }


function sportsmedicalcertificate_getUiElements() {
        freemedforms.forms.namespaceInUse = "";
        var formUi = freemedforms.forms.item("Subforms::Certificates::Sports::Medical::Certificate");
        freemedforms.forms.namespaceInUse = "Subforms::Certificates::Sports::Medical::Certificate";
        var ui = formUi.ui();
        sportsmedicalcertificate_nameOfSportUi = ui.findChild("nameOfSportLineEdit");
        sportsmedicalcertificate_additionalTextUi = ui.findChild("additionalTextLineEdit");
        sportsmedicalcertificate_competitionCheckUi = ui.findChild("competitionCheckBox");
        //siteSelectorListWidget = ui.findChild("siteSelectorListWidget");
        //raySelectorListWidget = ui.findChild("RaySelectorListWidget");
        //leftCheck = ui.findChild("left");
        //rightCheck = ui.findChild("right");
        //addButton = ui.findChild("addButton");
         sportsmedicalcertificate_htmlItem = freemedforms.forms.item("Certificate::Texteditor");
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

function sportsmedicalcertificate_connectUiElements()
{
    sportsmedicalcertificate_nameOfSportUi.textChanged.connect(this, sportsmedicalcertificate_createhtml);
    sportsmedicalcertificate_competitionCheckUi.stateChanged.connect(this, sportsmedicalcertificate_createhtml);
    sportsmedicalcertificate_additionalTextUi.textChanged.connect(this, sportsmedicalcertificate_createhtml);
}

function sportsmedicalcertificate_reTranslateUi()
{

}

    // take a date object and return a string
    function sportsmedicalcertificate_printDateFr(date)
    {
        if (date === undefined) {
              date = new Date("1900-01-01");
        }
        var output = daysFr[date.getDay()];
        output += " ";
        output += date.getDate();
        output += " ";
        var monthFr = monthsFr[date.getMonth()];
        output += monthFr
        output += " ";
        var year = date.getFullYear();
        output += year;
        return output;
    }


// take a date formatted as "Sun Apr 01 1928 01:00:00 GMT+0300 (EEST)"
// (like DoB returned by FreeHealth token)
// return a JavaScript date object
function sportsmedicalcertificate_dobToDateObj(date)
{
    var day = date.slice(0,3);
    var month = date.slice(4,7);
    var number = date.slice(8,10);
    var year = date.slice(11,15);
    var dayInt = daysEn3.indexOf(day);
    var monthInt = monthsEn3.indexOf(month);
    var numberInt = parseInt(number,10);
    var yearInt = parseInt(year,10);
    var d = new Date();
    d.setFullYear(yearInt, monthInt, numberInt);
    return d;
}

function sportsmedicalcertificate_createhtml()
{
/*
    html = "<div style=\"font-weight:bold;"
            + "font-size:large;"
            + "font-variant:small-caps;"
            + "text-decoration:underline;"
            + "margin-left:auto;"
            + "margin-right:auto;\">";
    html += "Certificat médical de non contre-indication à la pratique sportive";
    html += "</div>";
*/
    html = "<p>";
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
    html += ", certifie avoir examiné ";
    var fullPatientName = freemedforms.patient.fullName;
    html += fullPatientName;
    html += " né";
    if (freemedforms.patient.isFemale) {
        html += "e";
    }
    html += " le ";

    var patientDateTimeOfBirth = freemedforms.patient.dateOfBirth;
    var patientDateOfBirth = patientDateTimeOfBirth.toString();
    var patientDoB = sportsmedicalcertificate_dobToDateObj(patientDateOfBirth);
    patientDateOfBirth = sportsmedicalcertificate_printDateFr(patientDoB);
    html += patientDateOfBirth;

    html += " et n’avoir pas constaté, à la date de ce jour, de signes cliniques apparents contre-indiquant la pratique ";
    var sports = sportsmedicalcertificate_nameOfSportUi.text;
    html += sports;
    if (sportsmedicalcertificate_competitionCheckUi.checked == 1) {
        html += " en compétition";
    }
    html += ".";
    html += "</p>";

    html += "<p>";
    var additionalText = sportsmedicalcertificate_additionalTextUi.text;
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

    sportsmedicalcertificate_htmlItem.currentText = html;
}

});

// Setup Ui
namespace.com.freemedforms.certificates.sportsmedicalcertificate.setupUi();
