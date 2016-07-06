namespace.module('com.freemedforms.generic.cl', function (exports, require) {

  // Ui vars (retrieved from the ui)
  var cl_clearMedicalSpecialtyPushButtonUi;
  var cl_medicalSpecialtyCombo;
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
        cl_clearMedicalSpecialtyPushButtonUi = ui.findChild("medicalSpecialtyClearPushButton");

        //siteSelectorListWidget = ui.findChild("siteSelectorListWidget");
        //raySelectorListWidget = ui.findChild("RaySelectorListWidget");
        //leftCheck = ui.findChild("left");
        //rightCheck = ui.findChild("right");
        //addButton = ui.findChild("addButton");
        //samc_startDate = freemedforms.forms.item("Start::Date");
        //samc_endDate = freemedforms.forms.item("End::Date");
        cl_medicalSpecialtyCombo = freemedforms.forms.item("Medical::Specialty::Combo");
    }

function cl_connectUiElements()
    {
        //samc_startDateUi.dateChanged.connect(this, cl_medicalSpecialtyClear);
        //samc_endDateUi.dateChanged.connect(this, samc_createhtml);
        //samc_additionalTextUi.textChanged.connect(this, samc_createhtml);
        cl_clearMedicalSpecialtyPushButtonUi.clicked.connect(this, cl_medicalSpecialtyClear);
        //samc_endTodayPushButtonUi.clicked.connect(this, samc_setEndDate);
        //samc_clearStartDatePushButtonUi.clicked.connect(this, samc_clearStartDate);
        //samc_clearEndDatePushButtonUi.clicked.connect(this, samc_clearEndDate);

    }

function cl_medicalSpecialtyClear()
{
    print("inside cl_medicalSpecialtyClear()");
    print(cl_medicalSpecialtyCombo);
    print(cl_medicalSpecialtyCombo.type);
    //print(cl_medicalSpecialtyCombo.type());
    print(cl_clearMedicalSpecialtyPushButtonUi.currentIndex());
    //cl_clearMedicalSpecialtyPushButtonUi.clearEditText;

}

});

// Setup Ui
namespace.com.freemedforms.generic.cl.setupUi();
