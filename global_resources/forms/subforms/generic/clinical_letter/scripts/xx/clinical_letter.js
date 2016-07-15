namespace.module('com.freemedforms.generic.cl', function (exports, require) {

  // Ui vars (retrieved from the ui)
  var cl_medicalSpecialtyClearPushButtonUi;
  var cl_surgicalSpecialtyClearPushButtonUi;
  var cl_alliedHealthProfessionsClearPushButtonUi;
  var cl_otherRecipientClearPushButtonUi;
  var cl_medicalSpecialtyComboItem;
  var cl_medicalSpecialtyComboUi;
  var cl_surgicalSpecialtyComboUi;
  var cl_alliedHealthProfessionsComboUi;
  var cl_otherRecipientLineEditUi;
  exports.extend({
    'setupUi': setupUi
  });

                     function setupUi() {
                         cl_getUiElements();
                         cl_connectUiElements();
                     }


function cl_getUiElements() {
        freemedforms.forms.namespaceInUse = "";
        var formUi = freemedforms.forms.item("Subforms::Generic::Clinical::Letter");
        freemedforms.forms.namespaceInUse = "Subforms::Generic::Clinical::Letter";
        var ui = formUi.ui();
        cl_medicalSpecialtyClearPushButtonUi = ui.findChild("medicalSpecialtyClearPushButton");
        cl_surgicalSpecialtyClearPushButtonUi = ui.findChild("surgicalSpecialtyClearPushButton");
        cl_alliedHealthProfessionsClearPushButtonUi = ui.findChild("alliedHealthProfessionsClearPushButton");
        cl_otherRecipientClearPushButtonUi = ui.findChild("otherRecipientClearPushButton");


        cl_medicalSpecialtyComboUi = ui.findChild("medicalSpecialtyComboBox");
        cl_surgicalSpecialtyComboUi = ui.findChild("surgicalSpecialtyComboBox");
        cl_alliedHealthProfessionsComboUi = ui.findChild("alliedHealthProfessionsComboBox");
        cl_otherRecipientLineEditUi = ui.findChild("otherRecipientLineEdit");

        //siteSelectorListWidget = ui.findChild("siteSelectorListWidget");
        //raySelectorListWidget = ui.findChild("RaySelectorListWidget");
        //leftCheck = ui.findChild("left");
        //rightCheck = ui.findChild("right");
        //addButton = ui.findChild("addButton");
        //samc_startDate = freemedforms.forms.item("Start::Date");
        //samc_endDate = freemedforms.forms.item("End::Date");
        cl_medicalSpecialtyComboItem = freemedforms.forms.item("Medical::Specialty::Combo");
    }

function cl_connectUiElements()
    {
        //samc_startDateUi.dateChanged.connect(this, cl_medicalSpecialtyClear);
        //samc_endDateUi.dateChanged.connect(this, samc_createhtml);
        //samc_additionalTextUi.textChanged.connect(this, samc_createhtml);
        cl_medicalSpecialtyClearPushButtonUi.clicked.connect(this, cl_medicalSpecialtyClear);
        cl_surgicalSpecialtyClearPushButtonUi.clicked.connect(this, cl_surgicalSpecialtyClear);
        cl_alliedHealthProfessionsClearPushButtonUi.clicked.connect(this, cl_alliedHealthProfessionsClear);
        cl_otherRecipientClearPushButtonUi.clicked.connect(this, cl_otherRecipientClear);

        //samc_endTodayPushButtonUi.clicked.connect(this, samc_setEndDate);
        //samc_clearStartDatePushButtonUi.clicked.connect(this, samc_clearStartDate);
        //samc_clearEndDatePushButtonUi.clicked.connect(this, samc_clearEndDate);

    }

function cl_medicalSpecialtyClear()
{
    cl_medicalSpecialtyComboUi.currentText="";
}

function cl_surgicalSpecialtyClear()
{
    cl_surgicalSpecialtyComboUi.currentText="";
}

function cl_alliedHealthProfessionsClear()
{
    cl_alliedHealthProfessionsComboUi.currentText="";
}

function cl_otherRecipientClear()
{
    cl_otherRecipientLineEditUi.text="";
}

});

// Setup Ui
namespace.com.freemedforms.generic.cl.setupUi();
