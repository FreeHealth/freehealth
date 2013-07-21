namespace.module('com.freemedforms.subs.geriatrics.mmse', function (exports, require) {

  // Ui vars (retrieved from the ui)
  var mmsForm;
  var showDrawing;
  var optionsCombo;
  var optionsText = [ "fr" , "de", "xx" ];
  var showDrawingText = [ "fr" , "de", "xx" ];

  exports.extend({
    'setupUi': setupUi
  });

                     function setupUi() {
                         getUiElements();
                         createVariableContent();
                         connectUiElements();
                         retranslateUi();
                     }

                     function getUiElements() {
                         freemedforms.forms.namespaceInUse = "";
                         mmsForm = freemedforms.forms.item("Subs::Geriatrics::MMSE::Checked");
                         freemedforms.forms.namespaceInUse = "Subs::Geriatrics::MMSE::Checked";
                         var ui = mmsForm.ui();
                         optionsCombo = ui.findChild("optionsCombo");
                         showDrawing = ui.findChild("showDrawing");
                     }

                     function createVariableContent() {
                         // Three options :
                         optionsText["fr"] = [ "Tout mettre à correct",
                                              "Tout mettre à incorrect",
                                              "Tout effacer" ];
                         optionsText["de"] = [ "Definieren Sie alles, was als richtig",
                                              "Definieren Sie alles als unkorrekt",
                                              "Alle löschen" ];
                         optionsText["xx"] = [ "Define everything as correct",
                                              "Define everything as uncorrect",
                                              "Clear all" ];
                         showDrawingText["fr"] = "Montrer le dessin";
                         showDrawingText["de"] = "Zeigen sie die zeichnung";
                         showDrawingText["xx"] = "Show drawing";
                     }

                     function connectUiElements() {
                         freemedforms.forms.languageChanged.connect(this, retranslateUi);
                         optionsCombo['activated(int)'].connect(this, optionSelected);
                         showDrawing.clicked.connect(this, onShowDrawing);
                     }

                     function retranslateUi() {
                         // Find the optionsText language
                         var lang = freemedforms.forms.currentLanguage;
                         if (optionsText[lang].length <= 0) {
                             lang = "xx";
                         }
                         for(var i=0; i < optionsText[lang].length; i++) {
                             freemedforms.uiTools.setItemText(optionsCombo, i, optionsText[lang][i]);
                         }
                         showDrawing.text = showDrawingText[lang];
                     }

                     function optionSelected(row) {
                         var checked = false;
                         freemedforms.forms.namespaceInUse = "";
                         var items = mmsForm.childrenUuid();

                         switch(row)
                         {
                         case 0: // 0 : all correct
                             checked = true;
                             break;
                         case 1: // 1 : all uncorrect
                             checked = false;
                             break;
                         case 2: // 2 : clear
                             checked = false;
                             break;
                         }

                         for(var i=0; i < items.length; i++) {
                             freemedforms.forms.item(items[i]).checked = checked;
                         }
                     }

                     function onShowDrawing() {
                         freemedforms.uiTools.showScreenshot("__subForms__/geriatrics/mmse_check", "xx/drawing.png");
                     }
});

namespace.com.freemedforms.subs.geriatrics.mmse.setupUi();
