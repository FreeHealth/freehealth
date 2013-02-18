namespace.module('com.freemedforms.tools.obstetrics.pregnancycalendar', function (exports, require) {

    exports.extend({
                       'setupUi': setupUi,
                       'retranslateUi': retranslateUi,
                       'lmpDateChanged': lmpDateChanged,
                       'startDateChanged' : startDateChanged,
                       'computeCurrentWA' : computeCurrentWA
                   });

    // Ui vars (retrieved from the form)
    var lmp, startDate, firstEcho, secondEcho, thirdEcho, currentWA, currentDays, workText;

    function setupUi() {
        print("com.freemedforms.tools.obstetrics.pregnancycalendar Setup UI");
        freemedforms.forms.namespaceInUse = "";
        var formItem = freemedforms.forms.item("Subs::Tools::Obstetrics::Calendar");
        print(formItem);
        lmp = freemedforms.forms.item("Subs::Tools::Obstetrics::Calendar::LastMentrualPeriod");
        startDate = freemedforms.forms.item("Subs::Tools::Obstetrics::Calendar::PregnancyStartDate");
        firstEcho = freemedforms.forms.item("Subs::Tools::Obstetrics::Calendar::EchographiesGroup::FirstEcho");
        secondEcho = freemedforms.forms.item("Subs::Tools::Obstetrics::Calendar::EchographiesGroup::SecondEcho");
        thirdEcho = freemedforms.forms.item("Subs::Tools::Obstetrics::Calendar::EchographiesGroup::ThirdEcho");
        currentWA = freemedforms.forms.item("Subs::Tools::Obstetrics::Calendar::CurrentWA");
        currentDays = freemedforms.forms.item("Subs::Tools::Obstetrics::Calendar::CurrentDays");
        workText = freemedforms.forms.item("Subs::Tools::Obstetrics::Calendar::WorkGroup::Text");
    }

    function retranslateUi() {
        var lang = freemedforms.forms.currentLanguage;
    }

    function startDateChanged() {
        if (!startDate.isValid)
            return;
        if (startDate.currentValue.isNull || startDate.currentValue === "")
            return;
        // This member is automatically connected to the button "Compute" next to the startDate dateeditor
        print("StartDate changed: " + startDate.currentValue);
        computeObstetricalCalendar(startDate.currentValue);
    }

    function lmpDateChanged() {
        // unable to access lmp value
        if (!lmp.isValid)
            return;
        if (lmp.currentValue.isNull || lmp.currentValue === "")
            return;
        // This member is automatically connected to the button "Compute" next to the LMP dateeditor
        print("LMP changed: " + lmp.currentValue);
        computeObstetricalCalendar(lmp.currentValue);
    }

    function computeObstetricalCalendar(fromDate) {
        computeCurrentWA(fromDate);

        // Compute echographies
        var firstEchoDate = freemedforms.tools.addWeeks(fromDate, 12);
        var secondEchoDate = freemedforms.tools.addWeeks(fromDate, 22);
        var thirdEchoDate = freemedforms.tools.addWeeks(fromDate, 32);
        firstEcho.currentText = freemedforms.tools.dateToString(firstEchoDate, "ddd dd MMM yyyy");
        secondEcho.currentText = freemedforms.tools.dateToString(secondEchoDate, "ddd dd MMM yyyy");
        thirdEcho.currentText = freemedforms.tools.dateToString(thirdEchoDate, "ddd dd MMM yyyy");

        // French specific for the work
        var table = "<html><body><table width=\"100%\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">";
        table += "<tr><td>Grossesse à déclarer avant la 15éme SA</td><td>";
        var date = freemedforms.tools.addWeeks(fromDate, 15);
        table += freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</td></tr>";

        table += "<tr><td>Trisomie 21 et biomarqueurs entre (14ème-15éme SA)</td><td>";
        date = freemedforms.tools.addWeeks(fromDate, 14);
        table += freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + " -- ";
        date = freemedforms.tools.addWeeks(fromDate, 16);
        table += freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</td></tr>";

        table += "<tr><td>Terme prévu le (41éme SA)</td><td>";
        date = freemedforms.tools.addWeeks(fromDate, 41);
        table += "<span style=\"font-weight: 600\">";
        table += freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</span></td></tr>";
        table += "</table><br />";

        // Congès
        table += "<span style=\"font-weight: 600\">Congés maternités.</span><br />";
        table += "<table width =\"100%\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">";
        table += "<tr><td width =\"50%\">Si moins de trois enfants (6 S / 10 S)</td><td width =\"25%\">";
        date = freemedforms.tools.addWeeks(fromDate, 41-6);
        table += " du " + freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</td><td width =\"25%\">";
        date = freemedforms.tools.addWeeks(fromDate, 51);
        table += " au " + freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</td></tr>";

        table += "<tr><td width =\"50%\">À partir du troisième enfant (8 S / 18 S)</td><td width =\"25%\">";
        date = freemedforms.tools.addWeeks(fromDate, 41-8);
        table += " du " + freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</td><td width =\"25%\">";
        date = freemedforms.tools.addWeeks(fromDate, 41+18);
        table += " au " + freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</td></tr>";

        table += "<tr><td width =\"50%\">Pour des jumeaux (10 S / 22 S)</td><td width =\"25%\">";
        date = freemedforms.tools.addWeeks(fromDate, 41-12);
        table += " du " + freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</td><td width =\"25%\">";
        date = freemedforms.tools.addWeeks(fromDate, 41+22);
        table += " au " + freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</td></tr>";

        table += "<tr><td width =\"50%\">Pour des triplés (24 S - 22 S)</td><td width =\"25%\">";
        date = freemedforms.tools.addWeeks(fromDate, 41-24);
        table += " du " + freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</td><td width =\"25%\">";
        date = freemedforms.tools.addWeeks(fromDate, 41+22);
        table += " au " + freemedforms.tools.dateToString(date, "ddd dd MMM yyyy") + "</td></tr>";
        table += "</table></body></html>";

        workText.currentText = table;
    }

    function computeCurrentWA(fromDate)
    {
        if (!fromDate.isValid || fromDate === "") {
            if (startDate.currentValue !== "")
                fromDate = startDate.currentValue;
            else
                fromDate = lmp.currentValue;
        }
        // unable to access lmp value
        var days = freemedforms.tools.daysTo(fromDate, new Date());
        var weeks = (days - days%7) / 7;
        days = days%7;
        currentDays.currentText = days;
        currentWA.currentText = weeks;
    }
});

namespace.com.freemedforms.tools.obstetrics.pregnancycalendar.setupUi();

