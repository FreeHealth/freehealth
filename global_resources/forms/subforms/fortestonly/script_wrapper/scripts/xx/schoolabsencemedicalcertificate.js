namespace.module('com.freemedforms.certificates.samc', function (exports, require) {

  // Ui vars (retrieved from the ui)
  var userDate; // date time of the episode
  var samc_startDate;
  var samc_endDate;
  var samc_htmlItem;
  var samc_startDateUi;
  var samc_endDateUi;
  var samc_defaultDate = new Date(1900, 0, 1);
  var samc_startTodayPushButtonUi;
  var samc_endTodayPushButtonUi;
  var samc_clearStartDatePushButtonUi;
  var samc_clearEndDatePushButtonUi;

  var html;
  //var mainSites = new Array();
  var signeeText;
  var signeeArray = ["soussigné", "soussignée"];
  var samc_additionalTextUi;
  //var raySelector = new Array("xx", "fr");
  var daysEn = ["Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"];
  var daysFr = ["dimanche","lundi","mardi","mercredi","jeudi","vendredi","samedi"];
  var monthsFr = ["janvier","février","mars","avril","mai","juin","juillet","août","septembre","octobre","novembre","décembre"];
  var daysEn3 = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
  var monthsEn3 = ["Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"];
  exports.extend({
    'setupUi': setupUi
  });

                     function setupUi() {
                         samc_getUiElements();
                         samc_connectUiElements();
                         samc_reTranslateUi();
                     }


function samc_getUiElements() {
        freemedforms.forms.namespaceInUse = "";
        var formUi = freemedforms.forms.item("Subforms::Certificates::School::Absence::Medical::Certificate");
        freemedforms.forms.namespaceInUse = "Subforms::Certificates::School::Absence::Medical::Certificate";
        var ui = formUi.ui();
        samc_startDateUi = ui.findChild("startDateEdit");
        samc_endDateUi = ui.findChild("endDateEdit");
        samc_additionalTextUi = ui.findChild("additionalTextLineEdit");
        samc_startTodayPushButtonUi = ui.findChild("startTodayPushButton");
        samc_endTodayPushButtonUi = ui.findChild("endTodayPushButton");
        samc_clearStartDatePushButtonUi = ui.findChild("clearStartDatePushButton");
        samc_clearEndDatePushButtonUi = ui.findChild("clearEndDatePushButton");




        //siteSelectorListWidget = ui.findChild("siteSelectorListWidget");
        //raySelectorListWidget = ui.findChild("RaySelectorListWidget");
        //leftCheck = ui.findChild("left");
        //rightCheck = ui.findChild("right");
        //addButton = ui.findChild("addButton");
        samc_startDate = freemedforms.forms.item("Start::Date");
        samc_endDate = freemedforms.forms.item("End::Date");


        samc_htmlItem = freemedforms.forms.item("Certificate::Texteditor");
        userDate = formUi.userDate;
        print("here i am in the getUiElements() func", userDate);

    }

function samc_connectUiElements()
    {
        samc_startDateUi.dateChanged.connect(this, samc_createhtml);
        samc_endDateUi.dateChanged.connect(this, samc_createhtml);
        samc_additionalTextUi.textChanged.connect(this, samc_createhtml);
        samc_startTodayPushButtonUi.clicked.connect(this, samc_setStartDate);
        samc_endTodayPushButtonUi.clicked.connect(this, samc_setEndDate);
        samc_clearStartDatePushButtonUi.clicked.connect(this, samc_clearStartDate);
        samc_clearEndDatePushButtonUi.clicked.connect(this, samc_clearEndDate);

    }

function samc_clearStartDate()
    {
        samc_startDateUi.setDate(samc_defaultDate);
    }

function samc_clearEndDate()
        {
            samc_endDateUi.setDate(samc_defaultDate);
        }

function samc_setStartDate()
{
    var today = new Date();
    samc_startDateUi.setDate(today);
}

function samc_setEndDate()
{
    var today = new Date();
    samc_endDateUi.setDate(today);
}

// take date object, return date object without time
function onlyDate(date)
{
    var d = date;
    d.setHours(0, 0, 0, 0);
    return d;
}



function printToday()
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



function samc_reTranslateUi()
{

}

// take a "dd MM yyyy" date string and return a date object
function samc_frDateStringToDate(date)
{
    var year = date.slice(-4);
    var year = parseInt(year, 10);
    var month = date.slice(3, 5);
    var month = parseInt(month, 10);
    var dd = date.slice(0, 2);
    var dd = parseInt(dd, 10);
    var returndate = new Date();
    returndate.setDate(dd);
    returndate.setMonth(month-1); // January is month 0 for JavaScript...
    returndate.setFullYear(year);
    returndate.setHours(0, 0, 0, 0);
    return returndate;
}

// take a date object and return a string
function samc_printDateFr(date)
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
function samc_dobToDateObj(date)
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



function samc_createhtml()
{
    var startDateString = samc_startDateUi.text;
    var endDateString = samc_endDateUi.text;
    var startDate = samc_frDateStringToDate(startDateString);
    var endDate = samc_frDateStringToDate(endDateString);
    var outputStartDate = samc_printDateFr(startDate);
    var outputEndDate = samc_printDateFr(endDate);
    /*html = "<div style=\"font-weight:bold;"
            + "font-size:large;"
            + "font-variant:small-caps;"
            + "text-decoration:underline;"
            + "margin-left:auto;"
            + "margin-right:auto;\">";
    html += "Certificat médical pour absence scolaire";
    html += "</div>";*/

    var html = "";

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
    html += " atteste que ";
    var fullPatientName = freemedforms.patient.fullName;
    html += fullPatientName;
    html += ", né";
    if (freemedforms.patient.isFemale) {
        html += "e";
    }
    html += " le ";
    var patientDateTimeOfBirth = freemedforms.patient.dateOfBirth;
    var patientDateOfBirth = patientDateTimeOfBirth.toString();
    //var indexEndOfDate = indexOfColon - 3;
    //var patientDateOfBirth = patientDateTimeOfBirth.toString().slice(0, indexEndOfDate);
    var patientDoB = samc_dobToDateObj(patientDateOfBirth);
    patientDateOfBirth = samc_printDateFr(patientDoB);
    html += patientDateOfBirth;

    var start = onlyDate(startDate);
    var end = onlyDate(endDate);
    var today = new Date();
    today.setHours(0, 0, 0, 0);
    if ((start < today) || (end < today)) {
        html += ", a présenté ";
    } else {
        html += ", présente"
    }

    html += " un état de santé justifiant d'une absence scolaire ";

    if ( endDate.getTime() === samc_defaultDate.getTime() ) {
        html += "le ";
        html += outputStartDate;
    } else {

        html += "du ";


        html += outputStartDate;
        html += " au ";

        html += outputEndDate;
        html += " inclus";
    }
    html += ".";
    html += "</p>";

    html += "<p>";
    var additionalText = samc_additionalTextUi.text;
    html += additionalText;
    html += "</p>";
    html += "<div style=\"margin-left: 100%;\">";
    html += "<p>"
    html += "Fait à ";
    var city = freemedforms.user.city;
    html += city;
    html += ", le ";
    var todayDate = printToday();
    html += todayDate;
    html += "";
    html += "</p>";
    html += "</div>";
    html += "<p>Test userDate(): "
    html += userDate;
    html += "</p>";

    samc_htmlItem.currentText = html;


}

});

// Setup Ui
namespace.com.freemedforms.certificates.samc.setupUi();
