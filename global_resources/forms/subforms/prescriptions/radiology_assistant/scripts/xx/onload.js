namespace.module('com.freemedforms.xray.prescription.helper', function (exports, require) {

  // Ui vars (retrieved from the ui)
  var siteCombo, siteSelectorListWidget, leftCheck, rightCheck, addButton;
  var htmlItem;
  var mainSites = new Array();
  var siteSelector = new Array("xx", "fr");

  exports.extend({
    'setupUi': setupUi
  });

                     function setupUi() {
                         getUiElements();
                         createVariableContent();
                         connectUiElements();
                         siteCombo.currentIndex = 0;
                         reTranslateUi();
                     }

                     function createVariableContent() {
                         mainSites["xx"] = ["Upper Extremities",
                                          "Lower Extremities",
                                          "Shoulder",
                                          "Pelvis",
                                          "Thorax",
                                          "Abdomen",
                                          "Vertebral Spine",
                                          "Skull",
                                          "Facial Bones",
                                          "Mandible & TMJ"
                                         ];
                         mainSites["fr"] = ["Membre supérieur",
                                          "Membre inférieur",
                                          "Épaule",
                                          "Pelvis",
                                          "Thorax",
                                          "Abdomen",
                                          "Colonne vertébrale",
                                          "Tête",
                                          "Os de la face",
                                          "Mandibule & ATM"
                                         ];

                         siteSelector["xx"] = [];
                         siteSelector["fr"] = new Array();

                         // 0: Upper Extremities
                         siteSelector["xx"][0] = ["Finger - PA / AP",
                                                  "Finger - Oblique",
                                                  "Finger - Lateral",
                                                  "Thumb - PA/AP",
                                                  "Thumb - Oblique",
                                                  "Thumb - Lateral",
                                                  "Hand - PA",
                                                  "Hand - Oblique",
                                                  "Hand - Lateral",
                                                  "Hand - Lateral (Fan)",
                                                  "Hand - Oblique (Ballcatchers)",
                                                  "Hand - AP Axial (Brewerton's)",
                                                  "Hand - Bone Age",
                                                  "Wrist - PA",
                                                  "Wrist - Oblique (Internal Rotation)",
                                                  "Wrist - Oblique (External Rotation)",
                                                  "Wrist - Lateral",
                                                  "Wrist - Lateral (Flexion)",
                                                  "Wrist - Lateral (Extension)",
                                                  "Wrist - Carpal Tunnel / Canal",
                                                  "Wrist - Carpal Bridge",
                                                  "Wrist - PA (Ulnar Deviation)",
                                                  "Wrist - PA (Radial Deviation)",
                                                  "Wrist - PA (Clenched Fist)",
                                                  "Scaphoid - PA",
                                                  "Scaphoid - PA Axial",
                                                  "Scaphoid - PA (Ulnar deviation)",
                                                  "Scaphoid - PA (Radial deviation)",
                                                  "Scaphoid - Oblique (Ulnar deviation)",
                                                  "Scaphoid - Lateral",
                                                  "Forearm - AP",
                                                  "Forearm - Lateral",
                                                  "Elbow - AP",
                                                  "Elbow - External Oblique (Lateral Rotation)",
                                                  "Elbow - Internal Oblique (Medial Rotation)",
                                                  "Elbow - Lateral",
                                                  "Elbow - Radial Head",
                                                  "Elbow - AP (Partial Flexion)",
                                                  "Elbow - Acute Flexion",
                                                  "Humerus - AP",
                                                  "Humerus - Lateral",
                                                  "Humerus - Transthoracic Lateral",
                                                  "Humerus - Intertubercular Groove"];

                         // 1: Lower Extremities
                         siteSelector["xx"][1] = ["Toes - DP",
                                                  "Toes - Oblique (DPO)",
                                                  "Toes - Lateral",
                                                  "Sesamoids - Axial",
                                                  "Sesamoids - Oblique",
                                                  "Foot - DP",
                                                  "Foot - Medial Oblique",
                                                  "Foot - Lateral Oblique",
                                                  "Foot - Lateral",
                                                  "Foot - DP Weight Bearing",
                                                  "Foot - Lateral Weight Bearing",
                                                  "Foot - DP Composite Weight Bearing",
                                                  "Calcaneus - Axial",
                                                  "Calcaneus - Lateral",
                                                  "Calcaneus - Lateromedial Oblique",
                                                  "Ankle - AP",
                                                  "Ankle - Oblique (Mortise)",
                                                  "Ankle - Oblique (Internal Rotation)",
                                                  "Ankle - Oblique (External Rotation)",
                                                  "Ankle - Lateral",
                                                  "Ankle - Stress Projections",
                                                  "Subtalar Jt - PA Axial Oblique (Anthosens)",
                                                  "Subtalar Jt - AP Axial Oblique - Medial Rotation",
                                                  "Subtalar Jt - AP Axial Oblique - Lateral Rotation",
                                                  "Subtalar Jt - Lateromedial Oblique (Isherwood)",
                                                  "Subtalar Jt - AP Axial Oblique Medial (Isherwood)",
                                                  "Subtalar Jt - AP Axial Oblique Lateral (Isherwood)",
                                                  "Tibia / Fibula - AP",
                                                  "Tibia / Fibula - Lateral",
                                                  "Tibia / Fibula - Oblique (Medial / Lateral Rotation)",
                                                  "Knee - AP",
                                                  "Knee - Rolled Lateral",
                                                  "Knee - Horizontal Ray Lateral",
                                                  "Knee - Skyline (Tangential, Axial, Sunrise)",
                                                  "Knee - Intercondylar (AP Axial)",
                                                  "Knee - Intercondylar (PA Axial) (Camp-Coventry)",
                                                  "Knee - Intercondylar (PA Axial) (Holmblad)",
                                                  "Knee - Oblique (Lateral Rotation)",
                                                  "Knee - Oblique (Medial Rotation)",
                                                  "Knee - AP Bilateral Weightbearing",
                                                  "Knee - PA Bilateral Weightbearing (Rosenbergs)",
                                                  "Patella - PA",
                                                  "Patella - Lateral",
                                                  "Patella - Oblique (Medial Rotation)",
                                                  "Patella - Oblique (Lateral Rotation)",
                                                  "Femur - AP",
                                                  "Femur - Lateral",
                                                  "Long Leg View"];

                         // 2: Shoulder
                         siteSelector["xx"][2] = ["Shoulder - AP",
                                                  "Shoulder - AP Axial Oblique",
                                                  "Shoulder - Glenohumeral (GH)",
                                                  "Shoulder - Neers",
                                                  "Shoulder - Superoinferior Axial (SI)",
                                                  "Shoulder - Inferosuperior (IS) Axial (Lawrence Method)",
                                                  "Shoulder - Inferosuperior (IS) Axial (West Point Method)",
                                                  "Shoulder - Transthoracic Lateral",
                                                  "Shoulder - Lateral (Scapular Y)",
                                                  "Scapula - AP",
                                                  "Scapula - Lateral",
                                                  "Scapula - Oblique",
                                                  "Clavicle - AP shoulder",
                                                  "Clavicle - AP",
                                                  "Clavicle - AP Axial - 30° cephalad",
                                                  "Acromioclavicular - (Single) AP",
                                                  "Acromioclavicular - (Bilateral) AP",
                                                  "Acromioclavicular - AP Axial",
                                                  "Acromioclavicular - PA Axial Oblique",
                                                  "Acromioclavicular - Weightbearing"];

                         // 3: Pelvis
                         siteSelector["xx"][3] = ["Pelvis - AP",
                                                  "Pelvis - Lateral",
                                                  "Anterior Pelvic Bones - AP Axial \"Outlet View\" (Taylor)",
                                                  "Anterior Pelvic Bones - Superioinferior \"Inlet View\" (Lilienfield)",
                                                  "Hip - AP",
                                                  "Hip - Rolled Lateral Hip",
                                                  "Hip - Horizontal Ray Lateral Hip",
                                                  "Hip - Axiolateral (Clements-Nakayama)",
                                                  "Hip - Axiolateral (Freidman)",
                                                  "Hip - Acetabulum - Judet Method"];

                         // 4: Thorax
                         siteSelector["xx"][4] = ["Chest - PA Erect",
                                                  "Chest - Left Lateral (Erect)",
                                                  "Chest - AP Supine",
                                                  "Chest - Apical",
                                                  "Chest - Lordotic",
                                                  "Chest - Lordotic (Right Middle Lobe)",
                                                  "Chest - Lateral Decubitus",
                                                  "Chest - Dorsal Decubitus",
                                                  "Chest - Ventral Decubitus",
                                                  "Chest - Oblique",
                                                  "Ribs - PA (Anterior Ribs)",
                                                  "Ribs - AP (Posterior Upper Ribs 1-8)",
                                                  "Ribs - AP (Posterior Lower Ribs 9-12)",
                                                  "Ribs - Oblique (Axillary Ribs)",
                                                  "Sternum - RAO",
                                                  "Sternum - Lateral (Left or Right)",
                                                  "Sternoclavicular - PA",
                                                  "Sternoclavicular - Oblique",
                                                  "Sternoclavicular - Axiolateral (Kurzbauer)"];

                         // 5 : Abdomen
                         siteSelector["xx"][5] = ["Abdomen - Supine",
                                                  "Abdomen - Prone",
                                                  "Abdomen - Erect",
                                                  "Abdomen - Lateral Decubitus",
                                                  "Abdomen - Dorsal Decubitus",
                                                  "Abdomen - Lateral",
                                                  "Renal - Supine Abdomen (Inspiration)",
                                                  "Renal - AP Renal Area (Expiration)"];

                         // 6 : Vertebral Spine
                         siteSelector["xx"][6] = ["Cervical - Lateral",
                                                  "Cervical - Lateral (Flexion)",
                                                  "Cervical - Lateral (Extension)",
                                                  "Cervical - Oblique",
                                                  "Cervical - Oblique (Lodgemoore)",
                                                  "Cervical - AP",
                                                  "Cervical - AP (\"Wagging Jaw\")",
                                                  "Cervical - AP Vertibral Arch",
                                                  "Cervical - Oblique Vertibral Arch",
                                                  "Cervical - Odontoid Peg",
                                                  "Cervical - Odontoid Peg (Fuch Method)",
                                                  "Cervical - Cervicothoracic (Swimmers) (Twining Method)",
                                                  "Cervical - Cervicothoracic (Swimmers) (Pawlow Method)",
                                                  "Thoracic - AP",
                                                  "Thoracic - Lateral",
                                                  "Thoracic - Oblique",
                                                  "Lumbar - PA or AP",
                                                  "Lumbar - Lateral",
                                                  "Lumbar - Lateral (Flexion and Extension)",
                                                  "Lumbar - Lateral (Supine and Erect)",
                                                  "Lumbar - Oblique",
                                                  "Lumbar - L5/S1 AP or PA",
                                                  "Lumbar - L5/S1 Lateral",
                                                  "Sacrum - AP",
                                                  "Sacrum - Lateral",
                                                  "Sacroiliac Joints - AP",
                                                  "Sacroiliac Joints - Oblique LPO / RPO",
                                                  "Coccyx - AP",
                                                  "Coccyx - Lateral",
                                                  "Chiropractic Spine"];

                         // 7 : Skull
                         siteSelector["xx"][7] = ["Skull - AP",
                                                  "Skull - PA (Caldwell) 0°",
                                                  "Skull - PA (Caldwelll) 15°",
                                                  "Skull - PA (Caldwell ) 25° - 30°",
                                                  "Skull - PA (Haas)",
                                                  "Skull - Townes",
                                                  "Skull - Townes (Trauma)",
                                                  "Skull - Lateral",
                                                  "Skull - Lateral Horizontal Ray (Trauma)",
                                                  "Skull - SMV (Submentovertex) (Basal)",
                                                  "Sella Turcica - AP Axial",
                                                  "SellaTurcica - Lateral",
                                                  "Temporal Bone - Stenvers View",
                                                  "Petromastoid - Axiolateral (Modified Law)",
                                                  "Petromastoid - Axiolateral Oblique (Modified Law)",
                                                  "Petromastoid - Axiolateral (Henschen, Schuller, Lysholm)"];

                         // 8 : Facial Bones
                         siteSelector["xx"][8] = ["Facial - PA (Caldwell) 15°",
                                                  "Facial - PA 30° (Modified Parietocanthial)",
                                                  "Facial - OM",
                                                  "Facial - OM 30°",
                                                  "Facial - Lateral",
                                                  "Facial - Slit Townes",
                                                  "Facial - Slit (SMV) (Basal)",
                                                  "Nasal - OM",
                                                  "Nasal - Lateral",
                                                  "Nasal - Tangential",
                                                  "Orbit - Superior Orbital Fissures - PA 25°",
                                                  "Orbit - Inferior Orbital Fissures",
                                                  "Orbit - OM",
                                                  "Orbit - OM 30°",
                                                  "Orbit - OM Modified",
                                                  "Orbit - Lateral",
                                                  "Orbit - Optic Canal (Foramen) (Rhese Method)",
                                                  "Zygomas - Arches Slit (SMV) (Basal)",
                                                  "Zygomas - Arches (OM)",
                                                  "Zygoma - Arch (Tangential)",
                                                  "Zygomas - Slit Townes",
                                                  "Sinuses - PA 15° (Caldwell)",
                                                  "Sinuses - OM (Waters)",
                                                  "Sinuses - OM (Waters) Open Mouth",
                                                  "Sinuses - Lateral",
                                                  "Sinuses - Submentovertical (SMV)"];

                         // 9 : Mandible
                         siteSelector["xx"][9] = ["Mandible - PA (Rami)",
                                                  "Mandible - PA (Body)",
                                                  "Mandible - Lateral",
                                                  "Mandible - Oblique",
                                                  "Mandible - SMV",
                                                  "Mandible - (Towne method)",
                                                  "Mandible - Symphysis",
                                                  "Mandible - Orthopantomogram (OPG)",
                                                  "TMJ - Slit Townes (Open mouth)",
                                                  "TMJ - Lateral Oblique",
                                                  "TMJ - Orthopantomogram (OPG)"];

                     }

                     function getUiElements() {
                         freemedforms.forms.namespaceInUse = "";
                         var formUi = freemedforms.forms.item("Subs::Radiology::PrescrHelper::XRays");
                         freemedforms.forms.namespaceInUse = "Subs::Radiology::PrescrHelper::XRays";
                         var ui = formUi.ui();
                         siteCombo = ui.findChild("globalSites");
                         siteSelectorListWidget = ui.findChild("siteSelectorListWidget");
                         leftCheck = ui.findChild("left");
                         rightCheck = ui.findChild("right");
                         addButton = ui.findChild("addButton");
                         htmlItem = freemedforms.forms.item("Prescription");
                     }

                     function populateSiteCombo() {
                         siteCombo.clear();
                         freemedforms.uiTools.addItems(siteCombo, mainSites);
                     }

                     function connectUiElements() {
                         freemedforms.forms.languageChanged.connect(this, reTranslateUi);
                         siteCombo['activated(int)'].connect(this, populateSiteSelector);
                         addButton.clicked.connect(this, putSelectionToHtmlEditor);
                     }

                     function reTranslateUi() {
                         if (freemedforms.forms.currentLanguage=="fr") {
                             leftCheck.text = "Gauche";
                             rightCheck.text = "Droite";
                             siteCombo.clear();
                             freemedforms.uiTools.addItems(siteCombo, mainSites["fr"]);
                         } else {
                             leftCheck.text = "Left";
                             rightCheck.text = "Right";
                             siteCombo.clear();
                             freemedforms.uiTools.addItems(siteCombo, mainSites["xx"]);
                         }
                         populateSiteSelector(siteCombo.currentIndex);
                     }

                     function populateSiteSelector(row) {
                         freemedforms.uiTools.clear(siteSelectorListWidget);
                         if (freemedforms.forms.currentLanguage=="fr") {
                             // TODO When the Fr trans will be added, remplace this "xx" with "fr"
                             freemedforms.uiTools.addItems(siteSelectorListWidget, siteSelector["xx"][row]);
                         } else {
                             freemedforms.uiTools.addItems(siteSelectorListWidget, siteSelector["xx"][row]);
                         }
                     }

                     function putSelectionToHtmlEditor() {
                         // return html code
                         var html = "<span style=\"font-weight:bold;font-size:large;font-variant:small-caps;text-decoration:underline\">" + siteCombo.currentText + "</span><ul title=" + siteCombo.currentText + ">";
                         var selected = freemedforms.uiTools.selectedItems(siteSelectorListWidget);
                         for(var i=0; i < selected.length; i++) {
                             var lat;
                             if (leftCheck.checked)
                                 lat = leftCheck.text;
                             else
                                 lat = rightCheck.text;
                             html += "<li><b>" + selected[i] + "</b> : " + lat + "</li>";
                         }
                         html += "</ul>";
                         htmlItem.currentText = htmlItem.currentText + html;
                     }

});

// Setup Ui
namespace.com.freemedforms.xray.prescription.helper.setupUi();
