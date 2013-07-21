function onFirstGroupComboValueChanged()
{
    freemedforms.forms.namespaceInUse = "Test::Script::OnValueChanged";
    var me = freemedforms.forms.item("FirstGroup::TestingCombo");
    var other = freemedforms.forms.item("FirstGroup::Result");
    other.currentText=me.currentText;
}

function onSecondGroupComboValueChanged()
{
    freemedforms.forms.namespaceInUse = "Test::Script::OnValueChanged";
    var me = freemedforms.forms.item("SecondGroup::TestingCombo");
    var other = freemedforms.forms.item("SecondGroup::Result");
    other.currentText += "<b>New value selected:</b> " + me.currentText + "<br />";
}
