namespace.module('com.freemedforms.subs.french.protocole.ald', function (exports, require) {

    exports.extend({
                       'generatePdf': generatePdf
                   });


    function generatePdf()
    {
        // get the pdftkwrapper
        var pdf = freemedforms.pdf;
        if (!pdf.isAvailable) {
            freemedforms.log.error("ALD form", "Error in french specific ALD form");
            return;
        }

        // process data
        var patient = freemedforms.patient;
        if (!patient.isActive) {
            freemedforms.log.error("ALD form", "patient is not active");
            return;
        }

        var checked = "Oui";
        var unchecked = "non";

        pdf.beginFdfEncoding();

        // Patient data
        pdf.addFdfValue("nom prénom", patient.usualName + " " + patient.otherNames + " " + patient.firstName);
        pdf.addFdfValue("date naiss", freemedforms.tools.dateToString(patient.dateOfBirth, "ddMMyyyy"));
        // Address
        var street = patient.street;
        var adresse = "";
        if (street.search("\n")) {
            var splitter = street.split("\n");
            adresse = splitter.join(" ; ");
        } else {
            adresse = patient.street;
        }
        adresse += "\n" + patient.city + " " + patient.zipcode + " " + patient.state;
        pdf.addFdfValue("adresse", adresse);
        // Social Number
        var nss = patient.socialNumber1;
        if (patient.socialNumberOwnerName.length) {
            pdf.addFdfValue("nom prénom ass", patient.socialNumberOwnerName);
            if (nss.length == 15) {
                pdf.addFdfValue("immat ass", nss.currentText.substring(0, 13));
                pdf.addFdfValue("clé ass", nss.currentText.substring(13,15));
            }
        } else {
            if (nss.length == 15) {
                pdf.addFdfValue("immat", nss.substring(0, 13));
                pdf.addFdfValue("clé", nss.substring(13,15));
            }
        }

        freemedforms.forms.namespaceInUse = "Subs::Tools::FrenchSpecific::ALD";

        // diagnostic(s) de l'(des) affection(s) de longue durée motivant la demande et sa (leurs) date(s) présumée(s) de début
        item = freemedforms.forms.item("Diagnostic1::Label");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "Diagnostic1::Label not available");
            return;
        }
        pdf.addFdfValue("diag1", item.currentText);

        item = freemedforms.forms.item("Diagnostic2::Label");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "Diagnostic2::Label not available");
            return;
        }
        pdf.addFdfValue("diag2", item.currentText);

        item = freemedforms.forms.item("Diagnostic3::Label");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "Diagnostic3::Label not available");
            return;
        }
        pdf.addFdfValue("diag3", item.currentText);

        item = freemedforms.forms.item("Diagnostic1::Date");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "Diagnostic1::Date not available");
            return;
        }
        pdf.addFdfValue("date diag1", freemedforms.tools.dateToString(new Date(item.currentValue), "ddMMyyyy"));

        item = freemedforms.forms.item("Diagnostic2::Date");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "Diagnostic2::Date not available");
            return;
        }
        pdf.addFdfValue("date diag2", freemedforms.tools.dateToString(new Date(item.currentValue), "ddMMyyyy"));

        item = freemedforms.forms.item("Diagnostic3::Date");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "Diagnostic3::Date not available");
            return;
        }
        pdf.addFdfValue("date diag3", freemedforms.tools.dateToString(new Date(item.currentValue), "ddMMyyyy"));

        item = freemedforms.forms.item("Arguments");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "Arguments not available");
            return;
        }
        pdf.addFdfValue("arguments", item.currentText.replace(/\n/g, "\\n"), false);  // Max 9 Lines

        item = freemedforms.forms.item("Proposition");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "Proposition not available");
            return;
        }
        pdf.addFdfValue("ALD1", unchecked, false); // "ALD non exonérante"
        pdf.addFdfValue("ALD2", unchecked, false); // "ALD 30"
        pdf.addFdfValue("ALD3", unchecked, false); // "ALD hors-liste"
        pdf.addFdfValue("ALD4", unchecked, false); // "Poly-pathologie"
        pdf.addFdfValue("ALD5", unchecked, false); // "ALD autres"
        if (item.currentUuid == "nonExo") {
            pdf.addFdfValue("ALD1", checked, false); // "ALD non exonérante"
        } else if (item.currentUuid == "ald30") {
            pdf.addFdfValue("ALD2", checked, false); // "ALD 30"
        } else if (item.currentUuid == "horsListe") {
            pdf.addFdfValue("ALD3", checked, false); // "ALD hors-liste"
        } else if (item.currentUuid == "polyPatho") {
            pdf.addFdfValue("ALD4", checked, false); // "Poly-pathologie"
        } else if (item.currentUuid == "autres") {
            pdf.addFdfValue("ALD5", checked, false); // "ALD autres"
        }

        // suivi biologique prévu (type d'actes)
        item = freemedforms.forms.item("SuiviBio");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "SuiviBio not available");
            return;
        }
        var list = item.currentText;
        pdf.addFdfValue("suivi bio1", list[0]);
        pdf.addFdfValue("suivi bio2", list[1]);
        pdf.addFdfValue("suivi bio3", list[2]);

        // recours à des professionnels de santé para-médicaux
        item = freemedforms.forms.item("RecoursSpe");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "RecoursSpe not available");
            return;
        }
        list = item.currentText;
        pdf.addFdfValue("rec pro1", list[0]);
        pdf.addFdfValue("rec pro2", list[1]);
        pdf.addFdfValue("rec pro3", list[2]);

        // recours à des spécialistes (préciser la spécialité et le type d'acte spécialisé prévu)
        item = freemedforms.forms.item("RecoursPro");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "RecoursPro not available");
            return;
        }
        list = item.currentText;
        pdf.addFdfValue("rec spé1", list[0]);
        pdf.addFdfValue("rec spé2", list[1]);
        pdf.addFdfValue("rec spé3", list[2]);

        // spécialités pharmaceutiques ou classes thérapeutiques ou dispositifs médicaux
        item = freemedforms.forms.item("Pharma");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "Pharma not available");
            return;
        }
        list = item.currentText;
        pdf.addFdfValue("spé phar1", list[0]);
        pdf.addFdfValue("spé phar2", list[1]);
        pdf.addFdfValue("spé phar3", list[2]);
        pdf.addFdfValue("spé phar4", list[3]);
        pdf.addFdfValue("spé phar5", list[4]);
        pdf.addFdfValue("spé phar6", list[5]);
        pdf.addFdfValue("spé phar7", list[6]);
        pdf.addFdfValue("spé phar8", list[7]);
        pdf.addFdfValue("spé phar9", list[8]);
        pdf.addFdfValue("spé phar10", list[9]);
        pdf.addFdfValue("spé phar11", list[10]);
        pdf.addFdfValue("spé phar12", list[11]);

        // Information(s) concernant la maladie (pour le patient)
        item = freemedforms.forms.item("InfoPatients");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "InfoPatients not available");
            return;
        }
        pdf.addFdfValue("infos mal", item.currentText.replace(/\n/g, "\\n"), false); // Max 8 lines
        pdf.addFdfValue("infos mal2", ""); // Max 3 lines

        // durée prévisible de l'arrêt de travail, s'il y a lieu :
        item = freemedforms.forms.item("DureeAT::Value");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "DureeAT::Value not available");
            return;
        }
        if (item.currentValue > 0) {
            print(item.currentValue);
            var period = freemedforms.forms.item("DureeAT::Period");
            if (!period.isValid) {
                freemedforms.log.error("ALD form", "DureeAT::Period not available");
                return;
            }
            pdf.addFdfValue("durée arrêt", item.currentText + " " + period.currentText);
        }

        // durée prévisible des soins :
        item = freemedforms.forms.item("DureeSoins::Value");
        if (!item.isValid) {
            freemedforms.log.error("ALD form", "DureeSoins::Value not available");
            return;
        }
        if (item.currentValue > 0) {
            print(item.currentValue);
            var period = freemedforms.forms.item("DureeSoins::Period");
            if (!period.isValid) {
                freemedforms.log.error("ALD form", "DureeSoins::Period not available");
                return;
            }
            pdf.addFdfValue("durée soins", item.currentText + " " + period.currentText);
        }


        // "Case à cocher"
        pdf.addFdfValue("Case à cocher1", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher2", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher3", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher4", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher5", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher6", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher7", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher8", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher9", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher10", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher11", unchecked, false); // spé phar 1 check
        pdf.addFdfValue("Case à cocher12", unchecked, false); // spé phar 1 check

        pdf.addFdfValue("Case à cocher13", unchecked, false);   // suivi bio1 check
        pdf.addFdfValue("Case à cocher14", unchecked, false);   // suivi bio2 check
        pdf.addFdfValue("Case à cocher15", unchecked, false);   // suivi bio3 check

        pdf.addFdfValue("Case à cocher16", unchecked, false);   // rec spé1 check
        pdf.addFdfValue("Case à cocher17", unchecked, false);   // rec spé2 check
        pdf.addFdfValue("Case à cocher18", unchecked, false);   // rec spé3 check

        pdf.addFdfValue("Case à cocher19", unchecked, false);   // rec pro1 check
        pdf.addFdfValue("Case à cocher20", unchecked, false);   // rec pro2 check
        pdf.addFdfValue("Case à cocher21", unchecked, false);   // rec pro3 check

        // demande de rémunération
        pdf.addFdfValue("date examen", freemedforms.tools.dateToString(new Date(), "ddMMyyyy")); // date d el'examen

        // Identification du médecin
        var user = freemedforms.user;
        if (user.isValid) {
            freemedforms.log.error("ALD form", "No valid user");
            //return;
        }

        pdf.addFdfValue("IDENTIF MEDECIN", user.fullName.toUpperCase() + "\\n" + user.fullAddress.toUpperCase() + "\\n" + user.identifiants.join("; ").toUpperCase(), false);
        // cachet de l'établissement ou du centre de référence
        // pdf.addFdfValue("IDENTIF ETABLT", "IDENTIF ETABLT");

        // reclassement professionnel envisagé
        pdf.addFdfValue("Recl pro", unchecked, false); // Oui

        // identification du médecin traitant établissant le protocole
        pdf.addFdfValue("ident méd", user.fullName);
        pdf.addFdfValue("ident méd2", user.fullAddress);
        pdf.addFdfValue("num ident méd", user.identifiants.join("; "));

        // identification de l'établissement
        // pdf.addFdfValue("nom étab", "nom étab");
        // pdf.addFdfValue("adress étab", "adress étab");
        // pdf.addFdfValue("nom chef serv", "nom chef serv");
        // pdf.addFdfValue("num finess", "num finess");
        // pdf.addFdfValue("date entrée", "date entrée"); // si le malade est hospitalisé, date d'entrée
        // pdf.addFdfValue("act", checked, false); // activité privée

        ///////////////////////////////////////////////////////
        // Unmatched fields
        // pdf.addFdfValue("att", checked, false);
        // pdf.addFdfValue("exam", checked, false);
        // pdf.addFdfValue("règlem", checked, false);
        // pdf.addFdfValue("REF", checked, false);
        ///////////////////////////////////////////////////////

        // décision du médecin conseil
        // pdf.addFdfValue("acc1", "acc1");
        // pdf.addFdfValue("acc2", "acc2");
        // pdf.addFdfValue("acc3", "acc3");
        // pdf.addFdfValue("mal1", "mal1");
        // pdf.addFdfValue("mal2", "mal2");
        // pdf.addFdfValue("mal3", "mal3");
        // pdf.addFdfValue("date acc1", "date acc1");
        // pdf.addFdfValue("date acc2", "date acc2");
        // pdf.addFdfValue("date acc3", "date acc3");
        // pdf.addFdfValue("date val1", "date val1");
        // pdf.addFdfValue("date val2", "date val2");
        // pdf.addFdfValue("date val3", "date val3");
        // pdf.addFdfValue("motif refus", "motif refus");
        // pdf.addFdfValue("motif refus2", "motif refus2");
        // pdf.addFdfValue("date pro", "date pro"); // décision du médecin conseil: date
        // pdf.addFdfValue("date val pro", "date val pro"); // décision du médecin conseil: protocole valable jusqu'au

        pdf.addFdfValue("date déclaration", freemedforms.tools.dateToString(new Date(), "ddMMyyyy"));

        // Get the PDF file from the database
        var filename = freemedforms.forms.extractFormFile("__subForms__/french/protocole_ald", "./pdf/fr/ald.pdf");
        pdf.endFdfEncoding(filename);

        // Prepare the output file
        var output = freemedforms.tools.userDocumentPath + "/protocolesALD/";
        if (!freemedforms.tools.checkDirCreateIfNotExists(output)) {
            freemedforms.log.error("Unable to create dir: "+output);
            return;
        }

        // Compute output filename
        output += patient.fullName + "_" + freemedforms.tools.dateToString(new Date(), "yyyyMMdd_HHmmsszzz");
        output += ".pdf";
        output = output.replace("  ", " ");
        output = output.replace(" ", "_");

        // Generate PDF
        if (!pdf.fillPdfWithFdf(filename, pdf.getFdfContent(), output, "ISO-8859-1")) {
            freemedforms.log.error("ALD Form", "Unable to fill PDF file");
        }
    }

});
