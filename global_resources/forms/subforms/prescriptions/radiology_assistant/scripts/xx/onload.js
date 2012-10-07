namespace.module('com.freemedforms.xray.prescription.helper', function (exports, require) {

  // Ui vars (retrieved from the ui)
  var siteCombo, siteSelectorListWidget, leftCheck, rightCheck, addButton, raySelectorListWidget;
  var htmlItem;
  var mainSites = new Array();
  var siteSelector = new Array("xx", "fr");
  var raySelector = new Array("xx", "fr");

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
                         mainSites["xx"] = ["Upper Extremities", "Fingers and Toes",
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
                         mainSites["fr"] = ["Membre supérieur", "Doigts et Orteils",
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

                         siteSelector["xx"] = new Array();
                         siteSelector["fr"] = new Array();
						 
                         //raySelector
                         raySelector["xx"] = new Array();
                         raySelector["fr"] = new Array();
                         raySelector["xx"]= ["First", "Second", "Third", "Fourth", "Fifth"];
                         raySelector["fr"]= ["Premier", "Deuxième", "Troisième", "Quatrième", "Cinquième"];

                         // 0: Upper Extremities
                         siteSelector["xx"][0] = ["Hand - PA",
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
                         siteSelector["fr"][0] = [
                                                  "Main - de face",
                                                  "Main - de 3/4",
                                                  "Main - de profil",
                                                  "Main - de profil (Fan)",
                                                  "Main - de 3/4 (Ballcatchers)",
                                                  "Main - de face axial(Brewerton's)",
                                                  "Poignet - Age osseux",
                                                  "Poignet - de face",
                                                  "Poignet - de 3/4 (Rotation Interne )",
                                                  "Poignet - de 3/4 (Rotation Externe)",
                                                  "Poignet - de profil",
                                                  "Poignet - de profil (Flexion)",
                                                  "Poignet - de profil (Extension)",
                                                  "Poignet - Canal carpien",
                                                  "Poignet - \"Carpal Bridge\"",
                                                  "Poignet - de face (Déviation Ulnaire)",
                                                  "Poignet - de face (Déviation Radiale)",
                                                  "Poignet - de face (Point fermé)",
                                                  "Scaphoïde - de face",
                                                  "Scaphoïde - de face Axial",
                                                  "Scaphoïde - de face (Déviation Ulnaire)",
                                                  "Scaphoïde - de face (Déviation Radiale)",
                                                  "Scaphoïde - de 3/4 (Déviation Ulnaire)",
                                                  "Scaphoïde - de profil",
                                                  "Avant-Bras - de face",
                                                  "Avant-Bras - de profil",
                                                  "Coude - de face",
                                                  "Coude - oblique externe",
                                                  "Coude - oblique interne",
                                                  "Coude - de profil",
                                                  "Coude - Tête radiale",
                                                  "Coude - de face (Flexion partielle)",
                                                  "Coude - en Flexion",
												  "Coude - incidence rétro-olécrânienne",
                                                  "Humérus - de face",
                                                  "Humérus - de profil",
                                                  "Humérus - Transthoracique de profil",
                                                  "Humérus - sillon intertuberculaire"];
						
						// 1: Fingers and Toes
                         siteSelector["xx"][1] = [ "Finger - PA / AP",
                                                  "Finger - Oblique",
                                                  "Finger - Lateral",
												  "Toe - DP",
                                                  "Toe - Oblique (DPO)",
                                                  "Toe - Lateral"];
                         siteSelector["fr"][1] = [ "Doigt - de face",
                                                  "Doigt - de 3/4",
                                                  "Doigt - de profil",
												  "Orteil - de face",
                                                  "Orteil - de 3/4",
                                                  "Orteil - de profil"];
                        
						// 2: Lower Extremities
                         siteSelector["xx"][2] = [
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
                                                  "Hip - AP",
                                                  "Hip - Rolled Lateral Hip",
                                                  "Hip - Horizontal Ray Lateral Hip",
                                                  "Hip - Axiolateral (Clements-Nakayama)",
                                                  "Hip - Axiolateral (Freidman)",
                                                  "Hip - Acetabulum - Judet Method",
                                                  "Long Leg View"];
												  
                         siteSelector["fr"][2] = [
                                                  "Sésamoïdes - Axial",
                                                  "Sésamoïdes - de 3/4",
                                                  "Pied - de face",
                                                  "Pied - de 3/4 interne",
                                                  "Pied - de 3/4 externe",
                                                  "Pied - de profil",
                                                  "Pied - de face en charge",
                                                  "Pied - de profil en charge",
                                                  "Pied - de profil en charge \"composite\"",
                                                  "Calcanéum - Axial",
                                                  "Calcanéum - de profil",
                                                  "Calcanéum - Lateromedial de 3/4",
                                                  "Cheville - de face",
                                                  "Cheville - de 3/4 (Mortise)",
                                                  "Cheville - de 3/4 (Rotation Interne)",
                                                  "Cheville - de 3/4 (Rotation Externe)",
                                                  "Cheville - de profil",
                                                  "Cheville - clichés dynamiques en varus forcé",
                                                  "Articulation sous-astragalienne - de face Axial de 3/4 (Anthosens)",
                                                  "Articulation sous-astragalienne - de face Axial de 3/4 - Rotation interne",
                                                  "Articulation sous-astragalienne - de face Axial de 3/4 - Rotation externe",
                                                  "Articulation sous-astragalienne - Latéromédial de 3/4 (Isherwood)",
                                                  "Articulation sous-astragalienne - de face Axial de 3/4 interne (Isherwood)",
                                                  "Articulation sous-astragalienne - de face Axial de 3/4 externe (Isherwood)",
                                                  "Tibia / Fibula - de face",
                                                  "Tibia / Fibula - de profil",
                                                  "Tibia / Fibula - de 3/4 (Rotation interne / externe )",
                                                  "Genou - de face",
                                                  "Genou - genou fléchi, de profil",
                                                  "Genou - de profil",
                                                  "Genou - Défilé fémoro-patellaire",
                                                  "Genou - incidence du \"Schuss\"",
                                                  "Genou - Intercondylaire (de face Axial) (Camp-Coventry)",
                                                  "Genou - Intercondylaire (de face Axial) (Holmblad)",
                                                  "Genou - de 3/4 (Rotation externe)",
                                                  "Genou - de 3/4 (Rotation interne)",
                                                  "Genou - 2 genoux de face en charge",
                                                  "Genou - 2 genoux de profil en charge (Rosenbergs)",
                                                  "Patella - de face",
                                                  "Patella - de profil",
                                                  "Patella - de 3/4 (Rotation interne)",
                                                  "Patella - de 3/4 (Rotation externe)",
                                                  "Fémur - de face",
                                                  "Fémur - de profil",
                                                  
                                                  "Hanche - de face",
                                                  "Hanche - de profil Hanche fléchie",
                                                  "Hanche - de profil",
                                                  "Hanche - Axiolatéral(Clements-Nakayama)",
                                                  "Hanche - Axiolatéral (Freidman)",
                                                  "Hanche - Acétabulum - Judet Méthode",
                                                  "Membres inférieurs en entier"];										

                         // 3: Shoulder
                         siteSelector["xx"][3] = [
						"Shoulder - AP",
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
                                                  "Acromioclavicular - AP",
                                                  "Acromioclavicular - AP Axial",
                                                  "Acromioclavicular - PA Axial Oblique",
                                                  "Acromioclavicular - Weightbearing"];
												  
                         siteSelector["fr"][3] = ["Épaule - de face",
                                                  "Épaule - de face Axial de 3/4",
                                                  "Épaule - articulation glénohumérale",
                                                  "Épaule - Neers",
                                                  "Épaule - Supéroinférieur Axial (SI)",
                                                  "Épaule - Inférosupérior (IS) Axial (Lawrence Méthode)",
                                                  "Épaule - Inférosuperior (IS) Axial (West Point Méthode)",
                                                  "Épaule - Transthoracique de profil",
                                                  "Épaule - de profil (Scapular Y)",
                                                  "Scapula - de face",
                                                  "Scapula - de profil",
                                                  "Scapula - de 3/4",
                                                  "Clavicule - de face Epaule",
                                                  "Clavicule - de face",
                                                  "Clavicule - de face Axial - 30° cephalad",
                                                  "Articulation acromio-claviculaire - de face",
                                                  "Articulation acromio-claviculaire - de face axial",
                                                  "Articulation acromio-claviculaire - de face Axial de 3/4",
                                                  "Articulation acromio-claviculaire - en charge"];

                         // 4: Pelvis
                         siteSelector["xx"][4] = ["Pelvis - AP",
                                                  "Pelvis - Lateral",
                                                  "Anterior Pelvic Bones - AP Axial \"Outlet View\" (Taylor)",
                                                  "Anterior Pelvic Bones - Superioinferior \"Inlet View\" (Lilienfield)",
                                                     ];
												  
                         siteSelector["fr"][4] = ["Bassin - de face",
                                                  "Bassin - de profil",
                                                  "Bassin - Oblique ascendant (\"Outlet View\") à 45 degré (Taylor)",
                                                  "Bassin - Oblique descendant (\"Inlet View\") à 45 degré (Lilienfield)",
                                                  ];

                         // 5: Thorax
                         siteSelector["xx"][5] = ["Chest - PA Erect",
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
                         siteSelector["fr"][5] = ["Thorax - de face Debout",
                                                  "Thorax - de profil Gauche (Debout)",
                                                  "Thorax - de face, Décubitus Dorsal",
                                                  "Thorax - Apical",
                                                  "Thorax - incidence en lordose",
                                                  "Thorax - incidence en lordose (Lobe moyen droit)",
                                                  "Thorax - de profil Décubitus",
                                                  "Thorax - Décubitus Dorsal ",
                                                  "Thorax - Décubitus Ventral",
                                                  "Thorax - de 3/4",
                                                  "Côtes - de face (Côtes antérieures)",
                                                  "Côtes - de face (Côtes postérieures supérieures de 1 à 8)",
                                                  "Côtes - de face (Côtes postérieures inférieures de 9 à 12)",
                                                  "Côtes - de 3/4 (Côtes axillaires)",
                                                  "Sternum - oblique antérieur droit",
                                                  "Sternum - de profil",
                                                  "Articulation sterno-claviculaire - de face",
                                                  "Articulation sterno-claviculaire - de 3/4",
                                                  "Articulation sterno-claviculaire - Axiolatéral (Kurzbauer)"];

                         // 6 : Abdomen
                         siteSelector["xx"][6] = ["Abdomen - Supine",
                                                  "Abdomen - Prone",
                                                  "Abdomen - Erect",
                                                  "Abdomen - Lateral Decubitus",
                                                  "Abdomen - Dorsal Decubitus",
                                                  "Abdomen - Lateral",
                                                  "Renal - Supine Abdomen (Inspiration)",
                                                  "Renal - AP Renal Area (Expiration)"];
                         siteSelector["fr"][6] = ["ASP - Décubitus Dorsal",
                                                  "ASP - Décubitus Ventral",
                                                  "ASP - Debout",
                                                  "ASP - de profil Decubitus",
                                                  "ASP - Dorsal Decubitus",
                                                  "ASP - de profil",
                                                  "Rénal - Décubitus Dorsal ASP (Inspiration)",
                                                  "Rénal - de face aire rénale (Expiration)"];

                         // 7 : Vertebral Spine
                         siteSelector["xx"][7] = ["Cervical - Lateral",
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
                         siteSelector["fr"][7] = ["Rachis cervical - de profil",
                                                  "Rachis cervical - de profil (Flexion)",
                                                  "Rachis cervical - de profil (Extension)",
                                                  "Rachis cervical - de 3/4",
                                                  "Rachis cervical - de 3/4 (Lodgemoore)",
                                                  "Rachis cervical - de face",
                                                  "Rachis cervical - de face, machoire mobile (\"Procédé de Pelissier\")",
                                                  "Rachis cervical - de face arc vertébral",
                                                  "Rachis cervical - de 3/4 arc vertébral",
                                                  "Rachis cervical - processus odontoïde",
                                                  "Rachis cervical - processus odontoïde (Fuch Méthode)",
                                                  "Rachis cervical - Cervicothoracique (Swimmers) (Twining Method)",
                                                  "Rachis cervical - Cervicothoracique (Swimmers) (Pawlow Method)",
                                                  "Rachis thoracique - de face",
                                                  "Rachis thoracique - de profil",
                                                  "Rachis thoracique - de 3/4",
                                                  "Rachis lombaire - de face",
                                                  "Rachis lombaire - de profil",
                                                  "Rachis lombaire - de profil (Flexion et Extension)",
                                                  "Rachis lombaire - de profil (Décubitus Dorsal et Debout)",
                                                  "Rachis lombaire - de 3/4",
                                                  "Rachis lombaire - L5/S1 de face",
                                                  "Rachis lombaire - L5/S1 de profil",
                                                  "Sacrum - de face",
                                                  "Sacrum - de profil",
                                                  "Articulations sacro-iliaques - de face",
                                                  "Articulations sacro-iliaques - de 3/4",
                                                  "Coccyx - de face",
                                                  "Coccyx - de profil",
                                                  "\"Chiropractic Spine\""];

                         // 8 : Skull
                         siteSelector["xx"][8] = ["Skull - AP",
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
                         siteSelector["fr"][8] = ["Crâne - de face",
                                                  "Crâne - de face (Caldwell) 0°",
                                                  "Crâne - de face (Caldwelll) 15°",
                                                  "Crâne - de face (Caldwell ) 25° - 30°",
                                                  "Crâne - de face (Haas)",
                                                  "Crâne - Townes",
                                                  "Crâne - Townes (Trauma)",
                                                  "Crâne - de profil",
                                                  "Crâne - de profil rayon horizontal (Trauma)",
                                                  "Crâne - SMV (Submentovertex) (Basal)",
                                                  "Selle Turcique - de face Axial",
                                                  "Selle Turcique - de profil",
                                                  "Os temporal - Stenvers Vue",
                                                  "Mastoïde - Axiolatéral (Modified Law)",
                                                  "Mastoïde - Axiolatéral de 3/4 (Modified Law)",
                                                  "Mastoïde - Axiolatéral (Henschen, Schuller, Lysholm)"];

                         // 9 : Facial Bones
                         siteSelector["xx"][9] = ["Facial - PA (Caldwell) 15°",
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
                         siteSelector["fr"][9] = ["Massif facial - de face (Caldwell) 15°",
                                                  "Massif facial - de face 30° (Pariétocanthial Modifié)",
                                                  "Massif facial - OM",
                                                  "Massif facial - OM 30°",
                                                  "Massif facial - de profil",
                                                  "Massif facial - Slit Townes",
                                                  "Massif facial - Slit (SMV) (Basal)",
                                                  "OPN - OM",
                                                  "OPN - de profil",
                                                  "OPN - Tangentiel",
                                                  "Orbite - Fissures orbitaires superieures - de face 25°",
                                                  "Orbite - Fissures orbitaires inférieures",
                                                  "Orbite - OM",
                                                  "Orbite - OM 30°",
                                                  "Orbite - OM Modifié",
                                                  "Orbite - de profil",
                                                  "Orbite - canal optique (Foramen) (Rhese Méthode)",
                                                  "Zygomas - Arches Slit (SMV) (Basal)",
                                                  "Zygomas - Arches (OM)",
                                                  "Zygoma - Arch (Tangential)",
                                                  "Zygomas - Slit Townes",
                                                  "Radio des sinus - de face 15° (Caldwell)",
                                                  "Radio des sinus - OM (Waters)",
                                                  "Radio des sinus - OM (Waters) Bouche ouverte",
                                                  "Radio des sinus - de profil",
                                                  "Radio des sinus - Submentovertical (SMV)"];

                         // 10 : Mandible
                         siteSelector["xx"][10] = ["Mandible - PA (Rami)",
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
			siteSelector["fr"][10] = ["Mandibule - de face (Rami)",
                                                  "Mandibule - de face (Body)",
                                                  "Mandibule - de profil",
                                                  "Mandibule - de 3/4",
                                                  "Mandibule - SMV",
                                                  "Mandibule - (Towne method)",
                                                  "Mandibule - Symphyse",
                                                  "Mandibule - Orthopanthomogramme",
                                                  "Articulation temporo-mandibulaire - Slit Townes (bouche ouverte)",
                                                  "Articulation temporo-mandibulaire - de 3/4 latéral",
                                                  "Articulation temporo-mandibulaire - Orthopanthomogramme"];

                     }

                     function getUiElements() {
                         freemedforms.forms.namespaceInUse = "";
                         var formUi = freemedforms.forms.item("Subs::Radiology::PrescrHelper::XRays");
                         freemedforms.forms.namespaceInUse = "Subs::Radiology::PrescrHelper::XRays";
                         var ui = formUi.ui();
                         siteCombo = ui.findChild("globalSites");
                         siteSelectorListWidget = ui.findChild("siteSelectorListWidget");
                         raySelectorListWidget = ui.findChild("RaySelectorListWidget");
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
                         populateRaySelector() ;
                         enabledRightLeft();
                     }

                     function populateSiteSelector(row) {
                         freemedforms.uiTools.clear(siteSelectorListWidget);
                         if (freemedforms.forms.currentLanguage=="fr") {
                             freemedforms.uiTools.addItems(siteSelectorListWidget, siteSelector["fr"][row]);
                         } else {
                             freemedforms.uiTools.addItems(siteSelectorListWidget, siteSelector["xx"][row]);
                         }
                         populateRaySelector();
                         enabledRightLeft();
                     }
					
                    function populateRaySelector() {
                        freemedforms.uiTools.clear(raySelectorListWidget);
                        if (siteCombo.currentIndex == 1) {
                            raySelectorListWidget.enabled = true;
                         } else {
                            raySelectorListWidget.enabled = false;
                        }
			if (freemedforms.forms.currentLanguage=="fr") {
                             freemedforms.uiTools.addItems(raySelectorListWidget, raySelector["fr"]);
                         } else {
                             freemedforms.uiTools.addItems(raySelectorListWidget, raySelector["xx"]);
                         }
                     }	
                     
                     function enabledRightLeft() {
                        if (siteCombo.currentIndex > 3) 
                             {leftCheck.enabled = false;
                             rightCheck.enabled = false;}
                         else {leftCheck.enabled = true;
                             rightCheck.enabled = true;} 
                     }
					
                     function putSelectionToHtmlEditor() {
                         // return html code
                         var html = "<span style=\"font-weight:bold;font-size:large;font-variant:small-caps;text-decoration:underline\">" + siteCombo.currentText + "</span><ul title=" + siteCombo.currentText + ">";
                         var selected = freemedforms.uiTools.selectedItems(siteSelectorListWidget);
                         var r_selected = freemedforms.uiTools.selectedItems(raySelectorListWidget);
                         for(var i=0; i < selected.length; i++) {
                             var lat;
                             if  ( (!leftCheck.checked && !rightCheck.checked) || !leftCheck.enabled )
                                 lat = "";
                             else if (rightCheck.checked && !leftCheck.checked)
                                 lat =": "+ rightCheck.text;
                             else if (leftCheck.text && !rightCheck.checked)
                                 lat =": "+ leftCheck.text;
                             else lat=": "+ leftCheck.text+" + "+rightCheck.text;

                             if (siteCombo.currentIndex == 1) {
                                 var quantity = r_selected.join(" + ");
                                 html += "<li><b>"+ quantity + " " + selected[i] + "</b> " + lat + "</li>";
                             } else {
                                 html += "<li><b>" + selected[i] + "</b> " + lat + "</li>";
                             }
                         }
                         html += "</ul>";
                         htmlItem.currentText = htmlItem.currentText + html;
                     }

});

// Setup Ui
namespace.com.freemedforms.xray.prescription.helper.setupUi();
